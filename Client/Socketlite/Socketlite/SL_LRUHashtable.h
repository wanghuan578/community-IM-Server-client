#ifndef SOCKETLITE_LRUHASHTABLE_H
#define SOCKETLITE_LRUHASHTABLE_H

#include "SL_Config.h"
#include <vector>
#include <string>
#include <assert.h>

template <class TKey, class TData, class THashFcn = SL_DEFAULT_HASHFUNC<TKey> >
class SL_LRUHashtable
{
public:
    typedef TKey    key_type;
    typedef TData   data_type;

private:
    struct DataExt_;

    typedef typename SL_HASH_MAP<key_type, DataExt_, THashFcn> hashtable;
    typedef typename std::pair<const key_type, DataExt_> node_type;

    struct DataExt_
    {
        node_type   *prev;
        node_type   *next;
        data_type   real_data;
    };

public:
    SL_LRUHashtable()
        : max_size_(0)
        , head_(NULL)
        , tail_(NULL) 
    {
    }

    SL_LRUHashtable(size_t max_size)
        : max_size_(max_size)
        , head_(NULL)
        , tail_(NULL) 
    { 
        SL_HASH_REHASH(elements_ht_, max_size);
    }

    ~SL_LRUHashtable()
    {

    }

    inline size_t size() const
    { 
        return elements_ht_.size(); 
    }

    inline void init(size_t max_size)
    {
        max_size_ = max_size;
        SL_HASH_REHASH(elements_ht_, max_size);
    }

    bool add_element(const key_type &key, const data_type &data);
    bool remove_element(const key_type &key, data_type &data);
    bool remove_element(const key_type &key);
    bool pop_up(key_type &key, data_type &data);
    bool pop_up();
    void clear();

    bool get_element(const key_type &key, data_type &data);
    bool get_most_recent_element(data_type &data);
    bool get_least_recent_element(data_type &data);
    
    // this function is for debug purpose, ordered by most-recent ordering
    void get_ordered_keys(std::vector<key_type> &keys);

private:
    size_t      max_size_;
    hashtable   elements_ht_;
    node_type   *head_;
    node_type   *tail_;
};

template <class TKey, class TData, class THashFcn>
bool SL_LRUHashtable<TKey, TData, THashFcn>::add_element(const key_type &key, const data_type &data)
{
    typename hashtable::iterator it = elements_ht_.find(key);
    if (it == elements_ht_.end())
    {// add new node
        DataExt_ data_ext;
        data_ext.real_data = data;
        data_ext.prev = data_ext.next = NULL;

        if (elements_ht_.size() >= max_size_)
        {// size control
            pop_up();
        }

        std::pair<typename hashtable::iterator, bool> ins_result = elements_ht_.insert(node_type(key, data_ext));
        if (ins_result.second) 
        {// insert seccuss
            ins_result.first->second.next = head_;
            if (head_)
            {
                head_->second.prev = &(*(ins_result.first));
            }
            head_ = &(*(ins_result.first));;
            if (!tail_)
            {
                tail_ = head_;
            }
        }
        return true;
    }
    else
    {// update old node
        it->second.real_data = data;    // update the data
        if (head_ != &(*it))            // update the link
        {
            node_type *ptr_prev = it->second.prev;
            node_type *ptr_next = it->second.next;

            ptr_prev->second.next = ptr_next;    
            it->second.next = head_;
            it->second.prev = NULL;
            head_->second.prev = &(*it);
            head_ = &(*it);

            if (ptr_next)
            {
                ptr_next->second.prev = ptr_prev;
            }
            if (tail_ == &(*it))
            {
                tail_ = ptr_prev;            
            }
        }
    }

    return false;
}

template <class TKey, class TData, class THashFcn>
bool SL_LRUHashtable<TKey, TData, THashFcn>::remove_element(const key_type &key, data_type &data)
{
    typename hashtable::iterator it = elements_ht_.find(key);
    if (it == elements_ht_.end())
    {
        return false;
    }

    data = it->second.real_data;    
    node_type **pi_before;
    node_type **pi_after;
    if (it->second.prev == NULL)
    {
        pi_before = &head_;
    }
    else
    {
        pi_before = &(it->second.prev->second.next);
    }
    if (NULL == it->second.next)
    {
        pi_after = &tail_;
    }
    else
    {
        pi_after = &(it->second.next->second.prev);
    }
    
    *pi_before = it->second.next;
    *pi_after  = it->second.prev;
    elements_ht_.erase(it);

    return true;    
}

template <class TKey, class TData, class THashFcn>
bool SL_LRUHashtable<TKey, TData, THashFcn>::remove_element(const key_type &key)
{
    typename hashtable::iterator it = elements_ht_.find(key);
    if (it == elements_ht_.end())
    {
        return false;
    }

    node_type **pi_before;
    node_type **pi_after;
    if (it->second.prev == NULL)
    {
        pi_before = &head_;
    }
    else
    {
        pi_before = &(it->second.prev->second.next);
    }
    if (NULL == it->second.next)
    {
        pi_after = &tail_;
    }
    else
    {
        pi_after = &(it->second.next->second.prev);
    }
    
    *pi_before = it->second.next;
    *pi_after = it->second.prev;
    elements_ht_.erase(it);

    return true;
}

template <class TKey, class TData, class THashFcn>
bool SL_LRUHashtable<TKey, TData, THashFcn>::pop_up(key_type &key, data_type &data)
{
    if (NULL == tail_)
    {
        return false;
    }
    key = tail_->first;
    data = tail_->second.real_data;

    typename hashtable::iterator it = elements_ht_.find(key);
    assert(it != elements_ht_.end());
    elements_ht_.erase(it);

    node_type *ptr = tail_;
    tail_ = ptr->second.prev;
    if (NULL != tail_)
    {
        tail_->second.next = NULL;
    }
    else
    {
        head_ = NULL;
    }

    return true;
}

template <class TKey, class TData, class THashFcn>
bool SL_LRUHashtable<TKey, TData, THashFcn>::pop_up()
{
    if (NULL == tail_)
    {
        return false;
    }

    typename hashtable::iterator it = elements_ht_.find(tail_->first);
    assert(it != elements_ht_.end());
    elements_ht_.erase(it);

    node_type *ptr = tail_;
    tail_ = ptr->second.prev;
    if (NULL != tail_)
    {
        tail_->second.next = NULL;
    }
    else
    {
        head_ = NULL;
    }

    return true;
}

template <class TKey, class TData, class THashFcn>
void SL_LRUHashtable<TKey, TData, THashFcn>::clear()
{
    elements_ht_.clear();
    head_ = NULL;
    tail_ = NULL;
}

template <class TKey, class TData, class THashFcn>
bool SL_LRUHashtable<TKey, TData, THashFcn>::get_element(const key_type &key, data_type &data)
{
    typename hashtable::iterator it = elements_ht_.find(key);
    if (it == elements_ht_.end())
    {// not found
        return false;
    }

    data = it->second.real_data;
    if (head_ != &(*it))
    {// update the link
        node_type *ptr_prev = it->second.prev;
        node_type *ptr_next = it->second.next;

        ptr_prev->second.next = ptr_next;    
        it->second.next = head_;
        it->second.prev = NULL;
        head_->second.prev = &(*it);
        head_ = &(*it);

        if (ptr_next)
        {
            ptr_next->second.prev = ptr_prev;
        }
        if (tail_ == &(*it))
        {
            tail_ = ptr_prev;
        }
    }

    return true;
}

template <class TKey, class TData, class THashFcn>
bool SL_LRUHashtable<TKey, TData, THashFcn>::get_most_recent_element(data_type &data)
{
    if (NULL == head_)
    {
        return false;
    }

    data = head_->second.real_data;
    return true;
}

template <class TKey, class TData, class THashFcn>
bool SL_LRUHashtable<TKey, TData, THashFcn>::get_least_recent_element(data_type &data)
{
    if (NULL == tail_)
    {
        return false;
    }

    data = tail_->second.real_data;
    return true;
}

template <class TKey, class TData, class THashFcn>
void SL_LRUHashtable<TKey, TData, THashFcn>::get_ordered_keys(std::vector<key_type> &keys)
{
    keys.clear();
    for (node_type *p=head_; p; p=p->second.next)
    {
        keys.push_back(p->first);
    }
}

#endif

