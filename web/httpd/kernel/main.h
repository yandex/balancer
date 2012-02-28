#pragma once

class TInputStream;
class TContExecutor;

namespace NSrvKernel {
    class IModuleFactory;

    void RunMain(int argc, char** argv, IModuleFactory* loader);
    void RunMain(TInputStream& cfg, IModuleFactory* loader);
    void RunMain(TInputStream& cfg, TContExecutor* executor, IModuleFactory* loader);
}
