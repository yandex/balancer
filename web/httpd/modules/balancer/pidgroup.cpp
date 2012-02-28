#include "average.h"
#include "backend.h"
#include "iterator.h"
#include "pidgroup.h"

#include <web/httpd/kernel/iface.h>
#include <web/httpd/kernel/config.h>

#include <util/memory/pool.h>
#include <util/random/random.h>
#include <util/generic/intrlist.h>
#include <util/datetime/base.h>

using namespace NModBalancer;

namespace {
    static double FailAnswerTime(double /*answerTime*/) throw () {
        return 1000000.0;
    }

    static double WarmCoeff() throw () {
        return 0.9999;
    }

    class TBackEnds: public IBackEnds, public IConfig::IFunc, public TModuleParams {
        class TBackEnd: public IBackEnd, public TPoolable, public TIntrusiveListItem<TBackEnd> {
            friend class TBackEnds;
        public:
            inline TBackEnd(const TBackEndDescriptor& descr, TBackEnds* parent)
                : BackEnd_(descr)
                , Parent_(parent)
                , Weight_(BackEnd_.Weight())
                , BalanceWeight_(Weight_)
                , Data_(1)
                , TimeRate_(300000, 0.9)
            {
            }

            virtual IModule* Module() const throw () {
                return BackEnd_.Module();
            }

            virtual void OnCompleteRequest(const TDuration& duration) throw () {
                const double answerTime = duration.MicroSeconds();

                Parent_->UpdateSearchTime(answerTime);
                Normalize(answerTime);
            }

            virtual void OnFailRequest(const TDuration& duration) throw () {
                const double answerTime = duration.MicroSeconds();

                Parent_->UpdateSearchTime(answerTime);
                Normalize(FailAnswerTime(answerTime));
            }

            inline void PrintStats(TOutputStream& out) const {
                out << "<backend>";
                Module()->PrintStats(out);
                out << "<weight>" << (unsigned)(100 * Weight_ * Parent_->Size()) << "</weight>";
                out << "</backend>";
            }

            // slowly increases chance to select for unused/slow backends
            void Warm() throw () {
                Data_ *= WarmCoeff();
            }

        protected:
            void Normalize(double answerTime) throw () {
                const double oldTime = TimeRate_.M();
                TimeRate_.Add(answerTime);
                const double newTime = TimeRate_.M();

                // first order prediction + normalization
                Data_ = (((1.5 * newTime > oldTime) ? 2.0 * newTime - oldTime : newTime) + 1) / (Parent_->GetSearchTime() + 1);
                Parent_->Normalize();
            }

        private:
            TBackEndDescriptor BackEnd_;
            TBackEnds* Parent_;
            double Weight_;
            double BalanceWeight_;
            double Data_;
            TMiddleValueTracker<double> TimeRate_;
        };

        typedef TIntrusiveListWithAutoDelete<TBackEnd, TDestructor> TBackEndList;

    public:
        inline TBackEnds(const TModuleParams& mp)
            : TModuleParams(mp)
            , SearchTime_(1000000, 0.99)
        {
            Config->ForEach(this);
        }

        START_PARSE {
            Add(Copy(value->AsSubConfig()));

            return;
        } END_PARSE

        inline void Add(const TBackEndDescriptor& descr) {
            BackEnds_.PushBack(new (*Pool) TBackEnd(descr, this));
        }

        inline double GetSearchTime() const throw () {
            return SearchTime_.M();
        }

        inline void UpdateSearchTime(double value) throw () {
            SearchTime_.Add(value);
        }

        virtual void Normalize() {
            double sum = 0;

            for (TBackEndList::TIterator i = BackEnds_.Begin(); i != BackEnds_.End(); ++i) {
                sum += i->BalanceWeight_ / (i->Data_ + 0.0001);
            }

            for (TBackEndList::TIterator i = BackEnds_.Begin(); i != BackEnds_.End(); ++i) {
                i->Weight_ = (i->BalanceWeight_ / (i->Data_ + 0.0001)) / sum;
                i->Warm();
            }
        }

        virtual size_t Size() const throw () {
            return BackEnds_.Size();
        }

    private:
        virtual IBackEnd* DoNext(const TAttempt&) {
            YASSERT(!BackEnds_.Empty());

            const double threshold = RandomNumber<double>();
            double acc = 0;

            for (TBackEndList::TIterator i = BackEnds_.Begin(); i != BackEnds_.End(); ++i) {
                acc += i->Weight_;
                if (acc >= threshold) {
                    return &*i;
                }
            }

            return 0;
        }

        virtual void DoPrintStats(TOutputStream& out) const {
            out << "<type>weighted</type>";
            out << "<backends>";

            for (TBackEndList::TConstIterator it = BackEnds_.Begin(); it != BackEnds_.End(); ++it) {
                it->PrintStats(out);
            }

            out << "</backends>";
        }

    private:
        TBackEndList BackEnds_;
        TMiddleValueTracker<double> SearchTime_;
    };
}

IBackEnds* NModBalancer::CreatePIDGroup(const TModuleParams& mp) {
    return new TBackEnds(mp);
}
