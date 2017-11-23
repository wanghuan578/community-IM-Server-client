#ifndef SOCKETLITE_TIMER_QUEUE_H
#define SOCKETLITE_TIMER_QUEUE_H

#define SL_TIMER_DEFULT_INTERVAL    1
#define SL_TIMER_DEFAUL_REPAIR      10

class SL_Timer;
class SL_Timer_Queue
{
public:
    SL_Timer_Queue() 
    {
    }

    virtual ~SL_Timer_Queue() 
    {
    }

    virtual int add_timer(SL_Timer *timer) 
    { 
        return 0; 
    }

    virtual int del_timer(SL_Timer *timer) 
    { 
        return 0; 
    }

    virtual int clear_timer() 
    { 
        return 0; 
    }

    virtual int timer_loop() 
    { 
        return 0;
    }

    virtual int thread_timer_loop() 
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

