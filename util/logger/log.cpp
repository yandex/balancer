#include "log.h"
#include "file.h"
#include "stream.h"

#include <util/string/cast.h>
#include <util/stream/printf.h>
#include <util/system/yassert.h>
#include <util/generic/stroka.h>
#include <util/generic/yexception.h>

static inline TAutoPtr<TLogBackend> BackendFactory(const Stroka& path) {
    try {
        return new TFileLogBackend(path);
    } catch (...) {
        Cdbg << "Warning: " << path << ": " << CurrentExceptionMessage() << ". Use stdout instead." << Endl;
    }

    return new TStreamLogBackend(&Cerr);
}

class TLog::TImpl: public TRefCounted<TImpl, TAtomicCounter> {
        class TPriorityLogStream: public TOutputStream {
            public:
                inline TPriorityLogStream(TLogPriority p, TImpl* parent)
                    : Priority_(p)
                    , Parent_(parent)
                {
                }

                virtual void DoWrite(const void* buf, size_t len) {
                    Parent_->WriteData(Priority_, (const char*)buf, len);
                }

            private:
                TLogPriority Priority_;
                TImpl* Parent_;
        };

    public:
        inline TImpl(TAutoPtr<TLogBackend> backend)
            : BackEnd_(backend)
            , DefaultPriority_(LOG_DEF_PRIORITY)
        {
        }

        inline ~TImpl() throw () {
        }

        inline void ReopenLog() {
            if (!IsOpen()) {
                return;
            }

            BackEnd_->ReopenLog();
        }

        inline void AddLog(TLogPriority priority, const char* format, va_list& args) {
            if (!IsOpen()) {
                return;
            }

            TPriorityLogStream ls(priority, this);

            Printf(ls, format, args);
        }

        inline void ResetBackend(TAutoPtr<TLogBackend> backend) {
            BackEnd_.Reset(backend.Release());
        }

        inline bool IsOpen() const throw () {
            return NULL != BackEnd_.Get();
        }

        inline void CloseLog() throw () {
            BackEnd_.Destroy();

            YASSERT(!IsOpen());
        }

        inline void WriteData(TLogPriority priority, const char* data, size_t len) {
            if (IsOpen()) {
                BackEnd_->WriteData(TLogRecord(priority, data, len));
            }
        }

        inline TLogPriority DefaultPriority() throw () {
            return DefaultPriority_;
        }

        inline void SetDefaultPriority(TLogPriority priority) throw () {
            DefaultPriority_ = priority;
        }

    private:
        THolder<TLogBackend> BackEnd_;
        TLogPriority DefaultPriority_;
};

TLog::TLog()
    : Impl_(new TImpl(0))
{
}

TLog::TLog(const Stroka& fname) {
    THolder<TLogBackend> backend(BackendFactory(fname));

    Impl_ = new TImpl(backend);
}

TLog::TLog(TAutoPtr<TLogBackend> backend)
    : Impl_(new TImpl(backend))
{
}

TLog::~TLog() throw () {
}

bool TLog::IsOpen() const throw () {
    return Impl_->IsOpen();
}

void TLog::AddLog(const char* format, ...) {
    va_list args;
    va_start(args, format);

    try {
        Impl_->AddLog(Impl_->DefaultPriority(), format, args);
    } catch (...) {
        va_end(args);

        throw;
    }

    va_end(args);
}

void TLog::AddLog(TLogPriority priority, const char* format, ...) {
    va_list args;
    va_start(args, format);

    try {
        Impl_->AddLog(priority, format, args);
    } catch (...) {
        va_end(args);

        throw;
    }

    va_end(args);
}

void TLog::AddLog(const char* format, va_list& lst) {
    Impl_->AddLog(Impl_->DefaultPriority(), format, lst);
}

void TLog::ReopenLog() {
    Impl_->ReopenLog();
}

void TLog::CloseLog() {
    Impl_->CloseLog();
}

void TLog::SetDefaultPriority(TLogPriority priority) throw () {
    Impl_->SetDefaultPriority(priority);
}

TLogPriority TLog::DefaultPriority() const throw () {
    return Impl_->DefaultPriority();
}

bool TLog::OpenLog(const char* path) {
    if (path) {
        ResetBackend(BackendFactory(path));
    } else {
        ResetBackend(new TStreamLogBackend(&Cerr));
    }

    return true;
}

void TLog::ResetBackend(TAutoPtr<TLogBackend> backend) {
    Impl_->ResetBackend(backend);
}

void TLog::Write(TLogPriority priority, const char* data, size_t len) {
    Impl_->WriteData(priority, data, len);
}

void TLog::Write(const char* data, size_t len) {
    Impl_->WriteData(Impl_->DefaultPriority(), data, len);
}
