#pragma once

#include <util/system/yassert.h>
#include <util/ycoroutine.h>

namespace NSrvKernel {
    class TMyEvent {
    public:
        inline TMyEvent(TContExecutor* e) throw ()
            : E_(e)
            , Ev_(0)
            , S_(false)
        {
        }

        inline ~TMyEvent() throw () {
            YASSERT(Ev_ == 0);
        }

        inline TContExecutor* Executor() const throw () {
            return E_;
        }

        inline void Wait() throw () {
            WaitD(TInstant::Max());
        }

        inline void WaitD(const TInstant& d) throw () {
            if (!S_) {
                TContEvent ev(E_->Running()->ContPtr());

                Ev_ = &ev;
                Ev_->WaitD(d);
                Ev_ = 0;
            }

            S_ = false;
        }

        inline void Signal() throw () {
            S_ = true;

            if (Ev_) {
                Ev_->Wake();
            }
        }

    private:
        TContExecutor* E_;
        TContEvent* Ev_;
        bool S_;
    };
}
