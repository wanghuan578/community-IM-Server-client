
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

	inline void Append(TObject *server_obj)
	{
		Mutex_.lock();

		TObject *obj = ObjectPool_.alloc_object();

		if (NULL != obj)
		{
			obj->service_id = server_obj->service_id;

			obj->service_name = server_obj->service_name;

			obj->ip = server_obj->ip;

			obj->port = server_obj->port;

			obj->socket_handler = server_obj->socket_handler;

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

	inline int Remove(SL_Socket_Handler *socket_handler)
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

	inline TObject *Find(int service_id)
	{
		Mutex_.lock();

		TObject *obj = NULL;

		for (unsigned int i = 0; i < HandlerList_.size(); i++)
		{
			obj = HandlerList_.at(i);

			if (obj->service_id == service_id)
			{
				Mutex_.unlock();

				return obj;
			}
		}

		Mutex_.unlock();

		return NULL;
	}

	inline TObject *Find(const SL_Socket_Handler *socket_handler)
	{
		Mutex_.lock();

		TObject *obj = NULL;

		for (unsigned int i = 0; i < HandlerList_.size(); i++)
		{
			obj = HandlerList_.at(i);

			if (obj->socket_handler == socket_handler)
			{
				Mutex_.unlock();

				return obj;
			}
		}

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