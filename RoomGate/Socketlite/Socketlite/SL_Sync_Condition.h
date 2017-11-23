#ifndef SOCKETLITE_SYNC_CONDITION_H
#define SOCKETLITE_SYNC_CONDITION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif
#include "SL_Config.h"
#include "SL_Sync_Mutex.h"
#include "SL_Sync_Macros.h"

#if defined(SOCKETLITE_OS_WINDOWS)
class SL_Sync_Condition
{
public:
    inline SL_Sync_Condition()
    {
        condition_ = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    inline ~SL_Sync_Condition()
    {
        CloseHandle(condition_);
    }

    inline int signal()
    {
        if (SetEvent(condition_))
        {
            return 0;
        }
        return -1;
    }

	inline int reset()
	{
		if (ResetEvent(condition_))
		{
			return 0;
		}
		return -1;
	}

    inline int broadcast()
    {
        return signal();
    }

    inline int wait(SL_Sync_ThreadMutex *mutex)
    {
        if (NULL == mutex)
        {
            DWORD ret = WaitForSingleObject(condition_, INFINITE);
            if (ret == WAIT_OBJECT_0)
            {
                return 0;
            }
        }
        else
        {
            mutex->unlock();
            DWORD ret = WaitForSingleObject(condition_, INFINITE);
            if (ret == WAIT_OBJECT_0)
            {
                mutex->lock();
                return 0;
            }
            mutex->lock();
        }
        return -1;
    }

    inline int timed_wait(SL_Sync_ThreadMutex *mutex, int timeout_us) 
    {
        if (NULL == mutex)
        {
            DWORD ret;
            if (timeout_us > 1999)
            {
                ret = WaitForSingleObject(condition_, timeout_us/1000);
            }
            else if (timeout_us > 0)
            {
                ret = WaitForSingleObject(condition_, 1);
            }
            else if (timeout_us == 0)
            {
                return -1;
            }
            else
            {
                ret = WaitForSingleObject(condition_, INFINITE);
            }

            if (ret == WAIT_TIMEOUT)
            {
                return -1;
            }
            else if (ret == WAIT_OBJECT_0)
            {
                return 0;
            }
        }
        else
        {
            mutex->unlock();
            DWORD ret;
            if (timeout_us > 1999)
            {
                ret = WaitForSingleObject(condition_, timeout_us/1000);
            }
            else if (timeout_us > 0)
            {
                ret = WaitForSingleObject(condition_, 1);
            }
            else if (timeout_us == 0)
            {
                return -1;
            }
            else
            {
                ret = WaitForSingleObject(condition_, INFINITE);
            }

            if (ret == WAIT_TIMEOUT)
            {
                mutex->lock();
                return -1;
            }
            else if (ret == WAIT_OBJECT_0)
            {
                mutex->lock();
                return 0;
            }
            mutex->lock();
        }
        return -2;
	}

    inline SL_SYNC_CONDTIION_T* handle()
    {
        return &condition_;
    }

private:
    //= Prevent assignment and initialization.
    // Disallow copying and assignment.
    SL_Sync_Condition(const SL_Sync_Condition &);
    SL_Sync_Condition& operator= (const SL_Sync_Condition &);

private:
    SL_SYNC_CONDTIION_T condition_;
};

#elif defined(SOCKETLITE_HAVE_PTHREADS)
#include <sys/time.h>
class SL_Sync_Condition
{
public:
    inline SL_Sync_Condition()
    {
        pthread_cond_init(&condition_, 0);
    }

    inline ~SL_Sync_Condition()
    {
        pthread_cond_destroy(&condition_);
    }

    inline int signal()
    {
        int ret = pthread_cond_signal(&condition_);
        if (ret == 0)
        {
            return 0;
        }
        return -1;
    }

    inline int broadcast()
    {
        int ret = pthread_cond_broadcast(&condition_);
        if (ret == 0)
        {
            return 0;
        }
        return -1;
    }

    inline int wait(SL_Sync_ThreadMutex *mutex)
    {
        if (NULL != mutex)
        {
            int ret = pthread_cond_wait(&condition_, mutex->handle());
            if (ret == 0)
            {
                return 0;
            }
        }
        return -1;
    }

    inline int timed_wait(SL_Sync_ThreadMutex *mutex, int timeout_us) 
    {        
        if (NULL != mutex)
        {
            int ret = -1;
            if (timeout_us > 0)
            {
                timeval nowtime;
                gettimeofday(&nowtime, NULL);
                timespec abstime;
                abstime.tv_sec  = nowtime.tv_sec + timeout_us / 1000000L;
                abstime.tv_nsec = (nowtime.tv_usec + (timeout_us % 1000000L)) * 1000;
                if (abstime.tv_nsec >= 1000000000L)
                {
                    ++abstime.tv_sec;
                    abstime.tv_nsec -= 1000000000L;
                }
                ret = pthread_cond_timedwait(&condition_, mutex->handle(), &abstime);
            }
            else if (timeout_us == 0)
            {
                return -1;
            }
            else
            {
                ret = pthread_cond_wait(&condition_, mutex->handle());
            }

            if (ret == 0)
            {
                return 0;
            }
            else if (ret == ETIMEDOUT)
            {
                return -1;
            }
        }
        return -2;
    }

    inline SL_SYNC_CONDTIION_T* handle()
    {
        return &condition_;
    }

private:
    //= Prevent assignment and initialization.
    // Disallow copying and assignment.
    SL_Sync_Condition(const SL_Sync_Condition &);
    SL_Sync_Condition& operator= (const SL_Sync_Condition &);

private:
    SL_SYNC_CONDTIION_T condition_;
};

#endif

#endif

