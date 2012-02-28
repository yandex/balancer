#pragma once

#include "chunks.h"

#include <util/ycoroutine.h>
#include <util/system/yassert.h>

namespace NSrvKernel {
    static inline TChunkList& Cleared(TChunkList& lst) throw () {
        lst.Clear();

        return lst;
    }

    static inline bool HaveEmptyChunks(const TChunkList& lst) throw () {
        for (TChunkList::TConstIterator it = lst.Begin(); it != lst.End(); ++it) {
            if (!it->Length()) {
                return true;
            }
        }

        return false;
    }

    class IIoInput {
    public:
        inline void Recv(TChunkList& lst) {
            DoRecv(Cleared(lst));
        }

        void ReadAll(TChunkList& lst);

        virtual ~IIoInput() {
        }

    private:
        virtual void DoRecv(TChunkList& lst) = 0;
    };

    class IIoOutput {
    public:
        inline void Send(TChunkList& lst) {
            YASSERT(!HaveEmptyChunks(lst));

            DoSend(lst);
        }

        virtual ~IIoOutput() {
        }

    private:
        virtual void DoSend(TChunkList& lst) = 0;
    };

    static inline size_t Send(IIoOutput* out, TChunkList& lst) {
        const size_t ret = lst.FullLength();
        out->Send(lst);

        return ret;
    }

    static inline size_t Recv(IIoInput* in, TChunkList& lst) {
        in->Recv(lst);

        return lst.FullLength();
    }

    IIoOutput& StdOut();
    IIoOutput& StdErr();
}
