#include "module.h"

#include <web/httpd/kernel/http.h>
#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/config.h>

using namespace NSrvKernel;

namespace {
    class TDumpInput: public IIoInput {
    public:
        inline TDumpInput(IIoInput* s)
            : S_(s)
        {
        }

        virtual void DoRecv(TChunkList& lst) {
            TChunkList nlst;
            S_->Recv(nlst);
            MoveTo(&nlst, &lst);
        }

    private:
        IIoInput* S_;
    };

    class TDumpOutput: public IIoOutput {
    public:
        inline TDumpOutput(IIoOutput* s)
            : S_(s)
        {
        }

        virtual void DoSend(TChunkList& lst) {
            Cerr << lst;
            S_->Send(lst);
        }

    private:
        IIoOutput* S_;
    };
}

MODULE(debug) {
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
    {
        Config->ForEach(this);
    }

    START_PARSE {
        ON_KEY("response", Response_) {
            return;
        }

        ON_KEY("delay", Delay_) {
            return;
        }

        {
            S_.Reset(Loader->MustLoad(key, Copy(value->AsSubConfig())).Release());

            return;
        }
    } END_PARSE

    virtual void DoRun(const TConnDescr& descr) {
        if (Delay_.GetValue()) {
            descr.Properties->Cont->SleepT(Delay_);
        }

        if (!!Response_) {
            TChunkList lst;

            lst.Push(STRINGBUF("HTTP/1.1 200 Ok\r\n\r\n"));
            lst.Push(Response_);

            descr.Output->Send(lst);
        } else if (!!S_) {
            TDumpInput dbgi(descr.Input);
            TDumpOutput dbgo(descr.Output);

            S_->Run(descr.Copy(&dbgi, &dbgo));
        }
    }

    virtual void DoStats(TOutputStream& out) {
        S_->PrintStats(out);
    }

private:
    Stroka Response_;
    TDuration Delay_;
    THolder<IModule> S_;
};

IModuleHandle* NModDebug::Handle() {
    return TModule::ModuleHandle();
}
