#pragma once

#include "backend.h"

class TOutputStream;

class TStreamLogBackend: public TLogBackend {
    public:
        TStreamLogBackend(TOutputStream* slave);
        virtual ~TStreamLogBackend() throw ();

        virtual void WriteData(const TLogRecord& rec);
        virtual void ReopenLog();

    private:
        TOutputStream* Slave_;
};
