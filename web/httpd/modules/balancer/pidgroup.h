#pragma once

#include "defs.h"

namespace NModBalancer {
    IBackEnds* CreatePIDGroup(const TModuleParams& mp);
}
