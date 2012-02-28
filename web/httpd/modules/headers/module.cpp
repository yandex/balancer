#include "module.h"

#include <web/httpd/kernel/http.h>
#include <web/httpd/kernel/regexp.h>
#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/config.h>
#include <web/httpd/kernel/bufalloc.h>
#include <web/httpd/kernel/split.h>
#include <web/httpd/kernel/address.h>

#include <util/stream/mem.h>
#include <util/random/random.h>
#include <util/generic/ptr.h>
#include <util/generic/pair.h>
#include <util/generic/hash.h>
#include <util/generic/vector.h>

using namespace NSrvKernel;

namespace {
    const size_t UI64LEN = 20;
    const size_t REALIPLEN = HOSTLENBUF;
    const size_t STARTTIMELEN = UI64LEN;
    const size_t REQIDLEN = 2 * UI64LEN + 1;
    const size_t PORTLEN = UI64LEN;

    class TBuf: public TBufAlloc<REALIPLEN + STARTTIMELEN + REQIDLEN + PORTLEN> {
    };

    class IHdrGen {
    public:
        struct TParams {
            const TConnDescr* Descr;
            TBuf* Buf;

            inline char* Alloc(size_t len) const {
                return Buf->Alloc(len);
            }
        };

        virtual ~IHdrGen() {
        }

        virtual TStringBuf Gen(const TParams& p) const = 0;
    };

    struct TRealIpGen: public IHdrGen {
        virtual TStringBuf Gen(const TParams& p) const {
            return FormatHost(p.Descr->RemoteAddr(), p.Alloc(REALIPLEN), REALIPLEN);
        }
    };

    struct TStartTimeGen: public IHdrGen {
        virtual TStringBuf Gen(const TParams& p) const {
            char* buf = p.Alloc(STARTTIMELEN);

            return TStringBuf(buf, ToString(p.Descr->Properties->Start.MicroSeconds(), buf, STARTTIMELEN));
        }
    };

    struct TReqIdGen: public IHdrGen {
        virtual TStringBuf Gen(const TParams& p) const {
            char* buf = p.Alloc(REQIDLEN);
            TMemoryOutput out(buf, REQIDLEN);

            out << p.Descr->Properties->Start.MicroSeconds() << '-' << RandomNumber<ui64>();

            return TStringBuf(buf, out.Buf());
        }
    };
}

MODULE(headers) {
    struct THdrDel {
        inline void operator() (THeader* hdr) {
            TMatcher m(*R);

            if (Match(m, hdr->Key).Final()) {
                delete hdr;
            }
        }

        const TFsm* R;
    };

public:
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , Fsm_(TFsm::False())
        , KeepRealIpFsm_(TFsm::False())
        , KeepRealIp_(false)
        , AddReqidToCGI_(0)
    {
        Config->ForEach(this);

        if (!Module_) {
            ythrow TConfigParseError() << "no module configured";
        }

        if (Generators_.empty()) {
            ythrow TConfigParseError() << "no headers configured";
        }
    }

    START_PARSE {
        Stroka hdr;
        const Stroka realIpKey = "realip";
        const Stroka keepRealIpKey = "keep_realip";
        const Stroka realPortKey = "realport";
        const Stroka reqIdKey = "reqid";
        const Stroka startKey = "start";

        ON_KEY(realIpKey, hdr) {
            Add(realIpKey, hdr, new TRealIpGen());

            return;
        }

        ON_KEY(keepRealIpKey, hdr) {
            const TFsm fsm(hdr, TFsm::TOptions().SetCaseInsensitive(true));
            KeepRealIpFsm_ = fsm;
            KeepRealIpGen_ = TGenerator(hdr, new TRealIpGen());
            KeepRealIp_ = true;
            return;
        }

        ON_KEY(reqIdKey, hdr) {
            Add(reqIdKey, hdr, new TReqIdGen());

            return;
        }

        ON_KEY(startKey, hdr) {
            Add(startKey, hdr, new TStartTimeGen());

            return;
        }

        ON_KEY("add_reqid_to_cgi", AddReqidToCGI_) {
            return;
        }

        {
            Module_.Reset(Loader->MustLoad(key, Copy(value->AsSubConfig())).Release());

            return;
        }
    } END_PARSE

    virtual void DoRun(const TConnDescr& descr) {
        TBuf buf;

        const IHdrGen::TParams p = {
              &descr
            , &buf
        };

        // check if we have X-Forwarded-For-Y and keep it
        if (KeepRealIp_) {
            bool found = false;
            for (TRequest::THeaders::TConstIterator it = descr.Request->Headers().Begin(); it != descr.Request->Headers().End(); ++it) {
                TMatcher m(KeepRealIpFsm_);

                if (Match(m, it->Key).Final()) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                descr.Request->AddHeader(KeepRealIpGen_.first, KeepRealIpGen_.second->Gen(p));
            }
        }

        {
            THdrDel del = {
                &Fsm_
            };

            //erase same headers
            descr.Request->ForEachHeader(del);
        }

        // Appply all generators
        TChunkPtr reqidPtr;

        for (TGenerators::const_iterator it = Generators_.begin(); it != Generators_.end(); ++it) {
            const TGenerator& gen = it->second;
            TChunkPtr ptr = NewChunk(gen.second->Gen(p));

            if (AddReqidToCGI_ && it->first == "reqid") {
                reqidPtr = ptr->Copy();
            }

            descr.Request->AddHeader(gen.first, ptr);
        }

        // fix CGI
        if (AddReqidToCGI_ && !!reqidPtr) {
            TChunkList req;

            CopyTo(&descr.Request->Request(), &req);

            TChunkList method;
            TChunkList proto;

            Split(4, req, method, req);
            Split(req.FullLength() - 9, req, req, proto);

            TConnDescr d = descr;

            req.Push(STRINGBUF("&reqid="));
            req.Push(reqidPtr);

            TChunkList fullReq;
            CopyTo(&method, &fullReq);
            CopyTo(&req, &fullReq);
            CopyTo(&proto, &fullReq);

            TRequest fixedReq(fullReq);
            for (TRequest::THeaders::TConstIterator it = descr.Request->Headers().Begin(); it != descr.Request->Headers().End(); ++it) {
                THeader* hdr = fixedReq.NewHeader();

                CopyTo(&it->Key, &hdr->Key);
                CopyTo(&it->Value, &hdr->Value);
            }

            d.Request = &fixedReq;
            Module_->Run(d);
        } else {
            Module_->Run(descr);
        }
    }

    virtual void DoStats(TOutputStream& out) {
        out << "<inuse>";

        for (TGenerators::const_iterator it = Generators_.begin(); it != Generators_.end(); ++it) {
            const Stroka& name = it->first;
            const Stroka& hdr = it->second.first;

            out << "<" << name << ">" << hdr << "</" << name << ">";
        }

        out << "</inuse>";

        Module_->PrintStats(out);
    }

private:
    inline void Add(const Stroka& key, const Stroka& hdr, TAutoPtr<IHdrGen> gen) {
        const TFsm fsm(hdr, TFsm::TOptions().SetCaseInsensitive(true));

        if (Generators_.empty()) {
            Fsm_ = fsm;
        } else {
            Fsm_ = Fsm_ | fsm;
        }

        Generators_[key] = TGenerator(hdr, gen);
    }

private:
    typedef TPair<Stroka, TAutoPtr<IHdrGen> > TGenerator;
    typedef yhash<Stroka, TGenerator> TGenerators;
    yset<Stroka> KeepHeaders_;
    TGenerators Generators_;
    TFsm Fsm_;

    TFsm KeepRealIpFsm_;
    TGenerator KeepRealIpGen_;
    bool KeepRealIp_;

    ui32 AddReqidToCGI_;

    TAutoPtr<IModule> Module_;
};

IModuleHandle* NModHeaders::Handle() {
    return TModule::ModuleHandle();
}
