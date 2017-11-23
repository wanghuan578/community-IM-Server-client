
#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include "SL_DataType.h"
#include "SL_Socket_Handler.h"
#include <string>
#include <vector>
#include <assert.h>

using namespace std;

#define WM_RPC_MESSAGE (WM_USER + 100)

#define CENTERSERVER_KEEPALIVE_INTERVAL (1000 * 30)
#define CENTERSERVER_CONNECT_INTERVAL	(1000 * 5)

typedef int(*ClientBizToUICallback)(int message_type, int error_code, void *message);

typedef enum
{
	TIMER_SLOT_CENTER_SERVER_CONNECT		= 0,
	TIMER_SLOT_CENTER_SERVER_KEEPALIVE,
}TimerSlotType;

typedef enum
{
	SERVER_REGISTER_TYPE_CENTER				= 0,
	SERVER_REGISTER_TYPE_LOGIN,
	SERVER_REGISTER_TYPE_ROOMGATE,
	SERVER_REGISTER_TYPE_ROOM,
	SERVER_REGISTER_TYPE_DBPROXY,
}ServerRegisterType;

typedef enum
{
	RPC_HELLO_NOTIFY,
	RPC_MESSAGE_LOGIN_RESP,
	RPC_MESSAGE_CHAT_NOTIFY,
	PRC_MESSAGE_LOGOUT_RESP,
	RPC_MESSAGE_AVERAGE_CONSUMPTION,
	RPC_MESSAGE_MAX
}SL_RPC_MESSAGE;

struct CNObject
{
	uint32 service_id;
	string service_name;
	string ip;
	uint16 port;
	const SL_Socket_Handler *socket_handler;
};

struct CLoginUserInfo : public CNObject
{

};

struct CCenterServerInfo : public CNObject
{

};

struct CRoomGateInfo : public CNObject
{

};
#endif