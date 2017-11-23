#ifndef SOCKETLITE_CRYPTO_RAKNET_RAND_H
#define SOCKETLITE_CRYPTO_RAKNET_RAND_H
#include "SL_Crypto_Rand.h"

class SL_Crypto_RaknetRand : public SL_Crypto_Rand
{
	/// Initialise seed for Random Generator
	/// \param[in] seed The seed value for the random number generator.
	//extern void RAK_DLL_EXPORT seed_mt( unsigned int seed );
public:
    SL_Crypto_RaknetRand();
    virtual ~SL_Crypto_RaknetRand();

    int  seed(int seed);
    void seed();

    int random_byte(char *out, int out_len, SL_Crypto_Rand::TYPE type=SL_Crypto_Rand::RANDOM_CHAR, const char *special_char=NULL);
    int random_byte(char *out, int out_len, int low, int high);
    int random_int();
    int random_int(int low, int high);
    double random_double();
    double random_double(double low, double high);

protected:
    void seed_mt(unsigned int seed);

    /// internal
    //extern unsigned int RAK_DLL_EXPORT reload_mt( void );
    unsigned int reload_mt();

    /// Gets a random unsigned int
    /// return an integer random value.
    unsigned int random_mt();
    unsigned int random_mt(int low, int how);

    char random_mt_char();                          // 生成标准字符
    char random_mt_char(const char *special_char);  // 按特殊字符(specialchar)生成 增强型字符

    /// Gets a random float
    /// return 0 to 1.0f, inclusive
    float frandom_mt();

    /// Randomizes a buffer
    void fill_buffer_mt(char *buffer, unsigned int bytes);
    /// Randomizes a buffer by type
    void fill_buffer_mt(char *buffer, unsigned int bytes, int type);
    void fill_buffer_mt(char *buffer, unsigned int bytes, const char *special_char);

    unsigned int	state_[625];        // state vector(624) + 1 extra to not violate ANSI C
    unsigned int	*next_;             // next random value is computed from here
    int				left_;              // can *next++ this many times before reloading
};

#endif

