#ifndef SOCKETLITE_CRYPTO_HASH_H
#define SOCKETLITE_CRYPTO_HASH_H

class SL_Crypto_Hash
{
public:
    SL_Crypto_Hash()
    {
    }

    virtual ~SL_Crypto_Hash()
    {
    }

    virtual int get_digest_len() = 0;
    virtual int init() = 0;
    virtual int update(const unsigned char *input, unsigned int input_len) = 0;
    virtual int final(unsigned char *digest) = 0;
    virtual int final(const unsigned char *input, unsigned int input_len, unsigned char *digest) = 0;
};

#endif

