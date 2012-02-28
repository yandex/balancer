#include "alloc.h"

#include <util/generic/singleton.h>

template <>
struct TSingletonTraits<TDefaultAllocator> {
    static const size_t Priority = 256;
};

IAllocator* TDefaultAllocator::Instance() throw () {
    return Singleton<TDefaultAllocator>();
}
