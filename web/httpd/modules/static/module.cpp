#include "queue.h"
#include "module.h"

#include <web/httpd/kernel/http.h>
#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/config.h>

#include <util/string/http.h>

using namespace NSrvKernel;
using namespace NQueue;

MODULE(static) {
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , Index_("index.html")
        , Queue_(Executor)
        , Pool_(TDefaultAllocator::Instance())
    {
        Config->ForEach(this);

        if (!Dir_) {
            ythrow TConfigParseError() << "no dir configured";
        }

        Dir_ += '/';
    }

    START_PARSE {
        ON_KEY("dir", Dir_) {
            return;
        }

        ON_KEY("index", Index_) {
            return;
        }
    } END_PARSE

    virtual void DoRun(const TConnDescr& descr) {
        //TODO - optimize
        Stroka tmp(ToString(descr.Request->Request()));
        TParsedHttpRequest req(tmp);
        Stroka file = ToString(req.Request);

        if (!file || file == "/") {
            file = Index_;
        }

        const Stroka path = Dir_ + file;
        TThreadedFile f(&Queue_, &Pool_, path);
        TChunkList lst;

        lst.Push(STRINGBUF("HTTP/1.1 200 Ok\r\n"));

        if (file.find(".xhtml") != Stroka::npos) {
            lst.Push(STRINGBUF("Content-Type: application/xhtml+xml\r\n"));
        }

        lst.Push(CRLF);

        ui64 off = 0;
        TChunkPtr cur;

        while ((cur = f.Pread(off))->Length()) {
            off += cur->Length();
            lst.Push(cur);
            descr.Output->Send(lst);
            lst.Clear();
        }
    }

  private:
    Stroka Dir_;
    Stroka Index_;
    TThreadedQueue Queue_;
    TThreadedFile::TPool Pool_;
};

IModuleHandle* NModStatic::Handle() {
    return TModule::ModuleHandle();
}
