#include "platform.h"

#ifdef _win_
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #include <process.h>
#else
    #include <pthread.h>
    #include <sched.h>
#endif

void SchedYield() throw () {
#if defined(_unix_)
    sched_yield();
#else
    Sleep(0);
#endif
}

void ThreadYield() throw () {
#if defined(_mt_)
    #if defined(_freebsd_)
        pthread_yield();
    #else
        SchedYield();
    #endif
#endif
}
