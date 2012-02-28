#include "robust.h"
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
    template <size_t N>
    class TMedTr: public TMedianTracker<N> {
        public:
            inline double M() const throw () {
                return this->Get(90);
            }
    };

#if 1
    typedef TMedTr<300> TTr;
#else
    class TTr: public TMiddleValueTracker<double> {
    public:
        inline TTr()
            : TMiddleValueTracker<double>(1000000, 0.9)
        {
        }

        inline double Get(size_t) const throw () {
            return M();
        }
    };
#endif

    class TSuccRate: public TMiddleValueTracker<double> {
    public:
        inline TSuccRate()
            : TMiddleValueTracker<double>(1, 0.95)
        {
        }
    };

    class TBackEnds: public IBackEnds, public IConfig::IFunc, public TModuleParams {
        class TBackEnd: public IBackEnd, public TPoolable, public TIntrusiveListItem<TBackEnd> {
        public:
            inline TBackEnd(const TBackEndDescriptor& descr, TBackEnds* parent)
                : BackEnd_(descr)
                , Weight_(BackEnd_.Weight())
                , InitialWeight_(Weight_)
                , Parent_(parent)
            {
            }

            virtual ~TBackEnd() throw () {
            }

            virtual IModule* Module() const throw () {
                return BackEnd_.Module();
            }

            inline double Weight() const throw () {
                return Weight_;
            }

            inline void Normalize(double c) throw () {
                Weight_ *= c;
            }

            virtual void OnCompleteRequest(const TDuration& answerTime) throw () {
                TTr& tr = Parent_->Tr_;
                const double prev = Smooth(tr.M());
                tr.Add(answerTime.MicroSeconds());
                const double next = Smooth(tr.M());
                ++Succ_;

                Weight_ *= (1 + prev) / (1 + next);

                AdjustByError(1);
            }

            inline void AdjustByError(double val) {
                TSuccRate& sr = Parent_->Sr_;
                const double prev = Smooth(sr.M());
                sr.Add(val);
                const double next = Smooth(sr.M());

                if (val) {
                    Weight_ *= (0.01 + next) / (0.01 + prev) * (1 + 0.01 * (InitialWeight_ - Weight_) / (Weight_ + InitialWeight_ * 0.0001));
                } else {
                    Weight_ *= (0.01 + next) / (0.01 + prev);
                }
            }

            virtual void OnFailRequest(const TDuration& answerTime) throw () {
                (void)answerTime;

                ++Fail_;
                AdjustByError(0);
            }

            inline void Step() throw () {
                const double al = 0.9999;

                Weight_ = al * Weight_ + (1 - al) * InitialWeight_;
            }

            inline void PrintStats(TOutputStream& out) const {
                out << "<backend>";
                Module()->PrintStats(out);
                out << "<weight>" << (unsigned)(100 * Weight_) << "</weight>";
                out << "<fail>" << Fail_ << "</fail>";
                out << "<succ>" << Succ_ << "</succ>";
                out << "</backend>";
            }

            inline size_t RequestCount() const throw () {
                return Fail_ + Succ_;
            }

            inline double Smooth(double x) const throw () {
                const ui64 rc = RequestCount();

                if (rc > 1000) {
                    return x;
                }

                return rc * x / 1000;
            }

        private:
            TBackEndDescriptor BackEnd_;
            double Weight_;
            const double InitialWeight_;
            TBackEnds* Parent_;
            ui64 Fail_;
            ui64 Succ_;
        };

    public:
        inline TBackEnds(const TModuleParams& mp)
            : TModuleParams(mp)
            , Count_(0)
        {
            Config->ForEach(this);
        }

        virtual ~TBackEnds() throw () {
        }

        START_PARSE {
            Add(Copy(value->AsSubConfig()));

            return;
        } END_PARSE

        inline void Add(const TBackEndDescriptor& descr) {
            List_.PushBack(new (*Pool) TBackEnd(descr, this));
            ++Count_;
        }

        //very unoptimal
        virtual IBackEnd* DoNext(const TAttempt&) throw () {
            double sum = 0;

            for (TBackEndsList::TIterator it = List_.Begin(); it != List_.End(); ++it) {
                it->Step();
                sum += it->Weight();
            }

            sum = Size() / sum;

            for (TBackEndsList::TIterator it = List_.Begin(); it != List_.End(); ++it) {
                it->Normalize(sum);
            }

            double cur = 0;
            double rnd = RandomNumber<double>() * Size();

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
            return Count_;
        }

        virtual void DoPrintStats(TOutputStream& out) const {
            out << "<type>robust</type>";

            {
                out << "<tr>";

                const unsigned vals[] = {
                    50,
                    90,
                    95,
                    99
                };

                for (size_t i = 0; i < ARRAY_SIZE(vals); ++i) {
                    const unsigned v = vals[i];

                    out << "<med" << v << ">" << Tr_.Get(v) << "</med" << v << ">";
                }

                out << "</tr>";
            }

            out << "<sr>" << Sr_.M() << "</sr>";

            out << "<backends>";

            for (TBackEndsList::TConstIterator it = List_.Begin(); it != List_.End(); ++it) {
                it->PrintStats(out);
            }

            out << "</backends>";
        }

    private:
        typedef TIntrusiveListWithAutoDelete<TBackEnd, TDestructor> TBackEndsList;
        TBackEndsList List_;
        size_t Count_;
        TTr Tr_;
        TSuccRate Sr_;
    };
}

IBackEnds* NModBalancer::CreateRobust(const TModuleParams& mp) {
    return new TBackEnds(mp);
}
