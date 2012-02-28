#include "chunks.h"

#include <util/stream/output.h>

using namespace NSrvKernel;

template <>
void Out<TChunk>(TOutputStream& o, const TChunk& p) {
    o.Write(p.Data(), p.Length());
}

template <>
void Out<TChunkList>(TOutputStream& o, const TChunkList& lst) {
    for (TChunkList::TConstIterator it = lst.Begin(); it != lst.End(); ++it) {
        o << *it;
    }
}
