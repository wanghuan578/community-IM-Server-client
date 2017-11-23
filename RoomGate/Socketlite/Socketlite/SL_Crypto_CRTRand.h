#ifndef SOCKETLIETE_CRYPTO_CRTRAND_H
#define SOCKETLIETE_CRYPTO_CRTRAND_H
#include "SL_Crypto_Rand.h"

class SL_Crypto_CRTRand : public SL_Crypto_Rand
{
public:
    SL_Crypto_CRTRand();
    virtual ~SL_Crypto_CRTRand();

    int  seed(int seed);
    void seed();

    int random_byte(char *out, int out_len, SL_Crypto_Rand::TYPE type=SL_Crypto_Rand::RANDOM_CHAR, const char *special_char=NULL);
    int random_byte(char *out, int out_len, int low, int high);
    int random_int();
    int random_int(int low, int high);
    double random_double();
    double random_double(double low, double high);

protected:
    /*
    * Function: randomize
    * Usage: randomize();
    * -------------------
    * This function sets the random seed so that the random sequence
    * is unpredictable.  During the debugging phase, it is best not
    * to call this function, so that program behavior is repeatable.
    */
    void randomize(int seed = 0);

    /*
    * Function: random_integer
    * Usage: n = random_integer(low, high);
    * ------------------------------------
    * This function returns a random integer in the range low to high,
    * inclusive.
    */
    int random_integer(); //random number range [0,RAND_MAX]
    int random_integer(int low, int high);

    /*
    * Function: random_real
    * Usage: d = random_real(low, high);
    * ---------------------------------
    * This function returns a random real number in the half-open
    * interval [low .. high), meaning that the result is always
    * greater than or equal to low but strictly less than high.
    */
    double random_real(); //random number range [0,1]
    double random_real(double low, double high);

    /*
    * Function: random_chance
    * Usage: if (random_chance(p)) . . .
    * ---------------------------------
    * The random_chance function returns TRUE with the probability
    * indicated by p, which should be a floating-point number between
    * 0 (meaning never) and 1 (meaning always).  For example, calling
    * random_chance(.30) returns TRUE 30 percent of the time.
    */
    bool random_chance(double p);

    void fill_buffer_mt(char *buffer, unsigned int bytes);

    /// Randomizes a buffer by type
    void fill_buffer_mt(char *buffer, unsigned int bytes, int type);

    void fill_buffer_mt(char *buffer, unsigned int bytes, const char *special_char);

    char random_mt_char();                          // 生成标准字符
    char random_mt_char(const char* special_char);  // 按特殊字符(specialchar)生成 增强型字符
};

#endif

