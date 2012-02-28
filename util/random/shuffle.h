#pragma once

#include "random.h"

#include <util/generic/utility.h>

template <typename TRandIter, typename TRandIterEnd>
inline void Shuffle(TRandIter begin, TRandIterEnd end) {
    const size_t sz = end - begin;

    for (size_t i = 1; i < sz; ++i) {
        DoSwap(*(begin + i), *(begin + RandomNumber(i + 1)));
    }
}
