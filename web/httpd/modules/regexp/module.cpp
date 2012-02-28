#include "module.h"

#include <web/httpd/kernel/http.h>
#include <web/httpd/kernel/errors.h>
#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/config.h>
#include <web/httpd/kernel/rewind.h>
#include <web/httpd/kernel/concat.h>
#include <web/httpd/kernel/regexp.h>
#include <web/httpd/kernel/precharge.h>

#include <util/generic/ptr.h>
#include <util/generic/vector.h>

#include "optvalue.h"

using namespace NSrvKernel;
using namespace NModRegexp;

MODULE(regexp) {
    class TItem: public TRefCounted<TItem>, public TModuleParams, public IConfig::IFunc {
    public:
        inline TItem(const Stroka& name, const TModuleParams& mp)
            : TModuleParams(mp)
            , Name_(name)
            , CaseInsensitive_(OV_DEFAULT)
            , Surround_(OV_DEFAULT)
            , Priority_(1.0)
        {
            if (IsDefault()) {
                Regexp_ = ".*";
            }

            Config->ForEach(this);

            if (!Slave_) {
                ythrow TConfigParseError() << "no module configured";
            }

            if (!Regexp_) {
                ythrow TConfigParseError() << "no regexp configured";
            }

            if (IsDefault()) {
                Priority_ = -1.0;
            }
        }

        inline bool IsDefault() const throw () {
            return Name_ == "default";
        }

        inline float Priority() const throw () {
            return Priority_;
        }

        START_PARSE {
            ON_KEY ("priority", Priority_) {
                return;
            }

            ON_KEY("match", Regexp_) {
                return;
            }

            if (key == "case_insensitive") {
                CaseInsensitive_ = OptionValue(value->AsBool());

                return;
            }

            if (key == "surround") {
                Surround_ = OptionValue(value->AsBool());

                return;
            }

            {
                Slave_.Reset(Loader->MustLoad(key, Copy(value->AsSubConfig())).Release());

                return;
            }
        } END_PARSE

        inline const TFsm& Fsm() const throw () {
            return *Fsm_;
        }

        inline IModule* Slave() const throw () {
            return Slave_.Get();
        }

        inline void PrintStats(TOutputStream& out) {
            out << "<" << Name_ << ">";
            Slave_->PrintStats(out);
            out << "</" << Name_ << ">";
        }

        inline void InitFsm(bool defaultCaseInsensitive, bool defaultSurround) {
            TFsm::TOptions options;
            options.SetCaseInsensitive(CalcOptionValue(CaseInsensitive_, defaultCaseInsensitive));
            options.SetSurround(CalcOptionValue(Surround_, defaultSurround));
            Fsm_.Reset(new TFsm(Regexp_, options));
        }

    private:
        const Stroka Name_;
        Stroka Regexp_;
        EOptionValue CaseInsensitive_;
        EOptionValue Surround_;
        THolder<TFsm> Fsm_;
        THolder<IModule> Slave_;
        float Priority_;
    };

    typedef TIntrusivePtr<TItem> TItemRef;

    struct THdrChooser {
        THdrChooser(const TFsm* fsm)
            : R(fsm)
            , Header(0)
        {
        }

        inline void operator() (THeader* hdr) {
            if (Header) {
                return;
            }

            TMatcher m(*R);

            if (Match(m, hdr->Key).Final()) {
                Header = hdr;
            }
        }

        const TFsm* R;
        THeader* Header;
    };

public:
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , Fsm_(TFsm::False())
        , CaseInsensitive_(false)
        , Surround_(false)
        , Default_(0)
    {
        Config->ForEach(this);

        if (Items_.empty()) {
            ythrow TConfigParseError() << "no modules configured";
        }

        InitFsm();
    }

    START_PARSE {
        Stroka header;

        ON_KEY("http_header", header) {
            FsmHeader_.Reset(new TFsm(header, TFsm::TOptions().SetCaseInsensitive(true)));
            return;
        }

        if (key == "case_insensitive") {
            CaseInsensitive_ = value->AsBool();

            return;
        }

        if (key == "surround") {
            Surround_ = value->AsBool();

            return;
        }

        {
            Items_.push_back(new TItem(key, Copy(value->AsSubConfig())));

            return;
        }
    } END_PARSE

    virtual void DoRun(const TConnDescr& descr) {
        IModule* slave = NULL;

        if (!FsmHeader_) {
            slave = SelectHandle(descr.Request->Request());
        } else {
            THdrChooser chooser(FsmHeader_.Get());

            descr.Request->ForEachHeader(chooser);

            if (chooser.Header) {
                slave = SelectHandle(chooser.Header->Value);
            } else {
                TChunkList tmp;

                slave = SelectHandle(tmp);
            }
        }

        if (!slave) {
            ythrow THttpError(404) << "no module for request handling";
        }

        slave->Run(descr);
    }

    inline IModule* SelectHandle(const TChunkList& data) {
        TMatcher m(Fsm_);

        if (Match(m, data).Final()) {
            TMatcher::TMatchedRegexps mr = m.MatchedRegexps();
            const TItem* item = 0;

            for (const size_t* numIt = mr.first; numIt != mr.second; ++numIt) {
                const size_t num = *numIt;

                if (num < Items_.size()) {
                    const TItem* next = Items_[num].Get();

                    if (!item || (next->Priority() > item->Priority())) {
                        item = next;
                    }
                }
            }

            if (item) {
                return item->Slave();
            }
        }

        return Default();
    }

    inline IModule* Default() const throw () {
        if (Default_) {
            return Default_->Slave();
        }

        return 0;
    }

    virtual void DoStats(TOutputStream& out) {
        for (TItems::const_iterator it = Items_.begin(); it != Items_.end(); ++it) {
            (*it)->PrintStats(out);
        }
    }

private:
    inline void InitFsm() {
        for (TItems::iterator pItem = Items_.begin(); pItem != Items_.end(); ++pItem) {
            const TItemRef& item = *pItem;

            if (item->IsDefault()) {
                Default_ = item.Get();
            }

            item->InitFsm(CaseInsensitive_, Surround_);

            if (pItem == Items_.begin()) {
                Fsm_ = item->Fsm();
            } else {
                Fsm_ = Fsm_ | item->Fsm();
            }
        }
    }

private:
    typedef yvector<TItemRef> TItems;
    TItems Items_;
    TFsm Fsm_;
    THolder<TFsm> FsmHeader_;
    bool CaseInsensitive_;
    bool Surround_;
    TItem* Default_;
};

IModuleHandle* NModRegexp::Handle() {
    return TModule::ModuleHandle();
}
