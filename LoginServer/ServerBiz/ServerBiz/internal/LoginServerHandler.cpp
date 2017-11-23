
#include "LoginServerHandler.h"
#include "SL_Rpc_Message.h"
#include "MessageMapping.h"
#include "ServerBiz.h"
#include "TcpServerManager.h"
#include "CommonTypes.h"


CLoginServerHandler::CLoginServerHandler()
{
}

CLoginServerHandler::~CLoginServerHandler()
{
}

int CLoginServerHandler::do_open()
{
	SL_Socket_CommonAPI::socket_set_tcpnodelay(socket_, 1);

	CTcpServerManager::Instance()->GetLoginServerManager()->GetSendControl()->add_handler(this, this);

	SL_Seda_TcpSocketOpenEvent open_event;
	open_event.socket_handler = this;
	open_event.socket_handler_id = (int)socket_;
	open_event.rpc_proc = CMainStageHandler::HelloNotify;

	CTcpServerManager::Instance()->GetSedaStageHandler()->push_event(&open_event);

	return 0;
}

int CLoginServerHandler::do_message(const char *msg, int len)
{
	SL_Rpc_MessageParser::parse_head_ex<SL_Rpc_MessageHeadEx>((char *)msg, sizeof(SL_Rpc_MessageHeadEx));
	SL_Rpc_MessageHeadEx *msg_head = (SL_Rpc_MessageHeadEx*)msg;

#ifdef SET_SERVER_LOG
	SL_LOG_TRACE(logger, "msghead: type:"<<msg_head->type<<" from:"<<msg_head->source<<" to:"<<msg_head->destination<<" room_id:"<<msg_head->attach_id1<<" roomservice_id:"<<msg_head->attach_id2<<" roomgate_id:"<<msg_head->attach_id3);
#endif

	int ret = -1;

	SL_Seda_RpcMessageEvent rpc_event;

	switch (msg_head->type)
	{
	case login_server::MessageType::MT_CLIENT_REGISTER_REQ:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgUserRegisterReq>((char*)msg, len, (int)socket_, this, CMainStageHandler::UserRegister, &rpc_event, NULL);
		}
		break;

	case login_server::MessageType::MT_CLIENT_PASSWORD_LOGIN_REQ:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgUserPasswordLoginReq>((char*)msg, len, (int)socket_, this, CMainStageHandler::LoginRequest, &rpc_event, NULL);		
		}
		break;



	default:
		break;
	}

	if (ret >= 0)
	{
		CTcpServerManager::Instance()->GetSedaStageHandler()->push_event(&rpc_event);
	}

	return 0;
}

int CLoginServerHandler::do_close()
{
	SL_Seda_TcpSocketCloseEvent expire_event;

	expire_event.socket_handler_id = (int)socket_;
	expire_event.socket_handler = this;
	expire_event.rpc_proc = CMainStageHandler::ServiceDisconnect;
	expire_event.source = REGISTER_TYPE_USER;

	CTcpServerManager::Instance()->GetSedaStageHandler()->push_event(&expire_event);

	return 0;
}