#include "rwlock.h"

#ifdef _mt_
TRWMutex::TRWMutex()
    : State_(0)
    , BlockedWriters_(0)
{
}

void TRWMutex::AcquireRead() {
    {
        TGuard<TMutex> g(Lock_);

        while (BlockedWriters_ || State_ < 0) {
            ReadCond_.Wait(Lock_);
        }

        State_++;
    }

    ReadCond_.Signal();
}

bool TRWMutex::TryAcquireRead() {
    TGuard<TMutex> g(Lock_);

    if (BlockedWriters_ || State_ < 0) {
        return false;
    }

    State_++;

    return true;
}

void TRWMutex::ReleaseRead() {
    Lock_.Acquire();

    if (--State_ > 0) {
        Lock_.Release();
    } else if (BlockedWriters_) {
        Lock_.Release();
        WriteCond_.Signal();
    } else {
        Lock_.Release();
    }
}

void TRWMutex::AcquireWrite() {
    TGuard<TMutex> g(Lock_);

    while (State_ != 0) {
        ++BlockedWriters_;
        WriteCond_.Wait(Lock_);
        --BlockedWriters_;
    }

    State_ = -1;
}

bool TRWMutex::TryAcquireWrite() {
    TGuard<TMutex> g(Lock_);

    if (State_ != 0) {
        return false;
    }

    State_ = -1;

    return true;
}

void TRWMutex::ReleaseWrite() {
    Lock_.Acquire();
    State_ = 0;

    if (BlockedWriters_) {
        Lock_.Release();
        WriteCond_.Signal();
    } else {
        Lock_.Release();
        ReadCond_.Signal();
    }
}

void TRWMutex::Release() {
    Lock_.Acquire();

    if (State_ > 0) {
        if (--State_ > 0) {
            Lock_.Release();
        } else if (BlockedWriters_) {
            Lock_.Release();
            WriteCond_.Signal();
        }
    } else {
        State_ = 0;

        if (BlockedWriters_) {
            Lock_.Release();
            WriteCond_.Signal();
        } else {
            Lock_.Release();
            ReadCond_.Signal();
        }
    }
}
#endif
