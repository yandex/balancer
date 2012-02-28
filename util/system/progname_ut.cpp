#include "progname.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(TProgramNameTest) {
    SIMPLE_UNIT_TEST(TestIt) {
        Stroka progName = GetProgramName();

        try {
            UNIT_ASSERT(
                progName.find("ut_util") != Stroka::npos
                || progName.find("util-system_ut") != Stroka::npos);
        } catch (...) {
            Cerr << progName << Endl;

            throw;
        }
    }
}
