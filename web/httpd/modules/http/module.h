#pragma once


namespace NSrvKernel {
    class IModuleHandle;
}

namespace NModHttp {
    using namespace NSrvKernel;

    IModuleHandle* Handle();
}
