#pragma once

#include <util/system/defaults.h>
#include <util/charset/doccodes.h>

struct THtmlEntity {
    const char* name; // this name fixed by gperf
    ui16 Unicode;
    ui8 Ascii;
    bool IsHtmlCore;
};

const THtmlEntity* SelectHtmlEntity(const char* str, size_t len);

/******************************************************/
/*           step by step actions                     */
/******************************************************/

// with aggressive entities detection for standard base entities
wchar32 HtEntDecodeStep(ECharset cp, const unsigned char*& s, const unsigned char* end, unsigned char** map);

// no aggresive detection
wchar32 HtEntPureDecodeStep(ECharset cp, const unsigned char*& s, const unsigned char* end, unsigned char** map);

/******************************************************/
/*           complete actions                         */
/******************************************************/
size_t HtEntDecode(ECharset cp, const char* str, size_t len, wchar32* buffer, unsigned char* char_lengthes = 0);

size_t HtEntDecodeToYx(ECharset cp, const char* str, size_t len, char* dst);

size_t HtEntDecodeToChar(ECharset cp, const char* str, size_t len, ui16* buffer, unsigned char* char_lengthes = 0);

//! decodes HTML entities and converts non-ASCII characters to unicode, then converts unicode to UTF8 and percent-encodes
//! @param text     zero-terminated text of link
//! @param buffer   buffer receiving UTF8 percent-encoded text of link
//! @param buflen   length of output buffer
//! @param cp       code page object used to convert non-ASCII characters
//! @note HTML entities directly converted into unicode characters, non-ASCII characters
//!       converted into unicode using code page object if it is passed to the function,
//!       then unicode characters converted to UTF8 and percent-encoded,
//!       percent-encoded text in the link copied into output buffer as is
bool HtLinkDecode(const char* text, char* buffer, size_t buflen, ECharset cp = CODES_UNKNOWN);
