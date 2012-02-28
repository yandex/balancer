#pragma once

#include "backend.h"

class TNullLogBackend: public TLogBackend {
    public:
        TNullLogBackend();
        virtual ~TNullLogBackend() throw ();

        virtual void WriteData(const TLogRecord& rec);
        virtual void ReopenLog();
};
