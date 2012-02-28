#include "module.h"

#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/config.h>

#include <util/generic/ptr.h>
#include <util/generic/stroka.h>
#include <util/generic/hash_set.h>

using namespace NSrvKernel;

namespace {
    struct TSharedModule: public TRefCounted<TSharedModule> {
        struct TStore {
            typedef yhash_set<TSharedModule*> TSet;

            inline void Register(TSharedModule* s) {
                Set.insert(s);
            }

            inline void UnRegister(TSharedModule* s) throw () {
                Set.erase(s);
            }

            inline TSharedModule* Find(const Stroka& name) const {
                for (TSet::const_iterator it = Set.begin(); it != Set.end(); ++it) {
                    if ((*it)->Name == name) {
                        return const_cast<TSharedModule*>(*it);
                    }
                }

                ythrow yexception() << "no such module " << name.Quote();
            }

            static inline TStore* Instance() {
                return Singleton<TStore>();
            }

            TSet Set;
        };

        inline TSharedModule(TAutoPtr<IModule> slave, const Stroka& name)
            : Slave(slave)
            , Name(name)
        {
            TStore::Instance()->Register(this);
        }

        inline ~TSharedModule() throw () {
            TStore::Instance()->UnRegister(this);
        }

        TAutoPtr<IModule> Slave;
        Stroka Name;
    };

    typedef TIntrusivePtr<TSharedModule> TSharedModuleRef;
}

MODULE(shared) {
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , HasRealConfig_(false)
    {
        Config->ForEach(this);

        if (!Name_) {
            ythrow TConfigParseError() << "no name";
        }

        if (!!M_) {
            S_ = new TSharedModule(M_, Name_);
            HasRealConfig_ = true;
        }
    }

    START_PARSE {
        ON_KEY("uuid", Name_) {
            return;
        }

        {
            M_.Reset(Loader->MustLoad(key, Copy(value->AsSubConfig())).Release());

            return;
        }
    } END_PARSE

    virtual void DoRun(const TConnDescr& descr) {
        Slave()->Run(descr);
    }

    virtual void DoStats(TOutputStream& out) {
        out << "<uuid>" << Name_ << "</uuid>";

        if (HasRealConfig_) {
            Slave()->PrintStats(out);
        }
    }

private:
    inline IModule* Slave() {
        if (!S_) {
            S_ = TSharedModule::TStore::Instance()->Find(Name_);
        }

        return S_->Slave.Get();
    }

private:
    TAutoPtr<IModule> M_;
    Stroka Name_;
    TSharedModuleRef S_;
    bool HasRealConfig_;
};

IModuleHandle* NModShared::Handle() {
    return TModule::ModuleHandle();
}
