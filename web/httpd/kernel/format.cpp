#include "format.h"
#include "address.h"

#include <util/stream/output.h>

using namespace NSrvKernel;

template <>
void Out<THostFormat>(TOutputStream& o, const THostFormat& ip) {
    PrintHost(o, *ip.Addr);
}
