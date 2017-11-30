#ifndef SOCKETLITE_CRYPTO_BASEN_H
#define SOCKETLITE_CRYPTO_BASEN_H

class SL_Crypto_BaseN
{
public:
    enum TYPE
    {
        ENCODE = 1,
        DECODE = 2
    };

    SL_Crypto_BaseN()
    {
    }

    virtual ~SL_Crypto_BaseN()
    {
    }
    
    virtual int get_need_len(unsigned int len, SL_Crypto_BaseN::TYPE type) = 0;
    virtual int encode(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int *out_len) = 0;
    virtual int decode(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int *out_len) = 0;
};

#endif

