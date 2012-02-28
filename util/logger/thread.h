#pragma once

#include "backend.h"

#include <util/generic/ptr.h>

class TThreadedLogBackend: public TLogBackend {
    public:
        TThreadedLogBackend(TLogBackend* slave);
        TThreadedLogBackend(TLogBackend* slave, size_t queuelen);
        virtual ~TThreadedLogBackend() throw ();

        virtual void WriteData(const TLogRecord& rec);
        virtual void ReopenLog();

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};
