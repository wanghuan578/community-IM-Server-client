#ifndef SOCKETLITE_DISRUPTOR_INTERFACE_H
#define SOCKETLITE_DISRUPTOR_INTERFACE_H
#include "SL_DataType.h"
#include "SL_Seda_Interface.h"
#include "SL_Disruptor_Event.h"

class SL_Disruptor_IHandler : public SL_Seda_IStageHandler
{
public:
    SL_Disruptor_IHandler()
    {
    }

    virtual ~SL_Disruptor_IHandler()
    {
    }

    virtual void    add_dependent_handler(SL_Disruptor_IHandler *handler) = 0;
    virtual void    add_notify_handler(SL_Disruptor_IHandler *handler) = 0;
    virtual int64   handler_index() = 0;
    virtual int     signal_event(int64 event_index) = 0;
};

class SL_Disruptor_IEventQueue
{
public:
    SL_Disruptor_IEventQueue()
    {
    }

    virtual ~SL_Disruptor_IEventQueue()
    {
    }

    virtual int     init(uint capacity, uint event_max_len, int rewrite_count, int reread_count) = 0;
    virtual void    clear() = 0;

    virtual int     push(const SL_Disruptor_Event *event, bool timedwait_signal, int redo_count) = 0;
    virtual uint    capacity() const = 0;
    virtual uint    size() = 0;
    virtual bool    empty() = 0;

    virtual void    add_handler(SL_Disruptor_IHandler *handler, bool dependent_flag, bool notify_flag) = 0;
    virtual int64   cursor_index() = 0;
    virtual int     quit_event() = 0;

    virtual SL_Disruptor_Event* get_event(int64 event_index) = 0;
};

#endif

