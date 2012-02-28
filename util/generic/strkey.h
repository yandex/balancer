/*
 *  Created on: May 25, 2010
 *      Author: albert@
 *
 * $Id: strkey.h 709422 2012-01-26 23:53:43Z pg $
 */

#pragma once

#include <util/str_stl.h>
#include <util/generic/strbuf.h>

/* intended to be used as a string key in a map collection; creates
 * a permanent copy for insertions and doesn't for lookups only;
 * XXX: the important assumption is that elements are inserted
 * using the copy constructor which is where the copy is done! */

template <typename TpChar, typename TpTraits = TCharTraits<TpChar> >
    class TStringKeyT
{
protected:
    typedef TStringKeyT TdSelf;
    typedef TStringBufImpl<TpChar> TdBuf;
    typedef typename TpTraits::TWhatString::TString TdStr;

protected:
    const TdStr Str_;
    const TdBuf Buf_;

protected:
    TdStr BufStr() const
    {
        return ~Buf_ == ~Str_ ? Str_ : TdStr(Buf_);
    }

public:
    TStringKeyT(const TpChar *str)
        : Buf_(str)
    {}
    TStringKeyT(const TdBuf &str)
        : Buf_(str)
    {}
    TStringKeyT(const TdStr &str)
        : Str_(str)
        , Buf_(Str_)
    {}
    // makes a string copy if necessary
    TStringKeyT(const TdSelf &str)
        : Str_(str.BufStr())
        , Buf_(Str_)
    {}
    // this is a basic field copy
    TStringKeyT(const TdSelf *str)
        : Str_(str->Str_)
        , Buf_(str->Buf_)
    {}
    template<typename T, typename A>
    TStringKeyT(const NStl::basic_string<TpChar, T, A> &str)
        : Buf_(str)
    {}
    const TdBuf &Buf() const
    {
        return Buf_;
    }

public:
    friend bool operator==(const TdSelf &a, const TdSelf &b)
    {
        return a.Buf() == b.Buf();
    }
    friend bool operator<(const TdSelf &a, const TdSelf &b)
    {
        return a.Buf() < b.Buf();
    }
};

typedef TStringKeyT<char> TStringKey;
typedef TStringKeyT<wchar16> TWtringKey;


template <typename TCharT>
struct THash< TStringKeyT<TCharT> >
{
    size_t operator()(const TStringKeyT<TCharT>& str) const
    {
        return str.Buf().hash();
    }
};
