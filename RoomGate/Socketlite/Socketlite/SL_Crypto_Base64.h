#ifndef SOCKETLITE_CRYTO_BASE64_H
#define SOCKETLITE_CRYTO_BASE64_H
#include "SL_Crypto_BaseN.h"

class SL_Crypto_Base64 : public SL_Crypto_BaseN
{
public:
    SL_Crypto_Base64();
    virtual ~SL_Crypto_Base64();

    int get_need_len(unsigned int len, SL_Crypto_BaseN::TYPE type = SL_Crypto_BaseN::ENCODE);
    int encode(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int *out_len);
    int decode(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int *out_len);
};

#endif

