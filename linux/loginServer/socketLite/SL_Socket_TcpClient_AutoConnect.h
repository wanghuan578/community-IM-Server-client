#ifndef SOCKETLITE_SOCKET_TCP_CLIENT_AUTO_CONNECT_H
#define SOCKETLITE_SOCKET_TCP_CLIENT_AUTO_CONNECT_H

#include <set>
#include "SL_Config.h"
#include "SL_Thread.h"
#include "SL_Sync_SpinMutex.h"
#include "SL_Socket_TcpClient.h"

class SOCKETLITE_API SL_Socket_TcpClient_AutoConnect
{
public:
    SL_Socket_TcpClient_AutoConnect();
    ~SL_Socket_TcpClient_AutoConnect();
    
    int open(uint check_interval_us=500000);
    int close();

    int add_tcpclient(SL_Socket_Source *tcpclient);
    int del_tcpclient(SL_Socket_Source *tcpclient);
    int clear();

private:
#ifdef SOCKETLITE_OS_WINDOWS
    static unsigned int WINAPI autoconnect_proc(void *arg);
#else
    static void* autoconnect_proc(void *arg);
#endif

    SL_Sync_SpinMutex               mutex_;
    std::set<SL_Socket_Source* >    tcpclient_set_;
    SL_Thread                       autoconnect_thread_;
    uint                            check_interval_us_;
};

#endif

