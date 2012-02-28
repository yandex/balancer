#pragma once

#include "typehelpers.h"
#include "static_assert.h"

/*
 * terminal type for typelists and typelist algorithms
 */
struct TNone {
    enum {
        Length = 0
    };

    template <class V>
    struct THave {
        enum {
            Result = false
        };
    };

    template <template <class> class P>
    struct TSelectBy {
        typedef TNone TResult;
    };
};

/*
 * type list, in all it's fame
 */
template <class H, class T>
struct TTypeList {
    typedef H THead;
    typedef T TTail;

    /*
     * length of type list
     */
    enum {
        Length = 1 + TTail::Length
    };

    template <class V>
    struct THave {
        enum {
            Result = TSameType<H, V>::Result || T::template THave<V>::Result
        };
    };

    template <template <class> class P>
    struct TSelectBy {
        typedef typename TSelectType<P<THead>::Result, THead, typename TTail::template TSelectBy<P>::TResult>::TResult TResult;
    };
};

/*
 * helper defines for easy typelist creation
 */
#define TYPELIST_1(T1) TTypeList<T1, TNone>
#define TYPELIST_2(T1, T2) TTypeList<T1, TYPELIST_1(T2) >
#define TYPELIST_3(T1, T2, T3) TTypeList<T1, TYPELIST_2(T2, T3) >
#define TYPELIST_4(T1, T2, T3, T4) TTypeList<T1, TYPELIST_3(T2, T3, T4) >
#define TYPELIST_5(T1, T2, T3, T4, T5) TTypeList<T1, TYPELIST_4(T2, T3, T4, T5) >
#define TYPELIST_6(T1, T2, T3, T4, T5, T6) TTypeList<T1, TYPELIST_5(T2, T3, T4, T5, T6) >
#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) TTypeList<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >
#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) TTypeList<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >
#define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) TTypeList<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9) >
#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) TTypeList<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >
#define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) TTypeList<T1, TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >
#define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) TTypeList<T1, TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) >
#define TYPELIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) TTypeList<T1, TYPELIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) >
#define TYPELIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) TTypeList<T1, TYPELIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) >

/*
 * some very common typelists
 */
typedef TYPELIST_5(signed char, signed short, signed int, signed long, signed long long) TCommonSignedInts;
typedef TYPELIST_6(unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long, bool) TCommonUnsignedInts;
typedef TYPELIST_3(float, double, long double) TFloats;

namespace NTL {
    template <bool isSigned, class T, class TS, class TU>
    struct TTypeSelectorBase {
        typedef TTypeList<T, TS> TSignedInts;
        typedef TU TUnsignedInts;
    };

    template <class T, class TS, class TU>
    struct TTypeSelectorBase<false, T, TS, TU> {
        typedef TS TSignedInts;
        typedef TTypeList<T, TU> TUnsignedInts;
    };

    template <class T, class TS, class TU>
    struct TTypeSelector: public TTypeSelectorBase<((T)(-1) < 0), T, TS, TU> {
    };

    typedef TTypeSelector<char, TCommonSignedInts, TCommonUnsignedInts> T1;
    typedef TTypeSelector<wchar_t, T1::TSignedInts, T1::TUnsignedInts> T2;
}

typedef NTL::T2::TSignedInts TSignedInts;
typedef NTL::T2::TUnsignedInts TUnsignedInts;

STATIC_ASSERT(TSignedInts::THave<char>::Result)

template <unsigned sizeOf>
struct TSizeOfPredicate {
    template <class T>
    struct TResult {
        enum {
            Result = (sizeof(T) == sizeOf)
        };
    };
};
