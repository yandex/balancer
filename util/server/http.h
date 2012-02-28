#pragma once

#include "options.h"

#include <util/thread/queue.h>
#include <util/stream/http.h>
#include <util/generic/ptr.h>
#include <util/generic/vector.h>
#include <util/httpreqdata.h>
#include <util/memory/blob.h>

class IThreadPool;
class TClientRequest;
class TClientConnection;

class THttpServer {
        friend class TClientRequest;
        friend class TClientConnection;
    public:
        class ICallBack {
            public:
                struct TFailLogData {
                    int failstate;
                    Stroka url;
                };

                virtual ~ICallBack() {
                }

                virtual void OnFailRequest(int /*failstate*/) {
                }

                virtual void OnFailRequestEx(const TFailLogData& d) {
                    OnFailRequest(d.failstate);
                }

                virtual void OnException() {
                }

                virtual void OnMaxConn() {
                }

                virtual TClientRequest* CreateClient() = 0;

                virtual void OnListenStart() {
                }

                virtual void OnListenStop() {
                }

                virtual void OnWait() {
                }

                virtual void* CreateThreadSpecificResource () {
                    return 0;
                }

                virtual void DestroyThreadSpecificResource(void*) {
                }
        };

        typedef THttpServerOptions TOptions;

        THttpServer(ICallBack* cb, const TOptions& options = TOptions(), IThreadPool* pool = 0);
        virtual ~THttpServer();

        bool Start();

        // shutdown a.s.a.p.
        void Stop();

        // graceful shutdown with serving all already open connection
        void Shutdown();

        void Wait();
        int GetErrorCode();
        const char* GetError();
        void RestartRequestThreads(ui32 nTh, ui32 maxQS);
        const TOptions& Options() const throw ();

        class TImpl;
    private:
        THolder<TImpl> Impl_;
};

class TClientRequest: public IObjectInQueue {
        friend class THttpServer::TImpl;
    public:
        TClientRequest();
        virtual ~TClientRequest();

        inline THttpInput& Input() throw () {
            return *HttpConn_->Input();
        }

        inline THttpOutput& Output() throw () {
            return *HttpConn_->Output();
        }

        THttpServer* HttpServ() const throw ();
        const TSocket& Socket() const throw ();

        bool CheckLoopback();
        void ProcessFailRequest(int failstate);

    private:
        /*
         * Processes the request after 'connection' been created and 'Headers' been read
         * Returns 'false' if the processing must be continued by the next handler,
         * 'true' otherwise ('this' will be deleted)
         */
        virtual bool Reply(void* ThreadSpecificResource);
        virtual void Process(void* ThreadSpecificResource);

    public:
        yvector<Stroka> Headers;

    private:
        THolder<TClientConnection> Conn_;
        THolder<THttpServerConn> HttpConn_;
};

class THttpClientRequestEx: public TClientRequest {
    public:
        inline THttpClientRequestEx()
            : content_length(0)
        {
        }

    protected:
        bool Parse(char* req);
        bool ProcessHeaders();

    protected:
        size_t content_length;
        TBlob Buf;
        TServerRequestData RD;
};
