#include "shuffle.h"

#include <library/unittest/registar.h>

#include <util/generic/ylimits.h>

SIMPLE_UNIT_TEST_SUITE(TRandUtilsTest) {
    SIMPLE_UNIT_TEST(TestShuffle) {
        Stroka s0, s1;
        Shuffle(s1.begin(), s1.end());
        s1 = "0";
        Shuffle(s1.begin(), s1.end());
        s1 = "01";
        Shuffle(s1.begin(), s1.end());
        s1 = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        s0 = s1.copy();
        Shuffle(s1.begin(), s1.end());
        UNIT_ASSERT(s0 != s1); // if shuffle does work, chances it will fail are 1 to 64!.
    }
}
