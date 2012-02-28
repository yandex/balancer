#pragma once

#include "addstorage.h"

#include <util/generic/ptr.h>
#include <util/generic/intrlist.h>

class TMemoryGc {
        class TAllocation: public TIntrusiveListItem<TAllocation>
                         , public TAdditionalStorage<TAllocation> {
        };

    public:
        inline void* Allocate(size_t len) {
            TAllocation* ret = new (len) TAllocation;

            Allocs_.PushBack(ret);

            return ret->AdditionalData();
        }

        inline void Deallocate(void* ptr) throw () {
            if (ptr) {
                delete TAllocation::ObjectFromData(ptr);
            }
        }

    private:
        TIntrusiveListWithAutoDelete<TAllocation, TDelete> Allocs_;
};
