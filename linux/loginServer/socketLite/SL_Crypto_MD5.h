#ifndef SOCKETLITE_CRYPTO_MD5_H
#define SOCKETLITE_CRYPTO_MD5_H
#include "SL_Crypto_Hash.h"

#ifdef SOCKETLITE_USE_OPENSSL
#include <openssl/md5.h>
class SL_Crypto_MD5 : public SL_Crypto_Hash
{
public:
    SL_Crypto_MD5();
    virtual ~SL_Crypto_MD5();

    int get_digest_len();
    int init();
    int update(const unsigned char *input, unsigned int input_len);
    int final(unsigned char *digest);
    int final(const unsigned char *input, unsigned int input_len, unsigned char *digest);

private:
    MD5_CTX ctx_;
};
#else
class SL_Crypto_MD5 : public SL_Crypto_Hash
{
public:
    SL_Crypto_MD5();
    virtual ~SL_Crypto_MD5();

    int get_digest_len();
    int init();
    int update(const unsigned char *input, unsigned int input_len);
    int final(unsigned char *digest);
    int final(const unsigned char *input, unsigned int input_len, unsigned char *digest);
};
#endif

#endif

