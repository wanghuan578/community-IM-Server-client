#ifndef SOCKETLITE_THREAD_H
#define SOCKETLITE_THREAD_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
	#pragma once
#endif
#include "SL_Sync_Mutex.h"
#include "SL_Sync_Guard.h"
#include "SL_Sync_Atomic.h"

#if defined(SOCKETLITE_OS_WINDOWS)
#include <process.h>
typedef uint    SL_THREAD_ID;
typedef HANDLE  SL_THREAD_HANDLE;
typedef unsigned int (__stdcall *SL_ThreadProc)(void *arg);

class SL_Thread
{
public:
	SL_Thread()
		: thread_id_(0)
		, thread_handle_(NULL)
		, started_(0)
		, running_(0)
	{
	}

	~SL_Thread()
	{   
		//wanghuan debug
        //clear();
	}

	inline int clear()
	{
		if (NULL != thread_handle_)
		{
            started_.store(0);
            running_.store(0);
		    this->join();
		    CloseHandle(thread_handle_);
            thread_handle_ = NULL;
		}
		return 0;
	}

	int start(SL_ThreadProc threadproc, void *arg)
	{
		if (started_.load() > 0)
        {
			return 1;
        }

        started_.store(1);
        running_.store(1);
		thread_handle_ = reinterpret_cast<SL_THREAD_HANDLE>(::_beginthreadex(NULL, 0, threadproc, arg, 0, &thread_id_));
		if (NULL == thread_handle_)
        {
		    started_.store(0);
		    running_.store(0);
			return -1;
        }
		return 0;
	}

	inline void stop()
	{
		running_.store(0);
	}

	inline void join()
	{
		if (NULL != thread_handle_)
		{
			WaitForSingleObject(thread_handle_, INFINITE);
		}
	}

	inline int cancel()
	{
		if (NULL != thread_handle_)
		{
			int ret = TerminateThread(thread_handle_,0) ? 0:-1;
			return ret;
		}
		return 0;
	}

	inline int kill()
	{
		return this->cancel();
	}

    inline int exit(unsigned int exit_code=0)
    {
        started_.store(0);
        running_.store(0);
		return 0;
    }

	inline bool get_running()
	{
		if (running_.load() > 0)
        {
            return true;
        }
        return false;
	}

	inline uint thread_id() const
	{
		return thread_id_;
	}

	inline SL_THREAD_HANDLE* handle()
	{
		return &thread_handle_;
	}

	static void self(SL_THREAD_HANDLE &handle)
	{
		handle = GetCurrentThread();
	}

	static SL_THREAD_ID self()
	{
		return GetCurrentThreadId();
	}

	static void sleep(uint timeout_ms)
	{//timeout: ms
		Sleep(timeout_ms);
	}

protected:
	SL_THREAD_ID		    thread_id_;
	SL_THREAD_HANDLE        thread_handle_;
	SL_Sync_Atomic_Int32    started_;
	SL_Sync_Atomic_Int32    running_;
};

#elif defined(SOCKETLITE_HAVE_PTHREADS)
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
typedef pthread_t   SL_THREAD_ID;
typedef pthread_t   SL_THREAD_HANDLE;
typedef void* (*SL_ThreadProc)(void *arg);

class SL_Thread
{
public:

	SL_Thread()
		: thread_id_(0)
		, thread_handle_(0)
		, started_(0)
		, running_(0)
	{
	}

	~SL_Thread()
	{
        clear();
	}

	inline int clear()
	{
		if (started_.load() > 0)
		{
            started_.store(0);
            running_.store(0);
            this->join();
            thread_handle_ = 0;
		}
		return 0;
	}

	int start(SL_ThreadProc threadproc, void *arg)
	{
		if (started_.load() > 0)
        {
			return 1;
        }

        started_.store(1);
        running_.store(1);
		int ret  = pthread_create(&thread_handle_, NULL, threadproc, arg);
		if (ret != 0)
        {
            started_.store(0);
            running_.store(0);
			return -1;
        }
		return 0;
	}

	inline void stop()
	{
		running_.store(0);
	}
	inline void join()
	{
        if (0 != thread_handle_)
        {
		    pthread_join(thread_handle_, NULL);
            thread_handle_ = 0;
        }
	}

	inline int cancel()
	{
        if (0 != thread_handle_)
        {
			int ret = pthread_cancel(thread_handle_);
			return ret;
		}
		return 0;
	}

	inline int kill()
	{
        if (0 != thread_handle_)
        {
			int ret = pthread_kill(thread_handle_, 0);
			return ret;
		}
		return 0;
	}

    inline int exit(unsigned int exit_code=0)
    {
        //pthread_exit(NULL);
        started_.store(0);
        running_.store(0);
        return 0;
    }

    inline uint thread_id() const
    {
        return thread_id_;
    }

	inline bool get_running()
	{
        if (running_.load() > 0)
        {
            return true;
        }
        return false;
	}

	inline SL_THREAD_HANDLE* handle()
	{
		return &thread_handle_;
	}

	static void self(SL_THREAD_HANDLE &handle)
	{
		handle = pthread_self();
	}

	static SL_THREAD_ID self()
	{
		return pthread_self();
	}

	static void sleep(uint timeout_ms)
	{//timeout: ms

        //方法1
        //usleep(timeout_ms * 1000);

        //方法2
        struct timespec ts;
        ts.tv_sec  = timeout_ms / 1000;
        ts.tv_nsec = (timeout_ms - ts.tv_sec * 1000) * 1000000L;
        nanosleep(&ts, NULL);
	}

protected:
	SL_THREAD_ID		    thread_id_;
	SL_THREAD_HANDLE	    thread_handle_;
    SL_Sync_Atomic_Int32    started_;
    SL_Sync_Atomic_Int32    running_;
};

#endif  //SOCKETLITE_OS_WINDOWS

#endif  //SOCKETLITE_THREAD_H

