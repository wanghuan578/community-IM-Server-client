////////////////////////////////////////////////////////////////////////////////
//  basic structure of seda event

#ifndef SOCKETLITE_SEDA_EVENT_H
#define SOCKETLITE_SEDA_EVENT_H
#include <stddef.h>
#include "SL_DataType.h"
#include "SL_ByteBuffer.h"
#include "SL_Seda_Timer.h"
#include "SL_Socket_Handler.h"
#include "SL_Socket_INET_Addr.h"

////////////////////////////////////////////////////////////////////////////////
//event types

////////////////////////////////////////////////////////////////////////////////
//basic event structure
//NOTICE: this class can be inherited, 
//      but NEVER define virtual member-functions in the sub-classes

class SL_Seda_Event
{
public:
    static const int MAX_LENGTH = 1 * 1024 * 1024;

    inline int get_type() const
    {
        return type_;
    }

    inline int get_event_len() const
    {
        return len_ + sizeof(SL_Seda_Event);
    }

    inline uint8* get_event_buffer() const
    {
        return ((uint8 *)this);
    }

public:
    inline SL_Seda_Event()
        : type_(0)
        , len_(0) 
    {
    }

    inline SL_Seda_Event(int type, int len)
        : type_(type)
        , len_(len) 
    {
    }

    inline ~SL_Seda_Event() 
    {
    }

    int type_;  //type of SL_Seda_Event
    int len_;   //len of data, excluding sizeof(SL_Seda_Event)
};

////////////////////////////////////////////////////////////////////////////////
//template of signal events (events without data)
//  type_value: the number of event type

template <int type_value>
class SL_Seda_SignalEvent : public SL_Seda_Event
{
public:
    static const int EVENT_TYPE = type_value;

    inline static SL_Seda_SignalEvent<type_value>* change_event(SL_Seda_Event *event)
    {
        if (event->get_type() == EVENT_TYPE)
        {
            return (SL_Seda_SignalEvent<type_value> *)event;
        }
        return NULL;
    }

    inline static const SL_Seda_SignalEvent<type_value>* change_event(const SL_Seda_Event *event)
    {
        if (event->get_type() == EVENT_TYPE)
        {
            return (const SL_Seda_SignalEvent<type_value> *)event;
        }
        return NULL;
    }

    inline SL_Seda_SignalEvent(int type = type_value)
        : SL_Seda_Event(type, 0) 
    {
    }

    inline ~SL_Seda_SignalEvent() 
    {
    }
};

////////////////////////////////////////////////////////////////////////////////
//template of fixed-size events
//  TEvent: the final class name of the event
//  type_value: the number of event type

template <class TEvent, int type_value>
class SL_Seda_FixedSizeEventBase : public SL_Seda_Event
{
public:
    static const int EVENT_TYPE = type_value;

    inline static TEvent* change_event(SL_Seda_Event *event)
    {
        if (event->get_type() == EVENT_TYPE)
        {
            return (TEvent *)event;
        }
        return NULL;
    }

    inline static const TEvent* change_event(const SL_Seda_Event *event)
    {
        if (event->get_type() == EVENT_TYPE)
        {
            return (const TEvent *)event;
        }
        return NULL;
    }

protected:
    inline SL_Seda_FixedSizeEventBase(int type = type_value)
        : SL_Seda_Event(type, sizeof(TEvent)-sizeof(SL_Seda_Event)) 
    {
    }

    inline ~SL_Seda_FixedSizeEventBase() 
    {
    }
};

////////////////////////////////////////////////////////////////////////////////
//  base event types
struct SL_Seda_EventType
{
    enum
    {
        QUIT_EVENT = 0,
        THREAD_IDLE,
        TIMER_EXPIRE,

        TCP_LISTEN,
        TCP_SOCKET_OPEN,
        TCP_SOCKET_CLOSE,
        TCP_READ_DATA,
        TCP_READ_MESSAGE,

        UDP_SOCKET_OPEN,
        UDP_SOCKET_CLOSE,
        UDP_READ_DATA,

        RPC_MESSAGE,            //rpc(支持thrift和protobuf等消息格式)
		TRANSMIT_SERVER,
        USER_START = 1000,
        USER_START_NUMBER = 1000,
    };
};

typedef SL_Seda_SignalEvent<SL_Seda_EventType::QUIT_EVENT>  SL_Seda_QuitEvent;
typedef SL_Seda_SignalEvent<SL_Seda_EventType::THREAD_IDLE> SL_Seda_ThreadIdleEvent;
typedef int(*SL_Seda_RpcMessageProc)(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db_inst);
typedef int(*SL_Seda_RpcTransmitProc)(int from_user_id, int to_user_id, void *message, int len);

struct SL_Seda_TimerExpireEvent : public SL_Seda_FixedSizeEventBase<SL_Seda_TimerExpireEvent, SL_Seda_EventType::TIMER_EXPIRE>
{
    SL_Seda_Timer *timer;
    int slot;
};

struct SL_Seda_TcpSocketOpenEvent : public SL_Seda_FixedSizeEventBase<SL_Seda_TcpSocketOpenEvent, SL_Seda_EventType::TCP_SOCKET_OPEN>
{
	int socket_handler_id;
    SL_Socket_Handler *socket_handler;
	SL_Seda_RpcMessageProc rpc_proc;//wanghuan add
};

struct SL_Seda_TcpSocketCloseEvent : public SL_Seda_FixedSizeEventBase<SL_Seda_TcpSocketCloseEvent, SL_Seda_EventType::TCP_SOCKET_CLOSE>
{
	int32 source;
	int socket_handler_id;
	SL_Socket_Handler *socket_handler;
	SL_Seda_RpcMessageProc rpc_proc;
	int64 attach_id1;
	int64 attach_id2;
	void  *attach_object1;
	void  *attach_object2;
};

struct SL_Seda_TcpReadDataEvent : public SL_Seda_FixedSizeEventBase<SL_Seda_TcpReadDataEvent, SL_Seda_EventType::TCP_READ_DATA>
{
    SL_Socket_Handler *socket_handler;
    SL_ByteBuffer *data;
    int64 attach_id1;
    int64 attach_id2;
    void  *attach_object1;
    void  *attach_object2;
};

struct SL_Seda_TcpReadMessageEvent : public SL_Seda_FixedSizeEventBase<SL_Seda_TcpReadMessageEvent, SL_Seda_EventType::TCP_READ_MESSAGE>
{
    SL_Socket_Handler *socket_handler;
    SL_ByteBuffer *message;
    int64 attach_id1;
    int64 attach_id2;
    void  *attach_object1;
    void  *attach_object2;
};

struct SL_Seda_RpcMessageEvent : public SL_Seda_FixedSizeEventBase<SL_Seda_RpcMessageEvent, SL_Seda_EventType::RPC_MESSAGE>
{
    int socket_handler_id;
    SL_Socket_Handler *socket_handler;
    SL_Seda_RpcMessageProc rpc_proc;
    void *rpc_message;
    SL_Socket_INET_Addr *remote_addr;
};

struct SL_Seda_RpcTransmitEvent : public SL_Seda_FixedSizeEventBase<SL_Seda_RpcMessageEvent, SL_Seda_EventType::TRANSMIT_SERVER>
{
	int from_user_id;
	int to_user_id;
	void *transmit_message;
	int message_len;
	SL_Seda_RpcTransmitProc transmit_proc;
};
#endif

