#include "iface.h"

#include <util/generic/hash.h>

namespace NSrvKernel {
    class TModuleFactory: public IModuleFactory {
    public:
        inline TModuleFactory()
            : S_(0)
        {
        }

        inline TModuleFactory(IModuleFactory* slave)
            : S_(slave)
        {
        }

        inline void AddHandle(IModuleHandle* hndl) {
            Handles_[hndl->Name()] = hndl;
        }

    private:
        virtual IModuleHandle* DoSelectHandle(const Stroka& name) {
            THandles::const_iterator it = Handles_.find(name);

            if (it != Handles_.end()) {
                return it->second;
            }

            if (S_) {
                return S_->SelectHandle(name);
            }

            return 0;
        }

    private:
        IModuleFactory* S_;
        typedef yhash<Stroka, IModuleHandle*> THandles;
        THandles Handles_;
    };
}
