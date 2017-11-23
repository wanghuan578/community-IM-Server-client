#ifndef SOCKETLITE_CRYPTO_H
#define SOCKETLITE_CRYPTO_H

#include "SL_Config.h"
#include "SL_Crypto_BaseN.h"
#include "SL_Crypto_Cipher.h"
#include "SL_Crypto_Hash.h"
#include "SL_Crypto_Rand.h"

#ifdef SOCKETLITE_USE_OPENSSL
    #ifdef SOCKETLITE_OS_WINDOWS
        #pragma comment(lib, "libeay32.lib")
    #endif
#endif

#endif

