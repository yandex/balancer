#pragma once

namespace NSrvKernel {
    class IModuleHandle;
}

namespace NModTls {
    using namespace NSrvKernel;

    IModuleHandle* Handle();
}
