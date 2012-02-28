#pragma once

#include <util/network/address.h>
#include <util/stream/mem.h>

using namespace NAddr;

namespace NSrvKernel {
    class TAddrCopy: public IRemoteAddr {
    public:
        inline TAddrCopy(const IRemoteAddr* addr)
            : Len_(addr->Len())
        {
            memcpy(Addr(), addr->Addr(), Len_);
        }

        virtual sockaddr* Addr() const throw () {
            return (sockaddr*)&Buf_;
        }

        virtual socklen_t Len() const throw () {
            return Len_;
        }

    private:
        sockaddr_storage Buf_;
        size_t Len_;
    };

    const size_t HOSTLENBUF = 256;

    static inline TStringBuf FormatHost(const IRemoteAddr* addr, char* buf, size_t len) {
        TMemoryOutput out(buf, len);

        PrintHost(out, *addr);

        return TStringBuf(buf, out.Buf());
    }
}
