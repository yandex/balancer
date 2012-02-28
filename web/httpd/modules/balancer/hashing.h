#pragma once

#include "defs.h"

namespace NModBalancer {
    IBackEnds* CreateHashing(const TModuleParams& mp);
}
