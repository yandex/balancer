#include "url.h"
#include "cast.h"
#include "util.h"

#include <util/system/maxlen.h>
#include <util/system/defaults.h>
#include <util/memory/tempbuf.h>
#include <util/generic/ptr.h>
#include <util/charset/recyr.hh>
#include <util/generic/hash_set.h>
#include <util/generic/yexception.h>

namespace {

    const char szHexDigits[] = "0123456789ABCDEF";

    template <typename TChar>
    inline bool EncodeRFC1738(TChar* buf, size_t len, const char* url) {
        YASSERT(len); // it must be equal at least to 1 for null terminator
        TChar* const end = buf + len - 4;
        for (const char* p = url; *p && buf < end; ++p) {
            const unsigned char c = *p;
            if (c == '+') {
                *buf++ = '%';
                *buf++ = '2';
                *buf++ = '0';
            } else if (c >= 0x80) {
                *buf++ = '%';
                *buf++ = szHexDigits[c >> 4];
                *buf++ = szHexDigits[c & 15];
            } else
                *buf++ = c;
        }
        *buf = 0;
        if (buf >= end)
            return false;
        if (buf[-1] == '/')
            buf[-1] = 0; // remove trailing '/'
        return true;
    }

}

bool NormalizeUrl(char *pszRes, size_t nResBufSize, const char *pszUrl, size_t pszUrlSize) {
    YASSERT(nResBufSize);
    pszUrlSize = pszUrlSize == Stroka::npos? strlen(pszUrl) : pszUrlSize;
    size_t nBufSize = pszUrlSize * 3 + 10;
    TTempBuf buffer(nBufSize);
    char* const szUrlUtf = buffer.Data();
    size_t nRead, nWrite;
    Recode(CODES_WIN, CODES_UTF8, pszUrl, szUrlUtf, pszUrlSize, nBufSize, nRead, nWrite);
    szUrlUtf[nWrite] = 0;
    return EncodeRFC1738(pszRes, nResBufSize, szUrlUtf);
}

Stroka NormalizeUrl(const TStringBuf& url) {
    size_t len = +url + 1;
    while (true) {
       TTempBuf buffer(len);
       if (NormalizeUrl(buffer.Data(), len, ~url, +url))
           return buffer.Data();
       len <<= 1;
    }
    ythrow yexception() << "impossible";
}

Wtroka NormalizeUrl(const TWtringBuf& url) {
    const size_t buflen = url.size() * sizeof(wchar16);
    TTempBuf buffer(buflen + 1);
    char* const data = buffer.Data();
    size_t read = 0, written = 0;
    RecodeFromUnicode(CODES_UTF8, url.c_str(), data, url.size(), buflen, read, written);
    data[written] = 0;

    size_t len = written + 1;
    while (true) {
       TArrayHolder<wchar16> p(new wchar16[len]);
       if (EncodeRFC1738(p.Get(), len, data))
           return p.Get();
       len <<= 1;
    }
    ythrow yexception() << "impossible";
}

Stroka StrongNormalizeUrl(const TStringBuf& url) {
    Stroka result(~url, +url);
    result.to_lower();
    if (result.has_prefix("http://"))
        result = result.substr(7);
    if (result.has_prefix("www."))
        result = result.substr(4);
    TTempBuf normalizedUrl(FULLURL_MAX + 7);
    if (!::NormalizeUrl(normalizedUrl.Data(), FULLURL_MAX, ~result))
        return result;
    else
        return normalizedUrl.Data();
}

namespace {

    struct TUncheckedSize {
        bool Has(size_t) const {
            return true;
        }
    };

    struct TKnownSize {
        size_t MySize;
        TKnownSize(size_t sz)
            : MySize(sz)
        {}
        bool Has(size_t sz) const {
            return sz <= MySize;
        }
    };


    template <typename TChar1, typename TChar2>
    int Compare1Case2(const TChar1* s1, const TChar2* s2, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            if (TCharTraits<TChar1>::ToLower(s1[i]) != s2[i])
                return TCharTraits<TChar1>::ToLower(s1[i]) < s2[i] ? -1 : 1;
        }
        return 0;
    }

    template <typename TChar, typename TTraits, typename TBounds>
    inline size_t GetHttpPrefixSizeImpl(const TChar* url, const TBounds &urlSize) {
        const TChar httpPrefix[] = { 'h', 't', 't', 'p', ':', '/', '/', 0 };
        const TChar httpsPrefix[] = { 'h', 't', 't', 'p', 's', ':', '/', '/', 0 };
        if (urlSize.Has(7) && Compare1Case2(url, httpPrefix, 7) == 0)
            return 7;
        if (urlSize.Has(8) && Compare1Case2(url, httpsPrefix, 8) == 0)
            return 8;
        return 0;
    }

    template <typename T>
    inline T CutHttpPrefixImpl(const T& url) {
        typedef typename T::char_type TChar;
        typedef typename T::traits_type TTraits;
        size_t prefixSize = GetHttpPrefixSizeImpl<TChar,TTraits>(url.c_str(), TKnownSize(url.size()));
        if (prefixSize)
            return url.substr(prefixSize);
        return url;
    }

}

size_t GetHttpPrefixSize(const char* url) {
    return GetHttpPrefixSizeImpl<char,TCharTraits<char> >(url, TUncheckedSize());
}

size_t GetHttpPrefixSize(const wchar16* url) {
    return GetHttpPrefixSizeImpl<wchar16, TCharTraits<wchar16> >(url, TUncheckedSize());
}

size_t GetHttpPrefixSize(const TStringBuf& url) {
    return GetHttpPrefixSizeImpl<char, TCharTraits<char> >(url.c_str(), TKnownSize(url.size()));
}

size_t GetHttpPrefixSize(const TWtringBuf& url) {
    return GetHttpPrefixSizeImpl<wchar16, TCharTraits<wchar16> >(url.c_str(), TKnownSize(url.size()));
}

TStringBuf CutHttpPrefix(const TStringBuf& url) {
    return CutHttpPrefixImpl(url);
}

TWtringBuf CutHttpPrefix(const TWtringBuf& url) {
    return CutHttpPrefixImpl(url);
}

TStringBuf CutSchemePrefix(const TStringBuf& url) {
    static str_spn delim("!-/:-@[-`{|}", true);
    const char *n = delim.brk(~url, url.end());
    if (n + 2 >= url.end() || *n != ':' || n[1] != '/' || n[2] != '/')
        return url;
    return TStringBuf(n + 3, url.end());
}

template<bool KeepPort>
static inline TStringBuf GetHostAndPortImpl(const TStringBuf& url) {
    static str_spn hstend(KeepPort ? "/;?#" : "/:;?#");
    const char *n = hstend.brk(~url, url.end());
    if (n != url.end())
        return url.substr(0, n - ~url);
    else
        return url;
}

TStringBuf GetHost(const TStringBuf& url) {
    return GetHostAndPortImpl<false>(url);
}

TStringBuf GetHostAndPort(const TStringBuf& url) {
    return GetHostAndPortImpl<true>(url);
}

TStringBuf GetOnlyHost(const TStringBuf& url) {
    return GetHost(CutSchemePrefix(url));
}

TStringBuf GetPathAndQuery(const TStringBuf& url) {
    const size_t off = url.find('/', GetHttpPrefixSize(url));
    if (TStringBuf::npos == off) {
        return "/";
    }
    const size_t end = url.find('#', 1 + off);
    return TStringBuf::npos == end
        ? url.SubStr(off)
        : url.SubStr(off, end - off);
}

// this strange creature returns 2nd level domain, possibly with port
TStringBuf GetDomain(const TStringBuf &host) {
    const char *c = !host? ~host : host.end() - 1;
    for (bool wasPoint = false; c != ~host; --c) {
        if (*c == '.') {
            if (wasPoint) {
                c++;
                break;
            }
            wasPoint = true;
        }
    }
    return TStringBuf(c, host.end());
}

TStringBuf GetZone(const TStringBuf& host) {
    size_t pos = host.rfind('.');
    if (pos != Stroka::npos)
        return host.SubStr(pos + 1);
    else
        return host;
}

TStringBuf CutWWWPrefix(const TStringBuf& url) {
    if (+url >= 4 && url[3] == '.' && !strnicmp(~url, "www", 3))
        return url.substr(4);
    return url;
}

static inline bool IsSchemeChar(char c) throw () {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool HasPrefix(const Stroka& url) throw () {
    size_t pos = url.find("://");

    if (pos == Stroka::npos) {
        return false;
    }

    for (size_t i = 0; i < pos; ++i) {
        if (!IsSchemeChar(url[i])) {
            return false;
        }
    }

    return true;
}

Stroka AddSchemePrefix(const Stroka& url, Stroka scheme) {
    if (HasPrefix(url)) {
        return url;
    }

    scheme.append("://");
    scheme.append(url);

    return scheme;
}

#define X(c) (c >= 'A' ? ((c & 0xdf) - 'A') + 10 : (c - '0'))

static inline int x2c(unsigned char* x) {
    if (!isxdigit(x[0]) || !isxdigit(x[1]))
        return -1;
    return X(x[0]) * 16 + X(x[1]);
}

#undef X

static inline int Unescape(char *str) {
    char *to, *from;
    int dlen = 0;
    if ((str = strchr(str, '%')) == 0)
        return dlen;
    for (to = str, from = str; *from; from++, to++) {
        if ((*to = *from) == '%') {
            int c = x2c((unsigned char *)from+1);
            *to = char((c > 0) ? c : '0');
            from += 2;
            dlen += 2;
        }
    }
    *to = 0;  /* terminate it at the new length */
    return dlen;
}

size_t NormalizeUrlName(char* dest, const TStringBuf& source, size_t dest_size) {
    if (source.Empty() || source[0] == '?')
        return strlcpy(dest, "/", dest_size);
    size_t len = Min(dest_size - 1, source.length());
    memcpy(dest, source.data(), len);
    dest[len] = 0;
    len -= Unescape(dest);
    strlwr(dest);
    return len;
}

size_t NormalizeHostName(char* dest, const TStringBuf& source, size_t dest_size, ui16 defport) {
    size_t len = Min(dest_size - 1, source.length());
    memcpy(dest, source.data(), len);
    dest[len] = 0;
    char buf[8] = ":";
    size_t buflen = 1 + ToString(defport, buf + 1, sizeof(buf) - 2);
    buf[buflen] = '\0';
    char *ptr = strstr(dest, buf);
    if (ptr && ptr[buflen] == 0) {
        len -= buflen;
        *ptr = 0;
    }
    strlwr(dest);
    return len;
}

static const char* TopLevelDomains[] = {
    "ac", "ad", "ae", "aero", "af", "ag", "ai", "al", "am", "an", "ao", "aq", "ar", "arpa", "as", "asia", "at", "au", "aw", "ax", "az",
    "ba", "bb", "bd", "be", "bf", "bg", "bh", "bi", "biz", "bj", "bm", "bn", "bo", "br", "bs", "bt", "bv", "bw", "by", "bz",
    "ca", "cat", "cc", "cd", "cf", "cg", "ch", "ci", "ck", "cl", "cm", "cn", "co", "com", "coop", "cr", "cu", "cv", "cx", "cy", "cz",
    "de", "dj", "dk", "dm", "do", "dz",
    "ec", "edu", "ee", "eg", "er", "es", "et", "eu",
    "fi", "fj", "fk", "fm", "fo", "fr",
    "ga", "gb", "gd", "ge", "gf", "gg", "gh", "gi", "gl", "gm", "gn", "gov", "gp", "gq", "gr", "gs", "gt", "gu", "gw", "gy",
    "hk", "hm", "hn", "hr", "ht", "hu",
    "id", "ie", "il", "im", "in", "info", "int", "io", "iq", "ir", "is", "it",
    "je", "jm", "jo", "jobs", "jp",
    "ke", "kg", "kh", "ki", "km", "kn", "kp", "kr", "kw", "ky", "kz",
    "la", "lb", "lc", "li", "lk", "lr", "ls", "lt", "lu", "lv", "ly",
    "ma", "mc", "md", "me", "mg", "mh", "mil", "mk", "ml", "mm", "mn", "mo", "mobi", "mp", "mq", "mr", "ms", "mt", "mu", "museum", "mv", "mw", "mx", "my", "mz",
    "na", "name", "nc", "ne", "net", "nf", "ng", "ni", "nl", "no", "np", "nr", "nu", "nz",
    "om", "org",
    "pa", "pe", "pf", "pg", "ph", "pk", "pl", "pm", "pn", "pr", "pro", "ps", "pt", "pw", "py",
    "qa",
    "re", "ro", "rs", "ru", "rw",
    "sa", "sb", "sc", "sd", "se", "sg", "sh", "si", "sj", "sk", "sl", "sm", "sn", "so", "sr", "st", "su", "sv", "sy", "sz",
    "tc", "td", "tel", "tf", "tg", "th", "tj", "tk", "tl", "tm", "tn", "to", "tp", "tr", "travel", "tt", "tv", "tw", "tz",
    "ua", "ug", "uk", "us", "uy", "uz",
    "va", "vc", "ve", "vg", "vi", "vn", "vu",
    "wf", "ws",
    "xn--mgbaam7a8h", "xn--mgberp4a5d4ar", "xn--p1ai", "xn--wgbh1c", "xn--80akhbyknj4f",
    "xxx",
    "ye", "yt",
    "za", "zm", "zw",
    0
};

const char** GetTlds() {
    return TopLevelDomains;
}

struct TTLDHash: public yhash_set<TStringBuf, TCIHash<TStringBuf>, TCIEqualTo<TStringBuf> > {
    TTLDHash()
    {
        for (const char** tld = GetTlds(); *tld; ++tld) {
            insert(*tld);
        }
    }
};

bool IsTld(TStringBuf s) {
    const TTLDHash& hash = Default<TTLDHash>();
    return hash.find(s) != hash.end();
}

bool InTld(TStringBuf s) {
    size_t p = s.rfind('.');
    return p != TStringBuf::npos && IsTld(s.Skip(p + 1));
}
