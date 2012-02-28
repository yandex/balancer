#pragma once

#include <util/generic/ptr.h>
#include <util/generic/ylimits.h>

struct TIpAddress;
class TContExecutor;
class TSocketHolder;
class TNetworkAddress;

namespace NAddr {
    class IRemoteAddr;
}

class TContListener {
public:
    struct TOptions {
        inline TOptions()
            : ListenQueue(Max<size_t>())
            , EnableDeferAccept(false)
        {
        }

        inline TOptions& SetListenQueue(size_t len) throw () {
            ListenQueue = len;

            return *this;
        }

        inline TOptions& SetDeferAccept(bool enable) throw () {
            EnableDeferAccept = enable;

            return *this;
        }

        size_t ListenQueue;
        bool EnableDeferAccept;
    };

    class ICallBack {
    public:
        struct TAccept {
            TSocketHolder* S;
            const TIpAddress* Remote;
            const TIpAddress* Local;
        };

        struct TAcceptFull {
            TSocketHolder* S;
            const NAddr::IRemoteAddr* Remote;
            const NAddr::IRemoteAddr* Local;
        };

        virtual void OnAccept(const TAccept&) {
        }

        virtual void OnAcceptFull(const TAcceptFull&);

        /*
         * will be called from catch (...) {} context
         * so your can re-throw current exception and work around it
         */
        virtual void OnError() = 0;

        virtual ~ICallBack() {
        }
    };

    TContListener(ICallBack* cb, TContExecutor* e, const TOptions& opts = TOptions());
    ~TContListener() throw ();

    /// start listener threads
    void Listen();
    /// bind server on address. Can be called multiple times to bind on more then one address
    void Bind(const TIpAddress& addr);
    void Bind(const TNetworkAddress& addr);
    void Stop() throw ();

    void StopListenAddr(const TIpAddress& addr);
    void StartListenAddr(const TIpAddress& addr);

private:
    class TImpl;
    THolder<TImpl> Impl_;
};
