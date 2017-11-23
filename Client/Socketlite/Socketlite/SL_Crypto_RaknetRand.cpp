#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <math.h>
#include <string.h>
#include "SL_Crypto_RaknetRand.h"

#define RAKNET_RAND_N 				(624)     				// length of state vector
#define RAKNET_RAND_M               (397)                   // a period parameter
#define RAKNET_RAND_K               (0x9908B0DFU)           // a magic constant
#define RAKNET_RAND_hiBit(u)        ((u) & 0x80000000U)     // mask all but highest   bit of u
#define RAKNET_RAND_loBit(u)        ((u) & 0x00000001U)     // mask all but lowest    bit of u
#define RAKNET_RAND_loBits(u)       ((u) & 0x7FFFFFFFU)     // mask  the highest   bit of u
#define RAKNET_RAND_mixBits(u, v)   (RAKNET_RAND_hiBit(u)|RAKNET_RAND_loBits(v))    // move hi bit of u to hi bit of v

SL_Crypto_RaknetRand::SL_Crypto_RaknetRand()
{
	left_ = -1;
	seed();
}

SL_Crypto_RaknetRand::~SL_Crypto_RaknetRand()
{
}

int SL_Crypto_RaknetRand::random_byte(char *out, int out_len, SL_Crypto_Rand::TYPE type, const char *special_char)
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
	else if (type == SL_Crypto_Rand::ENLARGE_CHAR || special_char != NULL )
	{
		fill_buffer_mt(out, out_len, special_char);
	}
	else
	{
		fill_buffer_mt(out, out_len, SL_Crypto_Rand::STANDARD_CHAR);
	}
	return out_len;
}

int SL_Crypto_RaknetRand::random_byte(char *out, int out_len, int low, int high)
{
    int r;
    for (int i=0; i<out_len; ++i)
    {
        r = random_mt(low, high);
        *(out + i) = r;
    }
    return out_len;
}

int SL_Crypto_RaknetRand::random_int()
{
	int ret = random_mt();
	return abs(ret);
}

unsigned int SL_Crypto_RaknetRand::random_mt()
{
	if (--left_ < 0)
	{
		return (reload_mt());
	}

	unsigned int y;
	y = *next_++;
	y ^= ( y >> 11 );
	y ^= ( y << 7 ) & 0x9D2C5680U;
	y ^= ( y << 15 ) & 0xEFC60000U;
	return ( y ^ ( y >> 18 ) );

	// This change made so the value returned is in the same range as what rand() returns
	// return(y ^ (y >> 18)) % 32767;
}

unsigned int SL_Crypto_RaknetRand::random_mt(int low, int how)
{
	if (low == how)
	{
		return low;
	}

	unsigned int ret;
	ret = random_mt();
	ret = low  + ret % (abs(how - low));
	return ret;
}

int SL_Crypto_RaknetRand::random_int(int low, int high)
{
	return random_mt(low, high);
}

double SL_Crypto_RaknetRand::random_double()
{
	return frandom_mt();
}

double SL_Crypto_RaknetRand::random_double(double low, double high)
{
	if ( low > high )
	{
		double temp;
		temp = low;
		low  = high;
		high = temp;
	}
	return low + frandom_mt()*(high - low);
}

int SL_Crypto_RaknetRand::seed(int seed)
{
	seed_mt(seed);
	return 1;
}
void SL_Crypto_RaknetRand::seed()
{
	::srand((unsigned int)::time(NULL));
	seed(rand());
}

void SL_Crypto_RaknetRand::seed_mt(unsigned int seed)   // Defined in cokus_c.c
{
	//
	// We initialize state[0..(N-1)] via the generator
	//
	//  x_new = (69069 * x_old) mod 2^32
	//
	// from Line 15 of Table 1, p. 106, Sec. 3.3.4 of Knuth's
	// _The Art of Computer Programming_, Volume 2, 3rd ed.
	//
	// Notes (SJC): I do not know what the initial state requirements
	// of the Mersenne Twister are, but it seems this seeding generator
	// could be better.  It achieves the maximum period for its modulus
	// (2^30) iff x_initial is odd (p. 20-21, Sec. 3.2.1.2, Knuth); if
	// x_initial can be even, you have sequences like 0, 0, 0, ...;
	// 2^31, 2^31, 2^31, ...; 2^30, 2^30, 2^30, ...; 2^29, 2^29 + 2^31,
	// 2^29, 2^29 + 2^31, ..., etc. so I force seed to be odd below.
	//
	// Even if x_initial is odd, if x_initial is 1 mod 4 then
	//
	//  the   lowest bit of x is always 1,
	//  the  next-to-lowest bit of x is always 0,
	//  the 2nd-from-lowest bit of x alternates   ... 0 1 0 1 0 1 0 1 ... ,
	//  the 3rd-from-lowest bit of x 4-cycles   ... 0 1 1 0 0 1 1 0 ... ,
	//  the 4th-from-lowest bit of x has the 8-cycle ... 0 0 0 1 1 1 1 0 ... ,
	//   ...
	//
	// and if x_initial is 3 mod 4 then
	//
	//  the   lowest bit of x is always 1,
	//  the  next-to-lowest bit of x is always 1,
	//  the 2nd-from-lowest bit of x alternates   ... 0 1 0 1 0 1 0 1 ... ,
	//  the 3rd-from-lowest bit of x 4-cycles   ... 0 0 1 1 0 0 1 1 ... ,
	//  the 4th-from-lowest bit of x has the 8-cycle ... 0 0 1 1 1 1 0 0 ... ,
	//   ...
	//
	// The generator's potency (min. s>=0 with (69069-1)^s = 0 mod 2^32) is
	// 16, which seems to be alright by p. 25, Sec. 3.2.1.3 of Knuth.  It
	// also does well in the dimension 2..5 spectral tests, but it could be
	// better in dimension 6 (Line 15, Table 1, p. 106, Sec. 3.3.4, Knuth).
	//
	// Note that the random number user does not see the values generated
	// here directly since reloadMT() will always munge them first, so maybe
	// none of all of this matters.  In fact, the seed values made here could
	// even be extra-special desirable if the Mersenne Twister theory says
	// so-- that's why the only change I made is to restrict to odd seeds.
	//

	register unsigned int x = ( seed | 1U ) & 0xFFFFFFFFU, *s = state_;
	register int j;

	for ( left_ = 0, *s++ = x, j = RAKNET_RAND_N; --j; *s++ = ( x *= 69069U ) & 0xFFFFFFFFU);
}

unsigned int SL_Crypto_RaknetRand::reload_mt()
{
	register unsigned int * p0 = state_, *p2 = state_ + 2, *pM = state_ + RAKNET_RAND_M, s0, s1;
	register int j;

	if (left_ < -1)
	{
		seed_mt(4357U);
	}

	left_ = RAKNET_RAND_N - 1, next_ = state_ + 1;

	for (s0 = state_[ 0 ], s1 = state_[ 1 ], j = RAKNET_RAND_N - RAKNET_RAND_M + 1; --j; s0 = s1, s1 = *p2++)
	{
		* p0++ = *pM++ ^ ( RAKNET_RAND_mixBits( s0, s1 ) >> 1 ) ^ ( RAKNET_RAND_loBit( s1 ) ? RAKNET_RAND_K : 0U );
	}

	for (pM = state_, j = RAKNET_RAND_M; --j; s0 = s1, s1 = *p2++)
	{
		* p0++ = *pM++ ^ ( RAKNET_RAND_mixBits( s0, s1 ) >> 1 ) ^ ( RAKNET_RAND_loBit( s1 ) ? RAKNET_RAND_K : 0U );
	}

	s1 = state_[ 0 ], *p0 = *pM ^ ( RAKNET_RAND_mixBits( s0, s1 ) >> 1 ) ^ ( RAKNET_RAND_loBit( s1 ) ? RAKNET_RAND_K : 0U );
	s1 ^= ( s1 >> 11 );
	s1 ^= ( s1 << 7 ) & 0x9D2C5680U;
	s1 ^= ( s1 << 15 ) & 0xEFC60000U;
	return ( s1 ^ ( s1 >> 18 ) );
}

char SL_Crypto_RaknetRand::random_mt_char()
{
	char ret;
	ret = random_mt(48, 122);
	if (ret >=58 && ret <= 64)
	{
		ret =random_mt(48, 57);
	}
	if (ret >= 91 && ret <= 96)
	{
		ret = random_mt(97, 122);
	}
	return ret;
}

char SL_Crypto_RaknetRand::random_mt_char(const char *special_char)
{
	char ret;
	ret = random_mt(48, 122);
	if (ret >=58 && ret <= 64)
	{
		int len = (int)strlen(special_char);
		int char_num = random_mt(0, len-1);
		ret = special_char[char_num];
	}
	if (ret >= 91 && ret <= 96)
	{
		int len = (int)strlen(special_char);
		int char_num = random_mt(0, len-1);
		ret = special_char[char_num];
	}

	return ret;
}

float SL_Crypto_RaknetRand::frandom_mt()
{
	return (float) ((double) random_mt() / 4294967296.0);
}

void SL_Crypto_RaknetRand::fill_buffer_mt(char *buffer, unsigned int bytes)
{
	unsigned int r;
    for (unsigned int i=0; i<bytes; ++i)
    {
		r = random_mt(0, 255);
	    *(buffer + i) = r;
    }
}

void SL_Crypto_RaknetRand::fill_buffer_mt(char *buffer, unsigned int bytes, int type)
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
                r = random_mt(32, 126);
                *(buffer + i) = r;
            }
            break;
        case SL_Crypto_Rand::NUMBER_CHAR:
            for (unsigned int i=0; i<bytes; ++i)
            {
                r = random_mt(48, 57);
                *(buffer + i) = r;
            }
            break;
        case SL_Crypto_Rand::UPPERCASE_CHAR:
            for (unsigned int i=0; i<bytes; ++i)
            {
                r = random_mt(65, 90);
                *(buffer + i) = r;
            }
            break;
        case SL_Crypto_Rand::LOWERCASE_CHAR:
            for (unsigned int i=0; i<bytes; ++i)
            {
                r = random_mt(97, 112);
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

void SL_Crypto_RaknetRand::fill_buffer_mt(char *buffer, unsigned int bytes, const char *special_char)
{
	unsigned int r;
    for (unsigned int i=0; i<bytes; ++i)
    {
		r = random_mt_char(special_char);
	    *(buffer + i) = r;
    }
}

