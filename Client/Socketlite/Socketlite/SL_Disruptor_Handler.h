#ifndef SOCKETLITE_DISRUPTOR_HANDLER_H
#define SOCKETLITE_DISRUPTOR_HANDLER_H

#include <list>
#include "SL_DataType.h"
#include "SL_Socket_CommonAPI.h"
#include "SL_Sync_Atomic.h"
#include "SL_Thread.h"
#include "SL_Sync_Mutex.h"
#include "SL_Sync_Condition.h"
#include "SL_Seda_TimerQueue.h"
#include "SL_Disruptor_Event.h"
#include "SL_Disruptor_Interface.h"

class SL_Disruptor_Handler : public SL_Disruptor_IHandler
{
public:
    SL_Disruptor_Handler()
        : handler_index_(0)
        , event_queue_(NULL)
        , timedwait_interval_us_(1)
        , timedwait_flag_(0)
        , timedwait_signal_(false)
        , timer_event_flag_(false)
        , timer_min_interval_ms_(1000)
        , batch_signal_flag_(true)
    {
    }

    virtual ~SL_Disruptor_Handler()
    {
       close();
    }

    virtual int handle_open()
    {
        return 0;
    }

    virtual int handle_close()
    {
        return 0;
    }

    virtual int handle_event(const SL_Disruptor_Event *event)
    {
        return 0;
    }

    inline int open(SL_Disruptor_IEventQueue *event_queue, bool batch_signal_flag=true, uint timedwait_interval_us=1000, bool timedwait_signal=true)
    {
        event_queue_            = event_queue;
        batch_signal_flag_      = batch_signal_flag;
        timedwait_interval_us_  = timedwait_interval_us;
        timedwait_signal_       = timedwait_signal;

        return thread_.start(thread_proc, this);
    }

    inline int close()
    {
        if (NULL != event_queue_)
        {
            thread_.stop();
            thread_.join();
            event_queue_ = NULL;
        }
        return 0;
    }

    inline void add_dependent_handler(SL_Disruptor_IHandler *handler)
    {
        dependent_list_.push_back(handler);
        handler->add_notify_handler(this);
    }

    inline int64 handler_index()
    {
        return handler_index_.load();
    }

    inline SL_Seda_Timer* set_lru_timer(int slot, SL_Seda_Timer::TimerParam param)
    {
        if ((slot < 0) || (slot >= (int)lru_timer_managers_.size()))
        {
            return NULL;
        }
        return lru_timer_managers_[slot].set_timer(0, param);
    }

    inline int cancel_lru_timer(int slot, SL_Seda_Timer *timer)
    {
        if ((slot < 0) || (slot >= (int)lru_timer_managers_.size()))
        {
            return -1;
        }
        return lru_timer_managers_[slot].cancel_timer(timer);
    }

    inline int update_lru_timer(int slot, SL_Seda_Timer *timer)
    {
        if ((slot < 0) || (slot >= (int)lru_timer_managers_.size()))
        {
            return -1;
        }
        return lru_timer_managers_[slot].update_timer(timer);
    }

    inline int enable_lru_timers(SL_Seda_StageThreadLRUSlotInfo *slots, int slot_count)
    {
        if ( (slot_count > 0) && lru_timer_managers_.empty() )
        {
            lru_timer_managers_.resize(slot_count);
            for (int i=0; i<slot_count; ++i)
            {
                int ret = lru_timer_managers_[i].init(slots[i].capacity, slots[i].interval_ms, this, i);
                if (ret < 0)
                {
                    return ret;
                }
                if ( (slots[i].interval_ms < timer_min_interval_ms_) || (0 == timer_min_interval_ms_) )
                {
                    timer_min_interval_ms_ = slots[i].interval_ms;
                }
            }
            timer_event_flag_ = true;
            return 0;
        }
        return -1;
    }

private:
    inline int signal_event(int64 event_index)
    {
        if (timedwait_signal_)
        {
            if (timedwait_flag_.load() > 0)
            {
                int64 handler_index = handler_index_.load();
                if (event_index > handler_index)
                {
                    timedwait_flag_.store(0);
                    timedwait_condition_.signal();
                    return 1;
                }
            }
        }
        return 0;
    }

    inline void add_notify_handler(SL_Disruptor_IHandler *handler)
    {
        notify_list_.push_back(handler);
    }

    inline void check_timers(uint64 current_clock_ms, SL_Seda_TimerExpireEvent *event)
    {
        int timer_size = (int)lru_timer_managers_.size();
        for (int i=0; i<timer_size; ++i)
        {
            lru_timer_managers_[i].expire(current_clock_ms, event);
        }
    }

#ifdef SOCKETLITE_OS_WINDOWS
    static unsigned int WINAPI thread_proc(void *arg)
#else
    static void* thread_proc(void *arg)
#endif
    {
        SL_Disruptor_Handler *handler = (SL_Disruptor_Handler *)arg;
        handler->handle_open();

        uint64  current_clock_ms    = SL_Socket_CommonAPI::util_process_clock_ms();
        int     timer_event_count   = 0;
        bool    timer_event_flag    = handler->timer_event_flag_;
        bool    dependent_flag      = !handler->dependent_list_.empty();
        bool    notify_flag         = !handler->notify_list_.empty();

        uint timedwait_interval_us  = handler->timer_min_interval_ms_ * 1000;
        if (timedwait_interval_us > handler->timedwait_interval_us_)
        {
            timedwait_interval_us = handler->timedwait_interval_us_;
        }

        int64   current_index = 0;
        int64   next_index = 0;
        int64   temp_index = 0;
        std::list<SL_Disruptor_IHandler* >::iterator iter;
        std::list<SL_Disruptor_IHandler* >::iterator iter_end;

        SL_Disruptor_TimerExpireEvent timer_expire_event;
        SL_Disruptor_Event *event = NULL;

        if (!timer_event_flag)
        {//timer_event_flag = false
            while (1)
            {
                if (!dependent_flag)
                {
                    next_index  = handler->event_queue_->cursor_index();
                }
                else
                {
                    iter        = handler->dependent_list_.begin();
                    iter_end    = handler->dependent_list_.end();
                    next_index  = (*iter)->handler_index();
                    ++iter;
                    for (; iter != iter_end; ++iter)
                    {
                        temp_index = (*iter)->handler_index();
                        if (next_index > temp_index)
                        {
                            next_index = temp_index;
                        }
                    }
                    if (next_index > 0)
                    {
                        ++next_index;
                    }
                }

                if (current_index < next_index)
                {
                    for (; current_index < next_index; ++current_index)
                    {
                        event = handler->event_queue_->get_event(current_index);
                        if (SL_Seda_EventType::QUIT_EVENT != event->type_)
                        {
                            handler->handle_event(event);
                        }
                        else
                        {
                            handler->handler_index_.store(current_index);
                            goto QUIT_THREAD_PROC;
                        }
                    }

                    handler->handler_index_.store(current_index);
                    if (notify_flag)
                    {
                        iter = handler->notify_list_.begin();
                        iter_end = handler->notify_list_.end();
                        for (; iter != iter_end; ++iter)
                        {
                            (*iter)->signal_event(current_index);
                        }
                    }
                }
                else
                {
                    handler->timedwait_flag_.store(1);
#ifdef SOCKETLITE_OS_WINDOWS
                    handler->timedwait_condition_.timed_wait(NULL, timedwait_interval_us);
#else
                    handler->timedwait_mutex_.lock();
                    handler->timedwait_condition_.timed_wait(&handler->timedwait_mutex_, timedwait_interval_us);
                    handler->timedwait_mutex_.unlock();
#endif
                    handler->timedwait_flag_.store(0);
                }
            }
        }
        else
        {//timer_event_flag = true
            while (1)
            {
                if (!dependent_flag)
                {
                    next_index = handler->event_queue_->cursor_index();
                }
                else
                {
                    iter = handler->dependent_list_.begin();
                    iter_end = handler->dependent_list_.end();
                    next_index = (*iter)->handler_index();
                    ++iter;
                    for (; iter != iter_end; ++iter)
                    {
                        temp_index = (*iter)->handler_index();
                        if (temp_index < next_index)
                        {
                            next_index = temp_index;
                        }
                    }
                    if (next_index > 0)
                    {
                        ++next_index;
                    }
                }

                if (current_index < next_index)
                {
                    for (; current_index < next_index; ++current_index)
                    {
                        event = handler->event_queue_->get_event(current_index);
                        if (SL_Seda_EventType::QUIT_EVENT != event->type_)
                        {
                            handler->handle_event(event);

                            ++timer_event_count;
                            if (timer_event_count == SOCKETLITE_SEDA_TIMER_EVENT_FACTOR)
                            {
                                handler->check_timers(SL_Socket_CommonAPI::util_process_clock_ms(), &timer_expire_event);
                                timer_event_count = 0;
                            }
                        }
                        else
                        {
                            handler->handler_index_.store(current_index);
                            goto QUIT_THREAD_PROC;
                        }
                    }

                    handler->handler_index_.store(current_index);
                    if (notify_flag)
                    {
                        iter = handler->notify_list_.begin();
                        iter_end = handler->notify_list_.end();
                        for (; iter != iter_end; ++iter)
                        {
                            (*iter)->signal_event(current_index);
                        }
                    }
                }
                else
                {
                    current_clock_ms = SL_Socket_CommonAPI::util_process_clock_ms();
                    handler->check_timers(current_clock_ms, &timer_expire_event);
                    timer_event_count = 0;

                    handler->timedwait_flag_.store(1);
#ifdef SOCKETLITE_OS_WINDOWS
                    handler->timedwait_condition_.timed_wait(NULL, timedwait_interval_us);
#else
                    handler->timedwait_mutex_.lock();
                    handler->timedwait_condition_.timed_wait(&handler->timedwait_mutex_, timedwait_interval_us);
                    handler->timedwait_mutex_.unlock();
#endif
                    handler->timedwait_flag_.store(0);
                }
            }
        }

QUIT_THREAD_PROC:
        handler->handle_close();
        handler->thread_.exit();
        return 0;
    }

private:
    SL_Thread                           thread_;                    //处理线程
    SL_Sync_Atomic_Int64                handler_index_;             //处理位置(只增不减)
    SL_Disruptor_IEventQueue            *event_queue_;              //事件队列
    std::list<SL_Disruptor_IHandler* >  dependent_list_;            //依赖列表
    std::list<SL_Disruptor_IHandler* >  notify_list_;               //通知列表

    SL_Sync_ThreadMutex                 timedwait_mutex_;
    SL_Sync_Condition                   timedwait_condition_;
    uint                                timedwait_interval_us_;
    SL_Sync_Atomic_Int32                timedwait_flag_;            //条件触发标识
    bool                                timedwait_signal_;          //用于控制是否触发条件信号(因触发条件信号比较耗时)

    bool                                timer_event_flag_;          //定时器事件触发标志
    uint                                timer_min_interval_ms_;     //定时器最小间隔(ms)
    SL_Seda_TimerQueue                  timer_queue_;
    std::vector<SL_Seda_LRUTimerQueue>  lru_timer_managers_;

    bool                                batch_signal_flag_;         //处理批量事件后发信号通知后续handler
};

#endif

