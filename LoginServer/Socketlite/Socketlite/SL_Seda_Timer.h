#ifndef SOCKETLITE_SEDA_TIMER_H
#define SOCKETLITE_SEDA_TIMER_H
#include "SL_Config.h"

class SL_Seda_TimerList;
class SL_Seda_TimerQueue;
class SL_Seda_LRUTimerQueue;

class SL_Seda_Timer
{
public:
    typedef int64 TimerParam;

    SL_Seda_Timer()
        : param_(0)
        , end_clock_ms_(0)
        , interval_ms_(0)
        , is_active_(false)
    {
        prev_ = this;
        next_ = this;
    }

    ~SL_Seda_Timer()
    {
    }

    inline int init()
    {
        prev_           = this;
        next_           = this;
        param_          = 0;
        end_clock_ms_   = 0;
        interval_ms_    = 0;
        is_active_      = false;
        return 0;
    }

    inline SL_Seda_Timer* prev() const
    {
        return prev_;
    }

    inline SL_Seda_Timer* next() const
    {
        return next_;
    }

    inline bool alone() const
    {
        return next_ == this;
    }

    inline TimerParam param() const
    {
        return param_;
    }

    inline uint interval() const
    {
        return interval_ms_;
    }

    virtual int timeout() 
    { 
        return 0; 
    }

protected:
    SL_Seda_Timer   *prev_;             //初始指向其自身this,防止因NULL而出现异常
    SL_Seda_Timer   *next_;             //初始指向其自身this,防止因NULL而出现异常

    TimerParam      param_;             //外部参数
    uint64          end_clock_ms_;      //定时器触发时间点(时间单位为ms)
    uint            interval_ms_;       //定时间隔(时间单位为ms)
    bool            is_active_;         //表示此定时器是否已激活

    friend class SL_Seda_TimerList;
    friend class SL_Seda_TimerQueue;
    friend class SL_Seda_LRUTimerQueue;
};

struct SL_Seda_StageThreadLRUSlotInfo
{
    uint    interval_ms;
    uint    capacity;
};

#endif

