#pragma once

#include "iobase.h"

namespace NSrvKernel {
    class TRewindAbleInput: public IIoInput {
    public:
        inline TRewindAbleInput(IIoInput* slave)
            : Slave_(slave)
        {
        }

        inline void Rewind() throw () {
            MoveTo(&Tmp_, &Old_);
        }

        inline TChunkList& ExtractChunks() throw () {
            Rewind();

            return Old_;
        }

        inline void ExtractChunks(TChunkList& lst) throw () {
            MoveTo(&ExtractChunks(), &lst);
        }

        inline IIoInput* Slave() const throw () {
            return Slave_;
        }

        inline const TChunkList& Tmp() const throw() {
            return Tmp_;
        }

    private:
        virtual void DoRecv(TChunkList& lst) {
            TChunkList tmp;

            if (Old_.Empty()) {
                Slave_->Recv(tmp);
            } else {
                MoveTo(&Old_, &tmp);
            }

            CopyTo(&tmp, &lst);
            MoveTo(&tmp, &Tmp_);
        }

    private:
        TChunkList Old_;
        TChunkList Tmp_;
        IIoInput* Slave_;
    };
}
