/*
* File: random.h
* Version: 1.0
* Last modified on Fri Jul 22 16:44:36 1994 by eroberts
* -----------------------------------------------------
* This interface provides several functions for generating
* pseudo-random numbers.
*/

#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>
#include <string.h>
#include "SL_Crypto_CRTRand.h"


#ifndef RAND_MAX
#define RAND_MAX ((int) ((unsigned) ~0 >> 1))
#endif

SL_Crypto_CRTRand::SL_Crypto_CRTRand()
{
	seed();
}

SL_Crypto_CRTRand::~SL_Crypto_CRTRand()
{
}

int SL_Crypto_CRTRand::random_byte(char *out, int out_len, SL_Crypto_Rand::TYPE type, const char *special_char)
{
    if (type == SL_Crypto_Rand::RANDOM_CHAR)
    {
        fill_buffer_mt(out, out_len);
    }
    else if (type==SL_Crypto_Rand::VISIBLE_CHAR || \
        type==SL_Crypto_Rand::STANDARD_CHAR || \
        type==SL_Crypto_Rand::NUMBER_CHAR || \
        type==SL_Crypto_Rand::UPPERCASE_CHAR || \
        type==SL_Crypto_Rand::LOWERCASE_CHAR)
    {
        fill_buffer_mt(out, out_len, type);
    }
    else if (type==SL_Crypto_Rand::ENLARGE_CHAR || special_char != NULL )
    {
        fill_buffer_mt(out, out_len, special_char);
    }
    else
    {
        fill_buffer_mt(out, out_len, SL_Crypto_Rand::STANDARD_CHAR);
    }

	return out_len;
}

int SL_Crypto_CRTRand::random_byte(char *out, int out_len, int low, int high)
{
    int r;
    for (int i=0; i<out_len; ++i)
    {
        r = random_int(low, high);
        *(out + i) = r;
    }
    return out_len;
}

int SL_Crypto_CRTRand::random_int()
{
	int ret = random_integer();
	if (ret > RAND_MAX/2)
	{
		ret = ret<<16 | ret;
	}
	return ret;
}

int SL_Crypto_CRTRand::random_int(int low, int high)
{
    if (high-low > 0)
    {
        int ret = random_int();
	    ret = ret % (high-low) + low;
        return ret;
    }
    return low;
}

double SL_Crypto_CRTRand::random_double()
{
	return random_real();
}

double SL_Crypto_CRTRand::random_double(double low, double high)
{
	if (low > high)
	{
		double temp;
		temp = low;
		low  = high;
		high = temp;
	}
    return low + random_real()*(high - low);
}

int SL_Crypto_CRTRand::seed(int seed)
{
	randomize(seed);
	return 1;
}

void SL_Crypto_CRTRand::seed()
{
	::srand((unsigned int)::time(NULL));
    seed(rand());
}

/*
* Function: randomize
* -------------------
* This function operates by setting the random number
* seed to the current time.  The srand function is
* provided by the <stdlib.h> library and requires an
* integer argument.  The time function is provided
* by <time.h>.
*/
void SL_Crypto_CRTRand::randomize(int seed)
{
	if (seed == 0)
    {
		::srand((unsigned int)::time(NULL));
    }
	else
    {
	    ::srand(seed);
    }
}

/*
* Function: randomInteger
* -----------------------
* This function first obtains a random integer in
* the range [0..RAND_MAX] by applying four steps:
* (1) Generate a real number between 0 and 1.
* (2) Scale it to the appropriate range size.
* (3) Truncate the value to an integer.
* (4) Translate it to the appropriate starting point.
*/
int SL_Crypto_CRTRand::random_integer()
{
	return ::rand();
}

int SL_Crypto_CRTRand::random_integer(int low, int high)
{
	if (high < low)
	{
		int tmp = high;
		high = low;
		low  = tmp;
	};

	int k;
	double d;
	d = (double) (::rand() / ((double)RAND_MAX + 1));
	k = (int) (d * (high - low + 1));
	return (low + k);
}

/*
* Function: randomReal
* --------------------
* The implementation of RandomReal is similar to that
* of RandomInteger, without the truncation step.
*/
double SL_Crypto_CRTRand::random_real()
{
	return (double)(::rand() / ((double)RAND_MAX + 1));
}

double SL_Crypto_CRTRand::random_real(double low, double high)
{
	if (high < low)
	{
		double tmp = high;
		high = low;
		low  = tmp;
	};

	double d;
	d = (double)(::rand() / ((double)RAND_MAX + 1));
	return (low + d * (high - low));
}

/*
* Function: randomChance
* ----------------------
* This function uses RandomReal to generate a number
* between 0 and 100, which it then compares to p.
*/
bool SL_Crypto_CRTRand::random_chance(double p)
{
	return (random_real(0, 1) < p);
}

char SL_Crypto_CRTRand::random_mt_char(const char *special_char)
{
	char ret;
	ret = random_integer(48, 122);
	if (ret >=58 && ret <= 64)
	{
		int len = (int)strlen(special_char);
		int char_num = random_integer(0, len-1);
		ret = special_char[char_num];
	}

	if (ret >= 91 && ret <= 96)
	{
		int len = (int)strlen(special_char);
		int char_num = random_integer(0, len-1);
		ret = special_char[char_num];
	}

	return ret;
}

char SL_Crypto_CRTRand::random_mt_char()
{
	char ret;
	ret = random_integer(48, 122);
	if (ret >=58 && ret <= 64)
	{
		ret = random_integer(48, 57);
	}
	if (ret >= 91 && ret <= 96)
	{
	    ret = random_integer(97, 122);
	}
	return ret;
}

void SL_Crypto_CRTRand::fill_buffer_mt(char *buffer, unsigned int bytes)
{
	unsigned int r;
    for (unsigned int i=0; i<bytes; ++i)
    {
		r = random_integer(0, 255);
	    *(buffer + i) = r;
    }
}

void SL_Crypto_CRTRand::fill_buffer_mt(char *buffer, unsigned int bytes, int type)
{
	if ((type < SL_Crypto_Rand::RANDOM_CHAR) || (type >= SL_Crypto_Rand::TYPE_NUMS))
	{
		type = SL_Crypto_Rand::STANDARD_CHAR;
	}

	unsigned int r;
    switch (type)
    {
        case SL_Crypto_Rand::VISIBLE_CHAR:
            for (unsigned int i=0; i<bytes; ++i)
            {
                r = random_integer(32, 126);
                *(buffer + i) = r;
            }
            break;
        case SL_Crypto_Rand::NUMBER_CHAR:
            for (unsigned int i=0; i<bytes; ++i)
            {
                r = random_integer(48, 57);
                *(buffer + i) = r;
            }
            break;
        case SL_Crypto_Rand::UPPERCASE_CHAR:
            for (unsigned int i=0; i<bytes; ++i)
            {
                r = random_integer(65, 90);
                *(buffer + i) = r;
            }
            break;
        case SL_Crypto_Rand::LOWERCASE_CHAR:
            for (unsigned int i=0; i<bytes; ++i)
            {
                r = random_integer(97, 112);
                *(buffer + i) = r;
            }
            break;
        default:
            for (unsigned int i=0; i<bytes; ++i)
            {
                r = random_mt_char();
                *(buffer + i) = r;
            }
            break;
    }
}

void SL_Crypto_CRTRand::fill_buffer_mt(char *buffer, unsigned int bytes, const char *special_char)
{
	unsigned int r;
    for (unsigned int i=0; i<bytes; ++i)
    {
		r = random_mt_char(special_char);
	    *(buffer + i) = r;
    }
}

