#include "log.h"
#include "main.h"
#include "http.h"
#include "iface.h"
#include "format.h"
#include "chunks.h"
#include "config.h"
#include "luacfg.h"
#include "socket.h"
#include "headers.h"
#include "address.h"

#include <library/svnversion/svnversion.h>

#include <util/stream/file.h>
#include <util/server/listen.h>
#include <util/memory/smallobj.h>
#include <util/memory/mmapalloc.h>
#include <util/generic/ptr.h>
#include <util/generic/hash.h>
#include <util/ycoroutine.h>

using namespace NSrvKernel;

namespace {
    typedef TContListener::ICallBack::TAcceptFull TAcceptFull;
    typedef TConnDescr::TProperties TProperties;

    class TMainTask: public TContListener::ICallBack, public IConfig::IFunc, public ICtl, public TMemoryPool::IGrowPolicy {
        class TConnection: public TSmallObj, public TIntrusiveListItem<TConnection> {
            struct TPrioritySend: public IIoOutput {
                inline TPrioritySend(TConnection* parent, IIoOutput* slave)
                    : Parent(parent)
                    , Slave(slave)
                {
                }

                virtual void DoSend(TChunkList& lst) {
                    Slave->Send(lst);
                    Parent->OnSend();
                }

                TConnection* Parent;
                IIoOutput* Slave;
            };

        public:
            inline TConnection(TMainTask* parent, const TAcceptFull& a)
                : Parent_(parent)
                , S_(a.S->Release())
                , RemoteAddress_(a.Remote)
                , LocalAddress_(a.Local)
            {
                Parent_->Executor_->Create(*this, "connection");
            }

            inline void operator() (TCont* c) {
                THolder<TConnection> This(this);

                TProperties props = {
                      c
                    , &RemoteAddress_
                    , &LocalAddress_
                    , TInstant::Now()
                };

                Run(&props);
            }

            inline void Cancel() throw () {
                S_.Close();
            }

            inline void Run(TProperties* props) {
                TSocketIo io(&S_, props->Cont->Executor());
                TPrioritySend sender(this, &io);

                const TConnDescr connDescr = {
                      &io
                    , &sender
                    , 0
                    , 0
                    , props
                };

                Parent_->Module_->Run(connDescr);
                io.Flush();
            }

            inline void OnSend() throw () {
                Parent_->UpPriority(this);
            }

        private:
            TMainTask* Parent_;
            TSocketHolder S_;
            TAddrCopy RemoteAddress_;
            TAddrCopy LocalAddress_;
        };

        struct TAddrs: public yvector<TNetworkAddress>, public IConfig::IFunc {
            class TAddr : public IConfig::IFunc {
            public:
                inline TAddr(IConfig* config)
                    : Port_(80)
                {
                    config->ForEach(this);
                }

                START_PARSE {
                    ON_KEY("ip", Host_) {
                        return;
                    }

                    ON_KEY("port", Port_) {
                        return;
                    };
                } END_PARSE

                inline TNetworkAddress ToNetworkAddr() const {
                    if (Host_ == "*" || !Host_) {
                        return TNetworkAddress(Port_);
                    }

                    return TNetworkAddress(Host_, Port_);
                }

            private:
                Stroka Host_;
                ui16 Port_;
            };

            START_PARSE {
                push_back(TAddr(value->AsSubConfig()).ToNetworkAddr());

                return;
            } END_PARSE
        };

        struct TOwnExecutor {
            inline TOwnExecutor(TMemoryPool* pool)
                : PoolAlloc_(pool)
                , StackAllocator_(&PoolAlloc_, 80000)
                , ContRepPool_(&StackAllocator_)
                , Executor_(&ContRepPool_)
            {
            }

            TMemoryPoolAllocator PoolAlloc_;
            TGenericContStackAllocator StackAllocator_;
            TContRepPool ContRepPool_;
            TContExecutor Executor_;
        };

    public:
        inline TMainTask(TInputStream& cfgin, TContExecutor* executor, IModuleFactory* loader)
            : MaxConnections_(0)
            , Canceled_(0)
            , DeferAccept_(true)
            , Loader_(loader)
            , Log_(DefaultLog())
            , Pool_(1024 * 1024, this, MmapAllocator())
            , Executor_(executor)
        {
            if (!Executor_) {
                OwnExecutor_.Reset(new TOwnExecutor(&Pool_));
                Executor_ = &OwnExecutor_->Executor_;
            }

            InitHeadersParser();

            Config_.Reset(LuaConfigParser(cfgin).Release());
            Config_->ForEach(this);

            if (!Module_) {
                ythrow TConfigParseError() << "no modules configured";
            }

            if (ListenAddrs_.empty()) {
                ListenAddrs_.push_back(TNetworkAddress(80));
            }

            RestartListener();
        }

        virtual size_t Next(size_t prev) const throw () {
            return (size_t)(prev * 1.62);
        }

        START_PARSE {
            {
                Stroka log;

                ON_KEY("log", log) {
                    Log_ = Log(log);

                    return;
                }
            }

            if (key == "addrs") {
                value->AsSubConfig()->ForEach(&ListenAddrs_);

                return;
            }

            {
                ui16 port = 0;

                ON_KEY("port", port) {
                    ListenAddrs_.push_back(TNetworkAddress(port));

                    return;
                }
            }

            ON_KEY("maxconn", MaxConnections_) {
                return;
            }

            ON_KEY("deferaccept", DeferAccept_) {
                return;
            }

            {
                const TModuleParams mp = {
                      &Pool_
                    , value->AsSubConfig()
                    , Loader_
                    , this
                    , Executor_
                };

                Module_.Reset(Loader_->MustLoad(key, mp).Release());

                return;
            }
        } END_PARSE

        inline void Execute() {
            *Log_ << "server started" << Endl;
            Executor_->Execute();
            *Log_ << "server stopped" << Endl;
        }

        virtual void DoReopenLog() {
            for (TLogs::iterator it = Logs_.begin(); it != Logs_.end(); ++it) {
                it->second->ReopenLog();
            }
        }

        virtual TSystemLog* DoLog(const Stroka& name) {
            TLogRef& log = Logs_[name];

            if (!log) {
                log = new TSystemLog(name);
            }

            return log.Get();
        }

        virtual void DoShutDown() throw () {
            Listener_.Destroy();

            const TInstant deadLine = Now() + TDuration::Seconds(10);

            while (!Connections_.Empty()) {
                Executor_->Running()->ContPtr()->SleepT(TDuration::Seconds(0.01));

                if (Now() > deadLine) {
                    break;
                }
            }

            Executor_->Abort();
        }

        virtual bool DoListenerStarted() throw() {
            return !!Listener_;
        }

        virtual void DoPrintStats(TOutputStream& o) {
            o << "<?xml version=\"1.0\"?>";
            o << "<xml>";
            o << "<canceled>" << Canceled_ << "</canceled>";
            Module_->PrintStats(o);
            o << "</xml>";
        }

    private:
        virtual void DoStopListener() {
            Listener_.Reset(new TContListener(this, Executor_, TContListener::TOptions().SetDeferAccept(DeferAccept_)));
        }

        virtual void DoStartListener() {
            for (TAddrs::const_iterator it = ListenAddrs_.begin(); it != ListenAddrs_.end(); ++it) {
                Listener_->Bind(*it);
            }

            Listener_->Listen();
        }

        virtual void DoGetListenAddrs(TOutputStream& out) const {
            out << "<addrs>";
            for (TAddrs::const_iterator it = ListenAddrs_.begin(); it != ListenAddrs_.end(); ++it) {
                for (TNetworkAddress::TIterator addr = it->Begin(); addr != it->End(); ++addr) {
                    out << "<addr>" << FormatHost(TAddrInfo(&*addr)) << "</addr>";
                }
            }
            out << "</addrs>";
        }

        virtual void DoGetListenPorts(TOutputStream&) const {
            //TODO
        }

        virtual void OnAcceptFull(const TAcceptFull& acc) {
            //TODO
            //++Accepted_[*acc.Local];

            if (MaxConnections_) {
                //TODO - optimize

                while (Connections_.Size() > MaxConnections_) {
                    Connections_.PopFront()->Cancel();
                    ++Canceled_;
                }
            }

            Connections_.PushBack(new TConnection(this, acc));
        }

        inline void UpPriority(TConnection* conn) throw () {
            Connections_.PushBack(conn);
        }

        virtual void OnError() {
            *Log_ << "on error: " << CurrentExceptionMessage() << Endl;
        }

    private:
        typedef TSharedPtr<TSystemLog> TLogRef;
        typedef yhash<Stroka, TLogRef> TLogs;

        TLogs Logs_;
        TAddrs ListenAddrs_;
        size_t MaxConnections_;
        ui64 Canceled_;
        bool DeferAccept_;
        IModuleFactory* Loader_;
        TSystemLog* Log_;
        THolder<IConfig> Config_;
        TMemoryPool Pool_;
        THolder<TOwnExecutor> OwnExecutor_;
        TContExecutor* Executor_;
        THolder<TContListener> Listener_;
        TIntrusiveList<TConnection> Connections_;
        THolder<IModule> Module_;
    };
}

#include <util/datetime/cputimer.h>

namespace NSrvKernel {
    void RunMain(int argc, char** argv, IModuleFactory* loader) {
        if (argc < 2) {
            Cerr << PROGRAM_VERSION << Endl;

            ythrow TConfigParseError() << "insufficient params(" << argc << ")";
        }

        TBufferedFileInput fi(argv[1]);

        RunMain(fi, loader);
    }

    void RunMain(TInputStream& cfg, IModuleFactory* loader) {
        RunMain(cfg, 0, loader);
    }

    void RunMain(TInputStream& cfg, TContExecutor* executor, IModuleFactory* loader) {
        TMainTask(cfg, executor, loader).Execute();
    }
}
