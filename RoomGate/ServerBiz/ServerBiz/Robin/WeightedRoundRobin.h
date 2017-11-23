
#pragma once

#include "ScheduledServerData.h"
#include <list>
using namespace std;


class CWeightedRoundRobin
{
public:
	CWeightedRoundRobin();
	~CWeightedRoundRobin();

	void Add(CScheduledServerData* info);
	void Sort();
	int GetMaxWeight();
	int GetWeight(int index);
	CScheduledServerData *GetServerData(int index);
	CScheduledServerData *ServiceRequest();

private:
	list<CScheduledServerData*> ServerScheduledDataList_;
	int i;
	int j;
	int cw;
};