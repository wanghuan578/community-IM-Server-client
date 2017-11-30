#ifndef SOCKETLITE_SOCKET_EPOLL_RUNNER_H
#define SOCKETLITE_SOCKET_EPOLL_RUNNER_H

#include "SL_Config.h"
#include "SL_ByteBuffer.h"
#include "SL_Socket_CommonAPI.h"
#include "SL_Socket_Runner.h"
#include "SL_Socket_TcpServer.h"
#include "SL_Socket_TcpServer_Handler.h"
#include "SL_Sync_Mutex.h"
#include "SL_Thread.h"

#ifdef SOCKETLITE_OS_LINUX
#include <sys/epoll.h>

template <class TSyncMutex>
class SOCKETLITE_API SL_Socket_Epoll_Runner : public SL_Socket_Runner
{
public:
    inline SL_Socket_Epoll_Runner(uint max_events=100000, int epoll_mode=ET_MODE)
        : max_events_(max_events)
        , current_handle_size_(0)
        , epoll_mode_(epoll_mode)
        , events_(NULL)
    {
        set_buffer_size();
    }

    inline virtual ~SL_Socket_Epoll_Runner()
    {
        close();
    }

    inline int set_buffer_size(int recv_buffer_size=65536, int send_buffer_size=65536)
    {
        recv_buffer_.reserve(recv_buffer_size);
        return 0;
    }

    inline int get_recv_buffer_size() const
    {
        return recv_buffer_.buffer_size();
    }

    inline char* get_recv_buffer()
    {
        return recv_buffer_.buffer();
    }

    int open(int event_mask=SL_Socket_Handler::READ_EVENT_MASK, uint max_size=100000, uint max_timeout_ms=10, uint thread_number=0, int8 handler_close_status=SL_Socket_Handler::STATUS_CLOSE)
    {
        if (max_size <= 0)
        {
            return -1;
        }

        epoll_handle_ = epoll_create(max_size);
        if (epoll_handle_ < 0)
        {
            return -2;
        }
        if (max_events_ > max_size)
        {
            max_events_ = max_size;
        }
        events_         = new epoll_event[max_events_];
        event_mask_     = event_mask;
        max_size_       = max_size;
        max_timeout_ms_ = max_timeout_ms;
        handler_close_status_ = handler_close_status;
        return 0;
    }

    int close()
    {
        event_loop_thread_.stop();
        event_loop_thread_.join();
        current_handle_size_ = 0;
        if (NULL != events_)
        {
            delete []events_;
            events_ = NULL;
        }
        ::close(epoll_handle_);
        return 0;
    }

    inline int add_handle(SL_Socket_Handler *socket_handler, int event_mask)
    {
        if (NULL == socket_handler)
        {
            return -1;
        }
        if (socket_handler->socket_ < 0)
        {
            return -2;
        }

        mutex_.lock();
        if (socket_handler->event_mask_ != SL_Socket_Handler::NULL_EVENT_MASK)
        {
            mutex_.unlock();
            return -3;
        }
        if (current_handle_size_ >= max_size_)
        {
            mutex_.unlock();
            return -4;
        }
        struct epoll_event ev;
        memset(&ev, 0, sizeof(epoll_event));
        ev.data.ptr = socket_handler;
        if (epoll_mode_ == ET_MODE)
        {
            ev.events = EPOLLET;
        }
#ifdef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
        ev.events |= EPOLLIN;
        if (epoll_ctl(epoll_handle_, EPOLL_CTL_ADD, socket_handler->socket_, &ev) < 0)
        {
            mutex_.unlock();
            return -5;
        }
        socket_handler->event_mask_ = SL_Socket_Handler::READ_EVENT_MASK;
#else
        int temp_event_mask = SL_Socket_Handler::NULL_EVENT_MASK;
        if (event_mask & SL_Socket_Handler::READ_EVENT_MASK & event_mask_)
        {
            ev.events |= EPOLLIN;
            temp_event_mask |= SL_Socket_Handler::READ_EVENT_MASK;
        }
        if (event_mask & SL_Socket_Handler::WRITE_EVENT_MASK & event_mask_)
        {
            ev.events |= EPOLLOUT;
            temp_event_mask |= SL_Socket_Handler::WRITE_EVENT_MASK;
        }
        if (event_mask & SL_Socket_Handler::EXCEPT_EVENT_MASK & event_mask_)
        {
            ev.events |= EPOLLERR;
            temp_event_mask |= SL_Socket_Handler::EXCEPT_EVENT_MASK;
        }
        if (epoll_ctl(epoll_handle_, EPOLL_CTL_ADD, socket_handler->socket_, &ev) < 0)
        {
            mutex_.unlock();
            return -5;
        }
        socket_handler->event_mask_ = temp_event_mask;
#endif
        ++current_handle_size_;
        mutex_.unlock();
        return 0;
    }

    inline int del_handle(SL_Socket_Handler *socket_handler)
    {
        if (NULL == socket_handler)
        {
            return -1;
        }
        if (socket_handler->socket_ < 0)
        {
            return -2;
        }

        mutex_.lock();
        if (SL_Socket_Handler::NULL_EVENT_MASK == socket_handler->event_mask_)
        {
            mutex_.unlock();
            return -3;
        }
        if (epoll_ctl(epoll_handle_, EPOLL_CTL_DEL, socket_handler->socket_, NULL) < 0)
        {
            mutex_.unlock();
            return -4;
        }
        socket_handler->event_mask_ = SL_Socket_Handler::NULL_EVENT_MASK;
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
        if (NULL == socket_handler)
        {
            return -1;
        }
        if (socket_handler->socket_ < 0)
        {
            return -2;
        }

        mutex_.lock();
        if (SL_Socket_Handler::NULL_EVENT_MASK == socket_handler->event_mask_)
        {
            mutex_.unlock();
            return -3;
        }
        if (epoll_ctl(epoll_handle_, EPOLL_CTL_DEL, socket_handler->socket_, NULL) < 0)
        {
            mutex_.unlock();
            return -3;
        }
        socket_handler->event_mask_ = SL_Socket_Handler::NULL_EVENT_MASK;
        --current_handle_size_;
        mutex_.unlock();
        return 0;
    }

    inline int set_event_mask(SL_Socket_Handler *socket_handler, int event_mask)
    {
        if (NULL == socket_handler)
        {
            return -1;
        }
#ifndef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
        if (socket_handler->socket_ < 0)
        {
            return -2;
        }
        if (!(event_mask & SL_Socket_Handler::ALL_EVENT_MASK & event_mask_))
        {
            return -3;
        }

        mutex_.lock();
        struct epoll_event ev;
        memset(&ev, 0, sizeof(epoll_event));
        ev.data.ptr = socket_handler;
        if (epoll_mode_ == ET_MODE)
        {
            ev.events = EPOLLET;
        }
        int temp_event_mask = SL_Socket_Handler::NULL_EVENT_MASK;
        if (event_mask & SL_Socket_Handler::READ_EVENT_MASK & event_mask_)
        {
            ev.events |= EPOLLIN;
            temp_event_mask |= SL_Socket_Handler::READ_EVENT_MASK;
        }
        if (event_mask & SL_Socket_Handler::WRITE_EVENT_MASK & event_mask_)
        {
            ev.events |= EPOLLOUT;
            temp_event_mask |= SL_Socket_Handler::WRITE_EVENT_MASK;
        }
        if (event_mask & SL_Socket_Handler::EXCEPT_EVENT_MASK & event_mask_)
        {
            ev.events |= EPOLLERR;
            temp_event_mask |= SL_Socket_Handler::EXCEPT_EVENT_MASK;
        }
        if (epoll_ctl(epoll_handle_, EPOLL_CTL_MOD, socket_handler->socket_, &ev) < 0)
        {
            mutex_.unlock();
            return -4;
        }
        socket_handler->event_mask_ = temp_event_mask;
        mutex_.unlock();
#endif
        return 0;
    }

    inline size_t get_handler_size() const
    {
        return current_handle_size_;
    }

    inline int event_loop(int timeout_ms=1)
    {
        int ret = epoll_wait(epoll_handle_, events_, max_events_, timeout_ms);
        if (ret > 0)
        {
            SL_Socket_Handler *socket_handler;
#ifdef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
            for (int i=0; i<ret; ++i)
            {
                socket_handler = (SL_Socket_Handler *)events_[i].data.ptr;
                if (socket_handler->handle_read() < 0)
                {
                    socket_handler->next_status_ = handler_close_status_;
                    del_handle(socket_handler);
                }
            }
#else
            for (int i=0; i<ret; ++i)
            {
                socket_handler = (SL_Socket_Handler *)events_[i].data.ptr;
                if (events_[i].events & EPOLLIN)
                {
                    if (socket_handler->handle_read() < 0)
                    {
                        socket_handler->next_status_ = handler_close_status_;
                        del_handle(socket_handler);
                    }
                }
                if (events_[i].events & EPOLLOUT)
                {
                    if (socket_handler->handle_write() < 0)
                    {
                        socket_handler->last_errno_  = SL_Socket_Handler::ERROR_CLOSE_ACTIVE;
                        socket_handler->next_status_ = SL_Socket_Handler::STATUS_CLOSE;
                        del_handle(socket_handler);
                    }
                }
                if (events_[i].events & EPOLLERR)
                {
                    if (socket_handler->handle_exception() < 0)
                    {
                        socket_handler->last_errno_  = SL_Socket_Handler::ERROR_CLOSE_PASSIVE;
                        socket_handler->next_status_ = SL_Socket_Handler::STATUS_CLOSE;
                        del_handle(socket_handler);
                    }
                }
            }
#endif
        }
        return ret;
    }

    inline int event_dispatch()
    {
        return event_loop(-1);
    }

    inline int thread_event_loop(int timeout_ms=1)
    {
        max_timeout_ms_ = timeout_ms;
        return event_loop_thread_.start(event_loop_proc, this);
    }

    inline int thread_wait()
    {
        event_loop_thread_.join();
        return 0;
    }

    inline int thread_stop()
    {
        event_loop_thread_.stop();
        return 0;
    }

    static void* event_loop_proc(void *arg)
    {
        SL_Socket_Epoll_Runner<TSyncMutex> *runner =(SL_Socket_Epoll_Runner<TSyncMutex>*)arg;
        while (1)
        {
            if (!runner->event_loop_thread_.get_running())
            {
                break;
            }
            runner->event_loop(runner->max_timeout_ms_);
        }
        runner->event_loop_thread_.exit();
        return 0;
    }

private:
    enum EPOLL_MODE
    {
        LT_MODE = 1,
        ET_MODE = 2,
    };

    int     event_mask_;
    uint    max_size_;
    uint    max_events_;
    uint    max_timeout_ms_;
    uint    current_handle_size_;
    int8    handler_close_status_;

    int     epoll_mode_;
    int     epoll_handle_;
    struct  epoll_event *events_;

    TSyncMutex      mutex_;
    SL_Thread       event_loop_thread_;
    SL_ByteBuffer   recv_buffer_;
};

#endif

#endif

