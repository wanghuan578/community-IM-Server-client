#ifndef SOCKETLITE_TASK_MULTIQUEUE_H
#define SOCKETLITE_TASK_MULTIQUEUE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif
#include "SL_Config.h"
#include "SL_Random.h"
#include "SL_Sync_Semaphore.h"
#include "SL_Thread.h"
#include "SL_Queue.h"
#include <vector>

//可保证处理顺序的SL_Task
template <typename T>
class SL_Task_MultiQueue
{
public:
    SL_Task_MultiQueue() 
    {
    }

    virtual ~SL_Task_MultiQueue()
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
            work_thread->queue.init(queue_max_size);
            work_thread->semaphore.open(queue_max_size);
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
            work_thread->semaphore.release();
            work_thread->thread.join();
            work_thread->queue.clear();
            delete work_thread;
        }
        work_threads_.clear();
        return 0;
    }

    virtual int assign_thread()
    {
        uint thread_size = (uint)work_threads_.size();
        if (thread_size < 2)
        {
            return 0;
        }

        //分配给负荷最小的线程,从而整体上使每个线程负荷平均
        uint res_index = 0;
        uint min_queue_size = (uint)work_threads_[0]->queue.size();
        uint queue_size;
        for (uint i=1; i<thread_size; ++i)
        {
            queue_size = work_threads_[i]->queue.size();
            if (queue_size < min_queue_size)
            {
                min_queue_size = queue_size;
                res_index = i;
            }
        }
        return res_index;
    }

    inline int getq(T &t, int thread_index)
    {
        if (thread_index < 0)
        {
            return -1;
        }
        return work_threads_[thread_index]->queue.pop_front(t);
    }

    inline int putq(const T &t, int thread_index = -1)
    {//1)若成功返回工作线程Index 2)若失败返回负数
        if (thread_index < 0)
        {
            thread_index = assign_thread();
            if (thread_index < 0)
            {
                return -1;
            }
        }
        else
        {
            if (thread_index > work_threads_.size())
            {
                return -2;
            }
        }

        if (work_threads_[thread_index]->queue.push_back(t) > 0)
        {
            work_threads_[thread_index]->semaphore.release();
            return thread_index;
        }
        return -3;
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
        typename SL_Task_MultiQueue<T>::WorkThread *work_thread = (typename SL_Task_MultiQueue<T>::WorkThread *)arg;
        SL_Task_MultiQueue<T> *task = (SL_Task_MultiQueue<T> *)work_thread->parent;
        if (task->init_svc_run() < 0)
        {
            work_thread->thread.exit();
            return 0;
        }

        uint i = 0;
        uint pop_node_count = 0;
        int  change_svc_data = 0;
        void *svc_data = task->get_svc_data();
        T *node =  new T[task->batch_node_count_];

        while (1)
        {
            work_thread->semaphore.acquire();
            if (!work_thread->thread.get_running())
            {
                delete []node;
                task->put_svc_data(svc_data);
                task->fini_svc_run();
                work_thread->thread.exit();
                return 0;
            }
            if (work_thread->queue.pop_front(node, task->batch_node_count_, pop_node_count) >= 0)
            {
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
        work_thread->thread.exit();
        return 0;
    }

private:
    struct WorkThread
    {
        void    *parent;            
        uint    index;              //线程在线程集合中索引
        uint    loadfactor;         //负载因子

        SL_Sync_Semaphore semaphore;
        SL_Thread thread;
        SL_Queue<T, SL_Sync_SpinMutex> queue;
    };

    std::vector<WorkThread* > work_threads_;
    uint batch_node_count_;
};

#endif

