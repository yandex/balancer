#include "execpath.h"

#include <library/unittest/registar.h>

#include <util/system/platform.h>
#include <util/folder/dirut.h>

SIMPLE_UNIT_TEST_SUITE(TExecPathTest) {
    SIMPLE_UNIT_TEST(TestIt) {
        Stroka execPath = GetExecPath();

        try {
            UNIT_ASSERT(isexist(~execPath));
        } catch (...) {
            Cerr << execPath << Endl;

            throw;
        }
    }
}
