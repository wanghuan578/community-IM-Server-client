#ifndef SOCKETLITE_DISRUPTOR_SINGLE_WRITE_QUEUE_H
#define SOCKETLITE_DISRUPTOR_SINGLE_WRITE_QUEUE_H

#include "SL_Config.h"
#include "SL_Hash_Fun.h"
#include "SL_Sync_Atomic.h"
#include "SL_Disruptor_Event.h"
#include "SL_Disruptor_Interface.h"

class SL_Disruptor_SingleWriteQueue : public SL_Disruptor_IEventQueue
{
public:
    inline SL_Disruptor_SingleWriteQueue()
        : event_pool_(NULL)
        , cursor_index_(0)
        , capacity_(0)
        , index_bit_mask_(0)
        , event_max_len_(0)
        , rewrite_count_(-1)
        , dependent_flag_(false)
        , notify_flag_(false)
    {
    }

    virtual ~SL_Disruptor_SingleWriteQueue()
    {
        if (NULL != event_pool_)
        {
            free(event_pool_);
        }
    }

    inline int init(uint capacity, uint event_max_len=64, int rewrite_count=-1, int reread_count=-1)
    {
        int i = 0;
        for (; i<SL_NUM_2_POW; ++i)
        {
            if (capacity <= SL_2_POW_LIST[i])
            {
                capacity = SL_2_POW_LIST[i];
                break;
            }
        }
        if (i == SL_NUM_2_POW)
        {
            capacity = SL_2_POW_LIST[SL_NUM_2_POW-1];
        }

        int pool_size = capacity * event_max_len;
        event_pool_ = (char *)malloc(pool_size);
        if (NULL != event_pool_)
        {
            capacity_       = capacity;
            index_bit_mask_ = capacity - 1;
            event_max_len_  = event_max_len;
            cursor_index_.store(0);
            return 1;
        }
        return -1;
    }

    inline void clear()
    {
        if (NULL != event_pool_)
        {
            free(event_pool_);
            event_pool_ = NULL;
        }
        capacity_       = 0;
        index_bit_mask_ = 0;
        event_max_len_  = 0;
        rewrite_count_  = -1;
        dependent_flag_ = false;
        notify_flag_    = false;
        cursor_index_.store(0);
        dependent_list_.clear();
        notify_list_.clear();
    }

    inline int push(const SL_Disruptor_Event *event, bool timedwait_signal=true, int redo_count=0)
    {
        int64   cursor_index = cursor_index_.get();
        int64   handler_index;
        uint    queue_size;

        if (0 == redo_count)
        {
            redo_count = rewrite_count_;
        }

        if (dependent_flag_)
        {
            if (redo_count < 0)
            {
                while (1)
                {
                    handler_index   = min_dependent_index_i();
                    queue_size      = (uint)(cursor_index - handler_index);
                    if (queue_size < capacity_)
                    {
                        uint offset = (cursor_index & index_bit_mask_) * event_max_len_;
                        memcpy(event_pool_ + offset, event->get_event_buffer(), event->get_event_len());
                        ++cursor_index_;

                        if (timedwait_signal && notify_flag_)
                        {
                            std::list<SL_Disruptor_IHandler* >::iterator iter = notify_list_.begin();
                            std::list<SL_Disruptor_IHandler* >::iterator iter_end = notify_list_.end();
                            for (; iter != iter_end; ++iter)
                            {
                                (*iter)->signal_event(cursor_index);
                            }
                        }
                        return queue_size + 1;
                    }
                }
            }
            else
            {
                for (int i=0; i<redo_count; ++i)
                {
                    handler_index   = min_dependent_index_i();
                    queue_size      = (uint)(cursor_index - handler_index);
                    if (queue_size < capacity_)
                    {
                        uint offset = (cursor_index & index_bit_mask_) * event_max_len_;
                        memcpy(event_pool_ + offset, event->get_event_buffer(), event->get_event_len());
                        ++cursor_index_;

                        if (timedwait_signal && notify_flag_)
                        {
                            std::list<SL_Disruptor_IHandler* >::iterator iter = notify_list_.begin();
                            std::list<SL_Disruptor_IHandler* >::iterator iter_end = notify_list_.end();
                            for (; iter != iter_end; ++iter)
                            {
                                (*iter)->signal_event(cursor_index);
                            }
                        }
                        return queue_size + 1;
                    }
                }
            }
        }

        return -1;
    }

    inline uint capacity() const
    {
        return capacity_;
    }

    inline uint size()
    {
        int64 cursor_index      = cursor_index_.load();
        int64 dependent_index   = min_dependent_index();
        return (uint)(cursor_index - dependent_index);
    }

    inline bool empty()
    {
        int64 cursor_index      = cursor_index_.load();
        int64 dependent_index   = min_dependent_index();
        if (cursor_index - dependent_index > 0)
        {
            return false;
        }
        return true;
    }

    inline void add_handler(SL_Disruptor_IHandler *handler, bool dependent_flag, bool notify_flag)
    {
        if (dependent_flag)
        {
            dependent_list_.push_back(handler);
            dependent_flag_ = true;
        }
        if (notify_flag)
        {
            notify_list_.push_back(handler);
            notify_flag_ = true;
        }
    }

    inline int64 cursor_index()
    {
        return cursor_index_.load();
    }

    inline int quit_event()
    {
        SL_Disruptor_QuitEvent quit_event;
        return push(&quit_event, true, -1);
    }

    inline SL_Disruptor_Event* get_event(int64 event_index)
    {
        uint offset = (event_index & index_bit_mask_) * event_max_len_;
        return (SL_Disruptor_Event *)(event_pool_ + offset);
    }

private:
    inline int64 min_dependent_index()
    {
        if (dependent_flag_)
        {
            std::list<SL_Disruptor_IHandler* >::iterator iter = dependent_list_.begin();
            std::list<SL_Disruptor_IHandler* >::iterator iter_end = dependent_list_.end();
            int64 index = (*iter)->handler_index();
            int64 temp;
            ++iter;
            for (; iter != iter_end; ++iter)
            {
                temp = (*iter)->handler_index();
                if (index > temp)
                {
                    index = temp;
                }
            }
            return index;
        }
        return 0;
    }

    inline int64 min_dependent_index_i()
    {
        std::list<SL_Disruptor_IHandler* >::iterator iter = dependent_list_.begin();
        std::list<SL_Disruptor_IHandler* >::iterator iter_end = dependent_list_.end();
        int64 index = (*iter)->handler_index();
        int64 temp;
        ++iter;
        for (; iter != iter_end; ++iter)
        {
            temp = (*iter)->handler_index();
            if (index > temp)
            {
                index = temp;
            }
        }
        return index;
    }

private:
    char                    *event_pool_;                   //事件池
    SL_Sync_Atomic_Int64    cursor_index_;                  //队列写位置(只增不减)
    uint                    capacity_;                      //队列容量(必须为2的N次方)
    uint                    index_bit_mask_;                //下标位掩码(为capacity_ - 1)
    uint                    event_max_len_;                 //事件对象最大大小
    int                     rewrite_count_;                 //重写次数(-1:无限次数)

    std::list<SL_Disruptor_IHandler* >  dependent_list_;    //依赖列表
    std::list<SL_Disruptor_IHandler* >  notify_list_;       //通知列表
    bool dependent_flag_;
    bool notify_flag_;
};

#endif

