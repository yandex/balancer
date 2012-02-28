#pragma once

#include "defaults.h"

class TOutputStream;

size_t BackTrace(void** p, size_t len);

struct TResolvedSymbol {
    const char* Name;
    void* NearestSymbol;
};

TResolvedSymbol ResolveSymbol(void* sym, char* buf, size_t len);

void FormatBackTrace(TOutputStream* out);
void PrintBackTrace();
