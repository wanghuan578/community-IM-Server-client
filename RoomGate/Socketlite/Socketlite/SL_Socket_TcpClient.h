#ifndef SOCKETLITE_SOCKET_TCP_CLIENT_H
#define SOCKETLITE_SOCKET_TCP_CLIENT_H

#include <vector>
#include <string>
#include "SL_Config.h"
#include "SL_ByteBuffer.h"
#include "SL_Socket_CommonAPI.h"
#include "SL_Socket_INET_Addr.h"
#include "SL_Socket_Source.h"
#include "SL_Sync_SpinMutex.h"
#include "SL_Sync_Guard.h"

template <typename THandler, typename TObjectPool>
class SL_Socket_TcpClient : public SL_Socket_Source
{
public:
    SL_Socket_TcpClient()
        : local_port_(0)
        , is_bind_localaddr_(false)
        , connect_serveraddr_pos_(0)
        , connect_timeout_(0)
        , is_connected_(false)
        , is_autoconnect_(false)
    {
        serveraddr_list_.reserve(3);
        set_config();
    }

    virtual ~SL_Socket_TcpClient()
    {
        clear();
    }

    int open(const char *server_name, ushort server_port, const char *local_name=NULL, ushort local_port=0, bool is_bind_localaddr=false, bool is_ipv6=false)
    {
        local_name_         = local_name ? local_name:"";
        local_port_         = local_port;
        is_bind_localaddr_  = is_bind_localaddr;
        serveraddr_list_.clear();
        add_serveraddr(server_name, server_port, is_ipv6);
        connect_serveraddr_pos_ = serveraddr_list_.size()-1;
        if (!is_autoconnect_)
        {
            return connect();
        }
        return 0;
    }

    int open()
    {
        if (!is_autoconnect_)
        {
            return connect();
        }
        return -1;
    }

    int close()
    {
		mutex_.lock();
        is_autoconnect_ = false;
        is_connected_   = false;
        if (NULL != socket_handler_)
        {
            if (is_add_runner_)
            {
                socket_runner_->remove_handle(socket_handler_);
            }
            socket_handler_->close();
			if (is_free_self_handler_)
			{
				free_handler(socket_handler_);
			}
            socket_handler_ = NULL;
        }
		mutex_.unlock();
        return 0;
    }

    int clear()
    {
		mutex_.lock();
        is_autoconnect_ = false;
        is_connected_   = false;
        if (NULL != socket_handler_)
        {
            if (is_add_runner_)
            {
                socket_runner_->remove_handle(socket_handler_);
            }
            socket_handler_->close();
			if (is_free_self_handler_)
			{
				free_handler(socket_handler_);
			}
            socket_handler_ = NULL;
        }
        serveraddr_list_.clear();
		mutex_.unlock();
        return 0;
    }

    int recv(char *buf, int len, int flags)
    {
        if (!is_connected_)
        {
            return 0;
        }

        int error_id = 0;
        int recv_len = SL_Socket_CommonAPI::socket_recv(socket_handler_->get_socket(),buf, len, flags, NULL, error_id);
        if (recv_len == 0)
        {
            is_connected_ = false;
        }
        else if (recv_len < 0)
        {
            if ( (SL_EAGAIN == error_id) || (SL_EWOULDBLOCK == error_id) || (SL_EINTR == error_id) )
            {//非阻塞模式下正常情况
                return 0;
            }
            is_connected_ = false;
        }
        return recv_len;
    }

    int send(const char *buf, int len, int flags)
    {
        if (!is_connected_)
        {
            return 0;
        }

        int error_id = 0;
        int send_len = SL_Socket_CommonAPI::socket_send(socket_handler_->get_socket(), buf, len, flags, NULL, &error_id);
        if (send_len <= 0)
        {
            if ( (0 == error_id) || (SL_EAGAIN == error_id)|| (SL_EWOULDBLOCK == error_id) || (SL_IO_PENDING == error_id) || (SL_ENOBUFS == error_id) )
            {//非阻塞模式下正常情况
                return 0;
            }
            is_connected_ = false;
        }
        return send_len;
    }

    int set_config(ulong connect_timeout        = 0,
                   bool  is_autoconnect         = false,
                   bool  is_add_runner          = true,
                   uint  recvbuffer_size        = 4096,
                   uint  msgbuffer_size         = 4096, 
                   uint8 msglen_bytes           = 2,
                   uint8 msg_byteorder          = 0,
				   bool  is_free_self_handler   = false)
    {
        connect_timeout_        = connect_timeout;
        is_autoconnect_         = is_autoconnect;
        is_add_runner_          = is_add_runner;
        recvbuffer_size_        = recvbuffer_size;
        msgbuffer_size_         = msgbuffer_size;
        msglen_bytes_           = msglen_bytes;
        msg_byteorder_          = msg_byteorder;
		is_free_self_handler_   = is_free_self_handler;

        switch (msglen_bytes)
        {
        case 1:
            {
                get_msglen_proc_  = SL_Socket_Source::get_msglen_int8;
                set_msglen_proc_  = SL_Socket_Source::set_msglen_int8;
            }
            break;
        case 2:
            if (msg_byteorder)
            {
                get_msglen_proc_ = SL_Socket_Source::get_msglen_int16_network;
                set_msglen_proc_ = SL_Socket_Source::set_msglen_int16_network;
            }
            else
            {
                get_msglen_proc_ = SL_Socket_Source::get_msglen_int16_host;
                set_msglen_proc_ = SL_Socket_Source::set_msglen_int16_host;
            }
            break;
        case 4:
            if (msg_byteorder)
            {
                get_msglen_proc_ = SL_Socket_Source::get_msglen_int32_network;
                set_msglen_proc_ = SL_Socket_Source::set_msglen_int32_network;
            }
            else
            {
                get_msglen_proc_ = SL_Socket_Source::get_msglen_int32_host;
                set_msglen_proc_ = SL_Socket_Source::set_msglen_int32_host;
            }
            break;
        }

        return 0;
    }

    int set_interface(TObjectPool *object_pool, SL_Socket_Runner *socket_runner)
    {
        object_pool_    = object_pool;
        socket_runner_  = socket_runner;
        return 0;
    }

    int add_serveraddr(const char *server_name, ushort server_port, bool is_ipv6=false)
    {
        SL_Socket_INET_Addr server_addr;
        server_addr.set(server_name, server_port, is_ipv6);
        serveraddr_list_.push_back(server_addr);
        return 0;
    }

    int remove_serveraddr(int pos)
    {
        if ((pos < 0) || (pos > serveraddr_list_.size()-1))
        {
            return -1;
        }
        serveraddr_list_.erase(serveraddr_list_.begin()+pos);
        return 0;
    }

    int get_serveraddr_count()
    {
        return serveraddr_list_.size();
    }

    int get_connect_serveraddr_pos()
    {
        return connect_serveraddr_pos_;
    }

    SL_Socket_INET_Addr* get_serveraddr(int pos)
    {
        if ((pos < 0) || (pos > serveraddr_list_.size()-1))
        {
            return NULL;
        }
        return &serveraddr_list_[pos];
    }

    bool get_connected()
    {
		return is_connected_;
    }

    bool get_autoconnect()
    {
        return is_autoconnect_;
    }

    void set_autoconnect(bool is_autoconnect)
    {
        is_autoconnect_ = is_autoconnect;
    }

    THandler* get_socket_handler()
    {
        return (THandler *)socket_handler_;
    }

    int put_data(SL_ByteBuffer &buf)
    {
        THandler *sendcontrol_handler = (THandler *)socket_handler_;
        if (NULL != sendcontrol_handler)
        {
            return sendcontrol_handler->put_data(buf);
        }
        return -1;
    }

    int put_data(const char *buf, int len)
    {
        THandler *sendcontrol_handler = (THandler *)socket_handler_;
        if (NULL != sendcontrol_handler)
        {
            return sendcontrol_handler->put_data(buf, len);
        }
        return -1;
    }

    SL_Socket_Handler* alloc_handler()
    {
        return object_pool_->alloc_object();
    }

    void free_handler(SL_Socket_Handler *socket_handler)
    {
        object_pool_->free_object((THandler *)socket_handler);
        return;
    }

    int connect()
    {
        SL_Sync_Guard<SL_Sync_SpinMutex> gurd(mutex_);
        if (is_connected_)
        {
            return 0;
        }
        if (serveraddr_list_.empty())
        {
            return -1;
        }

        if (connect_serveraddr_pos_ >= serveraddr_list_.size())
        {
            connect_serveraddr_pos_ = 0;
        }
        SL_Socket_INET_Addr &server_addr = serveraddr_list_[connect_serveraddr_pos_];
        ++connect_serveraddr_pos_;

        int address_family = server_addr.is_ipv6() ? AF_INET6:AF_INET;
        SL_SOCKET fd = SL_Socket_CommonAPI::socket_open(address_family, SOCK_STREAM, IPPROTO_TCP);
        if (SL_INVALID_SOCKET == fd)
        {
            return -2;
        }

        if (is_bind_localaddr_)
        {
            SL_Socket_INET_Addr local_addr_;
            local_addr_.set(local_name_.c_str(), local_port_);
            SL_Socket_CommonAPI::socket_set_reuseaddr(fd, 1);
            if (SL_Socket_CommonAPI::socket_bind(fd, local_addr_.get_addr(), local_addr_.get_addr_size()) < 0)
            {
                SL_Socket_CommonAPI::socket_close(fd);
                return -3;
            }
        }

        int ret = 0;
        if (connect_timeout_ > 0)
        {
            struct timeval tv;
            tv.tv_sec  = connect_timeout_ / 1000;
            tv.tv_usec = (connect_timeout_ -  tv.tv_sec * 1000) * 1000;
            ret = SL_Socket_CommonAPI::socket_connect(fd, server_addr.get_addr(), server_addr.get_addr_size(), &tv);
        }
        else
        {
            ret = SL_Socket_CommonAPI::socket_connect(fd, server_addr.get_addr(), server_addr.get_addr_size());
        }
        if (ret < 0)
        {
            SL_Socket_CommonAPI::socket_close(fd);
            return -4;
        }
        else
        {
            SL_Socket_Handler *temp_socket_handler = alloc_handler();
            if (NULL == temp_socket_handler)
            {
                return -5;
            }
            if (temp_socket_handler->handle_open(fd, this, socket_runner_) < 0)
            {
                SL_Socket_CommonAPI::socket_close(fd);
                free_handler(temp_socket_handler);
                return -6;
            }
            if (is_add_runner_)
            {
                if (socket_runner_->add_handle(temp_socket_handler, SL_Socket_Handler::READ_EVENT_MASK) < 0)
                {
                    SL_Socket_CommonAPI::socket_close(fd);
                    free_handler(temp_socket_handler);
                    return -7;
                }
            }
            is_connected_ = true;
            socket_handler_ = temp_socket_handler;
            socket_handler_->handle_connect();
        }
        return 0;
    }

    int disconnect(SL_Socket_Handler *socket_handler)
    {
        mutex_.lock();
        if (NULL != socket_handler_)
        {
            if (socket_handler == socket_handler_)
            {
                is_connected_ = false;
                socket_handler_->close();
                if (is_free_self_handler_)
                {
                    free_handler(socket_handler_);
                }
                socket_handler_ = NULL;
            }
        }
        mutex_.unlock();
        return 0;
    }

protected:
    std::vector<SL_Socket_INET_Addr> serveraddr_list_;
    std::string     local_name_;
    ushort          local_port_;
    bool            is_bind_localaddr_;

    uint            connect_serveraddr_pos_;
    ulong           connect_timeout_;               //时间单位: ms
    volatile bool   is_connected_;                  //连接状态(fase:失败,true:成功)
    volatile bool   is_autoconnect_;                //是否自动重连
	SL_Sync_SpinMutex mutex_;

    TObjectPool     *object_pool_;
};

#endif

