#pragma once

#include "iobase.h"

#include <util/stream/output.h>
#include <util/stream/zerocopy.h>

namespace NSrvKernel {
    class TIoInput: public IZeroCopyInput {
    public:
        inline TIoInput(IIoInput* slave)
            : Slave_(slave)
        {
        }

    private:
        virtual bool DoNext(const void** outPtr, size_t* outLen) {
            if (Filled_.Empty()) {
                if (!Fill()) {
                    return false;
                }
            }

            Cur_.Reset(Filled_.PopFront());

            *outPtr = Cur_->Data();
            *outLen = Cur_->Length();

            return true;
        }

        inline bool Fill() {
            return (bool)Recv(Slave_, Filled_);
        }

    private:
        TChunkList Filled_;
        TChunkPtr Cur_;
        IIoInput* Slave_;
    };

    class TIoOutput: public TOutputStream {
    public:
        inline TIoOutput(IIoOutput* slave)
            : Slave_(slave)
        {
        }

    private:
        virtual void DoWrite(const void* data, size_t len) {
            //TODO
            TChunkList lst;
            lst.Push(NewChunk(TStringBuf((const char*)data, len)));
            Slave_->Send(lst);
        }

    private:
        IIoOutput* Slave_;
    };
}
