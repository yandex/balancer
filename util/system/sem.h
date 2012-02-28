#pragma once

#include "defaults.h"

#include <util/generic/ptr.h>

class TSemaphore {
    public:
        TSemaphore(const char* name, ui32 maxFreeCount);
        ~TSemaphore() throw ();

        //Increase the semaphore counter.
        //Returns 'true' if the semaphore counter increased
        bool Release();

        //Keep a thread held while the semaphore counter is equal 0.
        bool Acquire();

        //Try to enter the semaphore gate. A non-blocking variant of Acquire.
        //Returns 'true' if the semaphore counter decreased
        bool TryAcquire();

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};
