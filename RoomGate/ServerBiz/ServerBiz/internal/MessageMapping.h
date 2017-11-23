
#ifndef __MESSAGE_MAPPING_I_H__
#define __MESSAGE_MAPPING_I_H__

#include "SL_Rpc_Message.h"
#include "common_types.h"
#include "center_server_types.h"
#include "login_server_types.h"
#include "room_gate_types.h"
//#include "room_server_types.h"

using namespace community;


typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::HelloNotify, common::MessageType::MT_HELLO_NOTIFY>  MsgHelloNotify;
//typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::HelloNotifyEx, common::MessageType::MT_ROOMGATE_HELLO_NOTIFY>  MsgRoomGateHelloNotify;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::HelloReq, common::MessageType::MT_HELLO_REQ> MsgRoomGateHelloReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterServerRoomGateHelloRes, common::MessageType::MT_HELLO_RES> MsgRoomGateHelloRes;

//typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::RoomGateInitializeReq, center_server::MessageType::MT_INITIALIZE_ROOMGATE_REQ> MsgRoomGateInitializeReq;
//typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, center_server::MessageType::MT_INITIALIZE_ROOMGATE_RES> MsgRoomGateInitializeRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterServerRoomServerUpdateNotify, center_server::MessageType::MT_ROOMSERVER_UPDATE_NOTIFY> MsgCenterServerRoomServerUpdateNotify;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, center_server::MessageType::MT_ROOMSERVER_UPDATE_RES> MsgCenterServerRoomServerUpdateRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, room_gate::ConnectReq, room_gate::MessageType::CONNECT_REQ> MsgRoomGateConnectReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, room_gate::MessageType::CONNECT_RES> MsgRoomGateConnectRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterServerUserQueryReq, center_server::MessageType::MT_USER_QUERY_REQ> MsgCenterServerUserQueryReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterServerUserQueryRes, center_server::MessageType::MT_USER_QUERY_RES> MsgCenterServerUserQueryRes;


typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::KeepAliveReq, common::MessageType::MT_KEEPALIVE_REQ>   CommonMsgKeepAliveReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::KeepAliveRes, common::MessageType::MT_KEEPALIVE_RES>   CommonMsgKeepAliveRes;
#endif