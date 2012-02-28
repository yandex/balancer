#pragma once

#include "guard.h"
#include "mutex.h"
#include "condvar.h"
#include "defaults.h"

#ifdef _mt_

class TRWMutex {
    public:
        TRWMutex();

        void AcquireRead();
        bool TryAcquireRead();
        void ReleaseRead();

        void AcquireWrite();
        bool TryAcquireWrite();
        void ReleaseWrite();

        void Release();

    private:
        TMutex Lock_;
        int State_;
        TCondVar ReadCond_;
        TCondVar WriteCond_;
        int BlockedWriters_;
};

#else

class TRWMutex {
    public:
        void AcquireRead() {
        }

        bool TryAcquireRead() {
            return true;
        }

        void ReleaseRead() {
        }

        void AcquireWrite() {
        }

        bool TryAcquireWrite() {
            return true;
        }

        void ReleaseWrite() {
        }

        void Release() {
        }
};

#endif

struct TRWMutexReadOps {
     static inline void Acquire(TRWMutex* t) {
         t->AcquireRead();
     }

     static inline void Release(TRWMutex* t) {
         t->ReleaseRead();
     }
};

struct TRWMutexWriteOps {
     static inline void Acquire(TRWMutex* t) {
         t->AcquireWrite();
     }

     static inline void Release(TRWMutex* t) {
         t->ReleaseWrite();
     }
};

typedef TGuard<TRWMutex, TRWMutexReadOps> TReadGuard;
typedef TGuard<TRWMutex, TRWMutexWriteOps> TWriteGuard;
