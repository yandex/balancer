#undef NDEBUG
// yassert.h must be included before all headers
#include "yassert.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(YassertTest) {
    SIMPLE_UNIT_TEST(TestAcsLikeFunctionCall) {
        if (true)
            YASSERT(true); // this cannot be compiled if YASSERT is "if (!cond) { ... }"
        else
            YASSERT(false);

        bool var = false;
        if (false)
            YASSERT(false);
        else
            var = true; // this is unreachable if YASSERT is "if (!cond) { ... }"
        UNIT_ASSERT(var);
    }
}
