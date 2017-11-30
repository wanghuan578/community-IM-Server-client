#ifndef SOCKETLITE_SOCKET_COMMON_API_H
#define SOCKETLITE_SOCKET_COMMON_API_H

#include "SL_Config.h"
#ifdef SOCKETLITE_OS_WINDOWS
    #include <io.h>
    #include "Shlwapi.h"
    #pragma comment(lib, "shlwapi.lib")

    /* timeb.h is actually xsi legacy functionality */
    #include <sys/timeb.h>

    //windows多媒体库(主要使用其中timeGetTime())
    #pragma comment(lib, "Winmm.lib")

    //windows视频库VFW
    //#include <Vfw.h>
    //#pragma comment(lib, "Vfw32.lib")
#else
    #include <sys/time.h>
    #include <sys/ioctl.h>
#endif

class SOCKETLITE_API SL_Socket_CommonAPI
{
private:
    SL_Socket_CommonAPI();
    ~SL_Socket_CommonAPI();
    
public:
    //socket_init 参数timer_resolution含义： 1) =0 自动设置OS允许定时精度的最小值 2) >0手动设置定时精度(精度范围: 1-5 ms)
    static int          socket_init(int version_high, int version_low, int timer_resolution_ms=0);
    static int          socket_fini();
    static int          socket_get_lasterror();
    static int          socket_get_lasterror_wsa();
    static int          socket_get_error(SL_SOCKET fd);

    static SL_SOCKET    socket_open(int address_family, int type, int protocol);
    static int          socket_close(SL_SOCKET fd);
    static int          socket_shutdown(SL_SOCKET fd, int how);

    static SL_SOCKET    socket_accept(SL_SOCKET fd, struct sockaddr *addr, int *addrlen);
    static int          socket_bind(SL_SOCKET fd, const struct sockaddr *addr, int addrlen);
    static int          socket_listen(SL_SOCKET fd, int backlog);
    static int          socket_connect(SL_SOCKET fd, const struct sockaddr *addr, int addrlen);
    static int          socket_connect(SL_SOCKET fd, const struct sockaddr *addr, int addrlen, timeval *timeout);

    static int          socket_set_block(SL_SOCKET fd, bool block);
    static int          socket_set_tcpnodelay(SL_SOCKET fd, int flag);
    static int          socket_set_keepalive(SL_SOCKET fd, int flag);
    static int          socket_set_keepalivevalue(SL_SOCKET fd, ulong keepalivetime=7200000, ulong keepaliveinterval=1000);
    static int          socket_set_reuseaddr(SL_SOCKET fd, int flag);
    static int          socket_set_broadcast(SL_SOCKET fd, int flag);
    static int          socket_set_recv_buffersize(SL_SOCKET fd, int sz);
    static int          socket_set_send_buffersize(SL_SOCKET fd, int sz);
    static int          socket_set_recv_timeout(SL_SOCKET fd, int timeout);
    static int          socket_set_send_timeout(SL_SOCKET fd, int timeout);
    static int          socket_set_linger(SL_SOCKET fd, int onoff, int linger_time);
    static int          socket_set_dontlinger(SL_SOCKET fd, int onoff);
    static int          socket_get_recv_buffersize(SL_SOCKET fd);
    static int          socket_get_send_buffersize(SL_SOCKET fd);

    static int          socket_recv(SL_SOCKET fd, char *buf, int len, int flags, SL_OVERLAPPED *overlapped, int &error);
    static int          socket_recvn(SL_SOCKET fd, char *buf, int len, int flags, SL_OVERLAPPED *overlapped, int &error);
    static int          socket_recvv(SL_SOCKET fd, SL_IOVEC *iov, int iovcnt, int flags, SL_OVERLAPPED *overlapped, int &error);
    static int          socket_send(SL_SOCKET fd, const char *buf, int len, int flags=0, SL_OVERLAPPED *overlapped=NULL, int *error=NULL);
    static int          socket_sendn(SL_SOCKET fd, const char *buf, int len, int flags=0, SL_OVERLAPPED *overlapped=NULL, int *error=NULL);
    static int          socket_sendv(SL_SOCKET fd, const SL_IOVEC *iov, int iovcnt, int flags, SL_OVERLAPPED *overlapped, int &error);

    static int          socket_recvfrom(SL_SOCKET fd, char *buf, int len, int flags, struct sockaddr *src_addr, int *addrlen, SL_OVERLAPPED *overlapped, int &error);
    static int          socket_sendto(SL_SOCKET fd, const char *buf, int len, int flags, const struct sockaddr *dest_addr, int addrlen, SL_OVERLAPPED *overlapped=NULL, int *error=NULL);

    static void         util_sleep(uint timeout_ms);        //sleep以毫秒为单位
    static void         util_sleep_s(uint timeout_s);       //sleep以秒为单位
    static void         util_sleep_ms(uint timeout_ms);     //sleep以毫秒为单位
    static void         util_sleep_us(uint timeout_us);     //sleep以微秒为单位
    static void         util_sleep_ns(uint64 timeout_ns);   //sleep以纳秒为单位

    static int64        util_atoi64(const char *str);
    static uint64       util_atoui64(const char *str);
    static int          util_i64toa(int64 i, char *str, int radix);
    static int          util_ui64toa(uint64 i, char *str, int radix);
    static int          util_get_application_path(SL_TCHAR *app_path, int len);
    static int          util_get_application_name(SL_TCHAR *app_name, int len);

    //取得当前系统时间(自公元1970/1/1 00:00:00以来经过秒数)
    static uint64       util_time();
    //取得当前系统时间(自公元1970/1/1 00:00:00以来经过秒数)
    static uint64       util_time_s();
    //取得当前系统时间(自公元1970/1/1 00:00:00以来经过毫秒)
    static uint64       util_time_ms();
    //取得当前系统时间(自公元1970/1/1 00:00:00以来经过微秒)
    static uint64       util_time_us();
    //取得当前系统时间(自公元1970/1/1 00:00:00以来经过纳秒)
    static uint64       util_time_ns();
    //取得当前系统时间(自公元1970/1/1 00:00:00以来经过秒数,可以精确到微秒)
    static int          util_gettimeofday(struct timeval *tv, struct timezone *tz);

    //时间操作函数
    static void         util_timeradd(struct timeval *tvp, struct timeval *uvp, struct timeval *vvp);
    static void         util_timersub(struct timeval *tvp, struct timeval *uvp, struct timeval *vvp);
    static bool         util_timerisset(struct timeval *tvp);
    static void         util_timerclear(struct timeval *tvp);

    //时间比较函数
    //The return value for each of these functions indicates the lexicographic relation of tvp to uvp
    //<0 tvp less than uvp
    //=0 tvp identical to uvp
    //>0 tvp greater than uvp
    static int          util_timercmp(struct timeval *tvp, struct timeval *uvp);

    //取得进程时钟滴答(不受修改系统时钟影响/调整系统时间无关 毫秒:ms 只能用于计时)
    static uint64       util_gettickcount();
    //取得进程时钟滴答(不受修改系统时钟影响/调整系统时间无关 秒:s 只能用于计时)
    static uint64       util_process_clock_s();
    //取得进程时钟滴答(不受修改系统时钟影响/调整系统时间无关 毫秒:ms 只能用于计时)
    static uint64       util_process_clock_ms();
    //取得进程时钟滴答(不受修改系统时钟影响/调整系统时间无关 微秒:us 只能用于计时)
    static uint64       util_process_clock_us();
    //取得进程时钟滴答(不受修改系统时钟影响/调整系统时间无关 纳秒:ns 只能用于计时)
    static uint64       util_process_clock_ns();

    //两个进程时钟差
    static long         util_process_clock_difference(ulong end, ulong start);

    //取得当前时间戳(不受修改系统时钟影响/调整系统时间无关 毫秒:ms 只能用于计时)
    static uint64       util_timestamp();
    //取得当前时间戳(不受修改系统时钟影响/调整系统时间无关 毫秒:s 只能用于计时)
    static uint64       util_timestamp_s();
    //取得当前时间戳(不受修改系统时钟影响/调整系统时间无关 毫秒:ms 只能用于计时)
    static uint64       util_timestamp_ms();
    //取得当前时间戳(不受修改系统时钟影响/调整系统时间无关 微秒:us 只能用于计时)
    static uint64       util_timestamp_us();
    //取得当前时间戳(不受修改系统时钟影响/调整系统时间无关 纳秒:ns 只能用于计时)
    static uint64       util_timestamp_ns();

    //内存拷贝(使用内存对齐)
    static void*        util_memcpy(void *dest, const void *src, size_t n);

    //内存拷贝
    static void*        util_memcpy_char(void *dest, const void *src, size_t n);

    //检测文件是否存在
    static bool         util_is_exists_file(const SL_TCHAR *path_name);

    //取得文件大小
    static int          util_get_file_size(const SL_TCHAR *file_name, unsigned long &file_size);

    //取得错误号
    static int          util_get_errno();

    //取得本地系统所采用的字节序
    static int8         util_get_byteorder();

    //64位整数的主机字节序转为网络字节序
    static int64        util_htonll(int64 host_int64);

    //64位整数的网络字节序转为主机字节序
    static int64        util_ntohll(int64 net_int64);

private:
    static unsigned int timer_resolution_ms_;       //定时精度(毫秒) windows下1-5 ms
};

#endif

