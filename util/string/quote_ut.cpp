#include "quote.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(TCGIEscapeTest) {
    SIMPLE_UNIT_TEST(ReturnsEndOfTo) {
        char r[10];
        const char* returned = CGIEscape(r, "123");
        UNIT_ASSERT_VALUES_EQUAL(r + strlen("123"), returned);
        UNIT_ASSERT_VALUES_EQUAL('\0', *returned);
    }

    SIMPLE_UNIT_TEST(NotZeroTerminated) {
        char r[] = {'1', '2', '3', '4'};
        char buf[sizeof(r) * 3 + 2];

        Stroka ret(buf, CGIEscape(buf, r, sizeof(r)));

        UNIT_ASSERT_EQUAL(ret, "1234");
    }

    SIMPLE_UNIT_TEST(StringBuf) {
        char tmp[100];

        UNIT_ASSERT_VALUES_EQUAL(CgiEscape(tmp, "!@#$%^&*()"), STRINGBUF("!@%23$%25^%26*%28%29"));
    }
}

SIMPLE_UNIT_TEST_SUITE(TCGIUnescapeTest) {
    SIMPLE_UNIT_TEST(StringBuf) {
        char tmp[100];

        UNIT_ASSERT_VALUES_EQUAL(CgiUnescape(tmp, "!@%23$%25^%26*%28%29"), STRINGBUF("!@#$%^&*()"));
    }

    SIMPLE_UNIT_TEST(TestValidZeroTerm) {
        char r[10];

        CGIUnescape(r, "1234");
        UNIT_ASSERT_VALUES_EQUAL(r, "1234");

        CGIUnescape(r, "%3d");
        UNIT_ASSERT_VALUES_EQUAL(r, "=");

        CGIUnescape(r, "12%3D34");
        UNIT_ASSERT_VALUES_EQUAL(r, "12=34");

        CGIUnescape(r, "12%u003D34", CODES_WIN);
        UNIT_ASSERT_VALUES_EQUAL(r, "12=34");
    }

    SIMPLE_UNIT_TEST(TestInvalidZeroTerm) {
        char r[10];

        CGIUnescape(r, "%");
        UNIT_ASSERT_VALUES_EQUAL(r, "%");

        CGIUnescape(r, "%3");
        UNIT_ASSERT_VALUES_EQUAL(r, "%3");

        CGIUnescape(r, "%3g");
        UNIT_ASSERT_VALUES_EQUAL(r, "%3g");

        CGIUnescape(r, "12%3g34");
        UNIT_ASSERT_VALUES_EQUAL(r, "12%3g34");

        CGIUnescape(r, "%3u123", CODES_WIN);
        UNIT_ASSERT_VALUES_EQUAL(r, "%3u123");
    }

    SIMPLE_UNIT_TEST(TestValidNotZeroTerm) {
        char r[10];

        CGIUnescape(r, "123456789", 4);
        UNIT_ASSERT_VALUES_EQUAL(r, "1234");

        CGIUnescape(r, "%3d1234", 3);
        UNIT_ASSERT_VALUES_EQUAL(r, "=");

        CGIUnescape(r, "12%3D345678", 7);
        UNIT_ASSERT_VALUES_EQUAL(r, "12=34");

        CGIUnescape(r, "12%u003D345678", 10, CODES_WIN);
        UNIT_ASSERT_VALUES_EQUAL(r, "12=34");
    }

    SIMPLE_UNIT_TEST(TestInvalidNotZeroTerm) {
        char r[10];

        CGIUnescape(r, "%3d", 1);
        UNIT_ASSERT_VALUES_EQUAL(r, "%");

        CGIUnescape(r, "%3d", 2);
        UNIT_ASSERT_VALUES_EQUAL(r, "%3");

        CGIUnescape(r, "%3g1234", 3);
        UNIT_ASSERT_VALUES_EQUAL(r, "%3g");

        CGIUnescape(r, "12%3g345678", 7);
        UNIT_ASSERT_VALUES_EQUAL(r, "12%3g34");

        CGIUnescape(r, "%3u1234", 2, CODES_WIN);
        UNIT_ASSERT_VALUES_EQUAL(r, "%3");

        CGIUnescape(r, "%3u1234", 3, CODES_WIN);
        UNIT_ASSERT_VALUES_EQUAL(r, "%3u");

        CGIUnescape(r, "%3u1234", 4, CODES_WIN);
        UNIT_ASSERT_VALUES_EQUAL(r, "%3u1");
    }

    SIMPLE_UNIT_TEST(TestUnescapeAuto) {
        char r[30];

        char* p = CGIUnescapeAuto(r, "%D1%EB%EE%ED", CODES_WIN);
        UNIT_ASSERT_VALUES_EQUAL(r, "\xD0\xA1\xD0\xBB\xD0\xBE\xD0\xBD");
        UNIT_ASSERT_VALUES_EQUAL(p - r, 8);

        CGIUnescapeAuto(r, "%84R%84%7C%84%80%84%7E", CODES_SHIFT_JIS);
        UNIT_ASSERT_VALUES_EQUAL(r, "\xD0\xA1\xD0\xBB\xD0\xBE\xD0\xBD");

        CGIUnescapeAuto(r, "%F3+%26%2338%3B+%ED", CODES_KOI8);
        UNIT_ASSERT_VALUES_EQUAL(r, "\xD0\xA1 & \xD0\x9C");

        CGIUnescapeAuto(r, "%F3+%26%2338%3B+%ED", CODES_WIN);
        UNIT_ASSERT_VALUES_EQUAL(r, "\xD1\x83 & \xD0\xBD");

        CGIUnescapeAuto(r, "%F3+%26%2338%3B+\xED", CODES_KOI8);
        UNIT_ASSERT_VALUES_EQUAL(r, "\xD0\xA1 & \xD0\x9C");

        CGIUnescapeAuto(r, "%F3+%26%2338%3B+\xED", CODES_WIN);
        UNIT_ASSERT_VALUES_EQUAL(r, "\xD1\x83 & \xD0\xBD");

        CGIUnescapeAuto(r, "%D1%83+%26%2338%3B+\xD0\xBD", CODES_WIN);
        UNIT_ASSERT_VALUES_EQUAL(r, "\xD1\x83 &#38; \xD0\xBD");
    }
}

SIMPLE_UNIT_TEST_SUITE(TQuoteTest) {
    SIMPLE_UNIT_TEST(ReturnsEndOfTo) {
        char r[10];
        const char* returned = Quote(r, "123");
        UNIT_ASSERT_VALUES_EQUAL(r + strlen("123"), returned);
        UNIT_ASSERT_VALUES_EQUAL('\0', *returned);
    }

    SIMPLE_UNIT_TEST(SlashIsSafeByDefault) {
        char r[100];
        Quote(r, "/path;tail/path,tail/");
        UNIT_ASSERT_VALUES_EQUAL("/path%3Btail/path%2Ctail/", r);
        Stroka s("/path;tail/path,tail/");
        Quote(s);
        UNIT_ASSERT_VALUES_EQUAL("/path%3Btail/path%2Ctail/", s.c_str());
    }

    SIMPLE_UNIT_TEST(SafeColons) {
        char r[100];
        Quote(r, "/path;tail/path,tail/", ";,");
        UNIT_ASSERT_VALUES_EQUAL("%2Fpath;tail%2Fpath,tail%2F", r);
        Stroka s("/path;tail/path,tail/");
        Quote(s, ";,");
        UNIT_ASSERT_VALUES_EQUAL("%2Fpath;tail%2Fpath,tail%2F", s.c_str());
    }
}
