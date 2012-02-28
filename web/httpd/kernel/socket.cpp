#include "alloc.h"
#include "socket.h"
#include "split.h"

using namespace NSrvKernel;

typedef TOutputStream::TPart TPart;

static inline void CombinePart(TChunkList& lst, TChunkList& out) {
    if (lst.Empty()) {
        return;
    }

    TChunkPtr chunk(NewChunk(lst.FullLength()));

    chunk->Shrink(lst.CopyData((char*)chunk->Data(), chunk->Length()));

    out.Push(chunk);
    lst.Clear();
}

static inline void Combine(TChunkList& lst, TChunkList& out) {
    TChunkList part;

    while (!lst.Empty()) {
        TChunkPtr cur = lst.PopFront();

        if (cur->Length() < 128) {
            part.Push(cur);
        } else {
            CombinePart(part, out);
            out.Push(cur);
        }
    }

    CombinePart(part, out);
}

TSocketOut::~TSocketOut() {
    E_->Poller()->Remove(this);
}

void TSocketOut::PrepareSocket() {
    SetNoDelay(*S_, true);
}

void TSocketOut::Flush(size_t keep) {
    while (W_.FullLength() > keep) {
        if (Err_) {
            ythrow TSystemError(Err_) << STRINGBUF("send failed");
        }

        Wake_ = E_->Running()->ContPtr();
        Wake_->SleepI();
        Wake_ = 0;
    }
}

void TSocketOut::DoSend(TChunkList& in) {
    MoveTo(&in, &W_);
    E_->Poller()->Schedule(this);
    Flush(MaxInQueue_);
}

void TSocketOut::OnPollEvent(int status) throw () {
    if (Wake_) {
        Wake_->ReSchedule();
    }

    if (status) {
        Err_ = status;
        E_->Poller()->Remove(this);

        return;
    }

    TCont* c = E_->Running()->ContPtr();
    THolder<void, TFreeRange> parts(AllocMem(W_.Size() * sizeof(TPart)).Mem);
    TPart* pb = (TPart*)parts.Get();
    TPart* pe = pb;

    for (TChunkList::TConstIterator it = W_.Begin(); it != W_.End(); ++it) {
        *pe++ = TPart(it->Data(), it->Length());
    }

    TContIOVector vec(pb, pe - pb);

    const ssize_t res = c->DoWriteVector(*S_, &vec);

    if (res < 0) {
        const int err = (int)-res;

        VERIFY(!TCont::IsBlocked(err), "shit happen");

        Err_ = err;
        E_->Poller()->Remove(this);
    } else {
        if ((size_t)res == W_.FullLength()) {
            //full write complete, nothing to wait
            W_.Clear();
            E_->Poller()->Remove(this);
        } else {
            //some data left, continue wait
            TChunkList tmp;

            Split((size_t)res, W_, tmp, W_);
        }
    }
}

void TSocketIn::DoRecv(TChunkList& lst) {
    TChunkPtr chunk = NextChunk();
    const size_t readed = E_->Running()->ContPtr()->ReadI(*S_, chunk->Data(), chunk->Length()).Checked();

    if (readed) {
        if (readed == chunk->Length()) {
            //fill full chunk
            lst.Push(chunk);
        } else {
            const size_t left = chunk->Length() - readed;

            if (left < readed) {
                //too small data chunk left, better do not reuse it
                chunk->Shrink(readed);
                lst.Push(chunk);
            } else {
                //something left for future use
                lst.Push(chunk->SubChunk(readed));
                L_ = chunk->SubChunk(readed, chunk->Length());
            }
        }
    }
}
