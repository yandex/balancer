#include "rrobin.h"
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
            , Weight_(BackEnd_.Weight())
            , Fail_(0)
            , Succ_(0)
        {
        }

        virtual ~TBackEnd() throw () {
        }

        inline double WeightStep() const throw () {
            return BackEnd_.Weight();
        }

        static inline bool Compare(const TBackEnd& l, const TBackEnd& r) throw () {
            return (l.Weight() < r.Weight()) || ((l.Weight() == r.Weight()) && (&l < &r));
        }

        inline void IncUsageCount() throw () {
            Weight_ += WeightStep();
        }

        virtual IModule* Module() const throw () {
            return BackEnd_.Module();
        }

        virtual void OnCompleteRequest(const TDuration&) throw () {
            ++Succ_;
        }

        virtual void OnFailRequest(const TDuration&) throw () {
            ++Fail_;
        }

        inline double Weight() const throw () {
            return Weight_;
        }

        inline void PrintStats(TOutputStream& out) const {
            out << "<backend>";
            Module()->PrintStats(out);
            out << "<weight>" << Weight_ << "</weight>";
            out << "<fail>" << Fail_ << "</fail>";
            out << "<succ>" << Succ_ << "</succ>";
            out << "</backend>";
        }

    private:
        TBackEndDescriptor BackEnd_;
        double Weight_;
        ui64 Fail_;
        ui64 Succ_;
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
            out << "<type>roundrobin</type>";
            out <<"<backends>";
            for (TBackEndsList::TConstIterator it = List_.Begin(); it != List_.End(); ++it) {
                it->PrintStats(out);
            }
            out <<"</backends>";
        }

    private:
        typedef TIntrusiveListWithAutoDelete<TBackEnd, TDestructor> TBackEndsList;
        TIntrusiveListWithAutoDelete<TBackEnd, TDestructor> List_;
        typedef TAvlTree<TBackEnd, TBackEndCompare> TServers;
        TServers Servers_;
    };
}

IBackEnds* NModBalancer::CreateRoundRobin(const TModuleParams& mp) {
    return new TBackEnds(mp);
}
