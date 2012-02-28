#include "fnv.h"

#include <library/unittest/registar.h>

#include <util/stream/ios.h>

class TFnvTest: public TTestBase {
        UNIT_TEST_SUITE(TFnvTest);
            UNIT_TEST(TestFnv32)
            UNIT_TEST(TestFnv64)
        UNIT_TEST_SUITE_END();
    private:
        inline void TestFnv32() {
            UNIT_ASSERT_EQUAL(FnvHash<ui32>("1234567", 7), ULL(2849763999));
        }

        inline void TestFnv64() {
            UNIT_ASSERT_EQUAL(FnvHash<ui64>("1234567", 7), ULL(2449551094593701855));
        }
};

UNIT_TEST_SUITE_REGISTRATION(TFnvTest);
