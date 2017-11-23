
#ifndef __WEIGHTED_ROUND_ALGORITHM_H__
#define __WEIGHTED_ROUND_ALGORITHM_H__

#include "ServerData.h"
#include <vector>

using namespace std;


template <typename TDataType>
class CWeightedRound
{
public:
	CWeightedRound()
		:Index_(-1),
		CW_(0)
	{
	}

	~CWeightedRound()
	{
		TDataType *obj = NULL;

		for (unsigned int i = 0; i < DataList_.size(); i++)
		{
			obj = DataList_.at(i);

			delete obj;

			DataList_.erase(DataList_.begin() + i);
		}
	}

	inline void Append(TDataType *data)
	{
		Mutex_.lock();

		DataList_.push_back(data);

		Mutex_.unlock();
	}

	inline void Remove(TDataType *data)
	{
		Mutex_.lock();

		DataList_.remove(data);

		delete data;

		Mutex_.unlock();
	}

	inline int RemoveByFD(int socket_id)
	{
		Mutex_.lock();

		int ret = -1;

		TDataType *obj = NULL;

		for (unsigned int i = 0; i < DataList_.size(); i++)
		{
			obj = DataList_.at(i);

			if (socket_id == obj->GetSocketID())
			{
				DataList_.erase(DataList_.begin() + i);

				ret = 0;

				break;
			}
		}

		Mutex_.unlock();

		return ret;
	}

	inline int RemoveByID(int roomgate_id)
	{
		Mutex_.lock();

		int ret = -1;

		TDataType *obj = NULL;

		for (unsigned int i = 0; i < DataList_.size(); i++)
		{
			obj = DataList_.at(i);

			if (roomgate_id == obj->GetID())
			{
				DataList_.erase(DataList_.begin() + i);

				ret = 0;

				break;
			}
		}

		Mutex_.unlock();

		return ret;
	}

	inline TDataType *Find(int id)
	{
		//Mutex_.lock();

		//list<TDataType*>::iterator iter;

		//for (iter = DataList_.begin(); iter != DataList_.end(); iter++)
		//{
		//	if (id == (*iter)->GetID())
		//	{
		//		return *iter;
		//	}
		//}

		//return NULL;

		//Mutex_.unlock();

		Mutex_.lock();

		TDataType *obj = NULL;

		for (unsigned int i = 0; i < DataList_.size(); i++)
		{
			obj = DataList_.at(i);

			if (id == obj->GetID())
			{
				Mutex_.unlock();

				return obj;
			}
		}

		Mutex_.unlock();

		return NULL;
	}

	inline TDataType *ServiceRequest()
	{
		Mutex_.lock();

		if (0 < DataList_.size())
		{
			while(1) 
			{
				Index_ = (Index_ + 1) % DataList_.size();

				if (Index_ == 0) 
				{
					CW_ = CW_ - 1;

					if (CW_ <= 0) 
					{
						CW_ = GetMaxWeight();

						if (CW_ == 0)
						{
							Mutex_.unlock();

							return NULL;
						}
					}
				}

				if (GetWeight(Index_) >= CW_)
				{
					Mutex_.unlock();

					return GetServerData(Index_);
				}
			}
		}
		
		Mutex_.unlock();

		return NULL;
	}

	inline int Size()
	{
		return DataList_.size();
	}

protected:
	inline int GetMaxWeight()
	{
		//int weight = 0;

		//list<TDataType*>::iterator iter;

		//for (iter = DataList_.begin(); iter != DataList_.end(); iter++)
		//{
		//	if (weight < (*iter)->GetWeight())
		//	{
		//		weight = (*iter)->GetWeight();
		//	}
		//}

		//return weight;

		Mutex_.lock();

		int weight = 0;

		TDataType *obj = NULL;

		for (unsigned int i = 0; i < DataList_.size(); i++)
		{
			obj = DataList_.at(i);

			if (weight < obj->GetWeight())
			{
				weight = obj->GetWeight();
			}
		}

		Mutex_.unlock();

		return weight;
	}

	inline int GetWeight(int index)
	{
		//int i = 0;

		//list<TDataType*>::iterator iter;

		//for (iter = DataList_.begin(); iter != DataList_.end(); iter++)
		//{
		//	if (index == i++)
		//	{
		//		return (*iter)->GetWeight();
		//	}
		//}

		//return 0;

		Mutex_.lock();

		TDataType *obj = NULL;

		for (unsigned int i = 0; i < DataList_.size(); i++)
		{
			obj = DataList_.at(i);

			if (index == i++)
			{
				Mutex_.unlock();

				return obj->GetWeight();
			}
		}

		Mutex_.unlock();

		return 0;
	}

	inline TDataType *GetServerData(int index)
	{
		//int i = 0;

		//list<TDataType*>::iterator iter;

		//for (iter = DataList_.begin(); iter != DataList_.end(); iter++)
		//{
		//	if (index == i++)
		//	{
		//		return (*iter);
		//	}
		//}

		Mutex_.lock();

		TDataType *obj = NULL;

		for (unsigned int i = 0; i < DataList_.size(); i++)
		{
			obj = DataList_.at(i);

			if (index == i++)
			{
				Mutex_.unlock();

				return obj;
			}
		}

		Mutex_.unlock();

		return NULL;
	}

private:
	SL_Sync_ThreadMutex Mutex_;
	vector<TDataType*> DataList_;
	int Index_;
	int CW_;
};
#endif