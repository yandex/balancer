#include "profile.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <util/system/tls.h>
#include <util/string/cast.h>
#include <util/stream/file.h>
#include <util/system/mutex.h>
#include <util/system/atomic.h>
#include <util/system/thread.h>
#include <util/system/backtrace.h>
#include <util/network/socket.h>
#include <util/generic/vector.h>
#include <util/generic/singleton.h>

#if defined(_unix_)
    #include <pthread.h>
#endif

class TAllocData;

static void Register(TAllocData* data);

static TAtomic counter = 0;

class TAllocData {
    public:
        inline TAllocData()
            : Out_(Construct())
            , Buf_(Tmp_)
            , StoreBackTrace_(OptsHave('b'))
            , BegBT_(0)
            , EndBT_(2)
        {
            ::Register(this);
        }

        inline ~TAllocData() throw () {
            try {
                Flush();
            } catch (...) {
            }

            Out_.Destroy();
        }

        inline void Register(ui64 ptr, ui32 len) {
            const ui64 handle = AtomicAdd(counter, 1);
            char* buf = Ser_;

            *buf = 0xA1 - StoreBackTrace_;
            ++buf;

            memcpy(buf, &ptr, sizeof(ptr));
            buf += sizeof(ptr);

            memcpy(buf, &len, sizeof(len));
            buf += sizeof(len);

            memcpy(buf, &handle, sizeof(handle));
            buf += sizeof(handle);

            if (StoreBackTrace_) {
                const size_t size = BackTrace(BT_, EndBT_ + 1);

                YASSERT(size - BegBT_ < 0x100);
                *buf++ = char(size - BegBT_);

                for (size_t i = BegBT_; i < size; ++i) {
                    const ui64 bt = (ui64)(ptrdiff_t)BT_[i];

                    memcpy(buf, &bt, sizeof(bt));
                    buf += sizeof(bt);
                }
            }

            Append(Ser_, buf - Ser_);
        }

        inline void Unregister(ui64 ptr) {
            const ui64 handle = AtomicAdd(counter, 1);
            char* buf = Ser_;

            *buf = (char)(ui32)0xA2;
            ++buf;

            memcpy(buf, &ptr, sizeof(ptr));
            buf += sizeof(ptr);

            memcpy(buf, &handle, sizeof(handle));
            buf += sizeof(handle);

            Append(Ser_, buf - Ser_);
        }

        inline void Flush() {
            Out_->Write(Tmp_, Buf_ - Tmp_);
            Buf_ = Tmp_;
        }

        inline void Append(const void* buf, size_t len) {
            if (Avail() < len) {
                Flush();
            }

            memcpy(Buf_, buf, len);
            Buf_ += len;
        }

        inline size_t Avail() const throw () {
            return Tmp_ + sizeof(Tmp_) - Buf_;
        }

        inline void Reopen() {
            Out_.Reset(Construct());
        }

    private:
        static inline TOutputStream* Construct() {
            const char* name = getenv("DA_LOG");

            if (!name) {
                name = "/tmp/debug_log";
            }

            const char* p = strchr(name, ':');

            if (p) {
                return new TSocketOutput(TSocket(TNetworkAddress(Stroka(name, p - name), FromString<ui16>(p + 1))));
            }

            char buf[1024];
#if defined(_linux_) || defined(_freebsd_)
            snprintf(buf, sizeof(buf), "%s.%d.%" PRIu64 "", name, (int)getpid(), (ui64)TThread::CurrentThreadId());
#else
            snprintf(buf, sizeof(buf), "%s.%" PRIu64 "", name, (ui64)TThread::CurrentThreadId());
#endif

            return new TFileOutput(buf);
        }

        static inline bool OptsHave(char ch) {
            const char* opts = getenv("DA_OPTS");

            if (opts) {
                return strchr(opts, ch);
            }

            return false;
        }

    private:
        THolder<TOutputStream> Out_;
        char Tmp_[1024 * 64];
        char Ser_[1024];
        void* BT_[64];
        char* Buf_;
        const bool StoreBackTrace_;
        const size_t BegBT_;
        const size_t EndBT_;
};

class TAllocDatas {
        typedef yvector<TAllocData*> TDatas;
    public:
        static TAllocDatas& Instance() {
            return *Singleton<TAllocDatas>();
        }

        inline ~TAllocDatas() throw () {
            try {
                Flush();
            } catch (...) {
            }
        }

        inline void Register(TAllocData* data) {
            TGuard<TMutex> guard(Mutex_);

            Datas_.push_back(data);
        }

        inline void Reopen() {
            TGuard<TMutex> guard(Mutex_);

            for (TDatas::iterator it = Datas_.begin(); it != Datas_.end(); ++it) {
                (*it)->Flush();
                (*it)->Reopen();
            }
        }

    private:
        inline void Flush() {
            TGuard<TMutex> guard(Mutex_);

            for (TDatas::iterator it = Datas_.begin(); it != Datas_.end(); ++it) {
                (*it)->Flush();
            }

            Datas_.clear();
        }

    private:
        TMutex Mutex_;
        TDatas Datas_;
};

static void Register(TAllocData* data) {
    TAllocDatas::Instance().Register(data);
}

extern "C" {
#if defined(_unix_) && defined(_mt_)
    static void ReopenDatas() {
        TAllocDatas::Instance().Reopen();
    }
#endif

    static void RegisterReopenAtFork() {
#if defined(_unix_) && defined(_mt_)
        if (pthread_atfork(0, 0, ReopenDatas)) {
            //abort();
        }
#else
#endif
}
}

POD_STATIC_THREAD(bool) in_hook(false);
POD_STATIC_THREAD(TAllocData*) alloc_data((TAllocData*)0);

static inline TAllocData* Data() {
    TAllocData*& ad = alloc_data;

    if (!ad) {
        ad = new TAllocData;
    }

    return ad;
}

static inline void RunNewHook(void* ptr, size_t len) {
    Data()->Register((ui64)(ptrdiff_t)ptr, (ui32)len);
}

static inline void RunDelHook(void* ptr) {
    Data()->Unregister((ui64)(ptrdiff_t)ptr);
}

static void NewHook(void* ptr, size_t len) {
    if (in_hook) {
        return;
    }

    in_hook = true;

    try {
        RunNewHook(ptr, len);
    } catch (...) {
    }

    in_hook = false;
}

static void DelHook(void* ptr) {
    if (in_hook) {
        return;
    }

    in_hook = true;

    try {
        RunDelHook(ptr);
    } catch (...) {
    }

    in_hook = false;
}

void InitDebugAllocator() {
    if (getenv("DA_LOG")) {
        TAllocatorHooks::SetNewHook(NewHook);
        TAllocatorHooks::SetDelHook(DelHook);

        RegisterReopenAtFork();
    }
}
