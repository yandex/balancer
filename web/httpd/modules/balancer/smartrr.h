#pragma once

#include "defs.h"

namespace NModBalancer {
    IBackEnds* CreateSmartRoundRobin(const TModuleParams& mp);
}
