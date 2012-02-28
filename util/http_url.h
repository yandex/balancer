#pragma once

#include <util/uri/uri.h>
#include <util/uri/other.h>

// XXX: use NUri::TUri directly; this whole file is for backwards compatibility

using NUri::TUri;

class THttpURL
    : public TUri
{
public:
    typedef TField::EFlags TFlags;
    typedef TField::EField TField;
    typedef TScheme::EKind TSchemeKind;
    typedef TState::EParsed TParsedState;

public:
    enum {
        FeatureUnescapeStandard = TFeature::FeatureDecodeStandard,
        FeatureEscSpace         = TFeature::FeatureEncodeSpaceAsPlus,
        FeatureEscapeForSQL     = TFeature::FeatureEncodeForSQL,
        FeatureEscapeUnescaped  = TFeature::FeatureEncodeExtendedASCII,
    };

public:
    THttpURL(unsigned defaultPort = 80)
        : TUri(defaultPort)
    {}

    THttpURL(const TStringBuf& host, ui16 port, const TStringBuf& path
        , const TStringBuf& query = TStringBuf()
        , const TStringBuf& scheme = "http", unsigned defaultPort = 0)
        : TUri(host, port, path, query, scheme, defaultPort)
    {}

    THttpURL(const TUri& url)
        : TUri(url)
    {}

public: // XXX: don't use any of these legacy methods below

public: // use TUri::GetField() instead
    /// will return null-terminated if fld is not dirty
    const char* Get(EField fld) const
    {
        return GetField(fld).data();
    }

public: // use TUriUpdate class so that Rewrite() is only called once
    void Set(EField field, const TStringBuf& value)
    {
        if (SetInMemory(field, value))
            Rewrite();
    }

    template <size_t size>
    void Set(EField field, const char (&value)[size])
    {
        if (SetInMemory(field, value))
            Rewrite();
    }

public: // use TUri::FldXXX methods for better control
    // Partial quick set of the field, can be called for
    // multiple fields
    bool SetInMemory(EField field, const TStringBuf& value)
    {
        return FldMemSet(field, value);
    }

    template <size_t size>
    bool SetInMemory(EField field, const char (&value)[size])
    {
        return FldMemSet(field, value);
    }

    // clears a field
    void Reset(EField field)
    {
        FldClr(field);
    }

public: // use TUri::ReEncode
    static TOutputStream& WriteEsc(TOutputStream& out, const TStringBuf& val
        , long flags = FeaturesMaybeDecode | FeaturesMaybeEncode
    ) {
        return ReEncode(out, val, flags);
    }
};

static inline const char* HttpURLFieldToString(const NUri::TField::EField& t)
{
    return NUri::FieldToString(t);
}
static inline const char* HttpURLParsedStateToString(const NUri::TState::EParsed& t)
{
    return NUri::ParsedStateToString(t);
}
static inline const char* HttpUrlSchemeKindToString(const NUri::TScheme::EKind& t)
{
    return NUri::SchemeKindToString(t);
}
static inline const char* HttpURLLinkTypeToString(const NUri::TUri::TLinkType& t)
{
    return NUri::LinkTypeToString(t);
}
