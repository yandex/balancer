#include "file.h"
#include "record.h"

#include <util/system/file.h>
#include <util/system/rwlock.h>
#include <util/generic/stroka.h>

/*
 * file log
 */
class TFileLogBackend::TImpl: public TFile {
    public:
        inline TImpl(const Stroka& path)
            : TFile(OpenFile(path))
        {
        }

        inline void WriteData(const TLogRecord& rec) {
            //many writes are thread-safe
            TReadGuard guard(Lock_);

            Write(rec.Data, rec.Len);
        }

        inline void ReopenLog() {
            //but log rotate not thread-safe
            TWriteGuard guard(Lock_);

            LinkTo(OpenFile(GetName()));
        }

    private:
        static inline TFile OpenFile(const Stroka& path) {
            return TFile(path, OpenAlways | WrOnly | ForAppend | Seq);
        }

    private:
        TRWMutex Lock_;
};

TFileLogBackend::TFileLogBackend(const Stroka& path)
    : Impl_(new TImpl(path))
{
}

TFileLogBackend::~TFileLogBackend() throw () {
}

void TFileLogBackend::WriteData(const TLogRecord& rec) {
    Impl_->WriteData(rec);
}

void TFileLogBackend::ReopenLog() {
    Impl_->ReopenLog();
}
