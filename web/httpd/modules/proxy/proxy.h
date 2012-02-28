#pragma once

#include <util/system/defaults.h>

class TContExecutor;

namespace NSrvKernel {
    class IIoInput;
    class IIoOutput;

    struct TProxy {
        void Run();

        TContExecutor* E;
        IIoInput* CI;
        IIoOutput* CO;
        IIoInput* BI;
        IIoOutput* BO;
        bool FailOnEmptyReply;
    };
}
