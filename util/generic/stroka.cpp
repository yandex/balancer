#include "stroka.h"

#include <util/charset/wide.h>
#include <util/memory/alloc.h>
#include <util/digest/murmur.h>

#if !defined(DONT_USE_CODEPAGE)
    #include <util/charset/codepage.h>
    #define CODEPAGE_ARGUMENT const CodePage&cp
    #define TO_LOWER(c)       cp.ToLower(c)
    #define TO_UPPER(c)       cp.ToUpper(c)
    #define TO_TITLE(c)       cp.ToTitle(c)
#else
    #define CODEPAGE_ARGUMENT
    #define TO_LOWER(c)       tolower(c)
    #define TO_UPPER(c)       toupper(c)
    #define TO_TITLE(c)       toupper(c)
#endif


#define AssertLength(cond) {if (!(cond)) {ThrowLengthError(#cond);}}

#define POS_RANGE(pos, len) pos = ((pos) < (len) ? (pos) : (len))
#define NUM_RANGE(num, len, pos) num = ((num) < ((len) - (pos)) ? (num) : ((len) - (pos)))

namespace {

    template <typename TCharType>
    inline TCharType* AllocateImpl(size_t oldLen, size_t newLen, NDetail::TStringData<TCharType>* oldData) {
        typedef NDetail::TStringData<TCharType> TData;
        typedef NDetail::TStringDataTraits<TCharType> TDataTraits;
        if (0 == newLen) {
            return TData::GetNull();
        }
        const size_t dataSize = TDataTraits::CalcDataSize(newLen);
        if (dataSize > TDataTraits::MaxSize) {
            ThrowLengthError("TStringImpl::Allocate");
        }
        TData* ret = reinterpret_cast<TData*>(oldData == NULL ? y_allocate(dataSize) : y_reallocate(oldData, dataSize));
        ret->Refs = 1;
        ret->BufLen = (dataSize - TDataTraits::Overhead) / sizeof(TCharType);
        ret->Length = oldLen;
        ret->Chars[oldLen] = 0;
        return ret->Chars;
    }

}

namespace NDetail {

    template <>
    char* Allocate(size_t oldLen, size_t newLen, TStringData<char>* oldData) {
        return AllocateImpl(oldLen, newLen, oldData);
    }

    template <>
    wchar16* Allocate(size_t oldLen, size_t newLen, TStringData<wchar16>* oldData) {
        return AllocateImpl(oldLen, newLen, oldData);
    }

    void Deallocate(void* data) {
        y_deallocate(data);
    }

}

bool Stroka::to_lower(size_t pos, size_t n COMMA CODEPAGE_ARGUMENT) {
    bool changed = false;
    size_t len   = length(), i;
    char c;
    POS_RANGE(pos, len);
    NUM_RANGE(n, len, pos);
    for (i = pos; n; i++, n--) {
        c = TO_LOWER(p[i]);
        if (c != p[i]) {
            if (!changed) {
                CloneIfShared();
                changed = true;
            }
            p[i] = c;
        }
    }
    return changed;
}

bool Stroka::to_upper(size_t pos, size_t n COMMA CODEPAGE_ARGUMENT) {
    bool changed = false;
    size_t len   = length(), i;
    char c;
    POS_RANGE(pos, len);
    NUM_RANGE(n, len, pos);
    for (i = pos; n; i++, n--) {
        c = TO_UPPER(p[i]);
        if (c != p[i]) {
            if (!changed) {
                CloneIfShared();
                changed = true;
            }
            p[i] = c;
        }
    }
    return changed;
}

bool Stroka::to_title(CODEPAGE_ARGUMENT) {
    bool changed = false;
    size_t len   = length(), i;
    char c;
    if (len) {
        c = TO_TITLE(p[0]);
        if (c != p[0]) {
            if (!changed) {
                CloneIfShared();
                changed = true;
            }
            p[0] = c;
        }
    }
    for (i = 1; i < len; i++) {
        char c = TO_LOWER(p[i]);
        if (c != p[i]) {
            if (!changed) {
                CloneIfShared();
                changed = true;
            }
            p[i] = c;
        }
    }
    return changed;
}

Wtroka& Wtroka::AppendUtf8(const ::TFixedString<char>& s) {
    NDetail::RecodeAppend<char>(s, *this, CODES_UTF8);
    return *this;
}

Wtroka& Wtroka::AppendAscii(const ::TFixedString<char>& s) {
    ReserveAndResize(size() + s.Length);
    Wtroka::iterator dst = begin() + size() - s.Length;
    for (const char* src = s.Start; dst != end(); ++dst, ++src) {
        *dst = static_cast<wchar16>(*src);
    }
    return *this;
}

bool Wtroka::to_lower(size_t pos, size_t n) {
    bool changed = false;
    size_t len = length();
    POS_RANGE(pos, len);
    NUM_RANGE(n, len, pos);

    wchar16* begin = p + pos;
    wchar16* end = begin + n;
    for (wchar16* i = begin; i != end;) {
        wchar32 s = ReadSymbol(i, end);
        wchar32 c = ToLower(s);
        if (c != s) {
            if (EXPECT_FALSE(!changed)) {
                if (CloneIfShared()) {
                    end = p + pos + n;
                    i = p + pos + (i - begin);
                }
                changed = true;
            }
            WriteSymbol(c, i); // also skipping symbol
        } else {
            i = SkipSymbol(i, end);
        }
    }
    return changed;
}

bool Wtroka::to_upper(size_t pos, size_t n) {
    bool changed = false;
    size_t len = length();
    POS_RANGE(pos, len);
    NUM_RANGE(n, len, pos);

    wchar16* begin = p + pos;
    wchar16* end = begin + n;
    for (wchar16* i = begin; i != end;) {
        wchar32 s = ReadSymbol(i, end);
        wchar32 c = ToUpper(s);
        if (c != s) {
            if (EXPECT_FALSE(!changed)) {
                if (CloneIfShared()) {
                    end = p + pos + n;
                    i = p + pos + (i - begin);
                }
                changed = true;
            }
            WriteSymbol(c, i); // also skipping symbol
        } else {
            i = SkipSymbol(i, end);
        }
    }
    return changed;
}

bool Wtroka::to_title() {
    size_t len = length();
    if (len == 0)
        return false;

    bool changed = false;

    wchar16* begin = p;
    wchar16* end = p + len;
    wchar16* i = p;

    wchar32 s = ReadSymbol(i, end);
    wchar32 c = ::ToTitle(s);
    if (c != s) {
        YASSERT(!changed);
        if (CloneIfShared()) {
            end = p + len;
            i = p + (i - begin);
        }
        changed = true;
        WriteSymbol(c, i); // also skipping symbol
    } else {
        i = SkipSymbol(i, end);
    }
    YASSERT(i <= end);

    while (i != end) {
        s = ReadSymbol(i, end);
        c = ToLower(s);
        if (c != s) {
            if (EXPECT_FALSE(!changed)) {
                if (CloneIfShared()) {
                    end = p + len;
                    i = p + (i - begin);
                }
                changed = true;
            }
            WriteSymbol(c, i); // also skipping symbol
        } else {
            i = SkipSymbol(i, end);
        }
    }
    return changed;
}

void Wtroka::reverse() {
    size_t len = length();
    THolder<wchar16> temp(Allocate(len));
    Reverse(p, len, temp.Get());
    Relink(temp.Release());
}

#ifndef DONT_USE_CODEPAGE

int stroka::compare(const stroka &s1, const stroka &s2, const CodePage& cp)
{
    return cp.stricmp(s1.p, s2.p);
}

int stroka::compare(const char* p, const stroka &s2, const CodePage& cp)
{
    return cp.stricmp(p, s2.p);
}

int stroka::compare(const stroka &s1, const char* p, const CodePage& cp)
{
    return cp.stricmp(s1.p, p);
}

int stroka::compare(const TFixedString &p1, const TFixedString &p2, const CodePage& cp)
{
    int rv = cp.strnicmp(p1.Start, p2.Start, Min(p1.Length, p2.Length));
    return rv? rv : p1.Length < p2.Length? -1 : p1.Length == p2.Length? 0 : 1;
}

bool stroka::is_prefix(const TFixedString &what, const TFixedString &of, const CodePage& cp)
{
    size_t len = what.Length;
    return len <= of.Length && cp.strnicmp(what.Start, of.Start, len) == 0;
}

bool stroka::is_suffix(const TFixedString &what, const TFixedString &of, const CodePage& cp)
{
    size_t len = what.Length;
    size_t slen = of.Length;
    return (len <= slen) && (0 == cp.strnicmp(what.Start, of.Start + slen - len, len));
}

#endif

size_t stroka::hashVal(const char *s, size_t len COMMA CODEPAGE_ARGUMENT) {
    size_t h = len;
    for (; /* (*s) && */ len--; ++s)
       h = 5*h + TO_LOWER(*s);
    return h;
}
