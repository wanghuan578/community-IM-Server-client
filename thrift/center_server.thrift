include "common.thrift"
namespace cpp community.center_server

enum MessageType
{
    MT_LOGIN_REQ   					= 300,
    MT_LOGIN_RES,
    MT_LOGOUT_REQ,
    MT_LOGOUT_RES,
    MT_INITIALIZE_ROOMSERVER_REQ,
    MT_INITIALIZE_ROOMSERVER_RES,
	MT_INITIALIZE_ROOMGATE_REQ,
    MT_INITIALIZE_ROOMGATE_RES,
	MT_DBPROXY_READY_NOTIFY,
	MT_DBPROXY_READY_RES,
	MT_ROOMGATE_UPDATE_NOTIFY,
	MT_ROOMGATE_UPDATE_RES,
	MT_ROOMSERVER_UPDATE_NOTIFY,
	MT_ROOMSERVER_UPDATE_RES,
    MT_ENTER_ROOM_REQ,
    MT_ENTER_ROOM_RES,
    MT_LEAVE_ROOM_REQ,
    MT_LEAVE_ROOM_RES,
	MT_USER_QUERY_REQ,
	MT_USER_QUERY_RES,
}

struct CenterLoginServerHelloRes
{
    1:i16       				error_code,
    2:string    				error_text,
    3:list<common.ServiceInfo> 	room_gate_list,
	4:list<common.ServiceInfo> 	dbproxy_list,
}

struct CenterRoomGateHelloRes
{
    1:i16       				error_code,
    2:string    				error_text,
    3:list<common.ServiceInfo> 	room_server_list,
	4:list<common.ServiceInfo> 	dbproxy_list,
}

struct CenterRoomServerHelloRes
{
    1:i16       				error_code,
    2:string    				error_text,
    3:list<common.ServiceInfo> 	dbproxy_list,
}

struct CenterDBProxyHelloRes
{
    1:i16       				error_code,
    2:string    				error_text,
}

struct CenterLoginReq
{
    1:i32       user_id,
    2:string    user_name,
    3:string    session_id,
    4:i64       login_time,
}

struct CenterLogoutReq
{
    1:i32       user_id,
    2:string    user_name,
}

struct CenterRoomGateUpdateNotify
{
    1:i16       			update_type,	//0:add, -1:delete
    2:common.ServiceInfo 	room_gate_info,
}

struct CenterRoomServerUpdateNotify
{
    1:i16       			update_type,	//0:add, -1:delete
    2:common.ServiceInfo 	room_server_info,
}

struct CenterDBProxyReadyNotify
{
    1:i16       			update_type,	//0:add, -1:delete
    2:common.ServiceInfo 	dbproxy_info,
}

struct RoomServerInitializeReq
{
	1:i32	room_server_id,
}

struct RoomGateInitializeReq
{
	1:i32	room_gate_id,
}

struct CenterEnterRoomReq
{
    1:i32       user_id,
    2:string    user_name,
    3:i32       room_id,
	4:string    session_id,  
    5:i32       room_service_id,
    6:i32       room_gate_id,
}

struct CenterLeaveRoomReq
{
	1:i32       user_id,
    2:i32       room_id,
    3:i32       room_gate_id,
}

struct CenterUserQueryReq
{
    1:i32  user_id,
}

struct CenterUserInfo
{
    1:i32       user_id,
    2:string    user_name,
    3:i32       room_id,
    4:i32       room_service_id,
    5:i32       room_gate_id,
	6:string 	session_pw,
}

struct CenterUserQueryRes
{
    1:i16       					error_code,
    2:string    					error_text,
    3:CenterUserInfo			user_info,
}