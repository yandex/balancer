#include "opt.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(OptTest) {
    SIMPLE_UNIT_TEST(TestSimple) {
        int argc = 3;
        char* argv[] = {
            (char*) "cmd", (char*) "-x"
        };
        Opt opt(argc, argv, "");
        opt.Err = false; // be quiet
        UNIT_ASSERT_VALUES_EQUAL('?', opt.Get());
        UNIT_ASSERT_VALUES_EQUAL(EOF, opt.Get());
        UNIT_ASSERT_VALUES_EQUAL(EOF, opt.Get());
        UNIT_ASSERT_VALUES_EQUAL(EOF, opt.Get());
    }
}
