#ifndef SOCKETLITE_SEDA_STAGE_H
#define SOCKETLITE_SEDA_STAGE_H

#include <vector>
#include "SL_DataType.h"
#include "SL_Socket_CommonAPI.h"
#include "SL_Seda_StageThread.h"

template <typename TSedaStageHandler>
class SL_Seda_Stage : public SL_Seda_IStage
{
public:
    SL_Seda_Stage()
        : timedwait_interval_us_(1000)
        , next_thread_index_(0)
    {
    }

    virtual ~SL_Seda_Stage()
    {
        close();
    }

    int open(int thread_number, uint queue_max_size, uint event_max_len=64, uint timedwait_interval_us=1000, bool timedwait_signal=true, int type=0)
    {
        type_ = type;
        timedwait_interval_us_ = timedwait_interval_us;
        if (thread_number < 1)
        {
            thread_number = 1;
        }
        stage_threads_.reserve(thread_number);

        StageThread *stage_thread;
        for (int i=0; i<thread_number; ++i)
        {
            stage_thread = new StageThread(this, i, queue_max_size, event_max_len, timedwait_interval_us, timedwait_signal);
            stage_thread->start();
            stage_threads_.push_back(stage_thread);
        }
        return 0;
    }

    int close()
    {
        StageThread *stage_thread;
        typename std::vector<StageThread *>::iterator iter = stage_threads_.begin();
        typename std::vector<StageThread *>::iterator iter_end = stage_threads_.end();
        for (; iter != iter_end; ++iter)
        {
            stage_thread = *iter;
            stage_thread->stop();
            stage_thread->join();
            delete stage_thread;
        }
        stage_threads_.clear();
        return 0;
    }

    int join()
    {
        typename std::vector<StageThread *>::iterator iter = stage_threads_.begin();
        typename std::vector<StageThread *>::iterator iter_end = stage_threads_.end();
        for (; iter != iter_end; ++iter)
        {
            (*iter)->join();
        }
        return 0;
    }

    inline int assign_thread()
    {
        uint thread_size  = (uint)stage_threads_.size();
        uint thread_index = next_thread_index_++;
        if (thread_index >= thread_size)
        {
            thread_index -= thread_size;
            next_thread_index_ = thread_index + 1;
        }
        return thread_index;
    }

    inline int push_event(const SL_Seda_Event *event, int thread_index=-1, bool timedwait_signal=true)
    {
        int thread_size = stage_threads_.size();
        if (thread_size < 2)
        {
            thread_index = 0;
        }
        else
        {
            if ((thread_index < 0) || (thread_index >= thread_size))
            {
                thread_index = next_thread_index_++;
                if (thread_index >= thread_size)
                {
                    thread_index -= thread_size;
                    next_thread_index_ = thread_index + 1;
                }
            }
        }

        int ret = stage_threads_[thread_index]->push_event(event, timedwait_signal);
        if (ret < 0)
        {
            return -1;
        }
        return thread_index;
    }

    inline int get_type() const 
    {
        return type_;
    }

protected:
    typedef SL_Seda_StageThread<TSedaStageHandler> StageThread;

    std::vector<StageThread* > stage_threads_;
    uint    timedwait_interval_us_;
    uint    next_thread_index_;
    int     type_;
};

#endif

