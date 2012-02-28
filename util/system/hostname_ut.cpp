#include "hostname.h"

#include <library/unittest/registar.h>

class THostNameTest: public TTestBase {
        UNIT_TEST_SUITE(THostNameTest);
            UNIT_TEST(Test1)
        UNIT_TEST_SUITE_END();
    private:
        inline void Test1() {
            UNIT_ASSERT(*GetHostName() != '?');
        }
};

UNIT_TEST_SUITE_REGISTRATION(THostNameTest);
