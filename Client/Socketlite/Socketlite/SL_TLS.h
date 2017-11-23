//TLS:thread local storage

#ifndef SL_TLS_H
#define SL_TLS_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif
#include "SL_Config.h"

#ifdef SOCKETLITE_OS_WINDOWS
#define SL_TLS_OUT_OF_INDEXES TLS_OUT_OF_INDEXES 

template <typename T>
class SL_TLS
{
public:
    inline SL_TLS()
        : tls_key_(SL_TLS_OUT_OF_INDEXES)
    {
        init();
    }

    inline ~SL_TLS()
    {
        clear();
    }

    inline int init()
    {
        if (tls_key_ == SL_TLS_OUT_OF_INDEXES)
        {
            tls_key_ = TlsAlloc();
            if (tls_key_ == SL_TLS_OUT_OF_INDEXES)
            {
                return -1;
            }
        }
        return 0;
    }

    inline int clear()
    {
        if (tls_key_ != SL_TLS_OUT_OF_INDEXES)
        {
            TlsFree(tls_key_);
            tls_key_ = SL_TLS_OUT_OF_INDEXES;
        }
        return 0;
    }

    // Get the value.
    inline T* get() const
    {
        return (T*)(TlsGetValue(tls_key_));
    }

    inline T* operator->() const
    {
        return get();
    }

    inline operator T*() const
    {
        return get();
    }

    // Set the value.
    inline void set(T *value)
    {
        TlsSetValue(tls_key_, value);
    }

    inline void operator= (T *value)
    {
        set(value);
    }

private:
    DWORD   tls_key_;
};

#else

#include <pthread.h>
#define SL_TLS_OUT_OF_INDEXES -1 

template <typename T>
class SL_TLS
{    
public:
    inline SL_TLS()
        : tls_key_(SL_TLS_OUT_OF_INDEXES)
    {
        init();
    }

    inline ~SL_TLS()
    {
        clear();
    }

    inline int init()
    {
        if (tls_key_ == SL_TLS_OUT_OF_INDEXES)
        {
            int rc = pthread_key_create(&tls_key_, 0);
            if (rc != 0)
            {
                tls_key_ = SL_TLS_OUT_OF_INDEXES;
                return -1;
            }
        }
        return 0;
    }

    inline int clear()
    {
        if (tls_key_ != SL_TLS_OUT_OF_INDEXES)
        {
            pthread_key_delete(tls_key_);
            tls_key_ = SL_TLS_OUT_OF_INDEXES;
        }
        return 0;
    }

    // Get the value.
    inline T* get() const
    {
        return (T*)(pthread_getspecific(tls_key_));
    }

    inline T* operator->() const
    {
        return get();
    }

    inline operator T*() const
    {
        return get();
    }

    // Set the value.
    inline void set(T *value)
    {
        pthread_setspecific(tls_key_, value);
    }

    inline void operator= (T *value)
    {
        set(value);
    }

private:
    pthread_key_t   tls_key_;
};

#endif  //SOCKETLITE_OS_WINDOWS

#endif  //SL_TLS_H

