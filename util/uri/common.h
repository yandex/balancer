#pragma once

#include <util/generic/strbuf.h>
#include <util/stream/output.h>
#include <util/system/compat.h>

namespace NUri {

namespace NEncode {
    class TEncoder;
    struct TCharFlags;
}

class TParser;


struct TField
{
#define FIELD_NAME(f) Field ## f
#define FIELD_FLAG(f) Flag ## f = 1U << FIELD_NAME(f)

    enum EField
    {
        FIELD_NAME(Scheme),
        FIELD_NAME(User),
        FIELD_NAME(Pass),
        FIELD_NAME(Host),
        FIELD_NAME(Port),
        FIELD_NAME(Path),
        FIELD_NAME(Query),
        FIELD_NAME(Frag),

        // add fields above
        FieldMAX,
        // add aliases below

        FieldUsername = FieldUser,
        FieldPassword = FieldPass,
        FieldFragment = FieldFrag,
    };

    enum EFlags
    {
        FIELD_FLAG(Scheme),
        FIELD_FLAG(User),
        FIELD_FLAG(Pass),
        FIELD_FLAG(Host),
        FIELD_FLAG(Port),
        FIELD_FLAG(Path),
        FIELD_FLAG(Query),
        FIELD_FLAG(Frag),

        FlagHostPort  = FlagHost | FlagPort,
        FlagAuth      = FlagUser | FlagPass,
        FlagFragment  = FlagFrag,
        FlagAction    = FlagScheme | FlagHostPort | FlagPath,
        FlagNoFrag    = FlagAction | FlagQuery,
        FlagAll       = -1L
    };

#undef FIELD_NAME
#undef FIELD_FLAG
};


struct TState
{
    // The class is designed for http: and https: schemes only.
    // Other url's schemes, such as mail:, ftp: etc. are out of
    // spec. If url cannot be parsed by "standard" scheme and
    // FeatureSchemeFlexible is on, the code
    // ParsedOpaque is returned, and the stucture contains only
    // schema part.

    // However, URL of non-standard schema can be
    // parsed OK. Under this condition all the parts of URL
    // are formally satisfied the RFC terms, and ParsedOK is returned.
    // But: handle this with care, THttpURL is not a proper tool
    // for these undetermined cases.

    enum EParsed
    {
        ParsedOK     = 0,
        ParsedEmpty  = 1,
        ParsedOpaque = 2,
        ParsedBadFormat, // must follow all non-error states immediately
        ParsedBadPath,
        ParsedTooLong,
        ParsedBadPort,
        ParsedBadAuth,
        ParsedBadScheme,
        ParsedBadHost,
    };
};


struct TScheme {
    // don't forget to define a SchemeRegistry entry
    enum EKind
    {
        SchemeEmpty
        // add schemes below this line
        , SchemeHTTP
        , SchemeHTTPS
        , SchemeFTP
        , SchemeFILE
        // add schemes above this line
        , SchemeUnknown
    };
};


class TFeature
{
    friend class NEncode::TEncoder;
    friend struct NEncode::TCharFlags;
    friend class TParser;

#define FEATURE_NAME(f) _BitFeature ## f
#define FEATURE_FLAG(f) Feature ## f = 1UL << FEATURE_NAME(f)

protected:
    enum EBit
    {
        //==============================
        // Cases interpreted as errors:
        //==============================

        // allows authorization user/password in URL
        FEATURE_NAME(AuthSupported),

        // allows all known schemes in URL
        FEATURE_NAME(SchemeKnown),

        // allows all schemes, not only known
        FEATURE_NAME(SchemeFlexible),


        //==============================
        // Cases interpreted for processing (if required):
        //  (effects on result of Parse method)
        //==============================

        // path needs normalization
        // (simplification of directory tree: /../, /./, etc.
        FEATURE_NAME(PathOperation),

        // in scheme and host segments:
        // change upper case letters onto lower case ones
        FEATURE_NAME(ToLower),

        // decode unreserved symbols
        FEATURE_NAME(DecodeUnreserved),

        // legacy: decode standard symbols which may be safe for some fields
        FEATURE_NAME(DecodeStandardExtra),

        // decode symbols allowed (not necessarily safe to decode) only for a given field
        // (do not use directly, instead use FeatureDecodeSafe mask below)
        FEATURE_NAME(DecodeFieldAllowed),

        // in query segment: change escaped space to '+'
        FEATURE_NAME(EncodeSpaceAsPlus),

        // escape all string 'markup' symbols
        FEATURE_NAME(EncodeForSQL),

        // encoding of extended ascii symbols (8-bit)
        FEATURE_NAME(EncodeExtendedASCII),

        // encoding of extended delimiter set
        FEATURE_NAME(EncodeExtendedDelim),

        // omits absolute paths started by "/../"
        FEATURE_NAME(CorrectPath),

        // paths started by "/../" - ignore head
        FEATURE_NAME(NormalPath),

        // tries to fix errors (in particular, in fragment)
        FEATURE_NAME(TryToFix),

        // allow IDN hosts
        // @note after parsing host can contain percent-encoded characters that must be translated into IDN
        //       according to charset of the document
        FEATURE_NAME(AllowHostIDN),

        // robot interpreted network paths as BadFormat urls
        FEATURE_NAME(DenyNetworkPath),

        /* non-rfc use case: use only hierarchical uri (no opaque) and
         * prefer "host/abspath" over "relpath" when scheme is missing */
        FEATURE_NAME(HierURI),

        // standard prefers that all hex escapes were using uppercase A-F
        FEATURE_NAME(UpperEncoded),

        // internal usage: decode all encoded symbols
        FEATURE_NAME(DecodeANY),

        // add before this line
        _FeatureMAX
    };

protected:
    enum EPrivate {
        FEATURE_FLAG(DecodeANY),
        FEATURE_FLAG(DecodeFieldAllowed),
        FEATURE_FLAG(DecodeStandardExtra),
    };

public:
    enum EPublic {
        FeatureMAX = _FeatureMAX,
        FEATURE_FLAG(AuthSupported),
        FEATURE_FLAG(SchemeKnown),
        FEATURE_FLAG(SchemeFlexible),
        FEATURE_FLAG(PathOperation),
        FEATURE_FLAG(ToLower),
        FEATURE_FLAG(DecodeUnreserved),
        FEATURE_FLAG(EncodeSpaceAsPlus),
        FEATURE_FLAG(EncodeForSQL),
        FEATURE_FLAG(EncodeExtendedASCII),
        FEATURE_FLAG(EncodeExtendedDelim),
        FEATURE_FLAG(CorrectPath),
        FEATURE_FLAG(NormalPath),
        FEATURE_FLAG(TryToFix),
        FEATURE_FLAG(AllowHostIDN),
        FEATURE_FLAG(DenyNetworkPath),
        FEATURE_FLAG(HierURI),
        FEATURE_FLAG(UpperEncoded),
    };

#undef FEATURE_NAME
#undef FEATURE_FLAG

public:
    //==============================
    enum ESets {
        // these are guaranteed and will change buffer size

        FeatureDecodeStandard = 0
            | FeatureDecodeUnreserved
            | FeatureDecodeStandardExtra
            ,

        FeaturesDecode = 0
            | FeatureDecodeUnreserved
            | FeatureDecodeStandard
            ,

        FeaturesEncodeExtended = 0
            | FeatureEncodeExtendedASCII
            | FeatureEncodeExtendedDelim
            ,

        FeaturesEncode = 0
            | FeatureEncodeForSQL
            | FeaturesEncodeExtended
            ,

        // these are not guaranteed to apply to a given field

        FeatureDecodeAllowed = 0
            | FeatureDecodeUnreserved
            | FeatureDecodeFieldAllowed
            ,

        FeaturesMaybeDecode = 0
            | FeaturesDecode
            | FeatureDecodeAllowed
            ,

        FeaturesMaybeEncode = 0
            | FeaturesEncode
            ,

        //==============================
        FeaturesNormalizeSet   = 0
            | FeaturePathOperation
            | FeatureToLower
            | FeatureDecodeAllowed
            | FeatureEncodeSpaceAsPlus
            | FeatureEncodeForSQL
            | FeatureNormalPath
            | FeatureTryToFix
            | FeatureUpperEncoded
            ,

        FeaturesDefault = 0 // it reproduces old parsedURL
            | FeaturePathOperation
            | FeatureCorrectPath
            ,

        FeaturesAll = 0
            | FeatureAuthSupported
            | FeatureSchemeFlexible
            | FeaturesNormalizeSet
            ,

        FeaturesRobot = 0
            // http://tools.ietf.org/html/rfc3986#section-6.2.2
            | FeatureToLower          // 6.2.2.1
            | FeatureUpperEncoded     // 6.2.2.1
            | FeatureDecodeUnreserved // 6.2.2.2
            | FeaturePathOperation    // 6.2.2.3
            | FeatureCorrectPath
            | FeatureSchemeKnown
            | FeatureAllowHostIDN
            | FeatureDenyNetworkPath
            ,

        // these are mutually exclusive
        FeaturesPath = 0
            | FeatureCorrectPath
            | FeatureNormalPath
            ,
    };
};


static inline int strnicmp(const char* lt, const char* rt, size_t len)
{
    return lt == rt ? 0 : ::strnicmp(lt, rt, len);
}

static inline int CompareNoCasePrefix(const TStringBuf& lt, const TStringBuf& rt)
{
    return strnicmp(lt.data(), rt.data(), rt.length());
}

static inline bool EqualNoCase(const TStringBuf& lt, const TStringBuf& rt)
{
    return lt.length() == rt.length() && 0 == CompareNoCasePrefix(lt, rt);
}

static inline int CompareNoCase(const TStringBuf& lt, const TStringBuf& rt)
{
    if (lt.length() == rt.length())
        return CompareNoCasePrefix(lt, rt);
    return lt.length() < rt.length() ? -1 : 1;
}


class TSchemeInfo
{
public:
    const TScheme::EKind Kind;
    const ui16 Port;
    const TStringBuf Str;
    TSchemeInfo(TScheme::EKind kind, TStringBuf str, ui16 port = 0)
        : Kind(kind)
        , Port(port)
        , Str(str)
    {}
    bool Matches(const TStringBuf& scheme) const
    {
        return EqualNoCase(scheme, Str);
    }

public:
    static const TSchemeInfo& Get(const TStringBuf& scheme);
    static const TSchemeInfo& Get(TScheme::EKind scheme)
    {
        return Registry[scheme];
    }
    static TScheme::EKind GetKind(const TStringBuf& scheme)
    {
        return Get(scheme).Kind;
    }
    static TStringBuf GetCanon(TScheme::EKind scheme)
    {
        return Get(scheme).Str;
    }
    static ui16 GetDefaultPort(TScheme::EKind scheme)
    {
        return Get(scheme).Port;
    }

private:
    static const TSchemeInfo Registry[];
};


struct TParseFlags
{
    const long Allow;
    const long Extra;
    TParseFlags(long allow = 0, long extra = 0)
        : Allow(allow)
        , Extra(extra)
    {}
    long operator&(const TParseFlags &flags) const
    {
        return (Allow & flags.Allow) | (Extra & flags.Extra);
    }
    long operator&(long flags) const
    {
        return (Allow & flags);
    }
    TParseFlags operator|(const TParseFlags &flags) const
    {
        return TParseFlags(Allow | flags.Allow, Extra | flags.Extra);
    }
};


const char* FieldToString(const TField::EField& t);
const char* ParsedStateToString(const TState::EParsed& t);
const char* SchemeKindToString(const TScheme::EKind& t);

}


DECLARE_OUT_SPEC(inline, NUri::TField::EField, out, t)
{
    out << NUri::FieldToString(t);
}

DECLARE_OUT_SPEC(inline, NUri::TScheme::EKind, out, t)
{
    out << NUri::SchemeKindToString(t);
}

DECLARE_OUT_SPEC(inline, NUri::TState::EParsed, out, t)
{
    out << NUri::ParsedStateToString(t);
}

static inline ui16 DefaultPort(NUri::TScheme::EKind scheme)
{
    return NUri::TSchemeInfo::GetDefaultPort(scheme);
}

static inline NUri::TScheme::EKind SchemeKind(const TStringBuf& scheme)
{
    return NUri::TSchemeInfo::GetKind(scheme);
}
