#ifndef SOCKETLITE_SEDA_TIMER_QUEUE_H
#define SOCKETLITE_SEDA_TIMER_QUEUE_H
#include "SL_Seda_Interface.h"
#include "SL_Seda_Timer.h"

class SOCKETLITE_API SL_Seda_TimerList
{
public:
    SL_Seda_TimerList()
        : size_(0)
    {
    }

    ~SL_Seda_TimerList()
    {
        init();
    }
    
    inline int init()
    {
        head_.init();
        size_ = 0;
        return 0;
    }

    inline void push_front(SL_Seda_Timer *timer)
    {
        timer->prev_ = timer;
        if (head_.alone())
        {
            timer->next_ = timer;
            head_.prev_ = timer;
        }
        else
        {
            timer->next_ = head_.next_;
            head_.next_->prev_  = timer;
        }
        head_.next_ = timer;
        ++size_;
    }

    inline void push_back(SL_Seda_Timer *timer)
    {
        timer->next_ = timer;
        if (head_.alone())
        {
            timer->prev_ = timer;
            head_.next_  = timer;
        }
        else
        {
            timer->prev_ = head_.prev_;
            head_.prev_->next_ = timer;
        }
        head_.prev_ = timer;
        ++size_;
    }

    inline void pop_front()
    {
        if (!head_.alone())
        {               
            head_.next_ = head_.next_->next_;
            head_.next_->prev_ = head_.next_;
            if (--size_ == 0)
            {
                head_.init();
            }
        }
    }

    inline void pop_back()
    {
        if (!head_.alone())
        {
            head_.prev_ = head_.prev_->prev_;
            head_.prev_->next_ = head_.prev_;
            if (--size_ == 0)
            {
                head_.init();
            }
        }
    }

    //insert pos before
    inline void insert_before(SL_Seda_Timer *pos, SL_Seda_Timer *timer)
    {
        timer->next_ = pos;
        timer->prev_ = pos->prev_;
        if (pos != pos->prev_)
        {
            pos->prev_->next_ = timer;
        }
        pos->prev_ = timer;
        if (pos == head_.next_)
        {
            head_.next_  = timer;
            timer->prev_ = timer;
        }
        ++size_;
    }
    
    //insert pos after
    inline void insert_after(SL_Seda_Timer *pos, SL_Seda_Timer *timer)
    {
        timer->next_ = pos->next_;
        timer->prev_ = pos;
        if (pos != pos->next_)
        {
            pos->next_->prev_ = timer;
        }
        pos->next_ = timer;
        if (pos == head_.prev_)
        {
            head_.prev_  = timer;
            timer->next_ = timer;
        }
        ++size_;
    }

    inline void remove(SL_Seda_Timer *timer)
    {        
        if (0 == size_)
        {
            return;
        }

        timer->prev_->next_ = timer->next_;
        timer->next_->prev_ = timer->prev_;
        if (timer == head_.next_)
        {
            head_.next_ = timer->next_;
            head_.next_->prev_ = head_.next_;
        }
        if (timer == head_.prev_)
        {
            head_.prev_ = timer->prev_;
            head_.prev_->next_ = head_.prev_;
        }
        if (--size_ == 0)
        {
            head_.init();
        }
        timer->init();
    }

    inline SL_Seda_Timer* front() const
    {
        return head_.next_;
    }

    inline SL_Seda_Timer* back() const
    {
        return head_.prev_;
    }

    inline uint size() const
    {
        return size_;
    }

    inline bool empty() const
    {
        return (0 == size_);
    }

protected:
    SL_Seda_Timer   head_;      //此节点的next_指向第一个节点，prev_指向最后一个节点
    uint            size_;
};

class SOCKETLITE_API SL_Seda_TimerQueue
{
public:
    SL_Seda_TimerQueue();
    ~SL_Seda_TimerQueue();

    int init(uint queue_max_size, uint interval_ms, SL_Seda_IStageHandler *stage_handler=NULL, int slot=-1);
    int clear();

    SL_Seda_Timer* set_timer(uint interval_ms, SL_Seda_Timer::TimerParam param);
    int cancel_timer(SL_Seda_Timer *timer);
    int update_timer(SL_Seda_Timer *timer);
    int expire(uint64 current_clock_ms, SL_Seda_TimerExpireEvent *event);

    inline uint free_timer_size() const
    {
        return free_list_.size();
    }

    inline uint active_timer_size() const
    {
        return active_list_.size();
    }

private:
    SL_Seda_IStageHandler   *stage_handler_;

    SL_Seda_Timer           *pool_;                 //SL_Seda_Timer类型数组池
    SL_Seda_TimerList       free_list_;
    SL_Seda_TimerList       active_list_;

    uint64  last_clock_ms_;
    uint    queue_max_size_;
};

//LRUTimerQueue中所有定时器的时间间隔是固定的
class SOCKETLITE_API SL_Seda_LRUTimerQueue
{
public:
    SL_Seda_LRUTimerQueue();
    ~SL_Seda_LRUTimerQueue();

    int init(uint queue_max_size, uint interval_ms, SL_Seda_IStageHandler *stage_handler=NULL, int slot=-1);
    int clear();

    SL_Seda_Timer* set_timer(ulong interval_ms, SL_Seda_Timer::TimerParam param);
    int cancel_timer(SL_Seda_Timer *timer);
    int update_timer(SL_Seda_Timer *timer);
    int expire(uint64 current_clock_ms, SL_Seda_TimerExpireEvent *event);

    inline uint free_timer_size() const
    {
        return free_list_.size();
    }

    inline uint active_timer_size() const
    {
        return active_list_.size();
    }

private:
    SL_Seda_IStageHandler   *stage_handler_;

    SL_Seda_Timer           *pool_;                 //SL_Seda_Timer类型数组池
    SL_Seda_TimerList       free_list_;
    SL_Seda_TimerList       active_list_;

    uint64  last_clock_ms_;
    uint    interval_ms_;                           //定时间隔(时间单位为ms)
    uint    queue_max_size_;
    int     slot_;
};

#endif

