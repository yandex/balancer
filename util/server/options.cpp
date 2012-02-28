#include "options.h"

#include <util/network/ip.h>
#include <util/network/socket.h>
#include <util/generic/yexception.h>
#include <util/digest/numeric.h>
#include <util/generic/hash_set.h>

void THttpServerOptions::AddBindAddress(const Stroka& address, ui16 port) {
    if (address.empty() || address == STRINGBUF("*")) {
        m_BindSockaddr.push_back(TIpAddress(HostToInet(INADDR_ANY), port));
    } else {
        m_BindSockaddr.push_back(TIpAddress(address, port));
    }
}

namespace {
    struct TIpAddrEqual {
        inline bool operator() (const TIpAddress& l, const TIpAddress& r) const throw () {
            return l.sin_port == r.sin_port && l.sin_addr.s_addr == r.sin_addr.s_addr;
        }
    };

    struct TIpAddrHash {
        inline size_t operator() (const TIpAddress& l) const throw () {
            return CombineHashes<size_t>(l.sin_port, l.sin_addr.s_addr);
        }
    };
}

void THttpServerOptions::BindAddresses(TBindAddresses& ret) const {
    const ui16 defport = HostToInet(Port);
    yhash_set<TIpAddress, TIpAddrHash, TIpAddrEqual> used;

    for (size_t i = 0; i < +m_BindSockaddr; ++i) {
        TIpAddress addr(m_BindSockaddr[i]);

        if (!addr.sin_port) {
            addr.sin_port = defport;
        }

        if (used.find(addr) == used.end()) {
            ret.push_back(addr);
            used.insert(addr);
        }
    }

    if (ret.empty()) {
        ret.resize(1);

        TIpAddress& addr = ret[0];

        addr.sin_port = defport;
        addr.sin_addr.s_addr = HostToInet(INADDR_ANY);
    }
}
