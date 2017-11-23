#include <algorithm>
#include "SL_Socket_TcpServer_Handler.h"
#include "SL_Socket_SendControl_Handler.h"
#include "SL_Socket_SendControl_HandlerManager.h"

uint64 SL_Socket_SendControl_HandlerManager::current_timestamp_us_ = 0;
uint64 SL_Socket_SendControl_HandlerManager::current_time_us_ = 0;
uint64 SL_Socket_SendControl_HandlerManager::current_timestamp_ = 0;
uint64 SL_Socket_SendControl_HandlerManager::current_time_ = 0;

SL_Socket_SendControl_HandlerManager::SL_Socket_SendControl_HandlerManager()
    : iovec_(NULL)
    , iovec_count_(1024)
    , thread_number_(1)
    , send_block_size_(65536)
    , next_thread_index_(0)
    , timedwait_signal_(true)
    , direct_send_flag_(true)
{
}

SL_Socket_SendControl_HandlerManager::~SL_Socket_SendControl_HandlerManager()
{
    close();
}

int SL_Socket_SendControl_HandlerManager::open(int  thread_number, 
                                               int  send_block_size,
                                               int  iovec_count, 
                                               int  keepalive_time_ms, 
                                               int  send_delaytime_ms, 
                                               int  scan_waittime_us,
                                               int  close_delaytime_ms, 
                                               bool timedwait_signal,
                                               bool direct_send_flag)
{
    close();

    iovec_count_ = iovec_count;
    if (iovec_count_ > SL_MAX_IOVCNT)
    {
        iovec_count_    = SL_MAX_IOVCNT;
    }

    thread_number_      = thread_number;
    send_block_size_    = send_block_size;
    keepalive_time_ms_  = keepalive_time_ms;
    send_delaytime_ms_  = send_delaytime_ms;
    scan_waittime_us_   = scan_waittime_us;
    close_delaytime_ms_ = close_delaytime_ms;
    timedwait_signal_   = timedwait_signal;
    direct_send_flag_   = direct_send_flag;
    if (thread_number > 0)
    {
        send_threads_.reserve(thread_number);
        iovec_ = NULL;
        SendThread *send_thread;
        for (int i=0; i<thread_number; ++i)
        {
            send_thread                         = new SendThread;
            send_thread->parent                 = this;
            send_thread->index                  = i;
            send_thread->handlers_list_size     = 0;
            send_thread->temp_handlers_size     = 0;
            send_thread->temp_handlers_active   = &(send_thread->temp1_handlers);
            send_thread->temp_handlers_standby  = &(send_thread->temp2_handlers);
            send_thread->temp1_handlers.clear();
            send_thread->temp2_handlers.clear();
            send_thread->timedwait_flag.store(0);
            send_thread->thread.start(send_proc, send_thread);
            send_threads_.push_back(send_thread);
        }
    }
    else
    {
        iovec_ = new SL_IOVEC[iovec_count];
        SendThread *send_thread                 = new SendThread;
        send_thread->parent                     = this;
        send_thread->index                      = 0;
        send_thread->handlers_list_size         = 0;
        send_thread->temp_handlers_size         = 0;
        send_thread->temp_handlers_active       = &(send_thread->temp1_handlers);
        send_thread->temp_handlers_standby      = &(send_thread->temp2_handlers);
        send_thread->temp1_handlers.clear();
        send_thread->temp2_handlers.clear();
        send_thread->timedwait_flag.store(0);
        send_threads_.push_back(send_thread);
    }
    return 0;
}

int SL_Socket_SendControl_HandlerManager::close()
{
    if (NULL != iovec_)
    {
        delete []iovec_;
        iovec_ = NULL;
    }
    SendThread *send_thread;
    std::vector<SendThread* >::iterator iter = send_threads_.begin();
    std::vector<SendThread* >::iterator iter_end = send_threads_.end();
    if (thread_number_ > 0)
    {
        for (; iter != iter_end; ++iter)
        {
            send_thread = *iter;
            send_thread->thread.stop();
            send_thread->thread.join();
            send_thread->handlers_list.clear();
            delete send_thread;
        }
    }
    else
    {
        for (; iter != iter_end; ++iter)
        {
            send_thread = *iter;
            send_thread->handlers_list.clear();
            delete send_thread;
        }
    }
    send_threads_.clear();
    return 0;
}

size_t SL_Socket_SendControl_HandlerManager::handler_size()
{
    size_t handler_size = 0;
    SendThread *send_thread;
    std::vector<SendThread* >::iterator iter = send_threads_.begin();
    std::vector<SendThread* >::iterator iter_end = send_threads_.end();
    for (; iter != iter_end; ++iter)
    {
        send_thread = *iter;
        handler_size += (send_thread->temp_handlers_size + send_thread->handlers_list_size);
    }
    return handler_size;
}

int SL_Socket_SendControl_HandlerManager::assign_thread()
{
    int thread_index = 0;
    int thread_size = (int)send_threads_.size();
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

int SL_Socket_SendControl_HandlerManager::add_handler(SL_Socket_Handler *handler, SL_Socket_SendControl_Interface *sendcontrol)
{
    sendcontrol->handler_manager_ = this;
    sendcontrol->init_control();

    ITEM_SENDCONTROL item_sendcontrol(handler, sendcontrol);
    int thread_index = assign_thread();
    SendThread *send_thread = send_threads_[thread_index];
    sendcontrol->send_thread_index_ = thread_index;
    send_thread->mutex.lock();
    ++send_thread->temp_handlers_size;
    send_thread->temp_handlers_standby->push_back(item_sendcontrol);
    send_thread->mutex.unlock();

    if (timedwait_signal_)
    {
        if (send_thread->timedwait_flag.load() > 0)
        {
            send_thread->timedwait_flag.store(0);
            send_thread->timedwait_condition.signal();
        }
    }

    return 0;
}

int SL_Socket_SendControl_HandlerManager::event_loop(int timeout_ms)
{   
    SL_Socket_Handler *handler;
    SL_Socket_SendControl_Interface *sendcontrol;
    SendThread *send_thread     = send_threads_[0];

    current_timestamp_us_       = SL_Socket_CommonAPI::util_timestamp_us();
    current_time_us_            = SL_Socket_CommonAPI::util_time_us();
    current_timestamp_          = current_timestamp_us_ / 1000LL;
    current_time_               = current_time_us_ / 1000000LL;
    uint64 current_timestamp    = current_timestamp_;
    int write_success_times     = 0;

    //交换active/standby指针(使用std::swap函数)
    if (!send_thread->temp_handlers_standby->empty())
    {
        send_thread->mutex.lock();
        send_thread->handlers_list_size += send_thread->temp_handlers_size;
        send_thread->temp_handlers_size  = 0;
        std::swap(send_thread->temp_handlers_standby, send_thread->temp_handlers_active);
        send_thread->mutex.unlock();

        send_thread->handlers_list.insert(send_thread->handlers_list.end(), 
            send_thread->temp_handlers_active->begin(), 
            send_thread->temp_handlers_active->end());
        send_thread->temp_handlers_active->clear();
    }

    std::list<ITEM_SENDCONTROL>::iterator iter = send_thread->handlers_list.begin();
    std::list<ITEM_SENDCONTROL>::iterator iter_end = send_thread->handlers_list.end();
    while (iter != iter_end)
    {
        handler     = (*iter).first;
        sendcontrol = (*iter_end).second;

        switch (handler->current_status_)
        {
            case SL_Socket_Handler::STATUS_OPEN:
                {
                    ++iter;

                    //发送数据
                    if ( (0 == send_delaytime_ms_) || (current_timestamp - sendcontrol->last_fail_timestamp_ > send_delaytime_ms_) )
                    {
                        if (sendcontrol->write_data(iovec_, iovec_count_, current_timestamp) == SL_Socket_SendControl_Interface::WRITE_RETURN_SEND_SUCCESS)
                        {
                            ++write_success_times;
                        }
                    }

                    //防止死连接
                    if ( (keepalive_time_ms_ > 0) && (current_timestamp - sendcontrol->last_update_timestamp_ > keepalive_time_ms_) )
                    {
                        handler->last_errno_  = SL_Socket_Handler::ERROR_KEEPALIVE_TIMEOUT;
                        handler->next_status_ = SL_Socket_Handler::STATUS_CLOSE;
                        handler->get_socket_runner()->del_handle(handler);
                        handler->current_status_ = SL_Socket_Handler::STATUS_CLOSE;
                    }
                }
                break;
            case SL_Socket_Handler::STATUS_CLOSE_WAIT:
                {
                    ++iter;

                    //发送数据(因为应用主动关闭连接,但可能有数据需要发送)
                    if (sendcontrol->write_data(iovec_, iovec_count_, current_timestamp) == SL_Socket_SendControl_Interface::WRITE_RETURN_SEND_SUCCESS)
                    {
                        ++write_success_times;
                    }

                    //延迟关闭连接
                    if ( (0 == close_delaytime_ms_) || (current_timestamp - sendcontrol->last_update_timestamp_ > close_delaytime_ms_) )
                    {
                        handler->next_status_ = SL_Socket_Handler::STATUS_CLOSE;
                        handler->get_socket_runner()->del_handle(handler);
                        handler->current_status_ = SL_Socket_Handler::STATUS_CLOSE;
                    }
                }
                break;
            case SL_Socket_Handler::STATUS_CLOSE_SEND:
                {
                    ++iter;
                    handler->next_status_ = SL_Socket_Handler::STATUS_CLOSE;
                    handler->get_socket_runner()->del_handle(handler);
                    handler->current_status_ = SL_Socket_Handler::STATUS_CLOSE;
                }
                break;
            case SL_Socket_Handler::STATUS_CLOSE:
                {
                    //关闭连接
                    handler->close();

                    //从集合中删除handler
                    sendcontrol->clear_control();
                    --send_thread->handlers_list_size;
                    iter = send_thread->handlers_list.erase(iter);
                    iter_end = send_thread->handlers_list.end();

                    //回收连接对象
                    handler->get_socket_source()->free_handler(handler);
                }
                break;
            default:
                ++iter;
                break;
        }
    }

    return write_success_times;
}

#ifdef SOCKETLITE_OS_WINDOWS
unsigned int WINAPI SL_Socket_SendControl_HandlerManager::send_proc(void *arg)
#else
void* SL_Socket_SendControl_HandlerManager::send_proc(void *arg)
#endif
{
    SL_Socket_SendControl_HandlerManager::SendThread *send_thread = (SL_Socket_SendControl_HandlerManager::SendThread *)arg;
    SL_Socket_SendControl_HandlerManager *handler_manger = (SL_Socket_SendControl_HandlerManager *)send_thread->parent;

    SL_Socket_Handler *handler;
    SL_Socket_SendControl_Interface *sendcontrol;
    std::list<ITEM_SENDCONTROL>::iterator iter;
    std::list<ITEM_SENDCONTROL>::iterator iter_end;

    int iovec_count             = handler_manger->iovec_count_;
    SL_IOVEC *iovec             = new SL_IOVEC[iovec_count];
    int write_success_times     = 0;
    uint scan_waittime_us       = handler_manger->scan_waittime_us_;
    uint close_delaytime_ms     = handler_manger->close_delaytime_ms_;
    uint keepalive_time_ms      = handler_manger->keepalive_time_ms_;
    uint send_delaytime_ms      = handler_manger->send_delaytime_ms_;
    uint64 current_timestamp    = SL_Socket_CommonAPI::util_process_clock_ms();
        
    while (1)
    {
        if (!send_thread->thread.get_running())
        {
            delete []iovec;
            send_thread->thread.exit();
            return 0;
        }
        write_success_times = 0;

        if (0 == send_thread->index)
        {//第一个发送线程,更新全局时间
            SL_Socket_SendControl_HandlerManager::current_timestamp_us_ = SL_Socket_CommonAPI::util_timestamp_us();
            SL_Socket_SendControl_HandlerManager::current_time_us_      = SL_Socket_CommonAPI::util_time_us();
            SL_Socket_SendControl_HandlerManager::current_time_         = SL_Socket_SendControl_HandlerManager::current_time_us_ / 1000000LL;
            current_timestamp                                           = SL_Socket_SendControl_HandlerManager::current_timestamp_us_ / 1000LL;
            SL_Socket_SendControl_HandlerManager::current_timestamp_    = current_timestamp;
        }
        else
        {
            current_timestamp   = SL_Socket_SendControl_HandlerManager::current_timestamp_;
        }

        //交换active/standby指针(使用std::swap函数)
        if (!send_thread->temp_handlers_standby->empty())
        {
            send_thread->mutex.lock();
            send_thread->handlers_list_size += send_thread->temp_handlers_size;
            send_thread->temp_handlers_size  = 0;
            std::swap(send_thread->temp_handlers_standby, send_thread->temp_handlers_active);
            send_thread->mutex.unlock();

            send_thread->handlers_list.insert(send_thread->handlers_list.end(), 
                send_thread->temp_handlers_active->begin(), 
                send_thread->temp_handlers_active->end());
            send_thread->temp_handlers_active->clear();
        }

        iter = send_thread->handlers_list.begin();
        iter_end = send_thread->handlers_list.end();
        while (iter != iter_end)
        {
            handler     = (*iter).first;
            sendcontrol = (*iter).second;

            switch (handler->current_status_)
            {
                case SL_Socket_Handler::STATUS_OPEN:
                    {
                        ++iter;

                        //发送数据
                        if ( (0 == send_delaytime_ms) || (current_timestamp - sendcontrol->last_fail_timestamp_ > send_delaytime_ms) )
                        {
                            if (sendcontrol->write_data(iovec, iovec_count, current_timestamp) == SL_Socket_SendControl_Interface::WRITE_RETURN_SEND_SUCCESS)
                            {
                                ++write_success_times;
                            }
                        }

                        //防止死连接
                        if ( (keepalive_time_ms > 0) && (current_timestamp - sendcontrol->last_update_timestamp_ > keepalive_time_ms) )
                        {
                            handler->last_errno_  = SL_Socket_Handler::ERROR_KEEPALIVE_TIMEOUT;
                            handler->next_status_ = SL_Socket_Handler::STATUS_CLOSE;
                            handler->get_socket_runner()->del_handle(handler);
                            handler->current_status_ = SL_Socket_Handler::STATUS_CLOSE;
                        }
                    }
                    break;
                case SL_Socket_Handler::STATUS_CLOSE_WAIT:
                    {
                        ++iter;

                        //发送数据(因为应用主动关闭连接,但可能有数据需要发送)
                        if (sendcontrol->write_data(iovec, iovec_count, current_timestamp) == SL_Socket_SendControl_Interface::WRITE_RETURN_SEND_SUCCESS)
                        {
                            ++write_success_times;
                        }
                        
                        //延迟关闭连接
                        if ( (0 == close_delaytime_ms) || (current_timestamp - sendcontrol->last_update_timestamp_ > close_delaytime_ms) )
                        {
                            handler->next_status_ = SL_Socket_Handler::STATUS_CLOSE;
                            handler->get_socket_runner()->del_handle(handler);
                            handler->current_status_ = SL_Socket_Handler::STATUS_CLOSE;
                        }
                    }
                    break;
                case SL_Socket_Handler::STATUS_CLOSE_SEND:
                    {
                        ++iter;

                        handler->next_status_ = SL_Socket_Handler::STATUS_CLOSE;
                        handler->get_socket_runner()->del_handle(handler);
                        handler->current_status_ = SL_Socket_Handler::STATUS_CLOSE;
                    }
                    break;
                case SL_Socket_Handler::STATUS_CLOSE:
                    {
                        //关闭连接
                        handler->close();

                        //从集合中删除handler
                        sendcontrol->clear_control();
                        --send_thread->handlers_list_size;
                        iter = send_thread->handlers_list.erase(iter);
                        iter_end = send_thread->handlers_list.end();

                        //回收连接对象
                        handler->get_socket_source()->free_handler(handler);
                    }
                    break;
                default:
                    ++iter;
                    break;
            }
        }

        if (0 == write_success_times)
        {
            send_thread->timedwait_flag.store(1);
#ifdef SOCKETLITE_OS_WINDOWS
            send_thread->timedwait_condition.timed_wait(NULL, scan_waittime_us);
#else
            send_thread->timedwait_mutex.lock();
            send_thread->timedwait_condition.timed_wait(&send_thread->timedwait_mutex, scan_waittime_us);
            send_thread->timedwait_mutex.unlock();
#endif
            send_thread->timedwait_flag.store(0);
        }
    }

    delete []iovec;
    send_thread->thread.exit();
    return 0;
}

int SL_Socket_SendControl_HandlerManager::wakeup_thread(int thread_index)
{
    if (timedwait_signal_)
    {
        if ( (thread_index < 0) || (thread_index >= (int)send_threads_.size()) )
        {
            return -1;
        }

        SendThread *send_thread = send_threads_[thread_index];
        if (send_thread->timedwait_flag.load() > 0)
        {
            send_thread->timedwait_flag.store(0);
            send_thread->timedwait_condition.signal();
            return 1;
        }
    }
    return 0;
}

