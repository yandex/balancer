#pragma once

#include "defs.h"

namespace NModBalancer {
    IBackEnds* CreateRoundRobin(const TModuleParams& mp);
}
