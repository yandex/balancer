#include "base64.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(TBase64) {
    SIMPLE_UNIT_TEST(TestEncode) {
        UNIT_ASSERT_VALUES_EQUAL(Base64Encode("12z"), "MTJ6");
        UNIT_ASSERT_VALUES_EQUAL(Base64Encode("123"), "MTIz");
        UNIT_ASSERT_VALUES_EQUAL(Base64Encode("12"),  "MTI=");
        UNIT_ASSERT_VALUES_EQUAL(Base64Encode("1"),   "MQ==");
    }

    SIMPLE_UNIT_TEST(TestDecode) {
        UNIT_ASSERT_VALUES_EQUAL(Base64Decode("MTI="), "12");
    }
}
