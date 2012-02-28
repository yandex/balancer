#pragma once

#include <util/generic/noncopyable.h>

struct TLogRecord;

class TLogBackend: public TNonCopyable {
    public:
        TLogBackend() throw ();
        virtual ~TLogBackend() throw ();

        virtual void WriteData(const TLogRecord& rec) = 0;
        virtual void ReopenLog() = 0;
};
