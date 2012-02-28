#pragma once

#include "iobase.h"
#include "split.h"

namespace NSrvKernel {
    template <class TErr>
    class TLengthLimitedInput: public IIoInput {
    public:
        inline TLengthLimitedInput(IIoInput* s, size_t limit)
            : S_(s)
            , L_(limit)
        {
        }

        virtual void DoRecv(TChunkList& lst) {
            if (!L_) {
                TErr::OnErr();

                return;
            }

            S_->Recv(lst);
            Cut(L_, lst);
            L_ -= lst.FullLength();
        }

    private:
        IIoInput* S_;
        size_t L_;
    };
}
