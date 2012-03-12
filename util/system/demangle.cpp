#include "platform.h"

#if defined(_win_) || defined(_darwin_)
    #define NO_CXXABI
#endif

#if !defined(NO_CXXABI)
    #include <stdexcept>
    #include <cxxabi.h>
#endif

#include "demangle.h"

const char* TCppDemangler::Demangle(const char* name) {
#if defined(NO_CXXABI)
    return name;
#else
    int status;
    TmpBuf_.Reset(__cxxabiv1::__cxa_demangle(name, 0, 0, &status));

    if (!TmpBuf_) {
        return name;
    }

    return TmpBuf_.Get();
#endif
}
