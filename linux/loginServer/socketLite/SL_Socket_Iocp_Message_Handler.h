#ifndef SOCKETLITE_SOCKET_IOCP_MESSAGE_HANDLER_H
#define SOCKETLITE_SOCKET_IOCP_MESSAGE_HANDLER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#include "SL_ByteBuffer.h"
#include "SL_Socket_Iocp_Handler.h"
#include "SL_Socket_Message_Handler.h"

#ifdef SOCKETLITE_OS_WINDOWS

//SL_Socket_Iocp_Message_Handler所处理数据包的格式: 数据包长度+数据包内容
//类似于结构
//struct SL_Message 
//{
//    ushort len; 
//    char   content[0];
//}

class SL_Socket_Iocp_Message_Handler : public SL_Socket_Iocp_Handler
{
public:
    SL_Socket_Iocp_Message_Handler()
        : need_len_(0)
        , last_error_msglen_(0)
        , last_left_(0)
    {
    }

    virtual ~SL_Socket_Iocp_Message_Handler()
    {
    }

    virtual int handle_open(SL_SOCKET fd, SL_Socket_Source *socket_source, SL_Socket_Runner *socket_runner)
    {
        SL_Socket_Iocp_Handler::handle_open(fd, socket_source, socket_runner);
        msg_buffer_.reserve(socket_source_->get_msgbuffer_size());
        msg_buffer_.reset();
        need_len_           = 0;
        last_error_msglen_  = 0;
        last_left_          = 0;
        return 0;
    }

    //接收到完整消息时,回调此函数
    virtual int handle_message(const char *msg, int len)
    {
        return 0;
    }

    int handle_read();
    int write_message(const char *msg, int len);

protected:
    SL_ByteBuffer   msg_buffer_;            //消息缓存
    int             need_len_;              //组成一个消息包，还需要长度
    int             last_error_msglen_;     //组包出错时数据包长度
    int8            last_left_;             //当收到的数据长度小于消息len域所占长度时有效，一般为0
};

#endif

#endif

