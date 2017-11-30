#ifndef SOCKETLITE_SYNC_SEMAPHORE_H
#define SOCKETLITE_SYNC_SEMAPHORE_H
#include "SL_Config.h"

#ifdef SOCKETLITE_OS_WINDOWS
typedef HANDLE  SL_SYNC_SEMAPHORE_T;
class SL_Sync_Semaphore
{
public:
    inline SL_Sync_Semaphore()
        : semaphore_(NULL)
    {
    }

    inline SL_Sync_Semaphore(int max_count)
        : semaphore_(NULL)
    {
        open(max_count);
    }

    inline ~SL_Sync_Semaphore()
    {
        close();
    }

    inline int open(int max_count=1)
    {
        close();
        semaphore_ = CreateSemaphore(NULL, 0, max_count, NULL);
        if (semaphore_ != NULL)
        {
            return 1;
        }
        return -1;
    }

    inline int close()
    {
        if (semaphore_ != NULL)
        {
            CloseHandle(semaphore_);
            semaphore_ = NULL;
        }
        return 1;
    }

    inline int acquire()
    {
        return WaitForSingleObject(semaphore_, INFINITE);
    }

    inline int acquire(ulong timeout)
    {
        return WaitForSingleObject(semaphore_, timeout);
    }

    inline int release()
    {
        long previous_count;
        return ReleaseSemaphore(semaphore_, 1, &previous_count);
    }

    inline SL_SYNC_SEMAPHORE_T* handle()
    {
        return &semaphore_;
    }

private:
    SL_Sync_Semaphore(const SL_Sync_Semaphore &);
    SL_Sync_Semaphore& operator=(const SL_Sync_Semaphore &);

private:
    SL_SYNC_SEMAPHORE_T semaphore_;
};

#else
#include <semaphore.h>
typedef sem_t   SL_SYNC_SEMAPHORE_T;
class SL_Sync_Semaphore
{
public:
    inline SL_Sync_Semaphore()
    {
    }

    inline SL_Sync_Semaphore(int max_count)
    {
        open(max_count);
    }

    inline ~SL_Sync_Semaphore()
    {
        close();
    }

    inline int open(int max_count=1)
    {
        sem_init(&semaphore_, 0, 0);
        return 1;
    }

    inline int close()
    {
        sem_destroy(&semaphore_);
        return 1;
    }

    inline int acquire()
    {
        return sem_wait(&semaphore_);
    }

    inline int acquire(ulong timeout)
    {
        return sem_wait(&semaphore_);
    }

    inline int release()
    {
        return sem_post(&semaphore_);
    }

    inline SL_SYNC_SEMAPHORE_T* handle()
    {
        return &semaphore_;
    }
    
private:
    SL_Sync_Semaphore(const SL_Sync_Semaphore &);
    SL_Sync_Semaphore& operator=(const SL_Sync_Semaphore &);

private:
    SL_SYNC_SEMAPHORE_T semaphore_;
};

#endif

#endif

