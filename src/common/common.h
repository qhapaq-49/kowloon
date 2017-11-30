#ifndef _INC_COMMON_H
#define _INC_COMMON_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <vector>
#include <thread>
#include <mutex>
#include <cmath>

extern std::mutex mtx;

// bit周りはohtoさんのコードの写経だ

// 整数定義
using std::int8_t;
using std::uint8_t;
using std::int16_t;
using std::uint16_t;
using std::int32_t;
using std::uint32_t;
using std::int64_t;
using std::uint64_t;


#if defined(_MSC_VER)

static inline int bsf32(uint32_t a)noexcept{
    unsigned long i;
    _BitScanForward(&i, a);
    return i;
}

static inline int bsf64(uint64_t a)noexcept{
    unsigned long i;
    _BitScanForward64(&i, a);
    return i;
}

static inline int bsr32(uint32_t a)noexcept{
    unsigned long i;
    _BitScanReverse(&i, a);
    return i;
}

static inline int bsr64(uint64_t a)noexcept{
    unsigned long i;
    _BitScanReverse64(&i, a);
    return i;
}

static inline int ctz32(uint32_t a)noexcept{
    return bsf32(a);
}

static inline int ctz64(uint64_t a)noexcept{
    return bsf64(a);
}

static inline int clz32(uint32_t a)noexcept{
    return 31 - bsr32(a);
}

static inline int clz64(uint64_t a)noexcept{
    return 63 - bsr64(a);
}

#elif defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) )

static inline int bsf32(uint32_t a)noexcept{
    return __builtin_ctz(a);
}

static inline int bsf64(uint64_t a)noexcept{
    return __builtin_ctzll(a);
}

static inline int bsr32(uint32_t a)noexcept{
    int r;
    __asm__("bsrl %1, %0;" :"=r"(r) : "r"(a));
    return r;
}

static inline int bsr64(uint64_t a)noexcept{
    int64_t r;
    __asm__("bsrq %1, %0;" :"=r"(r) : "r"(a));
    return (int)r;
}

static inline int ctz32(uint32_t a)noexcept{
    return __builtin_ctzl(a);
}

static inline int ctz64(uint64_t a)noexcept{
    return __builtin_ctzll(a);
}

static inline int clz32(uint32_t a)noexcept{
    return __builtin_clzl(a);
}

static inline int clz64(uint64_t a)noexcept{
    return __builtin_clzll(a);
}

#else

static_assert(0, "no bsf-bsr.");

#endif

#endif
