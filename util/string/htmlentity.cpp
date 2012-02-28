#include "util.h"
#include "htmlentity.h"

#include <cstring>
#include <cstdlib>

#include <util/system/defaults.h>
#include <util/charset/recyr.hh>
#include <util/charset/codepage.h>

#if 0
#   define C_LOCALE 1
#endif

#if defined (C_LOCALE)
#   include <ctype.h>
#   include <locale.h>
#   include <assert.h>
#else
#   define islower(c)  ('a'<=(c)&&(c)<='z')
#   define toupper(c)  (islower(c)?(c)-32:(c))
#   define isalpha(c)  ('a'<=(c)&&(c)<='z'||'A'<=(c)&&(c)<='Z')
#   define isdigit(c)  ('0'<=(c)&&(c)<='9')
#   define isalnum(c)  (isalpha(c)||isdigit(c))
#   define isxdigit(c) ('a'<=(c)&&(c)<='f'||'A'<=(c)&&(c)<='F'||'0'<=(c)&&(c)<='9')
#endif

#define xdigit(c) (isdigit(c) ? (c)-'0' : islower(c) ? (c)-'a'+10 : (c)-'A'+10)
#define digit(c)  ((c)-'0')

#ifdef __GNUC__
#   ifdef __inline
#       undef __inline
#   endif
#   define __inline
#endif

size_t HtEntDecode(ECharset cp, const char* str, size_t len, wchar32* buf, unsigned char* map) {
#if defined (C_LOCALE)
    assert(strcmp(setlocale(LC_CTYPE, NULL), "C") == 0);
#endif
    const unsigned char *s = (const unsigned char *)str;
    const unsigned char *end = (const unsigned char *)(str + len);
    size_t ret = 0;
    while (s < end) {
        *buf++ = HtEntDecodeStep(cp, s, end, &map);
        ret++;
    }
    return ret;
}

#define TEST_CHAR_AT(condition,i) (s < (end-i) && condition(s[i]))

inline void DetectEntity(const unsigned char*& s, const unsigned char* end, size_t& clen, const THtmlEntity*& p) {
    // __012345678__
    // __&aa......__

    // to do: reorganize 'hash' and 'in_word_set' functions into one
    //        incrementally calculate 'hash'
    //        and call 'in_word_set' on each iteration
    for (clen = 3; clen < 9 && TEST_CHAR_AT(isalnum, clen); clen++)
        ;
    while (clen > 2 && (p = SelectHtmlEntity((const char*)s+1, clen-1)) == 0)
        --clen;
}

inline void SetEntity(const unsigned char*& s, const unsigned char* end, const THtmlEntity*& p, size_t& clen, wchar32& c) {
    assert(3 <= clen && clen <= 9);
    c = p->Unicode;
    clen += TEST_CHAR_AT(';'==, clen);
}

inline void SetAmp(size_t& clen, wchar32& c) {
    clen = 1;
    c = '&';
    // SetBadness(HTML_BAD_ENTITY_NAME);
}

inline void FixBadNumber(wchar32& c) {
    if (c == 0)
        c = BROKEN_RUNE;
#ifndef MICROSOFT_EXPLORER_AND_NETSCAPE_NAVIGATOR_FINALLY_FIXED_THEIR_BUG
    if (128 <= c && c < 160)
        c = CodePageByCharset(CODES_ASCII)->unicode[c];
#endif
    if (0xF000 <= c && c < 0xF100) // UNKNOWN PLANE
        c = '\x20';
}

template <typename Int>
inline bool HiBit(Int n) {
    return n >> (sizeof(n) * 8 - 1);
}

inline void DoNumber(const unsigned char*& s, const unsigned char* end, size_t& clen, wchar32& c) {
    // __0123456__
    // __&#9....__
    c = digit(s[2]);
    for (clen = 3; TEST_CHAR_AT(isdigit, clen); clen++)
        if (!HiBit(c))  // overflow - unicode should put BROKEN_RUNE
            c = c*10 + digit(s[clen]);
    clen += TEST_CHAR_AT(';'==, clen);
    FixBadNumber(c);
}

inline void DoXNumber(const unsigned char*& s, const unsigned char* end, size_t& clen, wchar32& c) {
    // __0123456__
    // __&#xH...__
    c = xdigit(s[3]);
    for (clen = 4; TEST_CHAR_AT(isxdigit, clen); clen++)
        if (!HiBit(c))  // until overflow
            c = c*16 + xdigit(s[clen]);
    clen += TEST_CHAR_AT(';'==, clen);
    FixBadNumber(c);
}

inline void DoSymbol(ECharset cp, const unsigned char* s, const unsigned char* end, size_t& clen, wchar32& c) {
    size_t written = 0;
    RECODE_RESULT res = RecodeToUnicode(cp, (const char*)s, &c, end - s, 1, clen, written);
    bool error = !(res == RECODE_OK || res == RECODE_EOOUTPUT);
    if (error || clen == 0)
        clen = 1;
    if (error || written == 0)
        c = BROKEN_RUNE;
}

wchar32 HtEntDecodeStep(ECharset cp, const unsigned char*& s, const unsigned char* end, unsigned char** map) {
    wchar32 c;
    size_t clen = 0;

    if (s[0] == '&' && s < (end-2)) { // &aa..., &#9..., &#x...
        if (isalpha(s[1]) && isalnum(s[2])) {
            const THtmlEntity* p = 0;
            DetectEntity(s, end, clen, p);
            if (p && (TEST_CHAR_AT(';'==, clen) ||
                 (p->IsHtmlCore && !TEST_CHAR_AT(isalnum, clen))))
                SetEntity(s, end, p, clen, c);
            else
                SetAmp(clen, c);
        } else if (s[1] == '#') {
            if (isdigit(s[2]))
                DoNumber(s, end, clen, c);
            else if ((s[2] == 'x'||s[2]=='X') && TEST_CHAR_AT(isxdigit,3))
                DoXNumber(s, end, clen, c);
            else
                SetAmp(clen, c);
        } else {
            SetAmp(clen, c);
        }
    } else {
        DoSymbol(cp, s, end, clen, c);
    }
    s += clen;
    if (map && *map)
        *(*map)++ = (unsigned char)clen;
    return c;
}

wchar32 HtEntPureDecodeStep(ECharset cp, const unsigned char*& s, const unsigned char* end, unsigned char** map) {
    wchar32 c;
    size_t clen = 0;

    if (s[0] == '&' && s < (end-2)) { // &aa..., &#9..., &#x...
        if (isalpha(s[1]) && isalnum(s[2])) {
            const THtmlEntity* p = 0;
            DetectEntity(s, end, clen, p);
            if (p && (TEST_CHAR_AT(';'==, clen)))
                SetEntity(s, end, p, clen, c);
            else
                SetAmp(clen, c);
        } else if (s[1] == '#') {
            if (isdigit(s[2]))
                DoNumber(s, end, clen, c);
            else if ((s[2] == 'x'||s[2]=='X') && TEST_CHAR_AT(isxdigit,3))
                DoXNumber(s, end, clen, c);
            else
                SetAmp(clen, c);
        } else {
            SetAmp(clen, c);
        }
    } else {
        DoSymbol(cp, s, end, clen, c);
    }
    s += clen;
    if (map && *map)
        *(*map)++ = (unsigned char)clen;
    return c;
}

size_t HtEntDecodeToYx(ECharset cp, const char* str, size_t len, char* dst) {
#if defined (C_LOCALE)
    assert(strcmp(setlocale(LC_CTYPE, NULL), "C") == 0);
#endif
    const unsigned char *s = (const unsigned char *)str;
    const unsigned char *end = (const unsigned char *)(str + len);
    size_t ret = 0;
    while (s < end) {
        wchar32 c = HtEntDecodeStep(cp, s, end, 0);
        *dst++ = WideCharToYandex.Tr(c);
        ret++;
    }
    return ret;
}

size_t HtEntDecodeToChar(ECharset cp, const char* str, size_t len, ui16* dst, unsigned char* m) {
#if defined (C_LOCALE)
    assert(strcmp(setlocale(LC_CTYPE, NULL), "C") == 0);
#endif
    const unsigned char* s = (const unsigned char*)str;
    const unsigned char* end = (const unsigned char*)(str + len);
    ui16* startDst = dst;
    while (s < end) {
        const wchar32 c = HtEntDecodeStep(cp, s, end, &m);
        const size_t len = WriteSymbol(c, dst);
        if (m != NULL && len > 1)
            *(m++) = 0;
    }
    return dst - startDst;
}

bool HtLinkDecode (const char* in, char* out, size_t buflen, ECharset cp) {
    // should be something like
    // wchar16 buffer[];
    // Decode(link -> buffer, CurrentCodePage)
    // CurrentCodePageEncoder.encode(buffer -> result)
    // return result

    static const char XDIGIT[] = "0123456789ABCDEFabcdef";

    static str_spn ss_xdigit(XDIGIT);
    static str_spn ss_digit("0123456789");
    static str_spn ss_alnum("abcdefghijklmnopqrstuvwxyz"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

    size_t written = 0;
    size_t elen = 0;

    for (const char *p = in; *p; p += elen) {
        bool entity = false;
        wchar32 charval = (unsigned char)*p;
        elen = 1;

        if (*p == '&') {
            if (p[1] == '#') {
                // &#entity_number;
                if (p[2] == 'x') {
                    if ((elen = ss_xdigit.spn(p + 3)) != 0 && p[3 + elen] == ';') {
                        charval = strtol(p + 3, 0, 16);
                        elen += 4;
                        entity = true;
                    }
                } else if ((elen = ss_digit.spn(p + 2)) != 0 && p[2 + elen] == ';') {
                    charval = strtol(p + 2, 0, 10);
                    elen += 3;
                    entity = true;
                }
            } else if (isalpha((unsigned char)p[1]) && (elen = ss_alnum.spn(p + 2)) != 0 && p[2 + elen] == ';') {
                // &entity_name;
                const THtmlEntity* ent = SelectHtmlEntity(p + 1, elen + 1);
                if (ent) {
                    charval = ent->Unicode;
                    elen += 3;
                    entity = true;
                }
            }
            if (!entity) {
                charval = '&';
                elen = 1;
            }
        }

        if (cp != CODES_UNKNOWN && !entity) {
            size_t written = 0;
            RECODE_RESULT res = RecodeToUnicode(cp, p, &charval, 6, 1, elen, written) ;
            if (res != RECODE_OK && res != RECODE_EOOUTPUT)
                return false;
            YASSERT(written == 1);
            entity = true;
        }

        if (charval <= 0x20 || charval >= 0x7F) {
            if (entity && charval >= 0x7F) {
                const size_t BUFLEN = 4; // 4 max length of UTF8 encoded character
                unsigned char buf[BUFLEN];
                size_t len = 0;
                if (utf8_put_rune(charval, len, buf, buf + BUFLEN) != RECODE_OK) // actually always OK
                    return false;
                const size_t n = len * 3;
                if (written + n < buflen) {
                    for (size_t i = 0; i < len; ++i) {
                        out[written++] = '%';
                        out[written++] = XDIGIT[buf[i] >> 4];
                        out[written++] = XDIGIT[buf[i] & 15];
                    }
                } else
                    return false;
            } else {
                if (written + 3 < buflen) {
                    out[written++] = '%';
                    out[written++] = XDIGIT[(unsigned char)*p >> 4];
                    out[written++] = XDIGIT[(unsigned char)*p & 15];
                } else
                    return false; // ERROR_SMALL_BUFFER
            }
        } else {
            if (written + 1 < buflen) {
                out[written++] = (unsigned char)charval;
            } else {
                return false; // ERROR_SMALL_BUFFER
            }
        }
    }
    out[written] = '\x00';

    return true;
}
