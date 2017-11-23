#ifndef SOCKETLITE_CIRCULAR_QUEUE_H
#define SOCKETLITE_CIRCULAR_QUEUE_H

//循环队列(环形队列): 当只有一读线程和一写线程时，可不用锁机制来保持内部数据一致性

#include "SL_DataType.h"
#include "SL_Sync_Guard.h"
#include <vector>

template <typename T, typename TSyncMutex>
class SL_Circular_Queue
{
public:
    SL_Circular_Queue()
    {
    }

    ~SL_Circular_Queue()
    {
    }    

protected:
    TSyncMutex      mutex_;
    std::vector<T>  queue_;
    size_t          read_pos_;
    size_t          write_pos_;
};

#endif
