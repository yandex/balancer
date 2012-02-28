#include "atomic.h"

#include <library/unittest/registar.h>

#include <util/generic/ylimits.h>

template <typename TAtomic>
class TAtomicTest
    : public TTestBase
{
    UNIT_TEST_SUITE(TAtomicTest);
        UNIT_TEST(TestAtomicInc1)
        UNIT_TEST(TestAtomicInc2)
        UNIT_TEST(TestAtomicDec)
        UNIT_TEST(TestAtomicAdd)
        UNIT_TEST(TestAtomicSub)
        UNIT_TEST(TestAtomicSwap)
        UNIT_TEST(TestCAS)
        UNIT_TEST(TestLockUnlock)
    UNIT_TEST_SUITE_END();

private:
    inline void TestLockUnlock() {
        TAtomic v = 0;

        UNIT_ASSERT(AtomicTryLock(&v));
        UNIT_ASSERT(!AtomicTryLock(&v));
        UNIT_ASSERT_VALUES_EQUAL(v, 1);
        AtomicUnlock(&v);
        UNIT_ASSERT_VALUES_EQUAL(v, 0);
    }

    inline void TestCAS() {
        TAtomic v = 0;

        UNIT_ASSERT(AtomicCas(&v, 1, 0));
        UNIT_ASSERT(!AtomicCas(&v, 1, 0));
        UNIT_ASSERT_VALUES_EQUAL(v, 1);
        UNIT_ASSERT(AtomicCas(&v, 0, 1));
        UNIT_ASSERT_VALUES_EQUAL(v, 0);
        UNIT_ASSERT(AtomicCas(&v, Max<intptr_t>(), 0));
        UNIT_ASSERT_VALUES_EQUAL(v, Max<intptr_t>());
    }

    inline void TestAtomicInc1() {
        TAtomic v = 0;

        UNIT_ASSERT(AtomicAdd(v, 1));
        UNIT_ASSERT_VALUES_EQUAL(v, 1);
        UNIT_ASSERT(AtomicAdd(v, 10));
        UNIT_ASSERT_VALUES_EQUAL(v, 11);
    }

    inline void TestAtomicInc2() {
        TAtomic v = 0;

        UNIT_ASSERT(AtomicIncrement(v));
        UNIT_ASSERT_VALUES_EQUAL(v, 1);
        UNIT_ASSERT(AtomicIncrement(v));
        UNIT_ASSERT_VALUES_EQUAL(v, 2);
    }

    inline void TestAtomicDec() {
        TAtomic v = 2;

        UNIT_ASSERT(AtomicDecrement(v));
        UNIT_ASSERT_VALUES_EQUAL(v, 1);
        UNIT_ASSERT(!AtomicDecrement(v));
        UNIT_ASSERT_VALUES_EQUAL(v, 0);
    }

    inline void TestAtomicAdd() {
        TAtomic v = 0;

        UNIT_ASSERT_VALUES_EQUAL(AtomicAdd(v, 1), 1);
        UNIT_ASSERT_VALUES_EQUAL(AtomicAdd(v, 2), 3);
        UNIT_ASSERT_VALUES_EQUAL(AtomicAdd(v, -4), -1);
        UNIT_ASSERT_VALUES_EQUAL(v, -1);
    }

    inline void TestAtomicSub() {
        TAtomic v = 4;

        UNIT_ASSERT_VALUES_EQUAL(AtomicSub(v, 1), 3);
        UNIT_ASSERT_VALUES_EQUAL(AtomicSub(v, 2), 1);
        UNIT_ASSERT_VALUES_EQUAL(AtomicSub(v, 3), -2);
        UNIT_ASSERT_VALUES_EQUAL(v, -2);
    }

    inline void TestAtomicSwap() {
        TAtomic v = 0;

        UNIT_ASSERT_VALUES_EQUAL(AtomicSwap(v, 3), 0);
        UNIT_ASSERT_VALUES_EQUAL(AtomicSwap(v, 5), 3);
        UNIT_ASSERT_VALUES_EQUAL(AtomicSwap(v, -7), 5);
        UNIT_ASSERT_VALUES_EQUAL(AtomicSwap(v, Max<intptr_t>()), -7);
        UNIT_ASSERT_VALUES_EQUAL(v, Max<intptr_t>());
    }
};

// chooses type *other than* T1
template<typename T1, typename T2, typename T3> struct TChooser {
    typedef T2 TdType;
};
template<typename T1, typename T2> struct TChooser<T1, T1, T2> {
    typedef T2 TdType;
};
template<typename T1> struct TChooser<T1, T1, T1> {};

typedef volatile TChooser<TAtomicBase, long, long long>::TdType TAltAtomic;

UNIT_TEST_SUITE_REGISTRATION(TAtomicTest<TAtomic>);
UNIT_TEST_SUITE_REGISTRATION(TAtomicTest<TAltAtomic>);
