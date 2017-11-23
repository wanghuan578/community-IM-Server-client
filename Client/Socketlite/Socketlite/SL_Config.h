// Copyright (c) 2007-2013, Bolide Zhang <bolidezhang@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef SOCKETLITE_CONFIG_H
#define SOCKETLITE_CONFIG_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif

#define SOCKETLITE_MAJOR_VERSION        3
#define SOCKETLITE_MINOR_VERSION        2
#define SOCKETLITE_SUB_MINOR_VERSION    26
#define SOCKETLITE_VERSION_STRING       "3.2.26"

#ifdef _DEBUG
    #define SOCKETLITE_DEBUG
#else
    #define SOCKETLITE_NDEBUG
#endif

//SL_Socket_Runner event mask(runner默认情况下只对读事件关注)
#define SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
//#define SOCKETLITE_RUNNER_EVENTMASK_ALL

//os type
//#define SOCKETLITE_OS_WINDOWS
//#define SOCKETLITE_OS_LINUX
//#define SOCKETLITE_OS_FREEBSD
//#define SOCKETLITE_OS_TYPE

//c++ compiler type
//#define SOCKETLIET_CC_MSVC
//#define SOCKETLITE_CC_GCC
//#define SOCKETLITE_CC_LLVM_CLANG
//#define SOCKETLITE_CC_INTEL
//#define SOCKETLITE_CC_BORLAND
//#define SOCKETLITE_CC_TYPE

//application type
//#define SOCKETLITE_HAVE_DLL
//#define SOCKETLITE_HAVE_STATICLIB

//dll type declare
//#define SOCKETLITE_DLL_EXPORT
//#define SOCKETLITE_DLL_IMPORT

//IPV6
#define SOCKETLITE_HAVE_IPV6

//C 标准
//1.C89
//#define SOCKETLITE_C_STD_1989
//2.C90
//#define SOCKETLITE_C_STD_1990
//3.C99
//#define SOCKETLITE_C_STD_1999
//4.C11
//#define SOCKETLITE_C_STD_2011

//C++ 标准
//1.C++98
//#define SOCKETLITE_CPLUSPLUS_STD_1998
//2.C++03
//#define SOCKETLITE_CPLUSPLUS_STD_2003
//3.C++TR1
//#define SOCKETLITE_CPLUSPLUS_STD_TR1
//4.C++TR2
//#define SOCKETLITE_CPLUSPLUS_STD_TR2
//5.C++11
//#define SOCKETLITE_CPLUSPLUS_STD_2011
//6.C++14
//#define SOCKETLITE_CPLUSPLUS_STD_2014
//7.C++17
//#define SOCKETLITE_CPLUSPLUS_STD_2017

//STL 实现提供商
//#define SOCKETLITE_STL_MS
//#define SOCKETLITE_STL_GNU_LIBSTDC++
//#define SOCKETLITE_STL_SGI
//#define SOCKETLITE_STL_STLPORT
//#define SOCKETLITE_STL_APACHE_STDCXX
//#define SOCKETLITE_STL_LLVM_LIBC++

//STL Extension
#define SOCKETLITE_USE_STLEXTENSION

//字节序相关
//#define SOCKETLITE_BIG_ENDIAN
//#define SOCKETLITE_LITTLE_ENDIAN
//#define SOCKETLITE_BYTE_ORDER
#define __SOCKETLITE_BIG_ENDIAN		1
#define __SOCKETLITE_LITTLE_ENDIAN	2
#if defined(SOCKETLITE_BIG_ENDIAN)
    #define SOCKETLITE_BYTE_ORDER __SOCKETLITE_BIG_ENDIAN
#endif
#if defined(SOCKETLITE_LITTLE_ENDIAN)
    #define SOCKETLITE_BYTE_ORDER __SOCKETLITE_LITTLE_ENDIAN
#endif

//内存分配 比较优秀malloc的实现
//memory allocation: malloc implement
//1.google tcmalloc
//#define SOCKETLITE_MALLOC_TCMALLOC    
//2.jemalloc freebsd/facebook/firefox
//#define SOCKETLITE_MALLOC_JEMALLOC
//3.hoard - The Hoard Memory Allocator
//#define SOCKETLITE_MALLOC_HOARD
//4.nedmalloc - ned Productions dlmalloc
//#define SOCKETLITE_MALLOC_NEDMALLOC
//5.Lockless malloc - Lockless Inc. Low level software to optimize performance
//#define SOCKETLITE_MALLOC_LOCKLESS
//6.intel tbbmalloc
//#define SOCKETLITE_MALLOC_TBBMALLOC
//7.ptmalloc - a) linux glibc default malloc b) 起源于 Doug Lea 的 dlmalloc
//#define SOCKETLITE_MALLOC_PTMALLOC
//8.dlmalloc - Doug Lea dlmalloc
//#define SOCKETLITEZ_MALLOC_DLMALLOC

//消息序列化编码方式
//#define SOCKETLITE_USE_THRIFT
//#define SOCKETLITE_USE_PROTOBUF
//#define SOCKETLITE_USE_MESSAGEPACK
#ifndef SOCKETLITE_USE_THRIFT
    #define SOCKETLITE_USE_THRIFT
    #define SOCKETLITE_ENCODE_THRIFT
#endif
#ifndef SOCKETLITE_USE_PROTOBUF
    #define SOCKETLITE_USE_PROTOBUF
    #define SOCKETLITE_ENCODE_PROTOBUF
#endif
#ifndef SOCKETLITE_USE_MESSAGEPACK
    #define SOCKETLITE_USE_MESSAGEPACK
    #define SOCKETLITE_ENCODE_MESSAGEPACK
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32)
    #ifndef SOCKETLITE_NO_SUPPORT_WINXP
        #ifndef _WIN32_WINNT
            #define _WIN32_WINNT 0x0500
        #endif
        #ifndef WINVER
            #define WINVER 0x0500
        #endif
    #endif

    #define SOCKETLITE_OS_WINDOWS
    #define SOCKETLIET_CC_MSVC
    #define SOCKETlITE_STL_MS
#else
    #define SOCKETLITE_OS_LINUX
    #define SOCKETLITE_CC_GCC
    #define SOCKETlITE_STL_SGI
#endif
#if defined(_WIN32)
    #define SOCKETLITE_OS_WIN32
#endif
#if defined(_WIN64)
    #define SOCKETLITE_OS_WIN64
#endif

#ifdef SOCKETLITE_OS_WINDOWS
    #ifndef SOCKETLITE_BYTE_ORDER
        #define SOCKETLITE_BYTE_ORDER __SOCKETLITE_LITTLE_ENDIAN
    #endif 
    #ifdef SOCKETLITE_HAS_DLL
        #ifdef SL_DLL_EXPORT
            #define SOCKETITE_API __declspec(dllexport)
        #else
            #define SOCKETLITE_API __declspec(dllimport)
        #endif
    #else
        #define SOCKETLITE_API
    #endif

    /* FD_SETSIZE */
    #ifndef FD_SETSIZE
        #define FD_SETSIZE  1024
    #else
        #undef FD_SETSIZE
        #define FD_SETSIZE  1024            
    #endif
    // Winsock库版本(默认winsock2)
    #if !defined(SOCKETLITE_USE_WINSOCK2)
        #define SOCKETLITE_USE_WINSOCK2 1
    #endif
    #if defined(SOCKETLITE_USE_WINSOCK2) && (SOCKETLITE_USE_WINSOCK2 != 0)
        #define SOCKETLITE_WINSOCK_VERSION 2, 2
        // will also include windows.h, if not present
        #include <winsock2.h>
        #if (_WIN32_WINNT >= 0x0500)  //window 2000 or later version
            //这个两个头文件先后顺序不能改变,否则在vc2008以上版本编译通不过
            #include <mstcpip.h>       
            #include <ws2tcpip.h>
        #endif
    #else
        #define SOCKETLITE_WINSOCK_VERSION 1, 0
        #include <winsock.h>
    #endif
    #pragma comment(lib, "ws2_32.lib")

    // 微软扩展SOCKET API
    #include <mswsock.h>
    #pragma comment(lib, "mswsock.lib")
    #include <nspapi.h>
    #pragma comment(lib, "wsock32.lib")

    // 常见头文件
    #include <errno.h>
    #include <sys/stat.h>
    #include <sys/types.h>

    typedef HANDLE              SL_HANDLE;
    typedef SOCKET              SL_SOCKET;
    typedef OVERLAPPED          SL_OVERLAPPED;

    struct SL_IOVEC
    {
        u_long  iov_len;
        char    *iov_base;
    };
    #define SL_MAX_IOVCNT       65536
    #define SL_IOVCNT           64

    #define SL_SOCKET_ERROR     SOCKET_ERROR
    #define SL_INVALID_SOCKET   INVALID_SOCKET
    #define SL_SHUT_RD          SD_RECEIVE
    #define SL_SHUT_WR          SD_SEND
    #define SL_SHUT_RDWR        SD_BOTH

    //一些常见Socket错误
    #define SL_EAGAIN           WSAEWOULDBLOCK
    #define SL_ENOBUFS          WSAENOBUFS
    #define SL_EINVAL           WSAEINVAL
    #define SL_EINTR            WSAEINTR
    #define SL_WSAENOTSOCK      WSAENOTSOCK
    #define SL_EISCONN          WSAEISCONN
    #define SL_ENOTCONN         WSAENOTCONN
    #define SL_ESHUTDOWN        WSAESHUTDOWN
    #define SL_EMSGSIZE         WSAEMSGSIZE
    #define SL_EWOULDBLOCK      WSAEWOULDBLOCK
    #define SL_IO_PENDING       WSA_IO_PENDING
#else
    #include <string.h>
    #include <errno.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/uio.h>
    #include <sys/socket.h>
    #include <sys/times.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <pthread.h>
    #include <signal.h>

    typedef int   SL_HANDLE;
    typedef int   SL_SOCKET;
    typedef void  SL_OVERLAPPED;

    typedef iovec SL_IOVEC;
    #define SL_MAX_IOVCNT       1024
    #define SL_IOVCNT           64

    #define SOCKETLITE_API
    #define SOCKETLITE_HAVE_PTHREADS
    #define SL_SOCKET_ERROR     (-1)
    #define SL_INVALID_SOCKET   (-1)
    #define SL_SHUT_RD          SHUT_RD
    #define SL_SHUT_WR          SHUT_WR
    #define SL_SHUT_RDWR        SHUT_RDWR

    //一些常见Socket错误
    #define SL_EAGAIN           EAGAIN
    #define SL_ENOBUFS          ENOBUFS
    #define SL_EINVAL           EINVAL
    #define SL_EINTR            EINTR
    #define SL_WSAENOTSOCK      ENOTSOCK
    #define SL_EISCONN          EISCONN
    #define SL_ENOTCONN         ENOTCONN
    #define SL_ESHUTDOWN        ESHUTDOWN
    #define SL_EMSGSIZE         EMSGSIZE
    #define SL_EWOULDBLOCK      EWOULDBLOCK
    #define SL_IO_PENDING       EAGAIN
#endif

//设置默认字节序
#ifndef SOCKETLITE_BYTE_ORDER
    #define SOCKETLITE_BYTE_ORDER __SOCKETLITE_LITTLE_ENDIAN
#endif

//设置默认操作系统
#ifndef SOCKETLITE_OS_WINDOWS
    #define SOCKETLITE_OS_LINUX
#endif

#ifdef SOCKETLITE_OS_LINUX
#endif

#ifndef SOCKETLITE_CPLUSPLUS_STD_2011
    #ifndef SOCKETLITE_CPLUSPLUS_STD_TR1
        #ifdef SOCKETlITE_STL_MS
            #include <hash_map>
            #include <hash_set>
            #define SL_STDEXT_NAMESPACE stdext
            #define SL_HASH_REHASH(c,n) {}
            #define SL_DEFAULT_HASHFUNC stdext::hash_compare
        #else
            #ifdef SOCKETLITE_CC_GCC
                #include <ext/hash_map>
                #include <ext/hash_set>
                #define SL_DEFAULT_HASHFUNC __gnu_cxx::hash
            #else
                #include <hash_map>
                #include <hash_set>
                #define SL_DEFAULT_HASHFUNC std::hash
            #endif
            #define SL_STDEXT_NAMESPACE __gnu_cxx
            #define SL_HASH_REHASH(c,n) c.resize(n)
        #endif
        #define SL_HASH_MAP         SL_STDEXT_NAMESPACE::hash_map
        #define SL_HASH_SET         SL_STDEXT_NAMESPACE::hash_set
        #define SL_HASH_MULTIMAP    SL_STDEXT_NAMESPACE::hash_multimap
        #define SL_HASH_MULTISET    SL_STDEXT_NAMESPACE::hash_multiset
    #else
        #ifdef SOCKETLITE_CC_GCC
            #include <tr1/unordered_map>
            #include <tr1/unordered_set>
        #else
            #include <unordered_map>
            #include <unordered_set>
        #endif
        #define SL_DEFAULT_HASHFUNC std::tr1::hash
        #define SL_HASH_MAP         std::tr1::unordered_map
        #define SL_HASH_SET         std::tr1::unordered_set
        #define SL_HASH_MULTIMAP    std::tr1::unordered_multimap
        #define SL_HASH_MULTISET    std::tr1::unordered_multiset
        #define SL_HASH_REHASH(c,n) c.rehash(n)
    #endif
#else
    #include <unordered_map>
    #include <unordered_set>
    #define SL_DEFAULT_HASHFUNC std::hash
    #define SL_HASH_MAP         std::unordered_map
    #define SL_HASH_SET         std::unordered_set
    #define SL_HASH_MULTIMAP    std::unordered_multimap
    #define SL_HASH_MULTISET    std::unordered_multiset
    #define SL_HASH_REHASH(c,n) c.rehash(n)
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef SOCKETLITE_OS_WINDOWS
    #include <tchar.h>
    #define SL_TEXT(x)      _T(x)
    #define SL_SNPRINTF     _snprintf
    #define SL_VSNPRINF     _vsnprintf
    typedef struct _stat    SL_STAT;
#else
    #define SL_TEXT(x)      x
    #define SL_SNPRINTF     snprintf
    #define SL_VSNPRINF     vsnprintf
    typedef struct stat     SL_STAT;
#endif

#include "SL_DataType.h"
#include "SL_Hash_Fun.h"

#endif

