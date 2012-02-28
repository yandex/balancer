#pragma once

#include "null.h"
#include "iobase.h"

namespace NSrvKernel {
    class TConcatInput: public IIoInput {
    public:
        inline TConcatInput(IIoInput* f, IIoInput* s)
            : F_(f)
            , S_(s)
        {
        }

    private:
        virtual void DoRecv(TChunkList& lst) {
            F_->Recv(lst);

            if (lst.Empty()) {
                F_ = S_;
                S_ = &TNullStream::Instance();

                F_->Recv(lst);
            }
        }

    private:
        IIoInput* F_;
        IIoInput* S_;
    };
}
