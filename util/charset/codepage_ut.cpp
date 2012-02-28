#include "wide.h"
#include "recyr.hh"
#include "codepage.h"

#include <library/unittest/registar.h>

#include <util/system/yassert.h>

#if defined (_MSC_VER)
#   pragma warning(disable:4309) /*truncation of constant value*/
#endif

namespace {

    const char yandexUpperCase[] = {
              0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,                                     0x8E,
              0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,       0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0x00
        };
    const char yandexLowerCase[] = {
              0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,                                     0x9E,
              0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6,       0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 0x00
        };

}

class TCodepageTest : public TTestBase {
private:
    UNIT_TEST_SUITE(TCodepageTest);
        UNIT_TEST(TestUTF);
        UNIT_TEST(TestUTFFromUnknownPlane);
        UNIT_TEST(TestBrokenMultibyte);
        UNIT_TEST(TestSurrogatePairs);
        UNIT_TEST(TestEncodingHints);
        UNIT_TEST(TestToLower);
        UNIT_TEST(TestToUpper);
        UNIT_TEST(TestUpperLower);
        UNIT_TEST(TestBrokenRune);
    UNIT_TEST_SUITE_END();

public:
    void TestUTF();
    void TestUTFFromUnknownPlane();
    void TestBrokenMultibyte();
    void TestSurrogatePairs();
    void TestEncodingHints();
    void TestToLower();
    void TestToUpper();

    inline void TestUpperLower() {
        const CodePage* cp = CodePageByCharset(CODES_ASCII);
        char tmp[100];

        TStringBuf s = STRINGBUF("abcde");

        TStringBuf upper(tmp, cp->ToUpper(s.begin(), s.end(), tmp));
        UNIT_ASSERT_VALUES_EQUAL(upper, STRINGBUF("ABCDE"));

        TStringBuf lower(tmp, cp->ToLower(upper.begin(), upper.end(), tmp));
        UNIT_ASSERT_VALUES_EQUAL(lower, STRINGBUF("abcde"));
    }

    void TestBrokenRune() {
        UNIT_ASSERT_VALUES_EQUAL(BROKEN_RUNE, 0xFFFDu);
    }
};

UNIT_TEST_SUITE_REGISTRATION(TCodepageTest);

void TCodepageTest::TestUTF() {
    for (wchar32 i = 0; i < 0x200000; i++) {
        unsigned char buffer[32];
        Zero(buffer);
        size_t rune_len;
        size_t ref_len = 0;

        if (i < 0x80)
            ref_len = 1;
        else if (i < 0x800)
            ref_len = 2;
        else if (i < 0x10000)
            ref_len = 3;
        else
            ref_len = 4;

        RECODE_RESULT res = utf8_put_rune(i, rune_len, buffer, buffer + 32);
        UNIT_ASSERT(res == RECODE_OK);
        UNIT_ASSERT(rune_len == ref_len);

        res = utf8_put_rune(i, rune_len, buffer, buffer + ref_len - 1);
        UNIT_ASSERT(res == RECODE_EOOUTPUT);

        wchar32 rune;
        res = utf8_read_rune(rune, rune_len, buffer, buffer + 32);
        UNIT_ASSERT(res == RECODE_OK);
        UNIT_ASSERT(rune == i);
        UNIT_ASSERT(rune_len == ref_len);

        res = utf8_read_rune(rune, rune_len, buffer, buffer + ref_len - 1);
        UNIT_ASSERT(res == RECODE_EOINPUT);

        if (ref_len > 1) {
            res = utf8_read_rune(rune, rune_len, buffer + 1, buffer + ref_len);
            UNIT_ASSERT(res == RECODE_BROKENSYMBOL);

            buffer[1] |= 0xC0;
            res = utf8_read_rune(rune, rune_len, buffer, buffer + ref_len);
            UNIT_ASSERT(res == RECODE_BROKENSYMBOL);

            buffer[1] &= 0x3F;
            res = utf8_read_rune(rune, rune_len, buffer, buffer + ref_len);
            UNIT_ASSERT(res == RECODE_BROKENSYMBOL);
        }
    }
}

void TCodepageTest::TestBrokenMultibyte() {
    const ECharset cp = CODES_EUC_JP;

    const char sampletext[] = { 227 };
    wchar32 recodeResult[100];

    size_t nwritten = 0;
    size_t nread = 0;

    RECODE_RESULT res = RecodeToUnicode(cp, sampletext, recodeResult, ARRAY_SIZE(sampletext), ARRAY_SIZE(recodeResult), nread, nwritten);
    UNIT_ASSERT(res == RECODE_OK);
    UNIT_ASSERT(nread == 1);
    UNIT_ASSERT(nwritten == 0);

    const char bigSample[] = { 0xC3, 0x87, 0xC3, 0x8E, 0xC2, 0xB0, 0xC3, 0x85, 0xC3, 0x85, 0xC3, 0xB8 };
    res = RecodeToUnicode(cp, sampletext, recodeResult, ARRAY_SIZE(bigSample), ARRAY_SIZE(recodeResult), nread, nwritten);
    UNIT_ASSERT(res == RECODE_OK);
    UNIT_ASSERT(nread == ARRAY_SIZE(bigSample));
}

void TCodepageTest::TestUTFFromUnknownPlane() {
    static const wchar32 sampletext[] = { 0x61, 0x62, 0x63, 0x20,
                                          0x430, 0x431, 0x432, 0x20,
                                          0x1001, 0x1002, 0x1003, 0x20,
                                          0x10001, 0x10002, 0x10003 };

    static const size_t BUFFER_SIZE = 1024;
    char bytebuffer[BUFFER_SIZE];

    size_t readchars = 0;
    size_t writtenbytes = 0;
    size_t samplelen = ARRAY_SIZE(sampletext);

    RECODE_RESULT res = RecodeFromUnicode(CODES_UTF8, sampletext, bytebuffer, samplelen,  BUFFER_SIZE, readchars, writtenbytes);

    UNIT_ASSERT (res == RECODE_OK);
    UNIT_ASSERT (samplelen == readchars);

    wchar32 charbuffer[BUFFER_SIZE];
    size_t readbytes = 0;
    size_t writtenchars = 0;

    res = RecodeToUnicode(CODES_UNKNOWNPLANE, bytebuffer, charbuffer, writtenbytes,  BUFFER_SIZE, readbytes, writtenchars);

    UNIT_ASSERT (res == RECODE_OK);
    UNIT_ASSERT (readbytes == writtenbytes);

    wchar32* charbufferend = charbuffer + writtenchars;
    DecodeUnknownPlane(charbuffer, charbufferend,  CODES_UTF8);

    UNIT_ASSERT (charbufferend == charbuffer + samplelen);
    for (size_t i = 0; i < samplelen; ++i)
       UNIT_ASSERT (sampletext[i] == charbuffer[i]);

    // Now, concatenate the thing with an explicit character and retest
    res = RecodeToUnicode(CODES_UNKNOWNPLANE, bytebuffer, charbuffer, writtenbytes,  BUFFER_SIZE, readbytes, writtenchars);
    UNIT_ASSERT (res == RECODE_OK);
    UNIT_ASSERT (readbytes == writtenbytes);

    charbuffer[writtenchars] = 0x1234;

    size_t morewrittenchars = 0;
    res = RecodeToUnicode(CODES_UNKNOWNPLANE, bytebuffer, charbuffer + writtenchars + 1, writtenbytes,  BUFFER_SIZE, readbytes, morewrittenchars);
    UNIT_ASSERT (res == RECODE_OK);
    UNIT_ASSERT (readbytes == writtenbytes);
    UNIT_ASSERT (writtenchars == morewrittenchars);

    charbuffer[2 * writtenchars + 1] = 0x5678;

    charbufferend = charbuffer + 2 * writtenchars + 2;
    DecodeUnknownPlane(charbuffer, charbufferend,  CODES_UTF8);

    UNIT_ASSERT (charbufferend == charbuffer + 2 * samplelen + 2);
    for (size_t i = 0; i < samplelen; ++i) {
       UNIT_ASSERT (sampletext[i] == charbuffer[i]);
       UNIT_ASSERT (sampletext[i] == charbuffer[samplelen + 1 + i]);
    }
    UNIT_ASSERT (0x1234 == charbuffer[samplelen]);
    UNIT_ASSERT (0x5678 == charbuffer[2* samplelen + 1]);

    // test TChar version
    // bytebuffer of len writtenbytes contains sampletext of len samplelen chars in utf8
    Wtroka wtr = CharToWide(TStringBuf(bytebuffer, writtenbytes), CODES_UNKNOWNPLANE);
    TChar* strend = wtr.begin() + +wtr;
    DecodeUnknownPlane(wtr.begin(), strend, CODES_UTF8);
    wtr.resize(strend - ~wtr, 'Q');
    UNIT_ASSERT_VALUES_EQUAL(+wtr, samplelen);
    for (size_t i = 0; i < +wtr; ++i) {
        if (sampletext[i] >= 0x10000) {
            UNIT_ASSERT_VALUES_EQUAL(wtr[i], ' ');
        } else {
            UNIT_ASSERT_VALUES_EQUAL(wtr[i], sampletext[i]);
        }
    }
}

static void TestSurrogates(const char* str, const wchar16* wide, size_t wideSize) {
    size_t sSize = strlen(str);
    size_t wSize = sSize * 2;
    TArrayHolder<wchar16> w(new wchar16[wSize]);

    size_t read = 0;
    size_t written = 0;
    RECODE_RESULT res = RecodeToUnicode(CODES_UTF8, str, w.Get(), sSize, wSize, read, written);
    UNIT_ASSERT(res == RECODE_OK);
    UNIT_ASSERT(read == sSize);
    UNIT_ASSERT(written == wideSize);
    UNIT_ASSERT(!memcmp(w.Get(), wide, wideSize));

    TArrayHolder<char> s(new char[sSize]);
    res = RecodeFromUnicode(CODES_UTF8, w.Get(), s.Get(), wideSize, sSize, read, written);
    UNIT_ASSERT(res == RECODE_OK);
    UNIT_ASSERT(read == wideSize);
    UNIT_ASSERT(written == sSize);
    UNIT_ASSERT(!memcmp(s.Get(), str, sSize));
}

void TCodepageTest::TestSurrogatePairs() {
    const char* utf8NonBMP = "\xf4\x80\x89\x84\xf4\x80\x89\x87\xf4\x80\x88\xba";
    wchar16 wNonBMPDummy[] = { 0xDBC0, 0xDE44, 0xDBC0, 0xDE47, 0xDBC0, 0xDE3A };
    TestSurrogates(utf8NonBMP, wNonBMPDummy, ARRAY_SIZE(wNonBMPDummy));

    const char* utf8NonBMP2 = "ab\xf4\x80\x89\x87n";
    wchar16 wNonBMPDummy2[] = { 'a', 'b', 0xDBC0, 0xDE47, 'n' };
    TestSurrogates(utf8NonBMP2, wNonBMPDummy2, ARRAY_SIZE(wNonBMPDummy2));
}

void TCodepageTest::TestEncodingHints() {
    UNIT_ASSERT(CODES_WIN == EncodingHintByName("windows-1251"));
    UNIT_ASSERT(CODES_WIN == EncodingHintByName("Windows1251"));
    UNIT_ASSERT(CODES_WIN == EncodingHintByName("WIN1251"));
    UNIT_ASSERT(CODES_WIN == EncodingHintByName("window-cp1251"));
    UNIT_ASSERT(CODES_WIN == EncodingHintByName("!!!CP1251???"));
    UNIT_ASSERT(CODES_WIN == EncodingHintByName("'ansi-cp1251;'"));
    UNIT_ASSERT(CODES_WIN == EncodingHintByName("charset=Microsoft-CP1251;"));

    UNIT_ASSERT(CODES_ISO_EAST == EncodingHintByName("iso-8859-2"));
    UNIT_ASSERT(CODES_ISO_EAST == EncodingHintByName("iso-2"));
    UNIT_ASSERT(CODES_ISO_EAST == EncodingHintByName("iso-latin-2"));
    UNIT_ASSERT(CODES_ISO_EAST == EncodingHintByName("charset=\"Latin2\";"));

    UNIT_ASSERT(CODES_UNKNOWN == EncodingHintByName("widow1251"));
    UNIT_ASSERT(CODES_UNKNOWN == EncodingHintByName("default"));
    UNIT_ASSERT(CODES_UNKNOWN == EncodingHintByName("$phpcharset"));

    UNIT_ASSERT(CODES_UNSUPPORTED != EncodingHintByName("ShiftJIS"));
    UNIT_ASSERT(CODES_UNSUPPORTED != EncodingHintByName("Shift_JIS"));
    UNIT_ASSERT(CODES_UNSUPPORTED != EncodingHintByName("Big5"));
    UNIT_ASSERT(CODES_UNSUPPORTED != EncodingHintByName("euc-kr"));
    UNIT_ASSERT(CODES_UNSUPPORTED != EncodingHintByName("EUC-JP"));
    UNIT_ASSERT(CODES_UNSUPPORTED != EncodingHintByName("charset='Shift_JIS';;"));
    UNIT_ASSERT(CODES_UNSUPPORTED != EncodingHintByName("ISO-2022-KR"));
    UNIT_ASSERT(CODES_UNSUPPORTED != EncodingHintByName("ISO-2022-jp"));
}

void TCodepageTest::TestToLower() {
    TTempBuf buf;
    char* data = buf.Data();
    const size_t n = ARRAY_SIZE(yandexUpperCase); // including NTS
    memcpy(data, yandexUpperCase, n);
    ToLower(data, n - 1);
    UNIT_ASSERT(strcmp(data, yandexLowerCase) == 0);
}

void TCodepageTest::TestToUpper() {
    TTempBuf buf;
    char* data = buf.Data();
    const size_t n = ARRAY_SIZE(yandexLowerCase); // including NTS
    memcpy(data, yandexLowerCase, n);
    ToUpper(data, n - 1);
    UNIT_ASSERT(strcmp(data, yandexUpperCase) == 0);
}
