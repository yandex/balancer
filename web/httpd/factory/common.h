#pragma once

namespace NSrvKernel {
    class IModuleFactory;
}

namespace NModulesFactory {
    using namespace NSrvKernel;

    IModuleFactory* CommonModules();
}
