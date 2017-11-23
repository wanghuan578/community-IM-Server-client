
#include "Serialize.h"
#include "SL_Crypto_MD5.h"
#include "SL_Crypto_Hex.h"
#include "SL_Crypto_AES.h"
//#include <openssl/ssl.h>
//#include <openssl/err.h>
#include <openssl/evp.h>
////#include <openssl/x509.h>
////#include <openssl/ossl_typ.h>

CSerialize::CSerialize()
{
}

CSerialize::~CSerialize()
{
}

string CSerialize::encode(SL_ByteBuffer &content, string &key)
{
	int nOutputLen = 0;
	char *aes_buff = aes_encrypt2(key, content.data(), content.data_size(), &nOutputLen);

	SL_Crypto_Hex hex;
	unsigned int nLenShowBuf = nOutputLen * 2;
	SL_ByteBuffer hex_buff(nLenShowBuf + 1);
	hex_buff.data_end(nLenShowBuf + 1);
	hex_buff.mzero();
	hex.encode((unsigned char*)aes_buff, nOutputLen, (unsigned char*)hex_buff.buffer(), &nLenShowBuf);

	delete [] aes_buff;

	return hex_buff.data();
}

string CSerialize::MD5(string &content)
{
	SL_Crypto_MD5 md5;
	char key[16] = {0};
	md5.init();
	md5.update((const unsigned char*)content.data(), content.size());
	md5.final((unsigned char*)key);

	SL_Crypto_Hex hex;
	hex.set_upper_case(false);
	char check_hex_code[33]={0};
	int len = 32;
	hex.encode((const unsigned char *)key, 16, (unsigned char *)check_hex_code, (unsigned int *)&len);

	return check_hex_code;
}

SL_ByteBuffer *CSerialize::decode(string &content, string &key)
{
	SL_Crypto_Hex hex;
	int hex_len = hex.get_need_len(content.size(), SL_Crypto_BaseN::DECODE);
	SL_ByteBuffer reverse_hex_buff(hex_len);
	int len = reverse_hex_buff.buffer_size();
	hex.decode((const unsigned char *)content.c_str(), content.size(), (unsigned char *)(reverse_hex_buff.buffer()), (unsigned int *)&len);
	reverse_hex_buff.data_end(len);
	
	return aes_decrypt2(key, reverse_hex_buff.data(), reverse_hex_buff.data_size());
}

char* CSerialize::aes_encrypt2(string sKey, char* inBuff, int inLen, int* outLen)
{
	evp_cipher_ctx_st ctx;

	int nBytes = 0;
	if ( inLen % AES_BLOCK_SIZE )
		nBytes = ((inLen/AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;
	else
		nBytes = inLen;

	//char* pBuff = new char[nBytes];
	//memset(pBuff, 0, nBytes);
	//memcpy(pBuff, inBuff, inLen);

	char* outBuff = new char[nBytes];
	memset(outBuff, 0, nBytes);

	int nHandBytes = 0;
	
	EVP_CIPHER_CTX_init(&ctx);
	EVP_EncryptInit_ex(&ctx, EVP_aes_256_ecb(), NULL, (const unsigned char *)sKey.c_str(), NULL);
	EVP_CIPHER_CTX_set_padding(&ctx, 0/*EVP_CIPH_NO_PADDING*/);
	while(nHandBytes < nBytes)
	{
		int outLen = 0;
		//EVP_EncryptUpdate(&ctx, (unsigned char *)(outBuff + nHandBytes), &outLen, (const unsigned char *)(pBuff + nHandBytes), AES_BLOCK_SIZE);
		EVP_EncryptUpdate(&ctx, (unsigned char *)(outBuff + nHandBytes), &outLen, (const unsigned char *)(inBuff + nHandBytes), AES_BLOCK_SIZE);
		nHandBytes += outLen;
	}
	EVP_CIPHER_CTX_cleanup(&ctx);
	*outLen = nHandBytes;

	//delete [] pBuff;

	return outBuff;
}

SL_ByteBuffer *CSerialize::aes_decrypt2(string sKey, char* in_buff, int in_len)
{
	int nBytes = 0;

	if (in_len % AES_BLOCK_SIZE )
		nBytes = ((in_len/AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;
	else
		nBytes = in_len;

	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	EVP_DecryptInit_ex(&ctx, EVP_aes_256_ecb(), NULL, (const unsigned char *)sKey.c_str(), NULL);

	int blockSize = EVP_CIPHER_block_size(ctx.cipher);
	EVP_CIPHER_CTX_set_padding(&ctx, 0);

	SL_ByteBuffer *decrypt_buff = new SL_ByteBuffer(nBytes + 1);
	decrypt_buff->data_end(nBytes + 1);
	decrypt_buff->mzero();

	int out_len = 0;
	int ret = EVP_DecryptUpdate(&ctx, (unsigned char*)decrypt_buff->data(), &out_len, (const unsigned char*)in_buff, in_len);
	
	decrypt_buff->data_end(out_len);

	return decrypt_buff;
}