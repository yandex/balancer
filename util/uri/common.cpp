#include "common.h"

#include <util/generic/map.h>

namespace NUri {

STATIC_ASSERT(TFeature::FeatureMAX <= sizeof(unsigned long) * 8);

const TSchemeInfo TSchemeInfo::Registry[] = {
    TSchemeInfo(TScheme::SchemeEmpty,   STRINGBUF("")) // scheme is empty and inited
    , TSchemeInfo(TScheme::SchemeHTTP,  STRINGBUF("http"),   80)
    , TSchemeInfo(TScheme::SchemeHTTPS, STRINGBUF("https"), 443)
    , TSchemeInfo(TScheme::SchemeFTP,   STRINGBUF("ftp"),    20)
    , TSchemeInfo(TScheme::SchemeFILE,  STRINGBUF("file"))
    // add above
    , TSchemeInfo(TScheme::SchemeUnknown, TStringBuf()) // scheme is empty and uninited
};

namespace {

struct TLessNoCase
{
    bool operator()(const TStringBuf& lt, const TStringBuf &rt) const
    {
        return 0 > CompareNoCase(lt, rt);
    }
};

class TSchemeInfoMap
{
    typedef ymap<TStringBuf, TScheme::EKind, TLessNoCase> TdMap;
    TdMap Map_;
public:
    TSchemeInfoMap()
    {
        for (int i = TScheme::SchemeEmpty; i < TScheme::SchemeUnknown; ++i)
        {
            const TSchemeInfo& info = TSchemeInfo::Get(TScheme::EKind(i));
            Map_.insert(MakePair(info.Str, info.Kind));
        }
    }
    TScheme::EKind Get(const TStringBuf& scheme) const
    {
        const TdMap::const_iterator it = Map_.find(scheme);
        return Map_.end() == it ? TScheme::SchemeUnknown : it->second;
    }
};

static const TSchemeInfoMap SchemeInfoMap;

}

const TSchemeInfo& TSchemeInfo::Get(const TStringBuf& scheme)
{
    return Registry[SchemeInfoMap.Get(scheme)];
}


const char* ParsedStateToString(const TState::EParsed& t)
{
    switch (t)
    {
    case TState::ParsedOK:
        return "ParsedOK";
    case TState::ParsedEmpty:
        return "ParsedEmpty";
    case TState::ParsedOpaque:
        return "ParsedOpaque";
    case TState::ParsedBadFormat:
        return "ParsedBadFormat";
    case TState::ParsedBadPath:
        return "ParsedBadPath";
    case TState::ParsedTooLong:
        return "ParsedTooLong";
    case TState::ParsedBadPort:
        return "ParsedBadPort";
    case TState::ParsedBadAuth:
        return "ParsedBadAuth";
    case TState::ParsedBadScheme:
        return "ParsedBadScheme";
    case TState::ParsedBadHost:
        return "ParsedBadHost";
    default:
        return "Parsed[Unknown]";
    }
    YASSERT(0);
    return "";
}

const char* FieldToString(const TField::EField& t)
{
    switch (t)
    {
    case TField::FieldScheme:
        return "scheme";
    case TField::FieldUser:
        return "username";
    case TField::FieldPass:
        return "password";
    case TField::FieldHost:
        return "host";
    case TField::FieldPort:
        return "port";
    case TField::FieldPath:
        return "path";
    case TField::FieldQuery:
        return "query";
    case TField::FieldFrag:
        return "fragment";
    case TField::FieldMAX:
        break;
    }
    YASSERT(0);
    return "";
}

const char *SchemeKindToString(const TScheme::EKind& t)
{
    const TSchemeInfo& info = TSchemeInfo::Get(t);
    if (!info.Str.Empty())
        return info.Str.data();
    return TScheme::SchemeEmpty == t ? "empty" : "unknown";
}

}
