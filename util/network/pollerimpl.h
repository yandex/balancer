#pragma once

#include <util/system/error.h>
#include <util/system/mutex.h>
#include <util/system/defaults.h>

#include <util/generic/ylimits.h>
#include <util/generic/utility.h>
#include <util/generic/yexception.h>

#include <util/network/socket.h>

#include <util/datetime/base.h>

#if defined(_freebsd_) || defined(_darwin_)
    #define HAVE_KQUEUE_POLLER
#endif

#if defined(_linux_)
    #define HAVE_EPOLL_POLLER
#endif

//now we always have it
#define HAVE_SELECT_POLLER

#if defined(HAVE_KQUEUE_POLLER)
    #include <sys/event.h>
#endif

#if defined(HAVE_EPOLL_POLLER)
    #include <sys/epoll.h>
#endif

enum EContPoll {
    CONT_POLL_READ = 1,
    CONT_POLL_WRITE = 2
};

static inline bool IsSocket(SOCKET fd) throw () {
    int val = 0;
    socklen_t len = sizeof(val);

    if (getsockopt(fd, SOL_SOCKET, SO_TYPE, (char*)&val, &len) == 0) {
        return true;
    }

    return LastSystemError() != ENOTSOCK;
}

struct TWithoutLocking {
    typedef TFakeMutex TMyMutex;
};

#if defined(HAVE_KQUEUE_POLLER)
static inline int Kevent(int kq, struct kevent* changelist, int nchanges,
    struct kevent* eventlist, int nevents, const struct timespec* timeout) throw () {
    int ret;

    do {
        ret = kevent(kq, changelist, nchanges, eventlist, nevents, timeout);
    } while (ret == -1 && errno == EINTR);

    return ret;
}

template <class TLockPolicy>
class TKqueuePoller {
public:
    typedef struct ::kevent TEvent;

    inline TKqueuePoller()
        : Fd_(kqueue())
    {
        if (Fd_ == -1) {
            ythrow TSystemError() << "kqueue failed";
        }
    }

    inline ~TKqueuePoller() throw () {
        close(Fd_);
    }

    inline int Fd() const throw () {
        return Fd_;
    }

    inline void SetImpl(void* data, int fd, int what) {
        TEvent e[2];

        Zero(e);

        EV_SET(e + 0, fd, EVFILT_READ, EV_ADD | ((what & CONT_POLL_READ) ? EV_ENABLE : EV_DISABLE), 0, 0, data);
        EV_SET(e + 1, fd, EVFILT_WRITE, EV_ADD | ((what & CONT_POLL_WRITE) ? EV_ENABLE : EV_DISABLE), 0, 0, data);

        if (Kevent(Fd_, e, 2, 0, 0, 0) == -1) {
            ythrow TSystemError() << "kevent add failed";
        }
    }

    inline void Remove(int fd) throw () {
        TEvent e[2];

        Zero(e);

        EV_SET(e + 0, fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
        EV_SET(e + 1, fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);

        VERIFY(!(Kevent(Fd_, e, 2, 0, 0, 0) == -1 && errno != ENOENT), "kevent remove failed");
    }

    inline size_t Wait(TEvent* events, size_t len, int timeout) throw () {
        struct timespec ts;

        ts.tv_sec = timeout / 1000000;
        ts.tv_nsec = (timeout % 1000000) * 1000;

        const int ret = Kevent(Fd_, 0, 0, events, len, &ts);

        VERIFY(ret >= 0, "kevent failed");

        return (size_t)ret;
    }

    static inline void* ExtractEvent(const TEvent* event) throw () {
        return event->udata;
    }

    static inline int ExtractStatus(const TEvent* event) throw () {
        if (event->flags & EV_ERROR) {
            return EIO;
        }

        return event->fflags;
    }

    static inline int ExtractFilterImpl(const TEvent* event) throw () {
        if (event->filter == EVFILT_READ) {
            return CONT_POLL_READ;
        }

        if (event->filter == EVFILT_WRITE) {
            return CONT_POLL_WRITE;
        }

        if (event->flags & EV_EOF) {
            return CONT_POLL_READ | CONT_POLL_WRITE;
        }

        return 0;
    }

private:
    int Fd_;
};
#endif

#if defined(HAVE_EPOLL_POLLER)
static inline int ContEpollWait(int epfd, struct epoll_event* events, int maxevents, int timeout) throw() {
    int ret;

    do {
        ret = epoll_wait(epfd, events, maxevents, Min<int>(timeout, 35 * 60 * 1000));
    } while (ret == -1 && errno == EINTR);

    return ret;
}

template <class TLockPolicy>
class TEpollPoller {
public:
    typedef struct ::epoll_event TEvent;

    inline TEpollPoller()
        : Fd_(epoll_create(1))
    {
        if (Fd_ == -1) {
            ythrow TSystemError() << "epoll_create failed";
        }
    }

    inline ~TEpollPoller() throw () {
        close(Fd_);
    }

    inline int Fd() const throw () {
        return Fd_;
    }

    inline void SetImpl(void* data, int fd, int what) {
        TEvent e;

        Zero(e);

        e.events = CalcWhat(what);
        e.data.ptr = data;

        if (epoll_ctl(Fd_, EPOLL_CTL_ADD, fd, &e) == -1) {
            if (epoll_ctl(Fd_, EPOLL_CTL_MOD, fd, &e) == -1) {
                ythrow TSystemError() << "epoll add failed";
            }
        }
    }

    inline void Remove(int fd) throw () {
        TEvent e;

        Zero(e);

        epoll_ctl(Fd_, EPOLL_CTL_DEL, fd, &e);
    }

    inline size_t Wait(TEvent* events, size_t len, int timeout) throw () {
        const int ret = ContEpollWait(Fd_, events, len, timeout / 1000);

        VERIFY(ret >= 0, "epoll wait error");

        return (size_t)ret;
    }

    static inline void* ExtractEvent(const TEvent* event) throw () {
        return event->data.ptr;
    }

    static inline int ExtractStatus(const TEvent* event) throw () {
        if (event->events & (EPOLLERR | EPOLLHUP)) {
            return EIO;
        }

        return 0;
    }

    static inline int ExtractFilterImpl(const TEvent* event) throw () {
        int ret = 0;

        if (event->events & EPOLLIN) {
            ret |= CONT_POLL_READ;
        }

        if (event->events & EPOLLOUT) {
            ret |= CONT_POLL_WRITE;
        }

        return ret;
    }

private:
    static inline int CalcWhat(int w) throw () {
        int ret = 0;

        if (w & CONT_POLL_READ) {
            ret |= EPOLLIN;
        }

        if (w & CONT_POLL_WRITE) {
            ret |= EPOLLOUT;
        }

        return ret;
    }

private:
    int Fd_;
};
#endif

#if defined(HAVE_SELECT_POLLER)
#include <util/memory/tempbuf.h>
#include <util/generic/hash.h>

#include "pair.h"

static inline int ContSelect(int n, fd_set* r, fd_set* w, fd_set* e, struct timeval* t) throw () {
    int ret;

    do {
        ret = select(n, r, w, e, t);
    } while (ret == -1 && errno == EINTR);

    return ret;
}

struct TSelectPollerNoTemplate {
    struct THandle {
        void* Data_;
        int Filter_;

        inline THandle()
            : Data_(0)
            , Filter_(0)
        {
        }

        inline void* Data() const throw () {
            return Data_;
        }

        inline void Set(void* d, int s) throw () {
            Data_ = d;
            Filter_ = s;
        }

        inline int Filter() const throw () {
            return Filter_;
        }
    };

    class TFds: public yhash<SOCKET, THandle> {
    public:
        inline void Set(SOCKET fd, void* data, int filter) {
            (*this)[fd].Set(data, filter);
        }

        inline void Remove(SOCKET fd) {
            erase(fd);
        }

        inline SOCKET Build(fd_set* r, fd_set* w, fd_set* e) const throw () {
            SOCKET ret = 0;

            for (const_iterator it = begin(); it != end(); ++it) {
                const SOCKET fd = it->first;
                const THandle& handle = it->second;

                FD_SET(fd, e);

                if (handle.Filter() & CONT_POLL_READ) {
                    FD_SET(fd, r);
                }

                if (handle.Filter() & CONT_POLL_WRITE) {
                    FD_SET(fd, w);
                }

                if (fd > ret) {
                    ret = fd;
                }
            }

            return ret;
        }
    };

    struct TEvent: public THandle {
        inline int Status() const throw () {
            return -Min(Filter(), 0);
        }

        inline void Error(void* d, int err) throw () {
            Set(d, -err);
        }

        inline void Success(void* d, int what) throw () {
            Set(d, what);
        }
    };
};

template <class TLockPolicy>
class TSelectPoller: public TSelectPollerNoTemplate {
    typedef typename TLockPolicy::TMyMutex TMyMutex;

public:
    inline TSelectPoller()
        : Begin_(0)
        , End_(0)
    {
        SocketPair(Signal_);
        SetNonBlock(WaitSock());
        SetNonBlock(SigSock());
    }

    inline ~TSelectPoller() throw () {
        closesocket(Signal_[0]);
        closesocket(Signal_[1]);
    }

    inline void SetImpl(void* data, SOCKET fd, int what) {
        {
            TGuard<TMyMutex> g(Lock_);

            Fds_.Set(fd, data, what);
        }

        Signal();
    }

    inline void Remove(SOCKET fd) throw () {
        {
            TGuard<TMyMutex> g(Lock_);

            Fds_.Remove(fd);
        }

        Signal();
    }

    inline size_t Wait(TEvent* events, size_t len, int timeout) throw () {
        do {
            if (Begin_ != End_) {
                const size_t ret = Min<size_t>(End_ - Begin_, len);

                memcpy(events, Begin_, sizeof(*events) * ret);
                Begin_ += ret;

                return ret;
            }

            if (len >= EventNumberHint()) {
                return WaitBase(events, len, timeout);
            }

            Begin_ = SavedEvents();
            End_ = Begin_ + WaitBase(Begin_, EventNumberHint(), timeout);
        } while (Begin_ != End_);

        return 0;
    }

    inline TEvent* SavedEvents() {
        if (!SavedEvents_) {
            SavedEvents_.Reset(new TEvent[EventNumberHint()]);
        }

        return SavedEvents_.Get();
    }

    inline size_t WaitBase(TEvent* events, size_t len, int timeout) throw () {
        TGuard<TMyMutex> g(Lock_);

        TTempBuf tmpBuf(3 * sizeof(fd_set));

        fd_set* in = (fd_set*)tmpBuf.Data();
        fd_set* out = &in[1];
        fd_set* errFds = &in[2];

        FD_ZERO(in);
        FD_ZERO(out);
        FD_ZERO(errFds);

        FD_SET(WaitSock(), in);

        const SOCKET maxFdNum = Max(Fds_.Build(in, out, errFds), WaitSock());
        struct timeval tout;

        tout.tv_sec = timeout / 1000000;
        tout.tv_usec = timeout % 1000000;

        Lock_.Release();
        int ret = ContSelect(int(maxFdNum + 1), in, out, errFds, &tout);
        Lock_.Acquire();

        if (ret > 0 && FD_ISSET(WaitSock(), in)) {
            --ret;
            TryWait();
        }

        VERIFY(ret >= 0 && (size_t)ret <= len, "select error");

        TEvent* eventsStart = events;

        for (typename TFds::const_iterator it = Fds_.begin(); it != Fds_.end(); ++it) {
            const SOCKET fd = it->first;
            const THandle& handle = it->second;

            if (FD_ISSET(fd, errFds)) {
                (events++)->Error(handle.Data(), EIO);
            } else {
                int what = 0;

                if (FD_ISSET(fd, in)) {
                    what |= CONT_POLL_READ;
                }

                if (FD_ISSET(fd, out)) {
                    what |= CONT_POLL_WRITE;
                }

                if (what) {
                    (events++)->Success(handle.Data(), what);
                }
            }
        }

        return events - eventsStart;
    }

    inline size_t EventNumberHint() const throw () {
        return sizeof(fd_set) * 8 * 2;
    }

    static inline void* ExtractEvent(const TEvent* event) throw () {
        return event->Data();
    }

    static inline int ExtractStatus(const TEvent* event) throw () {
        return event->Status();
    }

    static inline int ExtractFilterImpl(const TEvent* event) throw () {
        return event->Filter();
    }

private:
    inline void Signal() throw () {
        char ch = 13;

        send(SigSock(), &ch, 1, 0);
    }

    inline void TryWait() {
        char ch[32];

        while (recv(WaitSock(), ch, sizeof(ch), 0) > 0) {
            YASSERT(ch[0] == 13);
        }
    }

    inline SOCKET WaitSock() const throw () {
        return Signal_[1];
    }

    inline SOCKET SigSock() const throw () {
        return Signal_[0];
    }

private:
    TFds Fds_;

    TMyMutex Lock_;
    TArrayHolder<TEvent> SavedEvents_;
    TEvent* Begin_;
    TEvent* End_;

    SOCKET Signal_[2];
};
#endif

static inline TDuration PollStep(const TInstant& deadLine, const TInstant& now) throw () {
    if (deadLine < now) {
        return TDuration::Zero();
    }

    return Min(deadLine - now, TDuration::Seconds(1000));
}

template <class TBase>
class TGenericPoller: public TBase {
public:
    typedef typename TBase::TEvent TEvent;

    inline void Set(void* data, SOCKET fd, int what) {
        if (what) {
            this->SetImpl(data, fd, what);
        } else {
            this->Remove(fd);
        }
    }

    static inline int ExtractFilter(const TEvent* event) throw () {
        if (TBase::ExtractStatus(event)) {
            return CONT_POLL_READ | CONT_POLL_WRITE;
        }

        return TBase::ExtractFilterImpl(event);
    }

    inline size_t WaitD(TEvent* events, size_t len, TInstant deadLine, TInstant now = TInstant::Now()) throw () {
        if (!len) {
            return 0;
        }

        size_t ret;

        do {
            ret = this->Wait(events, len, (int)PollStep(deadLine, now).MicroSeconds());
        } while (!ret && ((now = TInstant::Now()) < deadLine));

        return ret;
    }
};

//we should really have template typedefs
#if defined(HAVE_KQUEUE_POLLER)
    #define TPollerImplBase TKqueuePoller
#elif defined(HAVE_EPOLL_POLLER)
    #define TPollerImplBase TEpollPoller
#elif defined(HAVE_SELECT_POLLER)
    #define TPollerImplBase TSelectPoller
#else
    #error "unsupported platform"
#endif

template <class TLockPolicy>
class TPollerImpl: public TGenericPoller<TPollerImplBase<TLockPolicy> > {
};
