#ifndef SOCKETLITE_BYTEBUFFER_H
#define SOCKETLITE_BYTEBUFFER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
    #pragma once
#endif
#include <stdlib.h>
#include <string.h>
#include "SL_DataType.h"
#include "SL_Sync_Atomic.h"

class SL_Buffer
{
public:
    SL_Buffer() 
    {
    }

    virtual ~SL_Buffer() 
    {
    }

    virtual void clear() 
    {
    }

    virtual void reset() 
    {
    }

    virtual char* buffer() const 
    { 
        return NULL;
    }

    virtual int buffer_size() const 
    { 
        return 0;
    }

    virtual bool data(const char *data, int size, bool owner) 
    { 
        return false; 
    }

    virtual char* data() const 
    { 
        return NULL; 
    }

    virtual int data_size() const 
    { 
        return 0; 
    }

    //返回未使用的容量
    virtual int free_size() const 
    { 
        return 0; 
    }

    virtual int data_begin() const 
    { 
        return 0; 
    }

    virtual void data_begin(int new_begin_index) 
    {
    }

    virtual int data_end() const 
    { 
        return 0; 
    }

    virtual void data_end(int new_end_index) 
    {
    }

    virtual bool empty() const 
    { 
        return true; 
    }

    virtual bool owner() const 
    { 
        return false; 
    }

    virtual void owner(bool owner) 
    {
    }

    virtual bool reserve(int new_size)
    {
        return false;
    }

    virtual bool resize(int new_size) 
    { 
        return reserve(new_size); 
    }

    virtual bool write(const char *data, int size, int multiple)
    {
        return false;
    }

    virtual bool write(const char *data)
    {
        return write(data, (int)strlen(data), 100);
    }

    virtual bool write(char c)
    {
        return write((const char *)&c, sizeof(char), 100);
    }

    virtual bool write(short n)
    {
        return write((const char *)&n, sizeof(short), 100);
    }

    virtual bool write(int n)
    {
        return write((const char *)&n, sizeof(int), 100);
    }

    virtual bool write(int64 n)
    {
        return write((const char *)&n, sizeof(int64), 100);
    }

    virtual bool write(float f)
    {
        return write((const char *)&f, sizeof(float), 100);
    }

    virtual bool write(double d)
    {
        return write((const char *)&d, sizeof(double), 100);
    }
};

class SL_ByteBuffer
{
public:
    inline SL_ByteBuffer()
        : buffer_(NULL)
        , buffer_size_(0)
        , data_begin_index_(0)
        , data_end_index_(0)
        , owner_(true)
    {
    }

    inline SL_ByteBuffer(int buffer_size)
        : data_begin_index_(0)
        , data_end_index_(0)
        , owner_(true)
    {
        char *new_buffer = (char *)malloc(buffer_size);
        if (NULL != new_buffer)
        {
            buffer_      = new_buffer;
            buffer_size_ = buffer_size;
        }
        else
        {
            buffer_      = NULL;
            buffer_size_ = 0;
        }
    }

	inline SL_ByteBuffer(char *buff, int len)
		:data_begin_index_(0),
		data_end_index_(len),
		owner_(true)
	{
		owner_ = false;
		char *new_buffer = (char*)malloc(len);
		if (NULL != new_buffer)
		{
			memcpy(new_buffer, buff, len);
			buffer_ = new_buffer;
			buffer_size_ = len;
			owner_ = true;
		}
		else
		{
			buffer_      = NULL;
			buffer_size_ = 0;
		}
	}

    inline SL_ByteBuffer(const SL_ByteBuffer &buf)
    {
        if (buf.owner_)
        {
            buf.owner_          = false;
            buffer_             = buf.buffer_;
            buffer_size_        = buf.buffer_size_;
            data_begin_index_   = buf.data_begin_index_;
            data_end_index_     = buf.data_end_index_;
            owner_              = true;
        }
        else
        {
            buffer_             = NULL;
            buffer_size_        = 0;
            data_begin_index_   = 0;
            data_end_index_     = 0;
            owner_              = false;
        }
    }

    inline ~SL_ByteBuffer()
    {
        if (owner_ && (NULL != buffer_))
        {
            free(buffer_);
        }
    }

    inline SL_ByteBuffer& operator= (const SL_ByteBuffer &buf)
    {
        if (this != &buf)
        {
            if (owner_ && (NULL != buffer_))
            {
                free(buffer_);
            }

            if (buf.owner_)
            {
                buf.owner_          = false;
                buffer_             = buf.buffer_;
                buffer_size_        = buf.buffer_size_;
                data_begin_index_   = buf.data_begin_index_;
                data_end_index_     = buf.data_end_index_;
                owner_              = true;
            }
            else
            {
                buffer_             = NULL;
                buffer_size_        = 0;
                data_begin_index_   = 0;
                data_end_index_     = 0;
                owner_              = false;
            }
        }
        return *this;
    }

    inline void clear()
    {
        if (owner_ && (NULL != buffer_))
        {
            free(buffer_);
        }
        buffer_             = NULL;
        buffer_size_        = 0;
        data_begin_index_   = 0;
        data_end_index_     = 0;
        owner_              = true;
    }

    inline void reset()
    {
        data_begin_index_   = 0;
        data_end_index_     = 0;
    }

    inline char* buffer() const
    {
        return buffer_;
    }

    inline int buffer_size() const
    {
        return buffer_size_;
    }

    inline bool data(const char *data, int size, bool owner = true)
    {
        if (size <= 0)
        {
            return false;
        }
        if (NULL == data)
        {
            return false;
        }
        if ( (data >= buffer_) && (data < (buffer_ + buffer_size_)) )
        {
            return false;
        }

        if (owner)
        {
            char *new_buffer;
            if (owner_)
            {
                new_buffer = (char *)realloc(buffer_, size);
            }
            else
            {   
                new_buffer = (char *)malloc(size);
            }
            if (NULL == new_buffer)
            {
                return false;
            }
            memcpy(new_buffer, data, size);
            buffer_ = new_buffer;
        }
        else
        {
            if (owner_ && (buffer_ != NULL))
            {
                free(buffer_);
            }
            buffer_ = (char *)data;
        }
        buffer_size_        = size;
        data_begin_index_   = 0;
        data_end_index_     = size;
        owner_              = owner;
        return true;
    }

    inline char* data() const
    {
        return buffer_ + data_begin_index_;
    }

    inline int data_size() const
    {
        return data_end_index_ - data_begin_index_;
    }

    //返回未使用的容量
    inline int free_size() const
    {
        return buffer_size_ - data_end_index_;
    }

    inline int data_begin() const
    { 
        return data_begin_index_;
    }

    void data_begin(int new_begin_index)
    {
        if (new_begin_index > buffer_size_)
        {
            new_begin_index = buffer_size_;
        }
        data_begin_index_ = new_begin_index;
        if (data_begin_index_ > data_end_index_)
        {
            data_end_index_ = data_begin_index_;
        }
    }

    inline int data_end() const
    {
        return data_end_index_;
    }

    inline void data_end(int new_end_index)
    {
        if (new_end_index > buffer_size_)
        {
            new_end_index = buffer_size_;
        }
        data_end_index_ = new_end_index;
        if (data_begin_index_ > data_end_index_)
        {
            data_begin_index_ = data_end_index_;
        }
    }

    inline bool empty() const
    {
        return data_end_index_ - data_begin_index_ == 0;
    }

    inline bool owner() const
    {
        return owner_;
    }

    inline void owner(bool owner)
    {
        owner_ = owner;
    }

    bool reserve(int new_size)
    {
        if (owner_)
        {
            if (new_size > buffer_size_)
            {
                char *new_buffer = (char *)realloc(buffer_, new_size);
                if (NULL != new_buffer)
                {
                    buffer_             = new_buffer;
                    buffer_size_        = new_size;
                    data_begin_index_   = 0;
                    data_end_index_     = 0;
                    return true;
                }
            }
        }
        else
        {
            char *new_buffer = (char *)malloc(new_size);
            if (NULL != new_buffer)
            {
                buffer_             = new_buffer;
                buffer_size_        = new_size;
                data_begin_index_   = 0;
                data_end_index_     = 0;
                owner_              = true;
                return true;
            }
        }
        return false;
    }

    inline bool resize(int new_size)
    {
        return reserve(new_size);
    }

	inline void mzero()
	{
		memset(buffer_, 0x00, buffer_size_);
	}

    //multiple: 缓冲区扩大倍数
    inline bool write(const char *data, int size, int multiple = 100)
    {
        if (!owner_)
        {
            return false;
        }

        if (free_size() >= size)
        {
            memcpy(buffer_ + data_end_index_, data, size);
            data_end_index_ += size;
        }
        else
        {   
            return adjust_buffer_i(data, size, multiple);
        }
        return true;
    }

    inline bool write(const char *data)
    {
        return write(data, (int)(strlen(data) + 1));
    }

    inline bool write(char c)
    {
        return write((const char *)&c, sizeof(char));
    }

    inline bool write(short n)
    {
        return write((const char *)&n, sizeof(short));
    }

    inline bool write(int n)
    {
        return write((const char *)&n, sizeof(int));
    }

    inline bool write(int64 n)
    {
        return write((const char *)&n, sizeof(int64));
    }

    inline bool write(float f)
    {
        return write((const char *)&f, sizeof(float));
    }

    inline bool write(double d)
    {
        return write((const char *)&d, sizeof(double));
    }

    inline int read(char *data, int size)
    {
        if (data_size() >= size)
        {
            memcpy(data, buffer_ + data_begin_index_, size);
            data_begin_index_ += size;
            return size;
        }
        return -1;
    }

    inline char* read(int size)
    {
        if (data_size() >= size)
        {
            char *ret = (buffer_ + data_begin_index_);
            data_begin_index_ += size;
            return ret;
        }
        return NULL;
    }

private:
    inline bool adjust_buffer_i(const char *data, int size, int multiple)
    {
        int  new_size    = buffer_size_ * multiple / 100 + buffer_size_ + size;
        char *new_buffer = (char *)realloc(buffer_, new_size);
        if (NULL != new_buffer)
        {
            memcpy(new_buffer + data_end_index_, data, size);
            buffer_         = new_buffer;
            buffer_size_    = new_size;
            data_end_index_ += size;
            return true;
        }
        return false;
    }

private:
    char            *buffer_;           //缓冲区
    int             buffer_size_;       //缓冲区大小
    int             data_begin_index_;  //数据开始位
    int             data_end_index_;    //数据结束位
    mutable bool    owner_;             //表示是否拥有所有权
};

class SL_SharedBuffer
{
public:
    inline SL_SharedBuffer()
        : data_begin_index_(0)
        , data_end_index_(0)
    {
        buffer_ = new Buffer_();
    }

    inline SL_SharedBuffer(int buffer_size)
        : data_begin_index_(0)
        , data_end_index_(0)
    {
        buffer_ = new Buffer_(buffer_size);
    }

    inline SL_SharedBuffer(const SL_SharedBuffer &buf)
        : data_begin_index_(0)
        , data_end_index_(0)
    {
        buf.buffer_->increment_refcount();
        buffer_             = buf.buffer_;
        data_begin_index_   = buf.data_begin_index_;
        data_end_index_     = buf.data_end_index_;
    }

    inline ~SL_SharedBuffer()
    {
        buffer_->release();
    }

    inline SL_SharedBuffer& operator=(const SL_SharedBuffer &buf)
    {
        buffer_->release();

        buf.buffer_->increment_refcount();
        buffer_             = buf.buffer_;
        data_begin_index_   = buf.data_begin_index_;
        data_end_index_     = buf.data_end_index_;
        return *this;
    }

    inline void clear()
    {
        reset();
        buffer_->clear();
    }

    inline bool reserve(int new_size)
    {
        return buffer_->reserve(new_size);
    }

    inline bool resize(int new_size)
    {
        return reserve(new_size);
    }

    inline void reset()
    {
        data_begin_index_ = 0;
        data_end_index_   = 0;
    }

    inline int use_count()
    {
        return buffer_->refcount_.load();
    }

    inline bool empty() const
    {
        return data_end_index_ - data_begin_index_ == 0;
    }

    inline int buffer_size() const
    {
        return buffer_->buffer_size();
    }

    inline char* buffer()
    {
        return buffer_->buffer();
    }

    //返回未使用的容量
    inline int free_size() const
    {
        return buffer_->free_size();
    }

    inline bool data(const char *data, int size, bool owner = true)
    {
        if (size <= 0)
        {
            return false;
        }
        if (NULL == data)
        {
            return false;
        }
        buffer_->release();
        reset();

        if (owner)
        {
            buffer_ = new Buffer_(size);
            write(data, size);
        }
        else
        {
            buffer_ = new Buffer_();
            buffer_->buffer_ = (char *)data;
            buffer_->buffer_size_ = size;
        }
        buffer_->owner_ = owner;
        return true;
    }

    inline char* data() const
    {
        return buffer_->buffer_ + data_begin_index_;
    }

    inline int data_size() const
    {
        return data_end_index_ - data_begin_index_;
    }

    inline int data_begin() const
    { 
        return data_begin_index_;
    }

    void data_begin(int new_begin_index)
    {
        if (new_begin_index > buffer_->buffer_size_)
        {
            new_begin_index = buffer_->buffer_size_;
        }
        data_begin_index_ = new_begin_index;
        if (data_begin_index_ > data_end_index_)
        {
            data_end_index_ = data_begin_index_;
        }
    }

    inline int data_end() const
    {
        return data_end_index_;
    }

    inline void data_end(int new_end_index)
    {
        if (new_end_index > buffer_->buffer_size_)
        {
            new_end_index = buffer_->buffer_size_;
        }
        data_end_index_ = new_end_index;
        if (data_begin_index_ > data_end_index_)
        {
            data_begin_index_ = data_end_index_;
        }
    }

    inline bool owner() const
    {
        return buffer_->owner_;
    }

    inline void owner(bool owner)
    {
        buffer_->owner_ = owner;
    }

    //multiple: 缓冲区扩大倍数
    inline bool write(const char *data, int size, int multiple = 100)
    {
        bool ret = buffer_->write(data, size, multiple);
        if (ret)
        {
            data_end_index_ += size;
        }
        return ret;
    }

    inline bool write(const char *data)
    {
        return write(data, (int)(strlen(data) + 1));
    }

    inline bool write(char c)
    {
        return write((const char *)&c, sizeof(char));
    }

    inline bool write(short n)
    {
        return write((const char *)&n, sizeof(short));
    }

    inline bool write(int n)
    {
        return write((const char *)&n, sizeof(int));
    }

    inline bool write(int64 n)
    {
        return write((const char *)&n, sizeof(int64));
    }

    inline bool write(float f)
    {
        return write((const char *)&f, sizeof(float));
    }

    inline bool write(double d)
    {
        return write((const char *)&d, sizeof(double));
    }
        
    inline int read(char *data, int size)
    {
        int data_size = data_end_index_ - data_begin_index_;
        if (data_size >= size)
        {
            memcpy(data, buffer_->buffer_ + data_begin_index_, size);
            data_begin_index_ += size;
            return size;
        }
        return -1;
    }

    inline char* read(int size)
    {
        int data_size = data_end_index_ - data_begin_index_;
        if (data_size >= size)
        {
            char *ret = buffer_->buffer_ + data_begin_index_;
            data_begin_index_ += size;
            return ret;
        }
        return NULL;
    }

private:
    class Buffer_
    {
    public:
        inline Buffer_()
            : buffer_(NULL)
            , buffer_size_(0)
            , use_size_(0)
            , refcount_(1)
            , owner_(true)
        {
        }

        inline Buffer_(int buffer_size)
            : use_size_(0)
            , refcount_(1)
            , owner_(true)
        {
            char *new_buffer = (char *)malloc(buffer_size);
            if (NULL != new_buffer)
            {
                buffer_      = new_buffer;
                buffer_size_ = buffer_size;
            }
            else
            {
                buffer_      = NULL;
                buffer_size_ = 0;
            }
        }

        inline ~Buffer_()
        {
        }

        inline bool reserve(int new_size)
        {   
            if (owner_ && (new_size > buffer_size_))
            {
                char *new_buffer = (char *)realloc(buffer_, new_size);
                if (NULL != new_buffer)
                {
                    buffer_      = new_buffer;
                    buffer_size_ = new_size;
                    return true;
                }
            }
            return false;
        }

        //加1, 返回变化后值
        inline long increment_refcount()
        {
            return ++refcount_;
        }
        
        //减1, 返回变化后值
        inline long decrement_refcount()
        {
            return --refcount_;
        }

        inline void clear()
        {
            if (NULL != buffer_)
            {
                if (owner_)
                {
                    free(buffer_);
                }
                buffer_ = NULL;
                buffer_size_ = 0;
                use_size_ = 0;
            }
        }

        inline void release()
        {
            if (--refcount_ == 0)
            {
                if (NULL != buffer_)
                {
                    if (owner_)
                    {
                        free(buffer_);
                    }
                    buffer_ = NULL;
                    buffer_size_ = 0;
                    use_size_ = 0;
                }
                                                                                
                delete this;
            }
        }
        
        inline int buffer_size() const
        {
            return buffer_size_;
        }

        inline char* buffer()
        {
            return buffer_;
        }

        inline int free_size() const
        {
            return buffer_size_ - use_size_;
        }

        //multiple: 缓冲区扩大倍数
        inline bool write(const char *data, int size, int multiple = 100)
        {
            if (free_size() >= size)
            {
                memcpy(buffer_ + use_size_, data, size);
                use_size_ += size;
                return true;
            }
            else
            {
                if (owner_)
                {
                    int new_size  = buffer_size_ * multiple / 100 + buffer_size_ + size;
                    char *new_buffer = (char *)realloc(buffer_, new_size);
                    if (NULL != new_buffer)
                    {
                        memcpy(new_buffer + use_size_, data, size);
                        use_size_      += size;
                        buffer_size_    = new_size;
                        buffer_         = new_buffer;
                        return true;
                    }
                }
            }
            return false;
        }

    public:
        char                    *buffer_;       //缓冲区
        int                     buffer_size_;   //缓冲区大小
        int                     use_size_;      //已使用大小
        SL_Sync_Atomic_Int32    refcount_;      //引用计数
        bool                    owner_;         //表示是否拥有所有权
    };

private:
    Buffer_ *buffer_;       
    int     data_begin_index_;  //数据开始位
    int     data_end_index_;    //数据结束位
};

#endif

