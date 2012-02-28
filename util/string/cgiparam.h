#pragma once

#include <util/charset/doccodes.h>
#include <util/generic/map.h>
#include <util/generic/singleton.h>
#include <util/generic/strbuf.h>
#include <util/generic/stroka.h>

struct TStringLess {
    template <class T1, class T2>
    inline bool operator() (const T1& t1, const T2& t2) const throw () {
        return TStringBuf(t1) < TStringBuf(t2);
    }
};

class TCgiParameters
    : public ymultimap<Stroka, Stroka>
{
public:
    TCgiParameters() {
    }

    explicit TCgiParameters(const TStringBuf& cgiParStr, ECharset enc = CODES_UNKNOWN) {
        Scan(cgiParStr, enc);
    }

    void Flush() {
        erase(begin(), end());
    }

    size_t EraseAll(const TStringBuf& name);

    size_t NumOfValues(const TStringBuf& name) const {
        return count(name);
    }

    Stroka operator() () const {
        return Print();
    }

    void Scan(const TStringBuf& cgiParStr, ECharset enc = CODES_UNKNOWN, bool form = true);
    void ScanAuto(const TStringBuf& cgiParStr, ECharset enc = CODES_UNKNOWN);
    void ScanAdd(const TStringBuf& cgiParStr, ECharset enc = CODES_UNKNOWN);
    void ScanAddAuto(const TStringBuf& cgiParStr, ECharset enc = CODES_UNKNOWN);
    void ScanAddUnescaped(const TStringBuf& cgiParStr);
    void ScanAddAll(const TStringBuf& cgiParStr, ECharset enc = CODES_UNKNOWN);

    Stroka Print() const;
    char* Print(char* res) const;
    size_t PrintSize() const throw ();

    TPair<const_iterator> Range(const TStringBuf& name) const {
        return equal_range(name);
    }
    const_iterator Find(const TStringBuf& name, size_t numOfValue = 0) const;
    bool Has(const TStringBuf& name, const TStringBuf& value) const;
    const Stroka& Get(const TStringBuf& name, size_t numOfValue = 0) const {
        const const_iterator it = Find(name, numOfValue);
        return end() == it ? Default<Stroka>() : it->second;
    }

    void InsertEscaped(const TStringBuf& name, const TStringBuf& value, ECharset enc = CODES_UNKNOWN);
    void InsertEscaped(const TStringBuf& name, const TStringBuf& value, ECharset enc, ECharset original);
    void InsertAutoEscaped(const TStringBuf& name, const TStringBuf& value, ECharset enc = CODES_UNKNOWN);

    inline void InsertUnescaped(const TStringBuf& name, const TStringBuf& value) {
        insert(MakePair(name, value));
    }

    // will replace one or more values with a single one
    void ReplaceUnescaped(const TStringBuf& key, const TStringBuf& val);

    // will join multiple values into a single one using a separator
    // if val is a [possibly empty] non-NULL string, append it as well
    void JoinUnescaped(
        const TStringBuf& key, TStringBuf sep, TStringBuf val = TStringBuf());

    bool Erase(const TStringBuf& name, size_t numOfValue = 0);

    inline const char* FormField(const TStringBuf& name, size_t numOfValue = 0) const {
        const_iterator it = Find(name, numOfValue);

        if (it == end()) {
            return 0;
        }

        return ~it->second;
    }
};
