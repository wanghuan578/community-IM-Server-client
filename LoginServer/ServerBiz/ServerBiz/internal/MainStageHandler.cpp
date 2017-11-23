
#include "MainStageHandler.h"
#include "MessageMapping.h"
#include "TcpServerManager.h"
#include "CommonTypes.h"
#include "SL_Crypto_CRTRand.h"
#include "SL_Crypto_MD5.h"
#include "SL_Crypto_Hex.h"
#include "SL_UUID.h"
#include "Serialize.h"
#include "SL_Crypto_AES.h"
#include "SL_OS_API.h"
#include "openssl/evp.h"

CMainStageHandler::CMainStageHandler()
{
}

CMainStageHandler::~CMainStageHandler()
{
}

int CMainStageHandler::handle_open()
{
	//SL_Seda_StageThreadLRUSlotInfo slots[2] = {0};

	//slots[TIMER_SLOT_USER_CONNECT_TIMEOUT].capacity = 1;
	//slots[TIMER_SLOT_USER_CONNECT_TIMEOUT].interval_ms = USER_CONNECT_TIMEOUT_INTERVAL;

	//slots[TIMER_SLOT_CENTER_SERVER_CONNECT].capacity = 1;
	//slots[TIMER_SLOT_CENTER_SERVER_CONNECT].interval_ms = CENTER_SERVER_CONNECT_INTERVAL;

	return 0;//stage_thread_->enable_lru_timers(slots, 2);
}

int CMainStageHandler::handle_close()
{
	return 0;
}

int CMainStageHandler::handle_event(const SL_Seda_Event *event, void *db_inst, void *cache)
{
	switch (event->get_type())
	{
	case SL_Seda_EventType::TCP_SOCKET_OPEN:
		{
			SL_Seda_TcpSocketOpenEvent *open_event = (SL_Seda_TcpSocketOpenEvent *)event;
			
			open_event->rpc_proc(open_event->socket_handler_id, open_event->socket_handler, this, NULL, NULL, db_inst);
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
					//for (int i = 0; i < CTcpServerManager::Instance()->GetLoginServerManager()->GetConnectUserHandlerList()->GetList()->size(); i++)
					//{
					//	CConnectUserInfo *connected_user = CTcpServerManager::Instance()->GetLoginServerManager()->GetConnectUserHandlerList()->GetList()->at(i);

					//	if (NULL != connected_user)
					//	{
					//		int64 current_time = CTcpServerManager::Instance()->GetLoginServerManager()->GetSendControl()->get_current_time();
					//		int64 interval = current_time - connected_user->log_time;

					//		if (interval > (USER_CONNECT_TIMEOUT_INTERVAL/1000 - 10))
					//		{
					//			//connected_user->socket_handler->close();
					//		}
					//	}
					//}
					
					//CTcpServerManager::Instance()->GetCenterServerManager()->UserConnectTimeout_ = stage_thread_->set_lru_timer(TIMER_SLOT_USER_CONNECT_TIMEOUT, NULL);
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
			
			rpc_event->rpc_proc(rpc_event->socket_handler_id, rpc_event->socket_handler, this, rpc_event->rpc_message, rpc_event->remote_addr, db_inst);
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

int CMainStageHandler::LoginResp(SL_Socket_Handler *socket_handler)
{	
	return 0;
}

int CMainStageHandler::KeepAliveRequest(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db_inst)
{
	CommonMsgKeepAliveReq *req = (CommonMsgKeepAliveReq*)rpc_message;
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;

	delete req;

	return 0;
}

int CMainStageHandler::HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db_inst)
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

int CMainStageHandler::UserRegister(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db_inst)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgUserRegisterReq *req = (MsgUserRegisterReq*)rpc_message;
	LP_DB_INST inst = (LP_DB_INST)db_inst;

	CConnectUserInfo *connected_user_info = CTcpServerManager::Instance()->GetLoginServerManager()->GetConnectUserHandlerList()->FindByFD(socket_handler_id);

	char sql[256] = {0};
	snprintf(sql, 255, "insert into tb_user_info(user_name, password, nick_name) values('%s', '%s', '%s');", req->body_.user_name.c_str(), req->body_.password.c_str(), req->body_.nick_name.c_str());

	MsgUserRegisterRes res;
	SL_ByteBuffer buff(1024);

	if(0 == SP_DB_CommonAPI::Query(inst, sql))
	{
		memset(sql, 0x00, sizeof(sql));
		snprintf(sql, 255, "select user_id from tb_user_info where user_name = '%s' and nick_name = '%s' and password = '%s'", req->body_.user_name.c_str(), req->body_.nick_name.c_str(), req->body_.password.c_str());
		LP_DB_RES res_set = SP_DB_CommonAPI::GetQuery(inst, sql);
		if (res_set)
		{
			res.body_.error_code = 0;
			res.body_.error_text = "OK";

			LP_DB_ROWS row = SP_DB_CommonAPI::FatchRows(res_set);
			//MYSQL_ROW row = mysql_fetch_row(res_set);
			res.body_.user_id = row[0];
			SP_DB_CommonAPI::DeleteRes(res_set);
		}
	}
	else
	{
		res.body_.error_code = common::ErrorCode::PASS_EXPIRYDATE;
		res.body_.error_text = "Database Error!";
	}

	res.serialize_ex(&buff);
	((CLoginServerHandler*)socket_handler)->put_data(buff);
	
	delete req;

	return 0;
}


int CMainStageHandler::LoginRequest(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db_inst)
{
	CMainStageHandler *seda_handler = (CMainStageHandler*)seda_stagehandler;
	MsgUserPasswordLoginReq *req = (MsgUserPasswordLoginReq*)rpc_message;
	LP_DB_INST inst = (LP_DB_INST)db_inst;

	CConnectUserInfo *connected_user_info = CTcpServerManager::Instance()->GetLoginServerManager()->GetConnectUserHandlerList()->FindByFD(socket_handler_id);
	MsgUserPasswordLoginRes user_login_res;

	if (NULL != connected_user_info)
	{
		char sql[128] = {0};
		snprintf(sql, 127, "select password, user_name, nick_name from tb_user_info where user_id = '%d';", req->body_.user_id);
	
		LP_DB_RES res = SP_DB_CommonAPI::GetQuery(inst, sql);

		if (res)
		{
			LP_DB_ROWS row = SP_DB_CommonAPI::FatchRows(res);
			string encode_key = row[0];
			string user_name = row[1];
			string nick_name = row[2];
			SP_DB_CommonAPI::DeleteRes(res);

			login_server::ClientPasswordLoginReqChecksum login_check_sum;

			login_check_sum.user_id = req->body_.user_id;
			login_check_sum.password = encode_key;
			login_check_sum.client_random = req->body_.client_random;
			login_check_sum.server_random = connected_user_info->server_random;

			if (req->body_.check_sum == CSerialize::MD5<login_server::ClientPasswordLoginReqChecksum>(login_check_sum))
			{
				user_login_res.body_.error_code = 0;
				user_login_res.body_.error_text = "OK";

				login_server::ClientLoginResUserInfo user_info;
				user_info.user_id = req->body_.user_id;
				user_info.user_name = user_name;
				user_info.nick_name = nick_name;

				user_login_res.body_.user_info = CSerialize::AES<login_server::ClientLoginResUserInfo>(user_info, CSerialize::MD5(encode_key));

				common::SessionTicket session_ticket;
				session_ticket.user_id = req->body_.user_id;
				session_ticket.user_name = user_name;
				session_ticket.nick_name = nick_name;
				session_ticket.client_version = req->body_.client_version;
				//session_ticket.session_id = session_id;//user_info->session_id;

				SL_Crypto_CRTRand rand;
				rand.seed();
				char session_key_buff[32] = {0};
				itoa(rand.random_int(), session_key_buff, 10);

				char call_statement[128] = {0};
				snprintf(call_statement, 127, "call sp_login_register(%d,%s)", req->body_.user_id, session_key_buff);
				SP_DB_CommonAPI::Query(inst, call_statement);

				session_ticket.session_key = session_key_buff;
				user_login_res.body_.session_ticket = CSerialize::AES<common::SessionTicket>(session_ticket, CSerialize::MD5(encode_key));
				
				char sql[128] = {0};
				snprintf(sql, 127, "select roomgate_id, roomgate_name, ip, port from tb_roomgate_server_info;");

				LP_DB_RES roomgate_set = SP_DB_CommonAPI::GetQuery(inst, sql);

				if (roomgate_set)
				{
					unsigned int num_fields = mysql_num_fields(roomgate_set);
					while ((row = mysql_fetch_row(roomgate_set)))
					{
						common::ServiceInfo info;

						info.service_id = atoi(row[0]);
						info.service_name = row[1];
						info.service_addr.ip = row[2];
						info.service_addr.port = atoi(row[3]);

						user_login_res.body_.room_gate_info.push_back(info);
					}

					SP_DB_CommonAPI::DeleteRes(roomgate_set);
				}

				SL_ByteBuffer buff(2048);
				user_login_res.serialize_ex<SL_ByteBuffer>(&buff);

				((CLoginServerHandler*)socket_handler)->put_data(buff);

				delete req;
			}
			else
			{
				user_login_res.body_.error_code = common::ErrorCode::PASS_EXPIRYDATE;
				user_login_res.body_.error_text = "checksum error!";

				SL_ByteBuffer buff(1024);
				user_login_res.serialize_ex(&buff);

				((CLoginServerHandler*)socket_handler)->put_data(buff);

				delete req;
			}
        }
		else
		{
			
			user_login_res.body_.error_code = common::ErrorCode::PASS_EXPIRYDATE;
			user_login_res.body_.error_text = "datebase error!";

			SL_ByteBuffer buff(1024);
			user_login_res.serialize_ex<SL_ByteBuffer>(&buff);

			((CLoginServerHandler*)socket_handler)->put_data(buff);

			delete req;
		}
	}

	return 0;
}


int CMainStageHandler::ServiceDisconnect(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db_inst)
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

	default:
		break;
	}

	return 0;
}
