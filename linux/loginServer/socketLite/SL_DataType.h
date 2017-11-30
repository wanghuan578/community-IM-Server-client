//**********************************************************************
//
// Copyright (C) 2007 Bolide Zhang <bolidezhang@gmail.com>
// All rights reserved.
//
// This copy of SocketLite is licensed to you under the terms described 
// in the LICENSE.txt file included in this distribution.
//
//**********************************************************************

#ifndef SOCKETLITE_DATATYPE_H
#define SOCKETLITE_DATATYPE_H
#include <string>
#include <sstream>

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif

typedef unsigned char           byte;
typedef unsigned short          ushort;
typedef unsigned int            uint;
typedef unsigned long           ulong;
typedef float                   f32;
typedef double                  f64;
typedef unsigned long long      ulonglong;

#ifdef _MSC_VER
    typedef __int8              int8;
    typedef __int16             int16;
    typedef __int32             int32;
    typedef __int64             int64;
    typedef unsigned __int8     uint8;
    typedef unsigned __int16    uint16;
    typedef unsigned __int32    uint32;
    typedef unsigned __int64    uint64;
#else
    typedef char                int8;
    typedef short               int16;
    typedef int                 int32;
    typedef long long           int64;
    typedef unsigned char       uint8;
    typedef unsigned short      uint16;
    typedef unsigned int        uint32;
    typedef unsigned long long  uint64;
#endif

//标准输入/输出文件号
enum { SL_STDIN_FILENO = 0, SL_STDOUT_FILENO = 1, SL_STDERR_FILENO = 2 };

//bool 类型特别处理
#define SL_BOOL                 byte
#define SL_FALSE                0
#define SL_TRUE                 1

//unicode
#ifndef _UNICODE
    typedef char                SL_TCHAR;
    typedef std::string         SL_TSTRING;
    typedef std::ostringstream  SL_TOSTRINGSTREAM;
    typedef std::stringstream   SL_TSTRINGSTREAM;
#else
    typedef wchar_t             SL_TCHAR;
    typedef std::wstring        SL_TSTRING;
    typedef std::wostringstream SL_TOSTRINGSTREAM;
    typedef std::wstringstream  SL_TSTRINGSTREAM;
#endif

#endif

