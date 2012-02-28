#include "iface.h"

#include <util/stream/output.h>

using namespace NSrvKernel;

void IModule::PrintStats(TOutputStream& out) {
    const char* name = Handle()->Name();

    out << "<" << name << ">";
    DoStats(out);
    out << "</" << name << ">";
}

void IModule::DoStats(TOutputStream&) {
}
