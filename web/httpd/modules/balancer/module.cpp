#include "module.h"
#include "arbiter.h"
#include "iterator.h"

#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/config.h>
#include <web/httpd/kernel/rewind.h>
#include <web/httpd/kernel/errors.h>
#include <web/httpd/kernel/countio.h>

#include <util/generic/ptr.h>

using namespace NSrvKernel;
using namespace NModBalancer;

MODULE(balancer) {
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , Attempts_(1)
        , Incomplete_(0)
    {
        Config->ForEach(this);

        if (!BackEnds_) {
            ythrow TConfigParseError() << "no backends configured";
        }

        if (!BackEnds_->Size()) {
            ythrow TConfigParseError() << "empty backends";
        }

        if (!Arbiter_) {
            Arbiter_.Reset(ConstructArbiter("fake", *this).Release());
        }
    }

    START_PARSE {
        ON_KEY("attempts", Attempts_) {
            return;
        }

        const TModuleParams mp = Copy(value->AsSubConfig());

        {
            TAutoPtr<IBackEnds> be = ConstructBackEnds(key, mp);

            if (!!be) {
                BackEnds_.Reset(be.Release());

                return;
            }
        }

        {
            TAutoPtr<IReAskArbiter> arb = ConstructArbiter(key, mp);

            if (!!arb) {
                Arbiter_.Reset(arb.Release());

                return;
            }
        }
    } END_PARSE

    virtual void DoStats(TOutputStream& out) {
        out << "<incomplete>" << Incomplete_ << "</incomplete>";

        BackEnds_->PrintStats(out);
        Arbiter_->PrintStats(out);
    }

    virtual void DoRun(const TConnDescr& descr) {
        TRewindAbleInput clientInput(descr.Input);
        TCountOutput clientOutput(descr.Output);
        TConnIterator iter(BackEnds_.Get());
        size_t attempts = Attempts_;

        while (attempts--) {
            clientInput.Rewind();
            IBackEnd* conn = iter.Next(descr);
            const TInstant start = TInstant::Now();

            try {
                conn->Module()->Run(descr.Copy(&clientInput, &clientOutput));
                conn->OnCompleteRequest(TInstant::Now() - start);
                Arbiter_->RegisterSucc();

                return;
            } catch (...) {
                try {
                    throw;
                } catch (const THttpError& err) {
                    //try to reconnect only for 50X errors
                    if (err.Code() / 100 != 5) {
                        throw;
                    }
                } catch (const TSystemError& err) {
                    if (err.Status() == EPIPE) {
                        throw;
                    }
                }

                Arbiter_->RegisterFail();
                conn->OnFailRequest(TInstant::Now() - start);

                if (clientOutput.Written()) {
                    ++Incomplete_;

                    throw;
                }

                if (!Arbiter_->Authorize()) {
                    throw;
                }
            }
        }

        ythrow THttpError(503) << "all attempts failed";
    }

private:
    THolder<IBackEnds> BackEnds_;
    THolder<IReAskArbiter> Arbiter_;
    size_t Attempts_;
    size_t Incomplete_;
};

IModuleHandle* NModBalancer::Handle() {
    return TModule::ModuleHandle();
}
