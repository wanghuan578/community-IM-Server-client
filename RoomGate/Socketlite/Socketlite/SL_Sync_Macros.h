#ifndef SOCKETLITE_SYNC_MACROS_H
#define SOCKETLITE_SYNC_MACROS_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif
#include "SL_Config.h"

#if defined(SOCKETLITE_OS_WINDOWS)
    typedef CRITICAL_SECTION                SL_SYNC_MUTEX_T;
    typedef SL_HANDLE                       SL_SYNC_CONDTIION_T;
    #define SL_SYNC_THREADMUTEX(x)          CRITICAL_SECTION(x)
    #define SL_SYNC_THREADMUTEX_INIT(x)     InitializeCriticalSection(x)
    #define SL_SYNC_THREADMUTEX_DESTROY(x)  DeleteCriticalSection(x)
    #define SL_SYNC_THREADMUTEX_LOCK(x)     EnterCriticalSection(x)
    #define SL_SYNC_THREADMUTEX_UNLOCK(x)   LeaveCriticalSection(x)
    #define SL_SYNC_THREADMUTEX_TRYLOCK(x)  TryEnterCriticalSection(x)
    #define SL_SYNC_THREADMUTEX_TRYLOCK_RETURN(x)       \
        do {                                            \
            BOOL result = TryEnterCriticalSection(x);   \
            if (FALSE == result)                        \
                return false;                           \
            else                                        \
                return true;                            \
        } while (0)

#elif defined(SOCKETLITE_HAVE_PTHREADS)
    typedef pthread_mutex_t                 SL_SYNC_MUTEX_T;
    typedef pthread_cond_t                  SL_SYNC_CONDTIION_T;
    #define SL_SYNC_THREADMUTEX(x)          pthread_mutex_t(x)
    #define SL_SYNC_THREADMUTEX_INIT(x)     pthread_mutex_init(x, 0)
    #define SL_SYNC_THREADMUTEX_DESTROY(x)  pthread_mutex_destroy(x)
    #define SL_SYNC_THREADMUTEX_LOCK(x)     pthread_mutex_lock(x)
    #define SL_SYNC_THREADMUTEX_UNLOCK(x)   pthread_mutex_unlock(x)
    #define SL_SYNC_THREADMUTEX_TRYLOCK(x)  pthread_mutex_trylock(x)
    #define SL_SYNC_THREADMUTEX_TRYLOCK_RETURN(x)       \
        do {                                            \
            int rc = pthread_mutex_trylock(x);          \
            return (rc == 0);                           \
        } while (0)

#endif

#endif

