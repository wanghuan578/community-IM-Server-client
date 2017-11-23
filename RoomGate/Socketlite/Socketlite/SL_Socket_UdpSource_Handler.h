#ifndef SOCKETLITE_SOCKET_UDP_SOURCE_HANDLER_H
#define SOCKETLITE_SOCKET_UDP_SOURCE_HANDLER_H

#include "SL_Socket_CommonAPI.h"
#include "SL_Socket_Handler.h"
#include "SL_Socket_INET_Addr.h"
#include "SL_ByteBuffer.h"
#include "SL_Socket_Source.h"

class SL_Socket_UdpSource_Handler : public SL_Socket_Handler
{
public:
    SL_Socket_UdpSource_Handler() 
    {
    }

    virtual ~SL_Socket_UdpSource_Handler() 
    {
    }

    int write_data(const char *buf, int len, SL_Socket_INET_Addr &dest_addr)
    {
        return SL_Socket_CommonAPI::socket_sendto(socket_, buf, len, 0, dest_addr.get_addr(), dest_addr.get_addr_size());
    }

    virtual int handle_read(const char *buf, int len, SL_Socket_INET_Addr &src_addr) 
    { 
        return 0;
    }

protected:
    inline int handle_read()
    {
        SL_Socket_INET_Addr src_addr(socket_source_->get_local_addr()->is_ipv6());
        sockaddr    *addr        = src_addr.get_addr();
        int         addrlen      = src_addr.get_addr_size();
        int         raw_buf_size = socket_runner_->get_recv_buffer_size();
        char        *raw_buf     = socket_runner_->get_recv_buffer();

        int error = 0;
        int ret = SL_Socket_CommonAPI::socket_recvfrom(socket_, raw_buf, raw_buf_size, 0, addr, &addrlen, NULL, error);
        if (ret > 0)
        {
            handle_read(raw_buf, ret, src_addr);
        }
        return 0;
    }

};

#endif

