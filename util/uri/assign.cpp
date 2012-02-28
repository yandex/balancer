#include "uri.h"
#include "parse.h"

#include <util/string/cast.h>
#include <util/system/maxlen.h>
#include <util/system/yassert.h>

namespace NUri {

TState::EParsed TUri::Assign(const TParser& parser)
{
    Clear();

    TState::EParsed ret = parser.State;
    if (ParsedBadFormat <= ret)
        return ret;

    const TSection& scheme = parser.Sections[FieldScheme];
    do {
        const TSchemeInfo& info = SetScheme(parser.Scheme);
        if (!info.Str.Empty()) {
            FldSetNoDirty(FieldScheme, info.Str);
            break;
        }

        if (ParsedOpaque == ret && scheme.IsSet()) {
            FldSet(FieldScheme, scheme.Get());
            break;
        }
    }
    while (false);

    if (ParsedOK != ret)
        return ret;

    do {
        // check for missing path
        if (parser.Sections[FieldPath].IsSet())
            break;
        // skip //netpath
        if (parser.NetPath)
            break;
        // skip if no host
        if (!parser.Sections[FieldHost].IsSet())
            break;
        // skip if it is a relative path
        if (!scheme.IsSet() && 0 == (parser.Flags & FeatureHierURI))
            break;
        FldSetNoDirty(FieldPath, "/");
    }
    while (false);

    size_t buflen = 0;
    for (int idx = 0; idx < FieldMAX; ++idx)
    {
        const TSection& section = parser.Sections[idx];
        if (section.IsSet() && !FldIsSet(EField(idx)))
            buflen += 1 + section.EncodedLen(); // includes null
    }
    if (0 == buflen) // no more sections set?
        return ParsedOK;

    Alloc(buflen - 1); // don't need the extra null, Stroka provides it

    TMemoryWriteBuffer out(Buffer.begin(), Buffer.length());
    for (int idx = 0; idx < FieldMAX; ++idx)
    {
        const EField fld = EField(idx);

        const TSection& section = parser.Sections[idx];
        if (!section.IsSet() || FldIsSet(fld))
            continue;

        if (!out.Empty())
            out << '\0'; // terminate the previous field

        bool qNormalizePath = false;
        long careFlags = section.GetFlags();
        if (careFlags)
        {
            if (careFlags & FeaturePathOperation)
            {
                assert(idx == FieldPath);
                qNormalizePath = true;
                careFlags &= ~FeaturePathOperation;
            }
        }

        char* beg = out.Buf();
        const TStringBuf val = section.Get();
        if (careFlags)
            ReEncodeField(out, val, fld, careFlags);
        else
            out << val;
        char* end = out.Buf();

        if (qNormalizePath)
        {
            int correctAbs = 0;
            if (parser.Flags & FeatureCorrectPath)
                correctAbs = 1;
            else if (parser.Flags & FeatureNormalPath)
                correctAbs = -1;

            if (!PathOperation(beg, end, correctAbs))
                return ParsedBadPath;

            YASSERT(beg >= out.Beg());
            out.SetPos(end);
        }

        FldSetNoDirty(fld, TStringBuf(beg, end));
    }

    Buffer.remove(out.Len());

    const TStringBuf& port = GetField(FieldPort);
    if (!port.Empty()) {
        try {
            Port = FromString<ui16>(port);
        }
        catch (...) {
            return ParsedBadPort;
        }
    }

    if (!(parser.Flags & FeatureAllowHostIDN)) {
        ret = CheckHost(GetField(FieldHost));
        if (ParsedOK != ret)
            return ret;
    }

    Rewrite();
    return ret;
}


TState::EParsed TUri::ParseUri(
    const TStringBuf& url, const TParseFlags& flags, ui32 maxlen)
{
    Clear();

    if (url.Empty())
        return ParsedEmpty;

    if (0 == maxlen)
        maxlen = URL_MAXLEN;
    if (url.length() >= maxlen)
        return ParsedTooLong;

    const TParser parser(flags, url);

    return Assign(parser);
}


TState::EParsed TUri::Parse(const TStringBuf& url
    , const TParseFlags& flags, const TStringBuf& url_base, ui32 maxlen)
{
    TState::EParsed ret;

    do {
        ret = ParseUri(url, flags, maxlen);
        if (ParsedOK != ret)
            break;

        if (url_base.Empty() || IsValidAbs())
            break;

        TUri base;
        ret = base.ParseUri(url_base, flags, maxlen);
        if (ParsedOK != ret)
            break;

        int correctAbs = 0;
        if (flags & FeatureCorrectPath)
            correctAbs = 1;
        else if (flags & FeatureNormalPath)
            correctAbs = -1;
        Merge(base, correctAbs);
    }
    while (false);

    Rewrite();
    return ret;
}

}
