#include <memory.h>
#include "SL_Crypto_AES.h"

#ifdef SOCKETLITE_USE_OPENSSL
SL_Crypto_AES::SL_Crypto_AES()
    : block_size_(AES_BLOCK_SIZE)
    , mode_(SL_CRYPTO_MODE_ECB)
{
}

SL_Crypto_AES::~SL_Crypto_AES()
{
}

int SL_Crypto_AES::get_need_len(unsigned int len, SL_Crypto_SymmetricCipher::TYPE type)
{
    if (SL_Crypto_SymmetricCipher::ENCRYPT == type)
    {
        int remainder = len % block_size_;
        return (remainder ? (len-remainder+block_size_+1) : (len+1));
    }
    else
    {
        return len-1;
    }
}

int SL_Crypto_AES::init(const unsigned char *key, const unsigned char *ivec, int mode, int key_len, int block_size)
{
	AES_set_encrypt_key(key, 8*block_size, &encrypt_key_);
	AES_set_decrypt_key(key, 8*block_size, &decrypt_key_);

    block_size_ = block_size;
	mode_		= mode;
	return 0;
}

int SL_Crypto_AES::encrypt_block(const unsigned char *input, unsigned char *out)
{
    AES_encrypt(input, out, &encrypt_key_);
	return 0;
}

int SL_Crypto_AES::decrypt_block(const unsigned char *input, unsigned char *out)
{
    AES_decrypt(input, out, &decrypt_key_);
	return 0;
}

int SL_Crypto_AES::encrypt(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int out_len)
{
	unsigned int endblock_len	= input_len % block_size_;
	unsigned int need_len		= endblock_len ? (input_len-endblock_len+block_size_+1) : (input_len+1);
	if (out_len < need_len)
	{
		return -1;
	}

	unsigned char *input_pos = (unsigned char *)input;
	unsigned char *out_pos   = (out + 1);
	unsigned int block_count = input_len/block_size_;
	for (unsigned int i=0; i<block_count; ++i)
	{
		encrypt_block(input_pos, out_pos);
		input_pos	+= block_size_;
		out_pos		+= block_size_;
	}
	if (endblock_len > 0)
	{            
		char endblock[SL_CRYPTO_BLOCK_SIZE_32] = {0};
		memcpy(endblock, input_pos, endblock_len);
		out[0] = block_size_-endblock_len;
		encrypt_block((const unsigned char *)endblock, out_pos);
	}
	else
	{
		out[0] = 0;
	}
	return need_len;
}

int SL_Crypto_AES::decrypt(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int out_len)
{
	if (out_len < input_len)
	{
		return -1;
	}

	unsigned char *input_pos = (unsigned char *)(input + 1);
	unsigned char *out_pos = out;
	unsigned int block_count = input_len/block_size_;
	for (unsigned int i=0; i<block_count; ++i)
	{
		decrypt_block(input_pos, out_pos);
		input_pos	+= block_size_;
		out_pos		+= block_size_;
	}        
	return (input_len-input[0]-1);
}
#else
SL_Crypto_AES::SL_Crypto_AES()
{
}

SL_Crypto_AES::~SL_Crypto_AES()
{
}

int SL_Crypto_AES::get_need_len(unsigned int len, SL_Crypto_SymmetricCipher::TYPE type)
{
    if (SL_Crypto_SymmetricCipher::ENCRYPT == type)
    {
        int remainder = len % block_size_;
        return (remainder ? (len-remainder+block_size_+1) : (len+1));
    }
    else
    {
        return len-1;
    }
}

int SL_Crypto_AES::init(const unsigned char *key, const unsigned char *ivec, int mode, int key_len, int block_size)
{
    return 0;
}

int SL_Crypto_AES::encrypt_block(const unsigned char *input, unsigned char *out)
{
    return 0;
}

int SL_Crypto_AES::decrypt_block(const unsigned char *input, unsigned char *out)
{
    return 0;
}

int SL_Crypto_AES::encrypt(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int out_len)
{
    return 0;
}

int SL_Crypto_AES::decrypt(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int out_len)
{
    return 0;
}

#endif

