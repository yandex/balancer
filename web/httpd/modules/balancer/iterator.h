#pragma once

#include "defs.h"

#include <util/datetime/base.h>

class TDuration;
class TMemoryPool;
class TOutputStream;

namespace NModBalancer {
    class IBackEnd {
    public:
        inline IBackEnd() throw () {
        }

        virtual ~IBackEnd() {
        }

        virtual IModule* Module() const throw () = 0;
        virtual void OnCompleteRequest(const TDuration& answerTime) throw () = 0;
        virtual void OnFailRequest(const TDuration& answerTime) throw () = 0;
    };

    struct TAttempt {
        const TConnDescr* Conn;
        size_t Attempt;
    };

    class IBackEnds {
    public:
        inline IBackEnds() throw () {
        }

        virtual ~IBackEnds() {
        }

        inline IBackEnd* Next(const TConnDescr& descr, size_t attempt) {
            const TAttempt a = {
                  &descr
                , attempt
            };

            return DoNext(a);
        }

        virtual size_t Size() const throw () = 0;
        void PrintStats(TOutputStream& out) const;

    private:
        virtual IBackEnd* DoNext(const TAttempt& attempt) = 0;
        virtual void DoPrintStats(TOutputStream& out) const = 0;
    };

    TAutoPtr<IBackEnds> ConstructBackEnds(const Stroka& type, const TModuleParams& mp);

    class TConnIterator {
    public:
        TConnIterator(IBackEnds* backEnds)
            : BackEnds_(backEnds)
            , Cur_(0)
            , Attempt_(0)
        {
        }

        inline IBackEnd* Next(const TConnDescr& descr) throw () {
            {
                IBackEnd* next;

                while (AlreadyHave(next = BackEnds_->Next(descr, Attempt_++)) && Cur_ < BackEnds_->Size()) {
                }

                Attempts_[Cur_++] = next;
            }

            return Current();
        }

        inline IBackEnd* Current() const throw () {
            return Attempts_[Cur_ - 1];
        }

    private:
        inline bool AlreadyHave(const IBackEnd* b) const throw () {
            for (size_t i = 0; i < Cur_; ++i) {
                if (Attempts_[i] == b) {
                    return true;
                }
            }

            return false;
        }

    private:
        IBackEnds* BackEnds_;
        IBackEnd* Attempts_[16];
        size_t Cur_;
        size_t Attempt_;
    };
}
