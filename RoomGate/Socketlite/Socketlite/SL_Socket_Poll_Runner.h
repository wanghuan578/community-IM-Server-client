#ifndef SOCKETLITE_SOCKET_POLL_RUNNER_H
#define SOCKETLITE_SOCKET_POLL_RUNNER_H

#include "SL_Config.h"
#include "SL_Socket_Runner.h"
#include "SL_Socket_CommonAPI.h"
#include "SL_Socket_TcpServer.h"
#include "SL_Socket_TcpServer_Handler.h"
#include "SL_Sync_Mutex.h"
#include "SL_Thread.h"
#include "SL_ByteBuffer.h"
#include <algorithm>
#include <map>
#include <vector>

#ifndef SOCKETLITE_OS_WINDOWS
#include <poll.h>

template <typename TSyncMutex>
class SOCKETLITE_API SL_Socket_Poll_Runner : public SL_Socket_Runner 
{
protected:
    inline short to_pollevent_i(int event_mask)
    {
        int poll_event = SL_Socket_Handler::NULL_EVENT_MASK;
        if (event_mask & SL_Socket_Handler::READ_EVENT_MASK)
        {
            poll_event |= POLLIN;
        }
        if (event_mask & SL_Socket_Handler::WRITE_EVENT_MASK)
        {
            poll_event |= POLLOUT;
        }
        if (event_mask & SL_Socket_Handler::EXCEPT_EVENT_MASK)
        {
            poll_event |= POLLIN;
        }
        return poll_event;
    }

public:
    SL_Socket_Poll_Runner()
        : socket_pollfds_(NULL)
    {
        temp_handlers_active_   = &temp1_handlers_;
        temp_handlers_standby_  = &temp2_handlers_;
        set_buffer_size();
    }

    virtual ~SL_Socket_Poll_Runner()
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

    int open(int event_mask=SL_Socket_Handler::READ_EVENT_MASK, uint max_size=1024, uint max_timeout_ms=10, uint thread_number=0, int8 handler_close_status=SL_Socket_Handler::STATUS_CLOSE)
    {
        if (max_size <= 0)
        {
            return -1;
        }

        event_mask_     = event_mask;
        max_size_       = max_size;
        max_timeout_ms_ = max_timeout_ms;
        handler_close_status_ = handler_close_status;

        temp1_handlers_.clear();
        temp2_handlers_.clear();
        socket_handlers_.clear();
        socket_handlers_.reserve(max_size * 2);
        socket_pollfds_ = new pollfd[max_size];
        return 0;
    }

    int close()
    {
        event_loop_thread_.stop();
        event_loop_thread_.join();
		if (NULL != socket_pollfds_)
        {
            delete []socket_pollfds_;
            socket_pollfds_ = NULL;
        }
        temp1_handlers_.clear();
        temp2_handlers_.clear();
        socket_handlers_.clear();
        return 0;
    }

    inline inline int add_handle(SL_Socket_Handler *socket_handler, int event_mask)
    {
        if (socket_handler->socket_ < 0)
        {
            return -1;
        }

        mutex_.lock();
        if (socket_handler->event_mask_ != SL_Socket_Handler::NULL_EVENT_MASK)
        {
            mutex_.unlock();
            return -2;
        }
        if (socket_handlers_.size() >= (max_size_-temp_handlers_standby_->size()))
        {
            mutex_.unlock();
            return -3;
        }
#ifdef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
        socket_handler->event_mask_ = SL_Socket_Handler::READ_EVENT_MASK;
#else
        int temp_event_mask = SL_Socket_Handler::NULL_EVENT_MASK;
        if (event_mask & SL_Socket_Handler::READ_EVENT_MASK & event_mask_)
        {
            temp_event_mask |= SL_Socket_Handler::READ_EVENT_MASK;
        }
        if (event_mask & SL_Socket_Handler::WRITE_EVENT_MASK & event_mask_)
        {
            temp_event_mask |= SL_Socket_Handler::WRITE_EVENT_MASK;
        }
        if (event_mask & SL_Socket_Handler::EXCEPT_EVENT_MASK & event_mask_)
        {
            temp_event_mask |= SL_Socket_Handler::EXCEPT_EVENT_MASK;
        }
        socket_handler->event_mask_ = temp_event_mask;
#endif
        (*temp_handlers_standby_)[socket_handler] = ADD_HANDLE;
        mutex_.unlock();
        return 0;
    }

    inline int del_handle(SL_Socket_Handler *socket_handler)
    {
        if (socket_handler->socket_ < 0)
        {
            return -1;
        }

        mutex_.lock();
        if (SL_Socket_Handler::NULL_EVENT_MASK == socket_handler->event_mask_)
        {
            mutex_.unlock();
            return -2;
        }
        socket_handler->event_mask_ = SL_Socket_Handler::NULL_EVENT_MASK;
        (*temp_handlers_standby_)[socket_handler] = DEL_HANDLE;
        mutex_.unlock();
        return 0;
    }

    inline int remove_handle(SL_Socket_Handler *socket_handler)
    {
        if (socket_handler->socket_ < 0)
        {
            return -1;
        }

        mutex_.lock();
        if (SL_Socket_Handler::NULL_EVENT_MASK == socket_handler->event_mask_)
        {
            mutex_.unlock();
            return -2;
        }
        socket_handler->event_mask_ = SL_Socket_Handler::NULL_EVENT_MASK;
        (*temp_handlers_standby_)[socket_handler] = REMOVE_HANDLE;
        mutex_.unlock();
        return 0;
    }

    inline int set_event_mask(SL_Socket_Handler *socket_handler, int event_mask)
    {
#ifndef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
        if (socket_handler->socket_ < 0)
        {
            return -1;
        }
        if (!(event_mask & SL_Socket_Handler::ALL_EVENT_MASK & event_mask_))
        {
            return -2;
        }

        mutex_.lock();
        int temp_event_mask = SL_Socket_Handler::NULL_EVENT_MASK;
        if (socket_handler->event_mask_ & SL_Socket_Handler::READ_EVENT_MASK)
        {
            if (event_mask & SL_Socket_Handler::READ_EVENT_MASK & event_mask_)
            {
                temp_event_mask |= SL_Socket_Handler::READ_EVENT_MASK;
            }
        }
        else if (event_mask & SL_Socket_Handler::READ_EVENT_MASK & event_mask_)
        {
            temp_event_mask |= SL_Socket_Handler::READ_EVENT_MASK;
        }
        if (socket_handler->event_mask_ & SL_Socket_Handler::WRITE_EVENT_MASK)
        {
            if (event_mask & SL_Socket_Handler::WRITE_EVENT_MASK & event_mask_)
            {
                temp_event_mask |= SL_Socket_Handler::WRITE_EVENT_MASK;
            }
        }
        else if (event_mask & SL_Socket_Handler::WRITE_EVENT_MASK & event_mask_)
        {
            temp_event_mask |= SL_Socket_Handler::WRITE_EVENT_MASK;
        }
        if (socket_handler->event_mask_ & SL_Socket_Handler::EXCEPT_EVENT_MASK)
        {
            if (event_mask & SL_Socket_Handler::EXCEPT_EVENT_MASK & event_mask_)
            {
                temp_event_mask |= SL_Socket_Handler::EXCEPT_EVENT_MASK;
            }
        }
        else if (event_mask & SL_Socket_Handler::EXCEPT_EVENT_MASK & event_mask_)
        {
            temp_event_mask |= SL_Socket_Handler::EXCEPT_EVENT_MASK;
        }
        socket_handler->event_mask_ = temp_event_mask;
        (*temp_handlers_standby_)[socket_handler] = SET_EVENT_MASK;
        mutex_.unlock();
#endif
        return 0;
    }

    inline size_t get_handler_size() const
    {
        return socket_handlers_.size();
    }

    int event_loop(int timeout_ms=10)
    {
        SL_Socket_Handler *socket_handler;
        int ret = 0;

        //交换active/standby指针(使用std::swap函数)
        if (!temp_handlers_standby_->empty())
        {
            mutex_.lock();
            std::swap(temp_handlers_standby_, temp_handlers_active_);
            mutex_.unlock();

            SL_Socket_Handler *end_handler;
            SL_Socket_Handler *source_handler;
            TEMP_SOCKET_HANDLES::iterator iter = temp_handlers_active_->begin();
            TEMP_SOCKET_HANDLES::iterator iter_end = temp_handlers_active_->end();
            for (; iter != iter_end; ++iter)
            {
                socket_handler = iter->first;
                if (iter->second == ADD_HANDLE)
                {
                    if (socket_handler->runner_pos_ < 0)
                    {
                        socket_handlers_.push_back(socket_handler);
                        socket_handler->runner_pos_ = socket_handlers_.size()-1;
                    }
                    socket_pollfds_[socket_handler->runner_pos_].fd      = socket_handler->socket_;
                    socket_pollfds_[socket_handler->runner_pos_].events  = to_pollevent_i(socket_handler->event_mask_);
                    socket_pollfds_[socket_handler->runner_pos_].revents = 0;
                }
                else if (iter->second == DEL_HANDLE)
                {
                    if (socket_handler->runner_pos_ < 0)
                    {
                        continue;
                    }
                    if (socket_handlers_.size() > 0)
                    {
                        if (socket_handler->runner_pos_ != (socket_handlers_.size()-1))
                        {
                            end_handler = socket_handlers_[socket_handlers_.size()-1];
                            socket_pollfds_[socket_handler->runner_pos_].fd = end_handler->socket_;
                            socket_pollfds_[socket_handler->runner_pos_].events = to_pollevent_i(end_handler->event_mask_);
                            end_handler->runner_pos_ = socket_handler->runner_pos_;
                            socket_handlers_[socket_handler->runner_pos_] = end_handler;
                        }
                        socket_handlers_.pop_back();
                    }
                    socket_handler->runner_pos_ = -1;
                    ret = socket_handler->handle_close();
                    socket_handler->socket_source_->disconnect(socket_handler);
                    if (ret >= 0)
                    {
                        socket_handler->socket_source_->free_handler(socket_handler);
                    }
                }
                else if (iter->second == REMOVE_HANDLE)
                {
                    if (socket_handler->runner_pos_ < 0)
                    {
                        continue;
                    }
                    if (socket_handlers_.size() > 0)
                    {
                        if (socket_handler->runner_pos_ != (socket_handlers_.size()-1))
                        {
                            end_handler = socket_handlers_[socket_handlers_.size()-1];
                            socket_pollfds_[socket_handler->runner_pos_].fd = end_handler->socket_;
                            socket_pollfds_[socket_handler->runner_pos_].events = to_pollevent_i(end_handler->event_mask_);
                            end_handler->runner_pos_ = socket_handler->runner_pos_;
                            socket_handlers_[socket_handler->runner_pos_] = end_handler;
                        }
                        socket_handlers_.pop_back();
                    }
                    socket_handler->runner_pos_ = -1;
                }
                else if (iter->second == SET_EVENT_MASK)
                {
                    socket_pollfds_[socket_handler->runner_pos_].fd = socket_handler->socket_;
                    socket_pollfds_[socket_handler->runner_pos_].events = to_pollevent_i(socket_handler->event_mask_);
                }
            }
            temp_handlers_active_->clear();
        }

        uint socket_pollfds_size = socket_handlers_.size();
        ret = poll(socket_pollfds_, socket_pollfds_size, timeout_ms);
        if (ret > 0)
        {
#ifdef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
            for (uint i=0; i<socket_pollfds_size; ++i)
            {
                if (socket_pollfds_[i].revents & POLLIN)
                {
                    if (socket_handler->handle_read() < 0)
                    {
                        socket_handler->next_status_ = handler_close_status_;
                        del_handle(socket_handler);
                    }
                    if ((--ret) <= 0)
                    {
                        break;
                    }
                }
            }
#else
            for (uint i=0; i<socket_pollfds_size; ++i)
            {
                if (socket_pollfds_[i].revents & POLLIN)
                {
                    if (socket_handler->handle_read() < 0)
                    {
                        socket_handler->next_status_ = handler_close_status_;
                        del_handle(socket_handler);
                    }
                    if ((--ret) <= 0)
                    {
                        break;
                    }
                }
                if (socket_pollfds_[i].revents & POLLOUT)
                {
                    if (socket_handler->handle_write() < 0)
                    {
                        socket_handler->last_errno_  = SL_Socket_Handler::ERROR_CLOSE_PASSIVE;
                        socket_handler->next_status_ = SL_Socket_Handler::STATUS_CLOSE;
                        del_handle(socket_handler);
                    }
                    if ((--ret) <= 0)
                    {
                        break;
                    }
                }
                if (socket_pollfds_[i].revents & (POLLERR|POLLHUP))
                {
                    if (socket_handler->handle_exception() < 0)
                    {
                        socket_handler->last_errno_  = SL_Socket_Handler::ERROR_CLOSE_PASSIVE;
                        socket_handler->next_status_ = SL_Socket_Handler::STATUS_CLOSE;
                        del_handle(socket_handler);
                    }
                    if ((--ret) <= 0)
                    {
                        break;
                    }
                }
            }
#endif
        }
        return ret;
    }

    inline size_t get_handler_size() const
    {
        return socket_handlers_.size();
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
        SL_Socket_Poll_Runner<TSyncMutex> *runner = (SL_Socket_Poll_Runner<TSyncMutex>*)arg;
        while (1)
        {
            if (!runner->event_loop_thread_.get_running())
            {
                break;
            }
            runner->event_loop(runner->max_timeout_);
        }
        runner->event_loop_thread_.exit();
        return 0;
    }

private:
    typedef std::vector<SL_Socket_Handler* >    SOCKET_HANDLES;
    typedef std::map<SL_Socket_Handler*, int>   TEMP_SOCKET_HANDLES;
    enum
    {
        ADD_HANDLE      = 1,
        DEL_HANDLE      = 2,
        REMOVE_HANDLE   = 3,
        SET_EVENT_MASK  = 4
    };

    int  event_mask_;
    uint max_size_;
    uint max_timeout_ms_;
    int8 handler_close_status_;

    TSyncMutex                      mutex_;
    SL_Thread                       event_loop_thread_;
    SOCKET_HANDLES                  socket_handlers_;
    struct pollfd                   *socket_pollfds_;

    TEMP_SOCKET_HANDLES             temp1_handlers_;
    TEMP_SOCKET_HANDLES             temp2_handlers_;
    TEMP_SOCKET_HANDLES             *temp_handlers_active_;
    TEMP_SOCKET_HANDLES             *temp_handlers_standby_;

    SL_ByteBuffer                   recv_buffer_;
};

#endif

#endif

