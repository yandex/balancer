#pragma once

#include <new>

#include <util/system/align.h>
#include <util/system/atexit.h>
#include <util/system/defaults.h>
#include <util/system/spinlock.h>

template <class T>
void Destroyer(void* ptr) {
    UNUSED(ptr);
    ((T*)ptr)->~T();
}

template <class T>
struct TSingletonTraits {
    static const size_t Priority = 65536;
};

template <class T>
T* SingletonInt() {
    static T* volatile ret;
    static char tBuf[sizeof(T) + PLATFORM_DATA_ALIGN];

    if (!ret) {
        static TAtomic lock;
        TGuard<TAtomic> guard(lock);

        if (!ret) {
            char* buf = AlignUp((char*)tBuf);
            T* res = new (buf) T;

            try {
                AtExit(Destroyer<T>, res, TSingletonTraits<T>::Priority);
            } catch (...) {
                Destroyer<T>(res);

                throw;
            }

            ret = res;
        }
    }

    return (T*)ret;
}

template <class T>
T* Singleton() {
    static T* ret;
    if (EXPECT_FALSE(!ret))
        ret = SingletonInt<T>();
    return ret;
}

template<class X>
const X& Default() {
    return *Singleton<X>();
}

