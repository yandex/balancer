#include "cast.h"
#include "http.h"
#include "regexp.h"
#include "headers.h"

using namespace NSrvKernel;

namespace {
    struct THeadersToCheck: public TFsm {
        inline THeadersToCheck()
            : TFsm(TFsm::False())
        {
            TFsm& f = *this;
            TFsm::TOptions options;

            options.SetCaseInsensitive(true);

            f =     TFsm("Connection", options);
            f = f | TFsm("Keep-Alive", options);
            f = f | TFsm("Content-Length", options);
            f = f | TFsm("Transfer-Encoding", options);
            f = f | TFsm("Server", options);
        }

        static inline const THeadersToCheck* Instance() throw () {
            return Singleton<THeadersToCheck>();
        }
    };

    struct TResponseParser {
        inline TResponseParser(TResponseProperties* props)
            : Props(props)
            , Fsm(THeadersToCheck::Instance())
        {
        }

        inline void operator() (THeader* hdr) {
            TMatcher m(*Fsm);

            if (Match(m, hdr->Key).Final()) {
                switch (*m.MatchedRegexps().first) {
                    //Transfer-Encoding
                    case 3:
                        Props->ChunkedTransfer = true;
                        break;

                    //Content-Length
                    case 2:
                        Props->ContentLength = FromString<size_t>(hdr->Value);
                        break;
                }

                delete hdr;
            }
        }

        inline void Parse(TResponse* r) {
            r->ForEachHeader(*this);
        }

        TResponseProperties* Props;
        const THeadersToCheck* Fsm;
    };

    static inline unsigned HttpVersion(const TChunkList& req) {
        if (req.Empty()) {
            return 0;
        }

        const TChunk& last = *(--req.End());

        if (last.Empty()) {
            return 0;
        }

        return *((const char*)last.Data() + last.Length() - 1) - '0';
    }
}

TResponseProperties::TResponseProperties(TResponse* r)
    : ChunkedTransfer(false)
    , ContentLength(0)
{
    TResponseParser(this).Parse(r);
}

TRequestProperties::TRequestProperties(TRequest* r)
    : KeepAlive(HttpVersion(r->Request()))
    , ChunkedTransfer(KeepAlive)
    , ContentLength(TResponseProperties(r).ContentLength)
{
}

void NSrvKernel::InitHeadersParser() {
    THeadersToCheck::Instance();
}
