#pragma once

#include "output.h"

#include <util/memory/tempbuf.h>

class TTempBufOutput
    : public TOutputStream
    , public TTempBuf
{
public:
    TTempBufOutput()
    {}
    explicit TTempBufOutput(size_t size)
        : TTempBuf(size)
    {}

public:
    virtual void DoWrite(const void* data, size_t len) {
        Append(data, len);
    }
};
