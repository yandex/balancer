#pragma once

#include <stlport/set>

#include <util/memory/alloc.h>
#include <util/str_stl.h>

template <class K, class L = TLess<K>, class A = DEFAULT_ALLOCATOR(K)>
class yset: public NStl::set<K, L, REBIND_ALLOCATOR(A, K)> {
        typedef NStl::set<K, L, REBIND_ALLOCATOR(A, K)> TBase;
    public:
        inline yset() {
        }

        template <class It>
        inline yset(It f, It l)
            : TBase(f, l)
        {
        }
};

template <class K, class L = TLess<K>, class A = DEFAULT_ALLOCATOR(K)>
class ymultiset: public NStl::multiset<K, L, REBIND_ALLOCATOR(A, K)> {
        typedef NStl::multiset<K, L, REBIND_ALLOCATOR(A, K)> TBase;
    public:
        inline ymultiset() {
        }

        template <class It>
        inline ymultiset(It f, It l)
            : TBase(f, l)
        {
        }
};
