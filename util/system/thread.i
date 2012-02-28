//do not use directly
#pragma once
#include "platform.h"

#if defined(_win_)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #include <process.h>

    typedef HANDLE THREADHANDLE;
#else
    #include <pthread.h>
    #include <sched.h>

    typedef pthread_t THREADHANDLE;
#endif

#include <util/digest/numeric.h>

static inline size_t SystemCurrentThreadIdImpl() throw () {
#ifdef _mt_
    #if defined(_unix_)
        return (size_t)pthread_self();
    #elif defined(_win_)
        return (size_t)GetCurrentThreadId();
    #else
        #error todo
    #endif
#else
    return 0;
#endif
}

template <class T>
static inline T ThreadIdHashFunction(T t) throw () {
    /*
     * we must permute threadid bits, because some strange platforms(such Linux)
     * have strange threadid numeric properties
     */
    return IntHash(t);
}

static inline size_t SystemCurrentThreadId() throw () {
    return ThreadIdHashFunction(SystemCurrentThreadIdImpl());
}
