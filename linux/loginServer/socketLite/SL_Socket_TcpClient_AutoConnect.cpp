#include "SL_Socket_CommonAPI.h"
#include "SL_Socket_TcpClient_AutoConnect.h"

SL_Socket_TcpClient_AutoConnect::SL_Socket_TcpClient_AutoConnect()
{
}

SL_Socket_TcpClient_AutoConnect::~SL_Socket_TcpClient_AutoConnect()
{
    close();
    clear();
}

int SL_Socket_TcpClient_AutoConnect::open(uint check_interval_us)
{
    if (check_interval_us > 0)
    {
        check_interval_us_ = check_interval_us;
    }
    else
    {
        check_interval_us_ = 500000;
    }
    return autoconnect_thread_.start(SL_Socket_TcpClient_AutoConnect::autoconnect_proc, this);
}

int SL_Socket_TcpClient_AutoConnect::close()
{
    autoconnect_thread_.stop();
    autoconnect_thread_.join();
    return 0;
}

int SL_Socket_TcpClient_AutoConnect::add_tcpclient(SL_Socket_Source *tcpclient)
{
    mutex_.lock();
    tcpclient_set_.insert(tcpclient);
    mutex_.unlock();
    return 0;
}

int SL_Socket_TcpClient_AutoConnect::del_tcpclient(SL_Socket_Source *tcpclient)
{
    mutex_.lock();
    tcpclient_set_.erase(tcpclient);
    mutex_.unlock();
    return 0;
}

int SL_Socket_TcpClient_AutoConnect::clear()
{
    mutex_.lock();
    tcpclient_set_.clear();
    mutex_.unlock();
    return 0;
}

#ifdef SOCKETLITE_OS_WINDOWS
unsigned int WINAPI SL_Socket_TcpClient_AutoConnect::autoconnect_proc(void *arg)
#else
void* SL_Socket_TcpClient_AutoConnect::autoconnect_proc(void *arg)
#endif
{
    SL_Socket_TcpClient_AutoConnect *autoconnect = (SL_Socket_TcpClient_AutoConnect*)arg;
    SL_Socket_Source *tcpclient;
    std::set<SL_Socket_Source* >::iterator iter;
    std::set<SL_Socket_Source* >::iterator iter_end;

    while (1)
    {
        if (!autoconnect->autoconnect_thread_.get_running())
        {
            autoconnect->autoconnect_thread_.exit();
            return 0;
        }
        
        autoconnect->mutex_.lock();
        iter = autoconnect->tcpclient_set_.begin();
        iter_end = autoconnect->tcpclient_set_.end();
        for (; iter != iter_end; ++iter)
        {
            if (!autoconnect->autoconnect_thread_.get_running())
            {
                autoconnect->mutex_.unlock();
                return 0;
            }
            
            tcpclient = *iter;
            if (!tcpclient->get_autoconnect())
            {
                continue;
            }
                        
            if (!tcpclient->get_connected())
            {
                tcpclient->connect();
            }
        }
        autoconnect->mutex_.unlock();
        
        SL_Socket_CommonAPI::util_sleep_us(autoconnect->check_interval_us_);
    }    

    autoconnect->autoconnect_thread_.exit();
    return 0;
}

