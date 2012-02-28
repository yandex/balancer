#include <sys/stat.h>
#include <sys/types.h>

#include <ctime>
#include <cerrno>
#include <cstring>

#include <util/system/compat.h> // stricmp, strnicmp, strlwr, strupr, stpcpy
#include <util/system/defaults.h>

#if defined _mt_

#include <util/system/event.h>
#include <util/system/atomic.h>
#include <util/system/thread.h>
#include <util/system/mutex.h>
#include <util/thread/pool.h>
#include <util/network/poller.h>
#include <util/generic/intrlist.h>
#include <util/generic/yexception.h>

#include "http.h"

class IPollAble {
    public:
        inline IPollAble() throw () {}
        virtual ~IPollAble() {}

        virtual void OnPollEvent() = 0;
};

typedef TMtpQueueBinder<TSimpleMtpQueue, THttpServer::ICallBack> THttpServerMtpQueue;

/*
 * -------------------------------------------------------
 *
 *      THttpServer    Implementation
 *
 * ------------------------------------------------------
 */

class TClientConnection: public IPollAble, public TIntrusiveListItem<TClientConnection> {
    public:
        TClientConnection(const TSocket& s, THttpServer::TImpl* serv);
        virtual ~TClientConnection() throw ();

        virtual void OnPollEvent();

        inline void Activate() throw ();
        inline void DeActivate();

    public:
        TSocket Socket_;
        THttpServer::TImpl* HttpServ_;
};

class THttpServer::TImpl {
    public:
        class TConnections {
            public:
                inline TConnections(TSocketPoller* poller)
                    : Poller_(poller)
                {
                }

                inline ~TConnections() throw () {
                }

                inline void Add(TClientConnection* c) throw () {
                    TGuard<TMutex> g(Mutex_);

                    Conns_.PushBack(c);
                    Poller_->WaitRead(c->Socket_, (void*)static_cast<const IPollAble*>(c));
                }

                inline void Erase(TClientConnection* c) throw () {
                    TGuard<TMutex> g(Mutex_);

                    Poller_->Unwait(c->Socket_);
                    c->Unlink();
                }

                inline void Clear() throw () {
                    TGuard<TMutex> g(Mutex_);

                    Conns_.Clear();
                }

                inline void RemoveOld() throw () {
                    TGuard<TMutex> g(Mutex_);

                    if (Conns_.Empty()) {
                        return;
                    }
                    TClientConnection* c = &*(Conns_.Begin());
                    c->Activate();
                    delete c;
                }

            public:
                TMutex Mutex_;
                TIntrusiveListWithAutoDelete<TClientConnection, TDelete> Conns_;
                TSocketPoller *Poller_;
        };

        static void* ListenSocketFunction(void* param) {
            ((TImpl*)param)->ListenSocket();

            return 0;
        }

        TAutoPtr<TClientRequest> CreateRequest(TAutoPtr<TClientConnection> c) {
            THolder<TClientRequest> obj(Cb_->CreateClient());

            obj->Conn_.Reset(c.Release());

            return obj;
        }

        void AddRequestFromSocket(const TSocket& s) {
            if (options.MaxConnections && ((size_t)cur_cons >= options.MaxConnections)) {
                Cb_->OnMaxConn();
                conns->RemoveOld();
            }

            (new TClientConnection(s, this))->DeActivate();
        }

        void save_errno() {
            error_no = WSAGetLastError();
        }

        int GetErrorCode() {
            return error_no;
        }

        const char* GetError() {
            return LastSystemErrorText(error_no);
        }

        bool Start() {
            poller.Reset(new TSocketPoller());
            conns.Reset(new TConnections(poller.Get()));

            //Start the listener thread
            bListenerRunningOK = false;
            bRequestListenStop = false;

            ListenStartEvent.Reset();
            try {
                ListenThread.Start();
            } catch (const yexception&) {
                save_errno();
                return false;
            }

            //Wait until the thread has completely started and return the success indicator
            ListenStartEvent.Wait();

            return bListenerRunningOK;
        }

        void Wait() {
            Cb_->OnWait();
            ListenThread.Join();
        }

        void Stop() {
            bRequestListenStop = true;
            ListenThread.Join();

            while (cur_cons) {
                usleep(10000);
                conns->Clear();
            }

            conns.Destroy();
            poller.Destroy();
        }

        void Shutdown() {
            bRequestListenStop = true;
        }

        void AddRequest(TAutoPtr<TClientRequest> req) {
            struct TFailRequest: public TClientRequest {
                    inline TFailRequest(TAutoPtr<TClientRequest> parent) {
                        Conn_.Reset(parent->Conn_.Release());
                        HttpConn_.Reset(parent->HttpConn_.Release());
                    }

                    virtual bool Reply(void*) {
                        ProcessFailRequest(0);

                        return true;
                    }
            };

            if (Requests->Add(req.Get())) {
                req.Release();
            } else {
                req = new TFailRequest(req);

                if (FailRequests->Add(req.Get())) {
                    req.Release();
                } else {
                    Cb_->OnFailRequest(-1);
                }
            }
        }

        class TListenSocket: public IPollAble, public TIntrusiveListItem<TListenSocket> {
            public:
                inline TListenSocket(const TSocket& s, TImpl* parent)
                    : S_(s)
                    , Server_(parent)
                {
                }

                virtual ~TListenSocket() throw () {
                }

                virtual void OnPollEvent() {
                    SOCKET s = ::accept(S_, NULL, NULL);

                    if (s == INVALID_SOCKET) {
                        ythrow yexception() << "accept: " << LastSystemErrorText();
                    }

                    Server_->AddRequestFromSocket(s);
                }

            private:
                TSocket S_;
                TImpl* Server_;
        };

        void ListenSocket() {
            error_no = 0;

            THttpServerOptions::TBindAddresses addrs;
            options.BindAddresses(addrs);

            TIntrusiveListWithAutoDelete<TListenSocket, TDelete> Reqs;

            for (unsigned i = 0; i < addrs.size(); ++i) {
                TIpAddress& addr = addrs[i];

                addr.sin_family = AF_INET;

                TSocket Socket(::socket(AF_INET, SOCK_STREAM, 0));

                if (Socket == INVALID_SOCKET) {
                    save_errno();
                    ListenStartEvent.Signal();

                    return;
                }

                int yes = 1;
                ::setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));

                if (::bind(Socket, addr, addr) == SOCKET_ERROR) {
                    save_errno();
                    ListenStartEvent.Signal();

                    return;
                }

                if (::listen(Socket, options.ListenBacklog) == SOCKET_ERROR) {
                    save_errno();
                    ListenStartEvent.Signal();

                    return;
                }

                THolder<TListenSocket> ls(new TListenSocket(Socket, this));
                poller->WaitRead(Socket, static_cast<IPollAble*>(ls.Get()));
                Reqs.PushBack(ls.Release());
            }

            Requests->Start(options.nThreads, options.MaxQueueSize);
            FailRequests->Start(options.nFThreads, options.MaxFQueueSize);
            Cb_->OnListenStart();
            bListenerRunningOK = true;
            ListenStartEvent.Signal();

            yvector<void*> events;
            events.resize(1);

            while (!bRequestListenStop) {
                try {
                    const size_t ret = poller->WaitT(~events, +events, TDuration::Seconds(1));

                    for (size_t i = 0; i < ret; ++i) {
                        ((IPollAble*)events[i])->OnPollEvent();
                    }

                    if (!options.MaxConnections) {
                        if (ret >= +events) {
                            events.resize(ret * 2);
                        }
                    }
                } catch (...) {
                    Cb_->OnException();
                }
            }

            Requests->Stop();
            FailRequests->Stop();
            Cb_->OnListenStop();
        }

        void RestartRequestThreads(ui32 nTh, ui32 maxQS) {
            Requests->Stop();
            options.nThreads = nTh;
            options.MaxQueueSize = maxQS;
            Requests->Start(options.nThreads, options.MaxQueueSize);
        }

        TImpl(THttpServer* parent, ICallBack* cb, const TOptions& options_, IThreadPool* pool)
            : ListenThread(ListenSocketFunction, this)
            , Requests(new THttpServerMtpQueue(cb, pool ? pool : SystemThreadPool()))
            , FailRequests(new TMtpQueue(pool ? pool : SystemThreadPool()))
            , options(options_)
            , Cb_(cb)
            , Parent_(parent)
        {
            cur_cons = 0;
            bListenerRunningOK = false;
            bRequestListenStop = false;
            error_no = 0;
        }

        ~TImpl() {
            try {
                Stop();
            } catch (...) {
            }
        }

        inline const TOptions& Options() const throw () {
            return options;
        }

        inline void DecreaseConnections() throw () {
            AtomicDecrement(cur_cons);
        }

        inline void IncreaseConnections() throw () {
            AtomicIncrement(cur_cons);
        }

        TThread ListenThread;
        Event ListenStartEvent;
        THolder<IMtpQueue> Requests;
        THolder<IMtpQueue> FailRequests;
        TAtomic cur_cons;
        THolder<TSocketPoller> poller;
        THolder<TConnections> conns;
        bool bRequestListenStop;
        bool bListenerRunningOK;
        int error_no;
        TOptions options;
        ICallBack* Cb_;
        THttpServer* Parent_;
};

THttpServer::THttpServer(ICallBack* cb, const TOptions& options, IThreadPool* pool)
    : Impl_(new TImpl(this, cb, options, pool))
{
}

THttpServer::~THttpServer() {
}

bool THttpServer::Start() {
    return Impl_->Start();
}

void THttpServer::Stop() {
    Impl_->Stop();
}

void THttpServer::Shutdown() {
    Impl_->Shutdown();
}

void THttpServer::Wait() {
    Impl_->Wait();
}

int THttpServer::GetErrorCode() {
    return Impl_->GetErrorCode();
}

const char* THttpServer::GetError() {
    return Impl_->GetError();
}

void THttpServer::RestartRequestThreads(ui32 n, ui32 queue) {
    Impl_->RestartRequestThreads(n, queue);
}

const THttpServer::TOptions& THttpServer::Options() const throw () {
    return Impl_->Options();
}

TClientConnection::TClientConnection(const TSocket& s, THttpServer::TImpl* serv)
    : Socket_(s)
    , HttpServ_(serv)
{
    SetNoDelay(Socket_, true);

    const TDuration& clientTimeout = HttpServ_->Options().ClientTimeout;
    if (clientTimeout != TDuration::Zero()) {
        SetSocketTimeout(Socket_, (long)clientTimeout.Seconds(), clientTimeout.MilliSecondsOfSecond());
    }

    HttpServ_->IncreaseConnections();
}

TClientConnection::~TClientConnection() throw () {
    HttpServ_->DecreaseConnections();
}

void TClientConnection::OnPollEvent() {
    THolder<TClientConnection> This(this);
    Activate();

    {
        char tmp[1];

        if (::recv(Socket_, tmp, 1, MSG_PEEK) < 1) {
            /*
             * We can received a FIN so our socket was moved to
             * TCP_CLOSE_WAIT state. Check it before adding work
             * for this socket.
             */

            return;
        }
    }

    THolder<TClientRequest> obj(HttpServ_->CreateRequest(This));

    HttpServ_->AddRequest(obj);
}

void TClientConnection::Activate() throw () {
    HttpServ_->conns->Erase(this);
}

void TClientConnection::DeActivate() {
    HttpServ_->conns->Add(this);
}

bool THttpClientRequestEx::Parse(char* req) {
    RD.SetSocket(Socket());

    if (!RD.Parse(req)) {
        Output() <<
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 39\r\n"
            "\r\n"
            "The server cannot be used as a proxy.\r\n";

        return false;
    }

    return true;
}

bool THttpClientRequestEx::ProcessHeaders() {
    for (size_t k = 1; k < Headers.size(); ++k) {
        RD.AddHeader(Headers[k].begin());
    }

    char* s = Headers[0].begin();

    if (strnicmp(s, "GET ", 4) == 0) {
        if (!Parse(s + 4)) {
            return false;
        }
    } else if (strnicmp(s, "POST ", 5) == 0) {
        /*
         * TODO: Why we don't cech a high boud of input?
         */
        try {
            Buf = TBlob::FromStream(Input());
        } catch (...) {
            Output() << "HTTP/1.1 400 Bad request\r\n\r\n";

            return false;
        }

        if (!Parse(s + 5)) {
            return false;
        }
    } else {
        Output() << "HTTP/1.1 501 Not Implemented\r\n\r\n";

        return false;
    }

    return true;
}

TClientRequest::TClientRequest() {
}

TClientRequest::~TClientRequest() {
}

bool TClientRequest::Reply(void* /*ThreadSpecificResource*/) {
    if (strnicmp(~Headers[0], "GET ", 4)) {
        Output() << "HTTP/1.0 501 Not Implemented\r\n\r\n";
    } else {
        Output() <<
            "HTTP/1.0 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "Hello World!\r\n";
    }

    return true;
}

bool TClientRequest::CheckLoopback() {
    struct sockaddr_in s_in;
    socklen_t slen = sizeof(s_in);
    struct sockaddr_in s_in_l;

    if (getpeername(Socket(), (sockaddr*)&s_in, &slen) != 0 ||
        getsockname(Socket(), (sockaddr*)&s_in_l, &slen) != 0) {
        Output() << "HTTP/1.0 500 Oops\r\n\r\ngetpeername\r\n";

        return false;
    } else if (s_in.sin_addr.s_addr != htonl(INADDR_LOOPBACK) &&
        s_in.sin_addr.s_addr != s_in_l.sin_addr.s_addr) {

        Output() << "HTTP/1.0 403 Permission denied\r\n"
            "Content-Type: text/html; charset=windows-1251\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<html><head><title>Permission denied</title></head>"
            "<body><h1>Permission denied</h1>"
            "<p>This request must be sent from the loopback (127.0.0.1) address.</p>"
            "</body></html>\r\n";

        return false;
    }

    return true;
}

void TClientRequest::Process(void* ThreadSpecificResource) {
    THolder<TClientRequest> This(this);

    try {
        if (!HttpConn_) {
            HttpConn_.Reset(new THttpServerConn((Socket())));

            HttpConn_->Output()->EnableKeepAlive(HttpServ()->Options().KeepAliveEnabled);
            HttpConn_->Output()->EnableCompression(HttpServ()->Options().CompressionEnabled);
        }

        if (Headers.empty()) {
            Headers.push_back(Input().FirstLine());

            const THttpHeaders& h = Input().Headers();

            for (THttpHeaders::TConstIterator it = h.Begin(); it != h.End(); ++it) {
                Headers.push_back(it->ToString());
            }
        }

        if (Reply(ThreadSpecificResource)) {
            if (HttpServ()->Options().KeepAliveEnabled && HttpConn_->CanBeKeepAlive()) {
                Output().Finish();
                Conn_->DeActivate();
                Conn_.Release();
            }

            /*
             * *this will be destroyed...
             */

            return;
        }
    } catch (...) {
        Conn_->HttpServ_->Cb_->OnException();

        throw;
    }

    This.Release();
}

void TClientRequest::ProcessFailRequest(int failstate) {
    Output() <<
        "HTTP/1.1 503 Service Unavailable\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 21\r\n"
        "\r\n"
        "Service Unavailable\r\n";

    Stroka url;

    if (!Headers.empty() && !strnicmp(~Headers[0], "GET ", 4)) {
        // Trying to extract url...
        const char* str = ~Headers[0];

        // Skipping spaces before url...
        size_t start = 3;
        while (str[start] == ' ' && str[start]) {
            ++start;
        }

        if (str[start]) {
            // Traversing url...
            size_t idx = start;

            while (str[idx] != ' ' && str[idx]) {
                ++idx;
            }

            url = Headers[0].substr(start, idx - start);
        }
    }

    const THttpServer::ICallBack::TFailLogData d = {
        failstate,
        url
    };

    // Handling failure...
    Conn_->HttpServ_->Cb_->OnFailRequestEx(d);
    Output().Flush();
}

THttpServer* TClientRequest::HttpServ() const throw () {
    return Conn_->HttpServ_->Parent_;
}

const TSocket& TClientRequest::Socket() const throw () {
    return Conn_->Socket_;
}

#endif
