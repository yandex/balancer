#include "mutex.h"

#include <util/generic/yexception.h>

#if defined (_mt_)

#if defined(_win_)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#else
    #include <pthread.h>
#endif

class TSysMutex::TImpl {
    public:
        inline TImpl() {
#if defined (_win_)
            InitializeCriticalSection(&Obj);
#else
            struct T {
                pthread_mutexattr_t Attr;

                inline T() {
                    if (pthread_mutexattr_init(&Attr)) {
                        ythrow yexception() << "mutexattr init failed(" << LastSystemErrorText() << ")";
                    }
                    if (pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE)) {
                        ythrow yexception() << "mutexattr set type failed";
                    }

                }

                inline ~T() throw () {
                    if (pthread_mutexattr_destroy(&Attr)) {
                        YASSERT(0 && "mutexattr destroy");
                    }
                }
            } pma;

            if (pthread_mutex_init(&Obj, &pma.Attr)) {
                ythrow yexception() << "mutex init failed(" << LastSystemErrorText() << ")";
            }
#endif
        }

        inline ~TImpl() throw () {
#if defined (_win_)
            DeleteCriticalSection(&Obj);
#else
            if (pthread_mutex_destroy(&Obj)) {
                YASSERT(0 && "mutex destroy failure");
            }
#endif
        }

        inline void Acquire() throw () {
#if defined (_win_)
            EnterCriticalSection(&Obj);
#else
            if (pthread_mutex_lock(&Obj)) {
                YASSERT(0 && "mutex lock failure");
            }
#endif
        }

#if defined (_win_)
        static bool TryEnterCriticalSectionInt(CRITICAL_SECTION* obj) {
#if(_WIN32_WINNT < 0x0400)
            if (-1L == ::InterlockedCompareExchange(&obj->LockCount, 0, -1)) {
                obj->OwningThread = (HANDLE)(DWORD_PTR)::GetCurrentThreadId();
                obj->RecursionCount = 1;

                return true;
            }

            if (obj->OwningThread == (HANDLE)(DWORD_PTR)::GetCurrentThreadId()) {
                ::InterlockedIncrement(&obj->LockCount);
                ++obj->RecursionCount;
                return true;
            }

            return false;
#else // _WIN32_WINNT < 0x0400
            return TryEnterCriticalSection(obj);
#endif // _WIN32_WINNT < 0x0400
        }
#endif // _win_

        inline bool TryAcquire() throw () {
#if defined (_win_)
            return TryEnterCriticalSectionInt(&Obj);
#else
            return pthread_mutex_trylock(&Obj) == 0;
#endif
        }

        inline void Release() throw () {
#if defined (_win_)
            LeaveCriticalSection(&Obj);
#else
            if (pthread_mutex_unlock(&Obj)) {
                YASSERT(0 && "mutex unlock failure");
            }
#endif
        }

        inline void* Handle() const throw () {
            return (void*)&Obj;
        }

    private:
#ifdef _win_
        CRITICAL_SECTION Obj;
#else
        pthread_mutex_t Obj;
#endif
};

TSysMutex::TSysMutex()
    : Impl_(new TImpl())
{
}

TSysMutex::~TSysMutex() throw () {
}

void TSysMutex::Acquire() throw () {
    Impl_->Acquire();
}

bool TSysMutex::TryAcquire() throw () {
    return Impl_->TryAcquire();
}

void TSysMutex::Release() throw () {
    Impl_->Release();
}

void* TSysMutex::Handle() const throw () {
    return Impl_->Handle();
}

#endif
