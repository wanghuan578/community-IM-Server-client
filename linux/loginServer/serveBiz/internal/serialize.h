
#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

#include "SL_Crypto_AES.h"
#include "SL_Crypto_Hex.h"
#include "SL_ByteBuffer.h"
#include <string>

using namespace std;

class CSerialize
{
public:
	CSerialize();
	~CSerialize();

	 static string Encode(SL_ByteBuffer &content, string &key);
	 static SL_ByteBuffer Decode(string &content, string &key);
};
#endif