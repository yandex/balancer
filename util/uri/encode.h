#pragma once

#include "common.h"

#include <util/stream/output.h>

namespace NUri {
namespace NEncode {

#define CHAR_TYPE_NAME(f) _ECT ## f
#define CHAR_TYPE_FLAG(f) ECF ## f = 1u << CHAR_TYPE_NAME(f)

enum ECharType
{
    CHAR_TYPE_NAME(Digit),
    CHAR_TYPE_NAME(Lower),
    CHAR_TYPE_NAME(Upper),
    CHAR_TYPE_NAME(Unres),
    CHAR_TYPE_NAME(Stdrd),
    CHAR_TYPE_NAME(Field), // has fields for which it is safe to decode
};

enum ECharFlag
{
    CHAR_TYPE_FLAG(Digit),
    CHAR_TYPE_FLAG(Lower),
    CHAR_TYPE_FLAG(Upper),
    CHAR_TYPE_FLAG(Unres),
    CHAR_TYPE_FLAG(Stdrd),
    CHAR_TYPE_FLAG(Field),
    // compound group flags
    ECGAlpha = ECFUpper | ECFLower,
    ECGAlnum = ECGAlpha | ECFDigit,
    ECGUnres = ECGAlnum | ECFUnres,
    ECGStdrd = ECGUnres | ECFStdrd,
};

#undef CHAR_TYPE_NAME
#undef CHAR_TYPE_FLAG

struct TCharFlags
{
    ui32 TypeFlags;
    long FeatFlags;
    ui32 FieldMask;
    TCharFlags(ui32 type = 0, long feat = 0, ui32 mask = 0)
        : TypeFlags(type)
        , FeatFlags(feat)
        , FieldMask(mask)
    {}
    void Set(const TCharFlags &val)
    {
        TypeFlags |= val.TypeFlags;
        FeatFlags |= val.FeatFlags;
        FieldMask |= val.FieldMask;
    }
    bool IsAllowed(ui32 fldmask) const
    {
        return ((TypeFlags & ECGUnres))
            || ((TypeFlags & ECFField) && (FieldMask & fldmask))
            ;
    }
    bool IsDecode(ui32 fldmask, long flags) const
    {
        flags &= FeatFlags;
        if (flags & TFeature::FeaturesEncode)
            return false;
        return (flags & TFeature::FeaturesDecode)
            || ((flags & TFeature::FeatureDecodeFieldAllowed) && (FieldMask & fldmask))
            ;
    }
};

class TEncoder
{
public:
    TEncoder(TOutputStream& out
        , long flags, TField::EField fld = TField::FieldMAX);

    void ReEncode(const TStringBuf &url);
    void ReEncode(const char* str, size_t len)
    {
        ReEncode(TStringBuf(str, len));
    }

protected:
    static bool IsType(unsigned char c, long flags)
    {
        return GetFlags(c).TypeFlags & flags;
    }

public:
    static bool IsDigit(unsigned char c)
    {
        return IsType(c, ECFDigit);
    }
    static bool IsUpper(unsigned char c)
    {
        return IsType(c, ECFUpper);
    }
    static bool IsLower(unsigned char c)
    {
        return IsType(c, ECFLower);
    }
    static bool IsAlpha(unsigned char c)
    {
        return IsType(c, ECGAlpha);
    }
    static bool IsAlnum(unsigned char c)
    {
        return IsType(c, ECGAlnum);
    }
    static bool IsUnres(unsigned char c)
    {
        return IsType(c, ECGUnres);
    }
    static const TCharFlags &GetFlags(unsigned char c)
    {
        return Grammar.Get(c);
    }

public:
    // process an encoded string, decoding safe chars and encoding unsafe
    static TOutputStream& ReEncode(TOutputStream& out
        , const TStringBuf& val, long flags, TField::EField fld = TField::FieldMAX)
    {
        TEncoder(out, flags, fld).ReEncode(val);
        return out;
    }

    // see also UrlUnescape() from string/quote.h
    static TOutputStream& Decode(
        TOutputStream& out, const TStringBuf& val, long flags)
    {
        YASSERT(0 == (TFeature::FeaturesMaybeEncode & flags));
        return ReEncode(out, val, flags | TFeature::FeatureDecodeANY);
    }

public:
    // process a raw string or char, encode as needed
    static TOutputStream& Encode(TOutputStream& out, unsigned char val);
    static TOutputStream& EncodeAll(TOutputStream& out, const TStringBuf& val);
    static TOutputStream& EncodeNotAlnum(TOutputStream& out, const TStringBuf& val);

    static TOutputStream& EncodeField(TOutputStream& out
        , const TStringBuf& val, TField::EField fld);
    static TOutputStream& EncodeField(TOutputStream& out
        , const TStringBuf& val, TField::EField fld, long flags);

    static TOutputStream& Encode(TOutputStream& out, const TStringBuf& val)
    {
        return EncodeField(out, val, TField::FieldMAX);
    }

    static TOutputStream& Encode(TOutputStream& out, const TStringBuf& val, long flags)
    {
        return EncodeField(out, val, TField::FieldMAX, flags);
    }

public:
    class TGrammar
    {
        TCharFlags Map_[256];
    public:
        TGrammar();
        void Set(unsigned char ch, const TCharFlags &val)
        {
            Map_[ch].Set(val);
        }
        const TCharFlags &Get(unsigned char ch) const
        {
            return Map_[ch];
        }
    };

protected:
    static const TGrammar Grammar;

protected:
    TOutputStream& Out;
    const TField::EField Fld;
    const ui32 FldMask;
    const long Flags;
    int HexValue;

    const bool Q_DecodeAny;
    const bool Q_EncodeSpc;
    const bool Q_EncodeExt;

protected:
    void HexReset()
    {
        HexValue = 0;
    }

    void HexDigit(char c)
    {
        HexAdd(c - '0');
    }
    void HexUpper(char c)
    {
        HexAdd(c - 'A' + 10);
    }
    void HexLower(char c)
    {
        HexAdd(c - 'a' + 10);
    }

    void HexAdd(int val)
    {
        HexValue <<= 4;
        HexValue += val;
    }

protected:
    void WriteHex(unsigned char val)
    {
        Encode(Out, val);
    }

    void WriteSym(char c)
    {
        Out << c;
    }

    void DoSym(char);
    void DoHex();

    void DoEx(char ch)
    {
        if (Q_EncodeExt)
            WriteHex(ch);
        else
            WriteSym(ch);
    }
};

}

typedef NEncode::TEncoder TEncoder;

}
