#include "SL_Socket_Iocp_Message_Handler.h"
#include "SL_Socket_CommonAPI.h"
#include "SL_Socket_Source.h"

#ifdef SOCKETLITE_OS_WINDOWS

int SL_Socket_Iocp_Message_Handler::handle_read()
{
    char *pos               = per_io_data_.data_buffer.data();
    int  pos_size           = per_io_data_.data_buffer.data_size();
    int  msg_byteorder      = socket_source_->get_msg_byteorder();
    int  msglen_bytes       = socket_source_->get_msglen_bytes();
    int  msgbuffer_size     = socket_source_->get_msgbuffer_size();
    int  recv_buffer_size   = per_io_data_.data_buffer.buffer_size();
    int  msglen;

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
        {//nedd_size_>0
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

    return 1;
}

int SL_Socket_Iocp_Message_Handler::write_message(const char *msg, int len)
{
    int msglen_bytes = socket_source_->get_msglen_bytes();
    int datalen = msglen_bytes + len;
    if (datalen > msg_buffer_.buffer_size())
    {
        return -1;
    }

    SL_ByteBuffer buf;
    switch (msglen_bytes)
    {
    case 2:
        {
            short i = datalen;
            buf.reserve(datalen);
            buf.write(i);
        }
        break;
    case 1:
        {
            char i = datalen;
            buf.reserve(datalen);
            buf.write(i);
        }
        break;
    case 4:
        {
            buf.reserve(datalen);
            buf.write(datalen);
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

#endif

