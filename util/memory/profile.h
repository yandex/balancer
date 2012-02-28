#pragma once

#include <util/system/defaults.h>

template <class C>
class TAllocatorHooksImpl {
    public:
        typedef void (*TDelHook)(void* ptr);
        typedef void (*TNewHook)(void* ptr, size_t len);

        static inline TNewHook NewHook() throw () {
            return New_;
        }

        static inline TDelHook DelHook() throw () {
            return Del_;
        }

        static inline void SetNewHook(TNewHook hook) throw () {
            New_ = hook;
        }

        static inline void SetDelHook(TDelHook hook) throw () {
            Del_ = hook;
        }

    private:
        static TNewHook New_;
        static TDelHook Del_;
};

template <class C>
typename TAllocatorHooksImpl<C>::TNewHook TAllocatorHooksImpl<C>::New_ = 0;

template <class C>
typename TAllocatorHooksImpl<C>::TDelHook TAllocatorHooksImpl<C>::Del_ = 0;

#define TAllocatorHooks TAllocatorHooksImpl<int>

void InitDebugAllocator();
