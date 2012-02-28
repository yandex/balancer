#pragma once

#include <stlport/vector>

#include <util/memory/alloc.h>

template <class T, class A = DEFAULT_ALLOCATOR(T)>
class yvector: public NStl::vector<T, REBIND_ALLOCATOR(A, T)> {
    public:
        typedef NStl::vector<T, REBIND_ALLOCATOR(A, T)> TBase;
        typedef typename TBase::size_type size_type;

        inline yvector()
            : TBase()
        {
        }

        inline yvector(const typename TBase::allocator_type& a)
            : TBase(a)
        {
        }

        inline explicit yvector(size_type count)
            : TBase(count)
        {
        }

        inline yvector(size_type count, const T& val)
            : TBase(count, val)
        {
        }

        template <class TIter>
        inline yvector(TIter first, TIter last)
            : TBase(first, last)
        {
        }

        inline size_type operator+ () const throw () {
            return this->size();
        }

        inline T* operator~ () throw () {
            if (this->empty()) {
                return 0;
            }

            return &(*this->begin());
        }

        inline const T* operator~ () const throw () {
            if (this->empty()) {
                return 0;
            }

            return &(*this->begin());
        }

        inline int ysize() const throw () {
            return (int)TBase::size();
        }

        inline void shrink_to_fit() {
            yvector<T, A>(*this).swap(*this);
        }
};
