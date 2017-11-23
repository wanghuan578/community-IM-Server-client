#ifndef SOCKETLITE_SOCKET_HANDLER_H
#define SOCKETLITE_SOCKET_HANDLER_H

#include "SL_Config.h"
#include "SL_Socket_CommonAPI.h"

class SL_Socket_Source;
class SL_Socket_Runner;
template <typename TSyncMutex> class SL_Socket_Select_Runner;
template <typename TSyncMutex> class SL_Socket_Poll_Runner;
template <typename TSyncMutex> class SL_Socket_Epoll_Runner;
template <typename TSyncMutex> class SL_Socket_Iocp_Runner;
template <typename TSyncMutex> class SL_Socket_kqueue_Runner;
template <typename TSocketRunner> class SL_Socket_Runner_Group;

class SL_Socket_Handler
{
public:
    enum EVENT_MASK
    {
        NULL_EVENT_MASK             = 0x00000000,
        READ_EVENT_MASK             = 0x00000001,
        WRITE_EVENT_MASK            = 0x00000002,
        EXCEPT_EVENT_MASK           = 0x00000004,
        TIMEOUT_EVENT_MASK          = 0x00000008,
        ALL_EVENT_MASK              = READ_EVENT_MASK | WRITE_EVENT_MASK | EXCEPT_EVENT_MASK  | TIMEOUT_EVENT_MASK,
    };

    enum SOCKET_STATUS
    {
        STATUS_CLOSE                = 0x00,             //连接关闭
        STATUS_OPEN                 = 0x01,             //连接打开
        STATUS_CLOSE_WAIT           = 0x02,             //连接等待关闭(应用层主动要关闭socket,socket还未关闭)
        STATUS_CLOSE_SEND           = 0x04,             //连接关闭发送通道(socket已关闭发送通道,但可接收数据)
        STATUS_CLOSE_RECV           = 0x08,             //连接关闭接收通道(socket已关闭接收通道,但可发送数据)
    };

    enum HANDLE_ERROR
    {
        ERROR_START_NUMBER          = 1000000,

        ERROR_CLOSE_PASSIVE         = 1000001,          //被动关闭(远端关闭remote)
        ERROR_CLOSE_ACTIVE          = 1000002,          //主动关闭(本地关闭local)
        ERROR_CLOSE_RECV            = 1000003,          //接收时组包出错关闭
        ERROR_CLOSE_SEND            = 1000004,          //发送数据出错关闭
        ERROR_KEEPALIVE_TIMEOUT     = 1000005,          //keepalive超时关闭

        ERROR_END_NUMBER            = 1009999,
    };

    inline SL_Socket_Handler()
        : event_mask_(NULL_EVENT_MASK)
        , runner_pos_(-1)
        , socket_source_(NULL)
        , socket_runner_(NULL)
        , socket_(SL_INVALID_SOCKET)
        , last_errno_(0)
        , current_status_(STATUS_CLOSE)
        , next_status_(STATUS_CLOSE)
    {
    }

    virtual ~SL_Socket_Handler() 
    {
    }

    inline SL_SOCKET get_socket() const
    {
        return socket_;
    }

    inline void set_socket(SL_SOCKET fd)
    {
        socket_ = fd;
    }

    inline void set_socket_source(SL_Socket_Source *socket_source)
    {
        socket_source_ = socket_source;
    }

    inline void set_socket_runner(SL_Socket_Runner *socket_runner)
    {
        socket_runner_ = socket_runner;
    }

    inline SL_Socket_Source* get_socket_source() const
    {
        return socket_source_;
    }

    inline SL_Socket_Runner* get_socket_runner() const
    {
        return socket_runner_;
    }

    virtual int handle_open(SL_SOCKET fd, SL_Socket_Source *socket_source, SL_Socket_Runner *socket_runner)
    {
        socket_         = fd;
        socket_source_  = socket_source;
        socket_runner_  = socket_runner;
        last_errno_     = 0;
        current_status_ = STATUS_OPEN;
        next_status_    = STATUS_OPEN;
        SL_Socket_CommonAPI::socket_set_block(fd, false);
        return 0;
    }

    virtual int handle_close()
    { 
        return 0;
    }

    virtual int handle_read()
    { 
        return 0; 
    }

    virtual int handle_write()
    { 
        return 0;
    }

    virtual int handle_exception()
    {
        return -1;
    }

    //tcpclient连接前，预处理事件
    virtual int handle_connecting()
    { 
        return 0; 
    }

    //tcpclient连接成功后，通知事件
    virtual int handle_connect() 
    { 
        return 0;
    }

    inline void close()
    {
        SL_Socket_CommonAPI::socket_close(socket_);
        socket_ = SL_INVALID_SOCKET;
    }

    int write_data(const char *buf, int len);

    inline int8 get_status()
    {
        return current_status_;
    }

private:
    int                 event_mask_;        //事件码(上层不能修改)
    int                 runner_pos_;        //在socket_runner_中位置(上层不能修改)

    template <typename TSyncMutex> friend class SL_Socket_Select_Runner;
    template <typename TSyncMutex> friend class SL_Socket_Poll_Runner;
    template <typename TSyncMutex> friend class SL_Socket_Epoll_Runner;
    template <typename TSyncMutex> friend class SL_Socket_Iocp_Runner;
    template <typename TSyncMutex> friend class SL_Socket_kqueue_Runner;
    template <typename TSocketRunner> friend class SL_Socket_Runner_Group;
    friend class SL_Socket_SendControl_HandlerManager;

protected:
    SL_Socket_Source    *socket_source_;
    SL_Socket_Runner    *socket_runner_;
    SL_SOCKET           socket_;

    int                 last_errno_;        //最后错误号
    int8                current_status_;    //当前状态
    int8                next_status_;       //下一状态(引入它的目的: 检测到连接关闭,远程主机可能只关闭了发送通道;还可继续接收数据)
};

#endif

