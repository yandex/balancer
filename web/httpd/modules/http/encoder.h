#include <web/httpd/kernel/http.h>
#include <web/httpd/kernel/chunks.h>

#include <util/string/cast.h>

namespace NSrvKernel {
    class TIdentityTransferEncoder: public TChunkList {
    public:
        inline void OnDataPart(TChunkList& part) {
            MoveTo<TChunkList>(&part, this);
        }

        inline void OnFinish() {
        }
    };

    class TChunkedTransferEncoder: public TChunkList {
    public:
        inline void OnDataPart(TChunkList& part) {
            WriteChunk(part);
        }

        inline void OnFinish() {
            TChunkList lst;

            WriteChunk(lst);
        }

    private:
        inline void WriteChunk(TChunkList& lst) {
            TChunkPtr buf(NewChunk(16));
            char* data = buf->Data();

            Push(buf->SubChunk(data, data + IntToString<16>(lst.FullLength(), data, buf->Length())));
            Push(CRLF);
            MoveTo<TChunkList>(&lst, this);
            Push(CRLF);
        }

    private:
        char Buf_[16];
    };

    template <class T>
    class THttpEncoder: public IIoOutput, public T, public TPushParseContext {
    public:
        inline THttpEncoder(IIoOutput* slave, TChunkList& customHeaders)
            : CustomHeaders_(&customHeaders)
            , S_(slave)
        {
        }

        inline void Finish() {
            if (CustomHeaders_) {
                ythrow THttpError(500) << STRINGBUF("malformed http stream");
            }

            this->OnFinish();
            Flush();
        }

    private:
        virtual void DoSend(TChunkList& lst) {
            if (CustomHeaders_) {
                if (ParseChunk(lst)) {
                    //send headers
                    BuildTo(*this, CustomHeaders_);
                    CustomHeaders_ = 0;

                    //and last data part
                    if (!lst.Empty()) {
                        this->OnDataPart(lst);
                    }
                }
            } else {
                this->OnDataPart(lst);
            }

            Flush();
        }

        inline void Flush() {
            if (!this->Empty()) {
                S_->Send(*this);
                this->Clear();
            }
        }

    private:
        TChunkList* CustomHeaders_;
        IIoOutput* S_;
    };
}
