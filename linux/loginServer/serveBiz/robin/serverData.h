
#ifndef __SERVER_DATA_H__
#define __SERVER_DATA_H__

#include <string>

using namespace std;


class CServerData
{
public:
	CServerData(string sn, int id, int socket_id, int weight, string ip, uint32 port)
	{
		RoomGateName_ = sn;
		RoomGateID_ = id;
		SocketID_ = socket_id;
		Weight_ = weight;
		IpAddr_ = ip;
		Port_ = port;
	}

	~CServerData()
	{

	}

	inline int Open(string sn, int weight, string ip, int port)
	{
		RoomGateName_ = sn;
		Weight_ = weight;
		IpAddr_ = ip;
		Port_ = port;

		return 0;
	}

	inline const int GetWeight() const
	{
		return Weight_;
	}

	inline void SetWeight(int weight)
	{
		Weight_ = weight;
	}

	inline string GetName()
	{
		return RoomGateName_;
	}

	inline uint32 GetID()
	{
		return RoomGateID_;
	}

	inline int GetSocketID()
	{
		return SocketID_;
	}

	inline string GetIpAddr()
	{
		return IpAddr_;
	}

	inline uint32 GetPort()
	{
		return Port_;
	}

	inline const bool operator < (const CServerData& obj) const
	{
		return (Weight_ < obj.GetWeight());  
	}

private:
	string RoomGateName_;
	int RoomGateID_;
	int SocketID_;
	int Weight_;
	string IpAddr_;
	uint32 Port_;
};
#endif