#ifndef SOCKETLITE_SOCKET_SOURCE_H
#define SOCKETLITE_SOCKET_SOURCE_H
#include "SL_Socket_INET_Addr.h"
#include "SL_Socket_Runner.h"

class SOCKETLITE_API SL_Socket_Source
{
public:
    SL_Socket_Source()
        : get_msglen_proc_(get_msglen_int16_host)
        , set_msglen_proc_(set_msglen_int16_host)
        , socket_handler_(NULL)
        , socket_runner_(NULL)
        , recvbuffer_size_(4096)
        , msgbuffer_size_(4096)
        , msglen_bytes_(2)
        , msg_byteorder_(0)
        , is_add_runner_(true)
        , is_free_self_handler_(false)
    {
    }

    virtual ~SL_Socket_Source() 
    {
    }

    virtual SL_Socket_INET_Addr* get_local_addr()
    {
        return NULL;
    }

    //SL_TcpClient的特定函数
    virtual int send(const char *buf, int len, int flag) 
    { 
        return 0; 
    }
    virtual int recv(char *buf, int len, int flag) 
    { 
        return 0; 
    }
    virtual bool get_connected()
    {
        return false;
    }
    virtual bool get_autoconnect()
    {
        return false;
    }
    virtual int connect()
    {
        return -1;
    }
    virtual int disconnect(SL_Socket_Handler *socket_handler)
    {
        return -1;
    }

    inline SL_Socket_Handler* get_socket_handler()
    {
        return socket_handler_;
    }

    inline SL_Socket_Runner* get_socket_runner()
    {
        return socket_runner_;
    }

    inline uint get_recvbuffer_size() const
    {
        return recvbuffer_size_;
    }

    inline uint get_msgbuffer_size() const
    {
        return msgbuffer_size_;
    }

    inline uint8 get_msglen_bytes() const
    {
        return msglen_bytes_;
    }

    inline uint8 get_msg_byteorder() const
    {
        return msg_byteorder_;
    }

    inline bool get_add_runner() const
    {
        return is_add_runner_;
    }

    static int get_msglen_int8(const char *msg, int len)
    {
        return *msg;
    }

    static int get_msglen_int16_host(const char *msg, int len)
    {
        return  *((int16 *)msg);
    }

    static int get_msglen_int16_network(const char *msg, int len)
    {
        return ntohs(*((int16 *)msg));
    }

    static int get_msglen_int32_host(const char *msg, int len)
    {
         return  *((int32 *)msg);
    }

    static int get_msglen_int32_network(const char *msg, int len)
    {
        return ntohl(*((int32 *)msg));
    }

    static void set_msglen_int8(char *msg, int len)
    {
        *msg = len;      
    }

    static void set_msglen_int16_host(char *msg, int len)
    {
        *((int16 *)msg) = len;
    }

    static void set_msglen_int16_network(char *msg, int len)
    {
        *((int16 *)msg) = htons(len);
    }

    static void set_msglen_int32_host(char *msg, int len)
    {
        *((int32 *)msg) = len;
    }

    static void set_msglen_int32_network(char *msg, int len)
    {
        *((int32 *)msg) = htonl(len);
    }

public:
    virtual SL_Socket_Handler* alloc_handler()
    {
        return NULL;
    }

    virtual void free_handler(SL_Socket_Handler *socket_handler)
    {
        return;
    }

    typedef int (*get_msglen_proc)(const char *msg, int len);
    typedef void (*set_msglen_proc)(char *msg, int len);

    get_msglen_proc     get_msglen_proc_;       //取得消息长度函数指针
    set_msglen_proc     set_msglen_proc_;       //设置消息长度函数指针

protected:
    SL_Socket_Handler   *socket_handler_;
    SL_Socket_Runner    *socket_runner_;

    uint                recvbuffer_size_;
    uint                msgbuffer_size_;
    uint8               msglen_bytes_;          //消息长度域所占字节数(一般为1,2,4)
    uint8               msg_byteorder_;         //消息的字节顺序(0:host-byte,1:big endian(network-byte) 2:little endian)
    bool                is_add_runner_;         //新连接是否加入Socket_Runner中，进行事件分派
    bool                is_free_self_handler_;  //可否释放Source本身的socket_handler_,
    											//若socket_handler_加入SL_Socket_SendControl_HandlerManager时，应为false
};

#endif

