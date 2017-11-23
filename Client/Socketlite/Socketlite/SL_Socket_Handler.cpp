#include "SL_Socket_Handler.h"
#include "SL_Socket_Source.h"

int SL_Socket_Handler::write_data(const char *buf, int len)
{
    if (socket_source_->get_socket_handler() == this)
    {
        return socket_source_->send(buf, len, 0);
    }
    return SL_Socket_CommonAPI::socket_send(socket_, buf, len);
}

