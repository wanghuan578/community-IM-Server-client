#ifndef SOCKETLITE_CIRCULAR_BYTEBUFFER_H
#define SOCKETLITE_CIRCULAR_BYTEBUFFER_H
#include <stdlib.h>

class SL_Circular_ByteBuffer
{
private:
    SL_Circular_ByteBuffer()
    {
    }

public:
    SL_Circular_ByteBuffer(unsigned int buffer_size)
    {
        m_readpos   = 0;
        m_writepos  = 0;
        m_count     = 0;
        m_buffer    = (char*)malloc(buffer_size);
        if (NULL != m_buffer)
        {
            m_buffer_size = buffer_size;
        }
        else
        {
            m_buffer_size = 0;
        }
    }

    ~SL_Circular_ByteBuffer()
    {
        if (NULL == m_buffer)
        {
            free(m_buffer);
            m_buffer = NULL;
        }
    }

    int buffer_size() const
    {
        return m_buffer_size;
    }

    int read(char *data, unsigned int size)
    {
        if (m_count > 0)
        {
            if (size > m_count)
            {
                size = m_count;
            }
            if (m_readpos <= m_writepos)
            {
                if (size > m_count)
                {
                    memcpy(data, m_buffer+m_readpos, m_count);
                    m_readpos = m_writepos;
                    m_count = 0;
                    return m_count;
                }
                else
                {
                    memcpy(data, m_buffer+m_readpos, size);
                    m_readpos += size;
                    m_count -= size;
                    if (m_readpos >= m_buffer_size)
                    {
                        m_readpos = 0;
                    }
                    return size;
                }
            }
            else
            {
                if (size <= m_buffer_size-m_readpos)
                {
                    memcpy(data, m_buffer+m_readpos, size);
                    m_readpos += size;
                    m_count -= size;
                    if (m_readpos >= m_buffer_size)
                    {
                        m_readpos = 0;
                    }
                    return size;
                }
                else
                {
                    memcpy(data, m_buffer+m_readpos, m_buffer_size-m_readpos);
                    size -= (m_buffer_size-m_readpos);
                    if (size < m_writepos)
                    {
                        memcpy(data, m_buffer, size);
                        m_readpos = size;
                        m_count = m_writepos-size;
                        return m_buffer_size-m_readpos+size;
                    }
                    else
                    {
                        memcpy(data, m_buffer, m_writepos);
                        m_readpos = m_writepos;
                        m_count = 0;
                        return m_buffer_size-m_readpos+m_writepos;
                    }
                }
            }
        }
        return 0;
    }

    int write(const char *data, unsigned int size)
    {
        if (size > m_buffer_size)
        {
            return -1;
        }

        if (size <= (m_buffer_size-m_writepos))
        {
            memcpy(m_buffer+m_writepos, data, size);
            if (
                (m_readpos > m_writepos) && 
                (m_readpos<(m_writepos+size))
                )
            {
                m_readpos = m_writepos+size;
            }
            m_writepos += size;
            if (m_writepos >= m_buffer_size)
            {
                m_writepos = 0;
            }
        }
        else
        {
            int right_size = m_buffer_size-m_writepos;
            memcpy(m_buffer+m_writepos, data, right_size);
            memcpy(m_buffer, data+right_size, size-right_size);

            if (
                (m_readpos >= m_writepos) || 
                (m_readpos < (size-right_size))
                )
            {
                m_readpos  = size-right_size; 
            }
            m_writepos = size-right_size;
        }
        m_count += size;
        if (m_count > m_buffer_size)
        {
            m_count = m_buffer_size;
        }
        return 0;
    }

    int rewrite(char *data, unsigned int size)
    {
        return 0;
    }

private:
    unsigned int m_buffer_size;
    unsigned int m_readpos;
    unsigned int m_writepos;
    unsigned int m_count;
    char *m_buffer;
};

#endif
