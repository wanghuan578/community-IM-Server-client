#ifndef SOCKETLITE_SOCKET_IOCP_RUNNER_H
#define SOCKETLITE_SOCKET_IOCP_RUNNER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif
#include "SL_Config.h"
#include "SL_Socket_CommonAPI.h"
#include "SL_Sync_Mutex.h"
#include "SL_Thread_Group.h"
#include "SL_Socket_Runner.h"
#include "SL_Socket_Iocp_Handler.h"

#ifdef SOCKETLITE_OS_WINDOWS

template <class TSyncMutex>
class SL_Socket_Iocp_Runner : public SL_Socket_Runner
{
public:
    SL_Socket_Iocp_Runner(ushort concurrency_thread_num=0)
        : completion_port_(NULL)
        , concurrency_thread_num_(concurrency_thread_num)
        , current_handle_size_(0)
    {
    }

    virtual ~SL_Socket_Iocp_Runner()
    {
        close();
    }

    inline int set_buffer_size(int recv_buffer_size=65536, int send_buffer_size=65536)
    {
        return 0;
    }

    int open(int event_mask=SL_Socket_Handler::READ_EVENT_MASK, uint max_size=100000, uint max_timeout_ms=100, uint thread_number=10, int8 handler_close_status=SL_Socket_Handler::STATUS_CLOSE)
    {
        close();
        if (max_size <= 0)
        {
            return -1;
        }
        max_size_ = max_size;
        handler_close_status_ = handler_close_status;

        //创建完成端口句柄
        completion_port_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, concurrency_thread_num_);
        if (INVALID_HANDLE_VALUE == completion_port_)
        {
            return -2;
        }

        //启动工作线程组
        work_thread_num_ = thread_number;
        return  thread_group_.start(event_loop_proc, this, thread_number, thread_number);
    }

    int close()
    {
        if (NULL == completion_port_)
        {
            return 0;
        }

        //使工作线程退出
        for (int i=0; i<work_thread_num_; ++i)
        {
            //发出退出消息
            PostQueuedCompletionStatus(completion_port_, -1, 0, NULL);
        }
        thread_group_.stop();
        thread_group_.join();
        CloseHandle(completion_port_);
        completion_port_ =  NULL;

        return 0;
    }

    inline int add_handle(SL_Socket_Handler *socket_handler, int event_mask) 
    {
        if (socket_handler->socket_ == SL_INVALID_SOCKET)
        {
            return -1;
        }
        if (socket_handler->runner_pos_ >= 0)
        {//socket_handler已加入runner
            return -2;
        }

        mutex_.lock();
        if (current_handle_size_ >= max_size_)
        {
            mutex_.unlock();
            return -3;
        }
        //将socket_handler关联到完成端口
        if (NULL == CreateIoCompletionPort((HANDLE)socket_handler->socket_, completion_port_, (ULONG_PTR)socket_handler, 0))
        {
            mutex_.unlock();
            return -4;
        }
        //投递接收操作
        if (((SL_Socket_Iocp_Handler*)socket_handler)->post_recv() < 0)
        {
            mutex_.unlock();
            return -5;
        }
        socket_handler->runner_pos_ = 0;
        ++current_handle_size_;
        mutex_.unlock();

        return 0;
    }

    inline int del_handle(SL_Socket_Handler *socket_handler) 
    { 
        if ((SL_INVALID_SOCKET == socket_handler->socket_) || (socket_handler->runner_pos_ < 0))
        {
            return -1;
        }

        mutex_.lock();
        socket_handler->runner_pos_ = -1;
        --current_handle_size_;
        mutex_.unlock();

        //清理工作
        int ret = socket_handler->handle_close();
        socket_handler->socket_source_->disconnect(socket_handler);
        if (ret >= 0)
        {
            socket_handler->socket_source_->free_handler(socket_handler);
        }

        return 0;
    }

    inline int remove_handle(SL_Socket_Handler *socket_handler) 
    { 
        if ((SL_INVALID_SOCKET == socket_handler->socket_) || (socket_handler->runner_pos_ < 0))
        {
            return -1;
        }

        mutex_.lock();
        socket_handler->runner_pos_ = -1;
        --current_handle_size_;
        mutex_.unlock();

        return 0; 
    }

    inline size_t get_handler_size() const
    { 
        return current_handle_size_;
    }

    inline int event_loop(int timeout_ms=1)
    {
		SL_Socket_CommonAPI::util_sleep_ms(timeout_ms);
        return 0;
    }

    inline int event_dispatch()
    {
        return event_loop(1);
    }

    inline int thread_event_loop(int timeout=-1)
    { 
        return 0;
    }

    inline int thread_wait()
    {
        thread_group_.join();
        return 0;
    }

    inline int thread_stop()
    { 
        thread_group_.stop();

        //使工作线程退出
        for (int i=0; i<work_thread_num_; ++i)
        {
            //发出退出消息
            PostQueuedCompletionStatus(completion_port_, -1, 0, NULL);
        }
        return 0;
    }

    static unsigned int WINAPI event_loop_proc(void *arg)
    {
        SL_Socket_Iocp_Runner<TSyncMutex> *runner = (SL_Socket_Iocp_Runner<TSyncMutex>*)arg;
        HANDLE completion_port = runner->completion_port_;

        DWORD   byteTransferred = 0;
        BOOL    success = false;
        SL_Socket_Iocp_Handler *per_handle_data = NULL;
        SL_Socket_Iocp_Handler::PER_IO_DATA *per_io_data = NULL;

        while (1)
        {
            if (!runner->thread_group_.get_running())
            {
                break;
            }

            per_handle_data = NULL;
            per_io_data     = NULL;
            byteTransferred = -1;

            success = GetQueuedCompletionStatus(completion_port, 
                &byteTransferred, 
                (LPDWORD)&per_handle_data, 
                (LPOVERLAPPED*)&per_io_data, 
                INFINITE);

            //退出信号到达，退出线程
            if (-1 == byteTransferred)
            {
                return 1;
            }
            //客户机已经断开连接或者连接出现错误
            if (0 == byteTransferred)
            {
                if (NULL != per_handle_data)
                {
                    per_handle_data->last_errno_  = SL_Socket_Handler::ERROR_CLOSE_PASSIVE;
                    per_handle_data->next_status_ = runner->handler_close_status_;
                    runner->del_handle(per_handle_data);
                }
                continue;
            }
            if ( (!success) || (NULL == per_handle_data) || (NULL == per_io_data) )
            { //出现异常情况
                continue;
            }

            if (per_io_data->opertype == SL_Socket_Iocp_Handler::RECV_POSTED)
            {//接收数据

                //消息处理
                per_io_data->data_buffer.data_end(byteTransferred);
                if (per_handle_data->handle_read() >= 0)
                {
                    per_handle_data->post_recv();
                }
                else
                {//小于0表示此Socket出异常, 所以删除
                    per_handle_data->next_status_ = SL_Socket_Handler::STATUS_CLOSE;
                    runner->del_handle(per_handle_data);
                }
            }
        }
        return 0;
    }

private:
    HANDLE          completion_port_;           //完成端口句柄
    ushort          concurrency_thread_num_;    //并行处量线程数
    ushort          work_thread_num_;           //工作线程数

    uint            max_size_;
    uint            current_handle_size_;
    int8            handler_close_status_;

    TSyncMutex      mutex_;
    SL_Thread_Group thread_group_;
};

#endif

#endif

