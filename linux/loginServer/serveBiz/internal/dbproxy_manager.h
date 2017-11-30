
#ifndef __DBPROXY_MANAGER_H__
#define __DBPROXY_MANAGER_H__

#include "SL_Socket_Select_Runner.h"
#include "client_control_base_adaptor.h"
#include "dbproxy_handler.h"
#include "service_handler_list.h"
#include "commonTypes.h"


class CDBProxyManager : public CClientControlBaseAdaptor<CDBProxyHandler, SL_Socket_Select_Runner<SL_Sync_ThreadMutex> >
{
public:
	CDBProxyManager()
		//:reconnect_timer_(NULL)
	{
	}

	~CDBProxyManager()
	{
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

	//inline CServiceHandlerList<CDBProxyInfo> *GetDBProxyHandler()
	//{
	//	return &DBProxyHandler_;
	//}

	//inline void SetReConnectTimer(SL_Seda_Timer *timer)
	//{
	//	reconnect_timer_ = timer;
	//}

	//inline SL_Seda_Timer *GetReConnectTimer()
	//{
	//	return reconnect_timer_;
	//}

private:
	int ServerID_;
	string ServerName_;
	CServiceHandlerList<CDBProxyInfo> DBProxyHandler_;
	//SL_Seda_Timer *reconnect_timer_;
};
#endif
