#ifndef SOCKETLITE_CRYPTO_CIPHER_H
#define SOCKETLITE_CRYPTO_CIPHER_H

//密码加解密类型
enum SL_CRYPTO_TYPE
{
    SL_CRYPTO_TYPE_NOCRYPT  = 0x00,     //不启用密码算法
    SL_CRYPTO_TYPE_ENCRYPT  = 0x01,     //加密
    SL_CRYPTO_TYPE_DECRYPT  = 0x02,     //解密

    SL_CRYPTO_TYPE_ALL      = SL_CRYPTO_TYPE_ENCRYPT | SL_CRYPTO_TYPE_DECRYPT
};

//常见加密模式
enum SL_CRYPTO_MODE 
{ 
    SL_CRYPTO_MODE_ECB      = 0, 
    SL_CRYPTO_MODE_CBC      = 1, 
    SL_CRYPTO_MODE_CFB      = 2,
    SL_CRYPTO_MODE_OFB      = 3,
    SL_CRYPTO_MODE_CTR      = 4,
    SL_CRYPTO_MODE_IGE      = 5,

    SL_CRYPTO_MODE_DEFAULT  = SL_CRYPTO_MODE_ECB
};                           

//常见密码块大小(字节)
enum SL_CRYPTO_BLOCK_SIZE
{ 
    SL_CRYPTO_BLOCK_SIZE_8  = 8, 
    SL_CRYPTO_BLOCK_SIZE_16 = 16,
    SL_CRYPTO_BLOCK_SIZE_24 = 24,
    SL_CRYPTO_BLOCK_SIZE_32 = 32,

    SL_CRYPTO_BLOCK_SIZE_DEFAULT = SL_CRYPTO_BLOCK_SIZE_16
};

//常见密钥长度(字节)
enum SL_CRYPTO_KEY_LEN
{ 
    SL_CRYPTO_KEY_LEN_8     = 8,
    SL_CRYPTO_KEY_LEN_16    = 16,
    SL_CRYPTO_KEY_LEN_24    = 24,
    SL_CRYPTO_KEY_LEN_32    = 32,

    SL_CRYPTO_KEY_LEN_DEFAULT = SL_CRYPTO_KEY_LEN_16
};

class SL_Crypto_SymmetricCipher
{
public:	
    enum TYPE
    {
        ENCRYPT = 1,
        DECRYPT = 2
    };

    SL_Crypto_SymmetricCipher()
    {
    }

    virtual ~SL_Crypto_SymmetricCipher()
    {
    }

    virtual int get_need_len(unsigned int len, SL_Crypto_SymmetricCipher::TYPE type = SL_Crypto_SymmetricCipher::ENCRYPT)
    {
        return len;
    }

    virtual int init(const unsigned char *key, const unsigned char *ivec, int mode=0, int key_Len=16, int block_size=16)
    {
        return 0;
    }

    virtual int encrypt_block(const unsigned char *input, unsigned char *out)
    {
        return 0;
    }

    virtual int decrypt_block(const unsigned char *input, unsigned char *out)
    {
        return 0;
    }

    virtual int encrypt(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int out_len)
    {
        return 0;
    }

    virtual int decrypt(const unsigned char *input, unsigned int input_len, unsigned char *out, unsigned int out_len)
    {
        return 0;
    }
};

#endif

