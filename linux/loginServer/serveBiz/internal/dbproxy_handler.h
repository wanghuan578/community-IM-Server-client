
#ifndef __DBPROXY_HANDLER_H__
#define __DBPROXY_HANDLER_H__

#include "SL_Socket_Message_Handler.h"
#include "SL_Socket_SendControl_Handler.h"
#include "SL_Sync_SpinMutex.h"
#include "SL_Logger.h"


class CDBProxyHandler : public SL_Socket_SendControl_Handler<SL_Socket_Message_Handler, SL_ByteBuffer, SL_Sync_SpinMutex, SL_Crypto_SymmetricCipher, SL_Crypto_SymmetricCipher>
{
public:
	CDBProxyHandler();
	~CDBProxyHandler();

public:
	int do_open();
	int do_message(const char *msg, int len);
	int do_close();

private:
	SL_LOGGER_CLASS_DECL(logger);
};
#endif