#pragma once

#include <util/system/atomic.h>
#include <util/system/defaults.h>
#include <util/system/guard.h>

class TSimpleCounter {
    public:
        inline TSimpleCounter(long initial = 0) throw ()
            : Counter_(initial)
        {
        }

        inline ~TSimpleCounter() throw () {
        }

        inline void Inc() throw () {
            ++Counter_;
        }

        inline long Dec() throw () {
            return --Counter_;
        }

        inline long Val() const throw () {
            return Counter_;
        }

    private:
        long Counter_;
};

class TAtomicCounter {
    public:
        inline TAtomicCounter(long initial = 0) throw ()
            : Counter_(initial)
        {
        }

        inline ~TAtomicCounter() throw () {
        }

        inline void Inc() throw () {
            AtomicIncrement(Counter_);
        }

        inline TAtomicBase Dec() throw () {
            return AtomicDecrement(Counter_);
        }

        inline TAtomicBase Val() const throw () {
            return AtomicGet(Counter_);
        }

    private:
        TAtomic Counter_;
};

template<>
struct TCommonLockOps<TAtomicCounter> {
    static inline void Acquire(TAtomicCounter* t) throw () {
        t->Inc();
    }
    static inline void Release(TAtomicCounter* t) throw () {
        t->Dec();
    }
};

template <bool threadSafe>
struct TRefCounterTraits;

template <>
struct TRefCounterTraits<true> {
    typedef TAtomicCounter TResult;
};

template <>
struct TRefCounterTraits<false> {
    typedef TSimpleCounter TResult;
};

template <bool threadSafe>
class TRefCounter {
    public:
        inline TRefCounter(long initval = 0) throw ()
            : Counter_(initval)
        {
        }

        inline ~TRefCounter() throw () {
        }

        inline void Inc() throw () {
            Counter_.Inc();
        }

        inline long Dec() throw () {
            return Counter_.Dec();
        }

        inline long Val() const throw () {
            return Counter_.Val();
        }

    private:
        typename TRefCounterTraits<threadSafe>::TResult Counter_;
};
