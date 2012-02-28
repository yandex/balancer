#pragma once

extern "C" {
    #include <util/private/jalloc/hack.h>
    #include <util/private/jalloc/spinlock.h>
}

#define SPINLOCK_INITIALIZER { _SPINLOCK_INITIALIZER }

struct TCMalloc_SpinLock {
    volatile spinlock_t private_lockword_;

    inline void Init() throw () {
        private_lockword_ = _SPINLOCK_INITIALIZER;
    }

    inline void Finalize() throw () {
    }

    inline void Lock() throw () {
        _SPINLOCK(&private_lockword_);
    }

    inline void Unlock() throw () {
        _SPINUNLOCK(&private_lockword_);
    }
};

class TCMalloc_SpinLockHolder {
    private:
        TCMalloc_SpinLock* lock_;

    public:
        inline explicit TCMalloc_SpinLockHolder(TCMalloc_SpinLock* l)
            : lock_(l)
        {
            l->Lock();
        }

        inline ~TCMalloc_SpinLockHolder() {
            lock_->Unlock();
        }
};

// Short-hands for convenient use by tcmalloc.cc
typedef TCMalloc_SpinLock SpinLock;
typedef TCMalloc_SpinLockHolder SpinLockHolder;
