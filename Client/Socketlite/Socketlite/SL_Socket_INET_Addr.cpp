#include <stdio.h>
#include "SL_Socket_INET_Addr.h"

SL_Socket_INET_Addr::SL_Socket_INET_Addr()
    : addr_type_(IPV4)
{
    reset();
}

SL_Socket_INET_Addr::SL_Socket_INET_Addr(bool ipv6)
{
    addr_type_ = ipv6 ? IPV6:IPV4;
    reset();
}

SL_Socket_INET_Addr::~SL_Socket_INET_Addr()
{
}

void SL_Socket_INET_Addr::reset()
{
    memset(&inet_addr_, 0, sizeof(inet_addr_));
    if (addr_type_ == IPV4)
    {
        inet_addr_.in4.sin_family = AF_INET;
    }
#ifdef SOCKETLITE_HAVE_IPV6
    else if (addr_type_ == IPV6)
    {
        inet_addr_.in6.sin6_family = AF_INET6;
    }
#endif
}

SL_Socket_INET_Addr::operator sockaddr* ()
{
#ifdef SOCKETLITE_HAVE_IPV6
    if (addr_type_ == IPV4)
    {
        return (sockaddr*)&inet_addr_.in4;
    }
    else
    {
        return (sockaddr*)&inet_addr_.in6;
    }
#else
    return (sockaddr*)&inet_addr_.in4;
#endif
}

sockaddr* SL_Socket_INET_Addr::get_addr() const
{
#ifdef SOCKETLITE_HAVE_IPV6
    if (addr_type_ == IPV4)
    {
        return (sockaddr *)&inet_addr_.in4;
    }
    else
    {
        return (sockaddr *)&inet_addr_.in6;
    }
#else
    return (sockaddr *)&inet_addr_.in4;
#endif
}

int SL_Socket_INET_Addr::get_addr_size() const
{
#ifdef SOCKETLITE_HAVE_IPV6
    if (addr_type_ == IPV4)
        return sizeof(inet_addr_.in4);
    else
        return sizeof(inet_addr_.in6);
#else
    return sizeof(inet_addr_.in4);
#endif
}

//实现方式:
// 1)WSAAddressToString(windows)
// 2)inet_ntoa(ipv4) 
// 3)inet_ntop(linux)
// 4)getnameinfo
int SL_Socket_INET_Addr::get_ip_addr(char *ip_addr, int len) const
{
    int ret = 0;
#ifdef SOCKETLITE_HAVE_IPV6
    if (addr_type_ == IPV4)
    {
        ret = getnameinfo((sockaddr*)&inet_addr_.in4, sizeof(inet_addr_.in4), ip_addr, len, 0, 0, NI_NUMERICHOST);
    }
    else
    {
        ret = getnameinfo((sockaddr*)&inet_addr_.in6, sizeof(inet_addr_.in6), ip_addr, len, 0, 0, NI_NUMERICHOST);
    }
#else
    ret = getnameinfo((sockaddr*)&inet_addr_.in4, sizeof(inet_addr_.in4), ip_addr, len, 0, 0, NI_NUMERICHOST);
#endif
    return ret;
}

ushort SL_Socket_INET_Addr::get_port_number() const
{
#ifdef SOCKETLITE_HAVE_IPV6
    if (addr_type_ == IPV4)
        return ntohs(inet_addr_.in4.sin_port);
    else
        return ntohs(inet_addr_.in6.sin6_port);
#else
    return ntohs(inet_addr_.in4.sin_port);
#endif
}

bool SL_Socket_INET_Addr::is_ipv4() const
{
    return addr_type_ == IPV4;
}

bool SL_Socket_INET_Addr::is_ipv6() const
{
    return addr_type_ == IPV6;
}

//实现方式：
// 1)getaddrinfo
// 2)gethostbyname(建议使用getaddrinfo, 此函数已被弃用deprecated)
int SL_Socket_INET_Addr::set(const char *hostname, ushort port, bool is_ipv6)
{
    memset(&inet_addr_, 0, sizeof(inet_addr_));
    char str_port[7] = {0};
    sprintf(str_port, "%d", port);
#ifdef SOCKETLITE_HAVE_IPV6    
    struct addrinfo hints = {0};
    struct addrinfo *ret = NULL;

    if (!is_ipv6)
    {
        addr_type_ = IPV4;
        hints.ai_flags  = AI_PASSIVE;
        hints.ai_family = AF_INET;
        inet_addr_.in4.sin_family = AF_INET;
        inet_addr_.in4.sin_port = htons(port);
    }
    else
    {
        addr_type_ = IPV6;
        hints.ai_flags  = AI_PASSIVE;
        hints.ai_family = AF_INET6;
        inet_addr_.in6.sin6_family = AF_INET6;
        inet_addr_.in6.sin6_port = htons(port);
    }
    int error = getaddrinfo(hostname, str_port, &hints, &ret);
    if (error != 0)
    {
        if (ret != NULL)
        {
            freeaddrinfo(ret);
        }
        return -1;
    }    
    if (ret->ai_family == AF_INET)
    {
        memcpy(&inet_addr_.in4, ret->ai_addr, ret->ai_addrlen);
        inet_addr_.in4.sin_port = htons(port);
    }
    else
    {
        memcpy(&inet_addr_.in6, ret->ai_addr, ret->ai_addrlen);
        inet_addr_.in6.sin6_port = htons(port);
    }
    freeaddrinfo(ret);
#else
    struct addrinfo hints = {0};
    struct addrinfo *ret = NULL;
    addr_type_ = IPV4;
    hints.ai_flags  = AI_PASSIVE;
    hints.ai_family = AF_INET;
    inet_addr_.in4.sin_family = AF_INET;
    inet_addr_.in4.sin_port = htons(port);
    int error = getaddrinfo(hostname, str_port, &hints, &ret);
    if (error != 0)
    {
        if (ret != NULL)
        {
            freeaddrinfo(ret);
        }
        return -1;
    }    
    memcpy(&inet_addr_.in4, ret->ai_addr, ret->ai_addrlen);
    inet_addr_.in4.sin_port = htons(port);
#endif
    return 0;
}

int SL_Socket_INET_Addr::set(const struct sockaddr *addr, int addrlen)
{
#ifdef SOCKETLITE_HAVE_IPV6
    if (addr->sa_family == AF_INET)
    {
        addr_type_ = IPV4;
        memcpy(&inet_addr_.in4, addr, addrlen);
    }
    else
    {
        addr_type_ = IPV6;
        memcpy(&inet_addr_.in6, addr, addrlen);
    }
#else
    addr_type_ = IPV4;
    memcpy(&inet_addr_.in4, addr, addrlen);
#endif
    return 0;
}

int SL_Socket_INET_Addr::set(SL_SOCKET fd, bool ipv6)
{
    int addrlen;
#ifdef SOCKETLITE_HAVE_IPV6
    if (!ipv6)
    {
        addr_type_ = IPV4;
        addrlen = sizeof(inet_addr_.in4);
        return getpeername(fd, (sockaddr *)&inet_addr_.in4, (socklen_t *)&addrlen);
    }
    else
    {
        addr_type_= IPV6;
        addrlen = sizeof(inet_addr_.in6);
        return getpeername(fd, (sockaddr *)&inet_addr_.in6, (socklen_t *)&addrlen);
    }
#else
    addr_type_ = IPV4;
    addrlen = sizeof(inet_addr_.in4);
    return getpeername(fd, (sockaddr *)&inet_addr_.in4, (socklen_t *)&addrlen);
#endif
}

int SL_Socket_INET_Addr::set(SL_SOCKET fd)
{
    sockaddr_storage ss;
    int ss_len = sizeof(sockaddr_storage);
    memset(&ss, 0, sizeof(sockaddr_storage));
    int ret = getpeername(fd, (sockaddr *)&ss, (socklen_t *)&ss_len);
    if (ret != 0)
        return ret;

#ifdef SOCKETLITE_HAVE_IPV6
    if (ss.ss_family == AF_INET)
    {//IPV4
        addr_type_ = IPV4;
        memcpy(&inet_addr_.in4, &ss, sizeof(inet_addr_.in4));
    }
    else
    {//IPV6
        addr_type_ = IPV6;
        memcpy(&inet_addr_.in6, &ss, sizeof(inet_addr_.in6));
    }            
#else
    addr_type_ = IPV4;
    memcpy(&inet_addr_.in4, &ss, sizeof(inet_addr_.in4));
#endif
    return 0;
}

int SL_Socket_INET_Addr::get_ip_remote_s(SL_SOCKET fd, char *ip_addr, int ip_len, ushort *port_number)
{
    sockaddr_storage ss;
    int ss_len = sizeof(sockaddr_storage);
    memset(&ss, 0, sizeof(sockaddr_storage));
    int ret = getpeername(fd, (sockaddr *)&ss,(socklen_t *)&ss_len);
    if (ret != 0)
    {
        return ret;
    }
    if (port_number != 0)
    {
#ifdef SOCKETLITE_HAVE_IPV6
        if (ss.ss_family == AF_INET)
        {//IPV4
            sockaddr_in *in4 = (sockaddr_in *)&ss;
            *port_number = ntohs(in4->sin_port);
        }
        else
        {//IPV6
            sockaddr_in6 *in6 = (sockaddr_in6 *)&ss;
            *port_number = ntohs(in6->sin6_port);
        }            
#else
        sockaddr_in *in4 = (sockaddr_in *)&ss;
        *port_number = ntohs(in4->sin_port);
#endif
    }
    ret = getnameinfo((sockaddr *)&ss, ss_len, ip_addr, ip_len, 0, 0, NI_NUMERICHOST);
    return ret;
}

int SL_Socket_INET_Addr::get_ip_local_s(SL_SOCKET fd, char *ip_addr, int ip_len, ushort *port_number)
{
    sockaddr_storage ss;
    int ss_len = sizeof(sockaddr_storage);
    memset(&ss, 0, sizeof(sockaddr_storage));
    int ret  = getsockname(fd, (sockaddr *)&ss, (socklen_t *)&ss_len);
    if (ret != 0)
    {
        return ret;
    }
    if (port_number != 0)
    {
#ifdef SOCKETLITE_HAVE_IPV6
        if (ss.ss_family == AF_INET)
        {//IPV4
            sockaddr_in *in4 = (sockaddr_in *)&ss;
            *port_number = ntohs(in4->sin_port);
        }
        else
        {//IPV6
            sockaddr_in6 *in6 = (sockaddr_in6 *)&ss;
            *port_number = ntohs(in6->sin6_port);
        }            
#else
        sockaddr_in *in4 = (sockaddr_in *)&ss;
        *port_number = ntohs(in4->sin_port);
#endif
    }
    ret = getnameinfo((sockaddr *)&ss, ss_len, ip_addr, ip_len, 0, 0, NI_NUMERICHOST);
    return ret;
}

int SL_Socket_INET_Addr::get_ip_s(const struct sockaddr *addr, int addrlen, char *ip_addr, int ip_len, ushort *port_number)
{
    int ret = 0;
    if (port_number != 0)
    {
#ifdef SOCKETLITE_HAVE_IPV6
        if (addr->sa_family == AF_INET)
        {//IPV4
            sockaddr_in *in4 = (sockaddr_in *)addr;
            *port_number = ntohs(in4->sin_port);
        }
        else
        {//IPV6
            sockaddr_in6 *in6 = (sockaddr_in6 *)addr;
            *port_number = ntohs(in6->sin6_port);
        }            
#else
        sockaddr_in *in4 = (sockaddr_in *)addr;
        *port_number = ntohs(in4->sin_port);
#endif
    }
    ret = getnameinfo(addr, addrlen, ip_addr, ip_len, 0, 0, NI_NUMERICHOST);
    return ret;    
}

int SL_Socket_INET_Addr::get_addr_s(const char *hostname, ushort port, sockaddr *addr, int addrlen)
{
    struct addrinfo hints = {0};
    struct addrinfo *ret = NULL;
    int error = 0;
    if (addrlen == sizeof(sockaddr_in))
    {
        hints.ai_family     = AF_INET;
        sockaddr_in *in4    = (sockaddr_in *)addr;
        in4->sin_port       = ntohs(port);
    }
    else
    {
        hints.ai_family     = AF_INET6;
        sockaddr_in6 *in6   = (sockaddr_in6 *)addr;
        in6->sin6_port      = ntohs(port);
    }
    if (hostname == NULL) 
    {
        return 0;
    }

    error = getaddrinfo(hostname, 0, &hints, &ret);
    if (error != 0)
    {
        if (ret != NULL)
        {
            freeaddrinfo(ret);
        }
        return -1;
    }    
    memcpy(addr, ret->ai_addr, ret->ai_addrlen);
    freeaddrinfo(ret);
    if (addr->sa_family == AF_INET)
    {
        sockaddr_in *in4    = (sockaddr_in *)addr;
        in4->sin_port       = ntohs(port);
    }
    else
    {
        sockaddr_in6 *in6   = (sockaddr_in6 *)addr;
        in6->sin6_port      = ntohs(port);
    }
    return 0;
}

ulong SL_Socket_INET_Addr::get_inet_addr_s(const char *ip)
{
    return inet_addr(ip);
}

