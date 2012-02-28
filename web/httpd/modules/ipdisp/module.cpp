#include "module.h"

#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/config.h>

#include <util/digest/crc.h>
#include <util/digest/numeric.h>

#include <util/generic/ptr.h>
#include <util/generic/hash.h>
#include <util/generic/stroka.h>

using namespace NSrvKernel;
using namespace NModIpDispatch;

namespace {
    struct TDescr {
        inline TDescr(ui64 host, ui16 port) throw ()
            : Host(host)
            , Port(port)
        {
        }

        static inline TDescr MatchAll() throw () {
            return TDescr(0, 0);
        }

        static inline TDescr MatchPort(ui16 port) throw () {
            return TDescr(0, port);
        }

        static inline TDescr MatchHost(ui64 host) throw () {
            return TDescr(host, 0);
        }

        inline TDescr(const IRemoteAddr& addr) throw ()
            : Host(0)
            , Port(0)
        {
            const sockaddr* sa = addr.Addr();

            switch (sa->sa_family) {
                case AF_INET: {
                    const TIpAddress a(*(const sockaddr_in*)sa);

                    Host = a.Host();
                    Port = a.Port();

                    break;
                }

                case AF_INET6: {
                    const sockaddr_in6* a = (const sockaddr_in6*)sa;

                    Host = Crc<ui64>(&a->sin6_addr.s6_addr, sizeof(a->sin6_addr.s6_addr));
                    Port = InetToHost(a->sin6_port);

                    break;
                }
            }
        }

        ui64 Host;
        ui16 Port;
    };

    struct TDescrOps {
        inline bool operator() (const TDescr& l, const TDescr& r) const throw () {
            return (l.Host == r.Host) && (l.Port == r.Port);
        }

        inline size_t operator() (const TDescr& d) const throw () {
            return CombineHashes<size_t>(d.Port, d.Host);
        }
    };

    typedef yvector<TDescr> TDescrs;
}

MODULE(ipdispatch) {
    class TElem: public IConfig::IFunc, public TModuleParams {
    public:
        inline TElem(const Stroka& name, const TModuleParams& mp)
            : TModuleParams(mp)
            , Name_(name)
            , Port_(0)
        {
            Config->ForEach(this);

            if (!Module_) {
                ythrow TConfigParseError() << "no module configured";
            }
        }

        START_PARSE {
            ON_KEY("ip", Host_) {
                return;
            }

            ON_KEY("port", Port_) {
                return;
            }

            {
                Module_.Reset(Loader->MustLoad(key, Copy(value->AsSubConfig())).Release());

                return;
            }
        } END_PARSE

        inline TDescrs Descriptors() const throw () {
            TDescrs ret;

            if (Host_ == "*" || Host_.empty()) {
                if (Port_) {
                    ret.push_back(TDescr::MatchPort(Port_));
                } else {
                    ret.push_back(TDescr::MatchAll());
                }
            } else {
                TNetworkAddress addr(Host_, Port_);

                for (TNetworkAddress::TIterator it = addr.Begin(); it != addr.End(); ++it) {
                    ret.push_back(TDescr(TAddrInfo(&*it)));
                }
            }

            return ret;
        }

        inline IModule* Module() const throw () {
            return Module_.Get();
        }

        inline void PrintStats(TOutputStream& out) const {
            out << "<" << Name_ << ">";
            //TODO
            //Control->GetAcceptedStatistics(out, TIpAddress(Ip_, Port_));
            Module_->PrintStats(out);
            out << "</" << Name_ << ">";
        }

    private:
        const Stroka Name_;
        Stroka Host_;
        ui16 Port_;
        TAutoPtr<IModule> Module_;
    };

    typedef TSharedPtr<TElem> TElemRef;

public:
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
    {
        Config->ForEach(this);

        if (Modules_.empty()) {
            ythrow TConfigParseError() << "no modules configured";
        }
    }

    START_PARSE {
        AddModule(key, value->AsSubConfig());

        return;
    } END_PARSE

private:
    inline void AddModule(const Stroka& key, IConfig* cfg) {
        TElemRef elem(new TElem(key, Copy(cfg)));
        TDescrs descrs = elem->Descriptors();

        for (TDescrs::const_iterator it = descrs.begin(); it != descrs.end(); ++it) {
            Modules_[*it] = elem;
        }
    }

    virtual void DoRun(const TConnDescr& descr) {
        const TDescr d(*descr.LocalAddr());
        TModules::const_iterator it = Modules_.find(d);

        if (it == Modules_.end()) {
            it = Modules_.find(TDescr::MatchPort(d.Port));
        }

        // backward configs compability
        if (it == Modules_.end()) {
            it = Modules_.find(TDescr::MatchHost(d.Host));
        }

        if (it == Modules_.end()) {
            it = Modules_.find(TDescr::MatchAll());
        }

        if (it == Modules_.end()) {
            return;
        }

        it->second->Module()->Run(descr);
    }

    virtual void DoStats(TOutputStream& out) {
        for (TModules::const_iterator it = Modules_.begin(); it != Modules_.end(); ++it) {
            it->second->PrintStats(out);
        }
    }

private:
    typedef yhash<TDescr, TElemRef, TDescrOps, TDescrOps> TModules;
    TModules Modules_;
};

IModuleHandle* NModIpDispatch::Handle() {
    return TModule::ModuleHandle();
}
