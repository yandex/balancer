#include "module.h"

#include <web/httpd/kernel/log.h>
#include <web/httpd/kernel/http.h>
#include <web/httpd/kernel/null.h>
#include <web/httpd/kernel/config.h>
#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/syncro.h>
#include <web/httpd/kernel/countio.h>
#include <web/httpd/kernel/address.h>
#include <web/httpd/modules/balancer/arbiter.h>

#include <util/random/random.h>
#include <util/datetime/base.h>
#include <util/ycoroutine.h>

using namespace NSrvKernel;
using namespace NModPinger;
using namespace NModBalancer;

namespace {
    class TRandomAddr: public IRemoteAddr {
    public:
        inline TRandomAddr()
            : Addr_(RandomNumber<TIpHost>(), RandomNumber<TIpPort>())
        {
        }

        virtual sockaddr* Addr() const {
            return (sockaddr*)&Addr_;
        }

        virtual socklen_t Len() const {
            return sizeof(Addr_);
        }

    private:
        TIpAddress Addr_;
    };
}

MODULE(pinger) {
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , PingActivator_(Executor)
        , Log_(Control->DefaultLog())
        , Request_("GET / HTTP/1.0")
        , Min_(TDuration::Seconds(0.01))
        , Max_(TDuration::Seconds(0.10))
    {
        Config->ForEach(this);

        if (!Arbiter_) {
            ythrow TConfigParseError() << "no arbiter configured";
        }

        if (!S_) {
            ythrow TConfigParseError() << "no module configured";
        }

        if (Max_ < Min_) {
            ythrow TConfigParseError() << "max delay < min delay";
        }

        Executor->Create<TModule, &TModule::Pinger>(this, "pinger");
    }

    START_PARSE {
        ON_KEY ("request", Request_) {
            return;
        }

        ON_KEY ("mindelay", Min_) {
            return;
        }

        ON_KEY ("maxdelay", Max_) {
            return;
        }

        {
            Stroka log;

            ON_KEY("log", log) {
                Log_ = Control->Log(log);

                return;
            }
        }

        {
            TAutoPtr<IReAskArbiter> arb = ConstructArbiter(key, Copy(value->AsSubConfig()));

            if (!!arb) {
                Arbiter_.Reset(arb.Release());

                return;
            }
        }

        {
            S_.Reset(Loader->MustLoad(key, Copy(value->AsSubConfig())).Release());

            return;
        }
    } END_PARSE

    virtual void DoStats(TOutputStream& out) {
        Arbiter_->PrintStats(out);
        S_->PrintStats(out);
    }

    virtual void DoRun(const TConnDescr& descr) {
        try {
            S_->Run(descr);
            Arbiter_->RegisterSucc();
        } catch (...) {
            Arbiter_->RegisterFail();

            if (!Arbiter_->Authorize()) {
                PingActivator_.Signal();
            }

            throw;
        }
    }

    inline void Pinger(TCont* c) {
        //TODO - proper shutdown
        do {
            PingActivator_.Wait();

            if (!Control->ListenerStarted()) {
                break;
            }

            *Log_ << "stop listener, run pinger" << Endl;

            Control->StopListener();

            TDuration delay = NextDelay(TDuration::Max(), 1.0);

            while (!Arbiter_->Authorize()) {
                c->SleepT(delay);

                if (Ping(c)) {
                    delay = NextDelay(delay, 0.5);
                    Arbiter_->RegisterSucc();
                } else {
                    delay = NextDelay(delay, 2.0);
                    Arbiter_->RegisterFail();
                }
            }

            Control->StartListener();
            *Log_ << "stop pinger, run listener" << Endl;
        } while (true);
    }

    inline bool Ping(TCont* c) {
        try {
            TRequest req(Request_);
            TCountOutput countOut(&TNullStream::Instance());
            TRandomAddr rndAddr;

            TConnDescr::TProperties props = {
                  c
                , &rndAddr
                , &rndAddr
                , TInstant::Now()
            };

            TConnDescr descr = {
                  &TNullStream::Instance()
                , &countOut
                , &req
                , RandomNumber<TRequestHash>()
                , &props
            };

            S_->Run(descr);

            return (bool)countOut.Written();
        } catch (...) {
        }

        return false;
    }

    inline TDuration NextDelay(const TDuration& delay, float mult) {
        return Min(Max(delay * mult, Min_), Max_);
    }

private:
    THolder<IModule> S_;
    THolder<IReAskArbiter> Arbiter_;
    TMyEvent PingActivator_;
    TSystemLog* Log_;
    Stroka Request_;
    TDuration Min_;
    TDuration Max_;
};

IModuleHandle* NModPinger::Handle() {
    return TModule::ModuleHandle();
}
