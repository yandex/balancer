#include "bitops.h"

#include <library/unittest/registar.h>

class TBitOpsTest: public TTestBase {
    UNIT_TEST_SUITE(TBitOpsTest);
        UNIT_TEST(TestCountTrailingZeroBits)
    UNIT_TEST_SUITE_END();
private:
    inline void TestCountTrailingZeroBits() {
        TestCTZ<unsigned int>();
        TestCTZ<unsigned long>();
        TestCTZ<unsigned long long>();
    }

    template <typename T>
    inline void TestCTZ() {
        for (unsigned int i = 0; i < (sizeof(T) << 3); ++i) {
            UNIT_ASSERT_EQUAL(CountTrailingZeroBits(T(1) << i), i);
        }
    }
};

UNIT_TEST_SUITE_REGISTRATION(TBitOpsTest);
