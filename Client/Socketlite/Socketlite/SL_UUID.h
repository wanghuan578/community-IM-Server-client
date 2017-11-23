//UUID: Universally Unique Identifier

#ifndef SL_UUID_H
#define SL_UUID_H
#include "SL_Config.h"

#ifdef SOCKETLITE_OS_WINDOWS

#include <Rpc.h>
#pragma comment(lib, "Rpcrt4.lib")

class SL_UUID
{
public:
    static int generate(char uuid_string[50])
    {
        UUID uuid;
        ::UuidCreate(&uuid);
        
        //方法一
        int ret = sprintf(uuid_string, "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uuid.Data1, uuid.Data2, uuid.Data3,
            uuid.Data4[0], uuid.Data4[1], uuid.Data4[2], uuid.Data4[3],
            uuid.Data4[4], uuid.Data4[5], uuid.Data4[6], uuid.Data4[7]);
        return ret;

        //方法二
        //unsigned char *str;
        //::UuidToString(&uuid, &str);
        //strncpy(uuid_string, str, 50);
        //RpcStringFree(&str);
        //return 0;
    };

private:
    SL_UUID()
    {
    };

    ~SL_UUID()
    {
    };
};

#else   //SOCKETLITE_OS_LINUX

//link libuuid.so
#include <uuid/uuid.h>

class SL_UUID
{
public:
    static int generate(char uuid_string[50])
    {
        uuid_t uuid;
        uuid_generate(uuid);
        uuid_unparse(uuid, uuid_string);
        return 0;
    };

private:
    SL_UUID()
    {
    };

    ~SL_UUID()
    {
    };
};

#endif  //SOCKETLITE_OS_WINDOWS

#endif  //SL_UUID_H

