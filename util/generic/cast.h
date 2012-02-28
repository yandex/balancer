#pragma once

#include "typetraits.h"

#include <cstdlib>
#include <typeinfo>

#include <util/system/compat.h>
#include <util/system/yassert.h>
#include <util/system/defaults.h>

#if !defined(NDEBUG)
    #define USE_DEBUG_CHECKED_CAST
#endif

template <class TTo, class TFrom>
static inline TTo ReinterpretCast(TFrom from) {
    union {
        TFrom from;
        TTo to;
    } a;

    a.from = from;

    return a.to;
}

/*
 * replacement for dynamic_cast(dynamic_cast in debug mode, else static_cast)
 */
template <class T, class F>
static inline T CheckedCast(F f) {
#if defined(USE_DEBUG_CHECKED_CAST)
    if (!f) {
        return 0;
    }

    T ret = dynamic_cast<T>(f);

    YASSERT(ret && "checked cast failed");

    return ret;
#else
    return static_cast<T>(f);
#endif
}

/*
 * be polite
 */
#undef USE_DEBUG_CHECKED_CAST

template <bool isUnsigned>
class TInteger;

template <>
class TInteger<true> {
public:
    template <class TUnsigned>
    static bool IsNegative(TUnsigned) {
        return false;
    }
};

template <>
class TInteger<false> {
public:
    template <class TSigned>
    static bool IsNegative(TSigned value) {
        return value < 0;
    }
};

template <class TType>
inline bool IsNegative(TType value) {
    return TInteger<TTypeTraits<TType>::IsUnsignedInt>::IsNegative(value);
}

template <class TSmallInt, class TLargeInt>
TSmallInt IntegerCast(TLargeInt largeInt) {
    TSmallInt smallInt = TSmallInt(largeInt);

    if (TTypeTraits<TSmallInt>::IsSignedInt && TTypeTraits<TLargeInt>::IsUnsignedInt) {
        if (IsNegative(smallInt) && largeInt > 0) {
            warn("IntegerCast: conversion from '%s' to '%s', positive value converted to negative",
                typeid(TLargeInt).name(), typeid(TSmallInt).name());
            abort();
        }
    }

    if (TLargeInt(smallInt) != largeInt) {
        warn("IntegerCast: conversion from '%s' to '%s', loss of data",
            typeid(TLargeInt).name(), typeid(TSmallInt).name());
        abort();
    }

    return smallInt;
}
