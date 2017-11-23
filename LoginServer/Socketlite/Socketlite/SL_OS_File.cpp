#include "SL_OS_File.h"
#ifdef SOCKETLITE_OS_WINDOWS

SL_OS_File::SL_OS_File()
    : fd_(-1)
{
}

SL_OS_File::~SL_OS_File()
{
    close();
}

int SL_OS_File::open(const SL_TCHAR *file_name, int flags, int mode)
{
    if (fd_ < 0)
    {
        fd_ = _topen(file_name, flags, mode);
        if (fd_ < 0)
        {
            return -1;
        }
        file_name_ = file_name;
    }
    return 0;
}

int SL_OS_File::creat(const SL_TCHAR *file_name, int mode)
{
    if (fd_ < 0)
    {
        fd_ = _tcreat(file_name, mode);
        if (fd_ < 0)
        {
            return -1;
        }
        file_name_ = file_name;
    }
    return 0;
}

int SL_OS_File::close()
{
    if (fd_ > SL_STDERR_FILENO)
    {
        _close(fd_);
        fd_ = -1;
    }
    return 0;
}

int SL_OS_File::read(void *buf, unsigned int nbytes)
{
    return _read(fd_, buf, nbytes);
}

int SL_OS_File::readn(void *buf, unsigned int nbytes)
{
    char *ptr = (char *)buf;
    unsigned int nleft = nbytes;
    int nread;

    while (nleft > 0)
    {
        nread = _read(fd_, ptr, nleft);
        if (nread < 0)
        {
            if (nleft == nbytes)
            {
                return -1;
            }
            else
            {
                break;
            }
        } 
        else if (nread == 0)
        {
            break;
        }
        nleft -= nread;
        ptr   += nread;
    }
    return (nbytes-nleft);
}

int SL_OS_File::write(const void *buf, unsigned int nbytes)
{
    return _write(fd_, buf, nbytes);
}

int SL_OS_File::writen(const void *buf, unsigned int nbytes)
{
    char *ptr = (char *)buf;
    unsigned int nleft = nbytes;
    int nwrite;

    while (nleft > 0)
    {
        nwrite = _write(fd_, ptr, nleft);
        if (nwrite < 0)
        {
            if (nleft == nbytes)
            {
                return -1;
            }
            else
            {
                break;
            }
        }
        else if (nwrite == 0)
        {
            break;
        }
        nleft -= nwrite;
        ptr   += nwrite;
    }
    return (nbytes-nleft);
}

int SL_OS_File::lseek(long offset, int whence)
{
    return _lseek(fd_, offset, whence);
}

int SL_OS_File::flush()
{
    return _commit(fd_);
}

int SL_OS_File::dup()
{
    int new_fd = _dup(fd_);
    if (new_fd < 0)
    {
        return -1;
    }
    fd_ = new_fd;
    return new_fd;
}

int SL_OS_File::dup2(int fd)
{
    int new_fd = _dup2(fd_, fd);
    if (new_fd < 0)
    {
        return -1;
    }
    fd_ = new_fd;
    return new_fd;
}

int SL_OS_File::get_fileno() const
{
    return fd_;
}

void SL_OS_File::set_fileno(int fileno) 
{
    close();
    fd_ = fileno;
}

const SL_TCHAR* SL_OS_File::get_file_name() const
{
    return file_name_.c_str();
}

#else

SL_OS_File::SL_OS_File()
    : fd_(-1)
{
}

SL_OS_File::~SL_OS_File()
{
    close();
}

int SL_OS_File::open(const SL_TCHAR *file_name, int flags, int mode)
{
    if (fd_ < 0)
    {
        fd_ = ::open(file_name, flags, mode);
        if (fd_ < 0)
        {
            return -1;
        }
        file_name_ = file_name;
    }
    return 0;
}

int SL_OS_File::creat(const SL_TCHAR *file_name, int mode)
{
    if (fd_ < 0)
    {
        fd_ = ::creat(file_name, mode);
        if (fd_ < 0)
        {
            return -1;
        }
        file_name_ = file_name;
    }
    return 0;
}

int SL_OS_File::close()
{
    if (fd_ > SL_STDERR_FILENO)
    {
        ::close(fd_);
        fd_ = -1;
    }
    return 0;
}

int SL_OS_File::read(void *buf, unsigned int nbytes)
{
    return ::read(fd_, buf, nbytes);
}

int SL_OS_File::readn(void *buf, unsigned int nbytes)
{
    char *ptr = (char *)buf;
    unsigned int nleft = nbytes;
    int nread;

    while (nleft > 0)
    {
        nread = ::read(fd_, ptr, nleft);
        if (nread < 0)
        {
            if (nleft == nbytes)
            {
                return -1;
            }
            else
            {
                break;
            }
        } 
        else if (nread == 0)
        {
            break;
        }
        nleft -= nread;
        ptr   += nread;
    }
    return (nbytes-nleft);
}

int SL_OS_File::write(const void *buf, unsigned int nbytes)
{
    return ::write(fd_, buf, nbytes);
}

int SL_OS_File::writen(const void *buf, unsigned int nbytes)
{
    char *ptr = (char *)buf;
    unsigned int nleft = nbytes;
    int nwrite;

    while (nleft > 0)
    {
        nwrite = ::write(fd_, ptr, nleft);
        if (nwrite < 0)
        {
            if (nleft == nbytes)
            {
                return -1;
            }
            else
            {
                break;
            }
        } 
        else if (nwrite == 0)
        {
            break;
        }
        nleft -= nwrite;
        ptr   += nwrite;
    }
    return (nbytes-nleft);
}

int SL_OS_File::lseek(long offset, int whence)
{
    return ::lseek(fd_, offset, whence);
}

int SL_OS_File::flush()
{
    return ::fsync(fd_);
}

int SL_OS_File::dup()
{
    int new_fd = ::dup(fd_);
    if (new_fd < 0)
    {
        return -1;
    }
    fd_ = new_fd;
    return new_fd;
}

int SL_OS_File::dup2(int fd)
{
    int new_fd = ::dup2(fd_, fd);
    if (new_fd < 0)
    {
        return -1;
    }
    fd_ = new_fd;
    return new_fd;
}

int SL_OS_File::get_fileno() const
{
    return fd_;
}

void SL_OS_File::set_fileno(int fileno) 
{
    close();
    fd_ = fileno;
}

const SL_TCHAR* SL_OS_File::get_file_name() const
{
    return file_name_.c_str();
}

#endif

