#ifndef SOCKETLITE_UTILITY_CIRCULAR_QUEUE_H
#define SOCKETLITE_UTILITY_CIRCULAR_QUEUE_H
#include "SL_Config.h"
#include "SL_Hash_Fun.h"
#include "SL_Sync_Atomic.h"
#include "SL_Sync_Mutex.h"
#include "SL_Sync_SpinMutex.h"

template <typename TValue, typename TSyncMutex=SL_Sync_NullMutex>
class SL_Utility_CircularQueue
{
public:
    inline SL_Utility_CircularQueue()
        : pool_(NULL)
        , write_index_(0)
        , read_index_(0)
        , capacity_(0)
        , index_bit_mask_(0)
        , rewrite_count_(1)
        , reread_count_(1)
    {
    }

    inline ~SL_Utility_CircularQueue()
    {
        if (NULL != pool_)
        {
            delete []pool_;
        }
    }

    inline int init(uint capacity, int rewrite_count = 1, int reread_count = 1)
    {
        rewrite_count_ = (rewrite_count < 1) ? -1 : rewrite_count;
        reread_count_  = (reread_count < 1) ? -1 : reread_count;

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
            capacity = SL_2_POW_LIST[SL_NUM_2_POW - 1];
        }

        pool_ = new TValue[capacity];
        if (NULL != pool_)
        {
            capacity_       = capacity;
            index_bit_mask_ = capacity - 1;
            write_index_ = 0;
            read_index_  = 0;
            return 1;
        }
        return -1;
    }

    inline void clear()
    {
        if (NULL != pool_)
        {
            delete []pool_;
            pool_ = NULL;
        }
        capacity_       = 0;
        index_bit_mask_ = 0;
        rewrite_count_  = 1;
        reread_count_   = 1;
        write_index_    = 0;
        read_index_     = 0;
    }

    inline int push(const TValue &value, int redo_count = 0)
    {
        uint    queue_size;
        uint    offset;

        if (0 == redo_count)
        {
            redo_count = rewrite_count_;
        }

        if (redo_count > 0)
        {//有限次数
            for (int i=0; i<redo_count; ++i)
            {
                mutex_.lock();
                queue_size  = (uint)(write_index_ - read_index_);
                if (queue_size < capacity_)
                {
                    offset = write_index_ & index_bit_mask_;
                    pool_[offset] = value;

                    ++write_index_;
                    mutex_.unlock();
                    return queue_size - 1;
                }
                mutex_.unlock();
            }
        }
        else
        {//无限次数
            while (1)
            {
                mutex_.lock();
                queue_size  = (uint)(write_index_ - read_index_);
                if (queue_size < capacity_)
                {
                    offset = (uint)(write_index_ & index_bit_mask_);
                    pool_[offset] = value;

                    ++write_index_;
                    mutex_.unlock();
                    return queue_size - 1;
                }
                mutex_.unlock();
            }
        }

        return -1;
    }

    inline int pop(TValue &value, int redo_count = 0)
    {
        uint    queue_size;
        uint    offset;

        if (0 == redo_count)
        {
            redo_count = reread_count_;
        }

        if (redo_count > 0)
        {//有限次数
            for (int i=0; i<redo_count; ++i)
            {
                mutex_.lock();
                queue_size  = (uint)(write_index_ - read_index_);
                if (queue_size > 0)
                {
                    offset  = (uint)(read_index_ & index_bit_mask_);
                    value   = pool_[offset];

                    ++read_index_;
                    mutex_.unlock();
                    return queue_size - 1;
                }
                mutex_.unlock();
            }
        }
        else
        {//无限次数
            while (1)
            {
                mutex_.lock();
                queue_size  = (uint)(write_index_ - read_index_);
                if (queue_size > 0)
                {
                    offset  = (uint)(read_index_ & index_bit_mask_);
                    value   = pool_[offset];

                    ++read_index_;
                    mutex_.unlock();
                    return queue_size - 1;
                }
                mutex_.unlock();
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
        uint size;
        mutex_.lock();
        size = (uint)(write_index_ - read_index_);
        mutex_.unlock();
        return size;
    }

    inline bool empty()
    {
        bool ret = true;
        mutex_.lock();
        if (write_index_ - read_index_ > 0)
        {
            ret = false;
        }
        mutex_.unlock();
        return ret;
    }

    inline int64 write_index()
    {
        int64 write_index;
        mutex_.lock();
        write_index = write_index_;
        mutex_.unlock();
        return write_index;
    }

    inline int64 read_index()
    {
        int64 read_index;
        mutex_.lock();
        read_index = read_index_;
        mutex_.unlock();
        return read_index;
    }

    inline void read_index(int64 index)
    {
        mutex_.lock();
        read_index_ = index;
        mutex_.unlock();
    }

    inline TValue& get(int64 index)
    {
        uint offset = index & index_bit_mask_;
        return pool_[offset];
    }

    inline const TValue& get(int64 index) const
    {
        uint offset = index & index_bit_mask_;
        return pool_[offset];
    }

private:
    TValue                  *pool_;             //对象数组
    int64                   write_index_;       //队列写位置(只增不减)
    int64                   read_index_;        //队列读位置(只增不减)
    uint                    capacity_;          //队列容量(必须为2的N次方)
    uint                    index_bit_mask_;    //下标位掩码(为capacity_ - 1)
    int                     rewrite_count_;     //重写次数(-1:无限次数)
    int                     reread_count_;      //重读次数(-1:无限次数)

    TSyncMutex              mutex_;
};

#endif

