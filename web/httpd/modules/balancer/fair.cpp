#include "fair.h"
#include "average.h"
#include "backend.h"
#include "iterator.h"

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
            inline TBackEnd(const TBackEndDescriptor& backEnd)
                : BackEnd_(backEnd)
                , Failures_(1, 0.85)
                , Weight_(CalcWeight())
                , Succ_(0)
                , Fail_(0)
            {
            }

            virtual ~TBackEnd() throw () {
            }

            virtual IModule* Module() const throw () {
                return BackEnd_.Module();
            }

            inline double WeightStep() const throw () {
                return BackEnd_.Weight();
            }

            inline double Weight() const throw () {
                return Weight_;
            }

            inline double CalcWeight() const throw () {
                return 1000000.0 / (1 + WeightStep() * AnswerTime_.Get(90) * (1 + Failures_.M()));
            }

            inline void ReCalcWeight() throw () {
                Weight_ = CalcWeight();
            }

            virtual void OnCompleteRequest(const TDuration& answerTime) throw () {
                ++Succ_;
                AnswerTime_.Add(answerTime.MicroSeconds());
                Failures_.Add(0);
                ReCalcWeight();
            }

            virtual void OnFailRequest(const TDuration& answerTime) throw () {
                (void)answerTime;

                ++Fail_;
                Failures_.Add(10);
                ReCalcWeight();
            }

            inline void PrintStats(TOutputStream& out) const {
                out << "<backend>";
                Module()->PrintStats(out);
                out << "<weight>" << (unsigned)(100 * Weight()) << "</weight>";
                out << "<fail>" << Fail_ << "</fail>";
                out << "<succ>" << Succ_ << "</succ>";
                out << "</backend>";
            }

        private:
            TBackEndDescriptor BackEnd_;
            TMedianTracker<100> AnswerTime_;
            TMiddleValueTracker<double> Failures_;
            double Weight_;
            ui64 Succ_;
            ui64 Fail_;
        };

    public:
        inline TBackEnds(const TModuleParams& mp)
            : TModuleParams(mp)
        {
            Config->ForEach(this);
        }

        virtual ~TBackEnds() throw () {
        }

        START_PARSE {
            Add(Copy(value->AsSubConfig()));

            return;
        } END_PARSE

        inline void Add(const TBackEndDescriptor& backEnd) {
            List_.PushBack(new (*Pool) TBackEnd(backEnd));
        }

        //very unoptimal
        virtual IBackEnd* DoNext(const TAttempt&) throw () {
            double sum = 0;

            for (TBackEndsList::TIterator it = List_.Begin(); it != List_.End(); ++it) {
                sum += it->Weight();
            }

            double cur = 0;
            double rnd = RandomNumber<double>() * sum;

            for (TBackEndsList::TIterator it = List_.Begin(); it != List_.End(); ++it) {
                cur += it->Weight();

                if (cur > rnd) {
                    return &*it;
                }
            }

            abort();
            return 0;
        }

        virtual size_t Size() const throw () {
            return List_.Size();
        }

        virtual void DoPrintStats(TOutputStream& out) const {
            out << "<type>fair</type>";
            out << "<backends>";

            for (TBackEndsList::TConstIterator it = List_.Begin(); it != List_.End(); ++it) {
                it->PrintStats(out);
            }

            out << "</backends>";
        }

    private:
        typedef TIntrusiveListWithAutoDelete<TBackEnd, TDestructor> TBackEndsList;
        TBackEndsList List_;
    };
}

IBackEnds* NModBalancer::CreateFair(const TModuleParams& mp) {
    return new TBackEnds(mp);
}
