#ifndef SOCKETLITE_TIMER_LIST_CLOCK_H
#define SOCKETLITE_TIMER_LIST_CLOCK_H

#include "SL_Socket_CommonAPI.h"
#include "SL_Timer.h"
#include "SL_Timer_Queue.h"
#include "SL_Sync_SpinMutex.h"
#include "SL_Thread.h"
#include <list>

template <typename TSyncMutex>
class SL_Timer_List_Clock : public SL_Timer_Queue
{
public:
    SL_Timer_List_Clock()
        : max_timer_id_(0)
    {
    }

    virtual ~SL_Timer_List_Clock()
    {
        max_timer_id_ = 0;
        clear_timer_i();
    }

    inline int set_config(uint interval_ms = SL_TIMER_DEFULT_INTERVAL, uint repair_ms = SL_TIMER_DEFAUL_REPAIR)
    {
        interval_ms_  = interval_ms;
        repair_ms_    = repair_ms;
        return 0;
    }

    inline int add_timer(SL_Timer *timer)
    {
        mutex_.lock();
        add_timer_i(timer);
        timer->id_ = ++max_timer_id_;
        mutex_.unlock();
        return 0;
    }

    inline int del_timer(SL_Timer *timer)
    {
        mutex_.lock();
        del_timer_i(timer);
        mutex_.unlock();
        return 0;
    }

    inline int clear_timer()
    {
        mutex_.lock();
        clear_timer_i();
        mutex_.unlock();
        return 0;
    }

    inline int timer_loop()
    {
        uint64 current_clock = SL_Socket_CommonAPI::util_process_clock_ms() + repair_ms_;
        std::list<SL_Timer* > temp_timer_list;
        SL_Timer *temp_timer;
        std::list<SL_Timer* >::iterator iter;
        std::list<SL_Timer* >::iterator iter_end;

        mutex_.lock();
        iter = timer_list_.begin();
        iter_end = timer_list_.end();
        while (iter != iter_end)
        {
            temp_timer = *iter;
            if (current_clock >= temp_timer->expire_time_.end_clock)
            {
                temp_timer_list.push_back(temp_timer);
                iter = timer_list_.erase(iter);
                if (temp_timer->type_ == SL_Timer::TIMER_TYPE_ONCE)
                {
                    temp_timer->state_ = SL_Timer::TIMER_STATE_TIMEOUT;
                }
                else if (temp_timer->type_ == SL_Timer::TIMER_TYPE_CYCLE)
                {
                    temp_timer->state_ = SL_Timer::TIMER_STATE_IDLE;
                    add_timer_i(temp_timer);
                }
                iter_end = temp_timer_list.end();
            }
            else
            {
                break;
            }
        }
        mutex_.unlock();

        //执行定时处理函数
        iter = temp_timer_list.begin();
        iter_end = temp_timer_list.end();
        for (; iter != iter_end; ++iter)
        {
            temp_timer = *iter;
            temp_timer->timeout();
        }
        temp_timer_list.clear();
                
        return 0;
    }

    inline int thread_timer_loop()
    {
        return timer_loop_thread_.start(timer_loop_proc, this);
    }

    inline int thread_wait()
    {
        timer_loop_thread_.join();
        return 0;
    }

    inline int thread_stop()
    {
        timer_loop_thread_.stop();
        return 0;
    }

private:    
#ifdef SOCKETLITE_OS_WINDOWS
    static unsigned int WINAPI timer_loop_proc(void *arg)
#else
    static void* timer_loop_proc(void *arg)
#endif
    {
        SL_Timer_List_Clock<TSyncMutex> *timer_list = (SL_Timer_List_Clock<TSyncMutex>*)arg;
        while (1)
        {
            if (!timer_list->timer_loop_thread_.get_running())
            {
                break;
            }
            SL_Socket_CommonAPI::util_sleep_ms(timer_list->interval_ms_);
            timer_list->timer_loop();
        }
        timer_list->timer_loop_thread_.exit();
        return 0;
    }

    inline int add_timer_i(SL_Timer *timer)
    {
        if (timer->state_ == SL_Timer::TIMER_STATE_ALIVE)
        {
            return -1;
        }

        timer->expire_time_.end_clock = SL_Socket_CommonAPI::util_process_clock_ms()+timer->interval_ms_;
        if (timer_list_.empty())
        {
            timer_list_.push_back(timer);
        }
        else
        {
            SL_Timer *temp_timer;
            std::list<SL_Timer*>::iterator iter = timer_list_.begin();
            std::list<SL_Timer*>::iterator iter_end = timer_list_.end();
            for (; iter != iter_end; ++iter)
            {
                temp_timer = *iter;
                if (timer->expire_time_.end_clock <= temp_timer->expire_time_.end_clock)
                {
                    timer_list_.insert(iter, timer);
                    return 0;
                }
            }
            timer_list_.push_back(timer);
        }
        return 0;
    }

    inline int del_timer_i(SL_Timer *timer)
    {
        if (timer->state_ != SL_Timer::TIMER_STATE_ALIVE)
        {
            return -1;
        }
        timer_list_.remove(timer);
        timer->state_ = SL_Timer::TIMER_STATE_IDLE;
        return 0;
    }

    inline int clear_timer_i()
    {
        timer_list_.clear();
        return 0;
    }

    TSyncMutex              mutex_;
    std::list<SL_Timer* >   timer_list_;

    uint                    repair_ms_;
    uint                    interval_ms_;
    uint                    max_timer_id_;

    SL_Thread               timer_loop_thread_;
};

#endif

