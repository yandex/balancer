#include <util/random/randcpp.h>

#include "registar.h"

Stroka NUnitTest::RandomString(size_t len, ui32 seed) {
    TRand rand;
    Stroka ret;

    rand.srandom(seed);
    ret.reserve(len);

    for (size_t i = 0; i < len; ++i) {
        ret.push_back(char(1 + (rand.random() % 126)));
    }

    return ret;
}
