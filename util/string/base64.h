#pragma once

#include <util/system/defaults.h>
#include <util/generic/strbuf.h>

#define BASE64_DECODE_BUF_SIZE(len) ((len + 3) / 4 * 3)
#define BASE64_ENCODE_BUF_SIZE(len) ((len + 2) / 3 * 4 + 1)

//decode
inline size_t Base64DecodeBufSize(size_t len) {
    return BASE64_DECODE_BUF_SIZE(len);
}

size_t Base64Decode(void* dst, const char* b, const char* e);

inline TStringBuf Base64Decode(const TStringBuf& src, void* tmp) {
    return TStringBuf((const char*)tmp, Base64Decode(tmp, src.begin(), src.end()));
}

inline Stroka Base64Decode(const TStringBuf& s) {
    Stroka ret;

    ret.ReserveAndResize(Base64DecodeBufSize(+s));
    ret.resize(+Base64Decode(s, ret.begin()));

    return ret;
}

//encode
inline size_t Base64EncodeBufSize(size_t len) {
    return BASE64_ENCODE_BUF_SIZE(len);
}

char* Base64Encode(char* outstr, const unsigned char* instr, size_t len);
char* Base64EncodeUrl(char* outstr, const unsigned char* instr, size_t len);

inline TStringBuf Base64Encode(const TStringBuf& src, void* tmp) {
    return TStringBuf((const char*)tmp, Base64Encode((char*)tmp, (const unsigned char*)~src, +src));
}

inline TStringBuf Base64EncodeUrl(const TStringBuf& src, void* tmp) {
    return TStringBuf((const char*)tmp, Base64EncodeUrl((char*)tmp, (const unsigned char*)~src, +src));
}

inline Stroka Base64Encode(const TStringBuf& s) {
    Stroka ret;

    ret.ReserveAndResize(Base64EncodeBufSize(+s));
    ret.resize(+Base64Encode(s, ret.begin()));

    return ret;
}

inline Stroka Base64EncodeUrl(const TStringBuf& s) {
    Stroka ret;

    ret.ReserveAndResize(Base64EncodeBufSize(+s));
    ret.resize(+Base64EncodeUrl(s, ret.begin()));

    return ret;
}
