#pragma once

#include <stlport/list>

#include <util/memory/alloc.h>

template <class T, class A = DEFAULT_ALLOCATOR(T)>
class ylist: public NStl::list<T, REBIND_ALLOCATOR(A, T)> {
    public:
        typedef NStl::list<T, REBIND_ALLOCATOR(A, T)> TBase;
        typedef typename TBase::size_type size_type;

        inline ylist()
            : TBase()
        {
        }

        inline ylist(size_type n, const T& val)
            : TBase(n, val)
        {
        }

        inline ylist(const typename TBase::allocator_type& a)
            : TBase(a)
        {
        }
};
