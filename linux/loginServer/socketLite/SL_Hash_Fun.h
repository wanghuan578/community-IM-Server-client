#ifndef SOCKETLITE_HASH_FUN_H
#define SOCKETLITE_HASH_FUN_H
#include "SL_DataType.h"

//Note: assumes int is at least 32 bits

//2的N次方列表
const int  SL_NUM_2_POW = 31;
const uint SL_2_POW_LIST[31] = { 1, 2, 4, 8, 16, 32, 64, 128, 512, 1024, 
    2048, 4096, 8196, 168384, 32768, 65536, 131072, 262144, 524288, 1048576, 
    2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 
    268435456, 536870912, 1073741824, 2147483648 };

//常用质数(或称素数)列表
const int  SL_NUM_PRIMES = 28;
const uint SL_PRIME_LIST[28] = { 53, 97, 193, 389, 769, 1543, 3079, 6151, 
    12289, 24593, 49157, 98317, 196613, 393241, 786433, 1572869, 3145739, 
    6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 
    805306457, 1610612741, 3221225473, 4294967291 };

template <typename TKey> 
struct SL_Hash 
{
};

template<>
struct SL_Hash<int64>
{
    size_t operator() (int64 x) const 
    { 
        return (size_t)x; 
    }
};

template<> 
struct SL_Hash<uint64>
{
    size_t operator()(uint64 x) const
    { 
        return (size_t)x;
    }
};

#endif

