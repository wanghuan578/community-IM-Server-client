#ifndef SOCKETLITE_SYNC_SPIN_MUTEX_H
#define SOCKETLITE_SYNC_SPIN_MUTEX_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif
#include "SL_Sync_Macros.h"

#ifdef SOCKETLITE_OS_WINDOWS
class SL_Sync_SpinMutex
{
public:
    inline SL_Sync_SpinMutex()
        : spin_mutex_(0)
    {         
    }

    inline ~SL_Sync_SpinMutex()
    {
    }

    inline void lock()
    {
        while (::InterlockedCompareExchange(&spin_mutex_, 1, 0) != 0) 
        {
            ::Sleep(0);
        }
    }

    inline void unlock()
    { 
        ::InterlockedExchange(&spin_mutex_, 0);
    }

    inline bool trylock()
    {
        return (::InterlockedCompareExchange(&spin_mutex_, 1, 0) == 0);
    }

    inline long handle()
    {
        return spin_mutex_;
    }

private:
    SL_Sync_SpinMutex(const SL_Sync_SpinMutex &);
    SL_Sync_SpinMutex& operator=(const SL_Sync_SpinMutex &);

    long spin_mutex_;
};
#else
class SL_Sync_SpinMutex
{
public:
    inline SL_Sync_SpinMutex(int pshared=0)
    { 
        pthread_spin_init(&spin_mutex_, pshared);
    }

    inline ~SL_Sync_SpinMutex()
    { 
        pthread_spin_destroy(&spin_mutex_);
    }

    inline void lock()
    { 
        pthread_spin_lock(&spin_mutex_);
    }

    inline void unlock()
    { 
        pthread_spin_unlock(&spin_mutex_);
    }

    inline bool trylock()
    {
        return (pthread_spin_trylock(&spin_mutex_) == 0);
    }

    inline pthread_spinlock_t* handle()
    {
        return &spin_mutex_;
    }

private:
    SL_Sync_SpinMutex(const SL_Sync_SpinMutex &);
    SL_Sync_SpinMutex& operator=(const SL_Sync_SpinMutex &);

    pthread_spinlock_t spin_mutex_;
};
#endif

#endif

