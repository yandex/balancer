#include "log.h"

#include <util/system/thread.h>
#include <util/stream/ios.h>
#include <util/logger/stream.h>
#include <util/datetime/base.h>

using namespace NSrvKernel;

namespace {
    class TBufEnd: public TStreamLogBackend, public TBufferOutput {
        public:
            inline TBufEnd(TBuffer& buf)
                : TStreamLogBackend(this)
                , TBufferOutput(buf)
            {
            }
    };
}

TSystemLog::TSystemLog()
    : TLog(new TBufEnd(Buffer_))
{
}

void TSystemLog::Open(const Stroka& path) {
    *static_cast<TLog*>(this) = TLog(path);
    Write(~Buffer_, +Buffer_);
    TBuffer().Swap(Buffer_);
}

void TSystemLog::LogPrefix(TLogElement& s) {
    s << "[" << TThread::CurrentThreadId() << "-" << MicroSeconds() << "] ";
}
