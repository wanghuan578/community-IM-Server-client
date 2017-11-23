
#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include "SL_DataType.h"
#include "SL_Socket_Handler.h"
#include <string>
#include <vector>
#include <assert.h>

using namespace std;

#define CENTERSERVER_KEEPALIVE_INTERVAL (1000 * 30)
#define CENTERSERVER_CONNECT_INTERVAL	(1000 * 5)

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

struct CObject
{
	uint32 service_id;
	string service_name;
	string ip;
	uint16 port;
	const SL_Socket_Handler *socket_handler;
};

struct CRoomServerInfo : public CObject
{
	string describtion;
	void *bridge_handler;
};

struct CCenterServerInfo : public CObject
{

};

struct CRoomServerAddr
{
	string ip;
	uint16 port;
};


class CUserObject
{
public:
	CUserObject()
	{
		user_id = 0;
		user_name = "";
		log_time = 0;
		server_random = 0;
		client_random = 0;
		socket_handler_id = 0;
		socket_handler = NULL;
		room_id = 0;
	}

	~CUserObject()
	{

	}

	uint32 user_id;
	string user_name;
	int64 log_time;
	int64 server_random;
	int64 client_random;
	int socket_handler_id;
	SL_Socket_Handler *socket_handler;
	string check_sum;
	string session_ticket;
	int room_id;
};

class CConnectUserInfo : public CUserObject
{

};

class CRoomGateUserInfo : public CUserObject
{

};
#endif