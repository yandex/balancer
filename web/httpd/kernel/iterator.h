#pragma once

#include "chunks.h"
#include "iobase.h"

#include <util/generic/yexception.h>

namespace NSrvKernel {
    class TStreamError: public yexception {
    };

    //TODO - optimize some cases
    class TStreamIterator {
    public:
        inline TStreamIterator(IIoInput* slave) throw ()
            : S_(slave)
        {
        }

        inline TStreamIterator(TChunkList& data) throw ()
            : S_(&TNullStream::Instance())
        {
            L_.Swap(data);
        }

        inline size_t ReadSome(void* buf, size_t len) {
            TChunkList tmp;

            Next(len, tmp);

            return tmp.CopyData(buf, len);
        }

        inline void Read(void* bufin, size_t len) {
            char* buf = (char*)bufin;

            while (len) {
                if (size_t read = ReadSome(buf, len)) {
                    buf += read;
                    len -= read;
                } else {
                    ythrow TStreamError() << STRINGBUF("no data");
                }
            }
        }

        inline void Read(TChunkList& lst, size_t len) {
            while (len) {
                TChunkList tmp;

                Next(len, tmp);

                if (size_t readed = tmp.FullLength()) {
                    len -= readed;
                    MoveTo(&tmp, &lst);
                } else {
                    ythrow TStreamError() << STRINGBUF("no data");
                }
            }
        }

        inline bool Next(char& ch) {
            return ReadSome(&ch, 1);
        }

        inline char Next() {
            char ch;

            if (!Next(ch)) {
                ythrow TStreamError() << STRINGBUF("no data");
            }

            return ch;
        }

        inline void Next(size_t len, TChunkList& lst) {
            if (Peek()) {
                Split(len, L_, lst, L_);
            }
        }

        inline void Next(TChunkList& lst) {
            if (Peek()) {
                MoveTo(&L_, &lst);
            }
        }

        inline bool Peek() {
            if (L_.Empty()) {
                S_->Recv(L_);
            }

            return !L_.Empty();
        }

        inline void UnGet(TChunkList& data) throw () {
            MoveTo(&L_, &data);
            MoveTo(&data, &L_);
        }

    private:
        IIoInput* S_;
        TChunkList L_;
    };

    class TIoStreamIterator: public TStreamIterator, public IIoInput {
    public:
        inline TIoStreamIterator(IIoInput* slave)
            : TStreamIterator(slave)
        {
        }

    private:
        virtual void DoRecv(TChunkList& lst) {
            Next(lst);
        }
    };
}
