#pragma once

#include "poller.h"

#include <util/system/mutex.h>
#include <util/system/error.h>
#include <util/system/context.h>
#include <util/system/defaults.h>
#include <util/system/valgrind.h>

#include <util/memory/tempbuf.h>
#include <util/memory/addstorage.h>
#include <util/memory/smallobj.h>

#include <util/network/socket.h>

#include <util/generic/ptr.h>
#include <util/generic/buffer.h>
#include <util/generic/vector.h>
#include <util/generic/rbtree.h>
#include <util/generic/utility.h>
#include <util/generic/intrlist.h>
#include <util/generic/yexception.h>

#include <util/datetime/base.h>

class TCont;
struct TContRep;
class TContEvent;
class TContExecutor;
class TContPollEvent;

typedef void (*TContFunc)(TCont*, void*);

#include "iostatus.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4265) // class has virtual functions, but destructor is not virtual
#endif

#if defined(DEBUG_CONT)
    #define DBGOUT(x) Cdbg << x << Endl
#else
    #define DBGOUT(x)
#endif

//assume s is non-blocking
SOCKET Accept4(SOCKET s, struct sockaddr* addr, socklen_t* addrlen);
SOCKET Socket4(int domain, int type, int protocol);

struct TContPollEventCompare {
    template <class T>
    static inline bool Compare(const T& l, const T& r) throw () {
        return l.DeadLine() < r.DeadLine() || (l.DeadLine() == r.DeadLine() && &l < &r);
    }
};

class TContPollEvent: public TRbTreeItem<TContPollEvent, TContPollEventCompare> {
public:
    inline TContPollEvent(TCont* cont, TInstant deadLine) throw ()
        : Cont_(cont)
        , DeadLine_(deadLine)
        , Status_(EINPROGRESS)
    {
    }

    inline int Status() const throw () {
        return Status_;
    }

    inline void SetStatus(int status) throw () {
        Status_ = status;
    }

    inline TCont* Cont() throw () {
        return Cont_;
    }

    inline TInstant DeadLine() const throw () {
        return DeadLine_;
    }

    inline void Wake(int status) throw () {
        SetStatus(status);
        Wake();
    }

private:
    inline void Wake() throw ();

private:
    TCont* Cont_;
    TInstant DeadLine_;
    int Status_;
};

template <class T>
inline int ExecuteEvent(T* event) throw ();

class IPollEvent: public TIntrusiveListItem<IPollEvent> {
public:
    inline IPollEvent(SOCKET fd, ui16 what)
        : Fd_(fd)
        , What_(what)
    {
    }

    inline SOCKET Fd() const throw () {
        return Fd_;
    }

    inline int What() const throw () {
        return What_;
    }

    virtual void OnPollEvent(int status) throw () = 0;

private:
    SOCKET Fd_;
    ui16 What_;
};

class TFdEvent: public TContPollEvent, public IPollEvent {
public:
    inline TFdEvent(TCont* cont, SOCKET fd, ui16 what, TInstant deadLine) throw ()
        : TContPollEvent(cont, deadLine)
        , IPollEvent(fd, what)
    {
    }

    inline ~TFdEvent() throw () {
        RemoveFromIOWait();
    }

    inline void RemoveFromIOWait() throw ();

    virtual void OnPollEvent(int status) throw () {
        Wake(status);
    }
};

class TTimerEvent: public TContPollEvent {
public:
    inline TTimerEvent(TCont* cont, TInstant deadLine) throw ()
        : TContPollEvent(cont, deadLine)
    {
    }
};

class TContPollEventHolder {
    public:
        inline TContPollEventHolder(void* memory, TCont* rep, SOCKET fds[], int what[], size_t nfds, TInstant deadline)
            : Events_((TFdEvent*)memory)
            , Count_(nfds)
        {
            for (size_t i = 0; i < Count_; ++i) {
                new (&(Events_[i])) TFdEvent(rep, fds[i], (ui16)what[i], deadline);
            }
        }

        inline ~TContPollEventHolder() throw () {
            for (size_t i = 0; i < Count_; ++i) {
                Events_[i].~TFdEvent();
            }
        }

        inline void ScheduleIoWait(TContExecutor* executor);

        inline TFdEvent* TriggeredEvent() {
            TFdEvent* ret = NULL;
            int status = EINPROGRESS;

            for (size_t i = 0; i < Count_; ++i) {
                TFdEvent& ev = Events_[i];

                switch (ev.Status()) {
                    case EINPROGRESS:
                        break;

                    case ETIMEDOUT:
                        if (status != EINPROGRESS) {
                            break;
                        } // else fallthrough

                    default:
                        status = ev.Status();
                        ret = &ev;
                }
            }

            return ret;
        }

    private:
        TFdEvent* Events_;
        size_t Count_;
};

#include "iovec.h"

class TCont {
        friend struct TContRep;
        friend class TContExecutor;
        friend class TContPollEvent;
    public:
        inline TCont(TContExecutor* executor, TContRep* rep, TContFunc func, void* arg, const char* name)
            : Executor_(executor)
            , Rep_(rep)
            , Func_(func)
            , Arg_(arg)
            , Name_(name)
            , Cancelled_(false)
        {
        }

        inline ~TCont() throw () {
        }

        inline void SetExecutor(TContExecutor* e) throw () {
            Executor_ = e;
        }

        inline void SwitchTo(TCont* next) throw () {
            DBGOUT("switch to " << next->Name());

#if defined(DEBUG_CONT) || defined(DEBUG_CONTEXT_SWITCHES) || !defined(NDEBUG)
            VERIFY(CanSwitchNow(), "can not switch contexts now");
#endif

            Context()->SwitchTo(next->Context());
        }

        inline TContMachineContext* Context() throw () {
#if defined(STACK_GROW_DOWN)
            return (TContMachineContext*)(((char*)(this)) + Align(sizeof(TCont)));
#else
    #error todo
#endif
        }

        inline const TContMachineContext* Context() const throw () {
            return const_cast<TCont*>(this)->Context();
        }

        inline TContExecutor* Executor() throw () {
            return Executor_;
        }

        inline const TContExecutor* Executor() const throw () {
            return Executor_;
        }

        inline TContRep* Rep() throw () {
            return Rep_;
        }

        inline const TContRep* Rep() const throw () {
            return Rep_;
        }

        inline const char* Name() const throw () {
            return Name_;
        }

        inline void PrintMe(TOutputStream& out) const throw () {
            out << "cont("
                << "func = " << (size_t)(void*)Func_ << ", "
                << "arg = " << (size_t)(void*)Arg_  << ", "
                << "name = " << Name_
                << ")";
        }

        inline void Yield() throw () {
            if (SleepD(TInstant::Zero())) {
                ReScheduleAndSwitch();
            }
        }

        inline void ReScheduleAndSwitch() throw ();

        inline int SelectD(SOCKET fds[], int what[], size_t nfds, SOCKET* outfd, TInstant deadline) {
            DBGOUT(*this << " prepare select");

            if (Cancelled()) {
                return ECANCELED;
            }

            if (nfds == 0) {
                return 0;
            }

            TTempBuf memoryBuf(nfds * sizeof(TContPollEvent));
            void* memory = memoryBuf.Data();
            TContPollEventHolder holder(memory, this, fds, what, nfds, deadline);
            holder.ScheduleIoWait(Executor());

            SwitchToScheduler();

            if (Cancelled()) {
                return ECANCELED;
            }

            TFdEvent* ev = holder.TriggeredEvent();

            if (ev) {
                if (outfd) {
                    *outfd = ev->Fd();
                }

                return ev->Status();
            }

            return EINPROGRESS;
        }

        inline int SelectT(SOCKET fds[], int what[], size_t nfds, SOCKET* outfd, TDuration timeout) {
            return SelectD(fds, what, nfds, outfd, timeout.ToDeadLine());
        }

        inline int SelectT(SOCKET fds[], int what[], size_t nfds, SOCKET* outfd) {
            return SelectD(fds, what, nfds, outfd, TInstant::Max());
        }

        inline int PollD(SOCKET fd, int what, TInstant deadline) {
            DBGOUT(*this << " prepare poll");

            TFdEvent event(this, fd, (ui16)what, deadline);

            return ExecuteEvent(&event);
        }

        inline int PollT(SOCKET fd, int what, TDuration timeout) {
            return PollD(fd, what, timeout.ToDeadLine());
        }

        inline int PollI(SOCKET fd, int what) {
            return PollD(fd, what, TInstant::Max());
        }

        /// @return ETIMEDOUT on success
        inline int SleepD(TInstant deadline) {
            DBGOUT(*this << " do sleep");

            TTimerEvent event(this, deadline);

            return ExecuteEvent(&event);
        }

        inline int SleepT(TDuration timeout) {
            return SleepD(timeout.ToDeadLine());
        }

        inline int SleepI() {
            return SleepD(TInstant::Max());
        }

        inline TContIOStatus ReadD(SOCKET fd, void* buf, size_t len, TInstant deadline) {
            DBGOUT(*this << " do read");

            while (true) {
                ssize_t res = DoRead(fd, (char*)buf, len);

                if (res >= 0) {
                    return TContIOStatus::Success((size_t)res);
                }

                {
                    const int err = LastSystemError();

                    if (!IsBlocked(err)) {
                        return TContIOStatus::Error(err);
                    }
                }

                if ((res = PollD(fd, CONT_POLL_READ, deadline)) != 0) {
                    return TContIOStatus::Error((int)res);
                }
            }
        }

        inline TContIOStatus ReadT(SOCKET fd, void* buf, size_t len, TDuration timeout) {
            return ReadD(fd, buf, len, timeout.ToDeadLine());
        }

        inline TContIOStatus ReadI(SOCKET fd, void* buf, size_t len) {
            return ReadD(fd, buf, len, TInstant::Max());
        }

        inline TContIOStatus WriteVectorD(SOCKET fd, TContIOVector* vec, TInstant deadline) {
            DBGOUT(*this << " do writev");

            size_t written = 0;

            while (!vec->Complete()) {
                ssize_t res = DoWriteVector(fd, vec);

                if (res >= 0) {
                    written += res;

                    vec->Proceed((size_t)res);
                } else {
                    {
                        const int err = LastSystemError();

                        if (!IsBlocked(err)) {
                            return TContIOStatus(written, err);
                        }
                    }

                    if ((res = PollD(fd, CONT_POLL_WRITE, deadline)) != 0) {
                        return TContIOStatus(written, (int)res);
                    }
                }
            }

            return TContIOStatus::Success(written);
        }

        inline TContIOStatus WriteVectorT(SOCKET fd, TContIOVector* vec, TDuration timeOut) {
            return WriteVectorD(fd, vec, timeOut.ToDeadLine());
        }

        inline TContIOStatus WriteVectorI(SOCKET fd, TContIOVector* vec) {
            return WriteVectorD(fd, vec, TInstant::Max());
        }

        inline TContIOStatus WriteD(SOCKET fd, const void* buf, size_t len, TInstant deadline) {
            DBGOUT(*this << " do write");

            size_t written = 0;

            while (len){
                ssize_t res = DoWrite(fd, (const char*)buf, len);

                if (res >= 0) {
                    written += res;
                    buf = (const char*)buf + res;
                    len -= res;
                } else {
                    {
                        const int err = LastSystemError();

                        if (!IsBlocked(err)) {
                            return TContIOStatus(written, err);
                        }
                    }

                    if ((res = PollD(fd, CONT_POLL_WRITE, deadline)) != 0) {
                        return TContIOStatus(written, (int)res);
                    }
                }
            }

            return TContIOStatus::Success(written);
        }

        inline TContIOStatus WriteT(SOCKET fd, const void* buf, size_t len, TDuration timeout) {
            return WriteD(fd, buf, len, timeout.ToDeadLine());
        }

        inline TContIOStatus WriteI(SOCKET fd, const void* buf, size_t len) {
            return WriteD(fd, buf, len, TInstant::Max());
        }

        inline void Exit();

        inline int Connect(TSocketHolder& s, const struct addrinfo& ai, TInstant deadLine) {
            DBGOUT(*this << " do connect");

            TSocketHolder res(Socket(ai));

            if (res.Closed()) {
                return LastSystemError();
            }

            const int ret = ConnectD(res, ai.ai_addr, (socklen_t)ai.ai_addrlen, deadLine);

            if (!ret) {
                s.Swap(res);
            }

            return ret;
        }

        inline int Connect(TSocketHolder& s, const TNetworkAddress& addr, TInstant deadLine) {
            int ret = EHOSTUNREACH;

            for (TNetworkAddress::TIterator it = addr.Begin(); it != addr.End(); ++it) {
                ret = Connect(s, *it, deadLine);

                if (!ret) {
                    return 0;
                }
            }

            return ret;
        }

        inline int Connect(TSocketHolder& s, const TNetworkAddress& addr, TDuration timeOut) {
            return Connect(s, addr, timeOut.ToDeadLine());
        }

        inline int Connect(TSocketHolder& s, const TNetworkAddress& addr) {
            return Connect(s, addr, TInstant::Max());
        }

        inline int ConnectD(SOCKET s, const struct sockaddr* name, socklen_t namelen, TInstant deadline) {
            DBGOUT(*this << " do connect");

            if (connect(s, name, namelen)) {
                const int err = LastSystemError();

                if (!IsBlocked(err) && err != EINPROGRESS) {
                    return err;
                }

                return PollD(s, CONT_POLL_WRITE, deadline);
            }

            return 0;
        }

        inline int ConnectT(SOCKET s, const struct sockaddr* name, socklen_t namelen, TDuration timeout) {
            return ConnectD(s, name, namelen, timeout.ToDeadLine());
        }

        inline int ConnectI(SOCKET s, const struct sockaddr* name, socklen_t namelen) {
            return ConnectD(s, name, namelen, TInstant::Max());
        }

        inline int AcceptD(SOCKET s, struct sockaddr* addr, socklen_t* addrlen, TInstant deadline) {
            DBGOUT(*this << " do accept");

            SOCKET ret;

            while ((ret = Accept4(s, addr, addrlen)) == INVALID_SOCKET) {
                int err = LastSystemError();

                if (!IsBlocked(err)) {
                    return -err;
                }

                err = PollD(s, CONT_POLL_READ, deadline);

                if (err) {
                    return -err;
                }
            }

            return (int)ret;
        }

        inline int AcceptT(SOCKET s, struct sockaddr* addr, socklen_t* addrlen, TDuration timeout) {
            return AcceptD(s, addr, addrlen, timeout.ToDeadLine());
        }

        inline int AcceptI(SOCKET s, struct sockaddr* addr, socklen_t* addrlen) {
            return AcceptD(s, addr, addrlen, TInstant::Max());
        }

        static inline SOCKET Socket(int domain, int type, int protocol) {
            return Socket4(domain, type, protocol);
        }

        static inline SOCKET Socket(const struct addrinfo& ai) {
            return Socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
        }

        static inline bool IsBlocked() throw () {
            return IsBlocked(LastSystemError());
        }

        static inline bool IsBlocked(int lasterr) throw () {
            return lasterr == EAGAIN || lasterr == EWOULDBLOCK;
        }

        /*
         * useful for keep-alive connections
         */
        static inline bool SocketNotClosedByOtherSide(SOCKET s) throw () {
            const int r = MsgPeek(s);

            return r > 0 || (r == -1 && IsBlocked());
        }

        static inline bool HavePendingData(SOCKET s) throw () {
            return MsgPeek(s) > 0;
        }

        static inline int MsgPeek(SOCKET s) throw () {
            char c;
            return recv(s, &c, 1, MSG_PEEK);
        }

        inline bool IAmRunning() const throw ();

        inline void Cancel() throw () {
            if (Cancelled()) {
                return;
            }

            DBGOUT(*this << " do cancel");

            Cancelled_ = true;

            if (!IAmRunning()) {
                DBGOUT(*this << " do cancel from other");

                ReSchedule();
            }
        }

        inline bool Cancelled() const throw () {
            return Cancelled_;
        }

        /*
         * please dont use this :)
         */
        inline void SwitchToScheduler() throw ();
        inline void ReSchedule() throw ();

    private:
        inline bool CanSwitchNow() const throw () {
            /*
             * we can not switch stacks while stack unwinding
             */
            return !UncaughtException();
        }

        inline void Execute() {
            YASSERT(Func_);

            Func_(this, Arg_);
        }

    public:
        static inline ssize_t DoRead(SOCKET fd, char* buf, size_t len) throw () {
#if defined(_win_)
            if (IsSocket(fd)) {
                return recv(fd, buf, (int)len, 0);
            }

            return _read((int)fd, buf, (int)len);
#else
            return read(fd, buf, len);
#endif
        }

        static inline ssize_t DoWrite(SOCKET fd, const char* buf, size_t len) throw () {
#if defined(_win_)
            if (IsSocket(fd)) {
                return send(fd, buf, (int)len, 0);
            }

            return _write((int)fd, buf, (int)len);
#else
            return write(fd, buf, len);
#endif
        }

        static inline ssize_t DoWriteVector(SOCKET fd, TContIOVector* vec) throw () {
            return writev(fd, (const iovec*)vec->Parts(), (int)vec->Count());
        }

    private:
        TContExecutor* Executor_;
        TContRep* Rep_;
        TContFunc Func_;
        void* Arg_;
        const char* Name_;
        bool Cancelled_;
};

#include "stack.h"

struct TContRep: public TIntrusiveListItem<TContRep>, public ITrampoLine {
    inline TContRep(TContStackAllocator* alloc)
        : real(alloc->Allocate())
        , full((char*)real->Data(), real->Length())
#if defined(STACK_GROW_DOWN)
        , stack(full.Ptr, EffectiveStackLength(full.Len))
        , cont(stack.End(), Align(sizeof(TCont)))
        , machine(cont.End(), Align(MachineContextSize()))
#else
        #error todo
#endif
    {
    }

    virtual void DoRun();

    void Construct(TContExecutor* executor, TContFunc func, void* arg, const char* name);
    void Destruct() throw ();

    inline TCont* ContPtr() throw () {
        return (TCont*)cont.Ptr;
    }

    inline const TCont* ContPtr() const throw () {
        return (const TCont*)cont.Ptr;
    }

    inline TContMachineContext* MachinePtr() throw () {
        return (TContMachineContext*)machine.Ptr;
    }

    static inline size_t OverHead() throw () {
        return Align(sizeof(TCont)) + Align(MachineContextSize());
    }

    static inline size_t EffectiveStackLength(size_t alloced) throw () {
        return alloced - OverHead();
    }

    static inline size_t ToAllocate(size_t stackLen) throw () {
        return Align(stackLen) + OverHead();
    }

    inline bool IAmRuning() const throw () {
        return ContPtr()->IAmRunning();
    }

    TContStackAllocator::TStackPtr real;

    TMemRange full;
#if defined(STACK_GROW_DOWN)
    TMemRange stack;
    TMemRange cont;
    TMemRange machine;
#else
    #error todo
#endif
};

struct TPollEventList: public TIntrusiveList<IPollEvent> {
    inline ui16 Flags() const throw () {
        ui16 ret = 0;

        for (TConstIterator it = Begin(); it != End(); ++it) {
            ret |= it->What();
        }

        return ret;
    }
};

class TEventWaitQueue {
    struct TCancel {
        inline void operator() (TContPollEvent* e) throw () {
            e->Cont()->Cancel();
        }

        inline void operator() (TContPollEvent& e) throw () {
            operator()(&e);
        }
    };

    typedef TRbTree<TContPollEvent, TContPollEventCompare> TIoWait;

public:
    inline void Register(TContPollEvent* event) {
        IoWait_.Insert(event);
        event->Cont()->Rep()->Unlink();
    }

    inline bool Empty() const throw () {
        return IoWait_.Empty();
    }

    inline void Abort() throw () {
        TCancel visitor;

        IoWait_.ForEach(visitor);
    }

    inline TInstant CancelTimedOut(TInstant now) throw () {
        TIoWait::TIterator it = IoWait_.Begin();

        if (it != IoWait_.End()) {
            if (it->DeadLine() > now) {
                return it->DeadLine();
            }

            do {
                (it++)->Wake(ETIMEDOUT);
            } while (it != IoWait_.End() && it->DeadLine() <= now);
        }

        return now;
    }

private:
    TIoWait IoWait_;
};

#include "poller.h"

template <class T>
class TBigArray {
    struct TValue: public T, public TObjectFromPool<TValue> {
        inline TValue() {
        }
    };

public:
    inline TBigArray()
        : Pool_(TMemoryPool::TExpGrow::Instance(), TDefaultAllocator::Instance())
    {
    }

    inline T* Get(size_t index) {
        if (Lst_.size() <= index) {
            Lst_.resize(index + 1);
        }

        TRef& ret = Lst_[index];

        if (!ret) {
            ret = new (&Pool_) TValue();
        }

        return ret.Get();
    }

private:
    typedef TAutoPtr<TValue> TRef;
    typename TValue::TPool Pool_;
    yvector<TRef> Lst_;
};

class TContPoller {
public:
    typedef IPollerFace::TEvent TEvent;
    typedef IPollerFace::TEvents TEvents;

    inline TContPoller()
        : P_(IPollerFace::Default())
    {
    }

    inline void Schedule(IPollEvent* event) {
        TPollEventList* lst = List(event->Fd());
        const ui16 oldFlags = lst->Flags();
        lst->PushFront(event);
        const ui16 newFlags = lst->Flags();

        if (newFlags != oldFlags) {
            P_->Set(lst, event->Fd(), newFlags);
        }
    }

    inline void Remove(IPollEvent* event) throw () {
        TPollEventList* lst = List(event->Fd());
        const ui16 oldFlags = lst->Flags();
        event->Unlink();
        const ui16 newFlags = lst->Flags();

        if (newFlags != oldFlags) {
            P_->Set(lst, event->Fd(), newFlags);
        }
    }

    inline size_t Wait(TEvents& events, TInstant deadLine) {
        events.clear();

        P_->Wait(events, deadLine);

        return events.size();
    }

private:
    inline TPollEventList* List(size_t fd) {
        return Lists_.Get(fd);
    }

private:
    TBigArray<TPollEventList> Lists_;
    TAutoPtr<IPollerFace> P_;
};

class TContRepPool {
        typedef TIntrusiveListWithAutoDelete<TContRep, TDelete> TFreeReps;
    public:
        inline TContRepPool(TContStackAllocator* alloc)
            : Alloc_(alloc)
        {
        }

        inline TContRepPool(size_t stackLen)
            : MyAlloc_(new TDefaultStackAllocator(TContRep::ToAllocate(AlignUp<size_t>(stackLen, 2 * STACK_ALIGN))))
            , Alloc_(MyAlloc_.Get())
        {
        }

        inline TContRep* Allocate() {
            if (Free_.Empty()) {
                return new TContRep(Alloc_);
            }

            return Free_.PopFront();
        }

        inline void Release(TContRep* cont) throw () {
            Free_.PushFront(cont);
        }

    private:
        THolder<TContStackAllocator> MyAlloc_;
        TContStackAllocator* Alloc_;
        TFreeReps Free_;
};

template <class Functor>
static void ContHelperFunc(TCont* cont, void* arg) {
    (*((Functor*)(arg)))(cont);
}

template <typename T, void(T::*M)(TCont*)>
static void ContHelperMemberFunc(TCont* c, void* arg) {
    T* obj = ReinterpretCast<T*>(arg);
    (obj->*M)(c);
}

/// Central coroutine class.
/// Note, coroutines are single-threaded, and all methods must be called from the single thread
class TContExecutor {
        friend class TCont;
        friend struct TContRep;
        friend class TContEvent;
        friend class TContPollEvent;
        friend class TContPollEventHolder;
        typedef TIntrusiveList<TContRep> TContList;

        struct TCancel {
            inline void operator() (TContRep* c) throw () {
                c->ContPtr()->Cancel();
            }
        };

        struct TNoOp {
            template <class T>
            inline void operator() (T*) throw () {
            }
        };

        struct TReleaseAll {
            inline void operator() (TContRep* c) throw () {
                c->ContPtr()->Executor()->Release(c);
            }
        };

    public:
        inline TContExecutor(size_t stackSize)
            : MyPool_(new TContRepPool(stackSize))
            , Pool_(*MyPool_)
            , Current_(0)
            , FailOnError_(false)
        {
        }

        inline TContExecutor(TContRepPool* pool)
            : Pool_(*pool)
            , Current_(0)
            , FailOnError_(false)
        {
        }

        inline ~TContExecutor() throw () {
        }

        /*
         * assume we already create all necessary coroutines
         */
        inline void Execute() {
            TNoOp nop;

            Execute(nop);
        }

        inline void Execute(TContFunc func, void* arg = 0) {
            Create(func, arg, "sys_main");

            RunScheduler();
        }

        template <class Functor>
        inline void Execute(Functor& f) {
            Execute((TContFunc)ContHelperFunc<Functor>, (void*)&f);
        }

        template <typename T, void(T::*M)(TCont*)>
        inline void Execute(T* obj) {
            Execute(ContHelperMemberFunc<T, M>, obj);
        }

        inline TContMachineContext* SchedCont() throw () {
            return &SchedContext_;
        }

        template <class Functor>
        inline TContRep* Create(Functor& f, const char* name) {
            return Create((TContFunc)ContHelperFunc<Functor>, (void*)&f, name);
        }

        template <typename T, void(T::*M)(TCont*)>
        inline TContRep* Create(T* obj, const char* name) {
            return Create(ContHelperMemberFunc<T, M>, obj, name);
        }

        inline TContRep* Create(TContFunc func, void* arg, const char* name) {
            TContRep* cont = CreateImpl(func, arg, name);

            ScheduleExecution(cont);

            return cont;
        }

        inline TContPoller* Poller() throw () {
            return &Poller_;
        }

        inline TEventWaitQueue* WaitQueue() throw () {
            return &WaitQueue_;
        }

        inline TContRep* Running() throw () {
            return Current_;
        }

        inline const TContRep* Running() const throw () {
            return Current_;
        }

        inline void Abort() throw () {
            WaitQueue_.Abort();
            TCancel visitor;
            Ready_.ForEach(visitor);
        }

        inline void SetFailOnError(bool fail) throw () {
            FailOnError_ = fail;
        }

        inline bool FailOnError() const throw () {
            return FailOnError_;
        }

        inline void ScheduleIoWait(TFdEvent* event) {
            DBGOUT(*event->Cont() << " schedule iowait");
            WaitQueue_.Register(event);
            Poller_.Schedule(event);
        }

        inline void ScheduleIoWait(TTimerEvent* event) throw () {
            DBGOUT(*event->Cont() << " schedule timer");
            WaitQueue_.Register(event);
        }

    private:
        inline TContRep* CreateImpl(TContFunc func, void* arg, const char* name) {
            TContRep* cont = Pool_.Allocate();

            cont->Construct(this, func, arg, name);
            cont->Unlink();

            return cont;
        }

        inline void Release(TContRep* cont) throw () {
            DBGOUT(*cont << " release");

            cont->Unlink();
            cont->Destruct();

            Pool_.Release(cont);
        }

        inline void Exit(TContRep* cont) throw () {
            DBGOUT(*cont << " exit");

            ScheduleToDelete(cont);
            cont->ContPtr()->SwitchToScheduler();

            VERIFY(false, "can not return from exit");
        }

        inline void RunScheduler() {
            DBGOUT("scheduler: started");

            while (!Ready_.Empty()) {
                TContRep* cont = Ready_.PopFront();

                DBGOUT(*cont << " prepare for activate");
                Activate(cont);

                WaitForIO();
                DeleteScheduled();
            }

            DBGOUT("scheduler: stopped");
        }

        inline void ScheduleToDelete(TContRep* cont) throw () {
            DBGOUT(*cont << "schedule to delete");
            ToDelete_.PushBack(cont);
        }

        inline void ScheduleExecution(TContRep* cont) throw () {
            DBGOUT(*cont << "schedule execution");
            Ready_.PushBack(cont);
        }

        inline void Activate(TContRep* cont) throw () {
            DBGOUT("activate");
            Current_ = cont;
            DBGOUT("scheduler: switch to " << *cont);
            SchedContext_.SwitchTo(cont->ContPtr()->Context());
        }

        inline void DeleteScheduled() throw () {
            TReleaseAll functor;

            ToDelete_.ForEach(functor);
        }

        void WaitForIO();

    private:
        TContList ToDelete_;
        TContList Ready_;
        TEventWaitQueue WaitQueue_;
        TContPoller Poller_;
        THolder<TContRepPool> MyPool_;
        TContRepPool& Pool_;
        TContMachineContext SchedContext_;
        TContRep* Current_;
        typedef TContPoller::TEvents TEvents;
        TEvents Events_;
        bool FailOnError_;
};

template <class T>
inline int ExecuteEvent(T* event) throw () {
    TCont* c = event->Cont();

    if (c->Cancelled()) {
        return ECANCELED;
    }

    /*
     * schedule wait
     */
    c->Executor()->ScheduleIoWait(event);

    /*
     * go to scheduler
     */
    c->SwitchToScheduler();
    /*
     * wait complete
     */

    if (c->Cancelled()) {
        return ECANCELED;
    }

    return event->Status();
}

inline void TContPollEventHolder::ScheduleIoWait(TContExecutor* executor) {
    for (size_t i = 0; i < Count_; ++i) {
        executor->ScheduleIoWait(&(Events_[i]));
    }
}

inline void TFdEvent::RemoveFromIOWait() throw () {
    Cont()->Executor()->Poller()->Remove(this);
}

inline void TContPollEvent::Wake() throw () {
    UnLink();
    Cont()->ReSchedule();
}

inline void TCont::Exit() {
    Executor()->Exit(Rep());
}

inline bool TCont::IAmRunning() const throw () {
    return Rep() == Executor()->Running();
}

inline void TCont::ReSchedule() throw () {
    DBGOUT("reschedule");

    Executor()->ScheduleExecution(Rep());
}

inline void TCont::SwitchToScheduler() throw () {
    DBGOUT("switch to scheduler");

    Context()->SwitchTo(Executor()->SchedCont());
}

inline void TCont::ReScheduleAndSwitch() throw () {
    ReSchedule();
    SwitchToScheduler();
}

#define EWAKEDUP 34567

#include "events.h"
#include "mutex.h"
#include "condvar.h"
#include "sockpool.h"

#if !defined(FROM_IMPL_CPP)
    #undef DBGOUT
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif
