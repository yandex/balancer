#include "module.h"

#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/config.h>
#include <web/httpd/kernel/errors.h>
#include <web/httpd/kernel/format.h>
#include <web/httpd/kernel/address.h>

using namespace NSrvKernel;
using namespace NModErrorLog;

MODULE(errorlog) {
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , Log_(Control->DefaultLog())
    {
        Config->ForEach(this);

        if (!Module_) {
            ythrow TConfigParseError() << "no module configured";
        }
    }

    START_PARSE {
        ON_KEY("tag", Tag_) {
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
            Module_ = Loader->MustLoad(key, Copy(value->AsSubConfig()));

            return;
        }
    } END_PARSE

private:
    virtual void DoRun(const TConnDescr& descr) {
        try {
            Module_->Run(descr);
        } catch (...) {
            *Log_ << Tag_
                  << STRINGBUF("(")
                  << FormatHost(descr.RemoteAddr())
                  << STRINGBUF(") ")
                  << CurrentExceptionMessage()
                  << Endl;

            throw;
        }
    }

    virtual void DoStats(TOutputStream& out) {
        Module_->PrintStats(out);
    }

private:
    TAutoPtr<IModule> Module_;
    Stroka Tag_;
    TSystemLog* Log_;
};

IModuleHandle* NModErrorLog::Handle() {
    return TModule::ModuleHandle();
}
