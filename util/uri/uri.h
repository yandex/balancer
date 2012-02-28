#pragma once

#include "common.h"
#include "encode.h"

#include <util/generic/singleton.h>
#include <util/generic/stroka.h>
#include <util/stream/mem.h>
#include <util/stream/output.h>
#include <util/stream/str.h>

namespace NUri {

/********************************************************/
class TUri
    : public TFeature
    , public TField
    , public TScheme
    , public TState
{
public:
    enum TLinkType
    {
        LinkIsBad,
        LinkBadAbs,
        LinkIsFragment,
        LinkIsLocal,
        LinkIsGlobal
    };

private:
    Stroka Buffer;
    TStringBuf Fields[FieldMAX];
    ui32 FieldsSet;
    ui16 Port;
    ui16 DefaultPort;
    TScheme::EKind Scheme;
    /// contains fields out of buffer (and possibly not null-terminated)
    ui32 FieldsDirty;

private:
    void Alloc(size_t len)
    {
        Dealloc(); // to prevent copy below
        Buffer.ReserveAndResize(len);
    }
    void Dealloc()
    {
        Buffer.clear();
    }

    void ClearImpl()
    {
        Port = 0;
        FieldsSet = 0;
        Scheme = SchemeEmpty;
        FieldsDirty = 0;
    }

    void CopyData(const TUri& url)
    {
        Buffer = url.Buffer;
        FieldsSet = url.FieldsSet;
        Port = url.Port;
        DefaultPort = url.DefaultPort;
        Scheme = url.Scheme;
        FieldsDirty = url.FieldsDirty;
    }

    void CopyImpl(const TUri& url)
    {
        for (int i = 0; i < FieldMAX; ++i)
            Fields[i] = url.Fields[i];

        Rewrite();
    }

private:
    static ui32 FldFlag(EField fld)
    {
        return 1 << fld;
    }

public:
    static bool FldIsValid(EField fld)
    {
        return 0 <= fld && FieldMAX > fld;
    }

    bool FldSetCmp(ui32 chk, ui32 exp) const
    {
        return (FieldsSet & chk) == exp;
    }

    bool FldIsSet(EField fld) const
    {
        return !FldSetCmp(FldFlag(fld), 0);
    }

private:
    // use when we know the field is dirty or RewriteImpl will be called
    void FldSetNoDirty(EField fld, const TStringBuf& value)
    {
        Fields[fld] = value;
        FieldsSet |= FldFlag(fld);
    }

    void FldSet(EField fld, const TStringBuf& value)
    {
        FldSetNoDirty(fld, value);
        FldMarkDirty(fld);
    }

private:
    /// depending on value, clears or sets it
    void FldChkSet(EField fld, const TStringBuf& value)
    {
        if (value.IsInited())
            FldSet(fld, value);
        else
            FldClr(fld);
    }
    void FldChkSet(EField fld, const TUri &other)
    {
        FldChkSet(fld, other.GetField(fld));
    }

    /// set only if initialized
    bool FldTrySet(EField fld, const TStringBuf& value)
    {
        const bool ok = value.IsInited();
        if (ok)
            FldSet(fld, value);
        return ok;
    }
    bool FldTrySet(EField fld, const TUri &other)
    {
        return FldTrySet(fld, other.GetField(fld));
    }

private:
    /// copies the value if it fits
    bool FldTryCpy(EField fld, const TStringBuf& value);

    // main method: sets the field value, possibly copies, etc.
    bool FldSetImpl(EField fld, TStringBuf value
        , bool strconst = false, bool nocopy = false);

public: // clear a field
    void FldClr(EField fld)
    {
        FieldsSet &= ~FldFlag(fld);
        Fields[fld].Clear();
        FldMarkClean(fld);
    }

    bool FldTryClr(EField field)
    {
        const bool ok = FldIsSet(field);
        if (ok)
            FldClr(field);
        return ok;
    }

public: // set a field value: might leave state dirty and require a Rewrite()
    // copies if fits and not dirty, sets and marks dirty otherwise
    bool FldMemCpy(EField field, const TStringBuf& value)
    {
        return FldSetImpl(field, value, false);
    }

    // uses directly, marks dirty
    bool FldMemSet(EField field, const TStringBuf& value)
    {
        return FldSetImpl(field, value, false, true);
    }

    // uses directly, doesn't mark dirty (value scope exceeds "this")
    bool FldMemUse(EField field, const TStringBuf& value)
    {
        return FldSetImpl(field, value, true);
    }

    // uses directly, doesn't mark dirty
    template <size_t size>
    bool FldMemSet(EField field, const char (&value)[size])
    {
        return FldSetImpl(field, AsStringBuf(value), true);
    }

private:
    bool IsInBuffer(const char* buf) const
    {
        return buf >= Buffer.data() && buf < Buffer.data() + Buffer.length();
    }

public:
    bool FldIsDirty() const
    {
        return 0 != FieldsDirty;
    }

    bool FldIsDirty(EField fld) const
    {
        return 0 != (FieldsDirty & FldFlag(fld));
    }

private:
    void FldMarkDirty(EField fld)
    {
        FieldsDirty |= FldFlag(fld);
    }

    void FldMarkClean(EField fld)
    {
        FieldsDirty &= ~FldFlag(fld);
    }

    void RewriteImpl();

public:
    static TState::EParsed CheckHost(const TStringBuf& host);

    TUri(unsigned defaultPort = 80)
        : FieldsSet(0)
        , Port(0)
        , DefaultPort(static_cast<ui16>(defaultPort))
        , Scheme(SchemeEmpty)
        , FieldsDirty(0)
    {
    }

    TUri(const TStringBuf& host, ui16 port, const TStringBuf& path
        , const TStringBuf& query = TStringBuf()
        , const TStringBuf& scheme = "http", unsigned defaultPort = 0);

    TUri(const TUri& url)
        : Buffer(url.Buffer)
        , FieldsSet(url.FieldsSet)
        , Port(url.Port)
        , DefaultPort(url.DefaultPort)
        , Scheme(url.Scheme)
        , FieldsDirty(url.FieldsDirty)
    {
        CopyImpl(url);
    }

    ~TUri()
    {
        Clear();
    }

    void Copy(const TUri& url)
    {
        if (&url != this) {
            CopyData(url);
            CopyImpl(url);
        }
    }

    void Clear()
    {
        Dealloc();
        ClearImpl();
    }

    // Process methods

    void Rewrite()
    {
        if (FldIsDirty())
            RewriteImpl();
    }

    TState::EParsed Assign(const TParser &parser);

    TState::EParsed ParseUri(const TStringBuf& url
        , const TParseFlags& flags = FeaturesDefault
        , ui32 maxlen = 0
    );

    TState::EParsed Parse(const TStringBuf& url
        , const TParseFlags& flags = FeaturesDefault
        , const TStringBuf& base_url = TStringBuf()
        , ui32 maxlen = 0
    );

    TState::EParsed ParseAbs(const TStringBuf& url
        , const TParseFlags& flags = FeaturesDefault
        , const TStringBuf& base_url = TStringBuf()
        , ui32 maxlen = 0
    ) {
        const TState::EParsed result = Parse(url, flags, base_url, maxlen);
        return ParsedOK != result || IsValidGlobal() ? result : ParsedBadFormat;
    }


    // correctAbs works with head "/.." portions:
    //  1 - reject URL
    //  0 - keep portions
    // -1 - ignore portions

    void Merge(const TUri& base, int correctAbs = -1);

    TLinkType Normalize(const TUri& base, const TStringBuf& link
        , const TStringBuf& codebase = TStringBuf()
        , long careFlags = FeaturesDefault);

protected:
    size_t PrintSize(ui32 flags = FlagAll) const;

    char* PrintImpl(char *str, size_t size, int flags = FlagAll) const
    {
        TMemoryOutput out(str, size);
        Print(out, flags) << '\0';
        return str;
    }

    bool IsAbsPathImpl() const
    {
        const TStringBuf& p = GetField(FieldPath);
        return 1 <= p.length() && p[0] =='/';
    }

public:
    // Output method, prints to stream
    TOutputStream &Print(TOutputStream &out, int flags = FlagAll) const;

    // Output method, print to str, allocate memory if str is NULL
    // Should be deprecated
    char* Print(char *str, size_t size, int flags = FlagAll) const
    {
        return NULL == str ? Serialize(flags) : Serialize(str, size, flags);
    }

    char* Serialize(char *str, size_t size, int flags = FlagAll) const
    {
        YASSERT(str);
        const size_t printSize = PrintSize(flags) + 1;
        return printSize > size ? NULL : PrintImpl(str, size, flags);
    }

    char* Serialize(int flags = FlagAll) const
    {
        const size_t size = PrintSize(flags) + 1;
        return PrintImpl(static_cast<char *>(malloc(size)), size, flags);
    }

    // Output method to str
    void Print(Stroka &str, int flags = FlagAll) const
    {
        str.reserve(str.length() + PrintSize(flags));
        TStringOutput out(str);
        Print(out, flags);
    }

    Stroka PrintS(int flags = FlagAll) const
    {
        Stroka str;
        Print(str, flags);
        return str;
    }

    // Only non-default scheme and port are printed
    char* PrintHost(char *str, size_t size) const
    {
        return Print(str, size, (Scheme != SchemeHTTP ? FlagScheme : 0) | FlagHostPort);
    }
    Stroka PrintHostS() const
    {
        return PrintS((Scheme != SchemeHTTP ? FlagScheme : 0) | FlagHostPort);
    }

    // Info methods
    int Compare(const TUri& A, int flags = FlagAll) const;

    int CompareField(EField fld, const TUri& url) const;

    const TStringBuf& GetField(EField fld) const
    {
        return FldIsValid(fld) && FldIsSet(fld) ? Fields[fld] : Default<TStringBuf>();
    }

    ui16 GetPort() const
    {
        return 0 == Port ? DefaultPort : Port;
    }

    TScheme::EKind GetScheme() const
    {
        return Scheme;
    }
    const TSchemeInfo& GetSchemeInfo() const
    {
        return TSchemeInfo::Get(Scheme);
    }

    bool IsNull(ui32 flags = FlagScheme | FlagHost | FlagPath) const
    {
        return !FldSetCmp(flags, flags);
    }

    bool IsNull(EField fld) const
    {
        return !FldIsSet(fld);
    }

    bool IsValidAbs() const
    {
        if (IsNull(FlagScheme | FlagHost | FlagPath))
            return false;
        return IsAbsPathImpl();
    }

    bool IsValidGlobal() const
    {
        if (IsNull(FlagScheme|FlagHost))
            return false;
        if (IsNull(FlagPath))
            return true;
        return IsAbsPathImpl();
    }

    // Inline helpers
    TUri& operator=(const TUri& u)
    {
        Copy(u);
        return *this;
    }

    bool operator!() const
    {
        return IsNull();
    }

    bool Equal(const TUri& A, int flags = FlagAll) const
    {
        return (Compare(A, flags)==0);
    }

    bool Less(const TUri& A, int flags = FlagAll) const
    {
        return (Compare(A, flags)<0);
    }

    bool operator==(const TUri& A) const
    {
        return Equal(A, FlagNoFrag);
    }

    bool operator!=(const TUri& A) const
    {
        return !Equal(A, FlagNoFrag);
    }

    bool operator<(const TUri& A) const
    {
        return Less(A, FlagNoFrag);
    }

    bool IsSameDocument(const TUri& other) const
    {
        // pre: both *this and 'other' should be normalized to valid abs
        assert(IsValidAbs());
        return Equal(other, FlagNoFrag);
    }

    bool IsLocal(const TUri& other) const
    {
        // pre: both *this and 'other' should be normalized to valid abs
        assert(IsValidAbs() && other.IsValidAbs());
        return Equal(other, FlagScheme|FlagHostPort);
    }

    TLinkType Locality(const TUri& other) const
    {
        if (IsSameDocument(other))
            return LinkIsFragment;
        else if (IsLocal(other))
            return LinkIsLocal;
        return LinkIsGlobal;
    }

    static TOutputStream& ReEncodeField(TOutputStream& out, const TStringBuf& val
        , EField fld
        , long flags = FeaturesMaybeDecode | FeaturesMaybeEncode
    ) {
        return NEncode::TEncoder::ReEncode(out, val, flags, fld);
    }

    static TOutputStream& ReEncode(TOutputStream& out, const TStringBuf& val
        , long flags = FeaturesMaybeDecode | FeaturesMaybeEncode
    ) {
        return ReEncodeField(out, val, FieldMAX, flags);
    }

    static bool PathOperation(char*& pathBeg, char*& pathEnd, int correctAbs);

private:
    const TSchemeInfo& SetScheme(const TSchemeInfo& info)
    {
        Scheme = info.Kind;
        DefaultPort = info.Port;
        return info;
    }
    const TSchemeInfo& SetScheme(TScheme::EKind scheme)
    {
        return SetScheme(TSchemeInfo::Get(scheme));
    }
    const TSchemeInfo& SetScheme(const TStringBuf &scheme)
    {
        return SetScheme(TSchemeInfo::Get(scheme));
    }
};

class TUriUpdate
{
    TUri &Uri_;

public:
    TUriUpdate(TUri &uri)
        : Uri_(uri)
    {}
    ~TUriUpdate()
    {
        Uri_.Rewrite();
    }

public:
    bool Set(TField::EField field, const TStringBuf& value)
    {
        return Uri_.FldMemSet(field, value);
    }

    template <size_t size>
    void Set(TField::EField field, const char (&value)[size])
    {
        return Uri_.FldMemSet(field, value);
    }

    void Clr(TField::EField field)
    {
        Uri_.FldClr(field);
    }
};

const char* LinkTypeToString(const TUri::TLinkType& t);

}

using NUri::TUri;
using NUri::TUriUpdate;

DECLARE_OUT_SPEC(inline, TUri, out, url)
{
    url.Print(out);
}

DECLARE_OUT_SPEC(inline, TUri::TLinkType, out, t)
{
    out << NUri::LinkTypeToString(t);
}
