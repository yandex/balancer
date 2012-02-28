#include "chartraits.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(TFastFindFirstOf) {
    SIMPLE_UNIT_TEST(Test0) {
        const char* s = "abcd";

        UNIT_ASSERT_EQUAL(FastFindFirstOf(s, 4, 0, 0) - s, 4);
    }

    SIMPLE_UNIT_TEST(Test1) {
        const char* s = "abcd";

        UNIT_ASSERT_EQUAL(FastFindFirstOf(s, 4, "b", 1) - s, 1);
    }

    SIMPLE_UNIT_TEST(Test1NotFound) {
        const char* s = "abcd";

        UNIT_ASSERT_EQUAL(FastFindFirstOf(s, 4, "x", 1) - s, 4);
    }

    SIMPLE_UNIT_TEST(Test2) {
        const char* s = "abcd";

        UNIT_ASSERT_EQUAL(FastFindFirstOf(s, 4, "xc", 2) - s, 2);
    }

    SIMPLE_UNIT_TEST(TestNot) {
        const char* s = "abcd";

        UNIT_ASSERT_EQUAL(FastFindFirstNotOf(s, 4, "ab", 2) - s, 2);
    }
}
