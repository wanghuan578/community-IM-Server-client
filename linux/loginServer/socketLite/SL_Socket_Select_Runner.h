#ifndef SOCKETLITE_SOCKET_SELECT_RUNNER_H
#define SOCKETLITE_SOCKET_SELECT_RUNNER_H

#include <algorithm>
#include <map>
#include <vector>
#include "SL_Config.h"
#include "SL_Socket_Runner.h"
#include "SL_Socket_CommonAPI.h"
#include "SL_Socket_TcpServer.h"
#include "SL_Socket_TcpServer_Handler.h"
#include "SL_Sync_Mutex.h"
#include "SL_Thread.h"
#include "SL_ByteBuffer.h"

#ifndef SOCKETLITE_OS_WINDOWS
    #include <sys/select.h>
#endif

template <typename TSyncMutex>
class SL_Socket_Select_Runner : public SL_Socket_Runner
{
public:
    SL_Socket_Select_Runner()
    {
        temp_handlers_active_   = &temp1_handlers_;
        temp_handlers_standby_  = &temp2_handlers_;
        set_buffer_size();
    }

    virtual ~SL_Socket_Select_Runner()
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
        if (max_size > FD_SETSIZE)
        {
            max_size    = FD_SETSIZE;
        }
        event_mask_     = event_mask;
        max_size_       = max_size;
        max_timeout_ms_ = max_timeout_ms;
        max_handle_     = 0;
        handler_close_status_ = handler_close_status;

        temp1_handlers_.clear();
        temp2_handlers_.clear();
        socket_handlers_.clear();
        socket_handlers_.reserve(max_size*2);

        FD_ZERO(&fd_set_in_.read);
        FD_ZERO(&fd_set_out_.read);
#ifndef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
        FD_ZERO(&fd_set_in_.write);
        FD_ZERO(&fd_set_in_.except);
        FD_ZERO(&fd_set_out_.write);
        FD_ZERO(&fd_set_out_.except);
#endif
        return 0;
    }

    int close()
    {
        event_loop_thread_.stop();
        event_loop_thread_.join();
        temp1_handlers_.clear();
        temp2_handlers_.clear();
        socket_handlers_.clear();
        max_handle_ = -1;
        return 0;
    }

    inline int add_handle(SL_Socket_Handler *socket_handler, int event_mask)
    {
        if (NULL == socket_handler)
        {
            return -1;
        }
        if ( (socket_handler->socket_ < 0) || (socket_handler->socket_ == SL_INVALID_SOCKET) )
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
        FD_SET(socket_handler->socket_, &fd_set_in_.read);
        socket_handler->event_mask_ = SL_Socket_Handler::READ_EVENT_MASK;
#else
        int temp_event_mask = SL_Socket_Handler::NULL_EVENT_MASK;
        if (event_mask & SL_Socket_Handler::READ_EVENT_MASK & event_mask_)
        {
            FD_SET(socket_handler->socket_, &fd_set_in_.read);
            temp_event_mask |= SL_Socket_Handler::READ_EVENT_MASK;
        }
        if (event_mask & SL_Socket_Handler::WRITE_EVENT_MASK & event_mask_)
        {
            FD_SET(socket_handler->socket_, &fd_set_in_.write);
            temp_event_mask |= SL_Socket_Handler::WRITE_EVENT_MASK;
        }
        if (event_mask & SL_Socket_Handler::EXCEPT_EVENT_MASK & event_mask_)
        {
            FD_SET(socket_handler->socket_, &fd_set_in_.except);
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
        if (NULL == socket_handler)
        {
            return -1;
        }
        if ( (socket_handler->socket_ < 0) || (socket_handler->socket_ == SL_INVALID_SOCKET) )
        {
            return -2;
        }

        mutex_.lock();
        if (SL_Socket_Handler::NULL_EVENT_MASK == socket_handler->event_mask_)
        {
            mutex_.unlock();
            return -3;
        }

#ifdef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
        FD_CLR(socket_handler->socket_, &fd_set_in_.read);
#else
        if (socket_handler->event_mask_ & SL_Socket_Handler::READ_EVENT_MASK)
        {
            FD_CLR(socket_handler->socket_, &fd_set_in_.read);
        }
        if (socket_handler->event_mask_ & SL_Socket_Handler::WRITE_EVENT_MASK)
        {
            FD_CLR(socket_handler->socket_, &fd_set_in_.write);
        }
        if (socket_handler->event_mask_ & SL_Socket_Handler::EXCEPT_EVENT_MASK)
        {
            FD_CLR(socket_handler->socket_, &fd_set_in_.except);
        }
#endif
        socket_handler->event_mask_ = SL_Socket_Handler::NULL_EVENT_MASK;
        (*temp_handlers_standby_)[socket_handler] = DEL_HANDLE;
        mutex_.unlock();
        return 0;
    }

    inline int remove_handle(SL_Socket_Handler *socket_handler)
    {
        if (NULL == socket_handler)
        {
            return -1;
        }
        if ( (socket_handler->socket_ < 0) || (socket_handler->socket_ == SL_INVALID_SOCKET) )
        {
            return -2;
        }

        mutex_.lock();
        if (SL_Socket_Handler::NULL_EVENT_MASK == socket_handler->event_mask_)
        {
            mutex_.unlock();
            return -3;
        }
#ifdef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
        FD_CLR(socket_handler->socket_, &fd_set_in_.read);
#else
        if (socket_handler->event_mask_ & SL_Socket_Handler::READ_EVENT_MASK)
        {
            FD_CLR(socket_handler->socket_, &fd_set_in_.read);
        }
        if (socket_handler->event_mask_ & SL_Socket_Handler::WRITE_EVENT_MASK)
        {
            FD_CLR(socket_handler->socket_, &fd_set_in_.write);
        }
        if (socket_handler->event_mask_ & SL_Socket_Handler::EXCEPT_EVENT_MASK)
        {
            FD_CLR(socket_handler->socket_, &fd_set_in_.except);
        }
#endif
        socket_handler->event_mask_ = SL_Socket_Handler::NULL_EVENT_MASK;
        (*temp_handlers_standby_)[socket_handler] = REMOVE_HANDLE;
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
        if ( (socket_handler->socket_ < 0) || (socket_handler->socket_ == SL_INVALID_SOCKET) )
        {
            return -2;
        }

        mutex_.lock();
        int temp_event_mask = SL_Socket_Handler::NULL_EVENT_MASK;
        if (socket_handler->event_mask_ & SL_Socket_Handler::READ_EVENT_MASK)
        {
            if (!(event_mask & SL_Socket_Handler::READ_EVENT_MASK & event_mask_))
            {
                FD_CLR(socket_handler->socket_, &fd_set_in_.read);
            }
            else
            {
                temp_event_mask |= SL_Socket_Handler::READ_EVENT_MASK;
            }
        }
        else if (event_mask & SL_Socket_Handler::READ_EVENT_MASK & event_mask_)
        {
            FD_SET(socket_handler->socket_, &fd_set_in_.read);
            temp_event_mask |= SL_Socket_Handler::READ_EVENT_MASK;
        }
        if (socket_handler->event_mask_ & SL_Socket_Handler::WRITE_EVENT_MASK)
        {
            if (!(event_mask & SL_Socket_Handler::WRITE_EVENT_MASK & event_mask_))
            {
                FD_CLR(socket_handler->socket_, &fd_set_in_.write);
            }
            else
            {
                temp_event_mask |= SL_Socket_Handler::WRITE_EVENT_MASK;
            }
        }
        else if (event_mask & SL_Socket_Handler::WRITE_EVENT_MASK & event_mask_)
        {
            FD_SET(socket_handler->socket_, &fd_set_in_.write);
            temp_event_mask |= SL_Socket_Handler::WRITE_EVENT_MASK;
        }
        if (socket_handler->event_mask_ & SL_Socket_Handler::EXCEPT_EVENT_MASK)
        {
            if (!(event_mask & SL_Socket_Handler::EXCEPT_EVENT_MASK & event_mask_))
            {
                FD_SET(socket_handler->socket_, &fd_set_in_.except);
            }
            else
            {
                temp_event_mask |= SL_Socket_Handler::EXCEPT_EVENT_MASK;
            }
        }
        else if (event_mask & SL_Socket_Handler::EXCEPT_EVENT_MASK & event_mask_)
        {
            FD_SET(socket_handler->socket_, &fd_set_in_.except);
            temp_event_mask |= SL_Socket_Handler::EXCEPT_EVENT_MASK;
        }
        socket_handler->event_mask_ = temp_event_mask;
        mutex_.unlock();
#endif
        return 0;
    }

    inline size_t get_handler_size() const
    {
        return socket_handlers_.size();
    }

    int event_loop(int timeout_ms=1)
    {
        SL_Socket_Handler *socket_handler;
        int ret = 0;

        //交换active/standby指针
        if (!temp_handlers_standby_->empty())
        {
            mutex_.lock();
            std::swap(temp_handlers_standby_, temp_handlers_active_);

            //fd_set的复制方法:
            //  1)标准的memcpy(&fd_set_out_.read, &fd_set_in_.read, sizeof(fd_set))
            //  2)fd_set的赋值操作(fd_set_out_.read = fd_set_in_.read)
            memcpy(&fd_set_out_.read, &fd_set_in_.read, sizeof(fd_set));
#ifndef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
            memcpy(&fd_set_out_.write, &fd_set_in_.write, sizeof(fd_set));
            memcpy(&fd_set_out_.except, &fd_set_in_.except, sizeof(fd_set));
#endif
            mutex_.unlock();

            SL_Socket_Handler *end_handler;
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
                    if (max_handle_ < (int)socket_handler->socket_)
                    {
                        max_handle_ = socket_handler->socket_;
                    }
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
                            end_handler->runner_pos_ = socket_handler->runner_pos_;
                            socket_handlers_[socket_handler->runner_pos_] = end_handler;
                        }
                        socket_handlers_.pop_back();
                    }
                    socket_handler->runner_pos_ = -1;
                    if (max_handle_ == (int)socket_handler->socket_)
                    {
                        --max_handle_;
                    }
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
                            end_handler->runner_pos_ = socket_handler->runner_pos_;
                            socket_handlers_[socket_handler->runner_pos_] = end_handler;
                        }
                        socket_handlers_.pop_back();
                    }
                    socket_handler->runner_pos_ = -1;
                    if (max_handle_ == (int)socket_handler->socket_)
                    {
                        --max_handle_;
                    }
                }
            }
            temp_handlers_active_->clear();
        }
        else
        {
            mutex_.lock();

            //fd_set的复制方法:
            //  1)标准的memcpy(&fd_set_out_.read, &fd_set_in_.read, sizeof(fd_set))
            //  2)fd_set的赋值操作(fd_set_out_.read = fd_set_in_.read)
            memcpy(&fd_set_out_.read, &fd_set_in_.read, sizeof(fd_set));
#ifndef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
            memcpy(&fd_set_out_.write, &fd_set_in_.write, sizeof(fd_set));
            memcpy(&fd_set_out_.except, &fd_set_in_.except, sizeof(fd_set));
#endif
            mutex_.unlock();
        }

#ifdef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
        if (timeout_ms >= 0)
        {
            timeval tv;
            tv.tv_sec   = timeout_ms / 1000;
            tv.tv_usec  = (timeout_ms - tv.tv_sec * 1000) * 1000;
            ret = select(max_handle_+1, &fd_set_out_.read, NULL, NULL, &tv);
        }
        else
        {
            ret = select(max_handle_+1, &fd_set_out_.read, NULL, NULL, NULL);
        }
        if (ret > 0)
        {
            SOCKET_HANDLES::iterator iter = socket_handlers_.begin();
            SOCKET_HANDLES::iterator iter_end = socket_handlers_.end();
            for (; iter != iter_end; ++iter)
            {
                socket_handler = *iter;
                if (FD_ISSET(socket_handler->socket_, &fd_set_out_.read))
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
        }
#else
        if (timeout_ms >= 0)
        {
            timeval tv;
            tv.tv_sec   = timeout_ms / 1000;
            tv.tv_usec  = (timeout_ms - tv.tv_sec * 1000) * 1000;
            ret = select(max_handle_+1, &fd_set_out_.read, &fd_set_out_.write, &fd_set_out_.except, &tv);
        }
        else
        {
            ret = select(max_handle_+1, &fd_set_out_.read, &fd_set_out_.write, &fd_set_out_.except, NULL);
        }
        if (ret > 0)
        {
            SOCKET_HANDLES::iterator iter = socket_handlers_.begin();
            SOCKET_HANDLES::iterator iter_end = socket_handlers_.end(); 
            for (; iter != iter_end; ++iter)
            {
                socket_handler = *iter;
                if (FD_ISSET(socket_handler->socket_, &fd_set_out_.read))
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
                if (FD_ISSET(socket_handler->socket_, &fd_set_out_.write))
                {
                    if (socket_handler->handle_write() < 0)
                    {
                        socket_handler->last_errno_  = SL_Socket_Handler::ERROR_CLOSE_ACTIVE;
                        socket_handler->next_status_ = SL_Socket_Handler::STATUS_CLOSE;
                        del_handle(socket_handler);
                    }
                    if ((--ret) <= 0)
                    {
                        break;
                    }
                }
                if (FD_ISSET(socket_handler->socket_, &fd_set_out_.except))
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
        }
#endif
#ifdef SOCKETLITE_OS_WINDOWS
        else if (ret < 0)
        {
            if (timeout_ms > 0)
            {
                SL_Socket_CommonAPI::util_sleep_ms(timeout_ms);
            }
            else
            {
                SL_Socket_CommonAPI::util_sleep_ms(1);
            }
        }
#endif
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

#ifdef SOCKETLITE_OS_WINDOWS
    static unsigned int WINAPI event_loop_proc(void *arg)
#else
    static void* event_loop_proc(void *arg)
#endif
    {
        SL_Socket_Select_Runner<TSyncMutex> *runner = (SL_Socket_Select_Runner<TSyncMutex> *)arg;
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
    typedef std::vector<SL_Socket_Handler* >    SOCKET_HANDLES;
    typedef std::map<SL_Socket_Handler*, int>   TEMP_SOCKET_HANDLES;

    enum
    {
        ADD_HANDLE      = 1,
        DEL_HANDLE      = 2,
        REMOVE_HANDLE   = 3,
        SET_EVENT_MASK  = 4
    };

#ifdef SOCKETLITE_RUNNER_EVENTMASK_ONLYREAD
    struct SL_FD_SET
    {
        fd_set  read;
    };
#else
    struct SL_FD_SET
    {
        fd_set  read;
        fd_set  write;
        fd_set  except;
    };
#endif

    int   event_mask_;
    uint  max_size_;
    uint  max_handle_;
    uint  max_timeout_ms_;
    uint8 handler_close_status_;

    SL_FD_SET                       fd_set_in_;
    SL_FD_SET                       fd_set_out_;

    TSyncMutex                      mutex_;
    SL_Thread                       event_loop_thread_;
    SOCKET_HANDLES                  socket_handlers_;

    TEMP_SOCKET_HANDLES             temp1_handlers_;
    TEMP_SOCKET_HANDLES             temp2_handlers_;
    TEMP_SOCKET_HANDLES             *temp_handlers_active_;
    TEMP_SOCKET_HANDLES             *temp_handlers_standby_;

    SL_ByteBuffer                   recv_buffer_;
};

#endif

