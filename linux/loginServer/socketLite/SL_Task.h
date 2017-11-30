#ifndef SOCKETLITE_TASK_H
#define SOCKETLITE_TASK_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif
#include "SL_Config.h"
#include "SL_Sync_Semaphore.h"
#include "SL_Thread_Group.h"
#include "SL_Queue.h"

template <typename T>
class SL_Task
{
public:
    SL_Task()
    {
    }
    virtual ~SL_Task()
    {
        close();
    }

    int open(int thread_number=4, int queue_max_size=10000, int batch_node_count=8, int timedwait_interval_us=1000)
    {
        batch_node_count_ = batch_node_count;
        queue_.init(queue_max_size);
        semaphore_.open(queue_max_size);
        return thread_group_.start(svc_run, this, thread_number, thread_number);
    }

    int close()
    {
        thread_group_.stop();
        size_t thread_count = thread_group_.get_thread_count();
        for (size_t i=0; i<thread_count; ++i)
        {
            semaphore_.release();
        }
        thread_group_.join();
        thread_group_.clear();
        queue_.clear();
        semaphore_.close();
        return 0;
    }

    inline int getq(T &t, int thread_index)
    {
        return queue_.pop_front(t);
    }

    inline int putq(T &t, int thread_index=-1)
    {
        if (queue_.push_back(t) > 0)
        {
            semaphore_.release();
            return 1;
        }
        return -1;
    }

    inline size_t queue_size()
    {
        return queue_.size();
    }

    virtual int init_svc_run()
    {
        return 0;
    }

    virtual int fini_svc_run()
    {
        return 0;
    }

    //取得每个线程局部数据
    virtual void* get_svc_data()
    {
        return NULL;
    }

    //回收每个线程局部数据
    virtual void put_svc_data(void *svc_data)
    {
        return;
    }

    virtual int svc(T &t, void *svc_data, int &change_svc_data)
    { 
        return 0;
    }

private:
#ifdef SOCKETLITE_OS_WINDOWS
    static unsigned int WINAPI svc_run(void *arg)
#else
    static void* svc_run(void *arg)
#endif
    {// Routine that runs the task routine as a daemon thread.
        SL_Task<T> *task =(SL_Task<T> *)arg;
        if (task->init_svc_run() < 0)
        {
            return 0;
        }

        uint i = 0;
        uint pop_node_count = 0;
        int  change_svc_data = 0;
        void *svc_data = task->get_svc_data();
        T *node = new T[task->batch_node_count_];

        while (1)
        {
            task->semaphore_.acquire();
            if (!task->thread_group_.get_running())
            {
                delete []node;
                task->put_svc_data(svc_data);
                task->fini_svc_run();
                return 0;
            }
            if (task->queue_.pop_front(node, task->batch_node_count_, pop_node_count) >= 0)
            {//处理消息
                for (i=0; i<pop_node_count; ++i)
                {
                    task->svc(node[i], svc_data, change_svc_data);
                    if (change_svc_data > 0)
                    {
                        change_svc_data = 0;
                        task->put_svc_data(svc_data);
                        svc_data = task->get_svc_data();
                    }
                }
            }
        }
        delete []node;
        task->put_svc_data(svc_data);
        task->fini_svc_run();
        return 0;
    }

private:
    SL_Sync_Semaphore   semaphore_;
    SL_Thread_Group     thread_group_;
    SL_Queue<T, SL_Sync_SpinMutex> queue_;
    uint                batch_node_count_;
};

#endif

