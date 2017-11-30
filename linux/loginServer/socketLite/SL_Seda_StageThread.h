#ifndef SOCKETLITE_SEDA_STAGE_THREAD_H
#define SOCKETLITE_SEDA_STAGE_THREAD_H

#include <algorithm>
#include <vector>
#include "SL_DataType.h"
#include "SL_Socket_CommonAPI.h"
#include "SL_Thread.h"
#include "SL_Sync_Mutex.h"
#include "SL_Sync_Condition.h"
#include "SL_Sync_Atomic.h"
#include "SL_Seda_Interface.h"
#include "SL_Seda_Event.h"
#include "SL_Seda_EventQueue.h"
#include "SL_Seda_TimerQueue.h"

template <typename TSedaStageHandler>
class SL_Seda_StageThread : public SL_Seda_IStageThread
{
public:
    SL_Seda_StageThread(SL_Seda_IStage *stage, int thread_index, uint queue_max_size, uint event_max_len, uint timedwait_interval_us, bool timedwait_signal)
        : stage_(stage)
        , thread_index_(thread_index)
        , idle_event_flag_(false)
        , idle_interval_ms_(SOCKETLITE_SEDA_DEFAULT_IDLE_INTERVAL)
        , timer_event_flag_(false)
        , timer_min_interval_ms_(SOCKETLITE_SEDA_DEFAULT_IDLE_INTERVAL)
        , timedwait_interval_us_(timedwait_interval_us)
        , timedwait_flag_(0)
        , timedwait_signal_(timedwait_signal)
    {
        stage_handler_.set_stage_thread(this);
        event_queue1_.init(queue_max_size, event_max_len);
        event_queue2_.init(queue_max_size, event_max_len);
        event_queue_active_  = &event_queue1_;
        event_queue_standby_ = &event_queue2_;
    }

    virtual ~SL_Seda_StageThread()
    {
    }

    inline int start()
    {
        return thread_.start(thread_proc, this);
    }

    inline void stop()
    {
        SL_Seda_QuitEvent quit_event;
        event_queue_standby_->push(&quit_event);
        timedwait_condition_.signal();
        thread_.stop();
    }

    inline void join()
    {
        return thread_.join();
    }

    inline int push_event(const SL_Seda_Event *event, bool timedwait_signal)
    {
        event_queue_mutex_.lock();
        int ret = event_queue_standby_->push(event);
        event_queue_mutex_.unlock();
        if ((1 == ret) && timedwait_signal && timedwait_signal_)
        {
            if (timedwait_flag_.load() > 0)
            {
                timedwait_flag_.store(0);
                timedwait_condition_.signal();
            }
        }
        return ret;
    }

    inline SL_Seda_Timer* set_timer(uint interval_ms, SL_Seda_Timer::TimerParam param)
    {
        if ( (interval_ms < timer_min_interval_ms_) || (0 == timer_min_interval_ms_) )
        {
            timer_min_interval_ms_ = interval_ms;
        }
        return timer_queue_.set_timer(interval_ms, param);
    }

    inline int cancel_timer(SL_Seda_Timer *timer)
    {
        return timer_queue_.cancel_timer(timer);
    }

    inline int update_timer(SL_Seda_Timer *timer)
    {
        return timer_queue_.update_timer(timer);
    }

    inline int enable_timer_event(uint capacity)
    {
        return timer_queue_.init(capacity, 0, &stage_handler_, -1);
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
                int ret = lru_timer_managers_[i].init(slots[i].capacity, slots[i].interval_ms, &stage_handler_, i);
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

    inline int enable_idle_event(bool enabled)
    {
        idle_event_flag_ = enabled;
        return 0;
    }

    inline int set_idle_interval(uint idle_interval_ms)
    {
        idle_interval_ms_ = idle_interval_ms;
        return 0;
    }

    inline int set_sleep_interval(uint sleep_interval_us)
    {
        timedwait_interval_us_ = sleep_interval_us;
        return 0;
    }

    inline int set_timedwait_interval(uint timedwait_interval_us)
    {
        timedwait_interval_us_ = timedwait_interval_us;
        return 0;
    }

    inline SL_Seda_IStage* get_stage() const
    {
        return stage_;
    }

    inline uint get_thread_index() const
    {
        return thread_index_;
    }

private:
    SL_Seda_StageThread()
    {
    }

    inline void check_timers(uint64 current_clock_ms, SL_Seda_TimerExpireEvent *event)
    {
        int timer_size = (int)lru_timer_managers_.size();
        for (int i=0; i<timer_size; ++i)
        {
            lru_timer_managers_[i].expire(current_clock_ms, event);
        }

        //普通定时器很少使用,故取消
        //timer_queue_.expire(current_clock_ms, event);
    }

    //交换event_queue的 active/standby 指针
    inline bool swap_event_queue()
    {
        if (!event_queue_standby_->empty())
        {
            event_queue_mutex_.lock();
            std::swap(event_queue_standby_, event_queue_active_);
            event_queue_mutex_.unlock();
            return true;
        }
        return false;
    }

#ifdef SOCKETLITE_OS_WINDOWS
    static unsigned int WINAPI thread_proc(void *arg)
#else
    static void* thread_proc(void *arg)
#endif
    {
        SL_Seda_StageThread<TSedaStageHandler> *stage_thread = (SL_Seda_StageThread<TSedaStageHandler> *)arg;
        stage_thread->stage_handler_.handle_open();

        uint64  current_clock_ms    = SL_Socket_CommonAPI::util_process_clock_ms();
        uint64  idle_last_clock_ms  = current_clock_ms;
        int     timer_event_count   = 0;
        uint    idle_interval_ms    = stage_thread->idle_interval_ms_;
        bool    idle_event_flag     = stage_thread->idle_event_flag_;
        bool    timer_event_flag    = stage_thread->timer_event_flag_;

        uint timedwait_interval_us  = stage_thread->timer_min_interval_ms_ * 1000;
        if (timedwait_interval_us > stage_thread->timedwait_interval_us_)
        {
            timedwait_interval_us = stage_thread->timedwait_interval_us_;
        }

        SL_Seda_ThreadIdleEvent  idle_event;
        SL_Seda_TimerExpireEvent timer_expire_event;
        SL_Seda_Event *event = NULL;

        if (!timer_event_flag)
        {
            if (!idle_event_flag)
            {//timer_event_flag = false and idle_event_flag = false
                while (1)
                {
                    if (stage_thread->event_queue_active_->pop(&event) > 0)
                    {
                        if (SL_Seda_EventType::QUIT_EVENT != event->type_)
                        {
                            stage_thread->stage_handler_.handle_event(event);
                        }
                        else
                        {
                            goto QUIT_THREAD_PROC;
                        }
                    }
                    else
                    {
                        if (!stage_thread->swap_event_queue())
                        {
                            stage_thread->timedwait_flag_.store(1);
#ifdef SOCKETLITE_OS_WINDOWS
                            stage_thread->timedwait_condition_.timed_wait(NULL, timedwait_interval_us);
#else
                            stage_thread->timedwait_mutex_.lock();
                            stage_thread->timedwait_condition_.timed_wait(&stage_thread->timedwait_mutex_, timedwait_interval_us);
                            stage_thread->timedwait_mutex_.unlock();
#endif
                            stage_thread->timedwait_flag_.store(0);

                            stage_thread->swap_event_queue();
                        }
                    }
                }
            }
            else
            {///timer_event_flag = false and idle_event_flag = true
                while (1)
                {
                    if (stage_thread->event_queue_active_->pop(&event) > 0)
                    {
                        if (SL_Seda_EventType::QUIT_EVENT != event->type_)
                        {
                            stage_thread->stage_handler_.handle_event(event);
                        }
                        else
                        {
                            goto QUIT_THREAD_PROC;
                        }
                    }
                    else
                    {
                        current_clock_ms = SL_Socket_CommonAPI::util_process_clock_ms();
                        if (current_clock_ms - idle_last_clock_ms >= idle_interval_ms)
                        {
                            stage_thread->stage_handler_.handle_event(&idle_event);
                            idle_last_clock_ms = current_clock_ms;
                        }

                        if (!stage_thread->swap_event_queue())
                        {
                            stage_thread->timedwait_flag_.store(1);
#ifdef SOCKETLITE_OS_WINDOWS
                            stage_thread->timedwait_condition_.timed_wait(NULL, timedwait_interval_us);
#else
                            stage_thread->timedwait_mutex_.lock();
                            stage_thread->timedwait_condition_.timed_wait(&stage_thread->timedwait_mutex_, timedwait_interval_us);
                            stage_thread->timedwait_mutex_.unlock();
#endif
                            stage_thread->timedwait_flag_.store(0);

                            stage_thread->swap_event_queue();
                        }
                    }            
                }
            }
        }
        else
        {
            if (!idle_event_flag)
            {//timer_event_flag = true and idle_event_flag = false
                while (1)
                {
                    if (stage_thread->event_queue_active_->pop(&event) > 0)
                    {
                        if (SL_Seda_EventType::QUIT_EVENT != event->type_)
                        {
                            stage_thread->stage_handler_.handle_event(event);

                            ++timer_event_count;
                            if (timer_event_count == SOCKETLITE_SEDA_TIMER_EVENT_FACTOR)
                            {
                                stage_thread->check_timers(SL_Socket_CommonAPI::util_process_clock_ms(), &timer_expire_event);
                                timer_event_count = 0;
                            }
                        }
                        else
                        {
                            goto QUIT_THREAD_PROC;
                        }
                    }
                    else
                    {
                        current_clock_ms = SL_Socket_CommonAPI::util_process_clock_ms();
                        stage_thread->check_timers(current_clock_ms, &timer_expire_event);
                        timer_event_count = 0;

                        if (!stage_thread->swap_event_queue())
                        {
                            stage_thread->timedwait_flag_.store(1);
#ifdef SOCKETLITE_OS_WINDOWS
                            stage_thread->timedwait_condition_.timed_wait(NULL, timedwait_interval_us);
#else
                            stage_thread->timedwait_mutex_.lock();
                            stage_thread->timedwait_condition_.timed_wait(&stage_thread->timedwait_mutex_, timedwait_interval_us);
                            stage_thread->timedwait_mutex_.unlock();
#endif
                            stage_thread->timedwait_flag_.store(0);

                            stage_thread->swap_event_queue();
                        }
                    }            
                }
            }
            else
            {//timer_event_flag = true and idle_event_flag = true
                while (1)
                {
                    if (stage_thread->event_queue_active_->pop(&event) > 0)
                    {
                        if (SL_Seda_EventType::QUIT_EVENT != event->type_)
                        {
                            stage_thread->stage_handler_.handle_event(event);

                            ++timer_event_count;
                            if (timer_event_count == SOCKETLITE_SEDA_TIMER_EVENT_FACTOR)
                            {
                                stage_thread->check_timers(SL_Socket_CommonAPI::util_process_clock_ms(), &timer_expire_event);
                                timer_event_count = 0;
                            }
                        }
                        else
                        {
                            goto QUIT_THREAD_PROC;
                        }
                    }
                    else
                    {
                        current_clock_ms = SL_Socket_CommonAPI::util_process_clock_ms();
                        stage_thread->check_timers(current_clock_ms, &timer_expire_event);
                        timer_event_count = 0;
                        if (current_clock_ms - idle_last_clock_ms >= idle_interval_ms)
                        {
                            stage_thread->stage_handler_.handle_event(&idle_event);
                            idle_last_clock_ms = current_clock_ms;
                        }

                        if (!stage_thread->swap_event_queue())
                        {
                            stage_thread->timedwait_flag_.store(1);
#ifdef SOCKETLITE_OS_WINDOWS
                            stage_thread->timedwait_condition_.timed_wait(NULL, timedwait_interval_us);
#else
                            stage_thread->timedwait_mutex_.lock();
                            stage_thread->timedwait_condition_.timed_wait(&stage_thread->timedwait_mutex_, timedwait_interval_us);
                            stage_thread->timedwait_mutex_.unlock();
#endif
                            stage_thread->timedwait_flag_.store(0);

                            stage_thread->swap_event_queue();
                        }
                    }
                }
            }
        }

QUIT_THREAD_PROC:
        stage_thread->stage_handler_.handle_close();
        stage_thread->thread_.exit();
        return 0;
    }

private:
    SL_Seda_IStage                      *stage_;
    TSedaStageHandler                   stage_handler_;
    uint                                thread_index_;              //线程在线程集合中索引
    SL_Thread                           thread_;

    bool                                idle_event_flag_;
    uint                                idle_interval_ms_;
    bool                                timer_event_flag_;          //定时器事件触发标志
    uint                                timer_min_interval_ms_;     //定时器最小间隔(ms)
    SL_Seda_TimerQueue                  timer_queue_;
    std::vector<SL_Seda_LRUTimerQueue>  lru_timer_managers_;

    //采用双队列提高性能
    SL_Seda_EventQueue                  *event_queue_active_;
    SL_Seda_EventQueue                  *event_queue_standby_;
    SL_Seda_EventQueue                  event_queue1_;
    SL_Seda_EventQueue                  event_queue2_;
    SL_Sync_SpinMutex                   event_queue_mutex_;

    SL_Sync_ThreadMutex                 timedwait_mutex_;
    SL_Sync_Condition                   timedwait_condition_;
    uint                                timedwait_interval_us_;
    SL_Sync_Atomic_Int32                timedwait_flag_;            //条件触发标识
    bool                                timedwait_signal_;          //用于控制是否触发条件信号(因触发条件信号比较耗时)
};

#endif

