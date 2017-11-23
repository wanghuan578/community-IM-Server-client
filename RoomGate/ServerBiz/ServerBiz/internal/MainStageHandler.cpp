
#include "MainStageHandler.h"
#include "MessageMapping.h"
#include "TcpServerManager.h"
#include "RoomServerManager.h"
#include "SL_Crypto_CRTRand.h"
#include "SL_Crypto_MD5.h"
#include "SL_Crypto_Hex.h"
#include "Serialize.h"


CMainStageHandler::CMainStageHandler()
{
}

CMainStageHandler::~CMainStageHandler()
{
}

int CMainStageHandler::handle_open()
{
	SL_Seda_StageThreadLRUSlotInfo slots[2] = {0};

	slots[TIMER_SLOT_CENTER_SERVER_CONNECT].capacity = 3;
	slots[TIMER_SLOT_CENTER_SERVER_CONNECT].interval_ms = CENTERSERVER_CONNECT_INTERVAL;

	slots[TIMER_SLOT_CENTER_SERVER_KEEPALIVE].capacity = 3;
	slots[TIMER_SLOT_CENTER_SERVER_KEEPALIVE].interval_ms = CENTERSERVER_KEEPALIVE_INTERVAL;

	return stage_thread_->enable_lru_timers(slots, 2);
}

int CMainStageHandler::handle_close()
{
	return 0;
}

int CMainStageHandler::handle_event(const SL_Seda_Event *event, void *db, void *cache)
{
	switch (event->get_type())
	{
	case SL_Seda_EventType::TRANSMIT_SERVER:
		{
			SL_Seda_RpcTransmitEvent *transmit_event = (SL_Seda_RpcTransmitEvent*)event;

			transmit_event->transmit_proc(transmit_event->from_user_id, transmit_event->to_user_id, transmit_event->transmit_message, transmit_event->message_len);
		}
		break;

	case SL_Seda_EventType::TCP_SOCKET_OPEN:
		{
			SL_Seda_TcpSocketOpenEvent *open_event = (SL_Seda_TcpSocketOpenEvent *)event;
			
			open_event->rpc_proc(0, open_event->socket_handler, this, NULL, NULL, db);
		}
		break;

	case SL_Seda_EventType::THREAD_IDLE:
		break;

	case SL_Seda_EventType::TIMER_EXPIRE:
		{
			SL_Seda_TimerExpireEvent *expire_event = (SL_Seda_TimerExpireEvent*)event;

			switch (expire_event->slot)
			{
			case TIMER_SLOT_CENTER_SERVER_KEEPALIVE:
				{
					//CommonMsgKeepAliveReq req;
					//req.head_.source = CTcpServerManager::Instance()->GetCenterServerManager()->GetServerID();
					//req.body_.req_id = 0;
					//req.body_.current_time_req = CTcpServerManager::Instance()->GetCenterServerManager()->GetSendControl()->get_current_time();
					//
					//SL_ByteBuffer buff(1024);
					//req.serialize_ex<SL_ByteBuffer>(&buff);
					//CTcpServerManager::Instance()->GetCenterServerManager()->GetTcpClientHandler()->put_data(buff);

					//printf("Send KeepAlive...\n");

					//stage_thread_->set_lru_timer(TIMER_SLOT_CENTER_SERVER_KEEPALIVE, NULL);
				}
				break;

			case TIMER_SLOT_CENTER_SERVER_CONNECT:
				{
					//if (!CTcpServerManager::Instance()->GetCenterServerManager()->Init())
					//{
					//	CTcpServerManager::Instance()->GetCenterServerManager()->SetReConnectTimer(stage_thread_->set_lru_timer(TIMER_SLOT_CENTER_SERVER_CONNECT, NULL));
					//}
					//else
					//{
					//	stage_thread_->cancel_timer(CTcpServerManager::Instance()->GetCenterServerManager()->GetReConnectTimer());
					//}
				}
				break;

			default:
				break;
			}
		}
		break;

	case SL_Seda_EventType::RPC_MESSAGE:
		{
			SL_Seda_RpcMessageEvent *rpc_event = (SL_Seda_RpcMessageEvent*)event;
			
			rpc_event->rpc_proc(rpc_event->socket_handler_id, rpc_event->socket_handler, this, rpc_event->rpc_message, rpc_event->remote_addr, db);
		}
		break;

	case SL_Seda_EventType::TCP_SOCKET_CLOSE:
		{
			SL_Seda_TcpSocketCloseEvent *expire_event = (SL_Seda_TcpSocketCloseEvent*)event;

			expire_event->rpc_proc(expire_event->socket_handler_id, expire_event->socket_handler, this, (void*)(&expire_event->source), NULL, NULL);
		}
		break;

	default:
		break;
	}

	return 0;
}

int CMainStageHandler::KeepAliveRequest(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db)
{
	CommonMsgKeepAliveReq *req = (CommonMsgKeepAliveReq*)rpc_message;

	delete rpc_message;

	return 0;
}

int CMainStageHandler::ConnectReq(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgRoomGateConnectReq *req = (MsgRoomGateConnectReq*)rpc_message;

	CTcpServerManager::Instance()->GetRoomGateManager()->GetConnectUserHandlerList()->Remove(socket_handler);

	CRoomGateUserInfo room_gate_info;
	room_gate_info.user_id = req->head_.source;
	room_gate_info.server_random = req->body_.server_random;
	room_gate_info.client_random = req->body_.client_random;
	room_gate_info.session_ticket = req->body_.sessoin_ticket;
	room_gate_info.check_sum = req->body_.checksum;
	room_gate_info.socket_handler_id = socket_handler_id;
	room_gate_info.socket_handler = socket_handler;

	CTcpServerManager::Instance()->GetRoomGateManager()->GetRoomGateUserHandlerList()->Append(room_gate_info);

	MsgCenterServerUserQueryReq user_query_req;
	user_query_req.body_.user_id = req->head_.source;
	//user_query_req.head_.source = req->head_.source;

	SL_ByteBuffer buff(1024);
	user_query_req.serialize_ex(&buff);

	CTcpServerManager::Instance()->GetCenterServerManager()->GetTcpClientHandler()->put_data(buff);
	
	delete req;

	return 0;
}

int CMainStageHandler::HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db)
{
	MsgHelloNotify notify;

	SL_Crypto_CRTRand rand;
	rand.seed();
	int64 server_random = rand.random_int();
	int64 server_time = CTcpServerManager::Instance()->GetRoomGateManager()->GetSendControl()->get_current_time();

	CConnectUserInfo connect_user_info;
	connect_user_info.server_random = server_random;
	connect_user_info.log_time = server_time;
	connect_user_info.socket_handler = socket_handler;
	connect_user_info.socket_handler_id = socket_handler_id;
	CTcpServerManager::Instance()->GetRoomGateManager()->GetConnectUserHandlerList()->Append(connect_user_info);

	notify.body_.error_code = 0;
	notify.body_.error_text = "OK";
	notify.body_.server_random = server_random;
	notify.body_.server_time = server_time;

	SL_ByteBuffer buff(1024);
	notify.serialize_ex<SL_ByteBuffer>(&buff);

	((CRoomGateHandler*)socket_handler)->put_data(buff);

	delete rpc_message;

	return 0;
}

int CMainStageHandler::CS_HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgHelloNotify *notify = (MsgHelloNotify*)rpc_message;

	MsgRoomGateHelloReq req;
	req.body_.service_info.service_id = CTcpServerManager::Instance()->GetRoomGateManager()->GetServerID();
	req.body_.service_info.service_name = CTcpServerManager::Instance()->GetRoomGateManager()->GetServerName();
	req.body_.service_info.service_weight = CTcpServerManager::Instance()->GetRoomGateManager()->GetServiceWeight();
	char ip_s[16];
	ushort port = 0;
	CTcpServerManager::Instance()->GetRoomGateManager()->GetTcpServerHandler()->get_local_addr()->get_ip_local_s(socket_handler_id, ip_s, 16, &port);
	req.body_.service_info.service_addr.ip = ip_s;
	req.body_.service_info.service_addr.port = CTcpServerManager::Instance()->GetRoomGateManager()->GetListenPort();
	req.head_.flag = SERVER_REGISTER_TYPE_ROOMGATE;

	delete notify;

	SL_ByteBuffer buff(2048);
	req.serialize_ex<SL_ByteBuffer>(&buff);

	return ((CCenterServerHandler*)socket_handler)->put_data(buff);
}

int CMainStageHandler::CS_UserQueryRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgCenterServerUserQueryRes *res = (MsgCenterServerUserQueryRes*)rpc_message;

	CRoomGateUserInfo *user_info = CTcpServerManager::Instance()->GetRoomGateManager()->GetRoomGateUserHandlerList()->FindByID(res->head_.source);
	
	if (NULL != user_info)
	{
		MsgRoomGateConnectRes roomgate_connect_res;

		if (0 == res->body_.error_code)
		{
			login_server::SessionTicket session_ticket_t;

			if (NULL != user_info)
			{
				CSerialize::DeAES<login_server::SessionTicket>(session_ticket_t, user_info->session_ticket, res->body_.user_info.session_pw);
				//SL_ByteBuffer Decrypt_buff(CSerialize::Decode(user_info->session_ticket, res->body_.user_info.session_pw));

				//SL_Rpc_MessageParser::parse_to_body<login_server::SessionTicket>(&session_ticket_t, (char*)Decrypt_buff.data(), Decrypt_buff.data_size());
			}
			

			room_gate::ConnectChecksum room_gate_check_sum;
			room_gate_check_sum.client_random = user_info->client_random;
			room_gate_check_sum.server_random = user_info->server_random;
			room_gate_check_sum.session_key = session_ticket_t.session_key;
			room_gate_check_sum.user_id = user_info->user_id;

			SL_ByteBuffer buf_room_gate_check_sum(1024);
			SL_Rpc_MessageParser::serialize_to_buffer<community::room_gate::ConnectChecksum, SL_ByteBuffer>(&room_gate_check_sum, &buf_room_gate_check_sum);

			SL_Crypto_MD5 md5;
			char key[16] = {0};
			md5.init();
			md5.update((const unsigned char*)buf_room_gate_check_sum.data(), buf_room_gate_check_sum.data_size());
			md5.final((unsigned char*)key);

			SL_Crypto_Hex hex;
			hex.set_upper_case(false);
			char check_hex_code[33]={0};
			int len = 32;
			hex.encode((const unsigned char*)key, 16, (unsigned char*)check_hex_code, (unsigned int*)&len);

			if (0 == strncmp(check_hex_code, user_info->check_sum.c_str(), 32))
			{
				roomgate_connect_res.body_.error_code = common::ErrorCode::OK;
				roomgate_connect_res.body_.error_text = "OK";
			}
			else
			{
				roomgate_connect_res.body_.error_code = common::ErrorCode::TICKET_INVALID;
				roomgate_connect_res.body_.error_text = "Ticket Invalid!";
			}
		}
		else
		{
			roomgate_connect_res.body_.error_code = res->body_.error_code;
			roomgate_connect_res.body_.error_text = res->body_.error_text;
		}

		SL_ByteBuffer buff(1024);
		roomgate_connect_res.serialize_ex<SL_ByteBuffer>(&buff);

		((CRoomGateHandler*)user_info->socket_handler)->put_data(buff);
	}
	
	delete res;

	return 0;
}

int CMainStageHandler::RoomServerUpdateNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgCenterServerRoomServerUpdateNotify *notify = (MsgCenterServerRoomServerUpdateNotify*)rpc_message;
	
	if(0 == notify->body_.update_type)
	{
		CTcpServerManager::Instance()->GetRoomServerManager()->Open(notify->body_.room_server_info.service_id, notify->body_.room_server_info.service_name, notify->body_.room_server_info.service_addr.ip, notify->body_.room_server_info.service_addr.port);
	}
	else
	{

	}

	delete notify;

	return 0;
}

int CMainStageHandler::CS_HelloRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db)
{
	printf("RoomGateServer CenterServer Connected.\n");

	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgRoomGateHelloRes *res = (MsgRoomGateHelloRes*)rpc_message;

	if (0 == res->body_.error_code)
	{
		vector<common::ServiceInfo>::iterator iter;
		for (iter = res->body_.room_server_list.begin(); iter != res->body_.room_server_list.end(); iter++)
		{
			CTcpServerManager::Instance()->GetRoomServerManager()->Open(iter->service_id, iter->service_name, iter->service_addr.ip, iter->service_addr.port);
		}
	}
	
	delete res;
	
	return 0;
}

int CMainStageHandler::RS_HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgHelloNotify *notify = (MsgHelloNotify*)rpc_message;

	CTcpServerManager::Instance()->GetRoomServerManager()->RoomServerUpdate(notify->body_.service_id, socket_handler_id);

	delete notify;

	return 0;
}

int CMainStageHandler::TransmitToRSRequest(int from_user_id, int to_user_id, void *transmit_message, int len)
{
	SL_ByteBuffer buff((char*)transmit_message, len);

	//CTcpServerManager::Instance()->GetRoomServerManager()->GetTcpClientHandler()->put_data(buff);
	
	delete transmit_message;

	return 0;
}

int CMainStageHandler::TransmitToRGRequest(int from_user_id, int to_user_id, void *transmit_message, int len)
{
	int offset = 0;

	CRoomGateUserInfo *to_user = CTcpServerManager::Instance()->GetRoomGateManager()->GetRoomGateUserHandlerList()->FindByID(from_user_id);

	if (NULL != to_user)
	{
		SL_ByteBuffer buff((char*)transmit_message, len);

		((CRoomGateHandler*)(to_user->socket_handler))->put_data(buff);
	}

	delete transmit_message;//todo

	return 0;
}

int CMainStageHandler::ServiceDisconnect(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db)
{
	int16 src = *((int16*)rpc_message);

	switch (src)
	{
	case SERVER_REGISTER_TYPE_CENTER:
		{
			CTcpServerManager::Instance()->GetCenterServerManager()->GetServerHandler()->Remove(socket_handler);
		}
		break;

	case SERVER_REGISTER_TYPE_ROOMGATE:
		{
			CTcpServerManager::Instance()->GetRoomGateManager()->GetRoomGateUserHandlerList()->Remove(socket_handler);
		}
		break;

	case SERVER_REGISTER_TYPE_ROOM:
		{
			CTcpServerManager::Instance()->GetRoomServerManager()->RoomServerUpdate(socket_handler_id);
		}
		break;

	default:
		break;
	}

	return 0;
}
