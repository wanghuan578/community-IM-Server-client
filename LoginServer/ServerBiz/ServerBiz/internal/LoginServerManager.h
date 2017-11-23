
#ifndef __LOGIN_SERVER_MANAGER_H__
#define __LOGIN_SERVER_MANAGER_H__

#include "SL_Socket_Iocp_Runner.h"
#include "LoginServerHandler.h"
#include "ServerControlBaseAdaptor.h"
//#include "WeightedRound.h"
#include "ServerData.h"
#include "ServiceHandlerList.h"
#include "UserHandlerList.h"
#include "CommonTypes.h"

class CLoginServerManager : public CServerControlBaseAdaptor<CLoginServerHandler, SL_Socket_Iocp_Runner<SL_Sync_ThreadMutex>>
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
		return CServerControlBaseAdaptor::Initialize();
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

	inline CUserHandlerList<CLoginUserInfo> *GetLoginUserHandlerList()
	{
		return &LoginUserList_;
	}

	inline CUserHandlerList<CConnectUserInfo> *GetConnectUserHandlerList()
	{
		return &ConnectUserList_;
	}

private:
	int ServerID_;
	string ServerName_;
	CUserHandlerList<CLoginUserInfo> LoginUserList_;
	CUserHandlerList<CConnectUserInfo> ConnectUserList_;
};
#endif
