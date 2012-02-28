#pragma once

#include "chunks.h"

namespace NSrvKernel {
    static inline void SplitUnsafe(size_t len, TChunkList& from, TChunkList& before, TChunkList& after) throw () {
        while (!from.Empty()) {
            TChunkPtr cur(from.PopFront());

            if (cur->Length() <= len) {
                len -= cur->Length();
                before.Push(cur);
            } else {
                before.Push(cur->SubChunk(len));
                after.Push(cur->SubChunk(len, cur->Length()));
                MoveTo(&from, &after);
            }
        }
    }

    static inline void Split(size_t len, TChunkList& from, TChunkList& before, TChunkList& after) throw () {
        TChunkList tmp1, tmp2;

        SplitUnsafe(len, from, tmp1, tmp2);

        tmp1.Swap(before);
        tmp2.Swap(after);
    }

    static inline void Cut(size_t len, TChunkList& from, TChunkList& to) throw () {
        TChunkList tmp;

        Split(len, from, to, tmp);
    }

    static inline void Cut(size_t len, TChunkList& from) throw () {
        Cut(len, from, from);
    }
}
