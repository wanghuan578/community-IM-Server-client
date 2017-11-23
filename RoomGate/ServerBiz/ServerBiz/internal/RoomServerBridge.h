
#ifndef __ROOM_SERVER_BRIDGE_H__
#define __ROOM_SERVER_BRIDGE_H__

#include "SL_Sync_Mutex.h"
#include "SL_Socket_Select_Runner.h"
#include "RoomServerHandler.h"
#include "ClientControlBaseAdaptor.h"


class CRoomServerBridge : public CClientControlBaseAdaptor<CRoomServerHandler, SL_Socket_Select_Runner<SL_Sync_ThreadMutex>>
{
public:
	CRoomServerBridge()
	{
	}

	~CRoomServerBridge()
	{
	}

	inline bool Init()
	{
		return CClientControlBaseAdaptor::Initialize();
	}

	inline bool Open(string ip, uint16 port)
	{
		CClientControlBaseAdaptor::SetIpAddress(ip);
		CClientControlBaseAdaptor::SetPort(port);

		return CClientControlBaseAdaptor::Initialize();
	}

	inline void SetServerID(int server_id)
	{
		ServerID_ = server_id;
	}

	inline int GetServerID()
	{
		return ServerID_;
	}

	inline void SetServerName(string server_name)
	{
		ServerName_ = server_name;
	}

	inline string GetServerName()
	{
		return ServerName_;
	}

	inline void SetSocketID(int sid)
	{
		socket_id = sid;
	}

	inline int GetSocketID()
	{
		return socket_id;
	}

private:
	int ServerID_;
	string ServerName_;
	int socket_id;
};
#endif