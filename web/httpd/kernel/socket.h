#pragma once

#include "iobase.h"

class TSocketHolder;

namespace NSrvKernel {
    struct TSock {
        inline TSock(TSocketHolder* s, TContExecutor* e)
            : S_(s)
            , E_(e)
        {
        }

        TSocketHolder* S_;
        TContExecutor* E_;
    };

    class TSocketIn: public virtual TSock, public IIoInput {
    public:
        inline TSocketIn(TSocketHolder* s, TContExecutor* e)
            : TSock(s, e)
        {
        }

    private:
        virtual void DoRecv(TChunkList& lst);

        inline TChunkPtr NextChunk() {
            if (!L_) {
                return NewChunk();
            }

            return L_;
        }

    private:
        TChunkPtr L_;
    };

    class TSocketOut: public virtual TSock, public IIoOutput, public IPollEvent {
    public:
        inline TSocketOut(TSocketHolder* s, TContExecutor* e, size_t maxInQueue)
            : TSock(s, e)
            , IPollEvent(*s, CONT_POLL_WRITE)
            , Wake_(0)
            , MaxInQueue_(maxInQueue)
            , Err_(0)
        {
            PrepareSocket();
        }

        ~TSocketOut();

        inline void Flush() {
            Flush(0);
        }

    private:
        void Flush(size_t keep);
        void PrepareSocket();

        virtual void DoSend(TChunkList& lst);
        virtual void OnPollEvent(int status) throw ();

    private:
        TChunkList W_;
        TCont* Wake_;
        size_t MaxInQueue_;
        int Err_;
    };

    class TSocketIo: public TSocketIn, public TSocketOut {
    public:
        inline TSocketIo(TSocketHolder* s, TContExecutor* e, size_t maxInQueue = 32 * 1024)
            : TSock(s, e)
            , TSocketIn(s, e)
            , TSocketOut(s, e, maxInQueue)
        {
        }
    };
}
