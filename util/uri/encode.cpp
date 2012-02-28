#include "encode.h"

#include <util/string/cast.h>

namespace NUri {
namespace NEncode {

TEncoder::TEncoder(TOutputStream& out, long flags, TField::EField fld)
    : Out(out)
    , Fld(fld)
    , FldMask(1u << fld)
    , Flags(flags)
    , HexValue(0)
    , Q_DecodeAny(flags & TFeature::FeatureDecodeANY)
    , Q_EncodeSpc(flags & TFeature::FeatureEncodeSpaceAsPlus)
    , Q_EncodeExt(flags & TFeature::FeatureEncodeExtendedASCII)
{
}

namespace {

static inline void Add(TEncoder::TGrammar &grammar
    , char lo, char hi
    , ui32 type, long feat, ui32 mask = 0)
{
    const TCharFlags val(type, feat, mask);
    for ( ; lo <= hi; ++lo)
        grammar.Set(lo, val);
}

static inline void Add(TEncoder::TGrammar &grammar
    , const TStringBuf &set
    , ui32 type, long feat, ui32 mask = 0)
{
    const TCharFlags val(type, feat, mask);
    for (size_t i = 0; i != set.length(); ++i)
        grammar.Set(set[i], val);
}

}

// http://tools.ietf.org/html/rfc3986#section-2.2
#define GENDELIMS0 ":/?#[]@"
#define SUBDELIMS0 "!$&'()*+,;="
// http://tools.ietf.org/html/rfc3986#section-2.3
#define UNRESERVED "-._~"

// now find subsets which can sometimes be decoded

// remove '#' which can't ever be decoded
// don't mark anything allowed for pass (pass is completely encoded)
// safe in path, qry, frag
#define GENDELIMS1 ":@"
// allowed in qry, frag
#define GENDELIMS2 "/?"

// qry-unsafe chars
#define SUBDELIMS1 "&+=;"
// rest allowed in qry, frag
#define SUBDELIMS2 "!$'()*,"


const TEncoder::TGrammar TEncoder::Grammar;

// initialize the grammar map
TEncoder::TGrammar::TGrammar()
{
    // first set up unreserved characters safe in any field
    const long featUnres = TFeature::FeatureDecodeUnreserved;
    Add(*this, '0', '9', ECFDigit, featUnres);
    Add(*this, 'A', 'Z', ECFUpper, featUnres | TFeature::FeatureToLower);
    Add(*this, 'a', 'z', ECFLower, featUnres);
    Add(*this, UNRESERVED, ECFUnres, featUnres);

    // XXX: standard "safe" set used previously "-_.!~*();/:@$,", with comment:
    //  alnum + reserved + mark + ( '[', ']') - ('=' '+' '&' '\'' '"' '\\' '?')
    Add(*this, "!*();/:@$,", ECFStdrd, TFeature::FeatureDecodeStandardExtra);

    // now field-specific subsets of reserved characters (gen-delims + sub-delims)
    const long featSafe = TFeature::FeatureDecodeFieldAllowed;

    Add(*this, GENDELIMS1, ECFField, featSafe
        , TField::FlagPath | TField::FlagQuery | TField::FlagFrag);
    Add(*this, GENDELIMS2, ECFField, featSafe
        , TField::FlagQuery | TField::FlagFrag);

    Add(*this, SUBDELIMS1, ECFField, featSafe
        , TField::FlagUser);
    Add(*this, SUBDELIMS2, ECFField, featSafe
        , TField::FlagUser | TField::FlagQuery | TField::FlagFrag);

    // extended delims
    Add(*this, "[]|{}`^\"\\", 0, TFeature::FeatureEncodeExtendedDelim);

    // add characters with other features
    Set(' ', TCharFlags(0, TFeature::FeatureEncodeSpaceAsPlus));
    Add(*this, "'\"\\", 0, TFeature::FeatureEncodeForSQL);
}

TOutputStream& TEncoder::Encode(TOutputStream& out, unsigned char val)
{
    static const char sHexCodes[] = "0123456789ABCDEF";
    return out << '%' << sHexCodes [ (val>>4) & 0xF ] << sHexCodes [ val & 0xF ];
}

TOutputStream& TEncoder::EncodeAll(TOutputStream& out, const TStringBuf& val)
{
    for (size_t i = 0; i != val.length(); ++i)
        Encode(out, val[i]);
    return out;
}

TOutputStream& TEncoder::EncodeNotAlnum(TOutputStream& out, const TStringBuf& val)
{
    for (size_t i = 0; i != val.length(); ++i)
    {
        const char c = val[i];
        if (IsAlnum(c))
            out << c;
        else
            Encode(out, c);
    }
    return out;
}

TOutputStream& TEncoder::EncodeField(
    TOutputStream& out, const TStringBuf& val, TField::EField fld)
{
    const long fldmask = 1L << fld;
    for (size_t i = 0; i != val.length(); ++i)
    {
        const char ch = val[i];
        if (GetFlags(ch).IsAllowed(fldmask))
            out << ch;
        else
            Encode(out, ch);
    }
    return out;
}


TOutputStream& TEncoder::EncodeField(
    TOutputStream& out, const TStringBuf& val, TField::EField fld, long flags)
{
    const long fldmask = 1L << fld;
    for (size_t i = 0; i != val.length(); ++i)
    {
        const char ch = val[i];
        if (GetFlags(ch).IsDecode(fldmask, flags))
            out << ch;
        else
            Encode(out, ch);
    }
    return out;
}


const int dD = 'a' - 'A';

void TEncoder::DoHex()
{
    unsigned char ch = HexValue;
    HexValue = 0;

    do {
#define GOTOSYM continue
#define GOTOHEX break
        switch (0)
        {
        default:
            const TCharFlags &chflags = GetFlags(ch);
            const long flags = Flags & chflags.FeatFlags;

            if (flags & TFeature::FeatureToLower)
                ch += dD;

            // this is a special option for username/password
            if (Q_DecodeAny)
                GOTOSYM;

            if (128 <= ch)
                GOTOHEX;

            if (' ' == ch) {
                if (!Q_EncodeSpc)
                    GOTOHEX;
                ch = '+';
                GOTOSYM;
            }

            if (chflags.IsDecode(FldMask, flags))
                GOTOSYM;

            GOTOHEX;
        }
#undef GOTOSYM
#undef GOTOHEX

        WriteHex(ch);
        return;
    }
    while (false);

    WriteSym(ch);
}

void TEncoder::DoSym(char ch)
{
    const long flags = Flags & GetFlags(ch).FeatFlags;

    if (flags & TFeature::FeatureToLower)
        ch += dD;

    if (flags & TFeature::FeaturesEncode)
        WriteHex(ch);
    else
        WriteSym(ch);
}

}
}
