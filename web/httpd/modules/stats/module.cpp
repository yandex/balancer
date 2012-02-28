#include "module.h"

#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/config.h>
#include <web/httpd/kernel/errors.h>
#include <web/httpd/kernel/rewind.h>

#include <util/string/cast.h>
#include <util/generic/hash.h>

using namespace NSrvKernel;
using namespace NModStatistics;

MODULE(statistics) {
    class TTimeStatElem {
    public:
        TTimeStatElem(const Stroka& name, TDuration t)
            : Name_(name)
            , T_(t)
            , C_(0)
        {
        }

        void AddStat(TDuration t) {
            if (t <= T_) {
                ++C_;
            }
        }

        void PrintStats(TOutputStream& out) const {
            out << "<" << Name_ << ">" << C_ << "</" << Name_ << ">";
        }
    private:
        const Stroka Name_;
        const TDuration T_;
        ui64 C_;
    };

    class TTimeStats : public yvector<TTimeStatElem>, public IConfig::IFunc {
    public:
        START_PARSE {
            push_back(TTimeStatElem(key, FromString<TDuration>(value->AsString())));
            return;
        } END_PARSE

        void AddStat(TDuration t) {
            for (TTimeStats::iterator it = begin(); it != end(); ++it) {
                it->AddStat(t);
            }
        }

        void PrintStats(TOutputStream& out) const {
            out << "<timestats>";
            for (TTimeStats::const_iterator it = begin(); it != end(); ++it) {
                it->PrintStats(out);
            }
            out << "</timestats>";
        }
    };

public:
    template <class T>
    struct TCnt {
        inline TCnt(T& v) throw ()
            : V(v)
        {
            ++V;
        }

        inline ~TCnt() throw () {
            --V;
        }

        T& V;
    };

    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , UnknownErrors_(0)
        , Success_(0)
        , InProgr_(0)
        , Log_(0)
    {
        Config->ForEach(this);

        if (!Module_) {
            ythrow TConfigParseError() << "no module configured";
        }
    }

    START_PARSE {
        Stroka log;

        ON_KEY("log", log) {
            Log_ = Control->Log(log);

            return;
        }

        if (key == "timestats") {
            value->AsSubConfig()->ForEach(&TimeStats_);

            return;
        }

        {
            Module_ = Loader->MustLoad(key, Copy(value->AsSubConfig()));

            return;
        }
    } END_PARSE

private:
    virtual void DoRun(const TConnDescr& descr) {
        TCnt<ui64> cnt(InProgr_);

        TRewindAbleInput tt(descr.Input);

        try {
            TInstant start = Now();

            if (Log_) {
                TConnDescr copy_descr = descr.Copy(&tt);
                Module_->Run(copy_descr);
            } else {
                Module_->Run(descr);
            }
            ++Success_;

            TimeStats_.AddStat(Now() - start);
        } catch (const THttpError& err) {
            ++HttpErrors_[err.Code()];

            if (Log_) {
                *Log_ << "Got httpError " << err.Code() << Endl << tt.Tmp() << Endl;
            }
            throw;
        } catch (const TSystemError& err) {
            ++SystemErrors_[err.Status()];

            if (Log_) {
                *Log_ << "Got systemError " << err.Status() << Endl << tt.Tmp() << Endl;
            }
            throw;
        } catch (...) {
            ++UnknownErrors_;

            if (Log_) {
                *Log_ << "Got unknownError " << Endl << tt.Tmp() << Endl;
            }
            throw;
        }
    }

    virtual void DoStats(TOutputStream& out) {
        out << "<inprogr>" << InProgr_ << "</inprogr>";

        if (!HttpErrors_.empty()) {
            out << "<http>";

            for (THttpErrors::const_iterator it = HttpErrors_.begin(); it != HttpErrors_.end(); ++it) {
                out << "<error_" << it->first << ">" << it->second << "</error_" << it->first << ">";
            }

            out << "</http>";
        }

        if (!SystemErrors_.empty()) {
            out << "<system>";

            for (TSystemErrors::const_iterator it = SystemErrors_.begin(); it != SystemErrors_.end(); ++it) {
                const Stroka& name = TranslateSysErrCode(it->first);

                out << "<error_" << name << ">" << it->second << "</error_" << name << ">";
            }

            out << "</system>";
        }

        if (UnknownErrors_) {
            out << "<unknown>" << UnknownErrors_ << "</unknown>";
        }

        out << "<success>" << Success_ << "</success>";

        if (TimeStats_.size()) {
            TimeStats_.PrintStats(out);
        }

        Module_->PrintStats(out);
    }

private:
    inline const Stroka& TranslateSysErrCode(int code) {
        Stroka& ret = ErrCodes_[code];

        if (!ret) {
            ret = TranslateSysErrCodeHard(code);
        }

        return ret;
    }

    static inline Stroka TranslateSysErrCodeHard(int code) {
        if (code == EPIPE) {
            return "pipe";
        }

        if (code == ETIMEDOUT) {
            return "timedout";
        }

        if (code == ECONNRESET) {
            return "connreset";
        }

        return ToString(code);
    }

private:
    TAutoPtr<IModule> Module_;
    typedef yhash<int, ui64> TSystemErrors;
    TSystemErrors SystemErrors_;
    typedef yhash<ui16, ui64> THttpErrors;
    THttpErrors HttpErrors_;
    ui64 UnknownErrors_;
    ui64 Success_;
    ui64 InProgr_;
    yhash<int, Stroka> ErrCodes_;
    TSystemLog* Log_;
    TTimeStats TimeStats_;
};

IModuleHandle* NModStatistics::Handle() {
    return TModule::ModuleHandle();
}
