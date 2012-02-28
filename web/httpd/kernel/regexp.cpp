#include "regexp.h"
#include "chunks.h"

namespace NSrvKernel {
    TMatcher& Match(TMatcher& m, const TChunkList& lst) throw () {
        for (TChunkList::TConstIterator it = lst.Begin(); it != lst.End(); ++it) {
            m.Match(it->Data(), it->Length());
        }

        return m;
    }
}
