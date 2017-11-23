#ifndef SOCKETLITE_SOCKET_SEND_CONTROL_HANDLER_MANAGER_H
#define SOCKETLITE_SOCKET_SEND_CONTROL_HANDLER_MANAGER_H

#include <list>
#include <vector>
#include "SL_Sync_Atomic.h"
#include "SL_Sync_SpinMutex.h"
#include "SL_Sync_Mutex.h"
#include "SL_Sync_Condition.h"
#include "SL_Thread.h"
#include "SL_Socket_Handler.h"

//问题:如何保存模板类SL_Socket_SendControl_Handler<>的指针?
//解决方法1:
//      集合中存放SL_Socket_Handler类的指针,
//      通过dynamic_cast<SL_Socket_SendControl_Interface*>(handler), 
//      但dynamic_cast会显著降低程序的效率，在各种c++的编译器上都是如此,一般来说,
//      使用到dynamic_cast的部分效率降低15%以上是很正常的。所以如果你的程序能够不用dynamic_cast,那就不要用.
//      如:std::list<SL_Socket_Handler*>   sendcontrol_list_;
//解决方法2:
//      集合中存放<SL_Socket_Handler*, SL_Socket_SendControl_Interface*>的结构体
//      但内存占用要大些(sizeof(void*)

class SL_Socket_SendControl_Interface;
class SL_Socket_SendControl_HandlerManager
{
public:
    //系统当前时间戳,不受系统时间被用户改变的影响(用于减少当前时间戳系统调用) 单位: us 只能用于计时
    static uint64 current_timestamp_us_;

    //系统当前时间(用于减少取当前时间系统调用) 单位: us
    static uint64 current_time_us_;

    //系统当前时间戳,不受系统时间被用户改变的影响(用于减少当前时间戳系统调用) 单位: ms 只能用于计时
    static uint64 current_timestamp_;

    //系统当前时间(用于减少当前时间系统调用) 单位: s
    static uint64 current_time_;

public:
    SL_Socket_SendControl_HandlerManager();
    virtual ~SL_Socket_SendControl_HandlerManager();

    int     open(int  thread_number=1, 
                 int  send_block_size=65536, 
                 int  iovec_count=1024, 
                 int  keepalive_time_ms=0, 
                 int  send_delaytime_ms=1, 
                 int  scan_waittime_us=1000, 
                 int  close_delaytime_ms=60000, 
                 bool timedwait_signal=true,
                 bool direct_send_flag=true);

    int     close();
    int     event_loop(int timeout_ms=10);

    int     add_handler(SL_Socket_Handler *handler, SL_Socket_SendControl_Interface *sendcontrol);
    int     wakeup_thread(int thread_index);
    size_t  handler_size();

    static uint64 get_current_timestamp()
    {
        return current_timestamp_;
    }

    static uint64 get_current_time()
    {
        return current_time_;
    }

    static uint64 get_current_timestamp_us()
    {
        return current_timestamp_us_;
    }

    static uint64 get_current_time_us()
    {
        return current_time_us_;
    }

protected:
    virtual int assign_thread();

private:
#ifdef SOCKETLITE_OS_WINDOWS
    static unsigned int WINAPI send_proc(void *arg);
#else
    static void* send_proc(void *arg);
#endif

private:
    SL_IOVEC    *iovec_;
    int         iovec_count_;           //系统调用writev函数iovec数组的个数
    int         thread_number_;         //线程数(若<=0,表示不启动专门发送线程)
    int         send_block_size_;       //发送数据块的大小(每遍历一次发送一块)
    uint        keepalive_time_ms_;     //keepalive间隔
    uint        send_delaytime_ms_;     //发送延迟时间(ms)
    uint        scan_waittime_us_;      //扫描一遍后,wait时间(us)
    uint        close_delaytime_ms_;    //关闭延迟时间(ms), 因立即关闭socket, 客户端可能收不到最后发送的部分数据

    typedef std::pair<SL_Socket_Handler*, SL_Socket_SendControl_Interface* > ITEM_SENDCONTROL;
    struct SendThread
    {
        void   *parent;
        uint    index;                  //线程在线程集合中索引
        uint    handlers_list_size;     //handler集合的大小
        uint    temp_handlers_size;     //临时集合的大小

        std::list<ITEM_SENDCONTROL>     handlers_list;
        SL_Thread                       thread;
        SL_Sync_SpinMutex               mutex;
        std::list<ITEM_SENDCONTROL>     temp1_handlers;
        std::list<ITEM_SENDCONTROL>     temp2_handlers;
        std::list<ITEM_SENDCONTROL>     *temp_handlers_active;
        std::list<ITEM_SENDCONTROL>     *temp_handlers_standby;
        
        //睡眠唤醒控制
        SL_Sync_ThreadMutex             timedwait_mutex;
        SL_Sync_Condition               timedwait_condition;
        SL_Sync_Atomic_Int32            timedwait_flag;
    };
    std::vector<SendThread* > send_threads_;
    int  next_thread_index_;

    //用于控制是否触发条件信号(因触发条件信号比较耗时)
    bool timedwait_signal_;

    //直接发送标识(由逻辑线程直接发送数据,不在发送线程,省去数据拷贝和线程切换)
    bool direct_send_flag_;

    template <typename TSocketHandler, typename TByteBuffer, typename TSyncMutex, typename TEncryptCipher, typename TDecryptCipher> friend class SL_Socket_SendControl_Handler;
};

#endif

