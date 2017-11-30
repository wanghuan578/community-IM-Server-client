#ifndef SOCKETLITE_TIMER_H
#define SOCKETLITE_TIMER_H
#include "SL_Config.h"
#include "SL_Timer_Queue.h"

template <typename TSyncMutex> class SL_Timer_List_Time;
template <typename TSyncMutex> class SL_Timer_List_Clock;

class SOCKETLITE_API SL_Timer
{
public:
    typedef enum
    {
        TIMER_STATE_IDLE = 0,
        TIMER_STATE_ALIVE,
        TIMER_STATE_TIMEOUT
    }TimerState;
    typedef enum
    {
        TIMER_TYPE_ONCE = 0,
        TIMER_TYPE_CYCLE
    }TimerType;

    SL_Timer()
        : id_(0)
        , state_(TIMER_STATE_IDLE)
        , timer_queue_(NULL)
    {
    }

    SL_Timer(SL_Timer_Queue *timer_queue)
        : id_(0)
        , state_(TIMER_STATE_IDLE)
        , timer_queue_(timer_queue)
    {
    }

    virtual ~SL_Timer() 
    {
    }

    void set_config(uint user_define_id, 
                    uint interval_ms = 10, 
                    void *timer_data = 0, 
                    TimerType type = TIMER_TYPE_CYCLE, 
                    SL_Timer_Queue *timer_queue = NULL)
    {
        user_define_id_ = user_define_id;
        interval_ms_    = interval_ms;
        timer_data_     = timer_data;
        type_           = type;
        timer_queue_    = timer_queue;
    }

    inline int start()
    {
        if (timer_queue_ != NULL)
        {
            timer_queue_->add_timer(this);
        }
        return 0;
    }

    inline int stop()
    {
        if (timer_queue_ != NULL)
        {
            timer_queue_->del_timer(this);
        }
        return 0;
    }

    virtual int timeout() 
    { 
        return 0; 
    }

protected:
    uint                id_;
    uint                user_define_id_;
    uint                interval_ms_;
    TimerState          state_;
    TimerType           type_;
    void                *timer_data_;
    SL_Timer_Queue      *timer_queue_;

    union
    {
        struct timeval  end_time;
        uint64          end_clock;
    }expire_time_;
    
    template <typename TSyncMutex> friend class SL_Timer_List_Time;
    template <typename TSyncMutex> friend class SL_Timer_List_Clock;
};

#endif

