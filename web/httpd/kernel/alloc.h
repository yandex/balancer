#pragma once

#include <util/memory/smallobj.h>
#include <util/memory/mmapalloc.h>
#include <util/generic/vector.h>
#include <util/generic/ptr.h>
#include <util/generic/bitops.h>

namespace NSrvKernel {
    struct TMemRange {
        inline TMemRange(void* ptr, size_t len)
            : Mem(ptr)
            , Range(len)
        {
        }

        void* Mem;
        size_t Range;
    };

    class TAllocator: public TMemoryPool::TExpGrow {
        typedef TFixedSizeAllocator TAlloc;
    public:
        inline TMemRange Alloc(size_t len) {
            TAlloc* a = Allocator(FastClp2(len + sizeof(TAlloc*)));
            TAlloc** ret = (TAlloc**)a->Allocate();

            *ret = a;

            return TMemRange(ret + 1, a->Size() - sizeof(TAlloc*));
        }

        inline void Free(void* ptr) throw () {
            TAlloc** a = (TAlloc**)ptr - 1;

            (*a)->Release(a);
        }

        inline TAlloc* Allocator(size_t len) {
            if (len >= B_.size()) {
                B_.resize(len + 1);
            }

            TRef& ret = B_[len];

            if (!ret) {
                ret = new TAlloc(len, this, MmapAllocator());
            }

            return ret.Get();
        }

    private:
        typedef TAutoPtr<TAlloc> TRef;
        yvector<TRef> B_;
    };

    extern TAllocator ALLOCATOR;

    static inline TMemRange AllocMem(size_t len) {
        return ALLOCATOR.Alloc(len);
    }

    static inline void FreeMem(void* ptr) throw () {
        ALLOCATOR.Free(ptr);
    }

    struct TSmallObj {
        inline void* operator new(size_t len) {
            return AllocMem(len).Mem;
        }

        inline void operator delete(void* ptr, size_t) throw () {
            FreeMem(ptr);
        }
    };

    template <class T>
    struct TBigObj {
        inline size_t AdditionalLength() const throw () {
            return *(((const size_t*)(const void*)static_cast<const T*>(this)) - 1);
        }

        inline void* AdditionalData() const throw () {
            return (void*)(static_cast<const T*>(this) + 1);
        }

        inline void* operator new(size_t len, size_t add) {
            const size_t overHead = len + sizeof(size_t);
            TMemRange range = AllocMem(add + overHead);
            size_t* ret = (size_t*)range.Mem;

            *ret = range.Range - overHead;

            return ret + 1;
        }

        inline void operator delete(void* ptr) throw () {
            Del(ptr);
        }

        inline void operator delete(void* ptr, size_t) throw () {
            Del(ptr);
        }

        inline void operator delete(void* ptr, size_t, size_t) throw () {
            Del(ptr);
        }

        static inline void Del(void* ptr) throw () {
            FreeMem((size_t*)ptr - 1);
        }
    };

    struct TFreeRange {
        static inline void Destroy(void* ptr) throw () {
            FreeMem(ptr);
        }
    };
}
