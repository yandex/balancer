#pragma once

#include "defs.h"

#include <util/generic/ptr.h>

namespace NModBalancer {
    class TBackEndDescriptor {
        public:
            TBackEndDescriptor(const TModuleParams& mp);

            inline double Weight() const throw () {
                return Weight_;
            }

            inline IModule* Module() const throw () {
                return Module_.Get();
            }

        private:
            double Weight_;
            TSharedPtr<IModule> Module_;
    };
}
