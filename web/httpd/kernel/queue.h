#pragma once

#include "syncro.h"
#include "chunks.h"

namespace NSrvKernel {
    class TChunkQueue {
    public:
        inline TChunkQueue(TContExecutor* e) throw ()
            : AllDone_(false)
            , Event_(e)
        {
        }

        inline void Push(TChunkList& lst) throw () {
            MoveTo(&lst, &Chunks_);
            Event_.Signal();
        }

        inline bool Pop(TChunkList& lst) throw () {
            while (Chunks_.Empty()) {
                if (AllDone_) {
                    return false;
                }

                Event_.Wait();
            }

            MoveTo(&Chunks_, &lst);

            return true;
        }

        inline void Stop() throw () {
            AllDone_ = true;
            Event_.Signal();
        }

        inline size_t InQueue() const throw () {
            return Chunks_.FullLength();
        }

    private:
        bool AllDone_;
        TMyEvent Event_;
        TChunkList Chunks_;
    };
}
