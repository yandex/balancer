#include "subst.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(TStringSubst) {
    SIMPLE_UNIT_TEST(TestSubstGlobal) {
        Stroka s;
        s = "aaa";
        SubstGlobal(s, "a", "bb");
        UNIT_ASSERT_EQUAL(s, Stroka("bbbbbb"));
        s = "aaa";
        SubstGlobal(s, "a", "b");
        UNIT_ASSERT_EQUAL(s, Stroka("bbb"));
        s = "aaa";
        SubstGlobal(s, "a", "");
        UNIT_ASSERT_EQUAL(s, Stroka(""));
    }
}
