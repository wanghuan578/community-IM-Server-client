#ifndef SOCKETLITE_CRYPTO_RAND_H
#define SOCKETLITE_CRYPTO_RAND_H

class SL_Crypto_Rand
{
public:
    enum TYPE
    {
        RANDOM_CHAR = 0,        // 按unsinged char 随机生成内存块
        VISIBLE_CHAR,	        // 按char 随机生成 可显字符串(键盘可以输入)
        STANDARD_CHAR,	        // 按char 随机生成 标准字符串(不包括特殊字符串)
        ENLARGE_CHAR,           // 按char 随机生成 增强型标准字符串(包括自定义的特殊字符)
        NUMBER_CHAR,            // 按char 随机生成 纯数字(0-9)
        LOWERCASE_CHAR,         // 按char 随机生成 小写字母(a-z)
        UPPERCASE_CHAR,         // 按char 随机生成 大写字母(A-Z)

        TYPE_NUMS,
    };

    SL_Crypto_Rand()
    {
    }

    virtual ~SL_Crypto_Rand()
    {
    }

    virtual void seed() = 0;
    virtual int  seed(int seed) = 0;

    virtual int random_byte(char *out, int out_len, SL_Crypto_Rand::TYPE type, const char *special_char) = 0;
    virtual int random_byte(char *out, int out_len, int low, int high) = 0;
    virtual int random_int() = 0;
    virtual int random_int(int low, int high) = 0;
    virtual double random_double() = 0;
    virtual double random_double(double low, double high) = 0;
};

#endif

