
#ifndef __USER_HANDLER_LIST_H__
#define __USER_HANDLER_LIST_H__

#include "SL_ObjectPool.h"
#include "SL_Sync_Mutex.h"
#include "SL_Socket_Handler.h"
#include <string>

using namespace std;


template<typename TObject>
class CUserHandlerList
{
public:
	CUserHandlerList()
	{
	}

	~CUserHandlerList()
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

			obj->client_random = t_obj.client_random;

			obj->server_random = t_obj.server_random;

			obj->check_sum = t_obj.check_sum;

			obj->session_id = t_obj.session_id;

			obj->session_key = t_obj.session_key;

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

	inline TObject *Find(uint32 user_id, int &offset)
	{
		Mutex_.lock();

		TObject *obj = NULL;

		for (unsigned int i = 0; i < HandlerList_.size(); i++)
		{
			obj = HandlerList_.at(i);

			if (obj->user_id == user_id)
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

	inline TObject *FindByID(int user_id)
	{
		Mutex_.lock();

		TObject *obj = NULL;

		for (unsigned int i = 0; i < HandlerList_.size(); i++)
		{
			obj = HandlerList_.at(i);

			if (obj->user_id == user_id)
			{
				Mutex_.unlock();

				return obj;
			}
		}

		Mutex_.unlock();

		return NULL;
	}

	inline TObject *FindByFD(int socket_id)
	{
		Mutex_.lock();

		TObject *obj = NULL;

		for (unsigned int i = 0; i < HandlerList_.size(); i++)
		{
			obj = HandlerList_.at(i);

			if (obj->socket_handler_id == socket_id)
			{
				Mutex_.unlock();

				return obj;
			}
		}

		Mutex_.unlock();

		return NULL;
	}

	inline TObject *FindByRandom(int server_random)
	{
		Mutex_.lock();

		TObject *obj = NULL;

		for (unsigned int i = 0; i < HandlerList_.size(); i++)
		{
			obj = HandlerList_.at(i);

			if (obj->server_random == server_random)
			{
				Mutex_.unlock();

				return obj;
			}
		}

		Mutex_.unlock();

		return NULL;
	}

	inline TObject *FindByHandler(const SL_Socket_Handler *socket_handler)
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