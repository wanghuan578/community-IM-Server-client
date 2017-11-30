
#ifndef __LOGIN_SERVER_MANAGER_H__
#define __LOGIN_SERVER_MANAGER_H__

//#include "SL_Socket_Iocp_Runner.h"
#include "SL_Socket_Epoll_Runner.h"

#include "login_server_handler.h"
#include "server_control_base_adaptor.h"
#include "serverData.h"
#include "service_handler_list.h"
#include "user_handler_list.h"


class CLoginServerManager : public CServerControlBaseAdaptor<CLoginServerHandler, SL_Socket_Epoll_Runner<SL_Sync_ThreadMutex> >
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
