#ifndef SL_SYNC_ATOMIC_H
#define SL_SYNC_ATOMIC_H
#include "SL_Config.h"

#ifdef SOCKETLIET_CC_MSVC

class SL_Sync_Atomic_Int32
{
public:
    inline SL_Sync_Atomic_Int32()
        : value_(0)
    {
    }

    inline SL_Sync_Atomic_Int32(long init_value)
        : value_(init_value)
    {
    }

    ~SL_Sync_Atomic_Int32()
    {
    }

    static bool compare_and_swap(long *value, long comparand, long new_value)
    {
        long ret = ::InterlockedCompareExchange(value, new_value, comparand);
        if (ret == comparand)
        {
            return true;
        }
        return false;
    }

    static bool compare_exchange(long *value, long comparand, long new_value)
    {
        long ret = ::InterlockedCompareExchange(value, new_value, comparand);
        if (ret == comparand)
        {
            return true;
        }
        return false;
    }

    inline long fetch_add(long add_value)
    {
        return ::InterlockedExchangeAdd(&value_, add_value);
    }

    inline long fetch_sub(long sub_value)
    {
        return ::InterlockedExchangeAdd(&value_, -sub_value);
    }

    inline long get() const
    {
        return value_;
    }
  
    inline long load()
    {
        //方法1
        return static_cast<long const volatile &>(value_);

        //方法2
        //return ::InterlockedExchangeAdd(&value_, 0);
    }

    inline void store(long new_value)
    {
        ::InterlockedExchange(&value_, new_value);
    }

    inline long exchange(long new_value)
    {
        return ::InterlockedExchange(&value_, new_value);
    }
    
    inline bool compare_exchange(long comparand, long new_value)
    {
        long ret = ::InterlockedCompareExchange(&value_, new_value, comparand);
        if (ret == comparand)
        {
            return true;
        }
        return false;
    }

    inline bool compare(long comparand)
    {
        return compare_exchange(comparand, value_);
    }

    inline long operator+=(long add_value)
    {
        return ::InterlockedExchangeAdd(&value_, add_value) + add_value;
    }

    inline long operator-=(long sub_value)
    {
        return ::InterlockedExchangeAdd(&value_, -sub_value) - sub_value;
    }

    inline long operator++()
    {// preincrement 前置递增
        return ::InterlockedIncrement(&value_);
    }

    inline long operator--() 
    {// preincrement 前置递减
        return ::InterlockedDecrement(&value_);
    }

    inline long operator++(int)
    {// postincrement 后置递增
        return ::InterlockedExchangeAdd(&value_, 1);
    }

    inline long operator--(int)
    {// postincrement 后置递减
        return ::InterlockedExchangeAdd(&value_, -1);
    }

private:
    SL_Sync_Atomic_Int32(const SL_Sync_Atomic_Int32 &);
    SL_Sync_Atomic_Int32& operator=(const SL_Sync_Atomic_Int32 &);

private:
    long value_;
};

#if (_WIN32_WINNT < 0x0600) //Windows Vista 以下版本
    #include "SL_Sync_Mutex.h"
    #include "SL_Sync_SpinMutex.h"
    class SL_Sync_Atomic_Int64
    {
    public:
        inline SL_Sync_Atomic_Int64()
            : value_(0)
        {
        }

        inline SL_Sync_Atomic_Int64(int64 init_value)
            : value_(init_value)
        {
        }

        ~SL_Sync_Atomic_Int64()
        {
        }

        static bool compare_and_swap(int64 *value, int64 comparand, int64 new_value)
        {
            if (*value == comparand)
            {
                *value = new_value;
                return true;
            }
            return false;
        }

        static bool compare_exchange(int64 *value, int64 comparand, int64 new_value)
        {
            if (*value == comparand)
            {
                *value = new_value;
                return true;
            }
            return false;
        }

        inline int64 fetch_add(int64 add_value)
        {  
            mutex_.lock();
            int64 ret = value_;
            value_ += add_value;
            mutex_.unlock();
            return ret;
        }

        inline int64 fetch_sub(int64 sub_value)
        {
            mutex_.lock();
            int64 ret = value_;
            value_ -= sub_value;
            mutex_.unlock();
            return ret;
        }

        inline int64 get() const 
        {
            return value_;
        }

        inline int64 load()
        {
            //方法1
            return static_cast<int64 const volatile &>(value_);
        }

        inline void store(int64 new_value)
        {
            mutex_.lock();
            value_ = new_value;
            mutex_.unlock();
        }

        inline int64 exchange(int64 new_value)
        {
            mutex_.lock();
            int64 ret = value_;
            value_ = new_value;
            mutex_.unlock();
            return ret;
        }

        inline bool compare_exchange(int64 comparand, int64 new_value)
        {
            bool ret = false;
            mutex_.lock();
            if (value_ == comparand)
            {
                value_ = new_value;
                ret = true;
            }
            mutex_.unlock();
            return ret;
        }

        inline bool compare(int64 comparand)
        {
            return compare_exchange(comparand, value_);
        }

        inline int64 operator+=(int64 add_value)
        {
            mutex_.lock();
            value_ += add_value;
            int64 ret = value_;
            mutex_.unlock();
            return ret;
        }

        inline int64 operator-=(int64 sub_value)
        {
            mutex_.lock();
            value_ -= sub_value;
            int64 ret = value_;
            mutex_.unlock();
            return ret;
        }

        inline int64 operator++()
        {// preincrement 前置递增
            mutex_.lock();
            int64 ret = ++value_;
            mutex_.unlock();
            return ret;
        }

        inline int64 operator--() 
        {// preincrement 前置递减
            mutex_.lock();
            int64 ret = --value_;
            mutex_.unlock();
            return ret;
        }

        inline int64 operator++(int)
        {// postincrement 后置递增
            mutex_.lock();
            int64 ret = value_++;
            mutex_.unlock();
            return ret;
        }

        inline int64 operator--(int)
        {// postincrement 后置递减
            mutex_.lock();
            int64 ret = value_--;
            mutex_.unlock();
            return ret;
        }

    private:
        SL_Sync_Atomic_Int64(const SL_Sync_Atomic_Int64 &);
        SL_Sync_Atomic_Int64& operator=(const SL_Sync_Atomic_Int64 &);

    private:
        int64 value_;
        SL_Sync_SpinMutex mutex_;
    };
#else   //Windows Vista 以上版本
    #include <intrin.h>
    #pragma intrinsic(_InterlockedCompareExchange64)
    #ifdef SOCKETLITE_OS_WIN64
        #pragma intrinsic(_InterlockedExchangeAdd64)
        #pragma intrinsic(_InterlockedIncrement64)
        #pragma intrinsic(_InterlockedDecrement64)
    #endif

    class SL_Sync_Atomic_Int64
    {
    public:
        inline SL_Sync_Atomic_Int64()
            : value_(0)
        {
        }

        inline SL_Sync_Atomic_Int64(int64 init_value)
            : value_(init_value)
        {
        }

        ~SL_Sync_Atomic_Int64()
        {
        }

        static bool compare_and_swap(int64 *value, int64 comparand, int64 new_value)
        {
            int64 ret = _InterlockedCompareExchange64(value, new_value, comparand);
            if (ret == comparand)
            {
                return true;
            }
            return false;
        }

        static bool compare_exchange(int64 *value, int64 comparand, int64 new_value)
        {
            int64 ret = _InterlockedCompareExchange64(value, new_value, comparand);
            if (ret == comparand)
            {
                return true;
            }
            return false;
        }

        inline int64 fetch_add(int64 add_value)
        {
            #ifdef SOCKETLITE_OS_WIN64
                return _InterlockedExchangeAdd64(&value_, add_value);
            #else
                return _InterlockedCompareExchange64(&value_, value_ + add_value, value_);
            #endif
        }

        inline int64 fetch_sub(int64 sub_value)
        {
            #ifdef SOCKETLITE_OS_WIN64
                return _InterlockedExchangeAdd64(&value_, -sub_value);
            #else
                return _InterlockedCompareExchange64(&value_, value_ - sub_value, value_);
            #endif
        }

        inline int64 get()
        {
            return value_;
        }

        inline int64 load()
        {
            //方法1
            return static_cast<int64 const volatile &>(value_);

            //方法2
            //#ifdef SOCKETLITE_OS_WIN64
            //    return _InterlockedExchangeAdd64(&value_, 0);
            //#else
            //    return _InterlockedCompareExchange64(&value_, value_, value_);
            //#endif
        }

        inline void store(int64 new_value)
        {
            #ifdef SOCKETLITE_OS_WIN64
                _InterlockedExchange64(&value_, new_value);
            #else
                _InterlockedCompareExchange64(&value_, new_value, value_);
            #endif            
        }

        inline int64 exchange(int64 new_value)
        {
            return _InterlockedCompareExchange64(&value_, new_value, value_);
        }

        inline bool compare_exchange(int64 comparand, int64 new_value)
        {
            int64 ret = _InterlockedCompareExchange64(&value_, new_value, comparand);
            if (ret == comparand)
            {
                return true;
            }
            return false;
        }

        inline bool compare(int64 comparand)
        {
            return compare_exchange(comparand, value_);
        }

        inline int64 operator+=(int64 add_value)
        {
            #ifdef SOCKETLITE_OS_WIN64
                return _InterlockedExchangeAdd64(&value_, add_value) + add_value;
            #else
                return _InterlockedCompareExchange64(&value_, value_ + add_value, value_) + add_value;
            #endif
        }

        inline int64 operator-=(int64 sub_value)
        {
            #ifdef SOCKETLITE_OS_WIN64
                return _InterlockedExchangeAdd64(&value_, -sub_value) - sub_value;
            #else
                return _InterlockedCompareExchange64(&value_, value_- sub_value, value_) - sub_value;
            #endif
        }

        inline int64 operator++()
        {// preincrement 前置递增
            #ifdef SOCKETLITE_OS_WIN64
                return _InterlockedIncrement64(&value_);
            #else
                return _InterlockedCompareExchange64(&value_, value_ + 1, value_) + 1;
            #endif
        }

        inline int64 operator--() 
        {// preincrement 前置递减
            #ifdef SOCKETLITE_OS_WIN64
                return _InterlockedDecrement64(&value_);
            #else
                return _InterlockedCompareExchange64(&value_, value_ - 1, value_) - 1;
            #endif
        }

        inline int64 operator++(int)
        {// postincrement 后置递增
            #ifdef SOCKETLITE_OS_WIN64
                return _InterlockedExchangeAdd64(&value_, 1);
            #else
                return _InterlockedCompareExchange64(&value_, value_ + 1, value_);
            #endif
        }

        inline int64 operator--(int)
        {// postincrement 后置递减
            #ifdef SOCKETLITE_OS_WIN64
                return _InterlockedExchangeAdd64(&value_, -1);
            #else
                return _InterlockedCompareExchange64(&value_, value_ - 1, value_);
            #endif
        }

    private:
        SL_Sync_Atomic_Int64(const SL_Sync_Atomic_Int64 &);
        SL_Sync_Atomic_Int64& operator=(const SL_Sync_Atomic_Int64 &);

    private:
        int64 value_;
    };
#endif

#else   //SOCKETLITE_CC_GCC

class SL_Sync_Atomic_Int32
{
public:
    inline SL_Sync_Atomic_Int32()
        : value_(0)
    {
    }

    inline SL_Sync_Atomic_Int32(int32 init_value)
        : value_(init_value)
    {
    }

    inline ~SL_Sync_Atomic_Int32()
    {
    }

    static bool compare_and_swap(int32 *old_value, int32 comparand, int32 new_value)
    {
        return __sync_bool_compare_and_swap(old_value, comparand, new_value);
    }

    static bool compare_exchange(int32 *old_value, int32 comparand, int32 new_value)
    {
        return __sync_bool_compare_and_swap(old_value, comparand, new_value);
    }

    inline int32 fetch_add(int32 add_value)
    {
        return __sync_fetch_and_add(&value_, add_value);
    }

    inline int32 fetch_sub(int32 sub_value)
    {
        return __sync_fetch_and_sub(&value_, sub_value);
    }

    inline int32 get()
    {
        return value_;
    }

    inline int32 load()
    {
        return __sync_fetch_and_add(&value_, 0);
    }

    inline void store(int32 new_value)
    {
        __sync_val_compare_and_swap(&value_, value_, new_value);
    }

    inline int32 exchange(int32 new_value)
    {
       return __sync_val_compare_and_swap(&value_, value_, new_value);
    }

    inline bool compare_exchange(int32 comparand, int32 new_value)
    {
        return __sync_bool_compare_and_swap(&value_, comparand, new_value);
    }

    inline bool compare(int32 comparand)
    {
        return compare_exchange(comparand, value_);
    }

    inline int32 operator+=(int32 add_value) 
    {
        return __sync_add_and_fetch(&value_, add_value);
    }

    inline int32 operator-=(int32 sub_value) 
    {
        return __sync_sub_and_fetch(&value_, sub_value);
    }

    inline int32 operator++()
    {
        return __sync_add_and_fetch(&value_, 1);
    }

    inline int32 operator--() 
    {
        return __sync_sub_and_fetch(&value_, 1);
    }

    inline int32 operator++(int)
    {
        return __sync_fetch_and_add(&value_, 1);
    }

    inline int32 operator--(int)
    {
        return __sync_fetch_and_sub(&value_, 1);
    }

private:
    SL_Sync_Atomic_Int32(const SL_Sync_Atomic_Int32 &);
    SL_Sync_Atomic_Int32& operator=(const SL_Sync_Atomic_Int32 &);

private:
    int32 value_;
};

class SL_Sync_Atomic_Int64
{
public:
    inline SL_Sync_Atomic_Int64()
        : value_(0)
    {
    }

    inline SL_Sync_Atomic_Int64(int64 init_value)
        : value_(init_value)
    {
    }

    inline ~SL_Sync_Atomic_Int64()
    {
    }

    static bool compare_and_swap(int64 *old_value, int64 comparand, int64 new_value)
    {
        return __sync_bool_compare_and_swap(old_value, comparand, new_value);
    }

    static bool compare_exchange(int64 *old_value, int64 comparand, int64 new_value)
    {
        return __sync_bool_compare_and_swap(old_value, comparand, new_value);
    }

    inline int64 fetch_add(int64 add_value)
    {
        return __sync_fetch_and_add(&value_, add_value);
    }

    inline int64 fetch_sub(int64 sub_value)
    {
        return __sync_fetch_and_sub(&value_, sub_value);
    }

    inline int64 get()
    {
        return value_;
    }

    inline int64 load()
    {
        return __sync_fetch_and_add(&value_, 0);
    }

    inline void store(int64 new_value)
    {
        __sync_val_compare_and_swap(&value_, value_, new_value);
    }

    inline int exchange(int64 new_value)
    {
        return __sync_val_compare_and_swap(&value_, value_, new_value);
    }

    inline bool compare_exchange(int64 comparand, int64 new_value)
    {
        return __sync_bool_compare_and_swap(&value_, comparand, new_value);
    }

    inline bool compare(int64 comparand)
    {
        return compare_exchange(comparand, value_);
    }

    inline int64 operator+=(int64 add_value) 
    {
        return __sync_add_and_fetch(&value_, add_value);
    }

    inline int64 operator-=(int64 sub_value) 
    {
        return __sync_sub_and_fetch(&value_, sub_value);
    }

    inline int64 operator++()
    {
        return __sync_add_and_fetch(&value_, 1);
    }

    inline int64 operator--() 
    {
        return __sync_sub_and_fetch(&value_, 1);
    }

    inline int64 operator++(int)
    {
        return __sync_fetch_and_add(&value_, 1);
    }

    inline int64 operator--(int)
    {
        return __sync_fetch_and_sub(&value_, 1);
    }

private:
    SL_Sync_Atomic_Int64(const SL_Sync_Atomic_Int64 &);
    SL_Sync_Atomic_Int64& operator=(const SL_Sync_Atomic_Int64 &);

private:
    int64 value_;
};

#endif  //SOCKETLIET_CC_MSVC

#endif  //SL_SYNC_ATOMIC_H

