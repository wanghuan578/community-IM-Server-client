#ifndef SOCKETLITE_SOCKET_IOCP_HANDLER_H
#define SOCKETLITE_SOCKET_IOCP_HANDLER_H

#include "SL_Config.h"
#include "SL_Socket_CommonAPI.h"
#include "SL_Socket_Handler.h"
#include "SL_Socket_Source.h"
#include "SL_ByteBuffer.h"

#ifdef SOCKETLITE_OS_WINDOWS

class SL_Socket_Iocp_Handler : public SL_Socket_Handler
{
public:
    enum OPERTYPE
    {
        RECV_POSTED     = 1,
        SEND_POSTED     = 2,
        ACCEPT_POSTED   = 3
    };

    //单I/O操作数据
    struct PER_IO_DATA
    {
        //重叠结构
        OVERLAPPED    overlapped;

        //操作类型表示
        OPERTYPE      opertype;

        //缓冲区
        WSABUF        buffer;
        SL_ByteBuffer data_buffer;
    };

    SL_Socket_Iocp_Handler() 
    {
    }

    virtual ~SL_Socket_Iocp_Handler() 
    {
    }

    virtual int handle_open(SL_SOCKET fd, SL_Socket_Source *socket_source, SL_Socket_Runner *socket_runner)
    {
        SL_Socket_Handler::handle_open(fd, socket_source, socket_runner);
        per_io_data_.data_buffer.reserve(socket_source_->get_recvbuffer_size());
        per_io_data_.data_buffer.reset();
        per_io_data_.buffer.buf = per_io_data_.data_buffer.buffer();
        per_io_data_.buffer.len = per_io_data_.data_buffer.buffer_size();
        per_io_data_.opertype   = RECV_POSTED;
        return 0;
    }

    //投递接收操作
    virtual int post_recv()
    {
        //重置IO操作数据
        DWORD  flag = 0;
        DWORD  recv_byte = 0;
        ZeroMemory(&(per_io_data_.overlapped), sizeof(OVERLAPPED));

        //提交WSARecv请求
        int ret = WSARecv(socket_, &(per_io_data_.buffer), 1, &recv_byte, &flag, &(per_io_data_.overlapped), NULL);
        if (ret == SOCKET_ERROR)
        {
            ret = WSAGetLastError();
            if (ret != WSA_IO_PENDING)
            {//接收失败,一律Socket断开处理
                return -1;
            }
        }
        return 0;
    }

    //投递发送事件
    virtual int post_send()
    {
        return 0;
    }

protected:
    PER_IO_DATA per_io_data_;
};

#endif

#endif

