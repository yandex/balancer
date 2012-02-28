#include "common.h"
#include "encode.h"
#include "parse.h"

namespace NUri {

const TParseFlags TParser::FieldFlags[] =
{
    TParseFlags(0 // FieldScheme
        | TFeature::FeatureToLower
        , 0
    )

    , TParseFlags(0 // FieldUsername
        | TFeature::FeatureDecodeANY
        | TFeature::FeaturesDecode
        , 0
        | TFeature::FeatureToLower
    )

    , TParseFlags(0 // FieldPassword
        | TFeature::FeatureDecodeANY
        | TFeature::FeaturesDecode
        , 0
        | TFeature::FeatureToLower
    )

    , TParseFlags(0 // FieldHost
        | TFeature::FeatureToLower
        | TFeature::FeatureUpperEncoded
        | TFeature::FeaturesMaybeEncode
        , 0
    )

    , TParseFlags(0 // FieldPort
        , 0
    )

    , TParseFlags(0 // FieldPath
        | TFeature::FeaturesMaybeDecode
        | TFeature::FeaturesMaybeEncode
        | TFeature::FeaturePathOperation
        | TFeature::FeatureUpperEncoded
        , 0
        | TFeature::FeatureToLower
        | TFeature::FeatureEncodeSpaceAsPlus
    )

    , TParseFlags(0 // FieldQuery
        | TFeature::FeaturesMaybeDecode
        | TFeature::FeaturesMaybeEncode
        | TFeature::FeatureUpperEncoded
        | TFeature::FeatureEncodeSpaceAsPlus
        , 0
        | TFeature::FeatureToLower
    )

    , TParseFlags(0 // FieldFragment
        | TFeature::FeaturesMaybeDecode
        | TFeature::FeaturesMaybeEncode
        | TFeature::FeatureUpperEncoded
        , 0
        | TFeature::FeatureToLower
        | TFeature::FeatureEncodeSpaceAsPlus
    )
};


void TSection::TRange::AddFlagImpl(const char *ptr, long flag)
{
    if (0 == flag)
        return;
    if (NULL == Beg)
        Beg = ptr;
    Flags |= flag;
    if (flag & TFeature::FeaturesMaybeEncode)
        ++Encode;
    else if (flag & TFeature::FeaturesDecode)
        ++Decode;
}


void TSection::AddRange(const TRange &range, long mask)
{
    // update only if flags apply here
    mask &= range.Flags;
    if (0 == mask)
        return;
    Flags |= mask;
    if (mask & TFeature::FeaturesMaybeEncode)
        Encode += range.Encode;
    if (mask & TFeature::FeaturesDecode)
        Encode -= range.Decode;
}


void TParser::copyRequirementsImpl(const char *ptr)
{
    YASSERT(0 != CurRange.Flags);
    UNUSED(ptr);
#ifdef DO_PRN
    PrintHead(ptr, __FUNCTION__) << IntToString<16>(CurRange.Flags);
    PrintTail(CurRange.Beg, ptr);
#endif
    for (int fld = 0; fld < TField::FieldMAX; ++fld)
    {
        TSection& section = Sections[fld];
        // update only sections in progress
        if (NULL == section.Beg)
            continue;
        // and overlapping with the range
        if (NULL != section.End && section.End < CurRange.Beg)
            continue;
#ifdef DO_PRN
        PrintHead(ptr, __FUNCTION__, TField::EField(fld))
            << " " << IntToString<16>(CurRange.Flags);
        PrintTail(section.Beg, ptr);
#endif
        section.AddRange(CurRange, (FieldFlags[fld] & Flags));
    }
    CurRange.Reset();
}

void TParser::HexSet(const char* ptr)
{
    const unsigned char ch = HexValue;
    long flags = TFeature::FeatureDecodeANY | TEncoder::GetFlags(ch).FeatFlags;

    setRequirement(ptr, flags);
}

TState::EParsed TParser::ParseImpl()
{
#ifdef DO_PRN
    PrintHead(UriStr.data(), "[Parsing]") << "URL";
    PrintTail(UriStr);
#endif

    if (!doParse(UriStr.data(), UriStr.length()))
    {
        if (!(Flags & TFeature::FeatureTryToFix)
            || !Sections[TField::FieldFrag].Beg)
            return TState::ParsedBadFormat;
        //Here: error was in fragment, just ignore it
        ResetSection(TField::FieldFrag);
    }

    if (NetPath && (Flags & TFeature::FeatureDenyNetworkPath))
        return TState::ParsedBadFormat;

    const TSection& scheme = Sections[TField::FieldScheme];
    if (scheme.IsSet())
        Scheme = TSchemeInfo::GetKind(scheme.Get());

    if (!HttpMode)
    {
        // opaque case happens
        switch (Scheme)
        {
        case TScheme::SchemeHTTP:
        case TScheme::SchemeHTTPS:
            return TState::ParsedBadFormat;
        case TScheme::SchemeEmpty:
            return TState::ParsedBadScheme;
        default:
            break;
        }

        if (!(Flags & TFeature::FeatureSchemeFlexible))
            return TState::ParsedBadScheme;

        return TState::ParsedOpaque;
    }

    checkSectionCollision(TField::FieldUser, TField::FieldHost);
    checkSectionCollision(TField::FieldPass, TField::FieldPort);

#ifdef DO_PRN
    Cout << "[Parsed]";
    for (int idx = 0; idx < TField::FieldMAX; ++idx)
    {
        const TSection& section = Sections[idx];
        if (section.IsSet())
            Cout << ' ' << TField::EField(idx) << "=[" << section.Get() << ']';
    }
    Cout << Endl;
#endif

    if (0 == (Flags & TFeature::FeatureAuthSupported))
        if (Sections[TField::FieldUser].IsSet() ||
            Sections[TField::FieldPass].IsSet())
            return TState::ParsedBadAuth;

    TSection& path = Sections[TField::FieldPath];
    if (path.IsSet())
        // remove trailing . in a /path/.
        if (2 <= path.Len() && '.' == path.End[-1] && '/' == path.End[-2])
            --path.End;

    TSection& host = Sections[TField::FieldHost];
    if (host.IsSet())
        for ( ; host.End != host.Beg && '.' == host.End[-1]; )
            --host.End;

    if (scheme.IsSet()) {
        long wantCareFlags = 0;
        switch (Scheme)
        {
        case TScheme::SchemeHTTP:
            break;
        case TScheme::SchemeEmpty:
            Scheme = TScheme::SchemeUnknown;
            /* no break */
        case TScheme::SchemeUnknown:
            wantCareFlags =
                TFeature::FeatureSchemeFlexible | TFeature::FeatureHierURI;
            break;
        default:
            wantCareFlags =
                TFeature::FeatureSchemeFlexible | TFeature::FeatureSchemeKnown;
            break;
        }

        if (0 != wantCareFlags && 0 == (Flags & wantCareFlags))
            return TState::ParsedBadScheme;
    }

    return TState::ParsedOK;
}

}
