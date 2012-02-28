#include "average.h"
#include "backend.h"
#include "iterator.h"
#include "weighted2.h"

#include <web/httpd/kernel/iface.h>
#include <web/httpd/kernel/config.h>

#include <util/memory/pool.h>
#include <util/random/random.h>
#include <util/generic/intrlist.h>
#include <util/datetime/base.h>

using namespace NModBalancer;

namespace {
    class IEstimator {
        public:
            virtual ~IEstimator() {
            }

            virtual double OnCompleteRequest(const TDuration& answerTime) = 0;
            virtual double OnFailRequest(const TDuration& answerTime) = 0;
    };

    class TAnswerTimeQuality : public IEstimator {
        public:
            virtual double OnCompleteRequest(const TDuration& answerTime) {
                if (answerTime.MicroSeconds() > 1000000) {
                    return 0.;
                }
                return 1. / (1. + answerTime.MicroSeconds() / 1000000.);
            }
            virtual double OnFailRequest(const TDuration& /*answerTime*/) {
                return 0.;
            }
    };

    class TSlowAnswerTimeQuality : public IEstimator {
        public:
            virtual double OnCompleteRequest(const TDuration& answerTime) {
                if (answerTime.MicroSeconds() > 800000) {
                    return 0.;
                }
                return 1.;
            }
            virtual double OnFailRequest(const TDuration& /*answerTime*/) {
                return 0.;
            }
    };

    class TBackEnds: public IBackEnds, public IConfig::IFunc, public TModuleParams {
        struct TCorrectionParams : public IConfig::IFunc {
            public:
                TCorrectionParams()
                    : MinWeight_(0.05)
                    , MaxWeight_(5.)
                    , PlusDiffWeightPerSec_(0.05)
                    , MinusDiffWeightPerSec_(0.1)
                    , HistoryTime_(20000000)
                    , FeedbackTime_(1000000 * 100)

                {
                }

                START_PARSE {
                    ON_KEY("min_weight", MinWeight_) {
                        return;
                    }

                    ON_KEY("max_weight", MaxWeight_) {
                        return;
                    }

                    ON_KEY("plus_diff_per_sec", PlusDiffWeightPerSec_) {
                        return;
                    }

                    ON_KEY("minus_diff_per_sec", MinusDiffWeightPerSec_) {
                        return;
                    }

                    ON_KEY("history_time", HistoryTime_) {
                        return;
                    }

                    ON_KEY("feedback_time", FeedbackTime_) {
                        return;
                    }
                } END_PARSE

            public:
                double MinWeight_;
                double MaxWeight_;
                double PlusDiffWeightPerSec_;
                double MinusDiffWeightPerSec_;
                TDuration HistoryTime_;
                TDuration FeedbackTime_;
                bool SlowStart_;
        };

        class TBackEnd: public IBackEnd, public TPoolable, public TIntrusiveListItem<TBackEnd> {
            public:
                inline TBackEnd(const TBackEndDescriptor& descr, TBackEnds* parent)
                    : BackEnd_(descr)
                    , Parent_(parent)
                    , Weight_(1.)
                    , Emergency_(0.)
                    , QualityData_(Parent_->GetCorrectionParams()->HistoryTime_)
                    , LastCheckedWeight_(1.)
                    , LastCheckedTime_(Now())
                {
                }

                virtual IModule* Module() const throw () {
                    return BackEnd_.Module();
                }

                virtual void OnCompleteRequest(const TDuration& answerTime) throw () {
                    double quality = Parent_->GetEstimator()->OnCompleteRequest(answerTime);
                    QualityData_.Add(quality);
                    Weight_ = Correct(quality, Parent_->GetAvgQuality(), Weight_ / BackEnd_.Weight()) * BackEnd_.Weight();
                    Parent_->Normalize();
                    ++Succ_;
                }
                virtual void OnFailRequest(const TDuration& answerTime) throw() {
                    double quality = Parent_->GetEstimator()->OnFailRequest(answerTime);
                    QualityData_.Add(quality);
                    Weight_ = Correct(quality, Parent_->GetAvgQuality(), Weight_);
                    Parent_->Normalize();
                    ++Fail_;
                }

                double GetBackendWeight() const {
                    return BackEnd_.Weight();
                }

                double GetWeight() const {
                    return Weight_;
                }

                void Normalize(double c) {
                    Weight_ *= c;

                    Weight_ = Max(Parent_->GetCorrectionParams()->MinWeight_ * BackEnd_.Weight(), Weight_);
                    Weight_ = Min(Parent_->GetCorrectionParams()->MaxWeight_ * BackEnd_.Weight(), Weight_);
                }

                double GetEmergency() const {
                    return Emergency_;
                }
                void AddEmergency(double extra) {
                    Emergency_ += extra;
                }

                TPair<double, size_t> GetQuality() const {
                    return QualityData_.Get();;
                }

                inline void PrintStats(TOutputStream& out) const {
                    out << "\n<backend>\n";
                    Module()->PrintStats(out);
                    out << "<weight>" << (unsigned)(100 * Weight_) << "</weight>";
                    out << "<fail>" << Fail_ << "</fail>" << Endl;
                    out << "<succ>" << Succ_ << "</succ>" << Endl;
                    out << "<emergency>" << Emergency_ << "</emergency>";
                    out << "\n</backend>\n";
                }

            private:
                double Correct(double quality, double avgQuality, double curWeight) throw () {
                    YASSERT(quality >= 0. && quality <= 1.);
                    YASSERT(avgQuality >= 0. && avgQuality <= 1.);

                    TPair<double, size_t> newQuality = QualityData_.Get();

                    double qps = Max(0.1, newQuality.second / double(Parent_->GetCorrectionParams()->HistoryTime_.Seconds()));
                    // calculate new wegith
                    double newWeight;
                    if (newQuality.first / newQuality.second / Max(avgQuality, 0.00001) > 1) {
                        newWeight = curWeight * ((newQuality.first / newQuality.second / Max(avgQuality, 0.00001) - 1) * Parent_->GetCorrectionParams()->PlusDiffWeightPerSec_ / qps + 1);
                    } else {
                        newWeight = curWeight * ((newQuality.first / newQuality.second / Max(avgQuality, 0.00001) - 1) * Parent_->GetCorrectionParams()->MinusDiffWeightPerSec_ / qps + 1);
                    }

                    // apply feedback
                    newWeight = (1 - newWeight) / Parent_->GetCorrectionParams()->FeedbackTime_.Seconds() / qps + newWeight;

                    // apply restrictions
                    newWeight = Min(newWeight, Min(LastCheckedWeight_ * (1 + Parent_->GetCorrectionParams()->PlusDiffWeightPerSec_), Parent_->GetCorrectionParams()->MaxWeight_));
                    newWeight = Max(newWeight, Max(LastCheckedWeight_ * (1 - Parent_->GetCorrectionParams()->MinusDiffWeightPerSec_), Parent_->GetCorrectionParams()->MinWeight_));

                    if ((Now() - LastCheckedTime_).MicroSeconds() > 1000000) {
                        LastCheckedWeight_ = newWeight;
                        LastCheckedTime_ = Now();
                    }

                    return newWeight;
                }

            private:
                TBackEndDescriptor BackEnd_;
                TBackEnds* Parent_;
                double Weight_;
                double Emergency_;
                TTimeLimitedAvgTracker<1000> QualityData_;
                double LastCheckedWeight_;
                TInstant LastCheckedTime_;

                ui64 Fail_;
                ui64 Succ_;
        };

        typedef TIntrusiveListWithAutoDelete<TBackEnd, TDestructor> TBackEndList;

        public:
            inline TBackEnds(const TModuleParams& mp)
                : TModuleParams(mp)
            {
                Config->ForEach(this);

                if (!Estimator_) {
                    Estimator_.Reset(new TAnswerTimeQuality());
                }
            }

            START_PARSE {
                if (key == "correction_params") {
                    value->AsSubConfig()->ForEach(&CorrectionParams_);
                    return;
                }
                if (key == "quality") {
                    if (value->AsString() == "answer_time") {
                        Estimator_.Reset(new TAnswerTimeQuality);
                    } else if (value->AsString() == "slow_answer_time") {
                        Estimator_.Reset(new TSlowAnswerTimeQuality);
                    } else {
                        ythrow TConfigParseError() << "unknown estimation module " << value->AsString();
                    }
                    return;
                }

                Add(Copy(value->AsSubConfig()));

                return;
            } END_PARSE

            inline void Add(const TBackEndDescriptor& descr) {
                BackEnds_.PushBack(new (*Pool) TBackEnd(descr, this));
            }

            virtual IBackEnd* DoNext(const TAttempt&) {
                YASSERT(!BackEnds_.Empty());

                TBackEndList::TIterator i = BackEnds_.Begin();
                TBackEnd* retval = &*i;
                double max = i->GetEmergency();

                double total = 0.;
                for (; i != BackEnds_.End(); ++i) {
                    i->AddEmergency(i->GetWeight());
                    total += i->GetWeight();

                    const double cur = i->GetEmergency();

                    if (cur > max) {
                        max = cur;
                        retval = &*i;
                    }
                }

                retval->AddEmergency(-total);

                return retval;
            }

            virtual size_t Size() const throw () {
                return BackEnds_.Size();
            }

            virtual void DoPrintStats(TOutputStream& out) const {
                out << "<type>weighted</type>";
                out << "<backends>";

                for (TBackEndList::TConstIterator it = BackEnds_.Begin(); it != BackEnds_.End(); ++it) {
                    it->PrintStats(out);
                }

                out << "</backends>";
            }

            double GetAvgQuality() const {
                double sum = 0.;
                size_t total = 0;
                for (TBackEndList::TConstIterator it = BackEnds_.Begin(); it != BackEnds_.End(); ++it) {
                    TPair<double, size_t> r = it->GetQuality();
                    sum += r.first;
                    total += r.second;
                }
                if (total) {
                    sum = Max(Min(double(total), sum), 0.); // avoid calculation errors
                    return sum / total;
                } else {
                    return 1.; //??
                }
            }

            void Normalize() {
                double sum = 0;
                double startSum = 0;

                for (TBackEndList::TConstIterator it = BackEnds_.Begin(); it != BackEnds_.End(); ++it) {
                    sum += it->GetWeight();
                    startSum += it->GetBackendWeight();
                }

                for (TBackEndList::TIterator it = BackEnds_.Begin(); it != BackEnds_.End(); ++it) {
                    it->Normalize(startSum / sum);
                }

            }

            IEstimator* GetEstimator() const {
                return Estimator_.Get();
            }
            const TCorrectionParams* GetCorrectionParams() const {
                return &CorrectionParams_;
            }

        private:
            TBackEndList BackEnds_;
            TCorrectionParams CorrectionParams_;
            THolder<IEstimator> Estimator_;
    };

}

IBackEnds* NModBalancer::CreateWeighted2(const TModuleParams& mp) {
    return new TBackEnds(mp);
}
