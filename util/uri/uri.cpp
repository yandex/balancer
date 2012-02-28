#include "uri.h"
#include "parse.h"

#include <util/generic/chartraits.h>
#include <util/generic/map.h>
#include <util/generic/strkey.h>
#include <util/string/cast.h>
#include <util/string/util.h>
#include <util/system/maxlen.h>
#include <util/system/yassert.h>

namespace NUri {

TState::EParsed TUri::CheckHost(const TStringBuf& host)
{
    if (host.Empty())
        return ParsedOK;

    unsigned domainLevel = 0;
    unsigned domainLevelOfUnderscore = 0;

    bool isAlnum = false;
    bool startLabel = true;
    for (size_t i = 0; i != host.length(); ++i)
    {
        const char ch = host[i];

        if ('.' == ch) { // label separator
            if (!isAlnum || startLabel) // previous label must end in alnum
                return ParsedBadHost;
            startLabel = true;
            continue;
        }

        isAlnum = isalnum((const unsigned char) ch);

        if (startLabel) { // label is starting
            if (!isAlnum) // new label must start with alnum
                return ParsedBadHost;
            startLabel = false;
            ++domainLevel;
            continue;
        }

        if (isAlnum || '-' == ch)
            continue;

        if (ch == '_') { // non-standard case we allow for certain hosts
            domainLevelOfUnderscore = domainLevel;
            continue;
        }

        return ParsedBadHost;
    }

    if (0 < domainLevelOfUnderscore && domainLevel < 2 + domainLevelOfUnderscore)
        return ParsedBadHost;

    return ParsedOK;
}

/********************************************************/
TUri::TUri(const TStringBuf& host
    , ui16 port
    , const TStringBuf& path
    , const TStringBuf& query
    , const TStringBuf& scheme
    , unsigned defaultPort
)
    : FieldsSet(0)
    , Port(port)
    , DefaultPort(0)
    , Scheme(SchemeEmpty)
    , FieldsDirty(0)
{
    if (!scheme.Empty()) {
        const TSchemeInfo& info = SetScheme(scheme);
        if (info.Str.Empty())
            FldSet(FieldScheme, scheme);
        else
            FldSetNoDirty(FieldScheme, info.Str);
    }

    if (0 < defaultPort) // override the scheme's default port
        DefaultPort = static_cast<ui16>(defaultPort);

    char sport[6]; // enough for ui16
    if (0 != port) {
        const size_t len = ToString(port, sport, sizeof(sport));
        FldSet(FieldPort, TStringBuf(sport, len));
    }

    FldTrySet(FieldHost, host);
    FldTrySet(FieldPath, path);
    FldTrySet(FieldQuery, query);

    Rewrite();
}

/********************************************************/
bool TUri::FldSetImpl(
    EField field, TStringBuf value, bool strconst, bool nocopy)
{
    if (!FldIsValid(field))
        return false;

    switch (field)
    {
    case FieldScheme:
        {
            const TSchemeInfo& info = SetScheme(value);
            if (!info.Str.Empty()) {
                value = info.Str;
                strconst = true;
            }
        }
        break;

    case FieldPort:
        Port = value.Empty() ? 0 : FromString<ui16>(value);
        break;

    default:
        break;
    }

    if (!value.IsInited()) {
        FldClr(field);
        return false;
    }

    if (strconst) { // string constants don't need to be saved in the buffer
        FldMarkClean(field);
        FldSetNoDirty(field, value);
        return false;
    }

    if (nocopy) {
        FldSet(field, value);
        return true;
    }

    return FldTryCpy(field, value);
}

/********************************************************/
bool TUri::FldTryCpy(EField field, const TStringBuf& value)
{
    if (!FldIsDirty(field)) {
        do {
            if (!(Buffer.IsNonShared() && FldIsSet(field)))
                break;

            TStringBuf& fld = Fields[field];
            if (fld.length() < value.length())
                break;

            char* oldV = (char*) fld.data();
            if (!IsInBuffer(oldV))
                break;

            memcpy(oldV, value.data(), value.length());
            oldV[value.length()] = 0;
            fld.Trunc(value.length());
            return false;
        }
        while (false);

        FldMarkDirty(field);
    }

    FldSetNoDirty(field, value);
    return true;
}

/********************************************************/
void TUri::RewriteImpl()
{
    size_t len = 0;
    for (int i = 0; i < FieldMAX; ++i)
    {
        const EField fld = EField(i);
        if (FldIsSet(fld))
            len += 1 + Fields[fld].length();
    }

    if (!len)
        Buffer.clear();
    else {
        // don't need extra null
        Stroka newbuf(static_cast<int>(len - 1));
        TMemoryWriteBuffer out(newbuf.begin(), newbuf.length());
        for (int i = 0; i < FieldMAX; ++i)
        {
            const EField fld = EField(i);
            if (!FldIsSet(fld))
                continue;

            if (!out.Empty())
                out << '\0';
            const char *beg = out.Buf();
            const TStringBuf& val = Fields[fld];
            out << val;
            FldSetNoDirty(fld, TStringBuf(beg, val.length()));
        }
        Buffer.swap(newbuf);
    }

    FieldsDirty = 0;
}

/********************************************************/
void TUri::Merge(const TUri& base, int correctAbs)
{
    if (base.Scheme == SchemeUnknown)
        return;

    if (!base.IsValidGlobal())
        return;

    const bool noscheme = IsNull(FlagScheme);
    if (!noscheme && !base.IsNull(FlagScheme) &&
        !EqualNoCase(base.GetField(FieldScheme), GetField(FieldScheme)))
        return;

    const ui32 cleanFields = ~FieldsDirty;
    do {
        static const TStringBuf rootPath = STRINGBUF("/");

        if (noscheme) {
            const TStringBuf& scheme = base.GetField(FieldScheme);
            if (!scheme.Empty()) {
                FldSetNoDirty(FieldScheme, scheme);
                // check if it is canonical
                if (scheme.data() != base.GetSchemeInfo().Str.data())
                    FldMarkDirty(FieldScheme);
            }
            Scheme = base.Scheme;
            DefaultPort = base.DefaultPort;
        }

        if (!IsNull(FlagHost)) {
            // make sure we have a path if scheme is also set
            if (FldSetCmp(FlagScheme | FlagPath, FlagScheme))
                FldSetNoDirty(FieldPath, rootPath);
            break; // no merge
        }

        FldTrySet(FieldHost, base);
        FldChkSet(FieldPort, base);
        Port = base.Port;

        if (noscheme && IsNull(FlagQuery) && IsNull(FlagPath))
            FldTrySet(FieldQuery, base);

        if (IsNull(FlagAuth) && !base.IsNull(FlagAuth)) {
            FldChkSet(FieldUser, base);
            FldChkSet(FieldPass, base);
        }

        if (IsValidAbs())
            break;

        TStringBuf p0 = base.GetField(FieldPath);
        if (!p0.IsInited())
            p0 = rootPath;

        TStringBuf p1 = GetField(FieldPath);
        if (!p1.IsInited()) {
            if (p0.data() != rootPath.data())
                FldSet(FieldPath, p0);
            else
                FldSetNoDirty(FieldPath, rootPath);
            break;
        }
        if ('/' == p1[0])
            p1.Skip(1); // p0 will have one

        bool pathop = true;

        TTempBufOutput out(p0.length() + p1.length() + 4);
        out << p0;
        if ('/' != p0.back())
            out << "/../";
        else if ('.' != p1[0])
            pathop = false;
        out << p1;

        char *beg = out.Data();
        char *end = beg + out.Filled();
        if (pathop && !PathOperation(beg, end, correctAbs)) {
            Clear();
            break;
        }

        // Needs immediate forced rewrite because of TTempBuf
        FldSetNoDirty(FieldPath, TStringBuf(beg, end));
        RewriteImpl();
    }
    while (false);

    // rewrite only if borrowed fields from base
    if (cleanFields & FieldsDirty)
        RewriteImpl();
}

/********************************************************/
TUri::TLinkType TUri::Normalize(const TUri& base,
     const TStringBuf& link, const TStringBuf& codebase, long careFlags)
{

    // parse URL
    if (Parse(link, careFlags) != ParsedOK)
        return LinkIsBad;

    bool local = IsNull(FlagHost);

    // merge with base URL
    // taken either from _BASE_ property or from optional argument
    if (!codebase.Empty())
    {
        // if optional code base given -- parse it
        TUri codebaseUrl;
        if (codebaseUrl.Parse(codebase, careFlags) != ParsedOK ||
            !codebaseUrl.IsValidAbs())
            return LinkIsBad;
        Merge(codebaseUrl);
    } else {
        // Base is already in this variable
        // see SetProperty() for details
        Merge(base);
    }

    // check result: must be correct absolute URL
    if (!IsValidAbs())
        return LinkBadAbs;

    if (!local)
    {
        //  - we don't care about different ports for the same server
        //  - we don't care about win|www|koi|etc. preffixes for the same server
        if (GetPort()!=base.GetPort() ||
            base.IsNull(FlagHost) ||
            !EqualNoCase(GetField(FieldHost), base.GetField(FieldHost)))
            return LinkIsGlobal;
    }

    // find out if it is link to itself then ignore it
    if (!Compare(base, FlagPath | FlagQuery))
        return LinkIsFragment;

    return LinkIsLocal;
}

/********************************************************/

size_t TUri::PrintSize(ui32 flags) const
{
    size_t len = 10;
    flags &= FieldsSet; // can't output what we don't have
    ui32 opt = 1;
    for (int fld = 0; opt <= flags && fld < FieldMAX; ++fld, opt <<= 1)
    {
        if (opt & flags) {
            const TStringBuf& v = Fields[fld];
            if (v.IsInited()) {
                if (opt & FlagAuth)
                    len += 3 * v.length() + 1;
                else
                    len += v.length() + 1;
            }
        }
    }

    return len;
}

TOutputStream &TUri::Print(TOutputStream &out, int flags) const
{
    TStringBuf v;

    const int wantFlags = flags; // keep the original
    flags &= FieldsSet; // can't print what we don't have

    if (flags & FlagScheme)
    {
        v = Fields[FieldScheme];
        if (!v.Empty())
            out << v << ':';
        if (wantFlags & FlagHost)
            out << "//";
    }

    do {
        if (0 == (flags & FlagHost))
            break;

        const TStringBuf& host = Fields[FieldHost];
        if (!host.IsInited())
            break;

        if (flags & FlagAuth)
        {
            if (flags & FlagUser)
            {
                v = Fields[FieldUser];
                if (!v.Empty())
                    TEncoder::EncodeNotAlnum(out, v);
            }

            if (flags & FlagPass)
            {
                v = Fields[FieldPass];
                if (v.IsInited())
                {
                    out << ':';
                    TEncoder::EncodeAll(out, v);
                }
            }

            out << '@';
        }

        out << host;

        if ((flags & FlagPort) && 0 != Port && Port != DefaultPort)
        {
            v = Fields[FieldPort];
            if (!v.Empty())
                out << ':' << v;
        }
    }
    while (false);

    if (flags & FlagPath)
    {
        out << Fields[FieldPath];
    }

    if (flags & FlagQuery)
    {
        v = Fields[FieldQuery];
        if (v.IsInited())
            out << '?' << v;
    }

    if (flags & FlagFrag)
    {
        v = Fields[FieldFrag];
        if (v.IsInited())
            out << '#' << v;
    }

    return out;
}

/********************************************************/
int TUri::CompareField(EField fld, const TUri& url) const
{
    const TStringBuf &v0 = GetField(fld);
    const TStringBuf &v1 = url.GetField(fld);
    switch (fld)
    {
    case FieldScheme:
    case FieldHost:
        return CompareNoCase(v0, v1);
    default:
        return v0.compare(v1);
    }
}

/********************************************************/
int TUri::Compare(const TUri& url, int flags) const
{
    // first compare fields with default values
    if (flags & FlagPort)
    {
        const int ret = GetPort() - url.GetPort();
        if (ret)
            return ret;
        flags &= ~FlagPort;
    }

    // compare remaining sets of available fields
    const int rtflags = flags & url.FieldsSet;
    flags &= FieldsSet;
    const int fldcmp = flags - rtflags;
    if (fldcmp)
        return fldcmp;

    // field sets are the same, compare the fields themselves
    for (int i = 0; i < FieldMAX; ++i)
    {
        const EField fld = EField(i);
        if (flags & FldFlag(fld))
        {
            const int ret = CompareField(fld, url);
            if (ret)
                return ret;
        }
    }

    return 0;
}

/********************************************************/
bool TUri::PathOperation(char*& pathPtr, char*& pathEnd, int correctAbs)
{
    if (!pathPtr)
        return false;
    if (pathPtr == pathEnd)
        return true;

    if ((pathEnd - pathPtr) >= 2 && *(pathEnd - 2) == '/' && *(pathEnd - 1) == '.') {
        --pathEnd;
    }

    char* p_wr = pathEnd;
    int upCount = 0;

    char* p_prev = pathEnd;
    YASSERT(p_prev > pathPtr);
    while (p_prev>pathPtr && *(p_prev-1)=='/')
        p_prev--;

    for (char* p_rd = p_prev; p_rd; p_rd = p_prev)
    {
        YASSERT(p_rd[0] == '/' || p_rd == pathEnd);
        p_prev = 0;

        char* p = p_rd;

        if (p>pathPtr)
        {
            for (p--; *p!= '/'; p--)
            {
                if (p == pathPtr)
                    break;
            }
            if (*p == '/')
            {
                p_prev = p++;
                if ((p_prev - pathPtr >= 6 && !strnicmp(p_prev - 6, "http://", 7)) ||
                    (p_prev - pathPtr >= 7 && !strnicmp(p_prev - 7, "https://", 8)))
                {
                    --p_prev;
                    --p;
                }
                else {
                    //skip multiple from head '/'
                    while (p_prev>pathPtr && *(p_prev-1)=='/')
                        p_prev--;
                }
            }
        }

        YASSERT(p_prev == 0 || p_prev[0]=='/');
        //and the first symbol !='/' after p_prev is p

        if (p==p_rd)
        {
            //empty block:
            if (p_prev)
            { //either tail:
                YASSERT(p_rd==p_wr && *(p-1)=='/');
                --p_wr;
                continue;
            } else { //or head of abs path
                *(--p_wr) = '/';
                break;
            }
        }

        if (p[0] == '.')
        {
            if (p+1 == p_rd)
            {
                if (correctAbs || p_prev>pathPtr || pathPtr[0]!='/')
                    // ignore "./"
                    continue;
            } else {
                if ((p[1] == '.') && (p+2 == p_rd))
                {
                    // register "../" but not print
                    upCount++;
                    continue;
                }
            }
        }

        if (upCount)
        {
            //unregister "../" and not print
            upCount--;
            continue;
        }

        // print
        YASSERT(p<p_rd);
        YASSERT(!p_prev || *(p-1)=='/');
        if (p_wr == p_rd)
        {   //just skip
            p_wr = p;
        } else {   //copy
            int l = p_rd - p + 1;
            p_wr -= l;
            memmove(p_wr, p, l);
        }
    }

    if (upCount)
    {
        if (*pathPtr!='/')
        {
            if (pathEnd == p_wr && *(p_wr-1)=='.')
            {
                YASSERT(*(p_wr-2)=='.');
                p_wr -= 2;
                upCount--;
            }
            for (; upCount>0; upCount--)
            {
                *(--p_wr) = '/';
                *(--p_wr) = '.';
                *(--p_wr) = '.';
            }
        } else {
            if (correctAbs>0)
                return false;
            if (correctAbs==0)
            {
                //Bad path but present in RFC:
                // "Similarly, parsers must avoid treating "." and ".."
                // as special when they are not complete components of
                // a relative path. "
                for (; upCount>0; upCount--)
                {
                    *(--p_wr) = '.';
                    *(--p_wr) = '.';
                    *(--p_wr) = '/';
                }
            } else {
                upCount = false;
            }
        }
    }

    YASSERT(p_wr>=pathPtr);

    if (upCount)
        return false;
    pathPtr = p_wr;
    return true;
}

/********************************************************/
const char* LinkTypeToString(const TUri::TLinkType& t)
{
    switch (t)
    {
    case TUri::LinkIsBad:
        return "LinkIsBad";
    case TUri::LinkBadAbs:
        return "LinkBadAbs";
    case TUri::LinkIsFragment:
        return "LinkIsFragment";
    case TUri::LinkIsLocal:
        return "LinkIsLocal";
    case TUri::LinkIsGlobal:
        return "LinkIsGlobal";
    }
    YASSERT(0);
    return "";
}

}
