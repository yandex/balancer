#pragma once

#include "iobase.h"

namespace NSrvKernel {
    class TCountOutput: public IIoOutput {
    public:
        inline TCountOutput(IIoOutput* slave) throw ()
            : Slave_(slave)
            , Written_(0)
        {
        }

        inline size_t Written() const throw () {
            return Written_;
        }

    private:
        virtual void DoSend(TChunkList& lst) {
            Written_ += NSrvKernel::Send(Slave_, lst);
        }

    private:
        IIoOutput* Slave_;
        size_t Written_;
    };
}
