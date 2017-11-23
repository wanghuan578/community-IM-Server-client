#ifndef SOCKETLITE_SYNC_GURARD_H
#define SOCKETLITE_SYNC_GURARD_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif

template <typename TSyncMutex>
class SL_Sync_Guard
{
public:
    // Constructor acquires the lock.
    inline explicit SL_Sync_Guard(TSyncMutex &mutex)
        : mutex_(mutex)
    {
        mutex_.lock();
        locked_ = true;
    }

    // Destructor releases the lock.
    inline ~SL_Sync_Guard()
    {
        if (locked_)
        {
            mutex_.unlock();
        }
    }

    inline void lock() 
    {
        if (!locked_)
        {
            mutex_.lock();
            locked_ = true;
        }
    }

    inline void unlock() 
    {
        if (locked_)
        {
            locked_ = false;
            mutex_.unlock();
        }
    }

    // Test whether the lock is held.
    inline bool locked() const
    {
        return locked_;
    }

    // Get the underlying mutex.
    inline TSyncMutex& mutex()
    {
        return mutex_;
    }

private:
    //= Prevent assignment and initialization.
    // Disallow copying and assignment.
    SL_Sync_Guard();
    SL_Sync_Guard(const SL_Sync_Guard &);
    SL_Sync_Guard& operator= (const SL_Sync_Guard &);

private:
    TSyncMutex  &mutex_;
    bool        locked_;
};

#endif

