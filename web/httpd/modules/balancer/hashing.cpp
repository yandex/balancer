#include "hashing.h"
#include "backend.h"
#include "iterator.h"

#include <web/httpd/kernel/iface.h>
#include <web/httpd/kernel/config.h>
#include <web/httpd/kernel/http.h>

#include <util/memory/pool.h>
#include <util/random/random.h>
#include <util/generic/intrlist.h>
#include <util/datetime/base.h>

using namespace NModBalancer;

namespace {
    class TBackEnds: public IBackEnds, public IConfig::IFunc, public TModuleParams {
        class TBackEnd: public IBackEnd {
        public:
            inline TBackEnd(const TBackEndDescriptor& descr, TBackEnds* parent)
                : BackEnd_(descr)
                , Parent_(parent)
                , Fail_(0)
                , Succ_(0)
            {
            }

            virtual ~TBackEnd() throw () {
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

            inline void PrintStats(TOutputStream& out) const {
                out << "<backend>";
                Module()->PrintStats(out);
                out << "<fail>" << Fail_ << "</fail>";
                out << "<succ>" << Succ_ << "</succ>";
                out << "</backend>";
            }

        private:
            TBackEndDescriptor BackEnd_;
            TBackEnds* Parent_;
            ui64 Fail_;
            ui64 Succ_;
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

        inline void Add(const TBackEndDescriptor& descr) {
            BackEnds.push_back(TBackEnd(descr, this));
        }

        virtual IBackEnd* DoNext(const TAttempt& params) throw () {
            TRequestHash hash = params.Conn->Hash;

            if (params.Attempt) {
                //TODO
                hash = RandomNumber<TRequestHash>();
            }

            return &BackEnds[hash % BackEnds.size()];
        }

        virtual size_t Size() const throw () {
            return BackEnds.size();
        }

        virtual void DoPrintStats(TOutputStream& out) const {
            out << "<type>subnet</type>";
            out << "<backends>";

            for (TBackEndVector::const_iterator it = BackEnds.begin(); it != BackEnds.end(); ++it) {
                it->PrintStats(out);
            }

            out << "</backends>";
        }

    private:
        typedef yvector<TBackEnd> TBackEndVector;
        TBackEndVector BackEnds;
    };
}

IBackEnds* NModBalancer::CreateHashing(const TModuleParams& mp) {
    return new TBackEnds(mp);
}
