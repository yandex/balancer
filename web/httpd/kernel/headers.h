#pragma once

#include <util/system/defaults.h>

namespace NSrvKernel {
    class TRequest;
    //TODO - inherit from common ancestor
    typedef TRequest TResponse;

    struct TResponseProperties {
        TResponseProperties(TResponse* r);

        bool ChunkedTransfer;
        size_t ContentLength;
    };

    struct TRequestProperties {
        TRequestProperties(TRequest* r);

        bool KeepAlive;
        bool ChunkedTransfer;
        size_t ContentLength;
    };

    void InitHeadersParser();
}
