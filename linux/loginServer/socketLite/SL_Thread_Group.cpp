#include "SL_Thread_Group.h"

SL_Thread_Group::SL_Thread_Group()
    : running_(0)
{
}

SL_Thread_Group::~SL_Thread_Group()
{
    clear();
}

int SL_Thread_Group::clear()
{
    std::vector<SL_Thread* >::iterator iter = threads_.begin();
    std::vector<SL_Thread* >::iterator iter_end = threads_.end();
    for (; iter != iter_end; ++iter)
    {
        (*iter)->clear();
        delete *iter;
    }
    threads_.clear();
    return 0;
}

int SL_Thread_Group::start(SL_ThreadProc threadproc, void *arg, int max_thread_num, int init_thread_num)
{
    if (NULL == threadproc)
    {
        return -1;
    }

    clear();
    if (max_thread_num <= 0)
    {
        max_thread_num = 1;
    }
    if (init_thread_num <= 0)
    {
        init_thread_num = 0;
    }
    if (init_thread_num > max_thread_num)
    {
        init_thread_num = max_thread_num;
    }
    threads_.reserve(max_thread_num);
    threadproc_ = threadproc;
    arg_ = arg;

    running_.store(1);
    SL_Thread *thread;
    for (int i=0; i<init_thread_num; ++i)
    {
        thread = new SL_Thread();
        thread->start(threadproc, arg);
        threads_.push_back(thread);
    }
    return 0;
}

int SL_Thread_Group::stop()
{
    running_.store(0);
    std::vector<SL_Thread* >::iterator iter = threads_.begin();
    std::vector<SL_Thread* >::iterator iter_end = threads_.end();
    for (; iter != iter_end; ++iter)
    {
        (*iter)->stop();
    }
    return 0;
}

int SL_Thread_Group::join()
{
    std::vector<SL_Thread* >::iterator iter = threads_.begin();
    std::vector<SL_Thread* >::iterator iter_end = threads_.end();
    for (; iter != iter_end; ++iter)
    {
        (*iter)->join();
    }
    return 0;
}

int SL_Thread_Group::exit()
{
    std::vector<SL_Thread* >::iterator iter = threads_.begin();
    std::vector<SL_Thread* >::iterator iter_end = threads_.end();
    for (; iter != iter_end; ++iter)
    {
        (*iter)->exit();
    }
    return 0;
}

