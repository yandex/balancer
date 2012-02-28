#pragma once

#include "defs.h"

#include <util/generic/ptr.h>

class Stroka;
class TOutputStream;

namespace NModBalancer {
    class IReAskArbiter {
        public:
            virtual ~IReAskArbiter() {
            }

            virtual void RegisterSucc() = 0;
            virtual void RegisterFail() = 0;
            virtual bool Authorize() const = 0;

            void PrintStats(TOutputStream& out) const;

        private:
            virtual void DoPrintStats(TOutputStream& out) const = 0;
    };

    TAutoPtr<IReAskArbiter> ConstructArbiter(const Stroka& name, const TModuleParams& mp);
}
