#ifndef SOCKETLITE_ALLOC_H
#define SOCKETLITE_ALLOC_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif
#include <stdlib.h>
#include <string.h>
#include "SL_DataType.h"

//默认的内存块大小，经验值
#define SL_DEFAULT_MEMORY_BLOCK_SIZE    8192

class SL_Alloc_New
{
public:
    static void* allocate(size_t n)
    { 
        return new char[n];
    }

    static void deallocate(void *p)
    { 
        delete [](char *)p;
    }

    static void* reallocate(void *p, size_t n)
    {
        return NULL;
    }

};

class SL_Alloc_Malloc
{
public:
    static void* allocate(size_t n)
    { 
        return malloc(n);
    }

    static void deallocate(void *p)
    { 
        free(p);
    }

    static void* reallocate(void *p, size_t n)
    {
        return realloc(p, n);
    }

};

#ifdef SL_ALLOC_NEW
    #define SL_ALLOC    SL_Alloc_New
#else
    #define SL_ALLOC    SL_Alloc_Malloc
#endif

template <typename T>
class SL_Allocator
{
public:
    virtual int clear()
    {
    }

    virtual void* allocate(size_type n)
    { 
        return SL_ALLOC::allocate(n);
    }

    virtual void deallocate(void *p)
    { 
        SL_ALLOC::deallocate(p);
    }
    
    virtual pointer construct()
    {
        void *p = SL_ALLOC::allocate(sizeof(T));
        pointer object = new (p) value_type();
        return object;
    }

    virtual void construct(T *p)
    {
        new (p) value_type();
    }

    virtual void destroy(T *p)
    {
        ((pointer)p)->~T();
        deallocate(p);
    }

    static SL_Allocator* Instance()
    {
        return &m_Alloc;
    }
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

private:
    static SL_Allocator<char> m_Alloc;
};

#endif

