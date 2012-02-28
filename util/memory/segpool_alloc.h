#pragma once

#include "segmented_string_pool.h"

/**
  This is a partially implemented allocator class that uses segmented_pool
  as its allocatior base.
  In fact, segpool_alloc can only allocate, it can't deallocate. Therefore,
  it should be used with add-only containers that are just clear()'ed in the
  end of their life cycle.

  External segmented_pool object must be cleared explicitly after all
  containers use it in their allocator are cleared.
  Single segmented_pool object may be used for several containers.

  Use segpool_alloc_vt as allocator template parameter for container.
  Example:
  typedef yhash<A, B, hash<A>, TEqualTo<A>, segpool_alloc_vt> TSomething;
  ...
  TSomething something;
  something.GetNodeAllocator().pool = &alloc_pool;

  Some containers may provide no way to access their allocator. In that case,
  if you want to use them with segpool_alloc_vt, you should first either
  hack them, or fix segpool_alloc so that it would create ref-counted
  segmented_pool object on it's own

  ! Only tested with yhash
*/

template <class _Tp>
struct segpool_alloc {
    typedef segmented_pool<char> pool_type;
    pool_type* pool;
    typedef _Tp *pointer;
    typedef const _Tp *const_pointer;
    typedef _Tp &reference;
    typedef const _Tp &const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef void        value_type;
#ifndef NDEBUG
    ui64 pool_count, malloc_count, pool_free_count, malloc_free_count;
#endif
    segpool_alloc() : pool(0) {
        IF_DEBUG(pool_count = malloc_count = pool_free_count = malloc_free_count = 0);
    }
    segpool_alloc(pool_type* p)
      : pool(p) {
        IF_DEBUG(pool_count = malloc_count = pool_free_count = malloc_free_count = 0);
    }
    segpool_alloc(const segpool_alloc& a)
      : pool(a.pool) {
        IF_DEBUG(pool_count = malloc_count = pool_free_count = malloc_free_count = 0);
    }
    template <class _Tp1>
    segpool_alloc(const segpool_alloc<_Tp1>& a)
      : pool(a.pool) {
        IF_DEBUG(pool_count = malloc_count = pool_free_count = malloc_free_count = 0);
     }
    _Tp *allocate(size_t __n) {
        if (!pool) {
            _Tp* data = (_Tp*)malloc(__n * sizeof(_Tp));
            IF_DEBUG(if (data) malloc_count++);
            return data;
        }
        _Tp* data = (_Tp*)pool->append(0, __n * sizeof(_Tp));
        IF_DEBUG(pool_count++);
        return data;
    }
    void deallocate(pointer __p, size_t /*__n*/) {
        if (!pool) {
            IF_DEBUG(malloc_free_count++);
            free(__p);
        } else {
            IF_DEBUG(pool_free_count++);
            ;
        }
    }
    ~segpool_alloc() {
        //assert(pool_count == pool_free_count && malloc_count == malloc_free_count); <- uncomment when swap() problem is solved
        //printf("in ~segpool_alloc: size = %u, pool_count = %"PRId64", malloc_count = %"PRId64", pool_free_count = %"PRId64", malloc_free_count = %"PRId64"\n",
        //        sizeof(_Tp), pool_count, malloc_count, pool_free_count, malloc_free_count);
        //fflush(stdout);
    }
    template <class _Tp1> struct rebind {
        typedef segpool_alloc<_Tp1> other;
    };
    size_type max_size() const {
        return size_type(-1) / sizeof(_Tp);
    }
    void construct(pointer __p, const _Tp& __val) { new(__p) _Tp(__val); }
    void destroy(pointer __p) { __p->~_Tp(); }
};

template <class _Tp>
inline bool operator==(const segpool_alloc<_Tp>& a1, const segpool_alloc<_Tp>& a2) {
    return a1.pool == a2.pool;
}

template <class _Tp>
inline bool operator!=(const segpool_alloc<_Tp>& a1, const segpool_alloc<_Tp>& a2) {
  return a1.pool != a2.pool;
}

struct segpool_alloc_vt {
    template <class _Tp1> struct rebind {
        typedef segpool_alloc<_Tp1> other;
    };
};
