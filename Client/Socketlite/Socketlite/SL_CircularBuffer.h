#ifndef SOCKETLITE_CIRCULAR_BYTEBUFFER_H
#define SOCKETLITE_CIRCULAR_BYTEBUFFER_H
#include <stdlib.h>

class SL_CircularBuffer
{
private:
    inline SL_CircularBuffer()
    {
    }

public:
    inline SL_CircularBuffer(int bffer_size)
        : buffer_size_(bffer_size)
        , data_begin_index_(0)
        , data_end_index_(0)
        , data_size_(0)
    {
        buffer_ = (char *)malloc(bffer_size);
    }

    inline ~SL_CircularBuffer()
    {
        if (NULL == buffer_)
        {
            free(buffer_);
        }
    }

    inline int buffer_size() const
    {
        return buffer_size_;
    }
    
    inline int data_size() const
    {
        return data_size_;
    }

    inline void clear()
    {
        data_begin_index_   = 0;
        data_end_index_     = 0;
        data_size_          = 0;
    }

    inline int write(const char *data, int size)
    {
        int capacity    = buffer_size_;
        int write_bytes = min(size, capacity - size);
        if (capacity - data_end_index_ >= write_bytes)
        {
            memcpy(buffer_ + data_end_index_, data, write_bytes);
            data_end_index_ += write_bytes;
            if (data_end_index_ == capacity)
            {
                data_end_index_ = 0;
            }
        }
        else
        {
            int size_1 = capacity - data_end_index_;
            memcpy(buffer_ + data_end_index_, data, size_1);
            int size_2 = write_bytes - size_1;
            memcpy(buffer_, data + size_1, size_2);
            data_end_index_ = size_2;
        }
        data_size_ += write_bytes;
        return write_bytes;
    }

    inline int read(char *data, int size)
    {
        int capacity    = buffer_size_;
        int read_bytes  = min(size, data_size_);
        if (capacity - data_begin_index_ >= read_bytes)
        {
            memcpy(data, buffer_ + data_begin_index_, read_bytes);
            data_begin_index_ += read_bytes;
            if (data_begin_index_ == capacity)
            {
                data_begin_index_ = 0;
            }
        }
        else
        {
            int size_1 = capacity - data_begin_index_;
            memcpy(data, buffer_ + data_begin_index_, size_1);
            int size_2 = read_bytes - size_1;
            memcpy(data + size_1, buffer_, size_2);
            data_begin_index_ = size_2;
        }
        data_size_ -= read_bytes;
        return read_bytes;
    }

private:
    char    *buffer_;
    int     buffer_size_;
    int     data_begin_index_;
    int     data_end_index_;
    int     data_size_;
};

#endif

