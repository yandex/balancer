#include "codepage.h"
#include "normalization.h"

#include <util/stream/output.h>
#include <util/string/cast.h>

void CustomEncoder::addToTable(wchar32 ucode, unsigned char code, const CodePage* target) {
    unsigned char plane = (unsigned char)(ucode >> 8);
    unsigned char pos = (unsigned char)(ucode & 255);
    if (Table[plane] == DefaultPlane) {
        Table[plane] = new char[256];
        memset(Table[plane], 0, 256*sizeof(char));
    }

    if (Table[plane][pos] == 0) {
        Table[plane][pos] = code;
    } else {
        YASSERT(target && target->Names);
        if (static_cast<unsigned char>(Table[plane][pos]) > 127 && code) {
            Cerr << "WARNING: Only lower part of ASCII should have duplicate encodings "
                << target->Names[0]
                << " " << IntToString<16>(ucode)
                << " " << IntToString<16>(code)
                << " " << IntToString<16>(static_cast<unsigned char>(Table[plane][pos]))
            << Endl;
        }
    }
}

bool isGoodDecomp(wchar32 rune, wchar32 decomp){
    if (
           (NUnicode::NPrivate::CharInfo(rune) == NUnicode::NPrivate::CharInfo(decomp))
        || (IsAlpha(rune) && IsAlpha(decomp))
        || (IsNumeric(rune) && IsNumeric(decomp))
        || (IsQuotation(rune) && IsQuotation(decomp))
       )
    {
        return true;
    }
    return false;
}

void CustomEncoder::Create(const CodePage* target, bool extended) {
    YASSERT(target);

    DefaultChar = (const char*)target->DefaultChar;

    DefaultPlane = new char[256];

    memset(DefaultPlane, 0, 256*sizeof(char));
    for (size_t i = 0; i != 256; ++i)
        Table[i] = DefaultPlane;

    for (size_t i = 0; i != 256; ++i) {
        wchar32 ucode = target->unicode[i];
        if (ucode != BROKEN_RUNE) // always UNASSIGNED
            addToTable(ucode, (unsigned char) i, target);
    }

    if (!extended)
        return;

    for (wchar32 w = 1; w < 65535; w++){
        if (Code(w) == 0){
            wchar32 dw = w;
            while (IsComposed(dw) && Code(dw) == 0){
                const wchar32* decomp_p = NUnicode::Decomposition<true>(dw);
                YASSERT(decomp_p != NULL);

                dw = decomp_p[0];
                if (TCharTraits<wchar32>::GetLength(decomp_p) > 1 && (dw == (wchar32)' ' || dw == (wchar32)'('))
                    dw = decomp_p[1];
            }
            if (Code(dw) != 0 && isGoodDecomp(w, dw))
                addToTable(w, Code(dw), target);
        }
    }
}

CustomEncoder::~CustomEncoder() {
    for (size_t i = 0; i != 256; ++i) {
        if (Table[i] != DefaultPlane) {
            delete [] Table[i];
        }
    }
    delete [] DefaultPlane;
}

void Encoder::Tr(const wchar32* in, char* out, size_t len) const {
    while (len--)
        *out++ = Tr(*in++);
}

void Encoder::Tr(const wchar32* in, char* out) const {
    do {
        *out++ = Tr(*in);
    } while (*in++);
}

void Recoder::Create(const CodePage &source, const Encoder* wideTarget) {
    for (size_t i = 0; i != 256; ++i)
    {
        Table[i] = wideTarget->Tr(source.unicode[i]);
        YASSERT(Table[i] != 0 || i == 0);
    }
}

void Recoder::Create(const CodePage &page, const Encoder* widePage, wchar32 (*mapfunc)(wchar32)) {
    for (size_t i = 0; i != 256; ++i) {
        char c = widePage->Code((*mapfunc)(page.unicode[i]));
        Table[i] = (c == 0 && i != 0) ? (unsigned char)i : (unsigned char)c;
    }
}

void Recoder::Tr(const char* in, char* out, size_t len) const {
    while (len--)
        *out++ = Table[(unsigned char)*in++];
}

void Recoder::Tr(const char* in, char* out) const {
    do {
        *out++ = Table[(unsigned char)*in];
    } while (*in++);
}

void Recoder::Tr(char* in_out, size_t len) const {
    while (len--) {
        *in_out = Table[(unsigned char)*in_out];
        in_out++;
    }
}

void Recoder::Tr(char* in_out) const {
    // assuming that '\0' <--> '\0'
    do {
        *in_out = Table[(unsigned char)*in_out];
    } while (*in_out++);
}
