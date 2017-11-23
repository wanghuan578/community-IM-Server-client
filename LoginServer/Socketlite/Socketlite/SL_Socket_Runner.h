#ifndef SOCKETLITE_SOCKET_RUNNER_H
#define SOCKETLITE_SOCKET_RUNNER_H
#include "SL_Config.h"
#include "SL_Socket_Handler.h"

class SOCKETLITE_API SL_Socket_Runner
{
public:
    SL_Socket_Runner() 
    {
    }

    virtual ~SL_Socket_Runner() 
    {
    }
    
    virtual int set_buffer_size(int recv_buffer_size, int send_buffer_size) 
    { 
        return 0; 
    }

    virtual int get_recv_buffer_size() const
    { 
        return 0; 
    }

    virtual char* get_recv_buffer() 
    { 
        return NULL; 
    }

    //handler_close_status: 检测到handler导常时,将SL_Socket_Handler::next_status_置为handler_close_status
    virtual int open(int event_mask, uint max_size, uint max_timeout_ms, uint thread_number, int8 handler_close_status=SL_Socket_Handler::STATUS_CLOSE) 
    { 
        return 0; 
    }

    virtual int close() 
    { 
        return 0; 
    }

    virtual int add_handle(SL_Socket_Handler *socket_handler, int event_mask) 
    { 
        return 0; 
    }

    virtual int del_handle(SL_Socket_Handler *socket_handler) 
    { 
        return 0; 
    }

    virtual int remove_handle(SL_Socket_Handler *socket_handler) 
    { 
        return 0; 
    }

    virtual int set_event_mask(SL_Socket_Handler *socket_handler, int event_mask) 
    { 
        return 0; 
    } 

    virtual size_t get_handler_size() const 
    { 
        return 0; 
    }

    virtual int event_dispatch() 
    { 
        return 0; 
    }

    virtual int event_loop(int timeout_ms)
    { 
        return 0; 
    }

    virtual int thread_event_loop(int timeout_ms) 
    { 
        return 0; 
    }

    virtual int thread_wait() 
    { 
        return 0; 
    }

    virtual int thread_stop() 
    { 
        return 0; 
    }
};

#endif

