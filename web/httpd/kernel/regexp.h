#pragma once

#include <library/pire/regexp.h>

namespace NSrvKernel {
    using namespace NRegExp;
    class TChunkList;

    TMatcher& Match(TMatcher& m, const TChunkList& lst) throw ();
}
