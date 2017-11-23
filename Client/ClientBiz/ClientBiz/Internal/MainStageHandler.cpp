
#include "MainStageHandler.h"
#include "MessageMapping.h"
#include "LoginServerHandler.h"
#include "SL_Rpc_Message.h"
#include "SL_Crypto_CRTRand.h"
#include "TcpClientManager.h"
#include "SL_Crypto_MD5.h"
#include "SL_Crypto_Hex.h"
#include "ClientBiz.h"
#include "Serialize.h"
#include "SL_Crypto_AES.h"


using namespace std;


//Keepalive的定时器Slot
const ushort MAIN_STAGE_KEEPALIVE_TIMER_SLOT = 0;

//超时检测机制
const ushort EXPIRE_TIMER_SLOT = 1;

CMainStageHandler::CMainStageHandler()
{
	//SL_Sync_Mutex *m_CS = new SL_Sync_Mutex;
}

CMainStageHandler::~CMainStageHandler()
{

}

int CMainStageHandler::handle_open()
{
	//SL_Seda_StageThreadLRUSlotInfo slots[2];
	//slots[MAIN_STAGE_KEEPALIVE_TIMER_SLOT].capacity = 3;
	//slots[MAIN_STAGE_KEEPALIVE_TIMER_SLOT].interval_ms = 30000;
	//slots[EXPIRE_TIMER_SLOT].capacity = 3;
	//slots[EXPIRE_TIMER_SLOT].interval_ms = 5000;
	//stage_thread_->enable_lru_timers(slots, 2);

	return 0;
}

int CMainStageHandler::handle_close()
{
	return 0;
}

int CMainStageHandler::handle_event(const SL_Seda_Event *event)
{
	switch (event->get_type())
	{
	case SL_Seda_EventType::THREAD_IDLE:
		break;

	case SL_Seda_EventType::TIMER_EXPIRE:
		{
			SL_Seda_TimerExpireEvent *expire_event = (SL_Seda_TimerExpireEvent*)event;

			switch (expire_event->slot)
			{
			case MAIN_STAGE_KEEPALIVE_TIMER_SLOT:
				{
				}
				break;

			case EXPIRE_TIMER_SLOT:
				{                    
				}
				break;
			default:
				break;
			}
		}
		break;

	case SL_Seda_EventType::RPC_MESSAGE:
		{
			SL_Seda_RpcMessageEvent *rpc_event = (SL_Seda_RpcMessageEvent *)event;
			rpc_event->rpc_proc(rpc_event->socket_handler_id, rpc_event->socket_handler, this, rpc_event->rpc_message, rpc_event->remote_addr);
		}
		break;

	default:
		break;
	}

	return 0;
}

int CMainStageHandler::LS_HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgHelloNotify *notify = (MsgHelloNotify*)rpc_message;
	
	CClientBiz::Instance()->SetServerRandom(notify->body_.server_random);
	
	CTcpClientManager::Instance()->Transfer_To_UI(common::MessageType::MT_HELLO_NOTIFY, SERVER_REGISTER_TYPE_LOGIN, notify);

	delete notify;

	return 0;
}

int CMainStageHandler::RG_HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgHelloNotify *notify = (MsgHelloNotify*)rpc_message;

	if (0 == notify->body_.error_code)
	{
		room_gate::ConnectChecksum room_gate_check_sum;
		room_gate_check_sum.client_random = CClientBiz::Instance()->GetClientRandom();
		room_gate_check_sum.server_random = notify->body_.server_random;
		room_gate_check_sum.session_key = CClientBiz::Instance()->GetSessionKey();
		room_gate_check_sum.user_id = CClientBiz::Instance()->GetUserID();

		
		//SL_ByteBuffer buf_room_gate_check_sum(1024);
		//SL_Rpc_MessageParser::serialize_to_buffer<community::room_gate::ConnectChecksum, SL_ByteBuffer>(&room_gate_check_sum, &buf_room_gate_check_sum);

		//SL_Crypto_MD5 md5;
		//char key[16] = {0};
		//md5.init();
		//md5.update((const unsigned char*)buf_room_gate_check_sum.data(), buf_room_gate_check_sum.data_size());
		//md5.final((unsigned char*)key);

		//SL_Crypto_Hex hex;
		//hex.set_upper_case(false);
		//char check_hex_code[33]={0};
		//int len = 32;
		//hex.encode((const unsigned char *)key, 16, (unsigned char *)check_hex_code, (unsigned int *)&len);

		MsgRoomGateConnectReq connect_req;
		connect_req.head_.source = CClientBiz::Instance()->GetUserID();
		connect_req.head_.attach_id1 = CClientBiz::Instance()->GetRoomID();
		connect_req.body_.checksum = CSerialize::MD5<room_gate::ConnectChecksum>(room_gate_check_sum);;
		connect_req.body_.client_random = CClientBiz::Instance()->GetClientRandom();
		connect_req.body_.server_random = notify->body_.server_random;
		connect_req.body_.sessoin_ticket = CClientBiz::Instance()->GetSessionTicket();

		SL_ByteBuffer msg_buf(4096);
		connect_req.serialize_ex<SL_ByteBuffer>(&msg_buf);

		CTcpClientManager::Instance()->GetRoomGateManager()->GetTcpClientHandler()->put_data(msg_buf);
	}

	delete notify;

	return 0;
}

 int CMainStageHandler::ConnectRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
 {
	 CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	 MsgRoomGateConnectRes *res = (MsgRoomGateConnectRes*)rpc_message;

	 switch (res->body_.error_code)
	 {
	 case common::ErrorCode::OK:
		 {
			 CClientBiz::Instance()->SetRoomGateID(res->head_.attach_id2);

			 MsgGetRoomListReq room_list_req;

			 SL_ByteBuffer buff(1024);
			 room_list_req.serialize_ex(&buff);

			 ((CRoomGateHandler*)socket_handler)->put_data(buff);
		 }
		 break;

	 case common::ErrorCode::TICKET_INVALID:
	 case common::ErrorCode::DB_OPERATION_EXCEPTION:
		 {

		 }
		 break;

	 default:
		 break;
	 }

	 CTcpClientManager::Instance()->Transfer_To_UI(room_gate::MessageType::MT_CONNECT_RES, res->body_.error_code, res);

	 delete res;

	 return 0;
 }

void CMainStageHandler::ConnectSucceed(int socket_handler_id, SL_Socket_Handler *socket_handler)
{
}

//int CMainStageHandler::DecryptSessionTicket(string &session_ticket)
//{
//	SL_ByteBuffer Decrypt_buff(CSerialize::Decode(session_ticket, CClientBiz::Instance()->GetPassword()));
//
//	login_server::SessionTicket session_ticket_t;
//
//	SL_Rpc_MessageParser::parse_to_body<login_server::SessionTicket>(&session_ticket_t, (char*)Decrypt_buff.data(), Decrypt_buff.data_size());
//
//	CClientBiz::Instance()->SetSessionID(session_ticket_t.session_id);
//	CClientBiz::Instance()->SetSessionKey(session_ticket_t.session_key);
//
//	return 0;
//}

void CMainStageHandler::DecryptUserInfo(void *res, void *user_info)
{
	//MsgUserPasswordLoginRes *pLoginRes = (MsgUserPasswordLoginRes*)res;
	//login_server::ClientLoginResUserInfo *info = (login_server::ClientLoginResUserInfo *)user_info;

	//SL_ByteBuffer Decrypt_buff(CSerialize::Decode(pLoginRes->body_.user_info, CClientBiz::Instance()->GetPassword()));

	//SL_Rpc_MessageParser::parse_to_body<login_server::ClientLoginResUserInfo>(info, (char*)Decrypt_buff.data(), Decrypt_buff.data_size());

	////对用户信息赋值
	//ClientBizApp_i::instance()->local_user_.user_id_ = user_info.user_id;
	//ClientBizApp_i::instance()->local_user_.user_name_ = user_info.user_name;
	//ClientBizApp_i::instance()->local_user_.email_ = user_info.email;
}

int CMainStageHandler::RegisterRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgUserRegisterRes *res = (MsgUserRegisterRes*)rpc_message;

	string uid = res->body_.user_id;

	return 0;

}

int CMainStageHandler::LoginRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgUserPasswordLoginRes *res = (MsgUserPasswordLoginRes*)rpc_message;
	
	CTcpClientManager::Instance()->GetLoginServerManager()->Destroy();
	login_server::ClientLoginResUserInfo user_info;
	if (0 == res->body_.error_code)
	{
		CClientBiz::Instance()->SetSessionTicket(res->body_.session_ticket);

		//seda_handler->DecryptUserInfo(res, &user_info);
		string key = CSerialize::MD5(CClientBiz::Instance()->GetPassword());
		CSerialize::DeAES<login_server::ClientLoginResUserInfo>(user_info, res->body_.user_info, key);

		if (!res->body_.room_gate_info.empty())
		{
			common::ServiceInfo roomgate_info = res->body_.room_gate_info.front();

			CTcpClientManager::Instance()->GetRoomGateManager()->Open(roomgate_info.service_addr.ip, roomgate_info.service_addr.port);
		}
	}

	CTcpClientManager::Instance()->Transfer_To_UI(login_server::MessageType::MT_CLIENT_LOGIN_RES, res->body_.error_code, res);

	delete res;

	return 0;
}

int CMainStageHandler::ServiceDisconnect(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	return 0;
}

int CMainStageHandler::GetRoomGateListRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgGetRoomListRes *res = (MsgGetRoomListRes*)rpc_message;

	if (0 == res->body_.error_code)
	{
		CTcpClientManager::Instance()->Transfer_To_UI(room_gate::MessageType::MT_GET_ROOMLIST_RES, res->body_.error_code, res);

		//vector<room_gate::RoomInfo>::iterator iter;

		//for (iter = res->body_.room_list.begin(); iter != res->body_.room_list.end(); iter++)
		//{
		//	MsgEnterRoomReq enter_room_req;
		//	enter_room_req.body_.room_password = "111111";
		//	enter_room_req.body_.is_robot = false;
		//	enter_room_req.body_.user_status = room_server::UserStatus::OFFLINE;

		//	enter_room_req.head_.source = CClientBiz::Instance()->GetUserID();
		//	enter_room_req.head_.attach_id1 = iter->room_id;
		//	enter_room_req.head_.attach_id2 = CClientBiz::Instance()->GetRoomGateID();

		//	SL_ByteBuffer buff(1024);
		//	enter_room_req.serialize_ex<SL_ByteBuffer>(&buff);

		//	((CRoomGateHandler*)socket_handler)->put_data(buff);

		//	break;
		//}
	}
	
	delete res;

	return 0;
}

int CMainStageHandler::EnterRoomRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgEnterRoomRes *res = (MsgEnterRoomRes*)rpc_message;

	CTcpClientManager::Instance()->Transfer_To_UI(room_server::MessageType::MT_ENTER_ROOM_RES, res->body_.error_code, res);

	delete res;

	return 0;
}

int CMainStageHandler::EnterRoomNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgEnterRoomNotify *notify = (MsgEnterRoomNotify*)rpc_message;

	if (notify->body_.user_id != CClientBiz::Instance()->GetUserID())
	{
		//CClientBiz::Instance()->UserListUpdate((room_server::UserInfo)notify->body_);

		CTcpClientManager::Instance()->Transfer_To_UI(room_server::MessageType::MT_ENTER_ROOM_NOTIFY, 0, notify);
	}
	
	delete notify;

	return 0;
}

int CMainStageHandler::LeaveRoomRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgLeaveRoomRes *res = (MsgLeaveRoomRes*)rpc_message;
	
	CTcpClientManager::Instance()->Transfer_To_UI(room_server::MessageType::MT_LEAVE_ROOM_RES, res->body_.error_code, res);

	delete res;

	return 0;
}

int CMainStageHandler::GetUserListRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgGetUserListRes *res = (MsgGetUserListRes*)rpc_message;
	
	CTcpClientManager::Instance()->Transfer_To_UI(room_server::MessageType::MT_GET_USER_LIST_RES, res->body_.error_code, res);
	
	delete res;

	return 0;
}

int CMainStageHandler::ChatNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgRoomServerChatNotify *notify = (MsgRoomServerChatNotify*)rpc_message;

	CTcpClientManager::Instance()->Transfer_To_UI(room_server::MessageType::MT_CHAT_NOTIFY, 0, notify);

	delete notify;

	return 0;
}