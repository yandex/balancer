#include "format.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(TOutputStreamFormattingTest) {
    SIMPLE_UNIT_TEST(TLeftPad) {
        TStringStream ss;
        ss << LeftPad(10, 4, '0');
        UNIT_ASSERT_VALUES_EQUAL("0010", ss.Str());

        ss.clear();
        ss << LeftPad(222, 1);
        UNIT_ASSERT_VALUES_EQUAL("222", ss.Str());
    }

    SIMPLE_UNIT_TEST(TRightPad) {
        TStringStream ss;
        ss << RightPad("aa", 4);
        UNIT_ASSERT_VALUES_EQUAL("aa  ", ss.Str());

        ss.clear();
        ss << RightPad("aa", 1);
        UNIT_ASSERT_VALUES_EQUAL("aa", ss.Str());
    }

    SIMPLE_UNIT_TEST(TestTime) {
        TStringStream ss;

        ss << "[" << Time << "] " << "qwqw" << TimeHumanReadable << Endl;
    }
}
