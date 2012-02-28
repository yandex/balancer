#pragma once

static inline void SetCommonSockOpts(SOCKET sock) {
    SetSockOpt(sock, SOL_SOCKET, SO_REUSEADDR, 1);

    sockaddr_in s_in;
    s_in.sin_family = AF_INET;
    s_in.sin_addr.s_addr = INADDR_ANY;
    s_in.sin_port = 0;

    if (bind(sock, (struct sockaddr*)&s_in, sizeof(s_in)) == -1) {
        warn("bind");
    }

    SetNoDelay(sock, true);
}

class TSocketPool;

class TPooledSocket {
        class TImpl: public TIntrusiveListItem<TImpl>, public TRefCounted<TImpl, TSimpleCounter, TImpl> {
            public:
                inline TImpl(SOCKET fd, TSocketPool* pool) throw ()
                    : Pool_(pool)
                    , IsKeepAlive_(false)
                    , Fd_(fd)
                {
                    Touch();
                }

                static inline void Destroy(TImpl* impl) throw () {
                    impl->DoDestroy();
                }

                inline void DoDestroy() throw () {
                    if (!Closed() && IsKeepAlive() && IsInGoodState()) {
                        ReturnToPool();
                    } else {
                        delete this;
                    }
                }

                inline bool IsKeepAlive() const throw () {
                    return IsKeepAlive_;
                }

                inline void SetKeepAlive(bool ka) {
                    ::SetKeepAlive(Fd_, ka);
                    IsKeepAlive_ = ka;
                }

                inline SOCKET Socket() const throw () {
                    return Fd_;
                }

                inline bool Closed() const throw () {
                    return Fd_.Closed();
                }

                inline void Close() throw () {
                    Fd_.Close();
                }

                inline bool IsInGoodState() const throw () {
                    int err = 0;
                    socklen_t len = sizeof(err);

                    getsockopt(Fd_, SOL_SOCKET, SO_ERROR, (char*)&err, &len);

                    return !err;
                }

                inline bool IsOpen() const throw () {
                    return IsInGoodState() && TCont::SocketNotClosedByOtherSide(Fd_);
                }

                inline void Touch() throw () {
                    TouchTime_ = TInstant::Now();
                }

                inline const TInstant& LastTouch() const throw () {
                    return TouchTime_;
                }

            private:
                inline void ReturnToPool() throw ();

            private:
                TSocketPool* Pool_;
                bool IsKeepAlive_;
                TSocketHolder Fd_;
                TInstant TouchTime_;
        };

        friend class TSocketPool;

    public:
        inline TPooledSocket()
            : Impl_(0)
        {
        }

        inline TPooledSocket(TImpl* impl)
            : Impl_(impl)
        {
        }

        inline ~TPooledSocket() throw () {
            if (UncaughtException() && !!Impl_) {
                Close();
            }
        }

        inline operator SOCKET () const throw () {
            return Impl_->Socket();
        }

        inline void SetKeepAlive(bool ka) {
            Impl_->SetKeepAlive(ka);
        }

        inline void Close() throw () {
            Impl_->Close();
        }

    private:
        TIntrusivePtr<TImpl> Impl_;
};

struct TConnectData {
    inline TConnectData(TCont* cont, const TInstant& deadLine)
        : Cont(cont)
        , DeadLine(deadLine)
    {
    }

    inline TConnectData(TCont* cont, const TDuration& timeOut)
        : Cont(cont)
        , DeadLine(Now() + timeOut)
    {
    }

    TCont* Cont;
    const TInstant DeadLine;
};

class TSocketPool {
        friend class TPooledSocket::TImpl;

    public:
        inline TSocketPool(int ip, int port)
            : Ip_(ip)
            , Port_(port)
        {
        }

        inline void EraseStale(const TInstant& maxAge) throw () {
            TSockets toDelete;

            {
                TGuard<TMutex> guard(Mutex_);

                for (TSockets::TIterator it = Pool_.Begin(); it != Pool_.End();) {
                    if (it->LastTouch() < maxAge) {
                        toDelete.PushBack(&*(it++));
                    } else {
                        ++it;
                    }
                }
            }
        }

        inline TPooledSocket Get(TConnectData* conn) {
            TPooledSocket ret;

            if (TPooledSocket::TImpl* alive = GetImpl()) {
                ret = TPooledSocket(alive);
            } else {
                ret = AllocateMore(conn);
            }

            ret.Impl_->Touch();

            return ret;
        }

        inline bool GetAlive(TPooledSocket& socket) {
            if (TPooledSocket::TImpl* alive = GetImpl()) {
                alive->Touch();
                socket = TPooledSocket(alive);
                return true;
            }
            return false;
        }

    private:
        inline TPooledSocket::TImpl* GetImpl() {
            TGuard<TMutex> guard(Mutex_);

            while (!Pool_.Empty()) {
                THolder<TPooledSocket::TImpl> ret(Pool_.PopFront());

                if (ret->IsOpen()) {
                    return ret.Release();
                }
            }
            return 0;
        }

        inline void Release(TPooledSocket::TImpl* impl) throw () {
            TGuard<TMutex> guard(Mutex_);

            Pool_.PushFront(impl);
        }

        inline TPooledSocket AllocateMore(TConnectData* conn) {
            TCont* cont = conn->Cont;

            sockaddr_in s_in;
            s_in.sin_family = AF_INET;
            s_in.sin_port = htons((u_short)Port_);
            s_in.sin_addr.s_addr = Ip_;

            while (true) {
                TSocketHolder s(cont->Socket(PF_INET, SOCK_STREAM, 0));

                if (s == INVALID_SOCKET) {
                    ythrow TSystemError(errno) << "can not create socket";
                }

                SetCommonSockOpts(s);
                SetZeroLinger(s);

                const int ret = cont->ConnectD(s, (sockaddr*)&s_in, sizeof(s_in), conn->DeadLine);

                if (ret) {
                    ythrow TSystemError(ret) << "can not connect(" << cont->Name() << ")";
                }

                THolder<TPooledSocket::TImpl> res(new TPooledSocket::TImpl(s, this));
                s.Release();

                if (res->IsOpen()) {
                    return res.Release();
                }
            }
        }

    private:
        int Ip_;
        int Port_;
        typedef TIntrusiveListWithAutoDelete<TPooledSocket::TImpl, TDelete> TSockets;
        TSockets Pool_;
        TMutex Mutex_;
};

inline void TPooledSocket::TImpl::ReturnToPool() throw () {
    Pool_->Release(this);
}

class TContIO: public TInputStream, public TOutputStream {
public:
    inline TContIO(SOCKET fd, TCont* cont)
        : Fd_(fd)
        , Cont_(cont)
    {
    }

    virtual void DoWrite(const void* buf, size_t len) {
        Cont_->WriteI(Fd_, buf, len).Checked();
    }

    virtual size_t DoRead(void* buf, size_t len) {
        return Cont_->ReadI(Fd_, buf, len).Checked();
    }

    inline SOCKET Fd() const throw () {
        return Fd_;
    }

private:
    SOCKET Fd_;
    TCont* Cont_;
};
