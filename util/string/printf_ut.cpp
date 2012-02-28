#include "printf.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(TStringPrintf) {
    SIMPLE_UNIT_TEST(TestSprintf) {
        Stroka s;
        int len = sprintf(s, "Hello %s", "world");
        UNIT_ASSERT_EQUAL(s, Stroka("Hello world"));
        UNIT_ASSERT_EQUAL(len, 11);
    }

    SIMPLE_UNIT_TEST(TestFcat) {
        Stroka s;
        int len = sprintf(s, "Hello %s", "world");
        UNIT_ASSERT_EQUAL(s, Stroka("Hello world"));
        UNIT_ASSERT_EQUAL(len, 11);
        len = fcat(s, " qwqw%s", "as");
        UNIT_ASSERT_EQUAL(s, Stroka("Hello world qwqwas"));
        UNIT_ASSERT_EQUAL(len, 7);
    }
}
