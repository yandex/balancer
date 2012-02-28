#include "null.h"

#include <util/generic/singleton.h>

using namespace NSrvKernel;

TNullStream& TNullStream::Instance() throw () {
    return *Singleton<TNullStream>();
}
