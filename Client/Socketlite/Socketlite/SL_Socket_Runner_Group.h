#ifndef SOCKETLITE_SOCKET_RUNNER_GROUP_H
#define SOCKETLITE_SOCKET_RUNNER_GROUP_H

#include "SL_Config.h"
#include "SL_Socket_Handler.h"
#include "SL_Socket_Runner.h"
#include "SL_ObjectPool.h"
#include "SL_Sync_Mutex.h"
#include <vector>

template <typename TRunner>
class SL_Socket_Runner_Group : public SL_Socket_Runner
{
public:
    SL_Socket_Runner_Group()
        : next_runner_index_(0)
    {
    }

    virtual ~SL_Socket_Runner_Group()
    {
        close();
        clear();
    }

    virtual TRunner* new_runner()
    {
        TRunner *runner;
        runner = new TRunner();
        return runner;
    }

    virtual void delete_runner(TRunner *runner)
    {
        delete runner;
    }

    virtual SL_Socket_Runner* assign_runner()
    {
        uint runners_size = (uint)socket_runners_.size();
        if (runners_size > 1)
        {
    	    uint runner_index = next_runner_index_++;
            if (runner_index >= runners_size)
            {
                runner_index -= runners_size;
                next_runner_index_ = runner_index + 1;
            }
            return socket_runners_[runner_index];
        }
        else if (1 == runners_size)
        {
            return socket_runners_[0];
        }
        return NULL;
    }

    int init(uint runner_num=10)
    {
        if (runner_num < 2)
        {
            runner_num = 2;
        }

        socket_runners_.reserve(runner_num);
        TRunner *runner;
        for (int i=0; i<runner_num; ++i)
        {
            runner = new_runner();
            socket_runners_.push_back(runner);
        }
        return 0;
    }

    int clear()
    {
        typename std::vector<TRunner* >::iterator iter = socket_runners_.begin();
        typename std::vector<TRunner* >::iterator iter_end = socket_runners_.end();
        for (; iter != iter_end; ++iter)
        {
            delete_runner(*iter);
        }
        socket_runners_.clear();
        return 0; 
    }

    inline int set_buffer_size(int recv_buffer_size=32768, int send_buffer_size=32768) 
    { 
        TRunner *runner;
        typename std::vector<TRunner* >::iterator iter = socket_runners_.begin();
        typename std::vector<TRunner* >::iterator iter_end = socket_runners_.end();
        for (; iter != iter_end; ++iter)
        {
            runner = *iter;
            runner->set_buffer_size(recv_buffer_size, send_buffer_size);
        }
        return 0; 
    }

    int open(int event_mask, uint max_size, uint max_timeout_ms, uint thread_number, int handler_close_status=SL_Socket_Handler::STATUS_CLOSE) 
    { 
        TRunner *runner;
        typename std::vector<TRunner* >::iterator iter = socket_runners_.begin();
        typename std::vector<TRunner* >::iterator iter_end = socket_runners_.end();
        for (; iter != iter_end; ++iter)
        {
            runner = *iter;
            runner->open(event_mask, max_size, max_timeout_ms, thread_number, handler_close_status);
        }
        return 0;
    }

    int close() 
    { 
        TRunner *runner;
        typename std::vector<TRunner* >::iterator iter = socket_runners_.begin();
        typename std::vector<TRunner* >::iterator iter_end = socket_runners_.end();
        for (; iter != iter_end; ++iter)
        {
            runner = *iter;
            runner->close();
        }
        return 0; 
    }

    inline int add_handle(SL_Socket_Handler *socket_handler, int event_mask)
    {
        SL_Socket_Runner *runner = assign_runner();
        if (NULL != runner)
        {
            if (runner->add_handle(socket_handler, event_mask) >= 0)
            {
                socket_handler->socket_runner_ = runner;
                return 0;
            }
        }
        return -1;
    }

    inline int del_handle(SL_Socket_Handler *socket_handler)
    {
        SL_Socket_Runner *handler_runner = socket_handler->get_socket_runner();
        if (this != handler_runner)
        {
            return handler_runner->del_handle(socket_handler);
        }
        return 0;
    }

    inline int remove_handle(SL_Socket_Handler *socket_handler)
    {
        SL_Socket_Runner *handler_runner = socket_handler->get_socket_runner();
        if (this != handler_runner)
        {
            return handler_runner->remove_handle(socket_handler);
        }
        return 0;
    }

    inline int set_event_mask(SL_Socket_Handler *socket_handler, int event_mask)
    {
        SL_Socket_Runner *handler_runner = socket_handler->get_socket_runner();
        if (this != handler_runner)
        {
            return handler_runner->remove_handle(socket_handler);
        }
        return 0;
    }

    inline size_t get_handler_size()
    {
        size_t handler_size = 0;
        TRunner *runner;
        typename std::vector<TRunner* >::iterator iter = socket_runners_.begin();
        typename std::vector<TRunner* >::iterator iter_end = socket_runners_.end();
        for (; iter != iter_end; ++iter)
        {
            runner = *iter;
            handler_size += runner->get_handler_size();
        }
        return handler_size;
    }

    inline int event_dispatch()
    {
        return event_loop(-1);
    }

    inline int event_loop(int timeout_ms)
    {
        TRunner *runner;
        typename std::vector<TRunner* >::iterator iter = socket_runners_.begin();
        typename std::vector<TRunner* >::iterator iter_end = socket_runners_.end();
        for (; iter != iter_end; ++iter)
        {
            runner = *iter;
            runner->event_loop(timeout_ms);
        }
        return 0; 
    }

    inline int thread_event_loop(int timeout_ms)
    {
        TRunner *runner;
        typename std::vector<TRunner* >::iterator iter = socket_runners_.begin();
        typename std::vector<TRunner* >::iterator iter_end = socket_runners_.end();
        for (; iter != iter_end; ++iter)
        {
            runner = *iter;
            runner->thread_event_loop(timeout_ms);
        }
        return 0;
    }

    inline int thread_wait()
    { 
        TRunner *runner;
        typename std::vector<TRunner* >::iterator iter = socket_runners_.begin();
        typename std::vector<TRunner* >::iterator iter_end = socket_runners_.end();
        for (; iter != iter_end; ++iter)
        {
            runner = *iter;
            runner->thread_wait();
        }
        return 0; 
    }

    inline int thread_stop()
    { 
        TRunner *runner;
        typename std::vector<TRunner* >::iterator iter = socket_runners_.begin();
        typename std::vector<TRunner* >::iterator iter_end = socket_runners_.end();
        for (; iter != iter_end; ++iter)
        {
            runner = *iter;
            runner->thread_stop();
        }
        return 0; 
    }

protected:
    uint next_runner_index_;
    std::vector<TRunner* > socket_runners_;
};

#endif

