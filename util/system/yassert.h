#pragma once

#include <util/system/defaults.h>

#if defined (_MSC_VER)
#   if defined(_DEBUG)
#     if defined (_CRTDBG_MAP_ALLOC)
#       include <cstdlib>  /* definitions for malloc/calloc */
#       include <malloc.h> /* must be before their redefinitions as _*_dbg() */
#     endif
#     include <crtdbg.h>
#     define PRECONDITIONX(cond,msg) do { if (!(cond)) _RPTF1(_CRT_ASSERT,"%s",(msg)); } while(0)
#     define CHECKX(cond,msg)        do { if (!(cond)) _RPTF1(_CRT_ERROR,"%s",(msg)); } while(0)
#     define PRECONDITION(cond)      _ASSERTE(cond)
#     define CHECK(cond)             _ASSERTE(cond)
#     define WARN(cond,msg)          do { if (cond) _RPTF1(_CRT_WARN,"%s\n",(msg)); } while(0)
#    else
#     define PRECONDITIONX(cond,msg)
#     define CHECKX(cond,msg)
#     define PRECONDITION(cond)
#     define CHECK(cond)
#     define WARN(cond,msg)
#   endif
#   define WARNX(g,c,l,m)
#   include <cassert>
#elif defined (__GNUC__)
#   ifdef _sun_
#     include <alloca.h>
#   endif
#   include <cassert>
#   define PRECONDITION(cond)      assert(cond)
#   define CHECK(cond)             assert(cond)
#if 0
#   define PRECONDITIONX(cond,msg) do { if (!(cond)) __assert(__FILE__, __LINE__, (msg)); } while(0)
#   define CHECKX(cond,msg)        do { if (!(cond)) __assert(__FILE__, __LINE__, (msg)); } while (0)
#endif
#   define WARN(c,m)
#   define WARNX(g,c,l,m)
#endif

#ifndef PRECONDITIONX
#   define PRECONDITIONX(cond,msg) PRECONDITION(cond)
#endif

#ifndef CHECKX
#   define CHECKX(cond,msg)        CHECK(cond)
#endif


#ifndef _MSC_VER
#if defined(__x86_64__) || defined(__i386__)
#define __debugbreak ydebugbreak
inline void ydebugbreak() {
    __asm__ volatile ("int $3\n");
}
#else
inline void __debugbreak()
{
    assert(0);
}
#endif
inline bool YaIsDebuggerPresent() { return false; }
#else
extern "C" {
    __declspec(dllimport) int __stdcall IsDebuggerPresent();
}
inline bool YaIsDebuggerPresent() { return IsDebuggerPresent() != 0; }
#endif

#undef YASSERT
#if !defined(NDEBUG) && !defined(__GCCXML__)
#  define YASSERT( a ) do { \
        try { \
            if ( EXPECT_FALSE( !(a) ) ) { \
                if (YaIsDebuggerPresent()) __debugbreak(); else assert(0&&(a)); \
            } \
        } catch (...) { \
            if (YaIsDebuggerPresent()) __debugbreak(); else assert(false && "Exception during assert"); \
        } \
    } while (0)
#else
#  define YASSERT( a ) do { if (false) { bool __xxx = a; UNUSED(__xxx); } } while (0)
#endif

/// method should not be used directly
void Panic(const char *file, int line, const char* function, const char* expr, const char* format, ...) printf_format(5,6) YNORETURN;

/// Assert that does not depend on NDEBUG macro and outputs message like printf
#define VERIFY(expr, format, ...) do { \
        if ( EXPECT_FALSE( !(expr) ) ) { \
            Panic(__FILE__, __LINE__, __FUNCTION__, #expr, format, ## __VA_ARGS__); \
        } \
    } while (0)

#ifndef NDEBUG
/// Assert that depend on NDEBUG macro and outputs message like printf
#   define VERIFY_DEBUG(expr, format, ...) do { \
        if ( EXPECT_FALSE( !(expr) ) ) { \
            Panic(__FILE__, __LINE__, __FUNCTION__, #expr, format, ## __VA_ARGS__); \
        } \
    } while (0)
#else
#   define VERIFY_DEBUG(expr, format, ...) do { \
        if (false) { \
            bool __xxx = expr; \
            UNUSED(__xxx); \
        } \
    } while (0)
#endif
