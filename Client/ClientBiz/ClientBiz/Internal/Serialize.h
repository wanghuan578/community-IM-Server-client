
#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

#include "SL_Crypto_MD5.h"
//#include "SL_Crypto_AES.h"
#include "SL_Crypto_Hex.h"
#include "SL_ByteBuffer.h"





#include <string>

using namespace std;

class CSerialize
{
public:
	CSerialize();
	~CSerialize();

	template<typename T>
	static string MD5(T &type, int size = 1024)
	{
		SL_ByteBuffer content(size);
		SL_Rpc_MessageParser::serialize_to_buffer<T, SL_ByteBuffer>(&type, &content);

		SL_Crypto_MD5 md5;
		char key[16] = {0};
		md5.init();
		md5.update((const unsigned char*)content.data(), content.data_size());
		md5.final((unsigned char*)key);

		SL_Crypto_Hex hex;
		hex.set_upper_case(false);
		char check_hex_code[33]={0};
		int len = 32;
		hex.encode((const unsigned char *)key, 16, (unsigned char *)check_hex_code, (unsigned int *)&len);

		return check_hex_code;
	}

	static char *aes_encrypt2(string sKey, char* inBuff, int inLen, int* outLen);
	static SL_ByteBuffer *aes_decrypt2(string sKey, char* inBuff, int inLen);
	//static char* Hex2Str(char* inBuff, int inLen);

	//string CSerialize::encode_ex()
	//{
	//	SL_Crypto_AES Rijndael;
	//	string true_key = "E10ADC3949BA59AB";
	//	Rijndael.init((const unsigned char*)true_key.c_str(), (const unsigned char *)"123456", SL_CRYPTO_MODE_ECB);
	//	char *content = "helloworld";
	//	int nOutputLen = Rijndael.get_need_len(strlen(content));
	//	SL_ByteBuffer aes_buf(nOutputLen);
	//	aes_buf.data_end(nOutputLen);
	//	aes_buf.mzero();
	//	int ret = Rijndael.encrypt((const unsigned char*)content, strlen(content), (unsigned char*)aes_buf.buffer(), nOutputLen);

	//	SL_Crypto_Hex hex;
	//	unsigned int nLenShowBuf = nOutputLen * 2;
	//	SL_ByteBuffer hex_buff(nLenShowBuf + 1);
	//	hex_buff.data_end(nLenShowBuf + 1);
	//	hex_buff.mzero();
	//	hex.encode((unsigned char*)aes_buf.data(), nOutputLen, (unsigned char*)hex_buff.buffer(), &nLenShowBuf);

	//	return hex_buff.data();
	//}
	//static string Hex(string &content);
	static string MD5(string &content);


	 template<typename T>
	 static string AES(T &type, string key, int size = 1024)
	 {
		 SL_ByteBuffer buff(size);
		 SL_Rpc_MessageParser::serialize_to_buffer<T, SL_ByteBuffer>(&type, &buff);

		 return encode(buff, key);
	 }

	 template<typename T>
	 static bool DeAES(T &type, string content, string key)
	 {
		SL_ByteBuffer *decrypt_buff = decode(content, key);

		int ret = SL_Rpc_MessageParser::parse_to_body<T>(&type, (char*)decrypt_buff->data(), decrypt_buff->data_size());

		delete decrypt_buff;

		return ret;
	 }

protected:
	static string encode(SL_ByteBuffer &content, string &key);
	static string encode(string &content, string &key);
	static SL_ByteBuffer *decode(string &content, string &key);
};
#endif