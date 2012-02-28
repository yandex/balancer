#pragma once

// #define DO_PRN

#include <cstddef>

#include "common.h"

#include <util/generic/strbuf.h>
#include <util/stream/output.h>
#include <util/string/cast.h>
#include <util/system/yassert.h>

namespace NUri {

class TParser;

class TSection
{
private:
    friend class TParser;

private:
    struct TRange
    {
        const char* Beg;
        long Flags;
        ui32 Encode;
        ui32 Decode;

        TRange(const char *beg = NULL)
            : Beg(beg)
            , Flags(0)
            , Encode(0)
            , Decode(0)
        {}

        void Reset(const char *beg = NULL)
        {
            *this = TRange(beg);
        }

        void AddFlag(const char *ptr, long flag)
        {
            if (0 != flag)
                AddFlagImpl(ptr, flag);
        }

        void AddFlagImpl(const char *ptr, long flag);
    };

private:
    const char* Beg;
    const char* End;
    long Flags;
    int Encode;

    TSection(const char *beg = NULL)
        : Beg(beg)
        , End(NULL)
        , Flags(0)
        , Encode(0)
    {}

    void Reset()
    {
        Enter(NULL);
    }

    void Reset(const char *pc)
    {
        YASSERT(!Beg || !pc || Beg < pc);
        Reset();
    }

    void Enter(const char* pc)
    {
        *this = TSection(pc);
    }

    bool Leave(const char* pc)
    {
        YASSERT(Beg);
        if (NULL != End)
            return false;
        End = pc;
        return true;
    }

    void Set(const TStringBuf& buf)
    {
        Enter(buf.data());
        Leave(buf.data() + buf.length());
    }

    void AddRange(const TRange &range, long mask);

public:
    bool IsSet() const
    {
        return End;
    }

    TStringBuf Get() const
    {
        return TStringBuf(Beg, End);
    }

    size_t Len() const
    {
        return End - Beg;
    }

    size_t EncodedLen() const
    {
        return Len() + 2 * Encode;
    }

    long GetFlags() const
    {
        return Flags;
    }
};

class TParser
{
public:
    TSection Sections[TField::FieldMAX];
    TScheme::EKind Scheme;
    bool HttpMode;
    bool NetPath;
    const TParseFlags Flags;
    const TStringBuf UriStr;
    TState::EParsed State;

public:
    TParser(const TParseFlags &flags, const TStringBuf& uri)
        : Scheme(TScheme::SchemeEmpty)
        , HttpMode(false)
        , NetPath(false)
        , Flags(flags | TFeature::FeatureDecodeANY)
        , UriStr(uri)
        , State(TState::ParsedEmpty)
        , HexValue(0)
    {
        YASSERT(0 == (Flags & TFeature::FeaturePathOperation)
            // can't define all of them
            || TFeature::FeaturesPath != (Flags & TFeature::FeaturesPath));
        State = ParseImpl();
    }

protected:
    static const TParseFlags FieldFlags[TField::FieldMAX];
    TSection::TRange CurRange;
    unsigned HexValue;

#ifdef DO_PRN
    TOutputStream& PrintAddr(const char *ptr) const
    {
        return Cout << "[" << IntToString<16>(ui64(ptr)) << "] ";
    }

    TOutputStream& PrintHead(const char *ptr, const char *func) const
    {
        return PrintAddr(ptr) << func << " ";
    }

    TOutputStream& PrintHead(const char *ptr
        , const char *func, const TField::EField &fld) const
    {
        return PrintHead(ptr, func) << fld;
    }

    TOutputStream& PrintTail(const TStringBuf &val) const
    {
        return Cout << " [" << val << "]" << Endl;
    }
    TOutputStream& PrintTail(const char* beg, const char *end) const
    {
        return PrintTail(TStringBuf(beg, end));
    }
#endif

    void ResetSection(TField::EField fld, const char* pc = NULL)
    {
#ifdef DO_PRN
        PrintHead(pc, __FUNCTION__, fld); PrintTail(pc);
#endif
        Sections[fld].Reset(pc);
    }

    void storeSection(const TStringBuf &val, TField::EField fld)
    {
#ifdef DO_PRN
        PrintHead(val.data(), __FUNCTION__, fld); PrintTail(val);
#endif
        Sections[fld].Set(val);
    }

    void startSection(const char* pc, TField::EField fld)
    {
#ifdef DO_PRN
        PrintHead(pc, __FUNCTION__, fld); PrintTail(pc);
#endif
        copyRequirements(pc);
        Sections[fld].Enter(pc);
    }

    void finishSection(const char* pc, TField::EField fld)
    {
#ifdef DO_PRN
        PrintHead(pc, __FUNCTION__, fld); PrintTail(pc);
#endif
        if (Sections[fld].Leave(pc))
            copyRequirements(pc);
    }

    void setRequirement(const char* ptr, long flags)
    {
#ifdef DO_PRN
        PrintHead(ptr, __FUNCTION__) << IntToString<16>(flags); PrintTail(ptr);
#endif
        CurRange.AddFlag(ptr, (Flags.Allow | Flags.Extra) & flags);
    }

    void copyRequirementsImpl(const char *ptr);
    void copyRequirements(const char* ptr)
    {
        if (NULL != CurRange.Beg && CurRange.Beg != ptr)
            copyRequirementsImpl(ptr);
    }

    void HexDigit(const char *ptr, char c)
    {
        UNUSED(ptr);
        HexAdd(c - '0');
    }
    void HexUpper(const char *ptr, char c)
    {
        UNUSED(ptr);
        HexAdd(c - 'A' + 10);
    }
    void HexLower(const char *ptr, char c)
    {
        setRequirement(ptr, TFeature::FeatureUpperEncoded);
        HexAdd(c - 'a' + 10);
    }
    void HexAdd(unsigned val)
    {
        HexValue <<= 4;
        HexValue += val;
    }
    void HexReset()
    {
        HexValue = 0;
    }
    void HexSet(const char* ptr);

    void checkSectionCollision(TField::EField fld1, TField::EField fld2)
    {
        if (Sections[fld1].IsSet() && Sections[fld2].IsSet() &&
            Sections[fld1].Beg == Sections[fld2].Beg)
        {
            Sections[fld1].Reset();
        }
    }

    bool doParse(const char* str_beg, size_t length);
    TState::EParsed ParseImpl();
};

}
