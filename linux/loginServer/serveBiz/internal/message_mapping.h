
#ifndef __MESSAGE_MAPPING_H__
#define __MESSAGE_MAPPING_H__

#include "SL_Rpc_Message.h"
#include "common_types.h"
#include "center_server_types.h"
#include "login_server_types.h"
#include "dbproxy_server_types.h"


using namespace community;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::HelloNotify, common::MessageType::MT_HELLO_NOTIFY> MsgHelloNotify;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::HelloReq, common::MessageType::MT_HELLO_REQ> MsgLoginServerHelloReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterLoginServerHelloRes, common::MessageType::MT_HELLO_RES> MsgLoginServerHelloRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterRoomGateUpdateNotify, center_server::MessageType::MT_ROOMGATE_UPDATE_NOTIFY> MsgCenterRoomGateUpdateNotify;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, center_server::MessageType::MT_ROOMGATE_UPDATE_RES> MsgCenterServerRoomGateUpdateRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterLoginReq, center_server::MessageType::MT_LOGIN_REQ> MsgCenterUserLoginReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, center_server::MessageType::MT_LOGIN_RES> MsgCenterUserLoginRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, login_server::ClientPasswordLoginReq, login_server::MessageType::MT_CLIENT_PASSWORD_LOGIN_REQ> MsgUserPasswordLoginReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, login_server::ClientLoginRes, login_server::MessageType::MT_CLIENT_LOGIN_RES> MsgUserPasswordLoginRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, center_server::CenterDBProxyReadyNotify, center_server::MessageType::MT_DBPROXY_READY_NOTIFY> MsgCenterDBProxyReadyNotify;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, center_server::MessageType::MT_DBPROXY_READY_RES> MsgCenterDBProxyReadyRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonNull, dbproxy_server::MessageType::MT_LOGIN_CHECK_REQ> MsgLoginCheckReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::CommonRes, dbproxy_server::MessageType::MT_LOGIN_CHECK_RES> MsgLoginCheckRes;

typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, dbproxy_server::LoginGetUserInfoReq, dbproxy_server::MessageType::MT_LOGIN_GET_USERINFO_REQ> MsgLoginGetUserInfoReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, dbproxy_server::LoginGetUserInfoRes, dbproxy_server::MessageType::MT_LOGIN_GET_USERINFO_RES> MsgLoginGetUserInfoRes;



typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::KeepAliveReq, common::MessageType::MT_KEEPALIVE_REQ>   CommonMsgKeepAliveReq;
typedef SL_Rpc_Message<SL_Rpc_MessageHeadEx, common::KeepAliveRes, common::MessageType::MT_KEEPALIVE_RES>   CommonMsgKeepAliveRes;

#endif