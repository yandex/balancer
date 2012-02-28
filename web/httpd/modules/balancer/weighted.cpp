#include "average.h"
#include "backend.h"
#include "iterator.h"
#include "weighted.h"

#include <web/httpd/kernel/iface.h>
#include <web/httpd/kernel/config.h>

#include <util/memory/pool.h>
#include <util/random/random.h>
#include <util/generic/intrlist.h>
#include <util/datetime/base.h>

using namespace NModBalancer;

namespace {
    class TBackEnds: public IBackEnds, public IConfig::IFunc, public TModuleParams {
        class TBackEnd: public IBackEnd, public TPoolable, public TIntrusiveListItem<TBackEnd> {
        public:
            inline TBackEnd(const TBackEndDescriptor& descr, TBackEnds* parent)
                : BackEnd_(descr)
                , Parent_(parent)
                , Weight_(1)
                , Emergency_(0)
                , FailRate_(0, 0.99)
                , Succ_(0)
                , Fail_(0)
            {
            }

            virtual IModule* Module() const throw () {
                return BackEnd_.Module();
            }

            virtual void OnCompleteRequest(const TDuration& answerTime) throw () {
                ++Succ_;
                const double oldTime = Parent_->GetSearchTime();

                Parent_->UpdateSearchTime(answerTime.MicroSeconds());
                Weight_ *= (oldTime + 1) / (Parent_->GetSearchTime() + 1);
                Parent_->Normalize();
                FailRate_.Add(0);
            }

            virtual void OnFailRequest(const TDuration& answerTime) throw () {
                (void)answerTime;

                ++Fail_;
                FailRate_.Add(1);
            }

            inline double GetWeight() const throw () {
                return Weight_;
            }

            inline void SetWeight(double weight) throw () {
                Weight_ = weight;
            }

            inline double GetEmergency() const throw () {
                return Emergency_;
            }

            inline void SetEmergency(double emergency) throw () {
                Emergency_ = emergency;
            }

            inline void AddEmergency(double value) throw () {
                Emergency_ += value;
            }

            inline double GetFailRate() const throw () {
                return Min<double>(FailRate_.M(), 0.99);
            }

            inline void PrintStats(TOutputStream& out) const {
                out << "<backend>";
                Module()->PrintStats(out);
                out << "<weight>" << (unsigned)(100 * Weight_ * Parent_->Size()) << "</weight>";
                out << "<fail-rate>" << (unsigned)(1000 * FailRate_.M()) << "</fail-rate>";
                out << "<fail>" << Fail_ << "</fail>";
                out << "<succ>" << Succ_ << "</succ>";
                out << "</backend>";
            }

        private:
            TBackEndDescriptor BackEnd_;
            TBackEnds* Parent_;
            double Weight_;
            double Emergency_;
            TMiddleValueTracker<double> FailRate_;
            ui64 Succ_;
            ui64 Fail_;
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
                sum += i->GetWeight();
            }

            for (TBackEndList::TIterator i = BackEnds_.Begin(); i != BackEnds_.End(); ++i) {
                i->SetWeight(i->GetWeight() / sum);
            }
        }

        virtual size_t Size() const throw () {
            return BackEnds_.Size();
        }

    private:
        TBackEnd* Choose() {
            TBackEndList::TIterator i = BackEnds_.Begin();
            TBackEnd* retval = &*i;
            double max = i->GetEmergency();

            for (; i != BackEnds_.End(); ++i) {
                i->AddEmergency(i->GetWeight());

                const double cur = i->GetEmergency();

                if (cur > max) {
                    max = cur;
                    retval = &*i;
                }
            }

            retval->AddEmergency(-1);

            return retval;
        }

        virtual IBackEnd* DoNext(const TAttempt&) {
            YASSERT(!BackEnds_.Empty());

            size_t count = Size();
            TBackEnd* retval = 0;

            while (!retval && --count) {
                retval = Choose();

                if (RandomNumber<double>() < retval->GetFailRate()) {
                    retval = 0;
                }
            }

            if (!retval) {
                return &*BackEnds_.Begin();
            }

            return retval;
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

IBackEnds* NModBalancer::CreateWeighted(const TModuleParams& mp) {
    return new TBackEnds(mp);
}
