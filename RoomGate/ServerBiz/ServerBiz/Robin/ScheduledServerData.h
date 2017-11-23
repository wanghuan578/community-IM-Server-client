
#pragma once

#include <string>

using namespace std;


class CScheduledServerData
{
public:
	CScheduledServerData(string sn, int weight, string ip, int port);
	~CScheduledServerData();

	int Open(string sn, int weight, string ip, int port);
	const int GetWeight() const;
	void SetWeight(int weight);
	string GetName();
	string GetIpAddr();
	int GetPort();
	const bool operator < (const CScheduledServerData& obj) const;

private:
	string ServerName_;
	int Weight_;
	string IpAddr_;
	int Port_;
};