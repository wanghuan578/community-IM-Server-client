

#include "ScheduledServerData.h"

CScheduledServerData::CScheduledServerData(string sn, int weight, string ip, int port)
{
	ServerName_ = sn;
	Weight_ = weight;
	IpAddr_ = ip;
	Port_ = port;
}

CScheduledServerData::~CScheduledServerData()
{

}

int CScheduledServerData::Open(string sn, int weight, string ip, int port)
{
	ServerName_ = sn;
	Weight_ = weight;
	IpAddr_ = ip;
	Port_ = port;

	return 0;
}

const int CScheduledServerData::GetWeight() const
{
	return Weight_;
}

void CScheduledServerData::SetWeight(int weight)
{
	Weight_ = weight;
}

string CScheduledServerData::GetName()
{
	return ServerName_;
}

string CScheduledServerData::GetIpAddr()
{
	return IpAddr_;
}

int CScheduledServerData::GetPort()
{
	return Port_;
}

const bool CScheduledServerData::operator < (const CScheduledServerData& obj) const
{
	return (Weight_ < obj.GetWeight());  
}