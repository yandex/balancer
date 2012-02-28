#define FROM_IMPL_CPP
#include "impl.h"

#include <util/generic/yexception.h>

#if defined(_linux_)
    #if (_POSIX_C_SOURCE >= 200809) && defined(SOCK_NONBLOCK)
        #define SYS_ACCEPT4
    #endif
#endif

namespace {
    struct TFeatureCheck {
        inline TFeatureCheck()
            : HaveFastAccept(false)
            , HaveSockNonBlock(false)
        {
#if defined(SYS_ACCEPT4)
            {
                accept4(-1, 0, 0, SOCK_NONBLOCK);
                HaveFastAccept = (errno != ENOSYS);
            }
#endif

#if defined(SOCK_NONBLOCK)
            {
                TSocketHolder tmp(socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0));

                HaveSockNonBlock = !tmp.Closed();
            }
#endif
        }

        static inline const TFeatureCheck* Instance() throw () {
            return Singleton<TFeatureCheck>();
        }

        bool HaveFastAccept;
        bool HaveSockNonBlock;
    };
}

SOCKET Accept4(SOCKET s, struct sockaddr* addr, socklen_t* addrlen) {
#if defined(SYS_ACCEPT4)
    if (TFeatureCheck::Instance()->HaveFastAccept) {
        return accept4(s, addr, addrlen, SOCK_NONBLOCK);
    }
#endif

    const SOCKET ret = accept(s, addr, addrlen);

#if !defined(_freebsd_)
    //freebsd inherit O_NONBLOCK flag
    if (ret != INVALID_SOCKET) {
        SetNonBlock(ret);
    }
#endif

    return ret;
}

SOCKET Socket4(int domain, int type, int protocol) {
#if defined(SOCK_NONBLOCK)
    if (TFeatureCheck::Instance()->HaveSockNonBlock) {
        return socket(domain, type | SOCK_NONBLOCK, protocol);
    }
#endif

    const SOCKET ret = socket(domain, type, protocol);

    if (ret == INVALID_SOCKET) {
        return ret;
    }

    SetNonBlock(ret);

    return ret;
}

template <>
void Out<TCont>(TOutputStream& out, const TCont& c) {
    c.PrintMe(out);
}

template <>
void Out<TContRep>(TOutputStream& out, const TContRep& c) {
    c.ContPtr()->PrintMe(out);
}

void TContRep::DoRun() {
    try {
        DBGOUT("execute");
        ContPtr()->Execute();
    } catch (...) {
        try {
            DBGOUT(CurrentExceptionMessage());
        } catch (...) {
        }

        VERIFY(!ContPtr()->Executor()->FailOnError(), "uncaught exception");
    }

    ContPtr()->Executor()->Exit(this);
}

void TContRep::Construct(TContExecutor* executor, TContFunc func, void* arg, const char* name) {
    TContClosure closure = {
          this
        , stack
    };

    THolder<TContMachineContext, TDestructor> mc(new (MachinePtr()) TContMachineContext(closure));

    new (ContPtr()) TCont(executor, this, func, arg, name);
    mc.Release();
}

void TContRep::Destruct() throw () {
    ContPtr()->~TCont();
    MachinePtr()->~TContMachineContext();
}

#if defined(_unix_)
    #include <sys/mman.h>
#endif

void TProtectedContStackAllocator::Protect(void* ptr, size_t len) throw () {
#if defined(_unix_)
    if (mprotect(ptr, len, PROT_NONE)) {
        abort();
    }
#else
    UNUSED(ptr);
    UNUSED(len);
#endif
}

void TProtectedContStackAllocator::UnProtect(void* ptr, size_t len) throw () {
#if defined(_unix_)
    if (mprotect(ptr, len, PROT_READ | PROT_WRITE | PROT_EXEC)) {
        abort();
    }
#else
    UNUSED(ptr);
    UNUSED(len);
#endif
}

void TContExecutor::WaitForIO() {
    while (Ready_.Empty() && !WaitQueue_.Empty()) {
        {
            const TInstant next = WaitQueue_.CancelTimedOut(TInstant::Now());

            if (!Poller_.Wait(Events_, next)) {
                WaitQueue_.CancelTimedOut(next);
            }
        }

        for (TEvents::const_iterator ev = Events_.begin(); ev != Events_.end(); ++ev) {
            TPollEventList* lst = (TPollEventList*)ev->Data;
            const int status = ev->Status;

            if (status) {
                for (TPollEventList::TIterator it = lst->Begin(); it != lst->End();) {
                    (it++)->OnPollEvent(status);
                }
            } else {
                const ui16 filter = ev->Filter;

                for (TPollEventList::TIterator it = lst->Begin(); it != lst->End();) {
                    if (it->What() & filter) {
                        (it++)->OnPollEvent(status);
                    } else {
                        ++it;
                    }
                }
            }
        }
    }
}
