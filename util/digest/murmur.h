#pragma once

/*
 * From http://murmurhash.googlepages.com/
 */

#include <util/system/defaults.h>

#include "iterator.h"

#define MURMUR_HASH_SEED 0

namespace NMurmurPrivate {
    static inline ui32 MurmurHash32(const void* key, size_t len, ui32 seed) throw () {
        const ui32 m = 0x5bd1e995;
        const int r = 24;
        ui32 h = ui32(seed ^ len);

        TUnalignedMemoryIterator<ui32> iter(key, len);

        while (!iter.AtEnd()) {
            ui32 k = iter.Next();

            k *= m;
            k ^= k >> r;
            k *= m;

            h *= m;
            h ^= k;
        }

        const unsigned char* data = iter.Last();

        switch (iter.Left()) {
            case 3:
                h ^= data[2] << 16;

            case 2:
                h ^= data[1] << 8;

            case 1:
                h ^= data[0];
                h *= m;
        };

        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h;
    }

    //-----------------------------------------------------------------------------
    // MurmurHash2, 64-bit versions, by Austin Appleby

    // The same caveats as 32-bit MurmurHash2 apply here - beware of alignment
    // and endian-ness issues if used across multiple platforms.

    // 64-bit hash for 64-bit platforms

    static inline ui64 MurmurHash64(const void* key, size_t len, ui64 seed) {
        const ui64 m = ULL(0xc6a4a7935bd1e995);
        const int r = 47;

        ui64 h = seed ^ (len * m);
        TUnalignedMemoryIterator<ui64> iter(key, len);

        while (!iter.AtEnd()) {
            ui64 k = iter.Next();

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;
        }

        const unsigned char* data2 = iter.Last();

        switch (iter.Left()) {
            case 7:
                h ^= ui64(data2[6]) << 48;

            case 6:
                h ^= ui64(data2[5]) << 40;

            case 5:
                h ^= ui64(data2[4]) << 32;

            case 4:
                h ^= ui64(data2[3]) << 24;

            case 3:
                h ^= ui64(data2[2]) << 16;

            case 2:
                h ^= ui64(data2[1]) << 8;

            case 1:
                h ^= ui64(data2[0]);
                h *= m;
        }

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }

    template <unsigned N>
    struct TMurHelper;

#define DEF_MUR(t)\
    template <>\
    struct TMurHelper<t> {\
        static const ui ## t Init = MURMUR_HASH_SEED;\
        static inline ui ## t MurmurHash(const void* buf, size_t len, ui ## t init) { \
            return MurmurHash ## t (buf, len, init);                        \
        }\
    };

    DEF_MUR(32)
    DEF_MUR(64)

#undef DEF_MUR
}

template <class T>
static inline T MurmurHash(const void* buf, size_t len, T init) {
    return (T)NMurmurPrivate::TMurHelper<8 * sizeof(T)>::MurmurHash(buf, len, init);
}

template <class T>
static inline T MurmurHash(const void* buf, size_t len) {
    return MurmurHash<T>(buf, len, (T)NMurmurPrivate::TMurHelper<8 * sizeof(T)>::Init);
}

#undef MIX
#undef MURMUR_HASH_SEED
