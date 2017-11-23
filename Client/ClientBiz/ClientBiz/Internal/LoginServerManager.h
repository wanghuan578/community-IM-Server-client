
#ifndef __LOGIN_SERVER_MANAGER_H__
#define __LOGIN_SERVER_MANAGER_H__

#include "SL_Sync_Mutex.h"
#include "SL_Socket_Select_Runner.h"
#include "LoginServerHandler.h"
#include "ClientControlBaseAdaptor.h"
#include "CommonTypes.h"



class CLoginServerManager : public CClientControlBaseAdaptor<CLoginServerHandler, SL_Socket_Select_Runner<SL_Sync_ThreadMutex>>
{
public:
	CLoginServerManager()
	{
	}

	~CLoginServerManager()
	{
	}

	inline bool Init()
	{
		return CClientControlBaseAdaptor::Initialize();
	}

	inline bool Open(string ip, int port)
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

private:
	int ServerID_;
	string ServerName_;
};
#endif