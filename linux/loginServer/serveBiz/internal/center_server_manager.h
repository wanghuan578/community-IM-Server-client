
#ifndef __CENTER_SERVER_MANAGER_H__
#define __CENTER_SERVER_MANAGER_H__

#include "SL_Sync_Mutex.h"
#include "SL_Socket_Select_Runner.h"
#include "center_server_handler.h"
#include "client_control_base_adaptor.h"
#include "service_handler_list.h"
#include "commonTypes.h"
#include "serverData.h"
#include "weightedRound.h"


class CCenterServerManager : public CClientControlBaseAdaptor<CCenterServerHandler, SL_Socket_Select_Runner<SL_Sync_ThreadMutex> >
{
public:
	CCenterServerManager()
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

	//inline CServiceHandlerList<CCenterServerInfo> *GetCenterServerHandler()
	//{
	//	return &CenterServerHandler_;
	//}

	inline CWeightedRound<CServerData> *GetRoomGateHandlerList()
	{
		return &RoomGateHandlerList_;
	}

private:
	int ServerID_;
	string ServerName_;

	//CServiceHandlerList<CCenterServerInfo> CenterServerHandler_;
	CWeightedRound<CServerData> RoomGateHandlerList_;
};
#endif
