
#include "RoomGateHandler.h"
//#include "SL_Rpc_Message.h"
#include "MessageMapping.h"
#include "TcpClientManager.h"



CRoomGateHandler::CRoomGateHandler()
{
}

CRoomGateHandler::~CRoomGateHandler()
{
}

int CRoomGateHandler::do_open()
{
	SL_Socket_CommonAPI::socket_set_tcpnodelay(socket_, 1);

	CTcpClientManager::Instance()->GetRoomGateManager()->GetSendControl()->add_handler(this, this);

	return 0;
}

int CRoomGateHandler::do_message(const char *msg, int len)
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
			ret = SL_Rpc_MessageParser::message_to_event<MsgHelloNotify>((char*)msg, len, socket_, this, CMainStageHandler::RG_HelloNotify, &rpc_event, NULL);
		}
		break;

	case room_gate::MessageType::MT_CONNECT_RES:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgRoomGateConnectRes>((char*)msg, len, socket_, this, CMainStageHandler::ConnectRes, &rpc_event, NULL);
		}
		break;
		
	case room_gate::MessageType::MT_GET_ROOMLIST_RES:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgGetRoomListRes>((char*)msg, len, socket_, this, CMainStageHandler::GetRoomGateListRes, &rpc_event, NULL);
		}
		break;

	case room_server::MessageType::MT_ENTER_ROOM_RES:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgEnterRoomRes>((char*)msg, len, socket_, this, CMainStageHandler::EnterRoomRes, &rpc_event, NULL);
		}
		break;

	case room_server::MessageType::MT_ENTER_ROOM_NOTIFY:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgEnterRoomNotify>((char*)msg, len, socket_, this, CMainStageHandler::EnterRoomNotify, &rpc_event, NULL);
		}
		break;

	case room_server::MessageType::MT_GET_USER_LIST_RES:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgGetUserListRes>((char*)msg, len, socket_, this, CMainStageHandler::GetUserListRes, &rpc_event, NULL);
		}
		break;

	case room_server::MessageType::MT_CHAT_NOTIFY:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgRoomServerChatNotify>((char*)msg, len, socket_, this, CMainStageHandler::ChatNotify, &rpc_event, NULL);
		}
		break;

	case room_server::MessageType::MT_LEAVE_ROOM_RES:
		{
			ret = SL_Rpc_MessageParser::message_to_event<MsgLeaveRoomRes>((char*)msg, len, socket_, this, CMainStageHandler::LeaveRoomRes, &rpc_event, NULL);
		}
		break;

	default:
		break;
	}

	if (ret >= 0)
	{
		CTcpClientManager::Instance()->GetSedaStageHandler()->push_event(&rpc_event);
	}

	return 0;
}

int CRoomGateHandler::do_close()
{
	SL_Seda_TcpSocketCloseEvent expire_event;

	expire_event.socket_handler_id = socket_;
	expire_event.socket_handler = this;
	expire_event.rpc_proc = CMainStageHandler::ServiceDisconnect;
	expire_event.source = SERVER_REGISTER_TYPE_ROOMGATE;

	CTcpClientManager::Instance()->GetSedaStageHandler()->push_event(&expire_event);

	return 0;
}