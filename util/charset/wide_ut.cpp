#include "wide.h"
#include "codepage.h"

#include <algorithm>

#include <library/unittest/registar.h>

#include <util/charset/recyr.hh>
#include <util/generic/hash_set.h>
#include <util/generic/static_assert.h>

#include <util/digest/numeric.h>

#if defined (_MSC_VER)
#   pragma warning(disable:4309) /*truncation of constant value*/
#endif

namespace {

    //! three UTF8 encoded russian letters (A, B, V)
    const char utext[] = { 0xd0, 0x90, 0xd0, 0x91, 0xd0, 0x92, 0x00 };

    const char asciiLatinAlphabet[] = "ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyz";
    const wchar16 wideLatinAlphabet[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'G', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'g', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0 };
    const char yandexCyrillicAlphabet[] = {
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,         // A - P
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,         // R - YA
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,         // a - p
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 0x00 }; // r - ya
    const wchar16 wideCyrillicAlphabet[] = {
        0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
        0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
        0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
        0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F, 0x00 };
    const char utf8CyrillicAlphabet[] = {
        0xd0, 0x90, 0xd0, 0x91, 0xd0, 0x92, 0xd0, 0x93, 0xd0, 0x94, 0xd0, 0x95, 0xd0, 0x96, 0xd0, 0x97,
        0xd0, 0x98, 0xd0, 0x99, 0xd0, 0x9a, 0xd0, 0x9b, 0xd0, 0x9c, 0xd0, 0x9d, 0xd0, 0x9e, 0xd0, 0x9f,
        0xd0, 0xa0, 0xd0, 0xa1, 0xd0, 0xa2, 0xd0, 0xa3, 0xd0, 0xa4, 0xd0, 0xa5, 0xd0, 0xa6, 0xd0, 0xa7,
        0xd0, 0xa8, 0xd0, 0xa9, 0xd0, 0xaa, 0xd0, 0xab, 0xd0, 0xac, 0xd0, 0xad, 0xd0, 0xae, 0xd0, 0xaf,
        0xd0, 0xb0, 0xd0, 0xb1, 0xd0, 0xb2, 0xd0, 0xb3, 0xd0, 0xb4, 0xd0, 0xb5, 0xd0, 0xb6, 0xd0, 0xb7,
        0xd0, 0xb8, 0xd0, 0xb9, 0xd0, 0xba, 0xd0, 0xbb, 0xd0, 0xbc, 0xd0, 0xbd, 0xd0, 0xbe, 0xd0, 0xbf,
        0xd1, 0x80, 0xd1, 0x81, 0xd1, 0x82, 0xd1, 0x83, 0xd1, 0x84, 0xd1, 0x85, 0xd1, 0x86, 0xd1, 0x87,
        0xd1, 0x88, 0xd1, 0x89, 0xd1, 0x8a, 0xd1, 0x8b, 0xd1, 0x8c, 0xd1, 0x8d, 0xd1, 0x8e, 0xd1, 0x8f, 0x00 };

    const wchar32 LEAD_BITS_MASK_2_BYTES = 0x1F;
    const wchar32 LEAD_BITS_MASK_3_BYTES = 0x0F;
    const wchar32 LEAD_BITS_MASK_4_BYTES = 0x03;

    wchar16 ws[] = {
      0x0009,
      0x000A, 0x2028, 0x2029,
      0x000B,
      0x000C,
      0x000D,
      0x0020, 0x1680, 0x180E,
        0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200A, 0x200B,
        0x202F, 0x205F, 0x3000,
      0x00A0
    };

    const size_t CaseTestDataSize = 10;
    wchar32 WideStringTestData[][CaseTestDataSize] = {
        { 0x01C4, 0x10428, 0x10429, 0x10447, 0x10441, 0x1C03, 0x00A0, 0x10400, 0x10415, 0x10437 }, // original
        { 0x01C6, 0x10428, 0x10429, 0x10447, 0x10441, 0x1C03, 0x00A0, 0x10428, 0x1043D, 0x10437 }, // lower
        { 0x01C4, 0x10400, 0x10401, 0x1041F, 0x10419, 0x1C03, 0x00A0, 0x10400, 0x10415, 0x1040F }, // upper
        { 0x01C5, 0x10428, 0x10429, 0x10447, 0x10441, 0x1C03, 0x00A0, 0x10428, 0x1043D, 0x10437 }, // title
    };

    Stroka CreateYandexText() {
        const int len = 256;
        char text[len] = { 0 };
        for (int i = 0; i < len; ++i) {
            text[i] = static_cast<char>(i);
        }
        return Stroka(text, len);
    }

    Wtroka CreateUnicodeText() {
        const int len = 256;
        wchar16 text[len] = {
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x00 - 0x0F
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x10 - 0x1F
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x20 - 0x2F
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x30 - 0x3F
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x40 - 0x4F
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x50 - 0x5F
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x60 - 0x6F
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x70 - 0x7F

            0x0301, 0x00C4, 0x00D6, 0x00DC, 0x0104, 0x0106, 0x0118, 0x0141, 0x00E0, 0x00E2, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x0490, 0x00AD, // 0x80 - 0x8F
            0x00DF, 0x00E4, 0x00F6, 0x00FC, 0x0105, 0x0107, 0x0119, 0x0142, 0x00EB, 0x00EE, 0x00EF, 0x00F4, 0x00F9, 0x00FB, 0x0491, 0x92CF, // 0x90 - 0x9F
            0x00A0, 0x0143, 0x00D3, 0x015A, 0x017B, 0x0179, 0x046C, 0x00A7, 0x0401, 0x0462, 0x0472, 0x0474, 0x040E, 0x0406, 0x0404, 0x0407, // 0xA0 - 0xAF
            0x00B0, 0x0144, 0x00F3, 0x015B, 0x017C, 0x017A, 0x046D, 0x2116, 0x0451, 0x0463, 0x0473, 0x0475, 0x045E, 0x0456, 0x0454, 0x0457  // 0xB0 - 0xBF
        };
        for (int i = 0; i < len; ++i) {
            if (i <= 0x7F) { // ASCII characters without 0x7 and 0x1B
                text[i] = static_cast<wchar16>(i);
            } else if (i >= 0xC0 && i <= 0xFF) { // russian characters (without YO and yo)
                text[i] = static_cast<wchar16>(i + 0x0350); // 0x0410 - 0x044F
            }
        }
        return Wtroka(text, len);
    }

    Stroka CreateUTF8Text() {
        char text[] = {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
            0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
            0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
            0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
            0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
            0xcc, 0x81, 0xc3, 0x84, 0xc3, 0x96, 0xc3, 0x9c, 0xc4, 0x84, 0xc4, 0x86, 0xc4, 0x98, 0xc5, 0x81,
            0xc3, 0xa0, 0xc3, 0xa2, 0xc3, 0xa7, 0xc3, 0xa8, 0xc3, 0xa9, 0xc3, 0xaa, 0xd2, 0x90, 0xc2, 0xad,
            0xc3, 0x9f, 0xc3, 0xa4, 0xc3, 0xb6, 0xc3, 0xbc, 0xc4, 0x85, 0xc4, 0x87, 0xc4, 0x99, 0xc5, 0x82,
            0xc3, 0xab, 0xc3, 0xae, 0xc3, 0xaf, 0xc3, 0xb4, 0xc3, 0xb9, 0xc3, 0xbb, 0xd2, 0x91, 0xe9, 0x8b,
            0x8f, 0xc2, 0xa0, 0xc5, 0x83, 0xc3, 0x93, 0xc5, 0x9a, 0xc5, 0xbb, 0xc5, 0xb9, 0xd1, 0xac, 0xc2,
            0xa7, 0xd0, 0x81, 0xd1, 0xa2, 0xd1, 0xb2, 0xd1, 0xb4, 0xd0, 0x8e, 0xd0, 0x86, 0xd0, 0x84, 0xd0,
            0x87, 0xc2, 0xb0, 0xc5, 0x84, 0xc3, 0xb3, 0xc5, 0x9b, 0xc5, 0xbc, 0xc5, 0xba, 0xd1, 0xad, 0xe2,
            0x84, 0x96, 0xd1, 0x91, 0xd1, 0xa3, 0xd1, 0xb3, 0xd1, 0xb5, 0xd1, 0x9e, 0xd1, 0x96, 0xd1, 0x94,
            0xd1, 0x97, 0xd0, 0x90, 0xd0, 0x91, 0xd0, 0x92, 0xd0, 0x93, 0xd0, 0x94, 0xd0, 0x95, 0xd0, 0x96,
            0xd0, 0x97, 0xd0, 0x98, 0xd0, 0x99, 0xd0, 0x9a, 0xd0, 0x9b, 0xd0, 0x9c, 0xd0, 0x9d, 0xd0, 0x9e,
            0xd0, 0x9f, 0xd0, 0xa0, 0xd0, 0xa1, 0xd0, 0xa2, 0xd0, 0xa3, 0xd0, 0xa4, 0xd0, 0xa5, 0xd0, 0xa6,
            0xd0, 0xa7, 0xd0, 0xa8, 0xd0, 0xa9, 0xd0, 0xaa, 0xd0, 0xab, 0xd0, 0xac, 0xd0, 0xad, 0xd0, 0xae,
            0xd0, 0xaf, 0xd0, 0xb0, 0xd0, 0xb1, 0xd0, 0xb2, 0xd0, 0xb3, 0xd0, 0xb4, 0xd0, 0xb5, 0xd0, 0xb6,
            0xd0, 0xb7, 0xd0, 0xb8, 0xd0, 0xb9, 0xd0, 0xba, 0xd0, 0xbb, 0xd0, 0xbc, 0xd0, 0xbd, 0xd0, 0xbe,
            0xd0, 0xbf, 0xd1, 0x80, 0xd1, 0x81, 0xd1, 0x82, 0xd1, 0x83, 0xd1, 0x84, 0xd1, 0x85, 0xd1, 0x86,
            0xd1, 0x87, 0xd1, 0x88, 0xd1, 0x89, 0xd1, 0x8a, 0xd1, 0x8b, 0xd1, 0x8c, 0xd1, 0x8d, 0xd1, 0x8e,
            0xd1, 0x8f
        };
        return Stroka(text, ARRAY_SIZE(text));
    }

    //! use this function to dump UTF8 text into a file in case of any changes
//    void DumpUTF8Text() {
//        Stroka s = WideToUTF8(UnicodeText);
//        NStl::ofstream f("utf8.txt");
//        f << NStl::hex;
//        for (int i = 0; i < (int)s.size(); ++i) {
//            f << "0x" << NStl::setw(2) << NStl::setfill('0') << (int)(ui8)s[i] << ", ";
//            if ((i + 1) % 16 == 0)
//                f << NStl::endl;
//        }
//    }

    void CheckRecodeOK(wchar32 expected, unsigned char* first, size_t n) {
        wchar32 w = 0;
        const unsigned char* p = first;

        RECODE_RESULT r = NDetail::ReadUTF8Char(w, p, first + n);
        UNIT_ASSERT(w == expected);
        UNIT_ASSERT(size_t(p - first) == n);
        UNIT_ASSERT(r == RECODE_OK);
    }

    void CheckBrokenSymbol(unsigned char* first, unsigned char* last) {
        wchar32 w = 0;
        const unsigned char* p = first;

        RECODE_RESULT r = NDetail::ReadUTF8Char(w, p, last);
        UNIT_ASSERT(w == BROKEN_RUNE);
        UNIT_ASSERT(p - first == 0);
        UNIT_ASSERT(r == RECODE_BROKENSYMBOL);
    }

    void CheckEndOfInput(unsigned char* first, size_t n) {
        wchar32 w = 0;
        const unsigned char* p = first;

        RECODE_RESULT r = NDetail::ReadUTF8Char(w, p, first + n);
        (void)w;
        UNIT_ASSERT(p - first == 0);
        UNIT_ASSERT(r == RECODE_EOINPUT);
    }

    void CheckCharLen(unsigned char* first, unsigned char* last, size_t len, RECODE_RESULT result) {
        size_t n = 0;
        RECODE_RESULT r = NDetail::GetUTF8CharLen(n, first, last);
        UNIT_ASSERT(n == len);
        UNIT_ASSERT(r == result);
    }

}

//! this unit tests ensure validity of Yandex-Unicode and UTF8-Unicode conversions
//! @note only those conversions are verified because they are used in index
class TConversionTest : public TTestBase {
private:
    //! @note every of the text can have zeros in the middle
    const Stroka YandexText;
    const Wtroka UnicodeText;
    const Stroka UTF8Text;

private:
    UNIT_TEST_SUITE(TConversionTest);
        UNIT_TEST(TestReadUTF8Char);
        UNIT_TEST(TestGetUTF8CharLen);
        UNIT_TEST(TestWriteUTF8Char);
        UNIT_TEST(TestCharToWide);
        UNIT_TEST(TestWideToChar);
        UNIT_TEST(TestUTF8ToWide);
        UNIT_TEST(TestWideToUTF8);
        UNIT_TEST(TestRecodeIntoString);
        UNIT_TEST(TestRecodeAppend);
        UNIT_TEST(TestGetNumOfUTF8Chars);
        UNIT_TEST(TestRecode);
        UNIT_TEST(TestUnicodeLimit);
        UNIT_TEST(TestUnicodeCase);
        UNIT_TEST(TestUnicodeDetails);
    UNIT_TEST_SUITE_END();

public:
    TConversionTest()
        : YandexText(CreateYandexText())
        , UnicodeText(CreateUnicodeText())
        , UTF8Text(CreateUTF8Text())
    {
    }

    void TestReadUTF8Char();
    void TestGetUTF8CharLen();
    void TestWriteUTF8Char();
    void TestCharToWide();
    void TestWideToChar();
    void TestUTF8ToWide();
    void TestWideToUTF8();
    void TestRecodeIntoString();
    void TestRecodeAppend();
    void TestGetNumOfUTF8Chars();
    void TestRecode();
    void TestUnicodeLimit();
    void TestUnicodeCase();
    void TestUnicodeDetails();
};

UNIT_TEST_SUITE_REGISTRATION(TConversionTest);

void TConversionTest::TestReadUTF8Char() {
    wchar32 e; // expected unicode char
    wchar32 c;
    unsigned long u; // single UTF8 encoded character
    unsigned char* const first = reinterpret_cast<unsigned char*>(&u);
    unsigned char* const last = first + sizeof(u);

    // all ASCII characters are converted with no change (zero converted successfully as well)
    for (c = 0; c <= 0x7F; ++c) {
        u = c;
        CheckRecodeOK(c, first, 1);
    }

    // broken symbols from the second half of ASCII table (1000 0000 - 1011 1111)
    for (c = 0x80; c <= 0xBF; ++c) {
        u = c;
        CheckBrokenSymbol(first, last);
    }

    // leading byte of 2-byte symbol: 1100 0000 - 1101 1111
    for (c = 0xC0; c <= 0xDF; ++c) {
        u = c;
        CheckBrokenSymbol(first, last);

        u |= 0x8000;
        // w: 0000 0000  0000 0000 - 0000 0111  1100 0000
        e = c & LEAD_BITS_MASK_2_BYTES;
        e <<= 6;
        CheckRecodeOK(e, first, 2);

        CheckEndOfInput(first, 1);
    }

    // leading byte of 3-byte symbol: 1110 0000 - 1110 1111
    for (c = 0xE0; c <= 0xEF; ++c) {
        u = c;
        CheckBrokenSymbol(first, last);

        u |= 0x808000;
        // w: 0000 0000  0000 0000 - 0000 0111  1100 0000
        e = c & LEAD_BITS_MASK_3_BYTES;
        e <<= 12;
        CheckRecodeOK(e, first, 3);

        CheckEndOfInput(first, 2);
        CheckEndOfInput(first, 1);
    }

    // leading byte of 4-byte symbol: 1111 0000 - 1111 0111
    for (c = 0xF0; c <= 0xF3; ++c) {
        u = c;
        CheckBrokenSymbol(first, last);

        u |= 0x80808000;
        // w: 0000 0000  0000 0000 - 0000 0111  1100 0000
        e = c & LEAD_BITS_MASK_4_BYTES;
        e <<= 18;
        CheckRecodeOK(e, first, 4);

        CheckEndOfInput(first, 3);
        CheckEndOfInput(first, 2);
        CheckEndOfInput(first, 1);
    }

    // broken symbols: 1111 1000 - 1111 1111
    for (c = 0xF8; c <= 0xFF; ++c) {
        u = c;
        CheckBrokenSymbol(first, last);
    }
}

void TConversionTest::TestGetUTF8CharLen() {
    wchar32 c;
    unsigned long u; // single UTF8 encoded character
    unsigned char* const first = reinterpret_cast<unsigned char*>(&u);
    unsigned char* const last = first + sizeof(u);

    // all ASCII characters are converted with no change (zero converted successfully as well)
    for (c = 0; c <= 0x7F; ++c) {
        u = c;
        CheckCharLen(first, last, 1, RECODE_OK);
    }

    // broken symbols from the second half of ASCII table (1000 0000 - 1011 1111)
    for (c = 0x80; c <= 0xBF; ++c) {
        u = c;
        CheckCharLen(first, last, 0, RECODE_BROKENSYMBOL);
    }

    // leading byte of 2-byte symbol: 1100 0000 - 1101 1111
    for (c = 0xC0; c <= 0xDF; ++c) {
        u = c;
        CheckCharLen(first, last, 0, RECODE_BROKENSYMBOL);

        u |= 0x8000;
        // w: 0000 0000  0000 0000 - 0000 0111  1100 0000
        CheckCharLen(first, last, 2, RECODE_OK);

        CheckCharLen(first, first + 1, 0, RECODE_EOINPUT);
    }

    // leading byte of 3-byte symbol: 1110 0000 - 1110 1111
    for (c = 0xE0; c <= 0xEF; ++c) {
        u = c;
        CheckCharLen(first, last, 0, RECODE_BROKENSYMBOL);

        u |= 0x808000;
        // w: 0000 0000  0000 0000 - 0000 0111  1100 0000
        CheckCharLen(first, last, 3, RECODE_OK);

        CheckCharLen(first, first + 2, 0, RECODE_EOINPUT);
        CheckCharLen(first, first + 1, 0, RECODE_EOINPUT);
    }

    // leading byte of 4-byte symbol: 1111 0000 - 1111 0111
    for (c = 0xF0; c <= 0xF3; ++c) {
        u = c;
        CheckCharLen(first, last, 0, RECODE_BROKENSYMBOL);

        u |= 0x80808000;
        // w: 0000 0000  0000 0000 - 0000 0111  1100 0000
        CheckCharLen(first, last, 4, RECODE_OK);

        CheckCharLen(first, first + 3, 0, RECODE_EOINPUT);
        CheckCharLen(first, first + 2, 0, RECODE_EOINPUT);
        CheckCharLen(first, first + 1, 0, RECODE_EOINPUT);
    }

    // broken symbols: 1111 1000 - 1111 1111
    for (c = 0xF8; c <= 0xFF; ++c) {
        u = c;
        CheckCharLen(first, last, 0, RECODE_BROKENSYMBOL);
    }
}

void TConversionTest::TestWriteUTF8Char() {
    wchar32 w;
    unsigned long u; // single UTF8 encoded character
    size_t n;

    for (w = 0x00; w < 0x80; ++w) {
        u = 0;
        NDetail::WriteUTF8Char(w, n, reinterpret_cast<unsigned char*>(&u));
        UNIT_ASSERT((u & 0xFFFFFF80) == 0x00000000);
        UNIT_ASSERT(n == 1);
    }

    for (w = 0x80; w < 0x800; ++w) {
        u = 0;
        NDetail::WriteUTF8Char(w, n, reinterpret_cast<unsigned char*>(&u));
        UNIT_ASSERT((u & 0xFFFFC000) == 0x00008000); // see constants in ReadUTF8Char
        UNIT_ASSERT(n == 2);
    }

    for (w = 0x800; w < 0x10000; ++w) {
        u = 0;
        NDetail::WriteUTF8Char(w, n, reinterpret_cast<unsigned char*>(&u));
        UNIT_ASSERT((u & 0xFFC0C000) == 0x00808000); // see constants in ReadUTF8Char
        UNIT_ASSERT(n == 3);
    }

    for (w = 0x10000; w < 0x80; ++w) {
        NDetail::WriteUTF8Char(w, n, reinterpret_cast<unsigned char*>(&u));
        UNIT_ASSERT((u & 0xC0C0C000) == 0x80808000); // see constants in ReadUTF8Char
        UNIT_ASSERT(n == 4);
    }
}

void TConversionTest::TestCharToWide() {
    Wtroka w = CharToWide(YandexText);

    UNIT_ASSERT(w.size() == 256);
    UNIT_ASSERT(w.size() == UnicodeText.size());

    for (int i = 0; i < 256; ++i) {
        UNIT_ASSERT_VALUES_EQUAL(w[i], UnicodeText[i]);
    }
}

void TConversionTest::TestWideToChar() {
    Stroka s = WideToChar(UnicodeText);

    UNIT_ASSERT(s.size() == 256);
    UNIT_ASSERT(s.size() == YandexText.size());

    for (int i = 0; i < 256; ++i) {
        UNIT_ASSERT_VALUES_EQUAL(s[i], YandexText[i]);
    }
}

static void TestSurrogates(const char* str, const wchar16* wide, size_t wideSize, ECharset enc) {
    Wtroka w = UTF8ToWide(str);

    UNIT_ASSERT(w.size() == wideSize);
    UNIT_ASSERT(!memcmp(w.c_str(), wide, wideSize));

    Stroka s = WideToChar(w, enc);

    UNIT_ASSERT(s == str);
}

void TConversionTest::TestUTF8ToWide() {
    Wtroka w = UTF8ToWide(UTF8Text);

    UNIT_ASSERT(w.size() == 256);
    UNIT_ASSERT(w.size() == UnicodeText.size());

    for (int i = 0; i < 256; ++i) {
        UNIT_ASSERT_VALUES_EQUAL(w[i], UnicodeText[i]);
    }

    wchar16 buffer[4] = { 0 };
    size_t written = 0;
    // the function must extract 2 symbols only
    bool result = UTF8ToWide(utext, 5, buffer, written);
    UNIT_ASSERT(!result);
    UNIT_ASSERT(buffer[0] == 0x0410);
    UNIT_ASSERT(buffer[1] == 0x0411);
    UNIT_ASSERT(buffer[2] == 0x0000);
    UNIT_ASSERT(buffer[3] == 0x0000);
    UNIT_ASSERT(written == 2);

    memset(buffer, 0, 4);
    written = 0;
    result = UTF8ToWide(utext, 1, buffer, written);
    UNIT_ASSERT(!result);
    UNIT_ASSERT(buffer[0] == 0x0000);
    UNIT_ASSERT(buffer[1] == 0x0000);
    UNIT_ASSERT(buffer[2] == 0x0000);
    UNIT_ASSERT(buffer[3] == 0x0000);
    UNIT_ASSERT(written == 0);

    w = UTF8ToWide(asciiLatinAlphabet, strlen(asciiLatinAlphabet), csYandex);
    UNIT_ASSERT(w == wideLatinAlphabet);
    w = UTF8ToWide(utf8CyrillicAlphabet, strlen(utf8CyrillicAlphabet), csYandex);
    UNIT_ASSERT(w == wideCyrillicAlphabet);
    w = UTF8ToWide(yandexCyrillicAlphabet, strlen(yandexCyrillicAlphabet), csYandex);
    UNIT_ASSERT(w == wideCyrillicAlphabet);

    const char* utf8NonBMP = "\xf4\x80\x89\x84\xf4\x80\x89\x87\xf4\x80\x88\xba";
    wchar16 wNonBMPDummy[] = { 0xDBC0, 0xDE44, 0xDBC0, 0xDE47, 0xDBC0, 0xDE3A };
    TestSurrogates(utf8NonBMP, wNonBMPDummy, ARRAY_SIZE(wNonBMPDummy), CODES_UTF8);

    const char* utf8NonBMP2 = "ab\xf4\x80\x89\x87n";
    wchar16 wNonBMPDummy2[] = { 'a', 'b', 0xDBC0, 0xDE47, 'n' };
    TestSurrogates(utf8NonBMP2, wNonBMPDummy2, ARRAY_SIZE(wNonBMPDummy2), CODES_UTF8);

    {
        const char* yandexNonBMP2 = "ab?n";
        UNIT_ASSERT(yandexNonBMP2 == WideToChar(wNonBMPDummy2, ARRAY_SIZE(wNonBMPDummy2), CODES_YANDEX));

        Stroka temp;
        temp.resize(ARRAY_SIZE(wNonBMPDummy2));
        size_t read = 0;
        size_t written = 0;
        RecodeFromUnicode(CODES_YANDEX, wNonBMPDummy2, temp.begin(), ARRAY_SIZE(wNonBMPDummy2), temp.size(), read, written);
        temp.remove(written);

        UNIT_ASSERT(yandexNonBMP2 == temp);
    }

    UNIT_ASSERT_VALUES_EQUAL(WideToUTF8(UTF8ToWide(WideToUTF8(UTF8ToWide<true>(
                    "m\xFB\xB2\xA5\xAA\xAFyeuse.sexwebcamz.com")))), Stroka(
                    "m\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBDyeuse.sexwebcamz.com"));
}

void TConversionTest::TestWideToUTF8() {
    Stroka s = WideToUTF8(UnicodeText);
    size_t len = 0;
    for (Wtroka::const_iterator i = UnicodeText.begin(), ie = UnicodeText.end(); i != ie; ++i)
        len += utf8_rune_len_by_ucs(*i);

    UNIT_ASSERT(s.size() == UTF8Text.size());
    UNIT_ASSERT(s.size() == len);

    for (int i = 0; i < static_cast<int>(s.size()); ++i) {
        UNIT_ASSERT_VALUES_EQUAL(s[i], UTF8Text[i]);
    }
}

void TConversionTest::TestRecodeIntoString() {
    Stroka sYandex(UnicodeText.size()*4, 'x');
    const char* sdata = ~sYandex;
    TStringBuf sres = NDetail::Recode<wchar16>(UnicodeText, sYandex, CODES_YANDEX);
    UNIT_ASSERT( sYandex == YandexText);        // same content
    UNIT_ASSERT(~sYandex == sdata);             // reserved buffer reused
    UNIT_ASSERT(~sYandex == ~sres);             // same buffer
    UNIT_ASSERT(+sYandex == +sres);             // same size

    Wtroka sUnicode;
    sUnicode.reserve(YandexText.size() * 4);
    const wchar16* wdata = ~sUnicode;
    TWtringBuf wres = NDetail::Recode<char>(YandexText, sUnicode, CODES_YANDEX);
    UNIT_ASSERT( sUnicode == UnicodeText);      // same content
    UNIT_ASSERT(~sUnicode == wdata);            // reserved buffer reused
    UNIT_ASSERT(~sUnicode == ~wres);            // same buffer
    UNIT_ASSERT(+sUnicode == +wres);            // same size

    Stroka sUtf8 = " ";
    size_t scap = sUtf8.capacity();
    sres = NDetail::Recode<wchar16>(UnicodeText, sUtf8, CODES_UTF8);
    UNIT_ASSERT( sUtf8 == UTF8Text);            // same content
    UNIT_ASSERT(sUtf8.capacity() > scap);       // increased buffer capacity (supplied was too small)
    UNIT_ASSERT(~sUtf8 == ~sres);               // same buffer
    UNIT_ASSERT(+sUtf8 == +sres);               // same size

    sUnicode.clear();
    wdata = ~sUnicode;
    Wtroka copy = sUnicode;     // increase ref-counter
    wres = NDetail::Recode<char>(UTF8Text, sUnicode, CODES_UTF8);
    UNIT_ASSERT( sUnicode == UnicodeText);      // same content
    UNIT_ASSERT(~sUnicode != wdata);            // re-allocated (shared buffer supplied)
    UNIT_ASSERT(~sUnicode == ~wres);            // same buffer
    UNIT_ASSERT(+sUnicode == +wres);            // same content
}

static Stroka GenerateJunk(size_t seed) {
    Stroka res;
    size_t hash = NumericHash(seed);
    size_t size = hash % 1024;
    res.reserve(size);
    for (size_t i = 0; i < size; ++i)
        res += static_cast<char>(NumericHash(hash + i) % 256);
    return res;
}

void TConversionTest::TestRecodeAppend() {
    {
        Stroka s1, s2;
        NDetail::RecodeAppend<wchar16>(Wtroka(), s1, CODES_YANDEX);
        UNIT_ASSERT(s1.empty());

        NDetail::RecodeAppend<wchar16>(UnicodeText, s1, CODES_WIN);
        s2 += WideToChar(UnicodeText, CODES_WIN);
        UNIT_ASSERT_EQUAL(s1, s2);

        NDetail::RecodeAppend<wchar16>(UnicodeText, s1, CODES_YANDEX);
        s2 += WideToChar(UnicodeText, CODES_YANDEX);
        UNIT_ASSERT_EQUAL(s1, s2);

        NDetail::RecodeAppend<wchar16>(Wtroka(), s1, CODES_YANDEX);
        UNIT_ASSERT_EQUAL(s1, s2);

        NDetail::RecodeAppend<wchar16>(UnicodeText, s1, CODES_UTF8);
        s2 += WideToChar(UnicodeText, CODES_UTF8);
        UNIT_ASSERT_EQUAL(s1, s2);

        for (size_t i = 0; i < 100; ++i) {
            Wtroka junk = CharToWide(GenerateJunk(i), CODES_YANDEX);
            NDetail::RecodeAppend<wchar16>(junk, s1, CODES_UTF8);
            s2 += WideToChar(junk, CODES_UTF8);
            UNIT_ASSERT_EQUAL(s1, s2);
        }
    }

    {
        Wtroka s1, s2;
        NDetail::RecodeAppend<char>(Stroka(), s1, CODES_YANDEX);
        UNIT_ASSERT(s1.empty());

        NDetail::RecodeAppend<char>(YandexText, s1, CODES_WIN);
        s2 += CharToWide(YandexText, CODES_WIN);
        UNIT_ASSERT_EQUAL(s1, s2);

        NDetail::RecodeAppend<char>(YandexText, s1, CODES_YANDEX);
        s2 += CharToWide(YandexText, CODES_YANDEX);
        UNIT_ASSERT_EQUAL(s1, s2);

        NDetail::RecodeAppend<char>(Stroka(), s1, CODES_YANDEX);
        UNIT_ASSERT_EQUAL(s1, s2);

        NDetail::RecodeAppend<char>(UTF8Text, s1, CODES_UTF8);
        s2 += CharToWide(UTF8Text, CODES_UTF8);
        UNIT_ASSERT_EQUAL(s1, s2);

        for (size_t i = 0; i < 100; ++i) {
            Stroka junk = GenerateJunk(i);
            NDetail::RecodeAppend<char>(junk, s1, CODES_YANDEX);
            s2 += CharToWide(junk, CODES_YANDEX);
            UNIT_ASSERT_EQUAL(s1, s2);
        }
    }
}

void TConversionTest::TestGetNumOfUTF8Chars() {
    size_t n = 0;
    bool result = GetNumberOfUTF8Chars(UTF8Text.c_str(), UTF8Text.size(), n);
    UNIT_ASSERT(result);
    UNIT_ASSERT(n == 256);

    n = 0;
    result = GetNumberOfUTF8Chars(utext, 5, n);
    UNIT_ASSERT(!result);
    UNIT_ASSERT(n == 2);

    n = 0;
    result = GetNumberOfUTF8Chars(utext, 1, n);
    UNIT_ASSERT(!result);
    UNIT_ASSERT(n == 0);
}

void TConversionTest::TestRecode() {
    for (int c = 0; c != CODES_MAX; ++c) {
        ECharset enc = static_cast<ECharset>(c);
        if (!SingleByteCodepage(enc))
            continue;

        typedef yhash_set<char> THash;
        THash hash;

        for (int i = 0; i != 256; ++i) {
            char ch = static_cast<char>(i);

            wchar32 wch;
            size_t read = 0;
            size_t written = 0;
            RECODE_RESULT res = RECODE_ERROR;

            res = RecodeToUnicode(enc, &ch, &wch, 1, 1, read, written);
            UNIT_ASSERT(res == RECODE_OK);
            if (wch == BROKEN_RUNE)
                continue;

            char rch = 0;
            res = RecodeFromUnicode(enc, &wch, &rch, 1, 1, read, written);
            UNIT_ASSERT(res == RECODE_OK);

            if (hash.has(rch)) { // there are some stupid encodings with duplicate characters
                continue;
            } else {
                hash.insert(rch);
            }

            UNIT_ASSERT(ch == rch);
        }
    }
}

void TConversionTest::TestUnicodeLimit() {
    for (int i = 0; i != CODES_MAX; ++i) {
        ECharset code = static_cast<ECharset>(i);
        if (!SingleByteCodepage(code))
            continue;

        const CodePage* page = CodePageByCharset(code);
        YASSERT(page);

        for (int c = 0; c < 256; ++c) {
            UNIT_ASSERT(page->unicode[c] < 1 << 16);
        }
    }
}

inline bool MustBeSurrogate(wchar32 ch) {
    return ch > 0xFFFF;
}

void TConversionTest::TestUnicodeCase() {
    // ToLower, ToUpper, ToTitle functions depend on equal size of both original and changed characters
    for (wchar32 i = 0; i != NUnicode::UnicodeInstancesLimit(); ++i) {
        UNIT_ASSERT(MustBeSurrogate(i) == MustBeSurrogate(ToLower(i)));
        UNIT_ASSERT(MustBeSurrogate(i) == MustBeSurrogate(ToUpper(i)));
        UNIT_ASSERT(MustBeSurrogate(i) == MustBeSurrogate(ToTitle(i)));
    }
}

void TConversionTest::TestUnicodeDetails() {
    Wtroka temp;
    for (wchar32 i = 0; i != NUnicode::UnicodeInstancesLimit(); ++i) {
        temp.clear();
        WriteSymbol(i, temp);
        UNIT_ASSERT(temp.size() == NDetail::SymbolSize(temp.c_str(), temp.c_str() + temp.size()));
    }
}

class TWideUtilTest: public TTestBase
{
    UNIT_TEST_SUITE(TWideUtilTest);
        UNIT_TEST(TestCollapse);
        UNIT_TEST(TestCollapseBuffer);
        UNIT_TEST(TestStrip);
        UNIT_TEST(TestIsSpace);
        UNIT_TEST(TestEscapeHtmlChars);
        UNIT_TEST(TestToLower);
        UNIT_TEST(TestToUpper);
        UNIT_TEST(TestWideString);
    UNIT_TEST_SUITE_END();

public:
    void TestCollapse() {
        Wtroka s;
        s.append(ws, ARRAY_SIZE(ws)).append(3, 'a').append(ws, ARRAY_SIZE(ws)).append(3, 'b').append(ws, ARRAY_SIZE(ws));
        Collapse(s);
        UNIT_ASSERT(s == CharToWide(" aaa bbb "));
        {
            const Wtroka w(CharToWide(" a b c "));
            s = w;
            Collapse(s);
            UNIT_ASSERT(s == w);
            UNIT_ASSERT(s.c_str() == w.c_str()); // Collapse() does not change the string at all
        }
        s = CharToWide("  123    456  ");
        Collapse(s);
        UNIT_ASSERT(s == CharToWide(" 123 456 "));

        s = CharToWide("  1\n\n\n23\t    4\f\f56  ");
        Collapse(s);
        UNIT_ASSERT(s == CharToWide(" 1 23 4 56 "));

        s = CharToWide(" 1\n\n\n\f\f56  ");
        Collapse(s);
        UNIT_ASSERT(s == CharToWide(" 1 56 "));

        s = CharToWide("  1\r\n,\n(\n23\t    4\f\f56  ");
        Collapse(s);
        UNIT_ASSERT(s == CharToWide(" 1 , ( 23 4 56 "));

        s = CharToWide("1 23  ");
        Collapse(s);
        UNIT_ASSERT(s == CharToWide("1 23 "));
        {
            const Wtroka w = CharToWide(" ");
            s = w;
            Collapse(s);
            UNIT_ASSERT(s == w);
            UNIT_ASSERT(s.c_str() == w.c_str()); // Collapse() does not change the string at all
        }
        s = CharToWide("   ");
        Collapse(s);
        UNIT_ASSERT(s == CharToWide(" "));

        s = CharToWide(",\r\n\"");
        Collapse(s);
        UNIT_ASSERT(s == CharToWide(", \""));

        s = CharToWide("-");
        Collapse(s);
        UNIT_ASSERT(s == CharToWide("-"));

        s.clear();
        Collapse(s);
        UNIT_ASSERT(s == Wtroka());
    }

    void TestCollapseBuffer() {
        Wtroka s;
        s.append(ws, ARRAY_SIZE(ws)).append(3, 'a').append(ws, ARRAY_SIZE(ws)).append(3, 'b').append(ws, ARRAY_SIZE(ws));
        size_t n = Collapse(s.begin(), s.size());
        s.resize(n);
        UNIT_ASSERT(s == CharToWide(" aaa bbb "));

        s = CharToWide(" a b c ");
        n = Collapse(s.begin(), s.size());
        UNIT_ASSERT(n == s.size()); // length was not changed
        UNIT_ASSERT(s == CharToWide(" a b c "));

        s = CharToWide("  123    456  ");
        n = Collapse(s.begin(), s.size());
        s.resize(n);
        UNIT_ASSERT(s == CharToWide(" 123 456 "));

        s = CharToWide("  1\n\n\n23\t    4\f\f56  ");
        n = Collapse(s.begin(), s.size());
        s.resize(n);
        UNIT_ASSERT(s == CharToWide(" 1 23 4 56 "));

        s = CharToWide(" 1\n\n\n\f\f56  ");
        n = Collapse(s.begin(), s.size());
        s.resize(n);
        UNIT_ASSERT(s == CharToWide(" 1 56 "));

        s = CharToWide("  1\r\n,\n(\n23\t    4\f\f56  ");
        n = Collapse(s.begin(), s.size());
        s.resize(n);
        UNIT_ASSERT(s == CharToWide(" 1 , ( 23 4 56 "));

        s = CharToWide("1 23  ");
        n = Collapse(s.begin(), s.size());
        s.resize(n);
        UNIT_ASSERT(s == CharToWide("1 23 "));

        s = CharToWide(" ");
        n = Collapse(s.begin(), s.size());
        UNIT_ASSERT(n == 1);
        UNIT_ASSERT(s == CharToWide(" "));

        s = CharToWide("   ");
        n = Collapse(s.begin(), s.size());
        s.resize(n);
        UNIT_ASSERT(s == CharToWide(" "));

        s = CharToWide(",\r\n\"");
        n = Collapse(s.begin(), s.size());
        s.resize(n);
        UNIT_ASSERT(s == CharToWide(", \""));

        s = CharToWide("-");
        n = Collapse(s.begin(), s.size());
        UNIT_ASSERT(n == 1);
        UNIT_ASSERT(s == CharToWide("-"));

        s = CharToWide("\t");
        n = Collapse(s.begin(), s.size());
        UNIT_ASSERT(n == 1);
        UNIT_ASSERT(s == CharToWide(" "));

        s.clear();
        n = Collapse(s.begin(), s.size());
        UNIT_ASSERT(n == 0);
        UNIT_ASSERT(s == Wtroka());
    }

    void TestStrip() {
        Wtroka s;

        Strip(s);
        UNIT_ASSERT(s == Wtroka());

        s = CharToWide(" \t\r\n");
        Strip(s);
        UNIT_ASSERT(s == Wtroka());

        s = CharToWide("\t\f\va \r\n");
        Strip(s);
        UNIT_ASSERT(s == CharToWide("a"));

        s = CharToWide("\r\na\r\nb\t\tc\r\n");
        Strip(s);
        UNIT_ASSERT(s == CharToWide("a\r\nb\t\tc"));

        const Wtroka w(CharToWide("a  b"));
        s = w;
        Strip(s);
        UNIT_ASSERT(s == w);
        UNIT_ASSERT(s.c_str() == w.c_str()); // Strip() does not change the string at all
    }

    void TestIsSpace() {
        UNIT_ASSERT(!IsSpace(Wtroka()));

        UNIT_ASSERT(IsSpace(ws, ARRAY_SIZE(ws)));

        Wtroka w;
        w.assign(ws, ARRAY_SIZE(ws)).append(Wtroka(1, '!'));
        UNIT_ASSERT(!IsSpace(w.c_str(), w.size()));

        w.assign(Wtroka(1, '_')).append(ws, ARRAY_SIZE(ws));
        UNIT_ASSERT(!IsSpace(w.c_str(), w.size()));

        w.assign(ws, ARRAY_SIZE(ws)).append(Wtroka(1, '$')).append(ws, ARRAY_SIZE(ws));
        UNIT_ASSERT(!IsSpace(w.c_str(), w.size()));
    }

    void TestEscapeHtmlChars() {
        // characters from the first half of the ASCII table
        for (wchar16 c = 1; c < 0x7F; ++c) {
            Wtroka w(1, c);
            EscapeHtmlChars<false>(w);

            switch (c) {
            case '<':
                UNIT_ASSERT(w == CharToWide("&lt;"));
                break;
            case '>':
                UNIT_ASSERT(w == CharToWide("&gt;"));
                break;
            case '&':
                UNIT_ASSERT(w == CharToWide("&amp;"));
                break;
            case '"':
                UNIT_ASSERT(w == CharToWide("&quot;"));
                break;
            default:
                UNIT_ASSERT(w == Wtroka(1, c));
                break;
            }
        }

        for (wchar16 c = 1; c < 0x7F; ++c) {
            Wtroka w(1, c);
            EscapeHtmlChars<true>(w);

            switch (c) {
            case '<':
                UNIT_ASSERT(w == CharToWide("&lt;"));
                break;
            case '>':
                UNIT_ASSERT(w == CharToWide("&gt;"));
                break;
            case '&':
                UNIT_ASSERT(w == CharToWide("&amp;"));
                break;
            case '"':
                UNIT_ASSERT(w == CharToWide("&quot;"));
                break;
            case '\r':
            case '\n':
                UNIT_ASSERT(w == CharToWide("<BR>"));
                break;
            default:
                UNIT_ASSERT(w == Wtroka(1, c));
                break;
            }
        }
    }

    void TestToLower() {
        const size_t n = 32;
        wchar16 upperCase[n];
        NStl::copy(wideCyrillicAlphabet, wideCyrillicAlphabet + n, upperCase);
        ToLower(upperCase, n);
        UNIT_ASSERT(TCharTraits<wchar16>::Compare(upperCase, wideCyrillicAlphabet + n, n) == 0);
    }

    void TestToUpper() {
        const size_t n = 32;
        wchar16 lowerCase[n];
        NStl::copy(wideCyrillicAlphabet + n, wideCyrillicAlphabet + n * 2, lowerCase);
        ToUpper(lowerCase, n);
        UNIT_ASSERT(TCharTraits<wchar16>::Compare(lowerCase, wideCyrillicAlphabet, n) == 0);
    }

    void TestWideString() {
        const Wtroka original = WideToWide(WideStringTestData[0], CaseTestDataSize);
        const Wtroka lower = WideToWide(WideStringTestData[1], CaseTestDataSize);
        const Wtroka upper = WideToWide(WideStringTestData[2], CaseTestDataSize);
        const Wtroka title = WideToWide(WideStringTestData[3], CaseTestDataSize);
        Wtroka temp;

        temp = original;
        temp.to_lower();
        UNIT_ASSERT(temp == lower);

        temp = original;
        ToLower(temp.begin(), temp.size());
        UNIT_ASSERT(temp == lower);

        temp = original;
        temp.to_upper();
        UNIT_ASSERT(temp == upper);

        temp = original;
        ToUpper(temp.begin(), temp.size());
        UNIT_ASSERT(temp == upper);

        temp = original;
        temp.to_title();
        UNIT_ASSERT(temp == title);

        temp = original;
        ToTitle(temp.begin(), temp.size());
        UNIT_ASSERT(temp == title);

        yvector<wchar32> buffer(WideStringTestData[0], WideStringTestData[0] + CaseTestDataSize);
        NStl::reverse(buffer.begin(), buffer.end());
        const Wtroka reversed = WideToWide(buffer.begin(), buffer.size());

        temp.resize(original.size());
        Reverse(original.begin(), original.size(), temp.begin());
        UNIT_ASSERT(temp == reversed);

        temp = original;
        temp.reverse();
        UNIT_ASSERT(temp == reversed);
    }
};

UNIT_TEST_SUITE_REGISTRATION(TWideUtilTest);
