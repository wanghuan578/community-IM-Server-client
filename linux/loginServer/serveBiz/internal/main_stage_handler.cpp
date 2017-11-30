
#include "main_stage_handler.h"
#include "message_mapping.h"
#include "tcp_server_manager.h"
#include "commonTypes.h"
#include "SL_Crypto_CRTRand.h"
#include "SL_Crypto_MD5.h"
#include "SL_Crypto_Hex.h"
#include "SL_UUID.h"
#include "serialize.h"
#include "SL_Crypto_AES.h"


CMainStageHandler::CMainStageHandler()
{
}

CMainStageHandler::~CMainStageHandler()
{
}

int CMainStageHandler::handle_open()
{
	SL_Seda_StageThreadLRUSlotInfo slots[2] = {0};

	slots[TIMER_SLOT_USER_CONNECT_TIMEOUT].capacity = 3;
	slots[TIMER_SLOT_USER_CONNECT_TIMEOUT].interval_ms = USER_CONNECT_TIMEOUT_INTERVAL;

	slots[TIMER_SLOT_CENTER_SERVER_CONNECT].capacity = 3;
	slots[TIMER_SLOT_CENTER_SERVER_CONNECT].interval_ms = CENTERSERVER_CONNECT_INTERVAL;

	return stage_thread_->enable_lru_timers(slots, 2);
}

int CMainStageHandler::handle_close()
{
	return 0;
}

int CMainStageHandler::handle_event(const SL_Seda_Event *event)
{
	switch (event->get_type())
	{
	case SL_Seda_EventType::TCP_SOCKET_OPEN:
		{
			SL_Seda_TcpSocketOpenEvent *open_event = (SL_Seda_TcpSocketOpenEvent *)event;
			
			open_event->rpc_proc(open_event->socket_handler_id, open_event->socket_handler, this, NULL, NULL);
		}
		break;

	case SL_Seda_EventType::THREAD_IDLE:
		break;

	case SL_Seda_EventType::TIMER_EXPIRE:
		{
			SL_Seda_TimerExpireEvent *expire_event = (SL_Seda_TimerExpireEvent*)event;

			switch (expire_event->slot)
			{
			case TIMER_SLOT_USER_CONNECT_TIMEOUT:
				{
					for (int i = 0; i < CTcpServerManager::Instance()->GetLoginServerManager()->GetConnectUserHandlerList()->GetList()->size(); i++)
					{
						CConnectUserInfo *connected_user = CTcpServerManager::Instance()->GetLoginServerManager()->GetConnectUserHandlerList()->GetList()->at(i);

						if (NULL != connected_user)
						{
							int64 current_time = CTcpServerManager::Instance()->GetLoginServerManager()->GetSendControl()->get_current_time();
							int64 interval = current_time - connected_user->log_time;

							if (interval > (USER_CONNECT_TIMEOUT_INTERVAL/1000 - 10))
							{
								//connected_user->socket_handler->close();
							}
						}
					}
					
					CTcpServerManager::Instance()->GetCenterServerManager()->UserConnectTimeout_ = stage_thread_->set_lru_timer(TIMER_SLOT_USER_CONNECT_TIMEOUT, NULL);
				}
				break;

			case TIMER_SLOT_CENTER_SERVER_CONNECT:
				{
					if (!CTcpServerManager::Instance()->GetCenterServerManager()->Init())
					{
						printf("CenterServer Connect Failed...\n");

						CTcpServerManager::Instance()->GetCenterServerManager()->ReConnectTimer_ = stage_thread_->set_lru_timer(TIMER_SLOT_CENTER_SERVER_CONNECT, NULL);
					}
					else
					{
						stage_thread_->cancel_timer(CTcpServerManager::Instance()->GetCenterServerManager()->ReConnectTimer_);
					}
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
			
			rpc_event->rpc_proc(rpc_event->socket_handler_id, rpc_event->socket_handler, this, rpc_event->rpc_message, rpc_event->remote_addr);
		}
		break;

	case SL_Seda_EventType::TCP_SOCKET_CLOSE:
		{
			SL_Seda_TcpSocketCloseEvent *expire_event = (SL_Seda_TcpSocketCloseEvent*)event;

			expire_event->rpc_proc(expire_event->socket_handler_id, expire_event->socket_handler, this, (void*)(&expire_event->source), NULL);
		}
		break;

	default:
		break;
	}

	return 0;
}

int CMainStageHandler::LoginResp(SL_Socket_Handler *socket_handler)
{	
	return 0;
}

int CMainStageHandler::KeepAliveRequest(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CommonMsgKeepAliveReq *req = (CommonMsgKeepAliveReq*)rpc_message;
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;

	delete req;

	return 0;
}

int CMainStageHandler::HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	
	SL_Crypto_CRTRand rand;
	rand.seed();
	int64 server_random = rand.random_int();
	int64 server_time = CTcpServerManager::Instance()->GetLoginServerManager()->GetSendControl()->get_current_time();

	CConnectUserInfo connect_user_info;
	connect_user_info.server_random = server_random;
	connect_user_info.log_time = server_time;
	connect_user_info.socket_handler = socket_handler;
	connect_user_info.socket_handler_id = socket_handler_id;
	CTcpServerManager::Instance()->GetLoginServerManager()->GetConnectUserHandlerList()->Append(connect_user_info);

	MsgHelloNotify notify;
	notify.body_.error_code = 0;
	notify.body_.error_text = "OK";
	notify.body_.server_random = server_random;
	notify.body_.server_time = server_time;

	SL_ByteBuffer buff(1024);
	notify.serialize_ex<SL_ByteBuffer>(&buff);

	((CLoginServerHandler*)socket_handler)->put_data(buff);

	return 0;
}

int CMainStageHandler::LoginRequest(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgUserPasswordLoginReq *req = (MsgUserPasswordLoginReq*)rpc_message;

	CConnectUserInfo *connected_user_info = CTcpServerManager::Instance()->GetLoginServerManager()->GetConnectUserHandlerList()->FindByFD(socket_handler_id);

	if (NULL != connected_user_info)
	{
		connected_user_info->user_id = req->body_.user_id;
		connected_user_info->log_time = connected_user_info->log_time;
		connected_user_info->client_random = req->body_.client_random;
		connected_user_info->server_random = connected_user_info->server_random;
		connected_user_info->check_sum = req->body_.check_sum;

		MsgLoginCheckReq login_check_req;
		login_check_req.head_.source = connected_user_info->user_id;
		//login_check_req.head_.attach_id1 = socket_handler_id;

		SL_ByteBuffer buff(512);
		login_check_req.serialize_ex<SL_ByteBuffer>(&buff);

		if (CTcpServerManager::Instance()->GetDBProxyManager()->HasConnected())
		{
			CTcpServerManager::Instance()->GetDBProxyManager()->GetTcpClientHandler()->put_data(buff);
		}
	}
	//else
	//{
	//	MsgUserPasswordLoginRes res;
	//	res.body_.error_code = common::ErrorCode::PASS_EXPIRYDATE;
	//	res.body_.error_text = "Pass Expire Date!";

	//	SL_ByteBuffer buff(128);
	//	res.serialize_ex<SL_ByteBuffer>(&buff);

	//	((CLoginServerHandler*)connected_user_info->socket_handler)->put_data(buff);
	//}
	
	delete req;

	return 0;
}

int CMainStageHandler::CenterUserLoginRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgCenterUserLoginRes *res = (MsgCenterUserLoginRes*)rpc_message;

	CLoginUserInfo *user_info = CTcpServerManager::Instance()->GetLoginServerManager()->GetLoginUserHandlerList()->FindByID(res->head_.source);

	if (NULL != user_info)
	{
		MsgUserPasswordLoginRes user_login_res;

		if (0 == res->body_.error_code)
		{
			user_login_res.body_.error_code = 0;
			user_login_res.body_.error_text = "OK";

			SL_Crypto_CRTRand rand;
			rand.seed();
			char session_key_buff[32] = {0};
			//itoa(rand.random_int(), session_key_buff, 10);
		sprintf(session_key_buff, "%d", rand.random_int());
			login_server::SessionTicket session_ticket;
			session_ticket.user_id = user_info->user_id;
			session_ticket.session_id = user_info->session_id;
			session_ticket.session_key = session_key_buff;

			SL_ByteBuffer session_ticket_buff(1024);
			SL_Rpc_MessageParser::serialize_to_buffer<login_server::SessionTicket, SL_ByteBuffer>(&session_ticket, &session_ticket_buff);

			user_login_res.body_.session_ticket = CSerialize::Encode(session_ticket_buff, user_info->password);

			CServerData *room_gate_data = CTcpServerManager::Instance()->GetCenterServerManager()->GetRoomGateHandlerList()->ServiceRequest();

			if (NULL != room_gate_data)
			{
				common::ServiceInfo info;
				info.service_id = room_gate_data->GetID();
				info.service_name = room_gate_data->GetName();
				info.service_addr.ip = room_gate_data->GetIpAddr();
				info.service_addr.port = room_gate_data->GetPort();

				user_login_res.body_.room_gate_info.push_back(info);
			}
		}
		else
		{
			user_login_res.body_.error_code = res->body_.error_code;
			user_login_res.body_.error_text = res->body_.error_text;
		}

		SL_ByteBuffer buff(1024);
		user_login_res.serialize_ex<SL_ByteBuffer>(&buff);

		((CLoginServerHandler*)user_info->socket_handler)->put_data(buff);
	}

	delete res;

	return 0;
}

int CMainStageHandler::CS_HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgHelloNotify *notify = (MsgHelloNotify*)rpc_message;

	CTcpServerManager::Instance()->GetCenterServerManager()->SetServerID(notify->body_.service_id);
	CTcpServerManager::Instance()->GetCenterServerManager()->SetServerName(notify->body_.service_name);
	CTcpServerManager::Instance()->GetCenterServerManager()->SetFD(socket_handler_id);
	CTcpServerManager::Instance()->GetCenterServerManager()->SetStatus(true);

	MsgLoginServerHelloReq req;
	req.body_.service_info.service_id = CTcpServerManager::Instance()->GetLoginServerManager()->GetServerID();
	req.body_.service_info.service_name = CTcpServerManager::Instance()->GetLoginServerManager()->GetServerName();
	
	char ip_s[16];
	//CTcpServerManager::Instance()->GetLoginServerManager()->GetTcpServerHandler()->get_local_addr()->get_ip_addr_ex(ip_s, sizeof(ip_s));//wanghuan debug

	req.body_.service_info.service_addr.ip = ip_s;
	req.body_.service_info.service_addr.port = CTcpServerManager::Instance()->GetLoginServerManager()->GetListenPort();
	req.head_.flag = REGISTER_TYPE_LOGIN;

	delete notify;

	SL_ByteBuffer buff(2048);
	req.serialize_ex<SL_ByteBuffer>(&buff);

	((CCenterServerHandler*)socket_handler)->put_data(buff);

	return 0;
}

int CMainStageHandler::CS_HelloRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgLoginServerHelloRes *res = (MsgLoginServerHelloRes*)rpc_message;

	printf("CenterServer Connected.\n");

	if (0 == res->body_.error_code)
	{
		vector<common::ServiceInfo>::iterator room_gate_iter;

		for (room_gate_iter = res->body_.room_gate_list.begin(); room_gate_iter != res->body_.room_gate_list.end(); room_gate_iter++)
		{
			CServerData *room_gate_data = new CServerData(room_gate_iter->service_name, room_gate_iter->service_id, 0, room_gate_iter->service_weight, room_gate_iter->service_addr.ip, room_gate_iter->service_addr.port);

			if (NULL != room_gate_data)
			{
				printf("RoomGate ID: %d, Weight: %d, Ip: %s, Port: %d\n", room_gate_iter->service_id, room_gate_iter->service_weight, room_gate_iter->service_addr.ip.c_str(), room_gate_iter->service_addr.port);

				CTcpServerManager::Instance()->GetCenterServerManager()->GetRoomGateHandlerList()->Append(room_gate_data);
			}
		}

		vector<common::ServiceInfo>::iterator dbp_iter;

		for (dbp_iter = res->body_.dbproxy_list.begin(); dbp_iter != res->body_.dbproxy_list.end(); dbp_iter++)
		{
			CTcpServerManager::Instance()->GetDBProxyManager()->Open(dbp_iter->service_addr.ip, dbp_iter->service_addr.port);
		}
	}
	
	delete res;
	
	return 0;
}

int CMainStageHandler::RoomGateUpdateNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgCenterRoomGateUpdateNotify *notify = (MsgCenterRoomGateUpdateNotify*)rpc_message;

	switch (notify->body_.update_type)
	{
	case 0:
		{
			CServerData *roomgate_data = new CServerData(notify->body_.room_gate_info.service_name, notify->body_.room_gate_info.service_id, 0, notify->body_.room_gate_info.service_weight, notify->body_.room_gate_info.service_addr.ip, notify->body_.room_gate_info.service_addr.port);

			if (NULL != roomgate_data)
			{
				printf("RoomServerUpdateNotify Add ServerID: %d, Ip: %s, Port: %d.\n", notify->body_.room_gate_info.service_id, notify->body_.room_gate_info.service_addr.ip.c_str(), notify->body_.room_gate_info.service_addr.port);

				CTcpServerManager::Instance()->GetCenterServerManager()->GetRoomGateHandlerList()->Append(roomgate_data);
			}
		}
		break;

	case 1:
		{
			printf("RoomServerUpdateNotify Delete ServerID: %d, Ip: %s, Port: %d.\n", notify->body_.room_gate_info.service_id, notify->body_.room_gate_info.service_addr.ip.c_str(), notify->body_.room_gate_info.service_addr.port);

			CTcpServerManager::Instance()->GetCenterServerManager()->GetRoomGateHandlerList()->RemoveByID(notify->body_.room_gate_info.service_id);
		}
		break;

	default:
		break;
	}

	delete notify;

	MsgCenterServerRoomGateUpdateRes res;
	res.body_.error_code = 0;
	res.body_.error_text = "OK";

	SL_ByteBuffer buff(1024);
	res.serialize_ex<SL_ByteBuffer>(&buff);

	((CCenterServerHandler*)socket_handler)->put_data(buff);

	return 0;
}

int CMainStageHandler::DBProxyReadyNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgCenterDBProxyReadyNotify *notify = (MsgCenterDBProxyReadyNotify*)rpc_message;

	CTcpServerManager::Instance()->GetDBProxyManager()->Open(notify->body_.dbproxy_info.service_addr.ip, notify->body_.dbproxy_info.service_addr.port);

	delete notify;

	return 0;
}

int CMainStageHandler::DBProxyHelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgHelloNotify *notify = (MsgHelloNotify*)rpc_message;

	printf("DBProxy Connected.\n");

	CTcpServerManager::Instance()->GetDBProxyManager()->SetServerID(notify->body_.service_id);
	CTcpServerManager::Instance()->GetDBProxyManager()->SetServerName(notify->body_.service_name);
	CTcpServerManager::Instance()->GetDBProxyManager()->SetFD(socket_handler_id);
	CTcpServerManager::Instance()->GetDBProxyManager()->SetStatus(true);

	delete notify;

	return 0;
}

int CMainStageHandler::LoginCheckRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgLoginCheckRes *res = (MsgLoginCheckRes*)rpc_message;

	MsgUserPasswordLoginRes password_res;

	CConnectUserInfo *connected_user_info = CTcpServerManager::Instance()->GetLoginServerManager()->GetConnectUserHandlerList()->FindByID(res->head_.source);

	if (NULL != connected_user_info)
	{
		if (common::ErrorCode::OK == res->body_.error_code)
		{
			CLoginUserInfo login_user_info;
			login_user_info.user_id = connected_user_info->user_id;
			login_user_info.socket_handler = connected_user_info->socket_handler;
			login_user_info.socket_handler_id = connected_user_info->socket_handler_id;
			login_user_info.log_time = connected_user_info->log_time;
			login_user_info.client_random = connected_user_info->client_random;
			login_user_info.server_random = connected_user_info->server_random;
			login_user_info.check_sum = connected_user_info->check_sum;

			CTcpServerManager::Instance()->GetLoginServerManager()->GetLoginUserHandlerList()->Append(login_user_info);
			CTcpServerManager::Instance()->GetLoginServerManager()->GetConnectUserHandlerList()->Remove(login_user_info.user_id);

			MsgLoginGetUserInfoReq user_info_req;
			char log_name[16] = {0};
			sprintf(log_name, "%d", connected_user_info->user_id);
			//itoa(connected_user_info->user_id, log_name, 10);
			user_info_req.body_.user_name = log_name;
			user_info_req.head_.source = login_user_info.user_id;

			SL_ByteBuffer buff(1024);
			user_info_req.serialize_ex<SL_ByteBuffer>(&buff);

			if (CTcpServerManager::Instance()->GetDBProxyManager()->HasConnected())
			{
				CTcpServerManager::Instance()->GetDBProxyManager()->GetTcpClientHandler()->put_data(buff);
			}

			return 0;
		}
		else
		{
			password_res.body_.error_code = res->body_.error_code;
			password_res.body_.error_text = res->body_.error_text;
		}

		SL_ByteBuffer buff(1024);
		password_res.serialize_ex(&buff);

		((CLoginServerHandler*)connected_user_info->socket_handler)->put_data(buff);
	}

	delete res;

	return 0;
}

int CMainStageHandler::LoginGetUserInfoRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgLoginGetUserInfoRes *user_info_res = (MsgLoginGetUserInfoRes*)rpc_message;

	MsgUserPasswordLoginRes login_res;

	if (common::ErrorCode::LOGNAME_NOT_EXIST == user_info_res->body_.error_code)
	{
		login_res.body_.error_code = common::ErrorCode::LOGNAME_NOT_EXIST;
		login_res.body_.error_text = "Logname Not Exist!";
	}
	else if (common::ErrorCode::LOGNAME_EXIST == user_info_res->body_.error_code)
	{
		login_res.body_.error_code = common::ErrorCode::LOGNAME_EXIST;
		login_res.body_.error_text = "Logname Exist!";
	}
	else
	{
		char check_hex_code[33]={0};

		if (0 == user_info_res->body_.error_code)
		{
			CLoginUserInfo *login_user_info = CTcpServerManager::Instance()->GetLoginServerManager()->GetLoginUserHandlerList()->FindByID(user_info_res->body_.user_info.user_id);

			if (NULL != login_user_info)
			{
				login_server::ClientPasswordLoginReqChecksum login_check_sum;
				login_check_sum.user_id = user_info_res->body_.user_info.user_id;
				login_check_sum.password = user_info_res->body_.user_info.password;
				login_check_sum.client_random = login_user_info->client_random;
				login_check_sum.server_random = login_user_info->server_random;

				SL_ByteBuffer buf_check_sum(1024);
				SL_Crypto_MD5 md5;
				char key[16] = {0};
				SL_Rpc_MessageParser::serialize_to_buffer<login_server::ClientPasswordLoginReqChecksum, SL_ByteBuffer>(&login_check_sum, &buf_check_sum);
				md5.init();
				md5.update((const unsigned char*)buf_check_sum.data(), buf_check_sum.data_size());
				md5.final((unsigned char*)key);

				SL_Crypto_Hex hex;
				hex.set_upper_case(false);
				int len = 32;
				hex.encode((const unsigned char *)key, 16, (unsigned char *)check_hex_code, (unsigned int *)&len);

				if (0 == strncmp(login_user_info->check_sum.c_str(), check_hex_code, 32))
				{
					login_user_info->password = user_info_res->body_.user_info.password;

					char session_key_buff[32] = {0};
					//itoa(login_user_info->server_random, session_key_buff, 10);
		sprintf(session_key_buff, "%d", login_user_info->server_random);
					login_user_info->session_key = session_key_buff;

					char uuid_buff[50] = {0};
					SL_UUID::generate(uuid_buff);
					login_user_info->session_id = uuid_buff;

					MsgCenterUserLoginReq center_user_login_req;
					center_user_login_req.body_.user_id = login_user_info->user_id;
					center_user_login_req.body_.user_name = login_user_info->user_name;
					center_user_login_req.body_.session_id = login_user_info->session_id;
					center_user_login_req.body_.login_time = login_user_info->log_time;

					SL_ByteBuffer buff(1024);
					center_user_login_req.serialize_ex<SL_ByteBuffer>(&buff);

					if (CTcpServerManager::Instance()->GetCenterServerManager()->HasConnected())
					{
						CTcpServerManager::Instance()->GetCenterServerManager()->GetTcpClientHandler()->put_data(buff);
					}
					
					return 0;
				}
				else
				{
					login_res.body_.error_code = common::ErrorCode::PASSWORD_ERROR;
					login_res.body_.error_text = "Login Password Invalid!";
				}
			}
		}
		else
		{

		}
	}

	SL_ByteBuffer buff(4096);
	login_res.serialize_ex<SL_ByteBuffer>(&buff);

	CLoginUserInfo *user_info = CTcpServerManager::Instance()->GetLoginServerManager()->GetLoginUserHandlerList()->FindByFD(user_info_res->head_.source);

	if (NULL != user_info)
	{
		((CLoginServerHandler*)user_info->socket_handler)->put_data(buff);
	}

	delete user_info_res;

	return 0;
}

int CMainStageHandler::ServiceDisconnect(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	int16 src = *((int16*)rpc_message);

	switch (src)
	{
	case REGISTER_TYPE_USER:
		{
			if (-1 == CTcpServerManager::Instance()->GetLoginServerManager()->GetConnectUserHandlerList()->Remove(socket_handler))
			{
				CTcpServerManager::Instance()->GetLoginServerManager()->GetLoginUserHandlerList()->Remove(socket_handler);
			}
		}
		break;

	case REGISTER_TYPE_CENTER:
		{
			CTcpServerManager::Instance()->GetCenterServerManager()->Destroy();
		}
		break;

	case REGISTER_TYPE_DBPROXY:
		{
			CTcpServerManager::Instance()->GetDBProxyManager()->Destroy();
		}
		break;

	default:
		break;
	}

	return 0;
}
