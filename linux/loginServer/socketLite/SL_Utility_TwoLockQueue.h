#ifndef SOCKETLITE_UTILITY_TWOLOCK_QUEUE_H
#define SOCKETLITE_UTILITY_TWOLOCK_QUEUE_H

//根据<<Simple, Fast, and Practical Non-Blocking Concurrent Queue Algorithms>>中Two-Lock Concurrent Queue Algorithm实现
//www.cs.rochester.edu/research/syncronization/pseudocode/queue.html

template <typename TValue, typename THeadMutex, typename TTailMutex>
class SL_Utility_TwoLockQueue
{
public:
    inline SL_Utility_TwoLockQueue()
    {
        NODE *node = new NODE();
        node->next = NULL;
        head_ = node;
        tail_ = node;
    }

    inline ~SL_Utility_TwoLockQueue()
    {
    }

    inline int push(const TValue &value)
    {
        NODE *node  = new NODE();
        node->value = value;
        node->next  = NULL;

        tail_mutex_.lock();
        tail_->next = node;
        tail_ = node;
        tail_mutex_.unlock();
        return 1;
    }

    inline int pop(TValue &value)
    {
        head_mutex_.lock();
        NODE *node = head_;
        NODE *new_head = node->next;
        if (NULL == new_head)
        {
            head_mutex_.unlock();
            return -1;
        }
        value = new_head->value;
        head_ = new_head;
        head_mutex_.unlock();

        delete node;
        return 1;
    }

private:
    typedef struct node_t
    {
        node_t *next;
        TValue  value;
    } NODE;

    NODE        *head_;
    NODE        *tail_;
    THeadMutex  head_mutex_;
    TTailMutex  tail_mutex_;
};

#endif

