
#include "serialize.h"


CSerialize::CSerialize()
{
}

CSerialize::~CSerialize()
{
}

string CSerialize::Encode(SL_ByteBuffer &content, string &key)
{
	SL_Crypto_AES Rijndael;
	string true_key = key;
	Rijndael.init((const unsigned char*)true_key.c_str(), (const unsigned char *)"123456", SL_CRYPTO_MODE_ECB);
	int nOutputLen = Rijndael.get_need_len(content.data_size());
	SL_ByteBuffer aes_buf(nOutputLen);
	aes_buf.data_end(nOutputLen);
	aes_buf.mzero();
	int ret = Rijndael.encrypt((const unsigned char*)content.data(), content.data_size(), (unsigned char*)aes_buf.buffer(), nOutputLen);

	SL_Crypto_Hex hex;
	unsigned int nLenShowBuf = nOutputLen * 2;
	SL_ByteBuffer hex_buff(nLenShowBuf + 1);
	hex_buff.data_end(nLenShowBuf + 1);
	hex_buff.mzero();
	hex.encode((unsigned char*)aes_buf.data(), nOutputLen, (unsigned char*)hex_buff.buffer(), &nLenShowBuf);

	string encode_str = hex_buff.data();

	return encode_str;
}

SL_ByteBuffer CSerialize::Decode(string &content, string &key)
{
	SL_Crypto_Hex hex;
	int hex_len = hex.get_need_len(content.size(), SL_Crypto_BaseN::DECODE);
	SL_ByteBuffer reverse_hex_buff(hex_len);
	int len = reverse_hex_buff.buffer_size();
	hex.decode((const unsigned char *)content.c_str(), content.size(), (unsigned char *)(reverse_hex_buff.buffer()), (unsigned int *)&len);
	reverse_hex_buff.data_end(len);

	SL_Crypto_AES aes;
	int need_len = aes.get_need_len(reverse_hex_buff.data_size());
	SL_ByteBuffer reverse_aes_buff(need_len);
	reverse_aes_buff.data_end(need_len);
	reverse_aes_buff.mzero();
	aes.init((const unsigned char *)key.c_str(), (const unsigned char *)"123456", SL_CRYPTO_MODE_ECB);
	aes.decrypt((const unsigned char *)reverse_hex_buff.data(), reverse_hex_buff.data_size(), (unsigned char *)reverse_aes_buff.buffer(), reverse_aes_buff.buffer_size());

	return reverse_aes_buff;	
}
