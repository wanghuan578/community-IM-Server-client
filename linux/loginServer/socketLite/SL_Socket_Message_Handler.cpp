#include "SL_Socket_Message_Handler.h"
#include "SL_Socket_CommonAPI.h"
#include "SL_Socket_Source.h"

int SL_Socket_Message_Handler::handle_read()
{
    int  msglen_bytes     = socket_source_->get_msglen_bytes();
    int  msgbuffer_size   = socket_source_->get_msgbuffer_size();
    int  recv_buffer_size = socket_runner_->get_recv_buffer_size();
    char *recv_buffer     = socket_runner_->get_recv_buffer();

    char *pos;
    int  pos_size;
    int  msglen;
    int  ret;
    int  error_id;

    while (1)
    {
        ret = SL_Socket_CommonAPI::socket_recv(socket_, recv_buffer, recv_buffer_size, 0, NULL, error_id);
        if (ret > 0)
        {
            pos = recv_buffer;
            pos_size = ret;
            do
            {                  
                if (0 == need_len_)
                {
                    if (0 == last_left_)
                    {
                        if (pos_size >= msglen_bytes)
                        {
                            msglen = socket_source_->get_msglen_proc_(pos, pos_size);
                            if ( (msglen <= 0) || (msglen > msgbuffer_size) )
                            {//消息大小无效或超过消息缓冲大小
                                last_errno_ = SL_Socket_Handler::ERROR_CLOSE_RECV;
                                last_error_msglen_ = msglen;
                                return -1;
                            }
                            if (pos_size >= msglen)
                            {
                                if (handle_message(pos, msglen) < 0)
                                {
                                    last_errno_ = SL_Socket_Handler::ERROR_CLOSE_ACTIVE;
                                    return -1;
                                }
                                pos      += msglen;
                                pos_size -= msglen;
                            }
                            else
                            {
                                msg_buffer_.write(pos, pos_size);
                                need_len_ = msglen - pos_size;
                                break;
                            }
                        }
                        else
                        {
                            msg_buffer_.write(pos, pos_size);
                            last_left_ = pos_size;
                            break;
                        }
                    }
                    else
                    {//last_left_>0
                        if (last_left_ + pos_size < msglen_bytes)
                        {
                            msg_buffer_.write(pos, pos_size);
                            last_left_ += pos_size;
                            break;
                        }
                        else
                        {
                            msg_buffer_.write(pos, msglen_bytes-last_left_);
                            pos      += (msglen_bytes-last_left_);
                            pos_size -= (msglen_bytes-last_left_);
                            msglen    = socket_source_->get_msglen_proc_(msg_buffer_.data(), msglen_bytes);
                            if ( (msglen <= 0) || (msglen > msgbuffer_size) )
                            {//消息大小无效或超过消息缓冲大小
                                last_errno_ = SL_Socket_Handler::ERROR_CLOSE_RECV;
                                last_error_msglen_ = msglen;
                                return -1;
                            }
                            last_left_ = 0;

                            if (pos_size + msglen_bytes >= msglen)
                            {
                                msg_buffer_.write(pos, msglen-msglen_bytes);
                                if (handle_message(msg_buffer_.data(), msglen) < 0)
                                {
                                    last_errno_ = SL_Socket_Handler::ERROR_CLOSE_ACTIVE;
                                    return -1;
                                }
                                msg_buffer_.reset();
                                pos      += (msglen-msglen_bytes);
                                pos_size -= (msglen-msglen_bytes);
                            }
                            else
                            {
                                msg_buffer_.write(pos, pos_size);
                                need_len_ = msglen-pos_size-msglen_bytes;
                                break;
                            }
                        }
                    }
                }
                else
                {//need_size_>0
                    if (pos_size >= need_len_)
                    {
                        msg_buffer_.write(pos, need_len_);
                        if (handle_message(msg_buffer_.data(), msg_buffer_.data_size()) < 0)
                        {
                            last_errno_ = SL_Socket_Handler::ERROR_CLOSE_ACTIVE;
                            return -1;
                        }
                        msg_buffer_.reset();
                        pos       += need_len_;
                        pos_size  -= need_len_;
                        need_len_  = 0;
                    }
                    else
                    {
                        msg_buffer_.write(pos, pos_size);
                        need_len_ -= pos_size;
                        break;
                    }
                }
            } while (pos_size > 0);

            if (ret < recv_buffer_size)
            {
                return 1;
            }
        }
        else if (ret == 0)
        {//连接已关闭
            last_errno_ = SL_Socket_Handler::ERROR_CLOSE_PASSIVE;
            return -1;
        }
        else
        {//ret < 0: 出现异常(如连接已关闭)
            if ( (SL_EAGAIN == error_id) || (SL_EWOULDBLOCK == error_id) || (SL_EINTR == error_id) )
            {//非阻塞模式下正常情况
                return 0;
            }
            last_errno_ = error_id;
            return -1;
        }
    }

    return 1;
}

int SL_Socket_Message_Handler::write_message(const char *msg, int len)
{
    int msglen_bytes = socket_source_->get_msglen_bytes();
    int data_len = msglen_bytes + len;
    if (data_len > (int)msg_buffer_.buffer_size())
    {
        return -1;
    }

    SL_ByteBuffer buf;
    switch (msglen_bytes)
    {
    case 2:
        {
            short i = data_len;
            buf.reserve(data_len);
            buf.write(i);
        }
        break;
    case 1:
        {
            char i = data_len;
            buf.reserve(data_len);
            buf.write(i);
        }
        break;
    case 4:
        {
            buf.reserve(data_len);
            buf.write(data_len);
        }
        break;
    default:
        return -1;
    }
    buf.write(msg, len);
    if (socket_source_->get_socket_handler() == this)
    {
        return socket_source_->send(buf.data(), buf.data_size(), 0);
    }
    return SL_Socket_CommonAPI::socket_send(socket_, buf.data(), buf.data_size());
}

//目前没有考虑跨平台的字节顺序
//一般字节顺序:BIG Endian，LITTLE Endian
//1)取得消息长度
int SL_Socket_Message_Handler::get_msglen(const char *msg, int len, int msglen_bytes, int byteorder)
{
    int msglen;
    switch (msglen_bytes)
    {
    case 2:
        msglen = *((int16*)msg);
        break;
    case 1:
        msglen = *msg;
        break;
    case 4:
        msglen = *((int32*)msg);
        break;
    default:
        return -1;
    }
    return msglen;
}

//2)设置消息长度
int SL_Socket_Message_Handler::set_msglen(char *msg, int len, int msglen_bytes, int byteorder)
{
    switch (msglen_bytes)
    {
    case 2:
        *((int16*)msg) = len;
        break;
    case 1:
        *msg = len;
        break;
    case 4:
        *((int32*)msg) = len;
        break;
    default:
        return -1;
    }
    return 0;
}

