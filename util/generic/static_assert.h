#pragma once

#include <util/system/defaults.h>

// Статический assert генерирует ошибку компиляции, если константное выражение при приведении к bool даёт false

template <bool>
struct TStaticAssertionFailed;

template <>
struct TStaticAssertionFailed <true> {
};

template <int x>
struct TStaticAssertTest {
};

// Микрософтовский __LINE__ не работает, используем __COUNTER__
#ifdef _MSC_VER
#define STATIC_ASSERT_COUNTER __COUNTER__
#else
#define STATIC_ASSERT_COUNTER __LINE__
#endif

#if defined (__GNUC__) && (__GNUC__ <= 2)
#define STATIC_ASSERT(cond)
#else
#define STATIC_ASSERT(cond) \
    typedef TStaticAssertTest<sizeof(TStaticAssertionFailed<static_cast<bool>((cond))>)> \
    JOIN(StaticAssertVar,STATIC_ASSERT_COUNTER);
#endif

template<int> struct TCompileTimeError;
template<> struct TCompileTimeError<true> {};
#define STATIC_CHECK(expression, message) \
    { \
        TCompileTimeError<((expression) != 0)> ERROR__##message; \
        (void)ERROR__##message; \
    }
