#include "SL_Socket_CommonAPI.h"
#include "SL_Seda_TimerQueue.h"
#include "SL_Seda_Event.h"

SL_Seda_TimerQueue::SL_Seda_TimerQueue()
    : stage_handler_(NULL)
    , pool_(NULL)
    , last_clock_ms_(0)
    , queue_max_size_(0)
{
}

SL_Seda_TimerQueue::~SL_Seda_TimerQueue()
{
    clear();
}

int SL_Seda_TimerQueue::init(uint queue_max_size, uint interval_ms, SL_Seda_IStageHandler *stage_handler, int slot)
{
    clear();

    pool_ = new SL_Seda_Timer[queue_max_size];
    if (NULL == pool_)
    {
        return -1;
    }

    for (uint i=0; i<queue_max_size; ++i)
    {
#ifdef SOCKETLITE_DEBUG
        pool_[i].param_ = i;
#endif
        free_list_.push_back(&pool_[i]);
    }
    stage_handler_ = stage_handler;
    return 0;
}

int SL_Seda_TimerQueue::clear()
{
    if (NULL != pool_)
    {
        delete []pool_;
        pool_ = NULL;
    }
    free_list_.init();
    active_list_.init();
    last_clock_ms_  = 0;
    queue_max_size_ = 0;
    stage_handler_  = NULL;
    return 0;
}

SL_Seda_Timer* SL_Seda_TimerQueue::set_timer(uint interval_ms, SL_Seda_Timer::TimerParam param)
{
    if (!free_list_.empty())
    {
        SL_Seda_Timer *timer = free_list_.front();
        free_list_.pop_front();
        timer->init();
        timer->param_        = param;
        timer->interval_ms_  = interval_ms;
        timer->end_clock_ms_ = SL_Socket_CommonAPI::util_process_clock_ms() + interval_ms;
        timer->is_active_    = true;

        //insert to active_list_
        if (active_list_.empty())
        {
            active_list_.push_back(timer);
        }
        else
        {
            SL_Seda_Timer *pos = active_list_.back();
            uint active_list_size = active_list_.size();
            for (uint i=0; i<active_list_size; ++i)
            {
                if (timer->end_clock_ms_ > pos->end_clock_ms_)
                {
                    active_list_.insert_after(pos, timer);
                    return timer;
                }
                pos = pos->prev_;
            }
            active_list_.push_front(timer);
        }
        return timer;
    }
    return NULL;
}

int SL_Seda_TimerQueue::cancel_timer(SL_Seda_Timer *timer)
{
    if (NULL != timer)
    {
        if (!timer->is_active_)
        {
            return -1;
        }
        active_list_.remove(timer);
        free_list_.push_front(timer);
    }
    return 0;
}

int SL_Seda_TimerQueue::update_timer(SL_Seda_Timer *timer)
{
    if (NULL != timer)
    {
        if (!timer->is_active_)
        {
            return -1;
        }
        active_list_.remove(timer);
        timer->end_clock_ms_ = SL_Socket_CommonAPI::util_process_clock_ms() + timer->interval_ms_;
        timer->is_active_    = true;

        //insert to active_list_
        if (active_list_.empty())
        {
            active_list_.push_back(timer);
        }
        else
        {
            SL_Seda_Timer *pos = active_list_.back();
            uint active_list_size = active_list_.size();
            for (uint i=0; i<active_list_size; ++i)
            {
                if (timer->end_clock_ms_ > pos->end_clock_ms_)
                {
                    active_list_.insert_after(pos, timer);
                    return 0;
                }
                pos = pos->prev_;
            }
            active_list_.push_front(timer);
        }
    }
    return 0;
}

int SL_Seda_TimerQueue::expire(uint64 current_clock_ms, SL_Seda_TimerExpireEvent *event)
{
    if (current_clock_ms > last_clock_ms_)
    {
        last_clock_ms_ = current_clock_ms;
    }
    else
    {
        return 0;
    }

    if (NULL != stage_handler_)
    {
        while (!active_list_.empty())
        {
            SL_Seda_Timer *timer = active_list_.front();
            if (current_clock_ms >= timer->end_clock_ms_)
            {
                active_list_.pop_front();
                free_list_.push_front(timer);
                event->slot  = -1;
                event->timer = timer;
                stage_handler_->handle_event(event);
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        while (!active_list_.empty())
        {
            SL_Seda_Timer *timer = active_list_.front();
            if (current_clock_ms >= timer->end_clock_ms_)
            {
                active_list_.pop_front();
                free_list_.push_front(timer);
                timer->timeout();
            }
            else
            {
                return 0;
            }
        }
    }
    return 0;
}

SL_Seda_LRUTimerQueue::SL_Seda_LRUTimerQueue()
    : stage_handler_(NULL)
    , pool_(NULL)
    , last_clock_ms_(0)
    , interval_ms_(0)
    , queue_max_size_(0)
    , slot_(-1)
{
}

SL_Seda_LRUTimerQueue::~SL_Seda_LRUTimerQueue()
{
    clear();
}

int SL_Seda_LRUTimerQueue::init(uint queue_max_size, uint interval_ms, SL_Seda_IStageHandler *stage_handler, int slot)
{
    clear();

    pool_ = new SL_Seda_Timer[queue_max_size];
    if (NULL == pool_)
    {
        return -1;
    }

    for (uint i=0; i<queue_max_size; ++i)
    {
#ifdef SOCKETLITE_DEBUG
        pool_[i].param_ = i;
#endif
        free_list_.push_back(&pool_[i]);
    }
    interval_ms_    = interval_ms;
    stage_handler_  = stage_handler;
    slot_           = slot;
    return 0;
}

int SL_Seda_LRUTimerQueue::clear()
{
    if (NULL != pool_)
    {
        delete []pool_;
        pool_ = NULL;
    }
    free_list_.init();
    active_list_.init();
    last_clock_ms_  = 0;
    queue_max_size_ = 0;
    stage_handler_  = NULL;
    return 0;
}

SL_Seda_Timer* SL_Seda_LRUTimerQueue::set_timer(ulong interval_ms, int64 param)
{
    if (!free_list_.empty())
    {
        SL_Seda_Timer *timer = free_list_.front();
        free_list_.pop_front();
        timer->init();
        timer->param_        = param;
        timer->interval_ms_  = interval_ms_;
        timer->end_clock_ms_ = SL_Socket_CommonAPI::util_process_clock_ms() + interval_ms_;
        timer->is_active_    = true;

        //insert to active list
        active_list_.push_back(timer);
        return timer;
    }
    return NULL;
}

int SL_Seda_LRUTimerQueue::cancel_timer(SL_Seda_Timer *timer)
{
    if (NULL != timer)
    {
        if (!timer->is_active_)
        {
            return -1;
        }
        active_list_.remove(timer);
        free_list_.push_front(timer);
    }
    return 0;
}

int SL_Seda_LRUTimerQueue::update_timer(SL_Seda_Timer *timer)
{
    if (NULL != timer)
    {
        if (!timer->is_active_)
        {
            return -1;
        }
        active_list_.remove(timer);
        timer->end_clock_ms_ = SL_Socket_CommonAPI::util_process_clock_ms() + timer->interval_ms_;
        timer->is_active_    = true;

        //insert to active list
        active_list_.push_back(timer);
    }
    return 0;
}

int SL_Seda_LRUTimerQueue::expire(uint64 current_clock_ms, SL_Seda_TimerExpireEvent *event)
{
    if (current_clock_ms > last_clock_ms_)
    {
        last_clock_ms_ = current_clock_ms;
    }
    else
    {
        return 0;
    }

    if (NULL != stage_handler_)
    {
        while (!active_list_.empty())
        {
            SL_Seda_Timer *timer = active_list_.front();
            if (current_clock_ms >= timer->end_clock_ms_)
            {
                active_list_.pop_front();
                free_list_.push_front(timer);
                event->slot  = slot_;
                event->timer = timer;
                stage_handler_->handle_event(event);
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        while (!active_list_.empty())
        {
            SL_Seda_Timer *timer = active_list_.front();
            if (current_clock_ms >= timer->end_clock_ms_)
            {
                active_list_.pop_front();
                free_list_.push_front(timer);
                timer->timeout();
            }
            else
            {
                return 0;
            }
        }
    }
    return 0;
}

