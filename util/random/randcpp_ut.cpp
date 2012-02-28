#include "randcpp.h"

#include <library/unittest/registar.h>

class TRandTest: public TTestBase {
        UNIT_TEST_SUITE(TRandTest);
            UNIT_TEST(TestRandom)
        UNIT_TEST_SUITE_END();
    private:
        inline void TestRandom() {
            TRand r;
            r.srandom(17);

            // PG said random() must be completely deterministic

            long expected[] = { 65, 57, 39, 47, 79, 59, 45, 11, 53, 27 };
            for (unsigned int i = 0; i < ARRAY_SIZE(expected); ++i) {
                // printf("%ld\n", r.random() % 100);
                UNIT_ASSERT_EQUAL(expected[i], r.random() % 100);
            }
        }
};

UNIT_TEST_SUITE_REGISTRATION(TRandTest);
