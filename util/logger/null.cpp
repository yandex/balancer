#include "null.h"

TNullLogBackend::TNullLogBackend() {
}

TNullLogBackend::~TNullLogBackend() throw () {
}

void TNullLogBackend::WriteData(const TLogRecord&) {
}

void TNullLogBackend::ReopenLog() {
}
