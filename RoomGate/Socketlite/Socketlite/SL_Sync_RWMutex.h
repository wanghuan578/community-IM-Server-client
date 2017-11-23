#ifndef SOCKETLITE_SYNC_RWMUTEX_H
#define SOCKETLITE_SYNC_RWMUTEX_H
#include "SL_Config.h"

#ifdef SOCKETLITE_OS_WINDOWS

//
// Multi-reader/Single-writer concurrency base class for Win32
//
// (c) 1999-2003 by Glenn Slayden (glenn@glennslayden.com)
// http://www.glennslayden.com/code/win32/reader-writer-lock
//
class SL_Sync_RWMutex
{
public:
    inline SL_Sync_RWMutex()
    {
        read_refcount_ = 0;
        InitializeCriticalSection(&write_mutex_);
        InitializeCriticalSection(&read_mutex_);
        read_clear_event_ = CreateEvent(NULL, TRUE, TRUE, NULL);
    }
    
    inline ~SL_Sync_RWMutex()
    {
        WaitForSingleObject(read_clear_event_, INFINITE);
        CloseHandle(read_clear_event_);
        DeleteCriticalSection(&write_mutex_);
        DeleteCriticalSection(&read_mutex_);
    }

    inline void read_lock()
    {
        EnterCriticalSection(&write_mutex_);
        EnterCriticalSection(&read_mutex_);
        if (++read_refcount_ == 1)
        {
            ResetEvent(read_clear_event_);
        }
        LeaveCriticalSection(&read_mutex_);
        LeaveCriticalSection(&write_mutex_);
    }

    inline bool read_trylock()
    {
        return false;
    }

    inline void read_unlock()
    {
        EnterCriticalSection(&read_mutex_);
        if (--read_refcount_ == 0)
        {
            SetEvent(read_clear_event_);
        }
        LeaveCriticalSection(&read_mutex_);
    }

    inline void write_lock()
    {
        EnterCriticalSection(&write_mutex_);
        WaitForSingleObject(read_clear_event_, INFINITE);
    }

    inline bool write_trylock()
    {
        return false;
    }

    inline void write_unlock()
    {
        LeaveCriticalSection(&write_mutex_);
    }

    inline void unlock()
    {
        if (::InterlockedCompareExchange(&read_refcount_, 0, 0) == 0)
        {
            LeaveCriticalSection(&write_mutex_);
        }
        else
        {
            EnterCriticalSection(&read_mutex_);
            if (--read_refcount_ == 0)
            {
                SetEvent(read_clear_event_);
            }
            LeaveCriticalSection(&read_mutex_);
        }
    }    
    
private:
    SL_Sync_RWMutex(const SL_Sync_RWMutex &);
    SL_Sync_RWMutex& operator=(const SL_Sync_RWMutex &);

private:
    CRITICAL_SECTION    write_mutex_;
    CRITICAL_SECTION    read_mutex_;
    HANDLE              read_clear_event_;
    long                read_refcount_;
};
#else
class SL_Sync_RWMutex
{
public:    
    inline SL_Sync_RWMutex()
    {
        pthread_rwlock_init(&rwmutex_, NULL);
    }
    
    inline ~SL_Sync_RWMutex()
    {
        pthread_rwlock_destroy(&rwmutex_);
    }

    inline void read_lock()
    {
        pthread_rwlock_rdlock(&rwmutex_);
    }

    inline bool read_trylock()
    {
        return pthread_rwlock_tryrdlock(&rwmutex_);
    }

    inline void read_unlock()
    {
        pthread_rwlock_unlock(&rwmutex_);
    }

    inline void write_lock()
    {
        pthread_rwlock_wrlock(&rwmutex_);
    }

    inline bool write_trylock()
    {
        return pthread_rwlock_trywrlock(&rwmutex_);
    }

    inline void write_unlock()
    {
        pthread_rwlock_unlock(&rwmutex_);
    }

    inline void unlock()
    {
        pthread_rwlock_unlock(&rwmutex_);
    }
        
private:
    SL_Sync_RWMutex(const SL_Sync_RWMutex &);
    SL_Sync_RWMutex& operator=(const SL_Sync_RWMutex &);

private:
    pthread_rwlock_t rwmutex_;
};

#endif

