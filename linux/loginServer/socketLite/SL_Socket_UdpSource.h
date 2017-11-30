#ifndef SOCKETLITE_SOCKET_UDP_SOURCE_H
#define SOCKETLITE_SOCKET_UDP_SOURCE_H

#include "SL_Config.h"
#include "SL_Socket_Source.h"
#include "SL_Socket_Runner.h"
#include "SL_Socket_INET_Addr.h"
#include "SL_Socket_UdpSource_Handler.h"
#include "SL_Thread_Group.h"

template <typename TUdpSourceHandler>
class SL_Socket_UdpSource : public SL_Socket_Source
{
public:
    SL_Socket_UdpSource()
    {
        set_config();
    }

    virtual ~SL_Socket_UdpSource()
    {
        close();
    }

    TUdpSourceHandler* get_socket_handler()
    {
        return &udpsource_handler_;
    }

    SL_Socket_INET_Addr* get_local_addr()
    {
        return &local_addr_;
    }

    int set_config(bool is_recv_thread=false, uint recvbuffer_size=32768)
    {
        is_recv_thread_  = is_recv_thread;
        recvbuffer_size_ = recvbuffer_size;
        return 0;
    }

    int set_interface(SL_Socket_Runner *socket_runner)
    {
        socket_runner_   = socket_runner;
        return 0;
    }

    int open(ushort local_port=0, const char *local_name=NULL, bool is_bind_localaddr=true, bool is_ipv6=false, int recv_thread_num=1)
    {
        int ret = 0;
        int address_family = is_ipv6 ? AF_INET6:AF_INET;
        SL_SOCKET fd = SL_Socket_CommonAPI::socket_open(address_family, SOCK_DGRAM, IPPROTO_UDP);
        if (SL_INVALID_SOCKET == fd)
        {
            return -1;
        }
        if (udpsource_handler_.handle_open(fd, this, socket_runner_) < 0)
        {
            ret = -2;
            goto EXCEPTION_EXIT_PROC;
        }
        if (is_bind_localaddr)
        {
            ret = local_addr_.set(local_name, local_port, is_ipv6);
            if (ret < 0)
            {
                ret = -3;
                goto EXCEPTION_EXIT_PROC;
            }
            SL_Socket_CommonAPI::socket_set_reuseaddr(fd, 1);
            ret = SL_Socket_CommonAPI::socket_bind(fd, local_addr_.get_addr(), local_addr_.get_addr_size());
            if (ret < 0)
            {
                ret = -4;
                goto EXCEPTION_EXIT_PROC;
            }
            if (is_recv_thread_)
            {
                SL_Socket_CommonAPI::socket_set_block(fd, true);
                if (recv_thread_group_.start(recv_proc, this, recv_thread_num, recv_thread_num) < 0)
                {
                    ret = -5;
                    goto EXCEPTION_EXIT_PROC;
                }
                socket_handler_ = &udpsource_handler_;
                return 0;
            }
        }
        if (NULL != socket_runner_)
        {
            is_recv_thread_ = false;
            if (socket_runner_->add_handle(&udpsource_handler_, SL_Socket_Handler::READ_EVENT_MASK) < 0)
            {
                ret = -6;
                goto EXCEPTION_EXIT_PROC;
            }
        }
        socket_handler_ = &udpsource_handler_;
        return 0;

EXCEPTION_EXIT_PROC:
        udpsource_handler_.set_socket(SL_INVALID_SOCKET);
        SL_Socket_CommonAPI::socket_close(fd);
        return ret;
    }

    int close()
    {
        if (NULL != socket_handler_)
        {
            if (is_recv_thread_)
            {
                recv_thread_group_.stop();
            }
            else
            {
                socket_runner_->remove_handle(&udpsource_handler_);
            }
            SL_Socket_CommonAPI::socket_close(udpsource_handler_.get_socket());
            udpsource_handler_.set_socket(SL_INVALID_SOCKET);
            udpsource_handler_.handle_close();
            socket_handler_ = NULL;
        }
        return 0;
    }

private:
#ifdef SOCKETLITE_OS_WINDOWS
    static unsigned int WINAPI recv_proc(void *arg)
#else
    static void* recv_proc(void *arg)
#endif
{
    SL_Socket_UdpSource *udpsource = (SL_Socket_UdpSource*)arg;
    SL_Socket_UdpSource_Handler *udpsource_handler = &(udpsource->udpsource_handler_);
    SL_SOCKET udp_fd = udpsource->udpsource_handler_.get_socket();

    SL_Socket_INET_Addr src_addr(udpsource->get_local_addr()->is_ipv6());
    sockaddr      *addr    = src_addr.get_addr();
    int           addrlen  = src_addr.get_addr_size();

    int           raw_buf_size = udpsource->get_recvbuffer_size();
    SL_ByteBuffer buf(raw_buf_size);
    char          *raw_buf = buf.buffer();

    int ret = 0;
    int error = 0;
    while (1)
    {
        if (!udpsource->recv_thread_group_.get_running())
        {
            break;
        }
        ret = SL_Socket_CommonAPI::socket_recvfrom(udp_fd, raw_buf, raw_buf_size, 0, addr, &addrlen, NULL, error);
        if (ret > 0)
        {
            udpsource_handler->handle_read(raw_buf, ret, src_addr);
        }
    }
    return 0;
}

protected:
    bool is_recv_thread_;
    SL_Thread_Group     recv_thread_group_; //多线程接收数据,提高并发性
    SL_Socket_INET_Addr local_addr_;
    TUdpSourceHandler   udpsource_handler_;
};

#endif

