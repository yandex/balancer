#pragma once

#include <util/generic/strbuf.h>

template <bool addAll, char sep1, char sep2, class F>
static inline void ScanKeyValue(TStringBuf s, F& f)
{
    TStringBuf key, val;
    while (!s.Empty())
    {
        val = s.NextTok(sep1);
        if (val.Empty())
            continue; // && case

        key = val.NextTok(sep2);
        if (addAll || val.IsInited())
            f(key, val); // includes empty keys
    }
}
