#pragma once

#include "address.h"

namespace NSrvKernel {
    struct THostFormat {
        inline THostFormat(const IRemoteAddr* addr) throw ()
            : Addr(addr)
        {
        }

        const IRemoteAddr* Addr;
    };

    inline THostFormat FormatHost(const IRemoteAddr& addr) throw () {
        return THostFormat(&addr);
    }

    inline THostFormat FormatHost(const IRemoteAddr* addr) throw () {
        return THostFormat(addr);
    }
}
