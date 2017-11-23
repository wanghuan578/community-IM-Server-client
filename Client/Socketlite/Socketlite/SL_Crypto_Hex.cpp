#include <string.h>
#include "SL_Crypto_Hex.h"

static char HEX_LOWER_CODES[] = "0123456789abcdef";
static char HEX_UPPER_CODES[] = "0123456789ABCDEF";

SL_Crypto_Hex::SL_Crypto_Hex()
{
	set_upper_case(true);
}

SL_Crypto_Hex::~SL_Crypto_Hex()
{

}

int SL_Crypto_Hex::get_need_len(unsigned int len, SL_Crypto_BaseN::TYPE type)
{
    if (SL_Crypto_BaseN::ENCODE == type)
	{//encode
		return len * 2;
	}
	else
	{//decode
		return len / 2;
	}
}

int SL_Crypto_Hex::encode(const unsigned char *input, unsigned int len, unsigned char *out, unsigned int *out_len)
{
	if (*out_len < len*2)
	{
		return -1;
	}

	int j = 0;
	unsigned int tmp_num = 0;
	for(unsigned int i=0; i<len; i++)
	{
		tmp_num  = input[i];
		out[j++] = hex_codes_[(tmp_num & 0xf0) >> 4];
		out[j++] = hex_codes_[tmp_num & 0x0f];
	}
	*out_len = j;
	return j;
}

int SL_Crypto_Hex::decode(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int *out_len)
{
	if (input_len / 2 > (*out_len))
	{
		return -1;
	}

	int j = 0;
	unsigned int tmp_num=0;
	for(unsigned int i=0; i<input_len; ++i)
	{
		tmp_num = 0;
		do
		{
			if (input[i] >= '0' && input[i] <= '9')
			{
				tmp_num += input[i]-'0';
			}
			else if (input[i] >= 'a' && input[i] <= 'f')
			{
				tmp_num += input[i]-'a'+10;
			}
			else if (input[i]>='A' && input[i]<='Z')
			{
				tmp_num += input[i]-'A'+10;
			}
			else
			{
				*out_len = 0;
				return -2;
			}

			if (i % 2 == 0)
			{
				tmp_num <<= 4;
				i++;
			}
			else
			{
				out[j++] = tmp_num;
				break;
			}
		} while (i % 2 == 1);
	}//for
	*out_len = j;

	return j;
}

void SL_Crypto_Hex::set_upper_case(bool upper_case)
{
	if (upper_case)
	{//letter upper
		hex_codes_ = HEX_UPPER_CODES;
	}
	else
	{//letter lower
		hex_codes_ = HEX_LOWER_CODES;
	}
}

