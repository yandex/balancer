#pragma once

#include "pair.h"

#include <stlport/map>

#include <util/memory/alloc.h>
#include <util/str_stl.h>

template <class K, class V, class Less = TLess<K>, class A = DEFAULT_ALLOCATOR(K)>
class ymap: public NStl::map<K, V, Less, REBIND_ALLOCATOR(A, FIX2ARG(TPair<K, V>))> {
        typedef NStl::map<K, V, Less, REBIND_ALLOCATOR(A, FIX2ARG(TPair<K, V>))> TBase;
    public:
        inline ymap() {
        }

        template <class It>
        inline ymap(It f, It l)
            : TBase(f, l)
        {
        }
};

template <class K, class V, class Less = TLess<K>, class A = DEFAULT_ALLOCATOR(K)>
class ymultimap: public NStl::multimap<K, V, Less, REBIND_ALLOCATOR(A, FIX2ARG(TPair<K, V>))> {
        typedef NStl::multimap<K, V, Less, REBIND_ALLOCATOR(A, FIX2ARG(TPair<K, V>))> TBase;
    public:
        inline ymultimap() {
        }

        template <class It>
        inline ymultimap(It f, It l)
            : TBase(f, l)
        {
        }
};
