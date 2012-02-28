#include "stream.h"
#include "record.h"

#include <util/stream/output.h>

TStreamLogBackend::TStreamLogBackend(TOutputStream* slave)
    : Slave_(slave)
{
}

TStreamLogBackend::~TStreamLogBackend() throw () {
}

void TStreamLogBackend::WriteData(const TLogRecord& rec) {
    Slave_->Write(rec.Data, rec.Len);
}

void TStreamLogBackend::ReopenLog() {
}
