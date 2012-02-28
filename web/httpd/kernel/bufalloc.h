#pragma once

#include <util/system/yassert.h>

namespace NSrvKernel {
    template <size_t N>
    struct TBufAlloc {
        char Buf[N];
        char* Cur;

        inline TBufAlloc() throw ()
            : Cur(Buf)
        {
        }

        inline size_t Left() const throw () {
            return Size() - Used();
        }

        inline size_t Used() const throw () {
            return Cur - Buf;
        }

        inline size_t Size() const throw () {
            return sizeof(Buf);
        }

        inline char* Alloc(size_t len) {
            YASSERT(Left() >= len);

            char* ret = Cur;

            Cur += len;

            return ret;
        }
    };

    struct TBufAlloced {
        template <class T>
        inline void* operator new(size_t bytes, T& pool) {
            return pool.Alloc(bytes);
        }

        inline void operator delete(void*, size_t) throw () {
        }

        template <class T>
        inline void operator delete(void*, T&) throw () {
        }
    };
}
