#ifndef SOCKETLITE_CRYPTO_HEX_H
#define SOCKETLITE_CRYPTO_HEX_H
#include "SL_Crypto_BaseN.h"

class SL_Crypto_Hex : public SL_Crypto_BaseN
{
public:
    SL_Crypto_Hex();
    virtual ~SL_Crypto_Hex();

    int  get_need_len(unsigned int len, SL_Crypto_BaseN::TYPE type = SL_Crypto_BaseN::ENCODE);
    int  encode(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int *out_len);
    int  decode(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int *out_len);
    void set_upper_case(bool upper = true);

private:
	char *hex_codes_;
};

#endif

