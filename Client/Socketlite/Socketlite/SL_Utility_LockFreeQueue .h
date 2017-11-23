#ifndef SOCKETLITE_UTILITY_LOCKFREE_QUEUE_H
#define SOCKETLITE_UTILITY_LOCKFREE_QUEUE_H
#include "SL_Config.h"
#include "SL_Hash_Fun.h"
#include "SL_Sync_Atomic.h"

template <typename TValue>
class SL_Utility_LockFreeQueue
{
public:
    inline SL_Utility_LockFreeQueue()
        : pool_(NULL)
        , write_end_index_(0)
        , read_end_index_(0)
        , write_index_(0)
        , read_index_(0)
        , capacity_(0)
        , index_bit_mask_(0)
        , rewrite_count_(1)
        , reread_count_(1)
    {
    }

    inline ~SL_Utility_LockFreeQueue()
    {
        if (NULL != pool_)
        {
            delete []pool_;
        }
    }

    inline int init(uint capacity, int rewrite_count=1, int reread_count=1)
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
            write_end_index_.store(0);
            read_end_index_.store(0);
            write_index_.store(0);
            read_index_.store(0);
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
        write_end_index_.store(0);
        read_end_index_.store(0);
        write_index_.store(0);
        read_index_.store(0);
    }

    inline int push(const TValue &value, int redo_count=0)
    {
        int64   write_index;
        int64   read_end_index;
        uint    queue_size;

        if (0 == redo_count)
        {
            redo_count = rewrite_count_;
        }

        if (redo_count > 0)
        {//有限次数
            for (int i=0; i<redo_count; ++i) 
            {
                write_index     = write_index_.load();
                read_end_index  = read_end_index_.load();
                queue_size      = (uint)(write_index - read_end_index);
                if (queue_size < capacity_)
                {
                    if (write_index_.compare_exchange(write_index, write_index + 1))
                    {
                        uint offset   = write_index & index_bit_mask_;
                        pool_[offset] = value;
                        while (1)
                        {
                            if (write_end_index_.compare_exchange(write_index, write_index + 1))
                            {
                                break;
                            }
                        }
                        return queue_size + 1;
                    }
                }
            }
        }
        else
        {//无限次数
            while (1)
            {
                write_index     = write_index_.load();
                read_end_index  = read_end_index_.load();
                queue_size      = (uint)(write_index - read_end_index);
                if (queue_size < capacity_)
                {
                    if (write_index_.compare_exchange(write_index, write_index + 1))
                    {
                        uint offset   = write_index & index_bit_mask_;
                        pool_[offset] = value;
                        while (1)
                        {
                            if (write_end_index_.compare_exchange(write_index, write_index + 1))
                            {
                                break;
                            }
                        }
                        return queue_size + 1;
                    }
                }
            }
        }

        return -1;
    }

    inline int pop(TValue &value, int redo_count = 0)
    {
        int64   write_end_index;
        int64   read_index;
        uint    queue_size;

        if (0 == redo_count)
        {
            redo_count = reread_count_;
        }

        if (redo_count > 0)
        {//有限次数
            for (int i=0; i<redo_count; ++i)
            {
                write_end_index = write_end_index_.load();
                read_index      = read_index_.load();
                queue_size      = (uint)(write_end_index - read_index);
                if (queue_size > 0)
                {
                    if (read_index_.compare_exchange(read_index, read_index + 1))
                    {
                        uint offset = read_index & index_bit_mask_;
                        value       = pool_[offset];
                        while (1)
                        {
                            if (read_end_index_.compare_exchange(read_index, read_index + 1))
                            {
                                break;
                            }
                        }
                        return queue_size - 1;
                    }
                }
            }
        }                
        else
        {//无限次数
            while (1)
            {
                write_end_index = write_end_index_.load();
                read_index      = read_index_.load();
                queue_size      = (uint)(write_end_index - read_index);
                if (queue_size > 0)
                {
                    if (read_index_.compare_exchange(read_index, read_index + 1))
                    {
                        uint offset = read_index & index_bit_mask_;
                        value       = pool_[offset];
                        while (1)
                        {
                            if (read_end_index_.compare_exchange(read_index, read_index + 1))
                            {
                                break;
                            }
                        }
                        return queue_size - 1;
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
        int64 write_end_index   = write_end_index_.load();
        int64 read_end_index    = read_end_index_.load();
        return (uint)(write_end_index - read_end_index);
    }

    inline bool empty()
    {
        int64 write_end_index   = write_end_index_.load();
        int64 read_end_index    = read_end_index_.load();
        if (write_end_index - read_end_index > 0)
        {
            return false;
        }
        return true;
    }

    inline int64 write_end_index()
    {
        return write_end_index_.load();
    }

    inline int64 read_end_index()
    {
        return read_end_index_.load();
    }

    inline void read_end_index(int64 index)
    {
        read_end_index_.store(index);
    }

    inline int64 write_index()
    {
        return write_index_.load();
    }

    inline int64 read_index()
    {
        return read_index_.load();
    }

    inline void read_index(int64 index)
    {
        read_index_.store(index);
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
    TValue                  *pool_;                     //对象数组
    SL_Sync_Atomic_Int64    write_end_index_;           //队列写结束位(只增不减)
    SL_Sync_Atomic_Int64    read_end_index_;            //队列读结束位(只增不减)
    SL_Sync_Atomic_Int64    write_index_;               //队列写位置(只增不减)
    SL_Sync_Atomic_Int64    read_index_;                //队列读位置(只增不减)
    uint                    capacity_;                  //队列容量(必须为2的N次方)
    uint                    index_bit_mask_;            //下标位掩码(为capacity_ - 1)
    int                     rewrite_count_;             //重写次数(-1:无限次数)
    int                     reread_count_;              //重读次数(-1:无限次数)
};

#endif

