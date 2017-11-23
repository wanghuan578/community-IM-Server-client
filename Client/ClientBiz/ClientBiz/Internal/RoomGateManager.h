
#ifndef __ROOM_GATE_MANAGER_H__
#define __ROOM_GATE_MANAGER_H__

#include "SL_Sync_Mutex.h"
#include "SL_Socket_Select_Runner.h"
#include "RoomGateHandler.h"
#include "ClientControlBaseAdaptor.h"

	


class CRoomGateManager : public CClientControlBaseAdaptor<CRoomGateHandler, SL_Socket_Select_Runner<SL_Sync_ThreadMutex>>
{
public:
	CRoomGateManager()
	{
	}

	~CRoomGateManager()
	{
	}

	inline bool Open(string ip, int port)
	{
		CClientControlBaseAdaptor::SetIpAddress(ip);
		CClientControlBaseAdaptor::SetPort(port);

		return CClientControlBaseAdaptor::Initialize();
	}

	inline bool Initalize()
	{
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

private:
	int ServerID_;
	string ServerName_;
	//CUserHandler<CUserList<CRoomServerInfo>> UserInfo_;
};
#endif