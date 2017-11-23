#ifndef SOCKETLITE_TIMER_LIST_TIME_H
#define SOCKETLITE_TIMER_LIST_TIME_H

#include "SL_Socket_CommonAPI.h"
#include "SL_Timer.h"
#include "SL_Timer_Queue.h"
#include "SL_Sync_Mutex.h"
#include "SL_Thread.h"
#include <list>

template <typename TSyncMutex>
class SL_Timer_List_Time : public SL_Timer_Queue
{
public:
    SL_Timer_List_Time()
        : max_timer_id_(0)
    {
    }

    virtual ~SL_Timer_List_Time()
    {
        max_timer_id_ = 0;
        clear_timer_i();
    }

    inline int set_config(uint interval_ms = SL_TIMER_DEFULT_INTERVAL, uint repair = SL_TIMER_DEFAUL_REPAIR)
    {
        interval_ms_        = interval_ms;
        repair_.tv_sec      = repair / 1000;
        repair_.tv_usec     = (repair - repair_.tv_sec * 1000) * 1000;
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
        struct timeval now, stand;
        SL_Socket_CommonAPI::util_gettimeofday(&now, NULL);
        SL_Socket_CommonAPI::util_timeradd(&now, &repair_, &stand);

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
            if (SL_Socket_CommonAPI::util_timercmp(&stand, &temp_timer->expire_time_.end_time) >= 0)
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
                iter_end = timer_list_.end();
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
        SL_Timer_List_Time<TSyncMutex> *timer_list = (SL_Timer_List_Time<TSyncMutex>*)arg;
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

    int add_timer_i(SL_Timer *timer)
    {
        if (timer->state_ == SL_Timer::TIMER_STATE_ALIVE)
        {
            return -1;
        }

        struct timeval now, interval;
        timer->state_       = SL_Timer::TIMER_STATE_ALIVE;
        interval.tv_sec     = timer->interval_ms_ / 1000;
        interval.tv_usec    = (timer->interval_ms_ - interval.tv_sec * 1000) * 1000;
        SL_Socket_CommonAPI::util_gettimeofday(&now, 0);
        SL_Socket_CommonAPI::util_timeradd(&now, &interval, &timer->expire_time_.end_time);

        if (timer_list_.empty())
        {
            timer_list_.push_back(timer);
        }
        else
        {
            SL_Timer *temp_timer;
            std::list<SL_Timer* >::iterator iter = timer_list_.begin();
            std::list<SL_Timer* >::iterator iter_end = timer_list_.end();
            for (; iter != iter_end; ++iter)
            {
                temp_timer = *iter;
                if (SL_Socket_CommonAPI::util_timercmp(&timer->expire_time_.end_time, &temp_timer->expire_time_.end_time) <= 0)
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

    struct timeval          repair_;
    uint                    interval_ms_;
    uint                    max_timer_id_;

    SL_Thread               timer_loop_thread_;
};

#endif

