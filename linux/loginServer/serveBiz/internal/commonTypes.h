
#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include "SL_DataType.h"
#include "SL_Socket_Handler.h"
#include <string>
#include <vector>
#include <assert.h>

using namespace std;

#define USER_CONNECT_TIMEOUT_INTERVAL	(1000 * 30)
#define CENTERSERVER_CONNECT_INTERVAL	(1000 * 2)

typedef enum
{
	TIMER_SLOT_CENTER_SERVER_CONNECT		= 0,
	TIMER_SLOT_USER_CONNECT_TIMEOUT,
}TimerSlotType;

typedef enum
{
	REGISTER_TYPE_CENTER				= 0,
	REGISTER_TYPE_LOGIN,
	REGISTER_TYPE_ROOMGATE,
	REGISTER_TYPE_ROOM,
	REGISTER_TYPE_DBPROXY,
	REGISTER_TYPE_USER,
}RegisterType;

struct CServerObject
{
	CServerObject()
	{
		service_id = 0;
		service_name = "";
		ip = "";
		port = 0;
		socket_handler_id = 0;
		socket_handler = NULL;
	}

	uint32 service_id;
	string service_name;
	string ip;
	uint16 port;
	int socket_handler_id;
	const SL_Socket_Handler *socket_handler;
};

struct CLoginServerInfo : public CServerObject
{

};

struct CCenterServerInfo : public CServerObject
{

};

struct CRoomGateInfo : public CServerObject
{

};

struct CDBProxyInfo : public CServerObject
{

};

struct CUserObject
{
	CUserObject()
	{
		user_id = 0;
		log_time = 0;
		server_random = 0;
		client_random = 0;
		socket_handler_id = 0;
		socket_handler = NULL;
	}

	uint32 user_id;
	string user_name;
	string password;
	int64 log_time;
	int64 server_random;
	int64 client_random;
	string check_sum;
	string session_id;
	string session_key;
	int socket_handler_id;
	SL_Socket_Handler *socket_handler;
};

struct CLoginUserInfo : public CUserObject
{
};

struct CConnectUserInfo : public CUserObject
{
};
#endif