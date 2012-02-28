#pragma once

#include "defaults.h"

#include <util/generic/typetraits.h>
#include <util/generic/static_assert.h>

#if !defined(_mt_)
    #include "atomic_fake.h"
#elif defined(__GNUC__) && (__GNUC__ > 3) && !defined(_i386_)
    #include "atomic_gcc.h"
#elif defined(__GNUC__) && (defined(_i386_) || defined(_x86_64_))
    #include "atomic_x86.h"
#elif defined _MSC_VER
    #include "atomic_win.h"
#else
    #error todo
#endif

#if defined(__GNUC__)
    #define ATOMIC_COMPILER_BARRIER() __asm__ __volatile__("" : : : "memory")
#endif

#if !defined(ATOMIC_COMPILER_BARRIER)
    #define ATOMIC_COMPILER_BARRIER()
#endif

typedef TTypeTraits<TAtomic>::TNonVolatile TAtomicBase;

STATIC_ASSERT(sizeof(TAtomic) == sizeof(void*));

static inline TAtomicBase AtomicSub(TAtomic& a, TAtomicBase v) {
    return AtomicAdd(a, -v);
}

#if !defined(_mt_)
    #define USE_GENERIC_SETGET
#elif defined(_x86_64_)
    #if defined(__GNUC__)
        static inline TAtomicBase AtomicGet(const TAtomic& a) {
            register TAtomicBase tmp;

            __asm__ __volatile__ (
                  "movq %1, %0"
                : "=r" (tmp)
                : "m" (a)
                :
            );

            return tmp;
        }

        static inline void AtomicSet(TAtomic& a, TAtomicBase v) {
            __asm__ __volatile__ (
                  "movq %1, %0"
                : "=m" (a)
                : "r" (v)
                :
            );
        }
    #elif defined(_win_)
        //TODO
        #define USE_GENERIC_SETGET
    #else
        #error unsupported platform
    #endif
#elif defined(_i386_)
    #define USE_GENERIC_SETGET
#else
    #error unsupported platform
#endif

#if defined(USE_GENERIC_SETGET)
static inline TAtomicBase AtomicGet(const TAtomic& a) {
    return a;
}

static inline void AtomicSet(TAtomic& a, TAtomicBase v) {
    a = v;
}
#endif

template <typename T>
inline TAtomic* AsAtomicPtr(T volatile *val)
{
    return reinterpret_cast<TAtomic*>(val);
}

template <typename T> struct TAtomicTraits {
    enum {
        Castable = TTypeTraits<T>::IsIntegral && sizeof(T) == sizeof(TAtomicBase),
    };
    typedef TEnableIf<Castable, bool> TdBool;
};

template <typename T>
inline TAtomicBase AtomicGet(T const volatile& target
    , typename TAtomicTraits<T>::TdBool::TResult = false)
{
    return AtomicGet(*AsAtomicPtr(&target));
}

template <typename T>
inline void AtomicSet(T volatile& target, TAtomicBase v
    , typename TAtomicTraits<T>::TdBool::TResult = false)
{
    AtomicSet(*AsAtomicPtr(&target), v);
}

template <typename T>
inline TAtomicBase AtomicIncrement(T volatile& target
    , typename TAtomicTraits<T>::TdBool::TResult = false)
{
    return AtomicIncrement(*AsAtomicPtr(&target));
}

template <typename T>
inline TAtomicBase AtomicDecrement(T volatile& target
    , typename TAtomicTraits<T>::TdBool::TResult = false)
{
    return AtomicDecrement(*AsAtomicPtr(&target));
}

template <typename T>
inline TAtomicBase AtomicAdd(T volatile& target, TAtomicBase value
    , typename TAtomicTraits<T>::TdBool::TResult = false)
{
    return AtomicAdd(*AsAtomicPtr(&target), value);
}

template <typename T>
inline TAtomicBase AtomicSub(T volatile& target, TAtomicBase value
    , typename TAtomicTraits<T>::TdBool::TResult = false)
{
    return AtomicSub(*AsAtomicPtr(&target), value);
}

template <typename T>
inline TAtomicBase AtomicSwap(T volatile& target, TAtomicBase exchange
    , typename TAtomicTraits<T>::TdBool::TResult = false)
{
    return AtomicSwap(*AsAtomicPtr(&target), exchange);
}

template <typename T>
inline TAtomicBase AtomicCas(T volatile* target, TAtomicBase exchange, TAtomicBase compare
    , typename TAtomicTraits<T>::TdBool::TResult = false)
{
    return AtomicCas(AsAtomicPtr(target), exchange, compare);
}


#if !defined(HAVE_CUSTOM_LOCKS)
static inline bool AtomicTryLock(TAtomic* a) {
    return AtomicCas(a, 1, 0);
}

static inline bool AtomicTryAndTryLock(TAtomic* a) {
    return (AtomicGet(*a) == 0) && AtomicTryLock(a);
}

static inline void AtomicUnlock(TAtomic* a) {
    ATOMIC_COMPILER_BARRIER();
    AtomicSet(*a, 0);
}

template <typename T>
inline TAtomicBase AtomicTryLock(T volatile* target
    , typename TAtomicTraits<T>::TdBool::TResult = false)
{
    return AtomicTryLock(AsAtomicPtr(target));
}

template <typename T>
inline TAtomicBase AtomicTryAndTryLock(T volatile* target
    , typename TAtomicTraits<T>::TdBool::TResult = false)
{
    return AtomicTryAndTryLock(AsAtomicPtr(target));
}

template <typename T>
inline void AtomicUnlock(T volatile* target
    , typename TAtomicTraits<T>::TdBool::TResult = false)
{
    AtomicUnlock(AsAtomicPtr(target));
}

template <typename T>
inline bool AtomicCas(T* volatile* target, T* exchange, T* compare) {
    return AtomicCas(AsAtomicPtr(target), (TAtomicBase) exchange, (TAtomicBase) compare);
}
#endif
