#include "scan.h"
#include "quote.h"
#include "cgiparam.h"

bool TCgiParameters::Erase(const TStringBuf& name, size_t pos) {
    const TPair<iterator> pair = equal_range(name);
    for (iterator it = pair.first; it != pair.second; ++it, --pos)
        if (0 == pos) {
            erase(it);
            return true;
        }
    return false;
}

size_t TCgiParameters::EraseAll(const TStringBuf& name) {
    size_t num = 0;
    const TPair<iterator> pair = equal_range(name);
    for (iterator it = pair.first; it != pair.second; erase(it++), ++num)
        ;
    return num;
}

void TCgiParameters::ReplaceUnescaped(const TStringBuf& key, const TStringBuf& val)
{
    const TPair<iterator> pair = equal_range(key);
    iterator it = pair.first;
    if (it == pair.second) // not found
        insert(it, MakePair(key, val));
    else {
        it->second = val;
        for (++it; it != pair.second; erase(it++))
            ;
    }
}

void TCgiParameters::JoinUnescaped(
    const TStringBuf& key, TStringBuf sep, TStringBuf val)
{
    const TPair<iterator> pair = equal_range(key);
    iterator it = pair.first;
    if (it == pair.second) { // not found
        if (val.IsInited())
            insert(it, MakePair(key, val));
    }
    else {
        Stroka &dst = it->second;
        for (++it; it != pair.second; erase(it++))
        {
            dst += sep;
            dst.AppendNoAlias(~it->second, +it->second);
        }
        if (val.IsInited()) {
            dst += sep;
            dst += val;
        }
    }
}

static inline Stroka DoUnescape(const TStringBuf& s, ECharset enc) {
    Stroka res;

    res.reserve(CgiUnescapeBufLen(+s));
    res.ReserveAndResize(+CgiUnescape(res.begin(), s, enc));

    return res;
}

void TCgiParameters::InsertEscaped(const TStringBuf& name, const TStringBuf& value, ECharset enc) {
    InsertUnescaped(DoUnescape(name, enc), DoUnescape(value, enc));
}

static inline Stroka DoUnescape(const TStringBuf& s, ECharset to, ECharset from) {
    Stroka res;

    res.reserve(CgiUnescapeBufLen(+s));
    res.ReserveAndResize(CGIUnescapeX(res.begin(), ~s, to, from) - res.begin());

    return res;
}

void TCgiParameters::InsertEscaped(const TStringBuf& name, const TStringBuf& value, ECharset enc, ECharset org) {
    InsertUnescaped(DoUnescape(name, enc, org), DoUnescape(value, enc, org));
}

static inline Stroka DoUnescapeAuto(const TStringBuf& s, ECharset enc) {
    Stroka res;

    res.reserve(CgiUnescapeBufLen(+s));
    res.ReserveAndResize(CGIUnescapeAuto(res.begin(), s, enc) - res.begin());

    return res;
}

void TCgiParameters::InsertAutoEscaped(const TStringBuf& name, const TStringBuf& value, ECharset enc) {
    InsertUnescaped(DoUnescapeAuto(name, enc), DoUnescapeAuto(value, enc));
}

template <bool addAll, class F>
static inline void DoScan(const TStringBuf& s, F& f) {
    ScanKeyValue<addAll, '&', '='>(s, f);
}

struct TAddEscaped {
    TCgiParameters* C;
    ECharset E;

    inline void operator() (const TStringBuf& key, const TStringBuf& val) {
        C->InsertEscaped(key, val, E);
    }
};

struct TAddAutoEscaped {
    TCgiParameters* C;
    ECharset E;

    inline void operator() (const TStringBuf& key, const TStringBuf& val) {
        C->InsertAutoEscaped(key, val, E);
    }
};

struct TAddUnescaped {
    TCgiParameters* C;

    inline void operator() (const TStringBuf& key, const TStringBuf& val) {
        C->InsertUnescaped(ToString(key), ToString(val));
    }
};

void TCgiParameters::Scan(const TStringBuf& query, ECharset enc, bool form) {
    Flush();
    form ? ScanAdd(query, enc) : ScanAddAll(query, enc);
}

void TCgiParameters::ScanAuto(const TStringBuf& query, ECharset enc) {
    Flush();
    if (!ValidCodepage(enc) || enc == CODES_UTF8)
        enc = CODES_WIN;
    ScanAddAuto(query, enc);
    Stroka encName = Get("reqenc");
    if (+encName){
        ECharset reqEnc = CharsetByName(~encName);
        if (ValidCodepage(reqEnc) && reqEnc != CODES_UTF8 && reqEnc != enc){
            Flush();
            ScanAddAuto(query, reqEnc);
        }
    }
}

void TCgiParameters::ScanAdd(const TStringBuf& query, ECharset enc) {
    TAddEscaped f = {this, enc};

    DoScan<false>(query, f);
}

void TCgiParameters::ScanAddAuto(const TStringBuf& query, ECharset enc) {
    TAddAutoEscaped f = {this, enc};

    DoScan<false>(query, f);
}

void TCgiParameters::ScanAddUnescaped(const TStringBuf& query) {
    TAddUnescaped f = {this};

    DoScan<false>(query, f);
}

void TCgiParameters::ScanAddAll(const TStringBuf& query, ECharset enc) {
    TAddEscaped f = {this, enc};

    DoScan<true>(query, f);
}

Stroka TCgiParameters::Print() const {
    Stroka res;

    res.reserve(PrintSize());
    const char* end = Print(res.begin());
    res.ReserveAndResize(end - ~res);

    return res;
}

char* TCgiParameters::Print(char* res) const {
    if (empty()) {
        return res;
    }

    for (const_iterator i = begin(); ; ) {
        res = CGIEscape(res, i->first);
        *res++ = '=';
        res = CGIEscape(res, i->second);
        if (++i == end())
            break;
        *res++ = '&';
    }

    return res;
}

size_t TCgiParameters::PrintSize() const throw () {
    size_t res = size(); //for '&'

    for (const_iterator i = begin(); i != end(); ++i) {
        res += CgiEscapeBufLen(+i->first + +i->second); //extra zero will be used for '='
    }

    return res;
}

TCgiParameters::const_iterator
TCgiParameters::Find(const TStringBuf& name, size_t pos) const
{
    const TPair<const_iterator> pair = equal_range(name);
    for (const_iterator it = pair.first; it != pair.second; ++it, --pos)
        if (0 == pos)
            return it;
    return end();
}

bool TCgiParameters::Has(const TStringBuf& name, const TStringBuf& value) const {
    const TPair<const_iterator> pair = equal_range(name);
    for (const_iterator it = pair.first; it != pair.second; ++it)
        if (value == it->second)
            return true;
    return false;
}
