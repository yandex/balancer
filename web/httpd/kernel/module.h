#pragma once

#include "iface.h"

#include <util/generic/singleton.h>

namespace NSrvKernel {
    template <class T, const char* N>
    class TGenericModule: virtual public IModule {
        class THandle: public IModuleHandle {
        public:
            virtual const char* DoName() const throw () {
                return N;
            }

            virtual IModule* DoConstruct(const TModuleParams& mp) {
                return new T(mp);
            }
        };

    public:
        static inline IModuleHandle* ModuleHandle() {
            return Singleton<THandle>();
        }

        virtual IModuleHandle* DoHandle() const throw () {
            return this->ModuleHandle();
        }
    };
}

#define MODULE(X) \
namespace { \
    namespace N ## X { \
        char NAME[] = #X; \
        struct TModule; \
    } \
    using namespace N ## X; \
} \
struct N ## X::TModule: public TGenericModule<TModule, NAME>, public TModuleParams, public IConfig::IFunc
