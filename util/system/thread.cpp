#include "tls.h"
#include "thread.h"
#include "thread.i"

#include <cstdio>
#include <cstdlib>

#include <util/generic/map.h>
#include <util/generic/ptr.h>
#include <util/generic/pair.h>
#include <util/generic/ymath.h>
#include <util/generic/ylimits.h>
#include <util/generic/yexception.h>

class TThread::TImpl {
    public:
        inline TImpl(TThreadProc threadProc, void* param, size_t stackSize) {
            Handle = 0;
            ThreadProc = threadProc;
            Param = param;
            StackSize = FastClp2(stackSize);
#if defined(_win_) && (_WIN32_WINNT < 0x0502)
            ThreadId = 0;
#endif
        }

        inline ~TImpl() {
            if (Handle) {
                Join();
            }
        }

        inline bool Running() const throw () {
            return Handle != 0;
        }

#if defined(_mt_)
        inline TId SystemThreadId() const throw () {
#if defined(_win_)
    #if (_WIN32_WINNT < 0x0502)
            return (TId)ThreadId;
    #else
            return (TId)GetThreadId(Handle);
    #endif
#else
            return (TId)Handle;
#endif
        }

        inline void* Join() {
            if (!Handle) {
                return 0;
            }

            void* tec = (void*)-1;

#if defined(_win_)
            ::WaitForSingleObject(Handle, INFINITE);
            ::GetExitCodeThread(Handle, (unsigned long*)&tec);
            ::CloseHandle(Handle);
#else
            pthread_join(Handle, &tec);
#endif

            Handle = 0;
#if defined(_win_) && (_WIN32_WINNT < 0x0502)
            ThreadId = 0;
#endif

            return tec;
        }

        inline void Detach() {
            if (!Handle) {
                return;
            }

#if defined(_win_)
            ::CloseHandle(Handle);
#else
            pthread_detach(Handle);
#endif

            Handle = 0;
#if defined(_win_) && (_WIN32_WINNT < 0x0502)
            ThreadId = 0;
#endif
        }

#if defined(_win_)
        typedef TPair<TThreadProc, void*> TArgs;
        static ui32 __stdcall Proxy(void* ptr) {
            YASSERT(ptr);
            NTls::TCleaner cleaner;
            THolder<TArgs> args(static_cast<TArgs*>(ptr));
            args->first(args->second);
            return 0;
        }

        inline void Start() {
            YASSERT(Handle == 0);
            THolder<TArgs> args(new TArgs(ThreadProc, Param));
#if (_WIN32_WINNT < 0x0502)
            Handle = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, (unsigned)StackSize, Proxy, (void*)args.Get(), 0, &ThreadId));
#else
            Handle = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, (unsigned)StackSize, Proxy, (void*)args.Get(), 0, NULL));
#endif
            if (Handle) {
                args.Release();
            } else {
                ythrow yexception() << "failed to create a thread";
            }
        }
#else
        inline void Start() {
            YASSERT(Handle == 0);

            pthread_attr_t* pattrs = 0;
            pthread_attr_t attrs;

            if (StackSize > 0) {
                pthread_attr_init(&attrs);

#if (!defined(__FreeBSD__) || (__FreeBSD__ >= 5))
                pattrs = &attrs;
                pthread_attr_setstacksize(pattrs, StackSize);
#endif
            }

            if (0 != pthread_create(&Handle, pattrs, ThreadProc, Param))
                ythrow yexception() << "failed to create thread: " << LastSystemErrorText();
        }
#endif
#else
        inline void* Join() {
            return 0;
        }

        inline void Detach() {
            return;
        }

        inline void Start() {
            YASSERT(Handle == 0);
            ThreadProc(Param);
        }

        inline TId SystemThreadId() const throw () {
            return 0;
        }
#endif

        inline TId Id() const throw () {
            return ThreadIdHashFunction(SystemThreadId());
        }

    private:
        TThreadProc ThreadProc;
        THREADHANDLE Handle;
        void* Param;
        size_t StackSize;
#if defined(_win_) && (_WIN32_WINNT < 0x0502)
        ui32 ThreadId;
#endif
};

TThread::TThread(const TParams& p)
    : Impl_(new TImpl(p.Proc, p.Data, p.StackSize))
{
}

TThread::TThread(TThreadProc threadProc, void* param)
    : Impl_(new TImpl(threadProc, param, 0))
{
}

TThread::~TThread() throw () {
}

void TThread::Start() {
    Impl_->Start();
}

void* TThread::Join() {
    return Impl_->Join();
}

void TThread::Detach() {
    Impl_->Detach();
}

bool TThread::Running() const throw () {
    return Impl_->Running();
}

TThread::TId TThread::Id() const throw () {
    return Impl_->Id();
}

TThread::TId TThread::CurrentThreadId() throw () {
    return SystemCurrentThreadId();
}

TThread::TId TThread::ImpossibleThreadId() throw () {
    return Max<TThread::TId>();
}
