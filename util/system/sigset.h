#pragma once

// Functions for manipulating signal sets

#include "compat.h"

#if defined _unix_
#   include <pthread.h>
#elif defined _win_
    // Flags for sigprocmask:
#   define SIG_BLOCK   1
#   define SIG_UNBLOCK 2
#   define SIG_SETMASK 3

typedef ui32 sigset_t;

#else
#   error not supported yet
#endif


inline int SigEmptySet(sigset_t *set) {
#if defined _unix_
     return sigemptyset(set);
#else
     return 0;
#endif
}

inline int SigFillSet(sigset_t *set) {
#if defined _unix_
     return sigfillset(set);
#else
     return 0;
#endif
}

inline int SigAddSet(sigset_t *set, int signo) {
#if defined _unix_
     return sigaddset(set, signo);
#else
     return 0;
#endif
}

inline int SigDelSet(sigset_t *set, int signo) {
#if defined _unix_
     return sigdelset(set, signo);
#else
     return 0;
#endif
}

inline int SigIsMember(const sigset_t *set, int signo) {
#if defined _unix_
     return sigismember(set, signo);
#else
     return 0;
#endif
}

inline int SigProcMask(int how, const sigset_t *set, sigset_t *oset) {
#if defined _unix_
#   if defined _mt_
    return pthread_sigmask(how, set, oset);
#   else
    return sigprocmask(how, set, oset);
#   endif
#else
    return 0;
#endif
}
