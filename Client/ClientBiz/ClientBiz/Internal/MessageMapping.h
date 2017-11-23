
#ifndef MESSAGE_MAPPING_I_H
#define MESSAGE_MAPPING_I_H

#include "SL_Rpc_Message.h"
#include "common_types.h"
#include "login_server_types.h"
#include "room_gate_types.h"
#include "room_server_types.h"



using namespace community;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::HelloNotify, common::MessageType::MT_HELLO_NOTIFY> MsgHelloNotify;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, login_server::ClientPasswordLoginReq, login_server::MessageType::MT_CLIENT_PASSWORD_LOGIN_REQ> MsgUserPasswordLoginReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, login_server::ClientLoginRes, login_server::MessageType::MT_CLIENT_LOGIN_RES> MsgUserPasswordLoginRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, room_gate::ConnectReq, room_gate::MessageType::MT_CONNECT_REQ> MsgRoomGateConnectReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, room_gate::MessageType::MT_CONNECT_RES> MsgRoomGateConnectRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonNull, room_gate::MessageType::MT_GET_ROOMLIST_REQ> MsgGetRoomListReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, room_gate::GetRoomlistRes, room_gate::MessageType::MT_GET_ROOMLIST_RES> MsgGetRoomListRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, SL_Rpc_NULL_MessageBody, room_server::MessageType::MT_GET_USER_LIST_REQ> MsgGetUserListReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, room_server::GetUserListRes, room_server::MessageType::MT_GET_USER_LIST_RES> MsgGetUserListRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, room_server::EnterRoomReq, room_server::MessageType::MT_ENTER_ROOM_REQ> MsgEnterRoomReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, room_server::EnterRoomRes, room_server::MessageType::MT_ENTER_ROOM_RES> MsgEnterRoomRes;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::UserInfo, room_server::MessageType::MT_ENTER_ROOM_NOTIFY> MsgEnterRoomNotify;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonNull, room_server::MessageType::MT_LEAVE_ROOM_REQ> MsgLeaveRoomReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, room_server::MessageType::MT_LEAVE_ROOM_RES> MsgLeaveRoomRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, room_server::ChatReq, room_server::MessageType::MT_CHAT_REQ> MsgRoomServerChatReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, room_server::MessageType::MT_CHAT_RES> MsgRoomServerChatRes;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, room_server::ChatReq, room_server::MessageType::MT_CHAT_NOTIFY> MsgRoomServerChatNotify;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, login_server::UserRegisterReq, login_server::MessageType::MT_CLIENT_REGISTER_REQ> MsgUserRegisterReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, login_server::UserRegisterRes, login_server::MessageType::MT_CLIENT_REGISTER_RES> MsgUserRegisterRes;


#endif