#pragma once

#include "platform.h"

#if defined _unix_
#   define LOCSLASH_C '/'
#   define LOCSLASH_S "/"
#else
#   define LOCSLASH_C '\\'
#   define LOCSLASH_S "\\"
#endif // _unix_

#if defined(__INTEL_COMPILER) && defined(__cplusplus)
    #include <new>
#endif

// low and high parts of integers
#if !defined(_win_)
#   include <sys/param.h>
#endif

#if defined (BSD)
#   include <machine/endian.h>
#   if (BYTE_ORDER == LITTLE_ENDIAN)
#    define _little_endian_
#   elif (BYTE_ORDER == BIG_ENDIAN)
#    define _big_endian_
#   else
#    error unknown endian not supported
#   endif
#elif (defined (_sun_) && !defined (__i386__)) || defined (_hpux_) || defined (WHATEVER_THAT_HAS_BIG_ENDIAN)
#   define _big_endian_
#else
#   define _little_endian_
#endif

// alignment
#if (defined (_sun_) && !defined (__i386__)) ||  defined (_hpux_) || defined(__alpha__) || defined(__ia64__) || defined (WHATEVER_THAT_NEEDS_ALIGNING_QUADS)
#   define _must_align8_
#endif

#if (defined (_sun_) && !defined (__i386__)) ||  defined (_hpux_) || defined(__alpha__) || defined(__ia64__) || defined (WHATEVER_THAT_NEEDS_ALIGNING_LONGS)
#   define _must_align4_
#endif

#if (defined (_sun_) && !defined (__i386__)) ||  defined (_hpux_) || defined(__alpha__) || defined(__ia64__) || defined (WHATEVER_THAT_NEEDS_ALIGNING_SHORTS)
#   define _must_align2_
#endif

// basic integer types
#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(_linux_) || defined(_cygwin_) || (defined(__FreeBSD__) && __FreeBSD__ > 4) || defined(_darwin_)

#   ifndef __STDC_CONSTANT_MACROS
#       define __STDC_CONSTANT_MACROS
#   endif
#   ifndef __STDC_FORMAT_MACROS
#       define __STDC_FORMAT_MACROS
#   endif
#   include <inttypes.h>
    typedef int8_t i8;
    typedef int16_t i16;
    typedef int32_t i32;
    typedef int64_t i64;
    typedef uint8_t ui8;
    typedef uint16_t ui16;
    typedef uint32_t ui32;
    typedef uint64_t ui64;
#   define LL(number)   INT64_C(number)
#   define ULL(number)  UINT64_C(number)

#else

#   include <climits>

    typedef signed char i8;
    typedef unsigned char ui8;

#if (INT_MAX == 0x7FFF)
    typedef int i16;
    typedef unsigned int ui16;
#   define  PRId32 "d"
#   define  PRIi32 "i"
#   define  PRIo32 "o"
#   define  PRIu32 "u"
#   define  PRIx32 "x"
#   define  PRIX32 "X"
#   define  SCNd32 "d"
#   define  SCNi32 "i"
#   define  SCNo32 "o"
#   define  SCNu32 "u"
#   define  SCNx32 "x"
#elif (SHRT_MAX == 0x7FFF)
    typedef short i16;
    typedef unsigned short ui16;
#   define  PRId16 "hd"
#   define  PRIi16 "hi"
#   define  PRIo16 "ho"
#   define  PRIu16 "hu"
#   define  PRIx16 "hx"
#   define  PRIX16 "hX"
#   define  SCNd16 "hd"
#   define  SCNi16 "hi"
#   define  SCNo16 "ho"
#   define  SCNu16 "hu"
#   define  SCNx16 "hx"
#else
#   error i16 does not exist
#endif

#if (INT_MAX == 0x7FFFFFFFL)
    typedef int i32;
    typedef unsigned int ui32;
#   define  PRId32 "d"
#   define  PRIi32 "i"
#   define  PRIo32 "o"
#   define  PRIu32 "u"
#   define  PRIx32 "x"
#   define  PRIX32 "X"
#   define  SCNd32 "d"
#   define  SCNi32 "i"
#   define  SCNo32 "o"
#   define  SCNu32 "u"
#   define  SCNx32 "x"
#elif (LONG_MAX == 0x7FFFFFFFL)
    typedef long i32;
    typedef unsigned long ui32;
#   define  PRId32 "ld"
#   define  PRIi32 "li"
#   define  PRIo32 "lo"
#   define  PRIu32 "lu"
#   define  PRIx32 "lx"
#   define  PRIX32 "lX"
#   define  SCNd32 "ld"
#   define  SCNi32 "li"
#   define  SCNo32 "lo"
#   define  SCNu32 "lu"
#   define  SCNx32 "lx"
#else
#   error i32 does not exist
#endif

// names for i64, ui64
// hyper;long long;LONGLONG;LARGE_INTEGER;__int64;int64_t;int64
// unsigned hyper;unsigned long long;DWORDLONG;ULONGLONG;ULARGE_INTEGER;
// unsigned __int64;u_int64_t;uint64_t;uint64;u_int64

#if (LONG_MAX > 0x7FFFFFFFL)
#   define COMPILER_SUPPORTS_64
    typedef long i64;
    typedef unsigned long ui64;
#   define  LL(number)  number##l
#   define  ULL(number) number##ul
#   define  PRId64 "ld"
#   define  PRIi64 "li"
#   define  PRIo64 "lo"
#   define  PRIu64 "lu"
#   define  PRIx64 "lx"
#   define  PRIX64 "lX"
#   define  SCNd64 "ld"
#   define  SCNi64 "li"
#   define  SCNo64 "lo"
#   define  SCNu64 "lu"
#   define  SCNx64 "lx"
#elif defined (__GNUC__)
#   define COMPILER_SUPPORTS_64
    typedef long long i64;
    typedef unsigned long long ui64;
#   define  LL(number)  number##ll
#   define  ULL(number) number##ull
#   define  PRId64 "lld"
#   define  PRIi64 "lli"
#   define  PRIo64 "llo"
#   define  PRIu64 "llu"
#   define  PRIx64 "llx"
#   define  PRIX64 "llX"
#   define  SCNd64 "lld"
#   define  SCNi64 "lli"
#   define  SCNo64 "llo"
#   define  SCNu64 "llu"
#   define  SCNx64 "llx"
#elif defined _MSC_VER /* && defined _WIN32 ? */
#   define COMPILER_SUPPORTS_64
    typedef __int64 i64;
    typedef unsigned __int64 ui64;
#   define  LL(number)  number##i64
#   define  ULL(number) number##ui64
#   define  PRId64 "I64d"
#   define  PRIi64 "I64i"
#   define  PRIo64 "I64o"
#   define  PRIu64 "I64u"
#   define  PRIx64 "I64x"
#   define  PRIX64 "I64X"
#   define  SCNd64 "I64d"
#   define  SCNi64 "I64i"
#   define  SCNo64 "I64o"
#   define  SCNu64 "I64u"
#   define  SCNx64 "I64x"
#endif

#if !defined(COMPILER_SUPPORTS_64)
    #error this platform not supported
#endif

#endif // basic integer types

#if !defined(PRIi8)
    #define PRIi8 "i"
#endif

#if !defined(PRIu8)
    #define PRIu8 "u"
#endif

// Макросы для типов size_t и ptrdiff_t
#if defined(_32_)
#   if defined(_darwin_)
#       define  PRISZT "lu"
#   elif !defined(_cygwin_)
#       define  PRISZT PRIu32
#   else
#       define  PRISZT "u"
#   endif
#   define  SCNSZT SCNu32
#   define  PRIPDT PRIi32
#   define  SCNPDT SCNi32
#   define  PRITMT PRIi32
#   define  SCNTMT SCNi32
#elif defined(_64_)
#   if defined(_darwin_)
#       define  PRISZT "lu"
#   else
#       define  PRISZT PRIu64
#   endif
#   define  SCNSZT SCNu64
#   define  PRIPDT PRIi64
#   define  SCNPDT SCNi64
#   define  PRITMT PRIi64
#   define  SCNTMT SCNi64
#else
#   error "Unsupported platform"
#endif

#if defined(__GNUC__) && (__GNUC__> 3 || __GNUC__ == 3 && __GNUC_MINOR__ >= 3)
#define alias_hack __attribute__((__may_alias__))
#endif

#ifndef alias_hack
#define alias_hack
#endif

typedef ui16 alias_hack ui16a;
typedef ui32 alias_hack ui32a;
typedef ui64 alias_hack ui64a;
#if defined(__cplusplus)
#if defined(_big_endian_)
union u_u16 {
  ui16a v;
  struct {
    ui8 hi8, lo8;
  } u;
} alias_hack;
union u_u32 {
  ui32a v;
  float alias_hack f;
  struct {
    u_u16 hi16, lo16;
  } u;
} alias_hack;
union u_u64 {
  ui64a v;
  double alias_hack f;
  struct {
    u_u32 hi32, lo32;
  } u;
} alias_hack;
#else /* _little_endian_ */
union u_u16 {
  ui16a v;
  struct {
    ui8 lo8, hi8;
  } alias_hack u;
} alias_hack;
union u_u32 {
  ui32a v;
  float alias_hack f;
  struct {
    u_u16 lo16, hi16;
  } u;
} alias_hack;
union u_u64 {
  ui64a v;
  double alias_hack f;
  struct {
    u_u32 lo32, hi32;
  } u;
} alias_hack;
#endif
#endif

#ifdef CHECK_LO_HI_MACRO_USAGE

inline void check_64(const ui64 &) {}
inline void check_64(const i64 &) {}
inline void check_64(const double &) {}
inline void check_32(const ui32 &) {}
inline void check_32(const i32 &) {}
inline void check_32(const float &) {}
inline void check_16(const ui16 &) {}
inline void check_16(const i16 &) {}

#define LO_32(x) (check_64(x),(*(u_u64*)&x).u.lo32.v)
#define HI_32(x) (check_64(x),(*(u_u64*)&x).u.hi32.v)
#define LO_16(x) (check_32(x),(*(u_u32*)&x).u.lo16.v)
#define HI_16(x) (check_32(x),(*(u_u32*)&x).u.hi16.v)
#define LO_8(x)  (check_16(x),(*(u_u16*)&x).u.lo8)
#define HI_8(x)  (check_16(x),(*(u_u16*)&x).u.hi8)
#define LO_8_LO_16(x) (check_32(x),(*(u_u32*)&x).u.lo16.u.lo8)
#define HI_8_LO_16(x) (check_32(x),(*(u_u32*)&x).u.lo16.u.hi8)

#else

#define LO_32(x) (*(u_u64*)&x).u.lo32.v
#define HI_32(x) (*(u_u64*)&x).u.hi32.v
#define LO_16(x) (*(u_u32*)&x).u.lo16.v
#define HI_16(x) (*(u_u32*)&x).u.hi16.v
#define LO_8(x)  (*(u_u16*)&x).u.lo8
#define HI_8(x)  (*(u_u16*)&x).u.hi8
#define LO_8_LO_16(x) (*(u_u32*)&x).u.lo16.u.lo8
#define HI_8_LO_16(x) (*(u_u32*)&x).u.lo16.u.hi8

#endif // CHECK_LO_HI_MACRO_USAGE

#if !defined (DONT_USE_SUPERLONG) && !defined (SUPERLONG_MAX)

#define SUPERLONG_MAX ~LL(0)

typedef i64 SUPERLONG;

// missing builtin functions are here
#if defined (_MSC_VER) && !defined(_STLPORT_VERSION) && !defined(__GCCXML__)
inline SUPERLONG __cdecl abs(SUPERLONG number)
{
    return(number >= 0 ? number : -number);
}

// also somewhere we need ostream operator << (i64/ui64)
#endif // _MSC_VER

#endif // SUPERLONG

//#if (WCHAR_MAX == 0x0000FFFFul)  typedef wchar_t wchar16;
// missing format specifications
// UCS-2, native byteorder
typedef ui16 wchar16;

// internal symbol type: UTF-16LE
typedef wchar16 TChar;

typedef ui32 wchar32;

#define MAX_DOCHANDLE   INT_MAX
#define MAX_UDOCHANDLE  UINT_MAX
typedef i32 dochandle;
typedef ui32 udochandle;

#if defined (_MSC_VER)
    #include <basetsd.h>
    #include <sys/types.h>

    #ifndef HAVE_SSIZE_T
        typedef SSIZE_T ssize_t;
        #define HAVE_SSIZE_T 1
    #endif

#pragma warning(default:4018) /*signed/unsigned mismatch*/
#pragma warning(disable:4127) /*conditional expression is constant*/
#pragma warning(disable:4200) /*nonstandard extension used : zero-sized array in struct/union*/
#pragma warning(disable:4201) /*nonstandard extension used : nameless struct/union*/
#pragma warning(disable:4355) /*'this' : used in base member initializer list*/
#pragma warning(default:4389) /*'operator' : signed/unsigned mismatch*/
#pragma warning(disable:4510) /*default constructor could not be generated*/
#pragma warning(disable:4511) /*copy constructor could not be generated*/
#pragma warning(disable:4512) /*assignment operator could not be generated*/
#pragma warning(disable:4554) /*check operator precedence for possible error; use parentheses to clarify precedence*/
#pragma warning(disable:4610) /*'object' can never be instantiated - user defined constructor required*/
#pragma warning(disable:4706) /*assignment within conditional expression*/
#pragma warning(disable:4800) /*forcing value to bool 'true' or 'false' (performance warning)*/
#pragma warning(disable:4996) /*The POSIX name for this item is deprecated*/

#define _WINSOCKAPI_
#define NOMINMAX

#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#   define PRAGMA(x)    _Pragma(#x)
#   define RCSID(idstr) PRAGMA(comment(exestr,idstr))
#else
#   define RCSID(idstr) static const char rcsid[] = idstr
#endif

#define DECLARE_NOCOPY(aClass) \
    private: \
       aClass(const aClass&); \
       aClass& operator=(const aClass&);

#ifdef _win_
#   include <malloc.h>
#elif defined(_sun_)
#   include <alloca.h>
#endif

#ifdef __GNUC__
#define printf_format(n,m) __attribute__((__format__(__printf__, n, m)))
#endif

#ifndef printf_format
#define printf_format(n,m)
#endif

#ifdef __GNUC__
#define YDEPRECATED __attribute__((deprecated))
#endif

#ifndef YDEPRECATED
#define YDEPRECATED
#endif

#ifdef __GNUC__
#define YNORETURN __attribute__ ((__noreturn__))
#endif

#ifndef YNORETURN
#define YNORETURN
#endif

#ifdef __GNUC__
#define YUNUSED __attribute__((unused))
#endif

#ifndef YUNUSED
#define YUNUSED
#endif

#if (defined(__GNUC__) && (__GNUC__ > 2))
#define EXPECT_TRUE(Cond) __builtin_expect(!!(Cond), 1)
#define EXPECT_FALSE(Cond) __builtin_expect(!!(Cond), 0)
#define FORCED_INLINE inline __attribute__ ((always_inline))
#define PREFETCH_READ(Pointer, Priority) __builtin_prefetch((const void *)(Pointer), 0, Priority)
#define PREFETCH_WRITE(Pointer, Priority) __builtin_prefetch((const void *)(Pointer), 1, Priority)
#endif

#ifndef PREFETCH_READ
#define PREFETCH_READ(Pointer, Priority) (void)(const void *)(Pointer), (void)Priority
#endif

#ifndef PREFETCH_WRITE
#define PREFETCH_WRITE(Pointer, Priority) (void)(const void *)(Pointer), (void)Priority
#endif


#ifndef EXPECT_TRUE
#define EXPECT_TRUE(Cond) (Cond)
#define EXPECT_FALSE(Cond) (Cond)
#endif

#if (!defined(NDEBUG) && __GNUC__==3 && __GNUC_MINOR__==4) || defined(_sun_)
#undef FORCED_INLINE
#define FORCED_INLINE inline
#endif

#ifdef _MSC_VER
#define FORCED_INLINE __forceinline
#endif

#ifndef FORCED_INLINE
#define FORCED_INLINE inline
#endif

#ifdef __GNUC__
#   define _packed __attribute__((packed))
#else
#   define _packed
#endif

#if defined(__GNUC__) && (__GNUC__> 3 || __GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define _warn_unused_result __attribute__((warn_unused_result))
#endif

#ifndef _warn_unused_result
#define _warn_unused_result
#endif

#ifdef NDEBUG
#define IF_DEBUG(X)
#else
#define IF_DEBUG(X) X
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

// Конкатенация двух символов, даже при условии, что один из аргументов является макросом
// (макрос с совпадающим именем и совсем другим поведением может быть определен также в perl5/[0-9\.]+/mach/CORE/thread.h)
#ifdef JOIN
#undef JOIN
#endif

#define JOIN(X, Y)     DO_JOIN(X, Y)
#define DO_JOIN(X, Y)  DO_JOIN2(X, Y)
#define DO_JOIN2(X, Y) X##Y

#define STRINGIZE(X) STRINGIZE_AUX(X)
#define STRINGIZE_AUX(X) #X

#ifndef UNUSED
#define UNUSED(var) (void)var
#endif
