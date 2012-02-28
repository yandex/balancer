#include "type.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(TStringClassify) {
    SIMPLE_UNIT_TEST(TestIsSpace) {
        UNIT_ASSERT_EQUAL(IsSpace(" "), true);
        UNIT_ASSERT_EQUAL(IsSpace("\t\r\n"), true);
        UNIT_ASSERT_EQUAL(IsSpace(""), false);
        UNIT_ASSERT_EQUAL(IsSpace("   a"), false);
    }

    SIMPLE_UNIT_TEST(TestIsTrue) {
        UNIT_ASSERT(istrue("1"));
        UNIT_ASSERT(istrue("yes"));
        UNIT_ASSERT(istrue("YeS"));
        UNIT_ASSERT(istrue("on"));
        UNIT_ASSERT(istrue("true"));
        UNIT_ASSERT(istrue("da"));

        UNIT_ASSERT(!istrue(""));// istrue won't return true on empty strings anymore

        UNIT_ASSERT(!istrue("foobar"));
    }
}
