#include "proxy.h"

#include <web/httpd/kernel/iobase.h>
#include <web/httpd/kernel/errors.h>
#include <web/httpd/kernel/syncro.h>

#include <util/ycoroutine.h>

using namespace NSrvKernel;

namespace {
    class TWorker {
    public:
        inline TWorker(TProxy* p, IIoInput* in, IIoOutput* out)
            : P_(p)
            , I_(in)
            , O_(out)
            , E_(p->E)
        {
        }

        inline void Start() {
            P_->E->Create(*this, "cont");
        }

        inline void operator() (TCont* c) {
            try {
                Run(c);
            } catch (...) {
            }

            E_.Signal();
        }

        inline void Run(TCont* c) {
            while (true) {
                TChunkList lst;

                I_->Recv(lst);

                if (lst.Empty()) {
                    return;
                }

                O_->Send(lst);
            }
        }

        inline void Wait() {
            E_.Wait();
        }

    private:
        TProxy* P_;
        IIoInput* I_;
        IIoOutput* O_;
        TMyEvent E_;
    };
}

void TProxy::Run() {
    TWorker f(this, CI, BO);
    TWorker t(this, BI, CO);

    f.Start();
    t.Run(E->Running()->ContPtr());
    f.Wait();
}
