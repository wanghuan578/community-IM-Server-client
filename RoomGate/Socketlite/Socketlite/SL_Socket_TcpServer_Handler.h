#ifndef SOCKETLITE_SOCKET_TCP_SERVER_HANDLER_H
#define SOCKETLITE_SOCKET_TCP_SERVER_HANDLER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif

#include "SL_Socket_CommonAPI.h"
#include "SL_Socket_Handler.h"
#include "SL_Socket_INET_Addr.h"
#include "SL_Socket_Source.h"

class SL_Socket_TcpServer_Handler : public SL_Socket_Handler
{
public:
    SL_Socket_TcpServer_Handler() 
    {
    }

    virtual ~SL_Socket_TcpServer_Handler() 
    {
    }

    virtual int do_accept(SL_SOCKET fd, SL_Socket_INET_Addr &sl_addr)
    {
        return 0;
    }

    inline int handle_close() 
    { 
        return -1; 
    }

    inline int handle_read()
    { 
        SL_Socket_INET_Addr sl_addr(socket_source_->get_local_addr()->is_ipv6());
        int addrlen = sl_addr.get_addr_size();

        //while存在的目的, 因为linux epoll边缘触发模式(EPOLLET)
        //监听的句柄被设置为EPOLLET,当同时多个连接建立的时候,
        //若只accept出一个连接进行处理,这样就可能导致后来的连接不能被及时处理,要等到下一次连接才会被激活
        SL_SOCKET fd;
        SL_Socket_Handler *handler;
        while (1)
        {   
            fd = SL_Socket_CommonAPI::socket_accept(socket_, sl_addr.get_addr(), &addrlen);
            if (fd != SL_INVALID_SOCKET)
            {
                if (do_accept(fd, sl_addr) < 0)
                {
                    SL_Socket_CommonAPI::socket_close(fd);
                    return 0;
                }
                handler = socket_source_->alloc_handler();
                if (handler != NULL)
                {
                    if (handler->handle_open(fd, socket_source_, socket_runner_) < 0)
                    {
                        SL_Socket_CommonAPI::socket_close(fd);
                        socket_source_->free_handler(handler);
                        return 0;
                    }
                    if (socket_source_->get_add_runner())
                    {
                        if (socket_source_->get_socket_runner()->add_handle(handler, SL_Socket_Handler::READ_EVENT_MASK) < 0)
                        {
                            socket_source_->free_handler(handler);
                        }
                    }
                }
                else
                {
                    SL_Socket_CommonAPI::socket_close(fd);
                }
            }
            else
            {
                break;
            }
        }
        return 0;
    }

    inline int handle_accept(SL_SOCKET fd, SL_Socket_INET_Addr &sl_addr)
    {
        if (do_accept(fd, sl_addr) < 0)
        {
            return -1;
        }

        SL_Socket_Handler *handler = socket_source_->alloc_handler();
        if (NULL != handler)
        {
            if (handler->handle_open(fd, socket_source_, socket_runner_) < 0)
            {
                socket_source_->free_handler(handler);
                return -2;
            }
            if (socket_source_->get_add_runner())
            {
                if (socket_source_->get_socket_runner()->add_handle(handler, SL_Socket_Handler::READ_EVENT_MASK) < 0)
                {
                    socket_source_->free_handler(handler);
                    return -3;
                }
            }
        }
        else
        {
            return -4;
        }
        return 0; 
    }
};

#endif

