
#include "LoginServerHandler.h"
#include "SL_Rpc_Message.h"
#include "MessageMapping.h"
#include "TcpClientManager.h"


CLoginServerHandler::CLoginServerHandler()
{
}

CLoginServerHandler::~CLoginServerHandler()
{
}

int CLoginServerHandler::do_open()
{
	SL_Socket_CommonAPI::socket_set_tcpnodelay(socket_, 1);

	CTcpClientManager::Instance()->GetLoginServerManager()->GetSendControl()->add_handler(this, this);

	return 0;
}

int CLoginServerHandler::do_message(const char *msg, int len)
{

	SL_Rpc_MessageParser::parse_head_ex<SL_Rpc_MessageHeadEx>((char*)msg, sizeof(SL_Rpc_MessageHeadEx));
	SL_Rpc_MessageHeadEx *msg_head = (SL_Rpc_MessageHeadEx*)msg;

#ifdef CLIENTBIZ_LOG
	SL_LOG_TRACE(logger, "msghead: type:"<<msg_head->type<<" from:"<<msg_head->source<<" to:"<<msg_head->destination<<" room_id:"<<msg_head->attach_id1<<" roomservice_id:"<<msg_head->attach_id2<<" roomgate_id:"<<msg_head->attach_id3);
#endif

	int ret = -1;

	SL_Seda_RpcMessageEvent rpc_event;

	switch (msg_head->type)
	{
	case common::MessageType::MT_HELLO_NOTIFY:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgHelloNotify>((char*)msg, len, socket_, this, CMainStageHandler::LS_HelloNotify, &rpc_event, NULL);
		}
		break;

	case login_server::MessageType::MT_CLIENT_LOGIN_RES:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgUserPasswordLoginRes>((char*)msg, len, socket_, this, CMainStageHandler::LoginRes, &rpc_event, NULL);
		}
		break;

	case login_server::MessageType::MT_CLIENT_REGISTER_RES:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgUserRegisterRes>((char*)msg, len, socket_, this, CMainStageHandler::RegisterRes, &rpc_event, NULL);
		}

	default:
		break;
	}

	if (ret >= 0)
	{
		CTcpClientManager::Instance()->GetSedaStageHandler()->push_event(&rpc_event);
	}

	return 0;
}

int CLoginServerHandler::do_close()
{
	SL_Seda_TcpSocketCloseEvent expire_event;
	expire_event.socket_handler_id = socket_;
	expire_event.socket_handler = this;
	expire_event.rpc_proc = CMainStageHandler::ServiceDisconnect;
	expire_event.source = SERVER_REGISTER_TYPE_LOGIN;

	CTcpClientManager::Instance()->GetSedaStageHandler()->push_event(&expire_event);

	return 0;
}