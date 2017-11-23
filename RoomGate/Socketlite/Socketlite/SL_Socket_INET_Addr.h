#ifndef SOCKETLITE_SOCKET_INET_ADDR_H
#define SOCKETLITE_SOCKET_INET_ADDR_H
#include "SL_Config.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif

class SOCKETLITE_API SL_Socket_INET_Addr
{
public:

    SL_Socket_INET_Addr();
    SL_Socket_INET_Addr(bool ipv6);
    ~SL_Socket_INET_Addr();

    void            reset();
    int             set(const char *hostname, ushort port, bool is_ipv6 = false);
    int             set(const struct sockaddr *addr, int addrlen);
    int             set(SL_SOCKET fd, bool ipv6);
    int             set(SL_SOCKET fd);

    operator        sockaddr* ();
    sockaddr*       get_addr() const;
    int             get_addr_size() const;
    int             get_ip_addr(char *ip_addr, int len) const;
    ushort          get_port_number() const;

    bool            is_ipv4() const;
    bool            is_ipv6() const;

    static int      get_ip_remote_s(SL_SOCKET fd, char *ip_addr, int ip_len, ushort *port_number);
    static int      get_ip_local_s(SL_SOCKET fd, char *ip_addr, int ip_len, ushort *port_number);
    static int      get_ip_s(const struct sockaddr *addr, int addrlen, char *ip_addr, int ip_len, ushort *port_number);
    static int      get_addr_s(const char *hostname, ushort port, sockaddr *addr, int addrlen);
    static ulong    get_inet_addr_s(const char *ip);

private:
    enum
    {
        IPV4, 
        IPV6
    }addr_type_;

    union
    {
        sockaddr_in  in4;
#ifdef SOCKETLITE_HAVE_IPV6
        sockaddr_in6 in6;
#endif
    }inet_addr_;

};

#endif

