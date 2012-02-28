#pragma once

#include <util/network/ip.h>
#include <util/network/init.h>

#include <util/generic/stroka.h>
#include <util/generic/vector.h>

#include <util/datetime/base.h>

class THttpServerOptions {
public:
    inline THttpServerOptions(ui16 port = 17000) throw ()
        : KeepAliveEnabled(true)
        , CompressionEnabled(false)
        , Port(port)
        , Host("localhost")
        , ServerName("YWS/1.0")
        , nThreads(0)
        , MaxQueueSize(0)
        , nFThreads(1)
        , MaxFQueueSize(0)
        , MaxConnections(100)
        , ListenBacklog(SOMAXCONN)
    {
    }

    typedef yvector<TIpAddress> TBindAddresses;
    void BindAddresses(TBindAddresses& ret) const;

    void AddBindAddress(const Stroka& address, ui16 port);

    inline void AddBindAddress(const Stroka& address) {
        AddBindAddress(address, 0);
    }

    inline THttpServerOptions& EnableKeepAlive(bool enable) throw () {
        KeepAliveEnabled = enable;

        return *this;
    }

    inline THttpServerOptions& EnableCompression(bool enable) throw () {
        CompressionEnabled = enable;

        return *this;
    }

    inline THttpServerOptions& SetThreads(ui32 threads) throw () {
        nThreads = threads;

        return *this;
    }

    inline THttpServerOptions& SetPort(ui16 port) throw () {
        Port = port;

        return *this;
    }

    inline THttpServerOptions& SetMaxConnections(ui32 mc = 0) throw () {
        MaxConnections = mc;

        return *this;
    }

    inline THttpServerOptions& SetMaxQueueSize(ui32 mqs = 0) throw () {
        MaxQueueSize = mqs;

        return *this;
    }

    inline THttpServerOptions& SetClientTimeout(const TDuration& timeout) throw () {
        ClientTimeout = timeout;

        return *this;
    }

    inline THttpServerOptions& SetListenBacklog(int val) throw () {
        ListenBacklog = val;

        return *this;
    }

    bool        KeepAliveEnabled;
    bool        CompressionEnabled;
    yvector<sockaddr_in> m_BindSockaddr;
    ui16        Port;               //The port on which to run the web server
    Stroka      Host;               //DNS entry
    const char* ServerName;         //The Web server name to return in HTTP headers
    ui32        nThreads;           //Count of threads for requests processing
    ui32        MaxQueueSize;       //Max allowed request count in queue
    ui32        nFThreads;
    ui32        MaxFQueueSize;
    ui32        MaxConnections;
    int         ListenBacklog;
    TDuration   ClientTimeout;
};
