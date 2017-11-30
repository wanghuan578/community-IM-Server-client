//**********************************************************************
//
// Copyright (C) 2005-2007 Zhang bao yuan(bolidezhang@gmail.com).
// All rights reserved.
//
// This copy of SocketLite is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#ifndef SOCKETLITE_OBJECT_POOL_H
#define SOCKETLITE_OBJECT_POOL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif
#include <list>
#include "SL_DataType.h"

template <typename T, typename TSyncMutex>
class SL_ObjectPool
{
public:
    inline SL_ObjectPool()
    {
    }

    inline ~SL_ObjectPool() 
    {
    }

    inline int init(unsigned int max_size=100000, unsigned int init_size=0, unsigned int grow_size=10)
    {
        return 0;
    }

    inline T* alloc_object()
    { 
        T *object = new T();
        return object;
    }

    inline void free_object(T *object)
    {
        delete object;
    }
};

template <typename T, typename TSyncMutex>
class SL_ObjectPool_Simple
{
private:
    SL_ObjectPool_Simple(const SL_ObjectPool_Simple &);
    void operator= (const SL_ObjectPool_Simple &);

public:
    inline SL_ObjectPool_Simple()
        : max_size_(100000)
        , grow_size_(10)
        , free_size_(0)
    {
    }

    inline ~SL_ObjectPool_Simple()
    {
        clear();
    }

    inline int clear()
    {
        mutex_.lock();
        typename std::list<T *>::iterator iter = free_objects_.begin();
        typename std::list<T *>::iterator iter_end = free_objects_.end();
        for (; iter != iter_end; ++iter)
        {
            delete *iter;
        }
        free_objects_.clear();
        free_size_ = 0;
        mutex_.unlock();
        return 0;
    }

    inline int init(unsigned int max_size=100000, unsigned int init_size=0, unsigned int grow_size=10)
    {
        clear();
        if (grow_size < 1)
        {
            grow_size = 1;
        }
        if (init_size>max_size)
        {
            init_size = max_size;
        }
        if (grow_size>max_size)
        {
            grow_size = max_size;
        }

        mutex_.lock();
        max_size_   = max_size;
        grow_size_  = grow_size;
        alloc_i(init_size);
        mutex_.unlock();
        return 0;
    }

    inline T* alloc_object()
    {
        T *object;
        mutex_.lock();
        if (!free_objects_.empty())
        {
            object = free_objects_.back();
            free_objects_.pop_back();
            --free_size_;
        }
        else
        {
            object = new T();
            if (NULL == object)
            {
                mutex_.unlock();
                return NULL;
            }
            alloc_i(grow_size_);
        }
        mutex_.unlock();
        return object;
    }

    inline void free_object(T *object)
    {
        mutex_.lock();
        if (free_size_ < max_size_)
        {
            free_objects_.push_back(object);
            ++free_size_;
        }
        else
        {
            delete object;
        }
        mutex_.unlock();
    }

private:
    inline bool alloc_i(unsigned int size)
    {
        T *object;
        for (unsigned int i=0; i<size; ++i)
        {            
            object = new T();
            if (NULL == object)
            {
                return false;
            }
            free_objects_.push_back(object);
            ++free_size_;
        }
        return true;
    }

private:
    unsigned int    max_size_;   //对象池最大容量
    unsigned int    grow_size_;  //增长幅度
    unsigned int    free_size_;  

    TSyncMutex      mutex_;
    std::list<T* >  free_objects_;
};

template <typename T, typename TSyncMutex>
class SL_ObjectPool_SimpleEx
{
private:
    SL_ObjectPool_SimpleEx(const SL_ObjectPool_SimpleEx &);
    void operator= (const SL_ObjectPool_SimpleEx &);

public:
    inline SL_ObjectPool_SimpleEx()
        : max_chunk_size_(10000)
        , per_chunk_size_(10)
        , current_chunk_size_(0)
    {
    }

    inline ~SL_ObjectPool_SimpleEx()
    {
        clear();
    }

    inline int clear()
    {
        mutex_.lock();
        if (current_chunk_size_ > 0)
        {
            typename std::list<T *>::iterator iter = object_chunks_.begin();
            typename std::list<T *>::iterator iter_end = object_chunks_.end();
            for (; iter != iter_end; ++iter)
            {
                delete [](*iter);
            }
            object_chunks_.clear();
        }        
        free_objects_.clear();
        current_chunk_size_ = 0;
        mutex_.unlock();
        return 0;
    }

    inline int init(unsigned int max_chunk_size=10000, unsigned int init_chunk_size=0, unsigned int per_chunk_size=10)
    {
        clear();
        if (init_chunk_size > max_chunk_size)
        {
            init_chunk_size = max_chunk_size;
        }

        mutex_.lock();
        max_chunk_size_ = max_chunk_size;
        per_chunk_size_ = per_chunk_size;
        for (unsigned int i=0; i<init_chunk_size; ++i)
        {
            alloc_i();
        }
        mutex_.unlock();
        return 0;
    }

    inline T* alloc_object()
    {
        T *object;
        mutex_.lock();
        if (!free_objects_.empty())
        {
            object = free_objects_.back();
            free_objects_.pop_back();
            mutex_.unlock();
            return object;
        }
        if (current_chunk_size_ >= max_chunk_size_)
        {
            mutex_.unlock();
            return NULL;
        }
        if (!alloc_i())
        {
            mutex_.unlock();
            return NULL;
        }
        if (!free_objects_.empty())
        {
            object = free_objects_.back();
            free_objects_.pop_back();
            mutex_.unlock();
            return object;
        }
        mutex_.unlock();
        return NULL;
    }

    inline void free_object(T *object)
    {
        mutex_.lock();
        free_objects_.push_back(object);
        mutex_.unlock();
    }

private:
    inline bool alloc_i()
    {
        T *chunk = new T[per_chunk_size_];
        if (NULL == chunk)
        {
            return false;
        }
        for(unsigned int i=0; i<per_chunk_size_; ++i)
        {
            free_objects_.push_back(&chunk[i]);
        }
        object_chunks_.push_back(chunk);
        ++current_chunk_size_;
        return true;
    }

    unsigned int    max_chunk_size_;      //对象池最大块数
    unsigned int    per_chunk_size_;      //每块中对象数
    unsigned int    current_chunk_size_;  

    TSyncMutex      mutex_;
    std::list<T* >  free_objects_;
    std::list<T* >  object_chunks_;
};

#endif

