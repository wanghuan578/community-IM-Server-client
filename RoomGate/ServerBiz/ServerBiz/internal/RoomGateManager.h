
#ifndef __ROOM_GATE_HANDLER_H__
#define __ROOM_GATE_HANDLER_H__


#include "SL_Socket_Iocp_Runner.h"
#include "RoomGateHandler.h"
#include "ServerControlBaseAdaptor.h"
#include "UserHandlerList.h"
#include <string>

using namespace std;


class CRoomGateManager : public CServerControlBaseAdaptor<CRoomGateHandler, SL_Socket_Iocp_Runner<SL_Sync_ThreadMutex>>
{
public:
	CRoomGateManager()
	{

	}

	~CRoomGateManager()
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

	inline short GetServiceWeight()
	{
		return ServiceWeight_;
	}

	inline void SetServiceWeight(short weight)
	{
		ServiceWeight_ = weight;
	}

	inline CUserHandlerList<CRoomGateUserInfo> *GetRoomGateUserHandlerList()
	{
		return &RoomGateUserHandlerList_;
	}

	inline CUserHandlerList<CConnectUserInfo> *GetConnectUserHandlerList()
	{
		return &ConnectUserList_;
	}
private:
	int ServerID_;
	string ServerName_;
	short ServiceWeight_;
	CUserHandlerList<CRoomGateUserInfo> RoomGateUserHandlerList_;
	CUserHandlerList<CConnectUserInfo> ConnectUserList_;
};
#endif