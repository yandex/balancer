#include "cast.h"

#include <library/unittest/registar.h>

class TGenericCastsTest: public TTestBase {
        UNIT_TEST_SUITE(TGenericCastsTest);
            UNIT_TEST(TestReinterpret)
        UNIT_TEST_SUITE_END();
    private:
        inline void TestReinterpret() {
            static char tmp[2];

            UNIT_ASSERT_EQUAL(reinterpret_cast<short*>(tmp), ReinterpretCast<short*>(tmp));
        }
};

UNIT_TEST_SUITE_REGISTRATION(TGenericCastsTest);
