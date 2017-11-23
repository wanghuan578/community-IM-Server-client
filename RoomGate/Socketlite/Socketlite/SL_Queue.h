#ifndef SOCKETLITE_QUEUE_H
#define SOCKETLITE_QUEUE_H

#include <list>
#include <deque>
#include "SL_DataType.h"
#include "SL_Sync_Guard.h"

template <typename T, typename TSyncMutex>
class SL_Queue
{
public:
    inline SL_Queue()
        : capacity_size_(10000)
        , queue_size_(0)
    {
    }

    inline ~SL_Queue()
    {
        clear();
    }

    inline int init(int capacity = 10000)
    {
        if (capacity < 1)
        {
            capacity_size_ = 1;
        }
        else
        {
            capacity_size_ = capacity;
        }
        clear();
        return 0;
    }

    inline void clear()
    {
        mutex_.lock();
        queue_.clear();
        queue_size_ = 0;
        mutex_.unlock();
    }

    inline int push_front_i(const T &node)
    {
        queue_.push_front(node);
        return ++queue_size_;
    }

    inline int push_front_i(const T node[], int node_count, int &push_node_count)
    {
        for (int i=0; i<node_count; ++i)
        {
            queue_.push_front(node[i]);
        }
        push_node_count = node_count;
        queue_size_ += node_count;
        return queue_size_;
    }

    inline int push_back_i(const T &node)
    {
        queue_.push_back(node);
        return ++queue_size_;
    }

    inline int push_back_i(const T node[], int node_count, int &push_node_count)
    {
        for (int i=0; i<node_count; ++i)
        {
            queue_.push_back(node[i]);
        }
        push_node_count = node_count;
        queue_size_ += node_count;
        return queue_size_;
    }

    inline int pop_front_i(T &node)
    {
        if (queue_size_ > 0)
        {
            node = queue_.front();
            queue_.pop_front();
            return --queue_size_;
        }
        return -1;
    }

    inline int pop_front_i(T node[], int node_count, int &pop_node_count)
    {
        pop_node_count = ((node_count < queue_size_) ? node_count : queue_size_);
        if (pop_node_count <= 0)
        {
            return -1;
        }
        for (int i=0; i<pop_node_count; ++i)
        {
            node[i] = queue_.front();
            queue_.pop_front();
        }
        queue_size_ -= pop_node_count;
        return queue_size_;
    }

    inline int pop_back_i(T &node)
    {
        if (queue_size_ > 0)
        {
            node = queue_.back();
            queue_.pop_back();
            return --queue_size_;
        }
        return -1;
    }

    inline int pop_back_i(T node[], int node_count, int &pop_node_count)
    {
        pop_node_count = ((node_count < queue_size_) ? node_count : queue_size_);
        if (pop_node_count <= 0)
        {
            return -1;
        }
        for (int i=0; i<pop_node_count; ++i)
        {
            node[i] = queue_.back();
            queue_.pop_back();
        }
        queue_size_ -= pop_node_count;
        return queue_size_;
    }

    inline int push_front(const T &node)
    {
        mutex_.lock();
        int ret = push_front_i(node);
        mutex_.unlock();
        return ret;
    }

    inline int push_front(const T node[], int node_count, int &push_node_count)
    {
        mutex_.lock();
        int ret = push_front_i(node, node_count, push_node_count);
        mutex_.unlock();
        return ret;
    }

    inline int push_back(const T &node)
    {
        mutex_.lock();
        int ret = push_back_i(node);
        mutex_.unlock();
        return ret;
    }

    inline int push_back(const T node[], int node_count, int &push_node_count)
    {
        mutex_.lock();
        int ret = push_back_i(node, node_count, push_node_count);
        mutex_.unlock();
        return ret;
    }

    inline int pop_front(T &node)
    {
        mutex_.lock();
        int ret = pop_front_i(node);
        mutex_.unlock();
        return ret;
    }

    inline int pop_front(T node[], int node_count, int &pop_node_count)
    {
        mutex_.lock();
        int ret = pop_front_i(node, node_count, pop_node_count);
        mutex_.unlock();
        return ret;
    }

    inline int pop_back(T &node)
    {
        mutex_.lock();
        int ret = pop_back_i(node);
        mutex_.unlock();
        return ret;
    }

    inline int pop_back(T node[], int node_count, int &pop_node_count)
    {
        mutex_.lock();
        int ret = pop_back_i(node, node_count, pop_node_count);
        mutex_.unlock();
        return ret;
    }

    inline int push(const T &node)
    {
        mutex_.lock();
        int ret = push_back_i(node);
        mutex_.unlock();
        return ret;
    }

    inline int pop(const T &node)
    {
        mutex_.lock();
        int ret = pop_back_i(node);
        mutex_.unlock();
        return ret;
    }

    inline int size() const
    {
        return queue_size_;
    }

    inline bool empty() const
    {
        return 0 == queue_size_;
    }

    inline T& front()
    {
        return queue_.front();
    }

    inline T& back()
    {
        return queue_.back();
    }

    inline TSyncMutex& mutex()
    {
        return mutex_;
    }

private:
    int             capacity_size_;
    int             queue_size_;    //加入的原因是因为std::list.size()只保证为时间效率为O(N)不是O(1)
    TSyncMutex      mutex_;
    std::list<T>    queue_;
    //std::deque<T>   queue_;
};

#endif

