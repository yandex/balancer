#pragma once

#include <sys/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define AcquireAdaptiveLockSlow AllocAcquireAdaptiveLockSlow
#define SchedYield AllocSchedYield
#define ThreadYield AllocThreadYield
#define NSystemInfo NAllocSystemInfo

typedef void (*TJDelHook)(void*);
typedef void (*TJNewHook)(void*, size_t);

TJNewHook JNewHook();
TJDelHook JDelHook();

size_t AllocNumberOfCpus();
size_t MyThreadId();

#if defined(__cplusplus)
};
#endif
