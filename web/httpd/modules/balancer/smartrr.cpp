#include "smartrr.h"
#include "backend.h"
#include "iterator.h"

#include <web/httpd/kernel/iface.h>
#include <web/httpd/kernel/config.h>

#include <util/memory/pool.h>
#include <util/generic/avltree.h>

using namespace NModBalancer;

namespace {
    struct TBackEndCompare {
        template <class T>
        static inline bool Compare(const T& l, const T& r) throw () {
            return T::Compare(l, r);
        }
    };

    class TBackEnd: public IBackEnd, public TAvlTreeItem<TBackEnd, TBackEndCompare>, public TPoolable
                  , public TIntrusiveListItem<TBackEnd> {
    public:
        inline TBackEnd(const TBackEndDescriptor& descr)
            : BackEnd_(descr)
            , Weight_(1.0)
            , WeightStep_(1.0)
            , CntComplete_(0)
            , CntFail_(0)
        {
        }

        virtual ~TBackEnd() throw () {
        }

        static inline bool Compare(const TBackEnd& l, const TBackEnd& r) throw () {
            return (l.Weight() < r.Weight()) || ((l.Weight() == r.Weight()) && (&l < &r));
        }

        inline void IncUsageCount() throw () {
            UpdateState();
            double mul = 1.0;
            switch (State_) {
                case 1:
                    mul = 2.0;
                    break;
                case 2:
                    mul = 8.0;
                    break;
                case 3:
                    mul = 64.0;
                    break;
            }
            Weight_ += WeightStep_ * mul;
        }

        virtual IModule* Module() const throw () {
            return BackEnd_.Module();
        }

        virtual void OnCompleteRequest(const TDuration&) throw () {
            CntComplete_++;
        }

        virtual void OnFailRequest(const TDuration&) throw () {
            CntFail_++;
        }

        inline double Weight() const throw () {
            return Weight_;
        }

    private:
        inline void UpdateState() {
            switch (State_) {
                case 0:
                    if (CntFail_) {
                        State_ = 1;
                        CntComplete_ = 0;
                        CntFail_ = 0;
                    }
                    break;
                case 1:
                    if (CntFail_ + CntComplete_ > 10) {
                        if (CntFail_ > CntComplete_) {
                            State_ = 2;
                        } else if (!CntFail_) {
                            State_ = 0;
                        }
                        CntComplete_ = 0;
                        CntFail_ = 0;
                    }
                    break;
                case 2:
                    if (CntFail_ + CntComplete_ > 10) {
                        if (!CntComplete_) {
                            State_ = 3;
                        } else if (CntComplete_ > CntFail_) {
                            State_ = 1;
                        }
                        CntComplete_ = 0;
                        CntFail_ = 0;
                    }
                    break;
                case 3:
                    if (CntComplete_) {
                        State_ = 2;
                        CntComplete_ = 0;
                        CntFail_ = 0;
                    }
                    break;
            }
        }

        TBackEndDescriptor BackEnd_;
        double Weight_;
        const double WeightStep_;
        unsigned CntComplete_;
        unsigned CntFail_;
        unsigned State_;
    };

    class TBackEnds: public IBackEnds, public IConfig::IFunc, public TModuleParams {
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

        inline void Add(const TBackEndDescriptor& descr) {
            TBackEnd* b = new (*Pool) TBackEnd(descr);
            List_.PushBack(b);
            Servers_.Insert(b);
        }

        virtual IBackEnd* DoNext(const TAttempt&) throw () {
            TBackEnd* next = &*Servers_.Begin();

            Servers_.Erase(next);
            next->IncUsageCount();
            Servers_.Insert(next);

            return next;
        }

        virtual size_t Size() const throw () {
            return List_.Size();
        }

        virtual void DoPrintStats(TOutputStream& out) const {
            out << "<type>smartroundrobin</type>";
        }

    private:
        TIntrusiveListWithAutoDelete<TBackEnd, TDestructor> List_;
        typedef TAvlTree<TBackEnd, TBackEndCompare> TServers;
        TServers Servers_;
    };
}

IBackEnds* NModBalancer::CreateSmartRoundRobin(const TModuleParams& mp) {
    return new TBackEnds(mp);
}
