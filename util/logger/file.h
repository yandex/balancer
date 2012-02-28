#pragma once

#include "backend.h"

#include <util/generic/ptr.h>

class Stroka;

class TFileLogBackend: public TLogBackend {
    public:
        TFileLogBackend(const Stroka& path);
        virtual ~TFileLogBackend() throw ();

        virtual void WriteData(const TLogRecord& rec);
        virtual void ReopenLog();

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};
