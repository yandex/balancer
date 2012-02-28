#include "flock.h"
#include "tempfile.h"

#include <library/unittest/registar.h>

class TFileLockTest: public TTestBase {
        UNIT_TEST_SUITE(TFileLockTest);
            UNIT_TEST(TestFlock)
        UNIT_TEST_SUITE_END();
    private:
        inline void TestFlock() {
            TTempFileHandle tmp("./file");

            UNIT_ASSERT_EQUAL(Flock(tmp.GetHandle(), LOCK_EX), 0);
            UNIT_ASSERT_EQUAL(Flock(tmp.GetHandle(), LOCK_UN), 0);
        }
};

UNIT_TEST_SUITE_REGISTRATION(TFileLockTest);
