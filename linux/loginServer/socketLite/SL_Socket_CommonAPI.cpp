#include "SL_Socket_CommonAPI.h"
#include "SL_OS_API.h"
#include <time.h>

#ifndef SOCKETLITE_OS_WINDOWS
    #include <byteswap.h>
#endif

unsigned int SL_Socket_CommonAPI::timer_resolution_ms_ = 0;

SL_Socket_CommonAPI::SL_Socket_CommonAPI()
{
}

SL_Socket_CommonAPI::~SL_Socket_CommonAPI()
{
}

int SL_Socket_CommonAPI::socket_init(int version_high, int version_low, int timer_resolution_ms)
{
#ifdef SOCKETLITE_OS_WINDOWS

    // initialize timer resolution
    // attempt to set the resolution as low as possible from 1ms to 5ms
    if (timer_resolution_ms <= 0)
    {//自动设置OS允许定时精度的最小值
        for (unsigned int i=1; i<=5; ++i)
        {
            if (timeBeginPeriod(i) == TIMERR_NOERROR)
            {
                timer_resolution_ms_ = i;
                break;
            }
        }
    }
    else
    {
        if (timer_resolution_ms > 5)
        {
            timer_resolution_ms = 5;
        }
        if (timeBeginPeriod(timer_resolution_ms) == TIMERR_NOERROR)
        {
            timer_resolution_ms_ = timer_resolution_ms;
        }
    }

    //initialize winsock
    WSADATA wsaData;
    WORD version_requested = MAKEWORD(version_high, version_low);
    return WSAStartup(version_requested, &wsaData);

#else
    return 0;
#endif
}

int SL_Socket_CommonAPI::socket_fini()
{
#ifdef SOCKETLITE_OS_WINDOWS
    // shutdown timer
    if (timer_resolution_ms_ > 0)
    {
        timeEndPeriod(timer_resolution_ms_);
    }

    return WSACleanup();
#else
    return 0;
#endif
}

int SL_Socket_CommonAPI::socket_get_lasterror()
{
#ifdef SOCKETLITE_OS_WINDOWS
    return GetLastError();
#else
    return errno;
#endif
}

int SL_Socket_CommonAPI::socket_get_lasterror_wsa()
{
#ifdef SOCKETLITE_OS_WINDOWS
    return WSAGetLastError();
#else
    return errno;
#endif
}

int SL_Socket_CommonAPI::socket_get_error(SL_SOCKET fd)
{
    int socket_error = 0;
    int socket_error_len =  sizeof(int);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *)&socket_error, (socklen_t *)&socket_error_len) < 0)
    {
        return -1;
    }
    return socket_error;
}

SL_SOCKET SL_Socket_CommonAPI::socket_open(int address_family, int type, int protocol)
{
    return socket(address_family, type, protocol);
}

int SL_Socket_CommonAPI::socket_close(SL_SOCKET fd)
{
    if ((fd < 0) || (fd == SL_INVALID_SOCKET))
    {
        return -1;
    }
#ifdef SOCKETLITE_OS_WINDOWS
    return closesocket(fd);
#else
    return close(fd);
#endif
}

int SL_Socket_CommonAPI::socket_shutdown(SL_SOCKET fd, int how)
{
    if ((fd < 0) || (fd == SL_INVALID_SOCKET))
    {
        return -1;
    }
    return shutdown(fd, how);
}

SL_SOCKET SL_Socket_CommonAPI::socket_accept(SL_SOCKET fd, struct sockaddr *addr, int *addrlen)
{
#ifdef SOCKETLITE_OS_WINDOWS
    return WSAAccept(fd, addr, addrlen, NULL, 0);
#else
    return accept(fd, addr, (socklen_t *)addrlen);
#endif
}

int SL_Socket_CommonAPI::socket_bind(SL_SOCKET fd, const struct sockaddr *addr, int addrlen)
{
    return bind(fd, addr, addrlen);
}

int SL_Socket_CommonAPI::socket_listen(SL_SOCKET fd, int backlog)
{
    return listen(fd, backlog);
}

int SL_Socket_CommonAPI::socket_connect(SL_SOCKET fd, const struct sockaddr *addr, int addrlen)
{
    return connect(fd, addr, addrlen);
}

 int SL_Socket_CommonAPI::socket_connect(SL_SOCKET fd, const struct sockaddr *addr, int addrlen,  timeval *timeout)
{
    if (NULL == timeout)
    {
        return connect(fd, addr, addrlen);
    }
    else
    {
        SL_Socket_CommonAPI::socket_set_block(fd, false);
        int ret = connect(fd, addr, addrlen);
        if (ret == 0)
        {
            return 0;
        }
        if (ret == SL_SOCKET_ERROR) 
        {
            fd_set read_set;
            fd_set write_set;
            FD_ZERO(&read_set);
            FD_ZERO(&write_set);
            FD_SET(fd, &read_set);
            FD_SET(fd, &write_set);
#ifdef SOCKETLITE_OS_WINDOWS
            int n = select(0, &read_set, &write_set, NULL , timeout);
#else
            int n = select(fd+1, &read_set, &write_set, NULL, timeout);
#endif
            if (n <= 0)
            {
                return -1;
            }
            if (FD_ISSET(fd, &read_set) || FD_ISSET(fd, &write_set))
            {
                if (socket_get_error(fd) == 0)
                {
                    return 0;
                }
            }
        }
    }
    return -1;
}

 int SL_Socket_CommonAPI::socket_set_block(SL_SOCKET fd, bool block)
{
#ifdef SOCKETLITE_OS_WINDOWS
    {
        unsigned long arg = (block ? 0:1);
        return ioctlsocket(fd, FIONBIO, &arg);
    }
#else
    //方法一
    int flags = fcntl(fd, F_GETFL);
    if (block)
    {
        flags &= ~O_NONBLOCK;
    }
    else
    {
        flags |= O_NONBLOCK;
    }
    if (fcntl(fd, F_SETFL, flags) == -1) 
    {
        return -1;
    }
    return 0;

    //方法二
    //int arg = (block ? 0:1);
    //return ::ioctl(fd, FIONBIO, &arg);
#endif
}

int SL_Socket_CommonAPI::socket_set_tcpnodelay(SL_SOCKET fd, int flag)
{
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
}

int SL_Socket_CommonAPI::socket_set_keepalive(SL_SOCKET fd, int flag)
{
    return setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&flag, sizeof(int));
}

int SL_Socket_CommonAPI::socket_set_keepalivevalue(SL_SOCKET fd, ulong keepalivetime, ulong keepaliveinterval)
{
#ifdef SOCKETLITE_OS_WINDOWS 
#if (_WIN32_WINNT>=0x0500)    //window 2000 or later version
    tcp_keepalive in_keepalive  = {0};
    tcp_keepalive out_keepalive = {0};
    ulong inlen  = sizeof(tcp_keepalive);
    ulong outlen = sizeof(tcp_keepalive);
    ulong bytesreturn = 0;
     
    //设置socket的keepalivetime,keepaliveinterval
    in_keepalive.onoff = 1;
    in_keepalive.keepalivetime = keepalivetime;
    in_keepalive.keepaliveinterval = keepaliveinterval;
                   
    //为选定的SOCKET设置Keep Alive，成功后SOCKET可通过KeepAlive自动检测连接是否断开 
    if ( SOCKET_ERROR == WSAIoctl(fd, SIO_KEEPALIVE_VALS,(LPVOID)&in_keepalive, inlen,(LPVOID)&out_keepalive, outlen, &bytesreturn, NULL, NULL) )
    {
        return -1;
    }
#endif
#elif defined(SOCKETLITE_OS_LINUX)
    ///* set first test time */ 
    //setsockopt(fd, SOL_TCP, TCP_KEEPIDLE , (const char *)&iIdleTime , sizeof(iIdleTime)); 
    ///* set test idle time */ 
    //setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (const char *)&iInterval, sizeof(iInterval)); 
    ///* set test count */ 
    //setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (const char *)&iCount, sizeof(iCount)); 
#endif
    return 0;
}

int SL_Socket_CommonAPI::socket_set_reuseaddr(SL_SOCKET fd, int flag)
{
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(int));
}

int SL_Socket_CommonAPI::socket_set_broadcast(SL_SOCKET fd, int flag)
{
    return setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char *)&flag, sizeof(int));
}

int SL_Socket_CommonAPI::socket_set_send_buffersize(SL_SOCKET fd, int sz)
{
    return setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&sz, sizeof(int));
}

int SL_Socket_CommonAPI::socket_get_send_buffersize(SL_SOCKET fd)
{
    int sz  = 0;
    int len = sizeof(int);
    if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&sz, (socklen_t *)&len) == SL_SOCKET_ERROR)
    {
        return -1;
    }
    return sz;
}

int SL_Socket_CommonAPI::socket_get_recv_buffersize(SL_SOCKET fd)
{
    int sz  = 0;
    int len = sizeof(int);
    if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&sz, (socklen_t *)&len) == SL_SOCKET_ERROR)
    {
        return -1;
    }
    return sz;
}

int SL_Socket_CommonAPI::socket_set_recv_buffersize(SL_SOCKET fd, int sz)
{
    return setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&sz, sizeof(int));
}

int SL_Socket_CommonAPI::socket_set_recv_timeout(SL_SOCKET fd, int timeout)
{
    return setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(int));
}

int SL_Socket_CommonAPI::socket_set_send_timeout(SL_SOCKET fd, int timeout)
{
    return setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(int));
}

int SL_Socket_CommonAPI::socket_set_linger(SL_SOCKET fd, int onoff, int linger_time)
{
    linger l;
    l.l_onoff  = onoff;
    l.l_linger = linger_time;
    return setsockopt(fd, SOL_SOCKET, SO_LINGER, reinterpret_cast<char *>(&l), sizeof(l));
}

int SL_Socket_CommonAPI::socket_set_dontlinger(SL_SOCKET fd, int onoff)
{
#ifdef SOCKETLITE_OS_WINDOWS
    return  setsockopt(fd, SOL_SOCKET, SO_DONTLINGER, reinterpret_cast<char *>(&onoff), sizeof(int));
#else
    return 0;
#endif
}

int SL_Socket_CommonAPI::socket_recv(SL_SOCKET fd, char *buf, int len, int flags, SL_OVERLAPPED *overlapped, int &error)
{
#ifdef SOCKETLITE_OS_WINDOWS
    long   bytes_recv = 0;
    WSABUF wsabuf;
    wsabuf.len = len;
    wsabuf.buf = (char *)buf;
    int ret = WSARecv(fd, &wsabuf, 1, (ulong *)&bytes_recv, (LPDWORD)&flags, overlapped, 0);
    if (ret < 0)
    {
        error = socket_get_lasterror_wsa();
        return ret;
    }
    return bytes_recv;
#else 
    int ret = recv(fd, buf, len, flags);
    if (ret < 0)
    {
        error = socket_get_lasterror_wsa();
    }
    return ret;
#endif
}

int SL_Socket_CommonAPI::socket_recvn(SL_SOCKET fd, char *buf, int len, int flags, SL_OVERLAPPED *overlapped, int &error)
{
    long bytes_recv = 0;
    int  ret;
    while (bytes_recv < len )
    {
        ret = recv(fd, buf+bytes_recv, len-bytes_recv, flags);
        if (ret < 0)
        {
            error = socket_get_lasterror_wsa();
            return bytes_recv;
        }
        else
        {
            bytes_recv += ret;
        }
    }
    return bytes_recv;
}

int SL_Socket_CommonAPI::socket_recvv(SL_SOCKET fd, SL_IOVEC *iov, int iovcnt, int flags, SL_OVERLAPPED *overlapped, int &error)
{
#ifdef SOCKETLITE_OS_WINDOWS
    long bytes_recv = 0;
    int ret = WSARecv(fd, (WSABUF *)iov, iovcnt, (ulong *)&bytes_recv, (LPDWORD)&flags, overlapped, 0);
    if (ret < 0)
    {
        error = socket_get_lasterror_wsa();
        return ret;
    }
    return bytes_recv;
#else
    int ret = readv(fd, iov, iovcnt);
    if (ret < 0)
    {
        error = socket_get_lasterror_wsa();
    }
    return ret;
#endif
}

int SL_Socket_CommonAPI::socket_send(SL_SOCKET fd, const char *buf, int len, int flags, SL_OVERLAPPED *overlapped, int *error)
{
#ifdef SOCKETLITE_OS_WINDOWS
    long   bytes_send = 0;
    WSABUF wsabuf;
    wsabuf.len = len;
    wsabuf.buf = (char *)buf;
    int ret = WSASend(fd, &wsabuf, 1, (ulong *)&bytes_send, flags, overlapped, 0);
    if (ret < 0)
    {
        if (NULL != error)
        {
            *error = socket_get_lasterror_wsa();
        }
        return ret;
    }
    return bytes_send;
#else 
    int ret = send(fd, buf, len, flags);
    if (ret < 0)
    {
        if (NULL != error)
        {
            *error = socket_get_lasterror_wsa();
        }
    }
    return ret;
#endif
}

int SL_Socket_CommonAPI::socket_sendn(SL_SOCKET fd, const char *buf, int len, int flags, SL_OVERLAPPED *overlapped, int *error)
{
    long bytes_send = 0;
    int  ret;
    while (bytes_send < len )
    {
        ret = send(fd, buf+bytes_send, len-bytes_send, flags);
        if (ret < 0)
        {
            if (NULL != error)
            {
                *error = socket_get_lasterror_wsa();
            }
            return bytes_send;
        }
        else
        {
            bytes_send += ret;
        }
    }
    return bytes_send;
}

int SL_Socket_CommonAPI::socket_sendv(SL_SOCKET fd, const SL_IOVEC *iov, int iovcnt, int flags, SL_OVERLAPPED *overlapped, int &error)
{
#ifdef SOCKETLITE_OS_WINDOWS
    long  bytes_send = 0;
    int ret = WSASend(fd, (WSABUF *)iov, iovcnt, (ulong *)&bytes_send, flags, overlapped, 0);
    if (ret < 0)
    {
        error = socket_get_lasterror_wsa();
        return ret;
    }
    return bytes_send;
#else
    int ret = writev(fd, iov, iovcnt);
    if (ret < 0)
    {
        error = socket_get_lasterror_wsa();
    }
    return ret;
#endif
}

int SL_Socket_CommonAPI::socket_recvfrom(SL_SOCKET fd, char *buf, int len, int flags, struct sockaddr *src_addr, int *addrlen, SL_OVERLAPPED *overlapped, int &error)
{
#ifdef SOCKETLITE_OS_WINDOWS
    long   bytes_recv = 0;
    WSABUF wsabuf;
    wsabuf.len = len;
    wsabuf.buf = (char *)buf;
    int ret = WSARecvFrom(fd, &wsabuf, 1, (ulong *)&bytes_recv, (LPDWORD)&flags, src_addr, addrlen, overlapped, 0);
    if (ret < 0)
    {
        error = socket_get_lasterror_wsa();
        return ret;
    }
    return bytes_recv;
#else 
    int ret = recvfrom(fd, buf, len, flags, src_addr, (socklen_t *)addrlen);
    if (ret < 0)
    {
        error = socket_get_lasterror_wsa();
    }
    return ret;
#endif
}

int SL_Socket_CommonAPI::socket_sendto(SL_SOCKET  fd, const char *buf, int len, int flags, const struct sockaddr *dest_addr, int addrlen, SL_OVERLAPPED *overlapped, int *error)
{
#ifdef SOCKETLITE_OS_WINDOWS
    long   bytes_send = 0;
    WSABUF wsabuf;
    wsabuf.len = len;
    wsabuf.buf = (char *)buf;
    int ret = WSASendTo(fd, &wsabuf, 1, (ulong *)&bytes_send, flags, dest_addr, addrlen, overlapped, 0);
    if (ret < 0)
    {
        if (NULL != error)
        {
            *error = socket_get_lasterror_wsa();
        }
        return ret;
    }
    return bytes_send;
#else 
    int ret = sendto(fd, buf, len, flags, dest_addr, addrlen);
    if (ret < 0)
    {
        if (NULL != error)
        {
            *error = socket_get_lasterror_wsa();
        }
    }
    return ret;
#endif
}

void SL_Socket_CommonAPI::util_sleep(uint timeout_ms)
{
#ifdef SOCKETLITE_OS_WINDOWS
    Sleep(timeout_ms);
#else
    //方法1
    //select();

    //方法2
    //usleep(timeout_ms * 1000);

    //方法3
    struct timespec ts;
    ts.tv_sec  = timeout_ms / 1000;
    ts.tv_nsec = (timeout_ms - ts.tv_sec * 1000) * 1000000L;
    nanosleep(&ts, 0);
#endif
}

void SL_Socket_CommonAPI::util_sleep_s(uint timeout_s)
{
#ifdef SOCKETLITE_OS_WINDOWS
    Sleep(timeout_s * 1000);
#else
    //方法1
    //select函数

    //方法2
    //sleep(timeout_s);

    //方法3
    //usleep(timeout_s * 10000000L);

    //方法4
    struct timespec ts;
    ts.tv_sec  = timeout_s;
    ts.tv_nsec = 0;
    nanosleep(&ts, NULL);
#endif
}

void SL_Socket_CommonAPI::util_sleep_ms(uint timeout_ms)
{
#ifdef SOCKETLITE_OS_WINDOWS
    Sleep(timeout_ms);
#else
    //方法1
    //select函数

    //方法2
    //usleep(timeout_ms * 1000);

    //方法3
    struct timespec ts;
    ts.tv_sec  = timeout_ms / 1000;
    ts.tv_nsec = (timeout_ms - ts.tv_sec * 1000) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}

void SL_Socket_CommonAPI::util_sleep_us(uint timeout_us)
{
#ifdef SOCKETLITE_OS_WINDOWS
    Sleep(timeout_us / 1000);
#else
    //方法1
    //select函数

    //方法2
    //usleep(timeout_us);

    //方法3
    struct timespec ts;
    ts.tv_sec  = timeout_us / 1000000L;
    ts.tv_nsec = (timeout_us - ts.tv_sec * 1000000L) * 1000;
    nanosleep(&ts, NULL);
#endif
}

void SL_Socket_CommonAPI::util_sleep_ns(uint64 timeout_ns)
{
#ifdef SOCKETLITE_OS_WINDOWS
    Sleep(timeout_ns / 1000000);
#else
    //方法1
    //select函数

    //方法2
    //usleep(timeout_us);

    //方法3
    struct timespec ts;
    ts.tv_sec  = timeout_ns / 1000000000L;
    ts.tv_nsec = timeout_ns - ts.tv_sec * 1000000000L;
    nanosleep(&ts, NULL);
#endif
}

int64 SL_Socket_CommonAPI::util_atoi64(const char *str)
{
    //方法1
    //_atoi64(atoll)

    //方法2
    //_strtoi64(strtoll)

#ifdef SOCKETLITE_OS_WINDOWS
    return _atoi64(str);
#else
    return atoll(str);
#endif
}

uint64 SL_Socket_CommonAPI::util_atoui64(const char *str)
{
#ifdef SOCKETLITE_OS_WINDOWS
    return _strtoui64(str, NULL, 10);
#else
    return strtoull(str, NULL, 10);
#endif
}

int SL_Socket_CommonAPI::util_i64toa(int64 i, char *str, int radix)
{   
#ifdef SOCKETLITE_OS_WINDOWS
    _i64toa(i, str, radix);
#else
    //lltoa(i, str, radix);
    sprintf(str, "%lld", i);
#endif
    return 0;
}

int SL_Socket_CommonAPI::util_ui64toa(uint64 i, char *str, int radix)
{
#ifdef SOCKETLITE_OS_WINDOWS
    _ui64toa(i, str, radix);
#else
    //ulltoa(i, str, radix);
    sprintf(str, "%llu", i);
#endif
    return 0;
}

int SL_Socket_CommonAPI::util_get_application_path(SL_TCHAR *path, int path_len)
{
    int pos = 0;
#ifdef SOCKETLITE_OS_WINDOWS
    int real_len = GetModuleFileName(NULL, path, path_len);
    for (pos=real_len; pos>0; --pos)
    {
        if (SL_TEXT('\\') == path[pos])
        {
            break;
        }
    }
#elif defined(SOCKETLITE_OS_LINUX)
    char proc_path[128] = {0};
    sprintf(proc_path, "/proc/%d/exe", getpid());
    int real_len = readlink(proc_path, path, path_len);
    for (pos=real_len; pos>0; --pos)
    {
        if ('/' == path[pos])
        {
            break;
        }
    }
#endif
    if (path_len > pos)
    {
        path[pos+1] = SL_TEXT('\0');
    }
    return pos;
}

int SL_Socket_CommonAPI::util_get_application_name(SL_TCHAR *app_name, int name_len)
{
    int real_len = 0;
#ifdef SOCKETLITE_OS_WINDOWS
    real_len = GetModuleFileName(NULL, app_name, name_len);
#elif defined(SOCKETLITE_OS_LINUX)
    char proc_path[128] = {0};
    sprintf(proc_path, "/proc/%d/exe", getpid());
    real_len = readlink(proc_path, app_name, name_len);
#endif
    if (name_len > real_len)
    {
        app_name[real_len+1] = SL_TEXT('\0');
    }
    return real_len;
}

uint64 SL_Socket_CommonAPI::util_time()
{
    return time(NULL);
}

uint64 SL_Socket_CommonAPI::util_time_s()
{
    return time(NULL);
}

uint64 SL_Socket_CommonAPI::util_time_ms()
{
#ifdef SOCKETLITE_OS_WINDOWS
    struct _timeb tb;
    _ftime_s(&tb);
    return (tb.time * 1000LL + tb.millitm);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL + tv.tv_usec / 1000LL);
#endif
}

uint64 SL_Socket_CommonAPI::util_time_us()
{
#ifdef SOCKETLITE_OS_WINDOWS
    struct _timeb tb;
    _ftime_s(&tb);
    return (tb.time * 1000000LL + tb.millitm * 1000LL);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000LL + tv.tv_usec);
#endif
}

uint64 SL_Socket_CommonAPI::util_time_ns()
{
#ifdef SOCKETLITE_OS_WINDOWS
    struct _timeb tb;
    _ftime_s(&tb);
    return (tb.time * 1000000000LL + tb.millitm * 1000000LL);
#else
    //方法1
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return (tp.tv_sec * 1000000000LL + tp.tv_nsec);

    ////方法2
    //struct timeval tv;
    //gettimeofday(&tv, NULL);
    //return (tv.tv_sec * 1000000000LL + tv.tv_usec * 1000LL);
#endif
}

int SL_Socket_CommonAPI::util_gettimeofday(struct timeval *tv, struct timezone *tz)
{
#ifdef SOCKETLITE_OS_WINDOWS
    struct _timeb tb;
    _ftime_s(&tb);

    tv->tv_sec  = tb.time;
    tv->tv_usec = tb.millitm * 1000L;
#else
    gettimeofday(tv, tz);
#endif

    return 0;
}

void SL_Socket_CommonAPI::util_timeradd(struct timeval *tvp, struct timeval *uvp, struct timeval *vvp)
{
    vvp->tv_sec  = tvp->tv_sec + uvp->tv_sec;
    vvp->tv_usec = tvp->tv_usec + uvp->tv_usec;
    if (vvp->tv_usec >= 1000000)
    {
        ++vvp->tv_sec;
        vvp->tv_usec -= 1000000;
    }
}

void SL_Socket_CommonAPI::util_timersub(struct timeval *tvp, struct timeval *uvp, struct timeval *vvp)
{
    vvp->tv_sec  = tvp->tv_sec - uvp->tv_sec;
    vvp->tv_usec = tvp->tv_usec - uvp->tv_usec;
    if (vvp->tv_usec < 0)
    {
        --vvp->tv_sec;
        vvp->tv_usec += 1000000;
    }
}

int SL_Socket_CommonAPI::util_timercmp(struct timeval *tvp, struct timeval *uvp)
{
    if (tvp->tv_sec > uvp->tv_sec)
    {
        return 1;
    }
    else
    {
        if (tvp->tv_sec < uvp->tv_sec)
        {
            return -1;
        }
        if (tvp->tv_usec > uvp->tv_usec)
        {
            return 1;
        }
        else if (tvp->tv_usec < uvp->tv_usec)
        {
            return -1;
        }
    }
    return 0;
}

bool SL_Socket_CommonAPI::util_timerisset(struct timeval *tvp)
{
    return (tvp->tv_sec || tvp->tv_usec);
}

void SL_Socket_CommonAPI::util_timerclear(struct timeval *tvp)
{
    tvp->tv_sec = tvp->tv_usec = 0;
}

uint64 SL_Socket_CommonAPI::util_gettickcount()
{
#ifdef SOCKETLITE_OS_WINDOWS
    //方法1 精度低些, 精确到 10-35ms
    //return GetTickCount();

    //方法2 精度高些, 精确到 1ms
    return timeGetTime();
#else  
    //方法1
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000LL + tp.tv_nsec / 1000000LL);

    //方法2
    //struct timeval tv;
    //gettimeofday(&tv, NULL);
    //return (tv.tv_sec*1000L + tv.tv_usec/1000);
#endif
}

uint64 SL_Socket_CommonAPI::util_process_clock_s()
{
#ifdef SOCKETLITE_OS_WINDOWS
    //方法1 精度低些, 精确到 10-35ms
    //return GetTickCount();

    //方法2 精度高些, 精确到 1ms
    return (timeGetTime() / 1000LL);
#else
    //方法1
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_sec;

    //方法2
    //struct timeval tv;
    //gettimeofday(&tv, NULL);
    //return tv.tv_sec;
#endif
}

uint64 SL_Socket_CommonAPI::util_process_clock_ms()
{
#ifdef SOCKETLITE_OS_WINDOWS
    //方法1 精度低些, 精确到 10-35ms
    //return GetTickCount();

    //方法2 精度高些, 精确到 1ms
    return timeGetTime();
#else
    //方法1
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000LL + tp.tv_nsec / 1000000LL);

    //方法2
    //struct timeval tv;
    //gettimeofday(&tv, NULL);
    //return (tv.tv_sec * 1000L + tv.tv_usec / 1000);
#endif
}

uint64 SL_Socket_CommonAPI::util_process_clock_us()
{
#ifdef SOCKETLITE_OS_WINDOWS
    //方法1 精度低些, 精确到 10-35ms
    //return (GetTickCount() * 1000);

    //方法2 精度高些, 精确到 1ms
    return (timeGetTime() * 1000LL);
#else
    //方法1
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000000LL + tp.tv_nsec / 1000LL);

    //方法2
    //struct timeval tv;
    //gettimeofday(&tv, NULL);
    //return (tv.tv_sec * 1000000LL + tv.tv_usec);
#endif
}

uint64 SL_Socket_CommonAPI::util_process_clock_ns()
{
#ifdef SOCKETLITE_OS_WINDOWS
    //方法1 精度低些, 精确到 10-35ms
    //return (GetTickCount() * 1000);

    //方法2 精度高些, 精确到 1ms
    return (timeGetTime() * 1000000LL);
#else
    //方法1
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000000000LL + tp.tv_nsec);

    //方法2
    //struct timeval tv;
    //gettimeofday(&tv, NULL);
    //return (tv.tv_sec * 1000000000LL + tv.tv_usec * 1000L);
#endif
}

long SL_Socket_CommonAPI::util_process_clock_difference(ulong end_clock, ulong start_clock)
{
    return (end_clock-start_clock);

//#ifdef SOCKETLITE_OS_WINDOWS
//    return (end_clock-start_clock);
//#else
//    static long clock_tick = 0;
//    if (clock_tick <= 0)
//    {
//        clock_tick = sysconf(_SC_CLK_TCK);
//    }
//    return (end_clock-start_clock)*1000/clock_tick;
//#endif

}


uint64 SL_Socket_CommonAPI::util_timestamp_s()
{
#ifdef SOCKETLITE_OS_WINDOWS
    //方法1 精度低些, 精确到 10-35ms
    //return GetTickCount();

    //方法2 精度高些, 精确到 1ms
    return (timeGetTime() / 1000LL);
#else
    //方法1
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_sec;

    //方法2
    //struct timeval tv;
    //gettimeofday(&tv, NULL);
    //return tv.tv_sec;
#endif
}

uint64 SL_Socket_CommonAPI::util_timestamp_ms()
{
#ifdef SOCKETLITE_OS_WINDOWS
    //方法1 精度低些, 精确到 10-35ms
    //return GetTickCount();

    //方法2 精度高些, 精确到 1ms
    return timeGetTime();
#else
    //方法1
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000LL + tp.tv_nsec / 1000000LL);

    //方法2
    //struct timeval tv;
    //gettimeofday(&tv, NULL);
    //return (tv.tv_sec * 1000L + tv.tv_usec / 1000);
#endif
}

uint64 SL_Socket_CommonAPI::util_timestamp_us()
{
#ifdef SOCKETLITE_OS_WINDOWS
    //方法1 精度低些, 精确到 10-35ms
    //return (GetTickCount() * 1000);

    //方法2 精度高些, 精确到 1ms
    return (timeGetTime() * 1000LL);
#else
    //方法1
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000000LL + tp.tv_nsec / 1000LL);

    //方法2
    //struct timeval tv;
    //gettimeofday(&tv, NULL);
    //return (tv.tv_sec * 1000000LL + tv.tv_usec);
#endif
}

uint64 SL_Socket_CommonAPI::util_timestamp_ns()
{
#ifdef SOCKETLITE_OS_WINDOWS
    //方法1 精度低些, 精确到 10-35ms
    //return (GetTickCount() * 1000);

    //方法2 精度高些, 精确到 1ms
    return (timeGetTime() * 1000000LL);
#else
    //方法1
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (tp.tv_sec * 1000000000LL + tp.tv_nsec);

    //方法2
    //struct timeval tv;
    //gettimeofday(&tv, NULL);
    //return (tv.tv_sec * 1000000000LL + tv.tv_usec * 1000L);
#endif
}

void* SL_Socket_CommonAPI::util_memcpy(void *dest, const void *src, size_t n)
{
    void *ret = dest;

    size_t unit_len = sizeof(int64);
    int64 *dest_align = (int64 *)dest;
    int64 *src_align  = (int64 *)src;
    for (; n >= unit_len; n -= unit_len)
    {   
        *dest_align++ = *src_align++;
    }
    
    char *dest_char = (char *)dest_align;
    char *src_char = (char *)src_align;
    for (unit_len=n, n=0; n<unit_len; ++n)
    {
        *dest_char++ = *src_char++;
    }
    return ret; 
}

//内存拷贝
void* SL_Socket_CommonAPI::util_memcpy_char(void *dest, const void *src, size_t n)
{
    void *ret = dest;

    char *char_dest = (char *)dest;
    char *char_src  = (char *)src;
    while (n--) 
    {
        *char_dest++ = *char_src++;
    }
    return ret;
}

bool SL_Socket_CommonAPI::util_is_exists_file(const SL_TCHAR *path_name)
{
    return SL_OS_API::futil_exists(path_name);
}

int SL_Socket_CommonAPI::util_get_file_size(const SL_TCHAR *file_name, unsigned long &file_size)
{
    return SL_OS_API::futil_filesize(file_name, file_size);
}

int SL_Socket_CommonAPI::util_get_errno()
{
    return errno;
}

//取得本地系统所采用的字节序
int8 SL_Socket_CommonAPI::util_get_byteorder()
{
    static int8 byte_order = 0;
    if (byte_order < 1)
    {
        const short n = 1;
        if (*(char *)&n)
        {  
            byte_order = __SOCKETLITE_LITTLE_ENDIAN;
        }
        else
        {
            byte_order = __SOCKETLITE_BIG_ENDIAN;
        }
    }
    return byte_order;
}

//64位整数的主机字节序转为网络字节序
int64 SL_Socket_CommonAPI::util_htonll(int64 host_int64)
{
#if SOCKETLITE_BYTE_ORDER == __SOCKETLITE_BIG_ENDIAN
    return net_int64;
#else
    #ifdef SOCKETLITE_OS_WINDOWS
        return ( (((unsigned long long)htonl(host_int64)) << 32) + htonl(host_int64 >> 32) );
    #else
        return bswap_64(host_int64);
    #endif
#endif
}

//64位整数的网络字节序转为主机字节序
int64 SL_Socket_CommonAPI::util_ntohll(int64 net_int64)
{
#if SOCKETLITE_BYTE_ORDER == __SOCKETLITE_BIG_ENDIAN
    return net_int64;
#else
#ifdef SOCKETLITE_OS_WINDOWS
    return ( (((unsigned long long)ntohl(net_int64)) << 32) + ntohl(net_int64 >> 32) );
#else
    return bswap_64(net_int64);
#endif
#endif
}

