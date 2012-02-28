#include "strip.h"

#include <library/unittest/registar.h>

#include <util/charset/wide.h>

SIMPLE_UNIT_TEST_SUITE(TStripStringTest) {
    SIMPLE_UNIT_TEST(TestStrip) {
        static const struct TTest {
            const char* Str;
            const char* Result;
        }
        tests[] = {
            { "  012  ", "012" },
            { "  012",   "012" },
            { "012\t\t", "012" },
            { "012",     "012" },
            { "012\r\n", "012" },
        };

        for (int i = 0; i < (int)ARRAY_SIZE(tests); ++i) {
            Stroka s;
            strip(Stroka(tests[i].Str), s);
            UNIT_ASSERT_EQUAL(s == tests[i].Result, true);
        };
    }

    SIMPLE_UNIT_TEST(TestWtrokaStrip) {
        UNIT_ASSERT_EQUAL(StripString(CharToWide(" abc ")), CharToWide("abc"));
    }

    SIMPLE_UNIT_TEST(TestCollapse) {
        Stroka s;
        collapse(Stroka("  123    456  "), s);
        UNIT_ASSERT_EQUAL(s == " 123 456 ", true);
    }

    SIMPLE_UNIT_TEST(TestCollapseText) {
        Stroka abs1("Очень длинная строка описания на неизвестном языке.");
        Stroka abs2(abs1);
        Stroka abs3(abs1);
        collapse_text(abs1, 204);
        collapse_text(abs2, 51);
        collapse_text(abs3, 49);
        UNIT_ASSERT_EQUAL(abs1 == "Очень длинная строка описания на неизвестном языке.", true);
        UNIT_ASSERT_EQUAL(abs2 == "Очень длинная строка описания на неизвестном ...", true);
        UNIT_ASSERT_EQUAL(abs3 == "Очень длинная строка описания на ...", true);
    }
}
