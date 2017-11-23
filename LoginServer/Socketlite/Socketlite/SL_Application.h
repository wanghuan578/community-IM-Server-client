#ifndef SOCKETLITE_APPLICATION_H
#define SOCKETLITE_APPLICATION_H
#include "SL_DataType.h"
#include "SL_Sync_SpinMutex.h"

template <typename T>
class SL_Application
{
public:

    //使用Singleton单件模式Lazy Evaluation隋性实例化
    static T* instance()
    {
        //使用双检查技巧 - double check
        if (NULL == app_)
        {
            mutex_.lock();
            if (NULL == app_)
            {
                app_ = new T();
            }
            mutex_.unlock();
        }
        return app_;
    }
    
    static void uninstance()
    {
        mutex_.lock();
        if (NULL != app_)
        {
            delete app_;
            app_ = NULL;
        }
        mutex_.unlock();
    }

    uint64 get_current_timestamp_us()
    {
        return current_timestamp_us_;
    }

    uint64 get_current_time_us()
    {
        return current_time_us_;
    }

    void set_current_timestamp_us(uint64 current_timestamp_us)
    {
        current_timestamp_us_ = current_timestamp_us;
    }

    void set_current_time_us(uint64 current_time_us)
    {
        current_time_us_ = current_time_us;
    }

    //返回毫秒为单位的时间戳
    uint64 get_current_timestamp()
    {
        return current_timestamp_us_ / 1000;
    }

    //返回秒为单位的当前时间
    uint64 get_current_time()
    {
        return current_time_us_ / 1000000LL;
    }

protected:
    SL_Application()
        : current_timestamp_us_(0)
        , current_time_us_(0)
    {
    }

    virtual ~SL_Application()
    {
    }
    
    //子类重载init函数
    virtual int init()
    {
        return 0;
    }

    //子类重载fini函数
    virtual int fini()
    {
        return 0;
    }

protected:
    //系统当前时间戳,不受系统时间被用户改变的影响(用于减少当前时间戳系统调用) 单位: us 只能用于计时
    uint64                      current_timestamp_us_;

    //系统当前时间(用于减少取当前时间系统调用) 单位: us
    uint64                      current_time_us_;

    static SL_Sync_SpinMutex    mutex_;
    static T                    *app_;
};

template<typename T>
SL_Sync_SpinMutex SL_Application<T>::mutex_;

template<typename T>
T* SL_Application<T>::app_ = NULL;

#endif

