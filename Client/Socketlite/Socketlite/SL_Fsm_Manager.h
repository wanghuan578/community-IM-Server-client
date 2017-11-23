//finite-state machine
#ifndef SOCKETLITE_FSM_MANAGER_H
#define SOCKETLITE_FSM_MANAGER_H

#include <list>
#include "SL_Config.h"
template<typename TFsm, typename TState, typename TObjectPool, typename TKey> class SL_Fsm_Base;
template<typename TFsm, typename TState, typename TObjectPool, typename TKey> class SL_Fsm_Manager;

template<typename TFsm, typename TState, typename TObjectPool, typename TKey>
class SL_Fsm_State
{
public:
    SL_Fsm_State()
    {
    }
    virtual ~SL_Fsm_State()
    {
    }
    virtual int enter(SL_Fsm_Base<TFsm, TState, TObjectPool, TKey> *fsm)
    {
        return 0;
    }
};

template<typename TFsm, typename TState, typename TObjectPool, typename TKey=uint>
class SL_Fsm_Base
{
    friend class SL_Fsm_Manager<TFsm, TState, TObjectPool, TKey>;
public:
    SL_Fsm_Base()
        : context_(NULL)
        , state_(NULL)
        , id_(0)
        , collect_flag_(false)
    {
    }

    ~SL_Fsm_Base()
    {
    }

    inline TKey get_id() const
    {
        return id_;
    }

    inline SL_Fsm_Manager<TFsm, TState, TObjectPool, TKey>* get_context()
    {
        return context_;
    }

    inline void collect()
    {
        if (!collect_flag_)
        {
            collect_flag_ = true;
            context_->delete_list_.push_back((TFsm *)this);
        }
    }

    inline void set_state(TState *state)
    {
        state_ = state;
        state_->enter((TFsm *)this);
    }

protected:
    SL_Fsm_Manager<TFsm, TState, TObjectPool, TKey> *context_;
    TState  *state_;
    TKey id_;

private:
    bool collect_flag_;     //回收标记,防止状态机被重复回收
};

template<typename TFsm, typename TState, typename TObjectPool, typename TKey=uint>
class SL_Fsm_Manager
{
    friend class SL_Fsm_Base<TFsm, TState, TObjectPool, TKey>;
public:
    typedef SL_HASH_MAP<TKey, TFsm* >   FsmMap;
    typedef typename FsmMap::iterator   FsmMapIterator;
    typedef typename std::list<TFsm* >  FsmList;
    typedef typename FsmList::iterator  FsmListIterator;
    typedef TObjectPool FsmPool;

    SL_Fsm_Manager()
        : next_id_(1)
    {
    }

    ~SL_Fsm_Manager()
    {
    }

    inline int init(unsigned int pool_size, unsigned int init_chunk_size=100, unsigned int per_chunk_size=10)
    {
        if (per_chunk_size < 1)
        {
            per_chunk_size = 1;
        }
        next_id_ = 1;
        SL_HASH_REHASH(fsm_map_, 10000);
        return fsm_pool_.init((pool_size/per_chunk_size)+1, init_chunk_size, per_chunk_size);
    }

    inline uint get_capacity()
    {
        return 0;
    }

    inline uint get_used_count()
    {
        return fsm_map_.size();
    }

    inline TFsm* create_fsm(TKey id)
    {        
        TFsm *fsm = get_fsm(id);
        if (NULL == fsm)
        {
            fsm = fsm_pool_.alloc_object();
            if (NULL != fsm)
            {
                fsm->id_            = id;
                fsm->context_       = this;
                fsm->collect_flag_  = false;
                fsm_map_.insert(typename FsmMap::value_type(id, fsm));
            }
        }
        return fsm;
    }

    inline TFsm* create_fsm()
    {
        TFsm *fsm = fsm_pool_.alloc_object();
        if (NULL != fsm)
        {
            fsm->id_            = next_id_++;
            fsm->context_       = this;
            fsm->collect_flag_  = false;
            fsm_map_.insert(typename FsmMap::value_type(fsm->id_, fsm));
        }
        return fsm;
    }

    inline TFsm* get_fsm(TKey id)
    {
        FsmMapIterator it = fsm_map_.find(id);
        if (it != fsm_map_.end())
        {
            return (TFsm *)(it->second);
        }
        return NULL;
    }

    inline void collect()
    {
        TFsm *fsm;
        FsmListIterator iter = delete_list_.begin();
        FsmListIterator iter_end = delete_list_.end();
        for (; iter != iter_end; ++iter)
        {
            fsm = *iter;
            if (fsm_map_.erase(fsm->id_) > 0)
            {//防止状态机被重复回收
                fsm_pool_.free_object(fsm);
            }
        }
        delete_list_.clear();
    }

    inline void free_fsm(TKey id)
    {
        FsmMapIterator iter = fsm_map_.find(id);
        if (iter != fsm_map_.end())
        {
            TFsm *fsm = iter->second;
            fsm->collect_flag_ = true;
            fsm_map_.erase(iter);
            fsm_pool_.free_object(fsm);
        }
    }

protected:
    FsmPool fsm_pool_;
    FsmMap  fsm_map_;
    FsmList delete_list_;
    TKey    next_id_;
};

#endif

