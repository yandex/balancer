#pragma once

#include "config.h"

namespace NSrvKernel {
    TAutoPtr<IConfig> LuaConfigParser(TInputStream& in);
}
