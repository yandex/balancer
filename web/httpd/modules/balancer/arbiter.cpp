#include "average.h"
#include "arbiter.h"

#include <web/httpd/kernel/iface.h>
#include <web/httpd/kernel/config.h>

#include <util/stream/ios.h>
#include <util/generic/utility.h>
#include <util/generic/yexception.h>

namespace NModBalancer {
    class TFakeArbiter: public IReAskArbiter {
        public:
            virtual void RegisterSucc() {
            }

            virtual void RegisterFail () {
            }

            virtual bool Authorize() const {
                return true;
            }

            virtual void DoPrintStats(TOutputStream& out) const {
                out << "<type>fake</type>";
            }
    };

    IReAskArbiter* ConstructFake(const TModuleParams&) {
        return new TFakeArbiter();
    }

    class TAdditiveArbiter: public IReAskArbiter, public IConfig::IFunc, public TModuleParams {
        public:
            virtual void RegisterSucc() {
                FailRate_ = Max<double>(0, FailRate_ - SuccSummand_);
            }

            virtual void RegisterFail () {
                FailRate_ += FailSummand_;
            }

            virtual bool Authorize() const {
                return FailRate_ < AlarmValue_;
            }

            virtual void DoPrintStats(TOutputStream& out) const {
                out << "<type>additive</type>";
                out << "<rate>" << FailRate_ << "</rate>";
            }

        public:
            TAdditiveArbiter(const TModuleParams& mp)
                : TModuleParams(mp)
                , FailRate_(0)
                , SuccSummand_(0.05)
                , FailSummand_(1)
                , AlarmValue_(0)
            {
                Config->ForEach(this);
                AlarmValue_ = 1.0 / SuccSummand_;
            }

            START_PARSE {
                ON_KEY("succinc", SuccSummand_) {
                    return;
                }

                ON_KEY("failinc", FailSummand_) {
                    return;
                }
            } END_PARSE

        private:
            double FailRate_;
            double SuccSummand_;
            double FailSummand_;
            double AlarmValue_;
    };

    IReAskArbiter* ConstructAdditiveArbiter(const TModuleParams& mp) {
        return new TAdditiveArbiter(mp);
    }

    class TWatermarkArbiter: public IReAskArbiter, public IConfig::IFunc, public TModuleParams {
        public:
            virtual void RegisterSucc() {
                FailRate_->Add(0);

                if ((false == Enabled_) && (FailRate_->M() < LoWatermark_)) {
                    Enabled_ = true;
                }
            }

            virtual void RegisterFail() {
                FailRate_->Add(1);

                if ((true == Enabled_) && (FailRate_->M() > HiWatermark_)) {
                    Enabled_ = false;
                }
            }

            virtual bool Authorize() const {
                return Enabled_;
            }

            virtual void DoPrintStats(TOutputStream& out) const {
                out << "<type>watermark</type>";
                out << "<rate>" << FailRate_->M() << "</rate>";
            }

        public:
            TWatermarkArbiter(const TModuleParams& mp)
                : TModuleParams(mp)
                , LoWatermark_(0.025)
                , HiWatermark_(0.05)
                , FailRate_(new TMiddleValueTracker<double>(0, 0.99))
                , Enabled_(true)
            {
                Config->ForEach(this);

                if (LoWatermark_ > HiWatermark_) {
                    ythrow TConfigParseError() << "lo > hi";
                }
            }

            START_PARSE {
                ON_KEY("lo", LoWatermark_) {
                    return;
                }

                ON_KEY("hi", HiWatermark_) {
                    return;
                }
            } END_PARSE

        private:
            double LoWatermark_;
            double HiWatermark_;
            THolder<TMiddleValueTracker<double> > FailRate_;
            bool Enabled_;
    };

    void IReAskArbiter::PrintStats(TOutputStream& out) const {
        out << "<arbiter>";
        DoPrintStats(out);
        out << "</arbiter>";
    }

    IReAskArbiter* ConstructWatermarkArbiter(const TModuleParams& mp) {
        return new TWatermarkArbiter(mp);
    }

    TAutoPtr<IReAskArbiter> ConstructArbiter(const Stroka& name, const TModuleParams& mp) {
        if ("additive" == name) {
            return ConstructAdditiveArbiter(mp);
        }

        if ("watermark" == name) {
            return ConstructWatermarkArbiter(mp);
        }

        if ("fake" == name) {
            return ConstructFake(mp);
        }

        return 0;
    }
}
