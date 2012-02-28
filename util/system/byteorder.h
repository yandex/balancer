#pragma once

#include "defaults.h"

//#define USE_GENERIC_ENDIAN_CVT

#if defined(_linux_) && !defined(USE_GENERIC_ENDIAN_CVT)
    #include <byteswap.h>
#elif defined(_darwin_)
    #include <machine/byte_order.h>
#else
    #include <util/generic/utility.h>
#endif

#if defined(_linux_) && !defined(USE_GENERIC_ENDIAN_CVT)
    #define SwapBytes16 bswap_16
    #define SwapBytes32 bswap_32
    #define SwapBytes64 bswap_64
#elif defined(_darwin_)
    #define SwapBytes16 OSSwapInt16
    #define SwapBytes32 OSSwapInt32
    #define SwapBytes64 OSSwapInt64
#else
    #if defined(_x86_) && defined(__GNUC__)
        #undef asm
        #define asm __asm__

        inline ui16 SwapBytes16(ui16 x) throw () {
            register ui16 val;

            asm (
                "rorw $8, %w0" : "=r" (val) : "0" (x) : "cc"
            );

            return val;
        }

        inline ui32 SwapBytes32(ui32 x) throw () {
            register ui32 val;

            asm (
                "bswap %0" : "=r" (val) : "0" (x)
            );

            return val;
        }

        #if defined(_x86_64_)
            inline ui64 SwapBytes64(ui64 x) throw () {
                register ui64 val;

                asm (
                    "bswapq %0" : "=r" (val) : "0" (x)
                );

                return val;
            }
        #endif

        #undef asm
    #else
        #define byte_n(__val, __n) ((((unsigned char*)(&__val))[ __n ]))

        inline ui16 SwapBytes16(ui16 val) throw () {
            DoSwap(byte_n(val, 0), byte_n(val, 1));

            return val;
        }

        inline ui32 SwapBytes32(ui32 val) throw () {
            DoSwap(byte_n(val, 0), byte_n(val, 3));
            DoSwap(byte_n(val, 1), byte_n(val, 2));

            return val;
        }

        #undef byte_n
    #endif

    #if !defined(_x86_64_) || defined _win64_
        inline ui64 SwapBytes64(ui64 val) throw () {
            union {
                ui64 val;
                ui32 p[2];
            } tmp, ret;

            tmp.val = val;
            ret.p[0] = SwapBytes32(tmp.p[1]);
            ret.p[1] = SwapBytes32(tmp.p[0]);

            return ret.val;
        }
    #endif
#endif

//for convinience
static inline ui8 SwapBytes8(ui8 v) throw () {
    return v;
}

namespace NSwapBytes {
    template <unsigned N>
    struct TSwapBytesHelper {
    };

#define DEF_SB(X)\
    template <>\
    struct TSwapBytesHelper<X> {\
        template <class T>\
        static inline T Swap(T t) throw () {\
            return (T)SwapBytes ## X((ui ## X)t);\
        }\
    };

    DEF_SB(8)
    DEF_SB(16)
    DEF_SB(32)
    DEF_SB(64)

#undef DEF_SB
}

template <class T>
inline T SwapBytes(T val) throw () {
    return NSwapBytes::TSwapBytesHelper<sizeof(T) * 8>::Swap(val);
}

template <class T>
inline T LittleToBig(T val) throw () {
    return SwapBytes(val);
}

template <class T>
inline T BigToLittle(T val) throw () {
    return LittleToBig(val);
}

template <class T>
inline T HostToInet(T val) throw () {
#if defined(_big_endian_)
    return val;
#elif defined(_little_endian_)
    return LittleToBig(val);
#else
    #error todo
#endif
}

template <class T>
inline T InetToHost(T val) throw () {
    return HostToInet(val);
}

template <class T>
inline T HostToLittle(T val) throw () {
#if defined(_big_endian_)
    return BigToLittle(val);
#elif defined(_little_endian_)
    return val;
#else
    #error todo
#endif
}

template <class T>
inline T LittleToHost(T val) throw () {
    return HostToLittle(val);
}
