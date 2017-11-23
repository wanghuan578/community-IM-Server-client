#ifndef SOCKETLITE_RPC_THRIFT_BUFFER_TRANSPORT_H
#define SOCKETLITE_RPC_THRIFT_BUFFER_TRANSPORT_H
#ifdef SOCKETLITE_USE_THRIFT
#include "transport/TBufferTransports.h"
#include "SL_ByteBuffer.h"

template <typename TByteBuffer>
class SL_Rpc_Thrift_BufferTransport : public apache::thrift::transport::TTransport 
{
public:
	inline SL_Rpc_Thrift_BufferTransport()
        : read_buffer_begin_(NULL)
        , read_buffer_end_(NULL)
        , write_buffer_(NULL)
	{
	}

	/**
	* Virtual deconstructor.
	*/
	inline virtual ~SL_Rpc_Thrift_BufferTransport() 
    {
    }

public:

	inline void setBuffer(char *read_buffer, int read_buffer_len, TByteBuffer *write_buffer)
	{
        read_buffer_begin_  = read_buffer;
        read_buffer_end_    = read_buffer + read_buffer_len;
        write_buffer_       = write_buffer;	
	}

	/**
	* Whether this transport is open.
	*/
	inline bool isOpen() 
    {
        return true;
    }

	/**
	* Tests whether there is more data to read or if the remote side is
	* still open. By default this is true whenever the transport is open,
	* but implementations should add logic to test for this condition where
	* possible (i.e. on a socket).
	* This is used by a server to check if it should listen for another
	* request.
	*/
	inline bool peek() 
	{
        return static_cast<int>(read_buffer_end_ - read_buffer_begin_);
	}

	/**
	* Opens the transport for communications.
	*
	* @return bool Whether the transport was successfully opened
	* @throws TTransportException if opening failed
	*/
	inline void open() 
    {
        //throw TTransportException(TTransportException::NOT_OPEN, "Cannot open base TTransport.");
	}

	/**
	* Closes the transport.
	*/
	inline void close() 
    {
        //throw TTransportException(TTransportException::NOT_OPEN, "Cannot close base TTransport.");
	}

	/**
	* Attempt to read up to the specified number of bytes into the string.
	*
	* @param buf  Reference to the location to write the data
	* @param len  How many bytes to read
	* @return How many bytes were actually read
	* @throws TTransportException If an error occurs
	*/
	inline uint32_t read(uint8_t *buf, uint32_t len) 
	{        
        uint32_t read_buffer_len = static_cast<uint32_t>(read_buffer_end_ - read_buffer_begin_);
        if (read_buffer_len >= len)
        {
            memcpy(buf, read_buffer_begin_, len);
            read_buffer_begin_ += len;
            return len;
        }
        return 0;
	}

	/**
	* Called when read is completed.
	* This can be over-ridden to perform a transport-specific action
	* e.g. logging the request to a file
	*
	*/
	inline void readEnd() 
    {
	    // default behaviour is to do nothing
	    return;
	}

	/**
	* Writes the string in its entirety to the buffer.
	*
	* @param buf  The data to write out
	* @throws TTransportException if an error occurs
	*/
	inline void write(const uint8_t *buf , uint32_t len) 
	{
        write_buffer_->write((const char*)buf, len);
	}

	/**
	* Called when write is completed.
	* This can be over-ridden to perform a transport-specific action
	* at the end of a request.
	*
	*/
	inline void writeEnd() 
    {
	    // default behaviour is to do nothing
	    return;
	}

	/**
	* Flushes any pending data to be written. Typically used with buffered
	* transport mechanisms.
	*
	* @throws TTransportException if an error occurs
	*/
	//把数据的长度写到头部
	inline void flush() 
	{
        return;
	}

	/**
	* Attempts to return a pointer to \c len bytes, possibly copied into \c buf.
	* Does not consume the bytes read (i.e.: a later read will return the same
	* data).  This method is meant to support protocols that need to read
	* variable-length fields.  They can attempt to borrow the maximum amount of
	* data that they will need, then consume (see next method) what they
	* actually use.  Some transports will not support this method and others
	* will fail occasionally, so protocols must be prepared to use read if
	* borrow fails.
	*
	* @oaram buf  A buffer where the data can be stored if needed.
	*             If borrow doesn't return buf, then the contents of
	*             buf after the call are undefined.
	* @param len  *len should initially contain the number of bytes to borrow.
	*             If borrow succeeds, *len will contain the number of bytes
	*             available in the returned pointer.  This will be at least
	*             what was requested, but may be more if borrow returns
	*             a pointer to an internal buffer, rather than buf.
	*             If borrow fails, the contents of *len are undefined.
	* @return If the borrow succeeds, return a pointer to the borrowed data.
	*         This might be equal to \c buf, or it might be a pointer into
	*         the transport's internal buffers.
	* @throws TTransportException if an error occurs
	*/
	inline const uint8_t* borrow(uint8_t *buf, uint32_t *len) 
	{
		return NULL;
	}

	/**
	* Remove len bytes from the transport.  This should always follow a borrow
	* of at least len bytes, and should always succeed.
	* TODO(dreiss): Is there any transport that could borrow but fail to
	* consume, or that would require a buffer to dump the consumed data?
	*
	* @param len  How many bytes to consume
	* @throws TTransportException If an error occurs
	*/
	inline void consume(uint32_t len) 
	{
        return;
	}

protected:
    //读缓冲区
    char *read_buffer_begin_;   //读缓冲区开始
    char *read_buffer_end_;     //读缓冲区结束   

    //写缓冲区
    TByteBuffer *write_buffer_;
};

#endif  //#ifdef SOCKETLITE_USE_THRIFT
#endif  //#ifndef SOCKETLITE_RPC_THRIFT_BUFFERTRANSPORT_H

