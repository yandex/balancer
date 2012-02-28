#pragma once

#include "defs.h"

namespace NModBalancer {
    IBackEnds* CreateRobust(const TModuleParams& mp);
}
