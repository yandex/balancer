#pragma once

#include "defaults.h"

#include <new>

#include <util/generic/ptr.h>
#include <util/generic/noncopyable.h>

#if defined(_mt_)
    #define THREAD(T)            ::NTls::TValue< T >
    #define STATIC_THREAD(T)     static ::NTls::TValue< T >
#else
    #define THREAD(T)            T
    #define STATIC_THREAD(T)     static T
#endif

// gcc and msvc support automatic tls for POD types
#if defined _mt_ && !defined(DISABLE_THRKEY_OPTIMIZATION)
    #if defined __GNUC__ && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ > 2)) && !(defined __FreeBSD__ && __FreeBSD__ < 5) && !defined(_cygwin_) && !defined(_darwin_)
        #define POD_THREAD(T) __thread T
        #define POD_STATIC_THREAD(T) static __thread T
    // msvc doesn't support __declspec(thread) in dlls, loaded manually (via LoadLibrary)
    #elif defined(_MSC_VER) && !defined(_WINDLL)
        #define POD_THREAD(T)        __declspec(thread) T
        #define POD_STATIC_THREAD(T) __declspec(thread) static T
    #endif
#endif

#if !defined(POD_THREAD) || !defined(POD_STATIC_THREAD)
    #define POD_THREAD(T) THREAD(T)
    #define POD_STATIC_THREAD(T) STATIC_THREAD(T)
#else
    #define HAVE_FAST_POD_TLS
#endif

namespace NTls {
    typedef void (*TDtor)(void*);

    class TKey {
    public:
        TKey(TDtor dtor);
        ~TKey() throw ();

        void* Get() const;
        void Set(void* ptr) const;

        static void Cleanup() throw ();

    private:
        class TImpl;
        THolder<TImpl> Impl_;
    };

    struct TCleaner {
        inline ~TCleaner() throw () {
            TKey::Cleanup();
        }
    };

    template <class T>
    class TValue: TNonCopyable {
        class TConstructor {
        public:
            TConstructor() throw () {
            }

            virtual ~TConstructor() throw () {
            }

            virtual T* Construct(void* ptr) const = 0;
        };

        class TDefaultConstructor: public TConstructor {
        public:
            virtual ~TDefaultConstructor() throw () {
            }

            virtual T* Construct(void* ptr) const {
                memset(ptr, 0, sizeof(T));
                return new (ptr) T;
            }
        };

        template <class T1>
        class TCopyConstructor: public TConstructor {
        public:
            inline TCopyConstructor(const T1& value)
                : Value(value)
            {
            }

            virtual ~TCopyConstructor() throw () {
            }

            virtual T* Construct(void* ptr) const {
                return new (ptr) T(Value);
            }

        private:
            T1 Value;
        };
    public:
        inline TValue()
            : Constructor_(new TDefaultConstructor())
            , Key_(Dtor)
        {
        }

        template <class T1>
        inline TValue(const T1& value)
            : Constructor_(new TCopyConstructor<T1>(value))
            , Key_(Dtor)
        {
        }

        template <class T1>
        inline T& operator= (const T1& val) {
            return Get() = val;
        }

        inline operator const T& () const {
            return Get();
        }

        inline operator T& () {
            return Get();
        }

        inline const T& operator->() const {
            return Get();
        }

        inline T& operator->() {
            return Get();
        }

        inline const T* operator&() const {
            return GetPtr();
        }

        inline T* operator&() {
            return GetPtr();
        }

        inline T& Get() const {
            return *GetPtr();
        }

        inline T* GetPtr() const {
            T* val = static_cast<T*>(Key_.Get());

            if (!val) {
                val = Constructor_->Construct(::operator new(sizeof(T)));
                Key_.Set((void*)val);
            }

            return val;
        }

    private:
        static void Dtor(void* ptr) {
            delete (T*)ptr;
        }

    private:
        THolder<TConstructor> Constructor_;
        TKey Key_;
    };
}
