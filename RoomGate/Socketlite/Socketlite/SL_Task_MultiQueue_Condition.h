#ifndef SOCKETLITE_TASK_MULTIQUEUE_CONDITION_H
#define SOCKETLITE_TASK_MULTIQUEUE_CONDITION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif
#include "SL_Config.h"
#include "SL_Random.h"
#include "SL_Sync_Condition.h"
#include "SL_Thread.h"
#include "SL_Queue.h"
#include <vector>

//可保证处理顺序的SL_Task
template <typename T>
class SL_Task_MultiQueue_Condition
{
public:
    SL_Task_MultiQueue_Condition()
        : next_thread_index_(0)
    {
    }

    virtual ~SL_Task_MultiQueue_Condition()
    {
        close();
    }

    int open(int thread_number=4, int queue_max_size=10000, int batch_node_count=8, int timedwait_interval_us=1000)
    {
        if (thread_number < 1)
        {
            thread_number = 1;
        }
        batch_node_count_ = batch_node_count;
        work_threads_.reserve(thread_number);

        WorkThread *work_thread;
        for (int i=0; i<thread_number; ++i)
        {
            work_thread = new WorkThread;
            work_thread->parent = this;
            work_thread->index  = i;
            work_thread->loadfactor = 0;
            work_thread->timedwait_interval_us = timedwait_interval_us;
            work_thread->timedwait_flag = false;
            work_thread->queue.init(queue_max_size);
            work_thread->thread.start(svc_run, work_thread);
            work_threads_.push_back(work_thread);
        }
        return 0;
    }

    int close()
    {
        WorkThread *work_thread;
        typename std::vector<WorkThread * >::iterator iter = work_threads_.begin();
        typename std::vector<WorkThread * >::iterator iter_end = work_threads_.end();
        for (; iter != iter_end; ++iter)
        {
            work_thread = *iter;
            work_thread->thread.stop();            
            work_thread->timedwait_condition.signal();
            work_thread->thread.join();
            work_thread->queue.clear();
            delete work_thread;
        }
        work_threads_.clear();
        return 0;
    }

    virtual int assign_thread()
    {
        uint thread_index = 0;
        uint thread_size = (uint)work_threads_.size();
        if (thread_size > 1)
        {
            thread_index = next_thread_index_++;
            if (thread_index >= thread_size)
            {
                thread_index -= thread_size;
                next_thread_index_ = thread_index + 1;
            }
        }
        return thread_index;
    }

    inline int getq(T &t, int thread_index)
    {
        if (thread_index < 0)
        {
            return -1;
        }
        return work_threads_[index]->queue.pop_front(t);
    }

    inline int putq(const T &t, int thread_index = -1)
    {//1)若成功返回工作线程index 2)若失败返回负数
        int thread_size = work_threads_.size();
        if (thread_size < 2)
        {
            thread_index = 0;
        }
        else
        {
            if ((thread_index < 0) || (thread_index >= thread_size))
            {
                thread_index = assign_thread();
            }
        }

        WorkThread work_thread = *work_threads_[thread_index];
        int ret = work_thread->queue.push_back(t);
        if (ret > 0)
        {
            if (work_thread->timedwait_flag)
            {
                work_thread->timedwait_flag = false;
                work_thread->timedwait_condition.signal();
            }
        }
        else if (ret < 1)
        {
            return -3;
        }
        return thread_index;
    }

    inline size_t queue_size()
    {
        size_t ret = 0;
        int thread_size = work_threads_.size();
        for (int i=0; i<thread_size; ++i)
        {
            ret += work_threads_[i]->queue.size();
        }
        return ret;
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
        typename SL_Task_MultiQueue_Condition<T>::WorkThread *work_thread = (typename SL_Task_MultiQueue_Condition<T>::WorkThread *)arg;
        SL_Task_MultiQueue_Condition<T> *task = (SL_Task_MultiQueue_Condition<T> *)work_thread->parent;
        if (task->init_svc_run() < 0)
        {
            work_thread->thread.exit();
            return 0;
        }

        uint i = 0;
        uint pop_node_count = 0;
        int  change_svc_data = 0;
        void *svc_data = task->get_svc_data();
        SL_Sync_ThreadMutex &mutex = work_thread->queue.mutex();
        T *node =  new T[task->batch_node_count_];

        while (1)
        {
            if (!work_thread->thread.get_running())
            {
                delete []node;
                task->put_svc_data(svc_data);
                task->fini_svc_run();
                work_thread->thread.exit();
                return 0;
            }

            mutex.lock();
            while (work_thread->queue.pop_front_i(node, task->batch_node_count_, pop_node_count) < 0)
            {
                work_thread->timedwait_flag = true;
                work_thread->timedwait_condition.wait(&mutex);
                work_thread->timedwait_flag = false;
                if (!work_thread->thread.get_running())
                {
                    mutex.unlock();
                    delete []node;
                    task->put_svc_data(svc_data);
                    task->fini_svc_run();
                    return 0;
                }
            }
            mutex.unlock();

            for (i=0; i<pop_node_count; ++i)
            {//处理消息
                task->svc(node[i], svc_data, change_svc_data);
                if (change_svc_data > 0)
                {
                    change_svc_data = 0;
                    task->put_svc_data(svc_data);
                    svc_data = task->get_svc_data();
                }
            }
        }

        delete []node;
        task->put_svc_data(svc_data);
        task->fini_svc_run();
        work_thread->thread.exit();
        return 0;
    }

private:
    struct WorkThread
    {
        void    *parent;
        uint    index;              //线程在线程集合中索引
        uint    loadfactor;         //负载因子

        uint    timedwait_interval_us;
        volatile bool timedwait_flag;
        SL_Sync_Condition timedwait_condition;

        SL_Thread thread;
        SL_Queue<T, SL_Sync_ThreadMutex> queue;
    };

    std::vector<WorkThread* > work_threads_;
    uint batch_node_count_;
    uint next_thread_index_;
};

#endif

