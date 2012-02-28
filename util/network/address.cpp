#include "address.h"

#if defined(_unix_)
    #include <sys/types.h>
    #include <sys/un.h>
#endif

using namespace NAddr;

template <bool printPort>
static inline void PrintAddr(TOutputStream& out, const IRemoteAddr& addr) {
    const sockaddr* a = addr.Addr();
    char buf[INET6_ADDRSTRLEN + 10];

    switch (a->sa_family) {
        case AF_INET: {
            const TIpAddress sa(*(const sockaddr_in*)a);

            out << IpToString(sa.Host(), buf, sizeof(buf));

            if (printPort) {
                out << ":" << sa.Port();
            }

            break;
        }

        case AF_INET6: {
            const sockaddr_in6* sa = (const sockaddr_in6*)a;

            if (!inet_ntop(AF_INET6, (void*)&sa->sin6_addr.s6_addr, buf, sizeof(buf))) {
                ythrow TSystemError() << "inet_ntop() failed";
            }

            out << buf;

            if (printPort) {
                out << ":" << InetToHost(sa->sin6_port);
            }

            break;
        }

#if defined(AF_LOCAL)
        case AF_LOCAL: {
            const sockaddr_un* sa = (const sockaddr_un*)a;

            out << TStringBuf(sa->sun_path);

            break;
        }
#endif

        default: {
            const char* b = (const char*)a;
            const char* e = b + addr.Len();

            out << "(raw " << a->sa_family << " ";

            while (b != e) {
                //just print raw bytes
                out << (int)*b++ << " ";
            }

            out << ")";

            break;
        }
    }
}

template <>
void Out<IRemoteAddr>(TOutputStream& out, const IRemoteAddr& addr) {
    PrintAddr<true>(out, addr);
}

void NAddr::PrintHost(TOutputStream& out, const IRemoteAddr& addr) {
    PrintAddr<false>(out, addr);
}

IRemoteAddrRef NAddr::GetSockAddr(SOCKET s) {
    TAutoPtr<TOpaqueAddr> addr(new TOpaqueAddr());

    if (getsockname(s, addr->Addr(), addr->LenPtr()) < 0) {
        ythrow TSystemError()<< "getsockname() failed";
    }

    return addr.Release();
}
