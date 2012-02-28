#include "module.h"

#include <web/httpd/kernel/http.h>
#include <web/httpd/kernel/format.h>
#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/address.h>

using namespace NSrvKernel;

namespace {
    struct TReqFmt {
        inline TReqFmt(const TRequest* req)
            : Req(req)
        {
        }

        inline void Write(TOutputStream& out) const {
            if (!Req) {
                return;
            }

            out << Req->Request() << STRINGBUF(" \"");

            for (TRequest::THeaders::TConstIterator it = Req->Headers().Begin(); it != Req->Headers().End(); ++it) {
                out << it->Key << STRINGBUF(": ") << it->Value << STRINGBUF(" ");
            }

            out << STRINGBUF("\"");
        }

        const TRequest* Req;
    };

    static inline TReqFmt Req(const TRequest* req) throw () {
        return TReqFmt(req);
    }
}

template <>
void Out<TReqFmt>(TOutputStream& out, const TReqFmt& req) {
    req.Write(out);
}

MODULE(accesslog) {
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , Log_(Control->DefaultLog())
    {
        Config->ForEach(this);

        if (!M_) {
            ythrow TConfigParseError() << "no module configured";
        }
    }

    START_PARSE {
        {
            Stroka log;

            ON_KEY ("log", log) {
                Log_ = Control->Log(log);

                return;
            }
        }

        {
            M_.Reset(Loader->MustLoad(key, Copy(value->AsSubConfig())).Release());

            return;
        }
    } END_PARSE

    virtual void DoRun(const TConnDescr& descr) {
        *Log_ << descr.Properties->Start.MicroSeconds() << STRINGBUF(" ")
              << FormatHost(descr.RemoteAddr()) << STRINGBUF(" ")
              << Req(descr.Request) << Endl;

        M_->Run(descr);
    }

    virtual void DoStats(TOutputStream& out) {
        M_->PrintStats(out);
    }

private:
    THolder<IModule> M_;
    TLog* Log_;
};

IModuleHandle* NModAccessLog::Handle() {
    return TModule::ModuleHandle();
}
