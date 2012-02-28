#include "alloc.h"
#include "mmapalloc.h"

#include <util/system/filemap.h>
#include <util/generic/singleton.h>

namespace {
    class TMmapAllocator: public IAllocator {
        public:
            virtual TBlock Allocate(size_t len) {
                TMappedAllocation m(len + sizeof(TMappedAllocation));
                TMappedAllocation* real = (TMappedAllocation*)m.Data();

                (new (real) TMappedAllocation(0))->swap(m);

                TBlock ret = {real + 1, len};

                return ret;
            }

            virtual void Release(const TBlock& block) {
                TMappedAllocation tmp(0);
                TMappedAllocation* real = ((TMappedAllocation*)block.Data) - 1;

                real->swap(tmp);
                real->~TMappedAllocation();
            }
    };
}

IAllocator* MmapAllocator() {
    return Singleton<TMmapAllocator>();
}
