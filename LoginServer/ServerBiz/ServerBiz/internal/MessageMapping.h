
#ifndef __MESSAGE_MAPPING_H__
#define __MESSAGE_MAPPING_H__

#include "SL_Rpc_Message.h"
#include "common_types.h"
#include "center_server_types.h"
#include "login_server_types.h"
#include "dbproxy_server_types.h"
//#include "room_server_types.h"

using namespace community;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::HelloNotify, common::MessageType::MT_HELLO_NOTIFY> MsgHelloNotify;
//typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::HelloNotifyEx, common::MessageType::MT_DBPROXY_HELLO_NOTIFY> MsgDBProxyHelloNotify;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::HelloReq, common::MessageType::MT_HELLO_REQ> MsgLoginServerHelloReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterServerLoginServerHelloRes, common::MessageType::MT_HELLO_RES> MsgLoginServerHelloRes;

//typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterRoomGateInitializeNotify, center_server::MessageType::MT_INITIALIZE_ROOMGATE_REQ> MsgRoomGateInitializeNotify;
//typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, center_server::MessageType::MT_INITIALIZE_ROOMGATE_RES> MsgRoomGateInitializeRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterServerRoomGateUpdateNotify, center_server::MessageType::MT_ROOMGATE_UPDATE_NOTIFY> MsgCenterServerRoomGateUpdateNotify;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, center_server::MessageType::MT_ROOMGATE_UPDATE_RES> MsgCenterServerRoomGateUpdateRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterServerLoginReq, center_server::MessageType::MT_LOGIN_REQ> MsgCenterServerUserLoginReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, center_server::MessageType::MT_LOGIN_RES> MsgCenterServerUserLoginRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, login_server::ClientPasswordLoginReq, login_server::MessageType::MT_CLIENT_PASSWORD_LOGIN_REQ> MsgUserPasswordLoginReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, login_server::ClientLoginRes, login_server::MessageType::MT_CLIENT_LOGIN_RES> MsgUserPasswordLoginRes;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, login_server::UserRegisterReq, login_server::MessageType::MT_CLIENT_REGISTER_REQ> MsgUserRegisterReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, login_server::UserRegisterRes, login_server::MessageType::MT_CLIENT_REGISTER_RES> MsgUserRegisterRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterServerDBProxyUpdateNotify, center_server::MessageType::MT_DBPROXY_UPDATE_NOTIFY> MsgCenterServerDBProxyUpdateNotify;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, center_server::MessageType::MT_DBPROXY_UPDATE_RES> MsgCenterServerDBProxyUpdateRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, dbproxy_server::LoginGetUserInfoReq, dbproxy_server::MessageType::MT_LOGIN_GET_USERINFO_REQ> MsgLoginGetUserInfoReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, dbproxy_server::LoginGetUserInfoRes, dbproxy_server::MessageType::MT_LOGIN_GET_USERINFO_RES> MsgLoginGetUserInfoRes;



typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::KeepAliveReq, common::MessageType::MT_KEEPALIVE_REQ>   CommonMsgKeepAliveReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::KeepAliveRes, common::MessageType::MT_KEEPALIVE_RES>   CommonMsgKeepAliveRes;

#endif