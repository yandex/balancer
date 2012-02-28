#pragma once

#include <util/system/yassert.h>
#include <util/generic/stroka.h>

inline static char DigitToChar(unsigned char digit) {
    if (digit < 10) {
        return (char)digit + '0';
    }

    return (char)(digit - 10) + 'A';
}

inline static int Char2Digit(char ch) {
    STATIC_ASSERT('9' < 'A');
    STATIC_ASSERT('F' < 'a');

    if (ch <= '9') {
        return ch - '0';
    }

    if (ch <= 'F') {
        return ch - 'A' + 10;
    }

    return ch - 'a' + 10;
}

inline static int Stroka2Byte(const char* s) {
    return Char2Digit(*s) * 16 + Char2Digit(*(s + 1));
}

inline static char* HexEncode(const void* in, size_t len, char* out) {
    const unsigned char* b = (const unsigned char*)in;
    const unsigned char* e = b + len;

    while (b != e) {
        *out++ = DigitToChar(*b / 16);
        *out++ = DigitToChar(*b++ % 16);
    }

    return out;
}

inline static void* HexDecode(const void* in, size_t len, void* ptr) {
    const char* b = (const char*)in;
    const char* e = b + len;

    char* out = (char*)ptr;

    while (b != e) {
        *out++ = (char)Stroka2Byte(b);
        b += 2;
    }

    return out;
}

static inline Stroka HexEncode(const Stroka& h) {
    Stroka ret;

    ret.ReserveAndResize(+h << 1);
    HexEncode(~h, +h, ret.begin());

    return ret;
}

static inline Stroka HexEncode(const void* in, size_t len) {
    Stroka ret;

    ret.ReserveAndResize(len << 1);
    HexEncode(in, len, ret.begin());

    return ret;
}

static inline Stroka HexDecode(const Stroka& h) {
    Stroka ret;

    ret.ReserveAndResize(+h >> 1);
    HexDecode(~h, +h, ret.begin());

    return ret;
}
