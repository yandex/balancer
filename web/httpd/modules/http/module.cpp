#include "module.h"
#include "encoder.h"

#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/concat.h>
#include <web/httpd/kernel/limitio.h>
#include <web/httpd/kernel/headers.h>
#include <web/httpd/kernel/precharge.h>
#include <web/httpd/kernel/iterator.h>

using namespace NSrvKernel;

namespace {
    struct TErrFunc {
        static inline TIOStatus OnErr() {
            ythrow THttpParseError(413) << STRINGBUF("too big request");
        }
    };

    struct TFixedLengthInput: public IIoInput {
        inline TFixedLengthInput(TIoStreamIterator* it, size_t len) throw ()
            : It(it)
            , Len(len)
        {
        }

        virtual void DoRecv(TChunkList& lst) {
            if (Len) {
                It->Next(Len, lst);
                Len -= lst.FullLength();
            }
        }

        TIoStreamIterator* It;
        size_t Len;
    };
}

MODULE(http) {
    struct TParsedRequest: public TRequest {
        TParsedRequest(TIoStreamIterator* it, size_t maxlen) {
            TChunkList unparsed;

            if (maxlen) {
                TLengthLimitedInput<TErrFunc> in(it, maxlen);

                Parse(&in, unparsed);
            } else {
                Parse(it, unparsed);
            }

            it->UnGet(unparsed);
        }
    };

    struct TContext: public TConnDescr {
        inline TContext(const TModule* parent, const TConnDescr& conn)
            : TConnDescr(conn)
            , Parent(parent)
            , ParsedRequest(Iter(), Parent->MaxLen_)
            , Props(&ParsedRequest)
        {
            Request = &ParsedRequest;
        }

        inline bool Run() {
            if (Request->Request().FullLength() > Parent->MaxReq_) {
                ythrow THttpParseError(414) << STRINGBUF("too big url");
            }

            if (!Parent->KeepAlive_) {
                Props.KeepAlive = false;
            }

            if (Props.KeepAlive) {
                //Headers.Push(STRINGBUF("Connection: Keep-Alive\r\n"));
            } else {
                Headers.Push(STRINGBUF("Connection: Close\r\n"));
            }

            if (Props.ChunkedTransfer) {
                Headers.Push(STRINGBUF("Transfer-Encoding: Chunked\r\n"));

                RunEncoded<TChunkedTransferEncoder>();
            } else {
                RunEncoded<TIdentityTransferEncoder>();
            }

            return Props.KeepAlive;
        }

        template <class TTransfer>
        void RunEncoded() {
            THttpEncoder<TTransfer> out(Output, Headers);

            Output = &out;
            RunDecoded();
            out.Finish();
        }

        void RunDecoded() {
            if (Props.ContentLength) {
                TFixedLengthInput in(Iter(), Props.ContentLength);

                Input = &in;
                Parent->M_->Run(*this);
            } else {
                Input = &TNullStream::Instance();
                Parent->M_->Run(*this);
            }
        }

        //TODO - make Input be it
        inline TIoStreamIterator* Iter() const throw () {
            return static_cast<TIoStreamIterator*>(Input);
        }

        const TModule* Parent;
        TChunkList Headers;
        TParsedRequest ParsedRequest;
        TRequestProperties Props;
    };

public:
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , ParseError_(0)
        , Discarded_(0)
        , MaxLen_(0)
        , MaxReq_(Max<size_t>())
        , KeepAlive_(true)
    {
        Config->ForEach(this);

        if (!M_) {
            ythrow TConfigParseError() << "no module configured";
        }
    }

    START_PARSE {
        ON_KEY("maxlen", MaxLen_) {
            return;
        }

        ON_KEY("maxreq", MaxReq_) {
            return;
        }

        ON_KEY("keepalive", KeepAlive_) {
            return;
        }

        {
            M_.Reset(Loader->MustLoad(key, Copy(value->AsSubConfig())).Release());

            return;
        }
    } END_PARSE

    template <class It>
    static inline bool Peek(It& it) {
        try {
            return it.Peek();
        } catch (const TSystemError&) {
        }

        return false;
    }

    virtual void DoRun(const TConnDescr& descr) {
        TIoStreamIterator it(descr.Input);

        while (true) {
            if (!Peek(it)) {
                ++Discarded_;

                //no data in stream
                break;
            }

            try {
                if (!TContext(this, descr.Copy(&it)).Run()) {
                    //not keep alive
                    break;
                }
            } catch (const THttpParseError&) {
                ++ParseError_;

                throw;
            }
        }
    }

    virtual void DoStats(TOutputStream& out) {
        out << "<badhttp>" << ParseError_ << "</badhttp>";
        out << "<discarded>" << Discarded_ << "</discarded>";
        M_->PrintStats(out);
    }

private:
    THolder<IModule> M_;
    ui64 ParseError_;
    ui64 Discarded_;
    size_t MaxLen_;
    size_t MaxReq_;
    bool KeepAlive_;
};

IModuleHandle* NModHttp::Handle() {
    return TModule::ModuleHandle();
}
