#ifndef SOCKETLITE_SEDA_EVENT_QUEUE_H
#define SOCKETLITE_SEDA_EVENT_QUEUE_H

#include "SL_Config.h"
#include "SL_Sync_Mutex.h"
#include "SL_Sync_SpinMutex.h"
#include "SL_Sync_Guard.h"
#include "SL_Seda_Event.h"
#include "SL_Seda_Interface.h"

class SL_Seda_EventQueue : public SL_Seda_IEventQueue
{
public:
    inline SL_Seda_EventQueue()
        : pool_begin_(NULL)
        , pool_end_(NULL)
        , write_index_(NULL)
        , read_index_(NULL)
        , capacity_(0)
        , queue_size_(0)
        , event_max_len_(0)
    {
    }

    virtual ~SL_Seda_EventQueue()
    {
        if (NULL != pool_begin_)
        {
            free(pool_begin_);
        }
    }

    inline int init(uint capacity, uint event_max_len = 64)
    {
        clear();

        int pool_size = capacity * event_max_len;
        pool_begin_ = (char *)malloc(pool_size);
        if (NULL != pool_begin_)
        {
            pool_end_       = pool_begin_ + pool_size;
            write_index_    = pool_begin_;
            read_index_     = pool_begin_;
            capacity_       = capacity;
            queue_size_     = 0;
            event_max_len_  = event_max_len;
            return 1;
        }
        return -1;
    }

    inline void clear()
    {
        if (NULL != pool_begin_)
        {
            free(pool_begin_);
            pool_begin_ = NULL;
            pool_end_   = NULL;
        }
        write_index_    = NULL;
        read_index_     = NULL;
        capacity_       = 0;
        queue_size_     = 0;
        event_max_len_  = 0;
    }

    inline int push(const SL_Seda_Event *event)
    {
        if (queue_size_ < capacity_)
        {
            memcpy(write_index_, event->get_event_buffer(), event->get_event_len());
            write_index_ += event_max_len_;
            if (write_index_ >= pool_end_)
            {
                write_index_ = pool_begin_;
            }
            int ret = ++queue_size_;
            return ret;
        }
        return -1;
    }

    inline int pop(SL_Seda_Event **event)
    {
        if (queue_size_ > 0)
        {
            *event = (SL_Seda_Event *)read_index_;
            read_index_ += event_max_len_;
            if (read_index_ >= pool_end_)
            {
                read_index_ = pool_begin_;
            }            
            --queue_size_;
            return 1;
        }
        return -1;
    }

    inline uint capacity() const
    {
        return capacity_;
    }

    inline uint size() const
    {
        return queue_size_;
    }

    inline bool empty() const
    {
        return 0 == queue_size_;
    }

private:
    char *pool_begin_;          //事件池开始位
    char *pool_end_;            //事件池结束位
    char *write_index_;         //写位置
    char *read_index_;          //读位置

    uint capacity_;             //队列容量
    uint queue_size_;           //队列大小
    uint event_max_len_;        //事件对象最大大小
};

#endif

