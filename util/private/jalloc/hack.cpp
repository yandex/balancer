#include "hack.h"

#include <util/system/thread.i>
#include <util/system/info.cpp>
#include <util/system/yield.cpp>
#include <util/system/spinlock.cpp>

#include <util/memory/profile.h>

TJNewHook JNewHook() {
    return TAllocatorHooks::NewHook();
}

TJDelHook JDelHook() {
    return TAllocatorHooks::DelHook();
}

size_t AllocNumberOfCpus() {
    return NAllocSystemInfo::CachedNumberOfCpus();
}

size_t MyThreadId() {
    return SystemCurrentThreadId();
}

#include "spinlock.h"

void SPIN_L(spinlock_t* l) {
    AcquireAdaptiveLock(l);
}

void SPIN_U(spinlock_t* l) {
    ReleaseAdaptiveLock(l);
}
