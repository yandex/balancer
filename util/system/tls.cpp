#include "tls.h"
#include "mutex.h"
#include "thread.h"

#include <util/generic/set.h>
#include <util/generic/hash.h>
#include <util/generic/deque.h>
#include <util/generic/singleton.h>

#if defined(_unix_)
    #include <pthread.h>
#endif

using namespace NTls;

class TGenericTls {
public:
    typedef size_t TSmallKey;

    class TKeys {
    public:
        inline TSmallKey Acquire() {
            return AtomicIncrement(Max_) - (TSmallKey)1;
        }

        inline void Release(TSmallKey) {
        }

        static inline TKeys* Instance() {
            return Singleton<TKeys>();
        }

    private:
        TAtomic Max_;
    };

    class TPerThreadStorage {
    public:
        struct TKey: public TNonCopyable {
            inline TKey(TDtor dtor)
                : Key(TKeys::Instance()->Acquire())
                , Dtor(dtor)
            {
            }

            inline ~TKey() throw () {
                TKeys::Instance()->Release(Key);
            }

            TSmallKey Key;
            TDtor Dtor;
        };

        class TStoredValue: public TRefCounted<TStoredValue> {
        public:
            inline TStoredValue(const TKey* key)
                : Data_(0)
                , Dtor_(key->Dtor)
            {
            }

            inline ~TStoredValue() throw () {
                if (Dtor_ && Data_) {
                    Dtor_(Data_);
                }
            }

            inline void Set(void* ptr) throw () {
                Data_ = ptr;
            }

            inline void* Get() const throw () {
                return Data_;
            }

        private:
            void* Data_;
            TDtor Dtor_;
        };

        typedef TIntrusivePtr<TStoredValue> TStoredValueRef;

        inline TStoredValue* Value(const TKey* key) {
            const size_t idx = (size_t)key->Key;

            if (idx >= Values_.size()) {
                Values_.resize(idx + 1);
            }

            TStoredValue*& ret = Values_[idx];

            if (!ret) {
                TStoredValueRef sv(new TStoredValue(key));

                Storage_.push_front(sv);
                ret = sv.Get();
            }

            return ret;
        }

    private:
        yvector<TStoredValue*> Values_;
        ydeque<TStoredValueRef> Storage_;
    };

    inline TPerThreadStorage* MyStorage() {
#if defined(HAVE_FAST_POD_TLS)
        POD_STATIC_THREAD(TPerThreadStorage*) my(0);

        if (!my) {
            my = MyStorageSlow();
        }

        return my;
#else
        return MyStorageSlow();
#endif
    }

    inline TPerThreadStorage* MyStorageSlow() {
        TGuard<TMutex> lock(Lock_);

        return &Datas_[TThread::CurrentThreadId()];
    }

    inline void Cleanup() throw () {
        TGuard<TMutex> lock(Lock_);

        Datas_.erase(TThread::CurrentThreadId());
    }

    static inline TGenericTls* Instance() {
        return Singleton<TGenericTls>();
    }

private:
    TMutex Lock_;
    yhash<TThread::TId, TPerThreadStorage> Datas_;
};

#if defined(_unix_)
class TKey::TImpl {
public:
    inline TImpl(TDtor dtor) {
        pthread_key_create(&Key_, dtor);
    }

    inline ~TImpl() throw () {
        pthread_key_delete(Key_);
    }

    inline void* Get() const {
        return pthread_getspecific(Key_);
    }

    inline void Set(void* val) const {
        pthread_setspecific(Key_, val);
    }

    static inline void Cleanup() {
    }

private:
    pthread_key_t Key_;
};
#else
class TKey::TImpl {
public:
    inline TImpl(TDtor dtor)
        : Key_(dtor)
    {
    }

    inline void* Get() {
        return TGenericTls::Instance()->MyStorage()->Value(&Key_)->Get();
    }

    inline void Set(void* ptr) {
        TGenericTls::Instance()->MyStorage()->Value(&Key_)->Set(ptr);
    }

    static inline void Cleanup() {
        TGenericTls::Instance()->Cleanup();
    }

private:
    TGenericTls::TPerThreadStorage::TKey Key_;
};
#endif

TKey::TKey(TDtor dtor)
    : Impl_(new TImpl(dtor))
{
}

TKey::~TKey() throw () {
}

void* TKey::Get() const {
    return Impl_->Get();
}

void TKey::Set(void* ptr) const {
    Impl_->Set(ptr);
}

void TKey::Cleanup() throw () {
    TImpl::Cleanup();
}
