#include "SL_Crypto_SHA1.h"

#ifdef SOCKETLITE_USE_OPENSSL
SL_Crypto_SHA1::SL_Crypto_SHA1()
{
	init();
}

SL_Crypto_SHA1::~SL_Crypto_SHA1()
{
}

int SL_Crypto_SHA1::get_digest_len()
{
	return SHA_DIGEST_LENGTH;
}

int SL_Crypto_SHA1::init()
{
	return SHA1_Init(&ctx_);
}

int SL_Crypto_SHA1::update(const unsigned char *input, unsigned int input_len)
{
	return SHA1_Update(&ctx_, input, input_len);
}

int SL_Crypto_SHA1::final(unsigned char *digest)
{
	SHA1_Final(digest, &ctx_);

	//recover init status
	SHA1_Init(&ctx_);

	return 0;
}

int SL_Crypto_SHA1::final(const unsigned char *input, unsigned int input_len, unsigned char *digest)
{
	SHA1_Update(&ctx_, input, input_len);
	SHA1_Final(digest, &ctx_);

	//recover init status
	SHA1_Init(&ctx_);

	return 0;
}
#else
SL_Crypto_SHA1::SL_Crypto_SHA1()
{
}

SL_Crypto_SHA1::~SL_Crypto_SHA1()
{
}

int SL_Crypto_SHA1::get_digest_len()
{
    return 0;
}

int SL_Crypto_SHA1::init()
{
    return 0;
}

int SL_Crypto_SHA1::update(const unsigned char *input, unsigned int input_len)
{
	return 0;
}

int SL_Crypto_SHA1::final(unsigned char *digest)
{
	return 0;
}

int SL_Crypto_SHA1::final(const unsigned char *input, unsigned int input_len, unsigned char *digest)
{
    return 0;
}

#endif

