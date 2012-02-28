#pragma once

#include "typelist.h"
#include "typehelpers.h"

#include <util/system/defaults.h>

template <bool, class>
struct TEnableIf {
};

template <class T>
struct TEnableIf<true, T> {
    typedef T TResult;
};

template <class T>
struct TPodTraits {
    enum {
        IsPod = false
    };
};

template <class T>
struct TToUnsignedInt {
    typedef T TResult;
};

template <class T>
struct TToSignedInt {
    typedef T TResult;
};

template <class T>
class TTypeTraits {
        /*
         * some helpers
         */
        template <class C>
        struct TPointerTraits {
            enum {
                IsPointer = false
            };

            typedef C TPointTo;
        };

        template <class C>
        struct TPointerTraits<C*> {
            enum {
                IsPointer = true
            };

            typedef C TPointTo;
        };

        template <class C>
        struct TReferenceTraits {
            enum {
                IsReference = false
            };

            typedef C TReferenceTo;
        };

        template <class C>
        struct TReferenceTraits<C&> {
            enum {
                IsReference = true
            };

            typedef C TReferenceTo;
        };

        template <class C>
        struct TConstTraits {
            enum {
                IsConstant = false
            };

            typedef C TResult;
        };

        template <class C>
        struct TConstTraits<const C> {
            enum {
                IsConstant = true
            };

            typedef C TResult;
        };

        template <class C>
        struct TVolatileTraits {
            enum {
                IsVolatile = false
            };

            typedef C TResult;
        };

        template <class C>
        struct TVolatileTraits<volatile C> {
            enum {
                IsVolatile = true
            };

            typedef C TResult;
        };

        template <class C>
        struct TArrayTraits {
            enum {
                IsArray = false
            };

            typedef C TResult;
        };

        template <class C, size_t n>
        struct TArrayTraits<C[n]> {
            enum {
                IsArray = true
            };

            typedef C TResult;
        };

        template <class C>
        struct TPointerToMemberTraits {
            enum {
                IsPointerToMember = false
            };
        };

        template <class C, class M>
        struct TPointerToMemberTraits<C M::*> {
            enum {
                IsPointerToMember = true
            };
        };

        template <class C, bool isConst, bool isVolatile>
        struct TApplyQualifiers {
            typedef C TResult;
        };

        template <class C>
        struct TApplyQualifiers<C, false, true> {
            typedef volatile C TResult;
        };

        template <class C>
        struct TApplyQualifiers<C, true, false> {
            typedef const C TResult;
        };

        template <class C>
        struct TApplyQualifiers<C, true, true> {
            typedef const volatile C TResult;
        };

    public:
        /*
         * qualifier traits
         */
        enum {
            IsConstant = TConstTraits<T>::IsConstant
        };

        enum {
            IsVolatile = TVolatileTraits<T>::IsVolatile
        };

        enum {
            IsPointer = TPointerTraits<T>::IsPointer
        };

        enum {
            IsPointerToMember = TPointerToMemberTraits<T>::IsPointerToMember
        };

        enum {
            IsReference = TReferenceTraits<T>::IsReference
        };

        enum {
            IsConstReference = IsConstant && IsReference
        };

        enum {
            IsArray = TArrayTraits<T>::IsArray
        };

        /*
         * type without 'const' qualifier
         */
        typedef typename TConstTraits<T>::TResult TNonConst;

        /*
         * type without 'volatile' qualifier
         */
        typedef typename TVolatileTraits<T>::TResult TNonVolatile;

        /*
         * type without qualifiers
         */
        typedef typename TConstTraits<TNonVolatile>::TResult TNonQualified;

        /*
         * T = [const, volatile] TReferenceTo&
         */
        typedef typename TReferenceTraits<T>::TReferenceTo TReferenceTo;

        /*
         * T = [const, volatile] TPointTo*
         */
        typedef typename TPointerTraits<T>::TPointTo TPointTo;

        /*
         * int traits
         */
        enum {
            IsSignedInt = TSignedInts::THave<TNonQualified>::Result
        };

        enum {
            IsUnsignedInt = TUnsignedInts::THave<TNonQualified>::Result
        };

        /*
         * float traits
         */
        enum {
            IsFloat = TFloats::THave<TNonQualified>::Result
        };

        /*
         * numerical traits
         */
        enum {
            IsIntegral = IsSignedInt || IsUnsignedInt
        };

        enum {
            IsArithmetic = IsIntegral || IsFloat
        };

        /*
         * traits too
         */
        enum {
            IsFundamental = IsArithmetic || TSameType<TNonQualified, void>::Result
        };

        enum {
            IsPrimitive = IsArithmetic || IsPointer || IsPointerToMember
        };

        enum {
            IsPod = TPodTraits<TNonQualified>::IsPod || IsPrimitive
        };

        enum {
            IsClassType = !IsPrimitive
        };

        /*
         * can be effectively passed to function as value
         */
        enum {
            IsValueType = IsPrimitive || IsReference || (IsPod && sizeof(T) <= sizeof(void*))
        };

        /*
         * can be used in function templates for effective parameters passing
         */
        typedef typename TSelectType<IsValueType, T, const TReferenceTo&>::TResult TFuncParam;

        /*
         * signed type to unsigned type conversion
         */
        typedef typename TToUnsignedInt<TNonQualified>::TResult TUnsignedTypeHelper;
        typedef typename TApplyQualifiers<TUnsignedTypeHelper, IsConstant, IsVolatile>::TResult TUnsigned;

        /*
         * unsigned type to signed type conversion
         */
        typedef typename TToSignedInt<TNonQualified>::TResult TSignedTypeHelper;
        typedef typename TApplyQualifiers<TSignedTypeHelper, IsConstant, IsVolatile>::TResult TSigned;
};

#define DECLARE_PODTYPE(type) \
    template <>\
    struct TPodTraits<type> {\
        enum {\
            IsPod = true\
        };\
    };

#define DEF_INT_CVT(a)\
    template <>\
    struct TToUnsignedInt<a> {\
        typedef unsigned a TResult;\
    };\
    \
    template <>\
    struct TToSignedInt<unsigned a> {\
        typedef a TResult;\
    }

DEF_INT_CVT(char);
DEF_INT_CVT(short);
DEF_INT_CVT(int);
DEF_INT_CVT(long);
DEF_INT_CVT(long long);

#undef DEF_INT_CVT

#define HAS_MEMBER(name)\
    template <class T>\
    struct TClassHas ## name {\
        struct TBase {\
            void name();\
        };\
        class THelper: public T, public TBase {\
            public:\
                template <class T1>\
                inline THelper(const T1& = T1()) {\
                }\
        };\
        template <class T1, T1 val>\
        class TChecker {};\
        struct TNo {\
            char ch;\
        };\
        struct TYes {\
            char arr[2];\
        };\
        template <class T1>\
        static TNo CheckMember(T1*, TChecker<void (TBase::*)(), &T1::name>* = 0);\
        static TYes CheckMember(...);\
        enum {\
            Result = (sizeof(TYes) == sizeof(CheckMember((THelper*)0)))\
        };\
    };\
    template <class T, bool isClassType>\
    struct TBaseHas ## name {\
        enum {\
            Result = false\
        };\
    };\
    template <class T>\
    struct TBaseHas ## name<T, true>: public TClassHas ## name<T> {\
    };\
    template <class T>\
    struct THas ## name: public TBaseHas ## name<T, TTypeTraits<T>::IsClassType> {\
    };
