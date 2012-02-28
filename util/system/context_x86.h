#pragma once

#if defined(_x86_64_)
    #include "context_x86_64.h"
#elif defined(_i386_)
    #include "context_i686.h"
#else
    #error todo
#endif

extern "C" void __mylongjmp(__myjmp_buf env, int val)
#if defined(__GNUC__)
    __attribute__ ((__noreturn__))
#endif
    ;

extern "C" int __mysetjmp(__myjmp_buf env)
#if defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 1)))
    __attribute__ ((__returns_twice__))
#endif
    ;
