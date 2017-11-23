
#ifndef __ROOM_SERVER_MANAGER_H__
#define __ROOM_SERVER_MANAGER_H__

#include "SL_Sync_Mutex.h"
#include "SL_Socket_Select_Runner.h"
#include "ClientControlBaseAdaptor.h"
#include "RoomServerBridge.h"

class CRoomServerManager
{
public:
	CRoomServerManager()
	{
		BridgeObjectPool_.init(50, 20, 2);		
	}

	~CRoomServerManager()
	{
		Destroy();
	}

	inline void Destroy()
	{
		for (unsigned int i = 0; i < RoomServerActiveList_.size(); i++)
		{
			RoomServerActiveList_.at(i)->Destroy();
		}

		RoomServerActiveList_.clear();
	}

	inline bool Open(int room_id, string room_name, string ip, uint16 port)
	{
		Bridge_ = BridgeObjectPool_.alloc_object();
		
		Bridge_->SetServerID(room_id);

		Bridge_->SetServerName(room_name);

		Bridge_->Open(ip, port);

		RoomServerInActiveList_.push_back(Bridge_);

		return 0;
	}

	inline int RoomServerUpdate(int room_id, int socket_id)
	{
		CRoomServerBridge *Bridge_ = NULL;

		for (unsigned int i = 0; i < RoomServerInActiveList_.size(); i++)
		{
			Bridge_ = RoomServerInActiveList_.at(i);

			if (room_id == Bridge_->GetServerID())
			{
				Bridge_->SetSocketID(socket_id);

				RoomServerActiveList_.push_back(Bridge_);

				RoomServerInActiveList_.erase(RoomServerInActiveList_.begin() + i);
			}
		}

		if (RoomServerInActiveList_.size() > 0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	inline void RoomServerUpdate(int socket_id)
	{
		CRoomServerBridge *Bridge_ = NULL;

		for (unsigned int i = 0; i < RoomServerActiveList_.size(); i++)
		{
			Bridge_ = RoomServerActiveList_.at(i);

			if (socket_id == Bridge_->GetSocketID())
			{
				RoomServerActiveList_.erase(RoomServerActiveList_.begin() + i);
			}
		}
	}

	inline CRoomServerBridge *GetRoomServerByID(int room_id)
	{
		CRoomServerBridge *Bridge_ = NULL;

		for (unsigned int i = 0; i < RoomServerActiveList_.size(); i++)
		{
			Bridge_ = RoomServerActiveList_.at(i);

			if (room_id == Bridge_->GetServerID())
			{
				return Bridge_;
			}
		}

		return Bridge_;
	}

	inline CRoomServerBridge *GetBridge()
	{
		return Bridge_;
	}

private:
	CRoomServerBridge *Bridge_;
	SL_ObjectPool_SimpleEx<CRoomServerBridge, SL_Sync_ThreadMutex> BridgeObjectPool_;
	vector<CRoomServerBridge*> RoomServerInActiveList_;
	vector<CRoomServerBridge*> RoomServerActiveList_;
};
#endif