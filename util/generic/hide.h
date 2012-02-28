#pragma once

#include "static_assert.h"

template<class T, size_t Sz, class TAligner = size_t>
struct THiddenImpl {
    THiddenImpl() {
        STATIC_ASSERT(Sz >= sizeof(T));
        new(DataImpl) T;
    }
    ~THiddenImpl() {
        Obj().~T();
    }
    FORCED_INLINE T &Obj() {
        return (T&)*DataImpl;
    }
    FORCED_INLINE const T &Obj() const {
        return (const T&)*DataImpl;
    }
private:
    union {
        char DataImpl[Sz];
        TAligner A;
    };
};
