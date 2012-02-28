#pragma once

#include <cstdlib>

#include <util/generic/ptr.h>

#include "codepage.h"
#include "doccodes.h"
#include "iconv.h"
#include "recyr_int.hh"

///////////////////////////////////////////////////////////////////////////////////////
//     input buf -> output buf                                                       //
///////////////////////////////////////////////////////////////////////////////////////
template<class TCharType>
inline RECODE_RESULT RecodeToUnicode(ECharset From, const char* in, TCharType* out, size_t in_size, size_t out_size, size_t &in_readed, size_t &out_writed){
    STATIC_ASSERT(sizeof(TCharType) > 1);
    return NCodepagePrivate::_recodeToUnicode(From, in, out, in_size, out_size, in_readed, out_writed);
}

template<class TCharType>
inline RECODE_RESULT RecodeFromUnicode(ECharset To, const TCharType* in, char* out, size_t in_size, size_t out_size, size_t &in_readed, size_t &out_writed){
	STATIC_ASSERT(sizeof(TCharType) > 1);
    return NCodepagePrivate::_recodeFromUnicode(To, in, out, in_size, out_size, in_readed, out_writed);
}

template<class TCharType>
inline RECODE_RESULT RecodeToUnicode(ECharset From, const char* in, TCharType* out, size_t in_size, size_t out_size){
    size_t in_readed = 0;
    size_t out_writed = 0;
    return RecodeToUnicode(From, in, out, in_size, out_size, in_readed, out_writed);
}

template<class TCharType>
inline RECODE_RESULT RecodeFromUnicode(ECharset To, const TCharType* in, char* out, size_t in_size, size_t out_size){
    size_t in_readed = 0;
    size_t out_writed = 0;
    return RecodeFromUnicode(To, in, out, in_size, out_size, in_readed, out_writed);
}

inline
RECODE_RESULT RecodeFromUnicode(ECharset theEncoding, const wchar16* chars, size_t length,
                                char* bytes, size_t size, size_t* read = 0, size_t* written = 0)
{
    size_t w = 0, r = 0;
    RECODE_RESULT rc = ::RecodeFromUnicode(theEncoding, chars, bytes, length, size, r, w);
    if (read)
        *read = r;
    if (written)
        *written = w;
    return rc;
}

inline RECODE_RESULT Recode(ECharset From, ECharset To, const char* in, char* out, size_t in_size, size_t out_size, size_t &in_readed, size_t &out_writed){
    in_readed = 0;
    out_writed = 0;

    if (!ValidCodepage(To) || !ValidCodepage(From))
        return RECODE_ERROR;

    if (To == From)
        return NCodepagePrivate::_recodeCopy(in, out, in_size, out_size, in_readed, out_writed);

    if (NCodepagePrivate::NativeCodepage(From) && NCodepagePrivate::NativeCodepage(To)) {
        if (From == CODES_UTF8)
            return NCodepagePrivate::_recodeFromUTF8(To, in, out, in_size, out_size, in_readed, out_writed);
        if (To == CODES_UTF8)
            return NCodepagePrivate::_recodeToUTF8(From, in, out, in_size, out_size, in_readed, out_writed);
        if (From == CODES_YANDEX)
            return NCodepagePrivate::_recodeFromYandex(To, in, out, in_size, out_size, in_readed, out_writed);
        if (To == CODES_YANDEX)
            return NCodepagePrivate::_recodeToYandex(From, in, out, in_size, out_size, in_readed, out_writed);
    } else if (NICONVPrivate::CanConvert(From, To)) {
        return NICONVPrivate::RecodeNoThrow(From, To, in, out, in_size, out_size, in_readed, out_writed);
    }

    size_t wideSize = in_size * 3;
    TArrayHolder<TChar> wide(new TChar[wideSize]);

    size_t wideRead = 0;
    size_t wideWritten = 0;

    RECODE_RESULT res = RecodeToUnicode(From, in, wide.Get(), in_size, wideSize, in_readed, wideWritten);
    if (res != RECODE_OK)
        return res;

    res = RecodeFromUnicode(To, wide.Get(), out, wideWritten, out_size, wideRead, out_writed);

    return res;
}

inline RECODE_RESULT Recode(ECharset From, ECharset To, const char* in, char* out, size_t in_size, size_t out_size){
    size_t in_readed = 0;
    size_t out_writed = 0;
    return Recode(From, To, in, out, in_size, out_size, in_readed, out_writed);
}

/**
 * Recode from one charset to another; throw an exception if conversion failed
 * @param[in] srccs the source character set
 * @param[in] dstcs the target character set
 * @param[in] in    the input string buffer
 * @param[out] out  the output string object if conversion was successful
 * @return false if conversion was not attempted (charsets were the same),
 *         true if successful
 */
inline bool Recode(ECharset srccs, ECharset tgtcs, const TStringBuf &in, Stroka &out) {
    if (tgtcs == srccs)
        return false;

    const size_t in_size = in.length();
    const size_t out_size = SingleByteCodepage(tgtcs) ? in_size : 3 * in_size;
    out.clear(); // so we don't copy stuff around when resizing
    out.ReserveAndResize(out_size);

    size_t in_read = 0;
    size_t out_written = 0;
    const RECODE_RESULT res = Recode(srccs, tgtcs
        , in.c_str(), out.begin(), in_size, out_size, in_read, out_written);
    if (RECODE_OK != res)
        ythrow yexception() << "Recode failed";
    if (out_written > out_size)
        ythrow yexception() << "Recode overrun the buffer: size="
            << out_size << " need=" << out_written;

    out.remove(out_written);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////
//     Stroka -> Stroka                                                              //
///////////////////////////////////////////////////////////////////////////////////////
inline Stroka Recode(ECharset srccs, ECharset tgtcs, const Stroka &in) {
    Stroka out;
    return tgtcs != srccs && Recode(srccs, tgtcs, in, out) ? out : in;
}
inline Stroka RecodeToYandex(ECharset From, const Stroka &in){
    return Recode(From, CODES_YANDEX, in);
}
inline Stroka RecodeFromYandex(ECharset To, const Stroka &in){
    return Recode(CODES_YANDEX, To, in);
}

inline Stroka RecodeToHTMLEntities(ECharset From, const Stroka &in){
    RECODE_RESULT res;
    size_t out_writed, in_readed;
    Stroka out;
    out.resize(in.length()*(4+4));
    res = NCodepagePrivate::_recodeToHTMLEntities(From, in.c_str(), out.begin(), in.length(), out.length(), in_readed, out_writed);
    if (res == RECODE_EOOUTPUT) {//input contains many 8-byte characters?
        out.resize(in.length()*(4+8));
        res = NCodepagePrivate::_recodeToHTMLEntities(From, in.c_str(), out.begin(), in.length(), out.length(), in_readed, out_writed);
    }
    if (res != RECODE_OK) {
        ythrow yexception() << "Recode to HTML entities failed"; 
    }

    out.resize(out_writed - 1);
    return out;
}
