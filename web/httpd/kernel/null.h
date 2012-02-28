#pragma once

#include "iobase.h"

namespace NSrvKernel {
    class TNullStream: public IIoInput, public IIoOutput {
    public:
        static TNullStream& Instance() throw ();

    private:
        virtual void DoRecv(TChunkList&) {
        }

        virtual void DoSend(TChunkList&) {
        }
    };
}
