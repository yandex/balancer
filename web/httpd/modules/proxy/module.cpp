#include "proxy.h"
#include "module.h"

#include <web/httpd/kernel/http.h>
#include <web/httpd/kernel/concat.h>
#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/chunks.h>
#include <web/httpd/kernel/config.h>
#include <web/httpd/kernel/socket.h>
#include <web/httpd/kernel/errors.h>
#include <web/httpd/kernel/limitio.h>
#include <web/httpd/kernel/headers.h>
#include <web/httpd/kernel/bufalloc.h>
#include <web/httpd/kernel/precharge.h>
#include <web/httpd/kernel/iterator.h>

#include <util/string/hex.h>

using namespace NSrvKernel;
using namespace NModProxy;

#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define MAX3(a, b, c) MAX2(MAX2((a), (b)), (c))

namespace {
    struct TTimeOutTable: public yvector<TDuration> {
        inline TDuration TimeOut(size_t num) const throw () {
            return (*this)[Min(num, size() - 1)];
        }
    };

    class TChunkedTransfer: public TStreamIterator, public IIoInput {
    public:
        inline TChunkedTransfer(IIoInput* slave)
            : TStreamIterator(slave)
            , L_(ParseNextChunkLength())
        {
        }

        virtual void DoRecv(TChunkList& lst) {
            if (!L_) {
                return;
            }

            Next(L_, lst);
            L_ -= lst.FullLength();

            if (!L_) {
                L_ = ParseNextChunkLength();
            }
        }

    private:
        size_t ParseNextChunkLength() {
            size_t ret = 0;
            char ch = SkipWS();

            while (true) {
                switch (ch) {
                    case '\r':
                        break;

                    case '\n':
                        return ret;

                    default:
                        ret = ret * 16 + Char2Digit(ch);
                        break;
                }

                ch = Next();
            }
        }

        inline char SkipWS() {
            while (true) {
                char ch = Next();

                switch (ch) {
                    case '\r':
                    case '\n':
                        break;

                    default:
                        return ch;
                }
            }
        }

    private:
        size_t L_;
    };

    struct THelper {
        inline THelper(TChunkList& unparsed, IIoInput* slave)
            : Slave(&Unparsed, slave)
        {
            Unparsed.Swap(unparsed);
        }

        TOwnedInput Unparsed;
        TConcatInput Slave;
    };

    class TPrechargedConcatInput: public TBufAlloced, public TConcatInput {
    public:
        inline TPrechargedConcatInput(TChunkList& unparsed, IIoInput* slave)
            : TConcatInput(&O_, slave)
        {
            O_.Swap(unparsed);
        }

    private:
        TOwnedInput O_;
    };

    template <class T>
    struct TInput: public TBufAlloced, public THelper, public T {
        inline TInput(TChunkList& unparsed, IIoInput* slave)
            : THelper(unparsed, slave)
            , T(&Slave)
        {
        }

        template <class T1>
        inline TInput(TChunkList& unparsed, IIoInput* slave, const T1& t1)
            : THelper(unparsed, slave)
            , T(&Slave, t1)
        {
        }
    };

    struct TErrHndl {
        static inline void OnErr() throw () {
        }
    };

    typedef TInput<TChunkedTransfer> TChunkedInput;
    typedef TInput<TLengthLimitedInput<TErrHndl> > TContentLengthInput;

    class TLimitedBackEndInput: public IIoInput {
    public:
        inline TLimitedBackEndInput(IIoInput* slave)
            : S_(slave)
        {
        }

        virtual void DoRecv(TChunkList& lst) {
            if (!P_) {
                BuildInputChain(lst);

                return;
            }

            P_->Recv(lst);
        }

        void BuildInputChain(TChunkList& lst) {
            TChunkList unparsed;
            TResponse r(S_, unparsed);
            TResponseProperties props(&r);

            r.BuildTo(lst);

            if (props.ChunkedTransfer) {
                P_.Reset(new (Alloc_) TChunkedInput(unparsed, S_));
            } else if (props.ContentLength) {
                P_.Reset(new (Alloc_) TContentLengthInput(unparsed, S_, props.ContentLength));
            } else {
                P_.Reset(new (Alloc_) TPrechargedConcatInput(unparsed, S_));
            }
        }

    private:
        IIoInput* S_;
        THolder<IIoInput, TDestructor> P_;
        TBufAlloc<MAX3(sizeof(TChunkedInput), sizeof(TContentLengthInput), sizeof(TPrechargedConcatInput))> Alloc_;
    };
}

MODULE(proxy) {
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , Port_(80)
        , ConnectTimeOut_(TDuration::Max())
        , NumConnecting_(0)
        , FailOnEmptyReply_(false)
        , MaxQueueLength_(Max<size_t>())
    {
        Config->ForEach(this);

        if (!Host_) {
            ythrow TConfigParseError() << "no host configured for proxy";
        }

        Addr_.Reset(new TNetworkAddress(Host_, Port_));
    }

    START_PARSE {
        ON_KEY("host", Host_) {
            return;
        }

        ON_KEY("port", Port_) {
            return;
        }

        ON_KEY("timeout", ConnectTimeOut_) {
            return;
        }

        ON_KEY("fail_on_empty_reply", FailOnEmptyReply_) {
            return;
        }

        ON_KEY("maxqueue", MaxQueueLength_) {
            return;
        }
    } END_PARSE

    virtual void DoStats(TOutputStream& out) {
        out << "<host>" << Host_ << "</host>";
        out << "<port>" << Port_ << "</port>";
        out << "<connecting>" << NumConnecting_ << "</connecting>";
    }

    virtual void DoRun(const TConnDescr& descr) {
        TCont* c = descr.Properties->Cont;
        TSocketHolder backEnd;

        ++NumConnecting_;
        const int ret = c->Connect(backEnd, *Addr_, ConnectTimeOut_);
        --NumConnecting_;

        if (ret) {
            ythrow TSystemError(ret) << STRINGBUF("connect error");
        }

        //TODO
        TSocketIo backEndIo(&backEnd, Executor, MaxQueueLength_);
        TLimitedBackEndInput limitedInput(&backEndIo);
        THttpRequestInput parsedRequest(descr.Request);

        parsedRequest.Push(STRINGBUF("Connection: Close\r\n"));

        TConcatInput input(&parsedRequest, descr.Input);

        TProxy proxy = {
              Executor
            , &input
            , descr.Output
            , &limitedInput
            , &backEndIo
            , FailOnEmptyReply_
        };

        proxy.Run();
    }

private:
    Stroka Host_;
    ui16 Port_;
    TDuration ConnectTimeOut_;
    TAutoPtr<TNetworkAddress> Addr_;
    size_t NumConnecting_;
    bool FailOnEmptyReply_;
    size_t MaxQueueLength_;
};

IModuleHandle* NModProxy::Handle() {
    return TModule::ModuleHandle();
}
