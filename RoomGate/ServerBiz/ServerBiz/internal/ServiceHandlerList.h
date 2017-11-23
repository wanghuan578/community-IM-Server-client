
#ifndef __SERVICE_HANDLER_LIST_H__
#define __SERVICE_HANDLER_LIST_H__

#include "SL_ObjectPool.h"
#include "SL_Sync_Mutex.h"
#include "SL_Socket_Handler.h"
#include <string>

using namespace std;


template<typename TObject>
class CServiceHandlerList
{
public:
	CServiceHandlerList()
	{
	}

	~CServiceHandlerList()
	{
	}

	inline int Open(unsigned int max_chunk_size = 10, unsigned int init_chunk_size = 5, unsigned int per_chunk_size = 1)
	{
		return ObjectPool_.init(max_chunk_size, init_chunk_size, per_chunk_size);
	}

	inline void Close()
	{
		ObjectPool_.clear();
	}

	inline int Size()
	{
		return HandlerList_.size();
	}

	inline void Append(TObject &t_obj)
	{
		Mutex_.lock();

		TObject *obj = ObjectPool_.alloc_object();

		if (NULL != obj)
		{
			obj->user_id = t_obj.user_id;

			obj->user_name = t_obj.user_name;

			obj->socket_handler_id = t_obj.socket_handler_id;

			obj->socket_handler = t_obj.socket_handler;

			obj->log_time = t_obj.log_time;

			HandlerList_.push_back(obj);
		}

		Mutex_.unlock();
	}

	inline void Append(uint32 service_id, string service_name, const SL_Socket_Handler *socket_handler, string ip, uint16 port)
	{
		Mutex_.lock();

		TObject *obj = ObjectPool_.alloc_object();

		if (NULL != obj)
		{
			obj->service_id = service_id;

			obj->service_name = service_name;

			obj->ip = ip;

			obj->port = port;

			obj->socket_handler = socket_handler;

			HandlerList_.push_back(obj);
		}
		
		Mutex_.unlock();
	}

	inline int Remove(uint32 user_id)
	{
		Mutex_.lock();

		int ret = -1;

		int offset;

		TObject *obj = Find(user_id, offset);

		if (NULL != obj)
		{
			ObjectPool_.free_object(obj);

			HandlerList_.erase(HandlerList_.begin() + offset);

			ret = 0;
		}

		Mutex_.unlock();

		return ret;
	}

	inline int Remove(const SL_Socket_Handler *socket_handler)
	{
		Mutex_.lock();

		int ret = -1;

		int offset;

		TObject *obj = Find(socket_handler, offset);

		if (NULL != obj)
		{
			ObjectPool_.free_object(obj);

			HandlerList_.erase(HandlerList_.begin() + offset);

			ret = 0;
		}

		Mutex_.unlock();

		return ret;
	}

	inline TObject *Find(uint32 service_id, int &offset)
	{
		Mutex_.lock();

		TObject *obj = NULL;

		for (unsigned int i = 0; i < HandlerList_.size(); i++)
		{
			obj = HandlerList_.at(i);

			if (obj->service_id == service_id)
			{
				offset = i;

				Mutex_.unlock();

				return obj;
			}
		}

		offset = -1;

		Mutex_.unlock();

		return NULL;
	}

	inline TObject *Find(const SL_Socket_Handler *socket_handler, int &offset)
	{
		Mutex_.lock();

		TObject *obj = NULL;

		for (unsigned int i = 0; i < HandlerList_.size(); i++)
		{
			obj = HandlerList_.at(i);

			if (obj->socket_handler == socket_handler)
			{
				offset = i;

				Mutex_.unlock();

				return obj;
			}
		}

		offset = -1;

		Mutex_.unlock();

		return NULL;
	}

	std::vector<TObject*> *GetList()
	{
		return &HandlerList_;
	}

private:
	SL_Sync_ThreadMutex Mutex_;
	SL_ObjectPool_SimpleEx<TObject, SL_Sync_ThreadMutex> ObjectPool_;
	std::vector<TObject*> HandlerList_;
};
#endif