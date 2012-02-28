#pragma once

#include <stlport/utility>
#include <util/system/defaults.h>

// dirty hack for old compilers
template<class T1, class T2>
struct TPairOfRefs {
    const T1 &first;
    const T2 &second;
    TPairOfRefs(const T1& x, const T2& y)
        : first(x)
        , second(y)
    {
    }
};

template <class T1, class T2 = T1>
struct TPair : NStl::pair<T1, T2> {
    typedef T1 TFirst;
    typedef T2 TSecond;

    TPair() {
    }

    TPair(const T1& x, const T2& y)
        : NStl::pair<T1, T2> (x, y)
    {
    }

    TPair(NStl::__move_source<TPair<T1, T2> > src)
        : NStl::pair<T1, T2> (src)
    {
    }

    TPair(NStl::__move_source<NStl::pair<T1, T2> > src)
        : NStl::pair<T1, T2> (src)
    {
    }

    template <class TU, class TV>
    TPair(const NStl::pair<TU, TV>& p)
        : NStl::pair<T1, T2>(p)
    {
    }

    template <class TU, class TV>
    TPair(const TPair<TU, TV>& p)
        : NStl::pair<T1, T2>(p)
    {
    }

    template <class TU, class TV>
    TPair(const TPairOfRefs<TU, TV>& p)
        : NStl::pair<T1, T2>(p)
    {
    }

    TFirst& First() {
        return this->first;
    }

    TSecond& Second() {
        return this->second;
    }

    const TFirst& First() const {
        return this->first;
    }

    const TSecond& Second() const {
        return this->second;
    }
};

template<class T1, class T2>
inline TPair<T1, T2> MakePair(T1 v1, T2 v2) {// return pair composed from arguments
    return (TPair<T1, T2>(v1, v2));
}

template<class T>
inline TPair<T, T> MakePair(T v) {// return pair composed from argument
    return (TPair<T, T>(v, v));
}
