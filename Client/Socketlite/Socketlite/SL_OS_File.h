#ifndef SOCKETLITE_OS_FILE_H
#define SOCKETLITE_OS_FILE_H

#include "SL_Config.h"
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#ifdef SOCKETLITE_OS_WINDOWS
#include <io.h>
#endif

class SL_OS_File
{
public:
    SL_OS_File();
    ~SL_OS_File();

    int open(const SL_TCHAR *file_name, int flags, int mode);
    int creat(const SL_TCHAR *file_name, int mode);
    int close();
    int read(void *buf, unsigned int nbytes);
    int readn(void *buf, unsigned int nbytes);
    int write(const void *buf, unsigned int nbytes);
    int writen(const void *buf, unsigned int nbytes);
    int lseek(long offset, int whence);
    int flush();
    int dup();
    int dup2(int fd);

    int  get_fileno() const;
    void set_fileno(int fileno);
    const SL_TCHAR* get_file_name() const;

protected:
    int fd_;
    SL_TSTRING file_name_;    
};

#endif

