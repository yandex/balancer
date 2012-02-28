#include "null.h"
#include "iobase.h"
#include "stream.h"
#include "rewind.h"
#include "concat.h"
#include "precharge.h"

#include <util/generic/singleton.h>

using namespace NSrvKernel;

namespace {
    class TStd: public IIoOutput {
    public:
        inline TStd(TOutputStream* out)
            : Out_(out)
        {
        }

        virtual void DoSend(TChunkList& lst) {
            *Out_ << lst;
        }

    private:
        TOutputStream* Out_;
    };

    class TStdOut: public TStd {
    public:
        inline TStdOut()
            : TStd(&Cout)
        {
        }
    };

    class TStdErr: public TStd {
    public:
        inline TStdErr()
            : TStd(&Cerr)
        {
        }
    };
}

IIoOutput& NSrvKernel::StdOut() {
    return *Singleton<TStdOut>();
}

IIoOutput& NSrvKernel::StdErr() {
    return *Singleton<TStdErr>();
}

void IIoInput::ReadAll(TChunkList& lst) {
    while (true) {
        TChunkList tmp;

        Recv(tmp);

        if (tmp.Empty()) {
            return;
        }

        MoveTo(&tmp, &lst);
    }
}
