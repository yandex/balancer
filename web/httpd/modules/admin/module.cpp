#include "module.h"

#include <web/httpd/kernel/http.h>
#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/config.h>

#include <library/svnversion/svnversion.h>

using namespace NSrvKernel;

namespace {
    #define MACHINE_DATA
    #include "fsm.cpp"
}

MODULE(admin) {
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , Version_(GetProgramSvnVersion())
    {
    }

    virtual void DoRun(const TConnDescr& descr) {
        TChunkList lst;
        TBufferOutput buf;

        lst.Push(STRINGBUF("HTTP/1.1 200 Ok\r\nContent-Type: application/xhtml+xml\r\n\r\n"));

        #define MACHINE_CODE
        #include "fsm.cpp"

        descr.Output->Send(lst);
    }

private:
    const TStringBuf Version_;
};

IModuleHandle* NModAdmin::Handle() {
    return TModule::ModuleHandle();
}
