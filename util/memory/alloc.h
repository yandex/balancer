#pragma once

#include <stlport/memory>

#include <cstdlib>

extern void ThrowBadAlloc();

inline void* y_allocate(size_t n) {
    void* r = malloc(n);

    if (r == 0) {
        ThrowBadAlloc();
    }

    return r;
}

inline void y_deallocate(void* p) {
    free(p);
}

inline void* y_reallocate(void* p, size_t new_sz) {
    void* r = realloc(p, new_sz);

    if (r == 0) {
        ThrowBadAlloc();
    }

    return r;
}

#define __STL_DEFAULT_ALLOCATOR(T) NStl::allocator<T >
#define DEFAULT_ALLOCATOR(T) __STL_DEFAULT_ALLOCATOR(T)
#define REBIND_ALLOCATOR(A, T) typename A::template rebind<T >::other
#define FIX2ARG(x, y) x, y

class IAllocator {
    public:
        struct TBlock {
            void* Data;
            size_t Len;
        };

        virtual ~IAllocator() {
        }

        virtual TBlock Allocate(size_t len) = 0;
        virtual void Release(const TBlock& block) = 0;
};

class TDefaultAllocator: public IAllocator {
    public:
        virtual TBlock Allocate(size_t len) {
            TBlock ret = {y_allocate(len), len};

            return ret;
        }

        virtual void Release(const TBlock& block) {
            y_deallocate(block.Data);
        }

        static IAllocator* Instance() throw ();
};
