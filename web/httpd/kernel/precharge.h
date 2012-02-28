#pragma once

#include "iobase.h"

namespace NSrvKernel {
    class TPrechargedInput: public IIoInput {
    public:
        inline TPrechargedInput(TChunkList& chunks)
            : Chunks_(chunks)
        {
        }

    private:
        virtual void DoRecv(TChunkList& lst) {
            MoveTo(&Chunks_, &lst);
        }

    private:
        TChunkList& Chunks_;
    };

    class TOwnedInput: public TChunkList, public IIoInput {
    public:
        inline TOwnedInput() throw () {
        }

        inline TOwnedInput(TChunkList& lst) throw () {
            MoveTo(&lst, static_cast<TChunkList*>(this));
        }

    private:
        virtual void DoRecv(TChunkList& lst) {
            MoveTo(static_cast<TChunkList*>(this), &lst);
        }
    };
}
