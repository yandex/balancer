#include "listen.h"

#include <util/network/ip.h>
#include <util/network/address.h>
#include <util/generic/ylimits.h>
#include <util/generic/intrlist.h>
#include <util/ycoroutine.h>

using namespace NAddr;

class TContListener::TImpl {
    struct TStoredAddrInfo: public TAddrInfo, public TNetworkAddress {
        inline TStoredAddrInfo(const struct addrinfo* ai, const TNetworkAddress& addr)
            : TAddrInfo(ai)
            , TNetworkAddress(addr)
        {
        }
    };

    class TOneSocketListener: public TIntrusiveListItem<TOneSocketListener> {
    public:
        inline TOneSocketListener(TImpl* parent, IRemoteAddrRef addr)
            : Parent_(parent)
            , C_(0)
            , ListenSocket_(socket(addr->Addr()->sa_family, SOCK_STREAM, 0))
            , Done_(false)
            , Addr_(addr)
        {
            if (ListenSocket_ == INVALID_SOCKET) {
                ythrow TSystemError() << "can not create socket";
            }

            FixIPv6ListenSocket(ListenSocket_);
            CheckedSetSockOpt(ListenSocket_, SOL_SOCKET, SO_REUSEADDR, 1, "reuse addr");
            SetNonBlock(ListenSocket_);

            if (bind(ListenSocket_, Addr_->Addr(), Addr_->Len()) < 0) {
                ythrow TSystemError() << "bind failed";
            }
        }

        inline ~TOneSocketListener() throw () {
            Stop();
        }

        inline void operator() (TCont* c) {
            C_ = c;

            try {
                Run();
            } catch (...) {
                try {
                    Parent_->Cb_->OnError();
                } catch (...) {
                }
            }

            C_ = 0;
        }

        inline void StartListen() {
            if (!C_) {
                const TOptions& opts = Parent_->Opts_;

                if (listen(ListenSocket_, (int)Min<size_t>(Max<int>(), opts.ListenQueue)) < 0) {
                    ythrow TSystemError() << "listen failed";
                }

                if (opts.EnableDeferAccept) {
                    SetDeferAccept(ListenSocket_);
                }

                TContRep* rep = Parent_->E_->Create(*this, "listen_job");
                C_ = rep->ContPtr();
            }
        }

        inline const IRemoteAddr* Addr() const {
            return Addr_.Get();
        }

        inline void Stop() throw () {
            if (C_) {
                Done_ = true;
                C_->Cancel();

                while (C_) {
                    Parent_->E_->Running()->ContPtr()->Yield();
                }
            }
        }

    private:
        inline void Run() {
            while (!Done_) {
                try {
                    TOpaqueAddr remote;
                    int res = C_->AcceptI(ListenSocket_, remote.Addr(), remote.LenPtr());

                    if (res < 0) {
                        const int err = -res;

                        if (err != ECONNABORTED) {
                            if (err == ECANCELED) {
                                break;
                            }

                            ythrow TSystemError(err) << "can not accept";
                        }
                    } else {
                        TSocketHolder c((SOCKET)res);

                        const ICallBack::TAcceptFull acc = {
                              &c
                            , &remote
                            , Addr()
                        };

                        Parent_->Cb_->OnAcceptFull(acc);
                    }
                } catch (...) {
                    try {
                        Parent_->Cb_->OnError();
                    } catch (...) {
                    }
                }
            }

            ListenSocket_.ShutDown(SHUT_RDWR);
            ListenSocket_.Close();
        }

    private:
        TImpl* Parent_;
        TCont* C_;
        TSocketHolder ListenSocket_;
        bool Done_;
        IRemoteAddrRef Addr_;
    };

public:
    inline TImpl(ICallBack* cb, TContExecutor* e, const TOptions& opts)
        : E_(e)
        , Cb_(cb)
        , Opts_(opts)
    {
    }

    inline void Listen() {
        for (TListeners::TIterator it = L_.Begin(); it != L_.End(); ++it) {
            it->StartListen();
        }
    }

    inline void Bind(const TIpAddress& addr) {
        L_.PushBack(new TOneSocketListener(this, new TIPv4Addr(addr)));
    }

    inline void Bind(const TNetworkAddress& addr) {
        for (TNetworkAddress::TIterator it = addr.Begin(); it != addr.End(); ++it) {
            L_.PushBack(new TOneSocketListener(this, new TStoredAddrInfo(&*it, addr)));
        }
    }

    inline void StopListenAddr(const TIpAddress& addr) {
        for (TListeners::TIterator it = L_.Begin(); it != L_.End(); ++it) {
            const sockaddr* remote = it->Addr()->Addr();

            if (remote->sa_family == AF_INET) {
                const sockaddr_in* sa = (const sockaddr_in*)remote;

                if (sa->sin_addr.s_addr == addr.sin_addr.s_addr && sa->sin_port == addr.sin_port) {
                    delete &*it;

                    return;
                }
            }
        }
    }

    inline void StartListenAddr(const TIpAddress& addr) {
        Bind(addr);
        Listen();
    }

private:
    TContExecutor* E_;
    ICallBack* Cb_;
    typedef TIntrusiveListWithAutoDelete<TOneSocketListener, TDelete> TListeners;
    TListeners L_;
    const TOptions Opts_;
};

TContListener::TContListener(ICallBack* cb, TContExecutor* e, const TOptions& opts)
    : Impl_(new TImpl(cb, e, opts))
{
}

TContListener::~TContListener() throw () {
}

void TContListener::Bind(const TIpAddress& addr) {
    if (!Impl_) {
        ythrow yexception() << "not running";
    }

    Impl_->Bind(addr);
}

void TContListener::Bind(const TNetworkAddress& addr) {
    if (!Impl_) {
        ythrow yexception() << "not running";
    }

    Impl_->Bind(addr);
}

void TContListener::Listen() {
    if (!Impl_) {
        ythrow yexception() << "not running";
    }

    Impl_->Listen();
}

void TContListener::Stop() throw () {
    Impl_.Destroy();
}

void TContListener::StopListenAddr(const TIpAddress& addr) {
    if (!Impl_) {
        ythrow yexception() << "not running";
    }

    Impl_->StopListenAddr(addr);
}

void TContListener::StartListenAddr(const TIpAddress& addr) {
    if (!Impl_) {
        ythrow yexception() << "not running";
    }

    Impl_->StartListenAddr(addr);
}

static inline TIpAddress Cvt(const sockaddr* sa) throw () {
    return *(struct sockaddr_in*)sa;
}

void TContListener::ICallBack::OnAcceptFull(const TAcceptFull& params) {
    if (params.Local->Addr()->sa_family == AF_INET) {
        const TIpAddress remote(Cvt(params.Remote->Addr()));
        const TIpAddress local(Cvt(params.Local->Addr()));

        const TAccept a = {
              params.S
            , &remote
            , &local
        };

        OnAccept(a);
    } else {
        Cerr << "drop unsupported request. fix your callback, please" << Endl;
    }
}
