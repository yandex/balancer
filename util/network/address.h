#pragma once

#include "ip.h"
#include "socket.h"

namespace NAddr {
    class IRemoteAddr {
    public:
        virtual ~IRemoteAddr() {
        }

        virtual sockaddr* Addr() const = 0;
        virtual socklen_t Len() const = 0;
    };

    typedef TAutoPtr<IRemoteAddr> IRemoteAddrRef;

    IRemoteAddrRef GetSockAddr(SOCKET s);
    void PrintHost(TOutputStream& out, const IRemoteAddr& addr);

    //for accept, recvfrom - see LenPtr()
    class TOpaqueAddr: public IRemoteAddr {
    public:
        inline TOpaqueAddr()
            : L_(sizeof(S_))
        {
            Zero(S_);
        }

        virtual sockaddr* Addr() const {
            return (sockaddr*)&S_;
        }

        virtual socklen_t Len() const {
            return L_;
        }

        inline socklen_t* LenPtr() {
            return &L_;
        }

    private:
        sockaddr_storage S_;
        socklen_t L_;
    };

    //for TNetworkAddress
    class TAddrInfo: public IRemoteAddr {
    public:
        inline TAddrInfo(const addrinfo* ai)
            : AI_(ai)
        {
        }

        virtual sockaddr* Addr() const {
            return AI_->ai_addr;
        }

        virtual socklen_t Len() const {
            return (socklen_t)AI_->ai_addrlen;
        }

    private:
        const addrinfo* AI_;
    };

    //compat, for TIpAddress
    class TIPv4Addr: public IRemoteAddr {
    public:
        inline TIPv4Addr(const TIpAddress& addr)
            : A_(addr)
        {
        }

        virtual sockaddr* Addr() const {
            return A_;
        }

        virtual socklen_t Len() const {
            return A_;
        }

    private:
        TIpAddress A_;
    };

    //same, for ipv6 addresses
    class TIPv6Addr: public IRemoteAddr {
    public:
        inline TIPv6Addr(const sockaddr_in6& a)
            : A_(a)
        {
        }

        virtual sockaddr* Addr() const {
            return (sockaddr*)&A_;
        }

        virtual socklen_t Len() const {
            return sizeof(A_);
        }

    private:
        sockaddr_in6 A_;
    };
}
