#ifndef SOCKETLITE_SEDA_INTERFACE_H
#define SOCKETLITE_SEDA_INTERFACE_H
#include "SL_DataType.h"
#include "SL_Seda_Event.h"
#include "SL_Seda_Timer.h"

class SL_Seda_IEventQueue;
class SL_Seda_IStage;
class SL_Seda_IStageHandler;
class SL_Seda_IStageThread;

#define SOCKETLITE_SEDA_DEFAULT_IDLE_INTERVAL       1000        //millisecond(ms)
#define SOCKETLITE_SEDA_DEFAULT_TIMEDWAIT_INTERVAL  2000        //microsecond(us)
#define SOCKETLITE_SEDA_TIMER_EVENT_FACTOR          1000        //每处理SOCKETLITE_SEDA_TIMER_EVENT_FACTOR个事件后，检测定时器

class SL_Seda_IEventQueue
{
public:
    SL_Seda_IEventQueue()
    {
    }

    virtual ~SL_Seda_IEventQueue()
    {
    }

    virtual int  init(uint capacity, uint event_max_len) = 0;
    virtual void clear() = 0;
    virtual int  push(const SL_Seda_Event *event) = 0;
    virtual int  pop(SL_Seda_Event **event) = 0;
    virtual uint capacity() const = 0;
    virtual uint size() const = 0;
    virtual bool empty() const = 0;
};

class SL_Seda_IStageHandler
{
public:
    SL_Seda_IStageHandler()
    {
    }

    virtual ~SL_Seda_IStageHandler()
    {
    }

    virtual int  handle_open() = 0;
    virtual int  handle_close() = 0;
    virtual int  handle_event(const SL_Seda_Event *event) = 0;
};

class SL_Seda_IStage
{
public:
    SL_Seda_IStage()
    {
    }

    virtual ~SL_Seda_IStage()
    {
    }

    virtual int open(int thread_number, uint queue_max_size, uint event_max_len, uint timedwait_interval_us, bool timedwait_signal, int type) = 0;
    virtual int close() = 0;
    virtual int push_event(const SL_Seda_Event *event, int thread_number, bool timedwait_signal) = 0;
    virtual int get_type() const = 0;
};

class SL_Seda_IStageThread
{
public:
    SL_Seda_IStageThread()
    {
    }

    virtual ~SL_Seda_IStageThread()
    {
    }

    virtual int  start() = 0;
    virtual void stop() = 0;
    virtual void join() = 0;
    virtual int  push_event(const SL_Seda_Event *event, bool timedwait_signal) = 0;

    virtual SL_Seda_Timer* set_timer(uint interval_ms, SL_Seda_Timer::TimerParam param) = 0;
    virtual int  cancel_timer(SL_Seda_Timer *timer) = 0;
    virtual int  update_timer(SL_Seda_Timer *timer) = 0;
    virtual int  enable_timer_event(uint capacity) = 0;

    virtual SL_Seda_Timer* set_lru_timer(int slot, SL_Seda_Timer::TimerParam param) = 0;
    virtual int  cancel_lru_timer(int slot, SL_Seda_Timer *timer) = 0;
    virtual int  update_lru_timer(int slot, SL_Seda_Timer *timer) = 0;
    virtual int  enable_lru_timers(SL_Seda_StageThreadLRUSlotInfo *slots, int slot_count) = 0;

    virtual int  enable_idle_event(bool enabled) = 0;
    virtual int  set_idle_interval(uint idle_interval_ms) = 0;
    virtual int  set_sleep_interval(uint sleep_interval_ms) = 0;
    virtual int  set_timedwait_interval(uint timedwait_interval_ms) = 0;

    virtual SL_Seda_IStage* get_stage() const = 0;
    virtual uint get_thread_index() const = 0;
};

#endif

