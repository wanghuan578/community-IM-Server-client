#ifndef SOCKETLITE_CIRCULAR_QUEUE_H
#define SOCKETLITE_CIRCULAR_QUEUE_H
#include <vector>
#include "SL_DataType.h"
#include "SL_Sync_Mutex.h"
#include "SL_Sync_SpinMutex.h"

//循环队列(也称环形队列)
//优点：不用频繁分配和释放节点

template <typename TValue, typename TSyncMutex=SL_Sync_NullMutex>
class SL_CircularQueue
{
public:
    inline SL_CircularQueue()
        : pool_begin_(NULL)
        , pool_end_(NULL)
        , read_index_(NULL)
        , write_index_(NULL)
        , capacity_(0)
        , queue_size_(0)
        , rewrite_flag_(false)
    {
    }

    inline SL_CircularQueue(uint capacity, bool rewrite=false)
        : pool_end_(NULL)
        , read_index_(NULL)
        , write_index_(NULL)
        , capacity_(0)
        , queue_size_(0)
        , rewrite_flag_(false)
    {
        pool_begin_ = new TValue[capacity];
        if (NULL != pool_begin_)
        {
            capacity_       = capacity;
            pool_end_       = pool_begin_ + capacity;
            read_index_     = pool_begin_;
            write_index_    = pool_begin_;
            rewrite_flag_   = rewrite;
        }
    }

    inline ~SL_CircularQueue()
    {
        if (NULL != pool_begin_)
        {
            delete []pool_begin_;
        }
    }

    inline void clear()
    {
        if (NULL != pool_begin_)
        {
            delete []pool_begin_;
        }
        pool_begin_     = NULL;
        pool_end_       = NULL;
        read_index_     = NULL;
        write_index_    = NULL;
        capacity_       = 0;
        queue_size_     = 0;
    }

    inline int init(uint capacity, bool rewrite = false)
    {
        clear();

        pool_begin_ = new TValue[capacity];
        if (NULL != pool_begin_)
        {
            capacity_       = capacity;
            pool_end_       = pool_begin_ + capacity;
            read_index_     = pool_begin_;
            write_index_    = pool_begin_;
            rewrite_flag_   = rewrite;
            return 1;
        }
        return 0;
    }

    inline uint capacity() const
    {
        return capacity_;
    }

    inline uint size() const
    {
        mutex_.lock();
        uint ret = queue_size_;
        mutex_.unlock;
        return ret;
    }

    inline bool empty() const
    {
        mutex_.lock();
        uint ret = queue_size_;
        mutex_.unlock;
        return 0 == ret;
    }

    inline bool full() const
    {
        mutex_.lock();
        uint ret = queue_size_;
        mutex_.unlock;
        return ret == capacity_;
    }

    inline int push(const TValue &value, bool rewrite = false)
    {
        int ret = -1;
        mutex_.lock();
        if (queue_size_ < capacity_)
        {
            *write_index_ = value;
            if (++write_index_ >= pool_end_)
            {
                write_index_ = pool_begin_;
            }
            ret = ++queue_size_;
        }
        else if (rewrite && rewrite_flag_)
        {
            *write_index_ = value;
            if (++write_index_ >= pool_end_)
            {
                write_index_ = pool_begin_;
            }
            if (++read_index_ >= pool_end_)
            {
                read_index_ = pool_begin_;
            }
            ret = queue_size_;
        }
        mutex_.unlock();
        return ret;
    }

    inline int pop(TValue &value)
    {
        mutex_.lock();
        if (queue_size_ > 0)
        {
            value = *read_index_;
            if (++read_index_ >= pool_end_)
            {
                read_index_ = pool_begin_;
            }
            --queue_size_;
            mutex_.unlock();
            return 1;
        }
        mutex_.unlock();
        return -1;
    }

    inline TValue& front()
    {
        return *read_index_;
    }

    inline TValue& back()
    {
        return *write_index_;
    }

    inline int push_back(const TValue &value, bool rewrite = false)
    {
        return push(value, rewrite);
    }

    inline int pop_front(TValue &value)
    {
        return pop(value);
    }

private:
    inline uint size_i() const
    {
        return queue_size_;
    }

    inline bool empty_i() const
    {
        return 0 == queue_size_;
    }

    inline bool full_i() const
    {
        return queue_size_ == capacity_;
    }

private:
    TValue      *pool_begin_;
    TValue      *pool_end_;
    TValue      *read_index_;
    TValue      *write_index_;

    uint        capacity_;          //队列容量
    uint        queue_size_;        //队列大小
    bool        rewrite_flag_;

    TSyncMutex  mutex_;
};

#endif

