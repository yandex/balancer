#include "opt2.h"

#include <library/unittest/registar.h>

//using namespace NLastGetopt;

SIMPLE_UNIT_TEST_SUITE(Opt2Test) {
    SIMPLE_UNIT_TEST(TestSimple) {
        int argc = 6;
        char* argv[] = {
                (char*) "cmd", (char*) "--aaaa=aaaa", (char*) "zz", (char*) "-c", (char*) "-d8", (char*) "ww",
        };

        Opt2 opt(argc, argv, "A:b:cd:e:", 2, "aaaa=A");

        const char *edef = "edef";
        const char *a = opt.Arg('A', "<var_name> - usage of -A");
        int         b = opt.Int('b', "<var_name> - usage of -b", 2);
        bool        c = opt.Has('c', "usage of -c");
        int         d = opt.Int('d', "<var_name> - usage of -d", 13);
        const char *e = opt.Arg('e', "<unused> - only default is really used", edef);

        UNIT_ASSERT(!opt.AutoUsage("<L> <M>"));
        UNIT_ASSERT_VALUES_EQUAL("aaaa", a);
        UNIT_ASSERT_VALUES_EQUAL(2, b);
        UNIT_ASSERT(c);
        UNIT_ASSERT_VALUES_EQUAL(8, d);
        UNIT_ASSERT_VALUES_EQUAL(edef, e);

        UNIT_ASSERT_VALUES_EQUAL(2u, opt.Pos.size());
        UNIT_ASSERT_VALUES_EQUAL("zz", opt.Pos.at(0));
        UNIT_ASSERT_VALUES_EQUAL("ww", opt.Pos.at(1));
    }
}
