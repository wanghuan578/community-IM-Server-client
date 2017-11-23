#include <string.h>
#include "SL_Crypto_Base64.h"

static const char *BASE64_CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const unsigned char BASE64_MAP[256] = {
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
	52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
	255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
	7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
	19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
	255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
	37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
	49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255 };

SL_Crypto_Base64::SL_Crypto_Base64()
{
}

SL_Crypto_Base64::~SL_Crypto_Base64()
{
}

int SL_Crypto_Base64::get_need_len(unsigned int len, SL_Crypto_BaseN::TYPE type)
{
    if (SL_Crypto_BaseN::ENCODE == type)
	{//encode
		return  (len + 2) * 4 / 3 + 1;
	}
	else
	{//decode
		return  len * 3 / 4 + 1;
	}
}

int SL_Crypto_Base64::encode(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int *out_len)
{
	unsigned long i, len2, leven;
	unsigned char *p;

	/* valid output size ? */
	len2 = (input_len + 2) * 4 / 3;
	if (*out_len < len2 + 1) 
	{
		return -2;
	}
	p = out;
	leven = (input_len / 3) * 3;
	for (i = 0; i < leven; i += 3) 
	{
		*p++ = BASE64_CODES[input[0] >> 2];
		*p++ = BASE64_CODES[((input[0] & 3) << 4) + (input[1] >> 4)];
		*p++ = BASE64_CODES[((input[1] & 0xf) << 2) + (input[2] >> 6)];
		*p++ = BASE64_CODES[input[2] & 0x3f];
		input += 3;
	}
	/* Pad it if necessary...  */
	if (i < input_len) 
	{
		unsigned a = input[0];
		unsigned b = (i+1 < input_len) ? input[1] : 0;
		unsigned c = 0;

		*p++ = BASE64_CODES[a >> 2];
		*p++ = BASE64_CODES[((a & 3) << 4) + (b >> 4)];
		*p++ = (i+1 < input_len) ? BASE64_CODES[((b & 0xf) << 2) + (c >> 6)] : '=';
		*p++ = '=';
	}

	/* append a NULL byte */
	*p = '\0';

	/* return ok */
	*out_len = p - out;
	return *out_len;
}

int SL_Crypto_Base64::decode(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int *out_len)
{
	unsigned long t, x, y, z;
	unsigned char c;
	int	g = 3;

	for (x = y = z = t = 0; x < input_len; x++) 
	{
		c = BASE64_MAP[input[x]];
		if (c == 255)
        {
            continue;
        }
		if (c == 254) 
		{ 
			c = 0;
			--g;
		}
		t = (t << 6) | c;
		if (++y == 4)
		{
			if (z + g > *out_len) 
			{ 
				return -1;
			}
			out[z++] = (unsigned char)((t >> 16) & 255);
			if (g > 1)
            {
                out[z++] = (unsigned char)((t >> 8) & 255);
            }
			if (g > 2)
            {
                out[z++] = (unsigned char)(t & 255);
            }
			y = t = 0;
		}
	}
	if (y != 0) 
	{
		return -2;
	}
	*out_len = z;
	return z;
}

