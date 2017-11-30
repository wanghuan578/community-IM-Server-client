#include "SL_Crypto_MD5.h"

#ifdef SOCKETLITE_USE_OPENSSL
SL_Crypto_MD5::SL_Crypto_MD5()
{
	init();
}

SL_Crypto_MD5::~SL_Crypto_MD5()
{
}

int SL_Crypto_MD5::get_digest_len()
{
	return MD5_DIGEST_LENGTH;
}

int SL_Crypto_MD5::init()
{
	return MD5_Init(&ctx_);
}

int SL_Crypto_MD5::update(const unsigned char *input, unsigned int input_len)
{
	return MD5_Update(&ctx_, input, input_len);
}

int SL_Crypto_MD5::final(unsigned char *digest)
{
	MD5_Final(digest, &ctx_);

	//recover init status
	MD5_Init(&ctx_);

	return 0;
}

int SL_Crypto_MD5::final(const unsigned char *input, unsigned int input_len, unsigned char *digest)
{
	MD5_Update(&ctx_, input, input_len);
	MD5_Final(digest, &ctx_);

	//recover init status
	MD5_Init(&ctx_);

	return 0;
}
#else
SL_Crypto_MD5::SL_Crypto_MD5()
{
}

SL_Crypto_MD5::~SL_Crypto_MD5()
{
}

int SL_Crypto_MD5::get_digest_len()
{
    return 0;
}

int SL_Crypto_MD5::init()
{
    return 0;
}

int SL_Crypto_MD5::update(const unsigned char *input, unsigned int input_len)
{
	return 0;
}

int SL_Crypto_MD5::final(unsigned char *digest)
{
	return 0;
}

int SL_Crypto_MD5::final(const unsigned char *input, unsigned int input_len, unsigned char *digest)
{
    return 0;
}
#endif

