

#include "TcpServerManager.h"
#include "MessageMapping.h"




CCenterServerHandler::CCenterServerHandler()
{
}

CCenterServerHandler::~CCenterServerHandler()
{
}

int CCenterServerHandler::do_open()
{
	SL_Socket_CommonAPI::socket_set_tcpnodelay(socket_, 1);

	CTcpServerManager::Instance()->GetCenterServerManager()->GetSendControl()->add_handler(this, this);

	return 0;
}

int CCenterServerHandler::do_message(const char *msg, int len)
{
	SL_Rpc_MessageParser::parse_head_ex<SL_Rpc_MessageHeadEx>((char *)msg, sizeof(SL_Rpc_MessageHeadEx));
	SL_Rpc_MessageHeadEx *msg_head = (SL_Rpc_MessageHeadEx*)msg;

#ifdef SET_SERVER_LOG
	SL_LOG_TRACE(logger, "msghead: type:"<<msg_head->type<<" from:"<<msg_head->source<<" to:"<<msg_head->destination<<" room_id:"<<msg_head->attach_id1<<" roomservice_id:"<<msg_head->attach_id2<<" roomgate_id:"<<msg_head->attach_id3);
#endif

	int ret = -1;

	SL_Seda_RpcMessageEvent rpc_event;

	int len1 = sizeof(SL_Seda_RpcMessageEvent);

	switch (msg_head->type)
	{
	case common::MessageType::MT_HELLO_NOTIFY:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgHelloNotify>((char*)msg, len, socket_, this, CMainStageHandler::CS_HelloNotify, &rpc_event, NULL);		
		}
		break;

	case common::MessageType::MT_HELLO_RES:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgRoomGateHelloRes>((char*)msg, len, socket_, this, CMainStageHandler::CS_HelloRes, &rpc_event, NULL);		
		}
		break;

	case center_server::MessageType::MT_ROOMSERVER_UPDATE_NOTIFY:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgCenterServerRoomServerUpdateNotify>((char*)msg, len, socket_, this, CMainStageHandler::RoomServerUpdateNotify, &rpc_event, NULL);		
		}
		break;

	case center_server::MessageType::MT_USER_QUERY_RES:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgCenterServerUserQueryRes>((char*)msg, len, socket_, this, CMainStageHandler::CS_UserQueryRes, &rpc_event, NULL);	
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

int CCenterServerHandler::do_close()
{
	SL_Seda_TcpSocketCloseEvent expire_event;

	expire_event.socket_handler_id = socket_;
	expire_event.socket_handler = this;
	expire_event.rpc_proc = CMainStageHandler::ServiceDisconnect;
	expire_event.attach_id1 = SERVER_REGISTER_TYPE_CENTER;

	CTcpServerManager::Instance()->GetSedaStageHandler()->push_event(&expire_event);

	return 0;
}