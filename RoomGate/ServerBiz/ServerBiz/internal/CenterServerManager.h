
#ifndef __CENTER_SERVER_MANAGER_H__
#define __CENTER_SERVER_MANAGER_H__

#include "SL_Sync_Mutex.h"
#include "SL_Socket_Select_Runner.h"
#include "CenterServerHandler.h"
#include "ClientControlBaseAdaptor.h"
#include "ServiceHandlerList.h"
#include "CommonTypes.h"


class CCenterServerManager : public CClientControlBaseAdaptor<CCenterServerHandler, SL_Socket_Select_Runner<SL_Sync_ThreadMutex>>
{
public:
	CCenterServerManager()
		:reconnect_timer_(NULL)
	{
	}

	~CCenterServerManager()
	{
	}

	inline bool Init()
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

	inline CServiceHandlerList<CCenterServerInfo> *GetServerHandler()
	{
		return &ServerHandler_;
	}

	inline void SetReConnectTimer(SL_Seda_Timer *timer)
	{
		reconnect_timer_ = timer;
	}

	inline SL_Seda_Timer *GetReConnectTimer()
	{
		return reconnect_timer_;
	}

private:
	int ServerID_;
	string ServerName_;
	CServiceHandlerList<CCenterServerInfo> ServerHandler_;
	SL_Seda_Timer *reconnect_timer_;
};
#endif