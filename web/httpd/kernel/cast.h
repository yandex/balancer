#pragma once

#include "chunks.h"

namespace NSrvKernel {
    template <class T>
    static inline T FromString(const TChunkList& lst) {
        T ret = 0;

        for (TChunkList::TConstIterator it = lst.Begin(); it != lst.End(); ++it) {
            const char* b = it->Data();
            const char* e = b + it->Length();

            while (b != e) {
                ret = ret * T(10) + (T)(*b++ - '0');
            }
        }

        return ret;
    }
}
