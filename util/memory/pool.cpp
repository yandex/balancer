
#include "pool.h"

#include <util/generic/singleton.h>

template <>
struct TSingletonTraits<TMemoryPool::TLinearGrow> {
    static const size_t Priority = 256;
};

template <>
struct TSingletonTraits<TMemoryPool::TExpGrow> {
    static const size_t Priority = 256;
};

TMemoryPool::IGrowPolicy* TMemoryPool::TLinearGrow::Instance() throw () {
    return Singleton<TLinearGrow>();
}

TMemoryPool::IGrowPolicy* TMemoryPool::TExpGrow::Instance() throw () {
    return Singleton<TExpGrow>();
}
