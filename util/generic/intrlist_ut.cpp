#include "intrlist.h"

#include <library/unittest/registar.h>

#include <util/stream/ios.h>

class TListTest: public TTestBase {
        UNIT_TEST_SUITE(TListTest);
            UNIT_TEST(TestIterate);
            UNIT_TEST(TestRIterate);
            UNIT_TEST(TestForEach);
            UNIT_TEST(TestForEachWithDelete);
            UNIT_TEST(TestSize);
            UNIT_TEST(TestQuickSort);
            UNIT_TEST(TestCut);
            UNIT_TEST(TestAppend);
        UNIT_TEST_SUITE_END();
    private:
        void TestSize();
        void TestIterate();
        void TestRIterate();
        void TestForEach();
        void TestForEachWithDelete();
        void TestQuickSort();
        void TestCut();
        void TestAppend();
};

UNIT_TEST_SUITE_REGISTRATION(TListTest);

class TInt: public TIntrusiveListItem<TInt> {
    public:
        inline TInt(int value) throw ()
            : Value_(value)
        {
        }

        inline operator int& () throw () {
            return Value_;
        }

        inline operator const int& () const throw () {
            return Value_;
        }

    private:
        int Value_;
};

static inline TOutputStream& operator<< (TOutputStream& o, const TInt& i) {
    return o << (int)i;
}

class TMyList: public TIntrusiveList<TInt> {
    public:
        inline TMyList(int count) {
            while (count > 0) {
                PushFront(new TInt(count--));
            }
        }

        inline ~TMyList() throw () {
            while (!Empty()) {
                delete PopBack();
            }
        }
};

struct TIntGreater: private TGreater<int> {
    inline bool operator() (const TInt& l, const TInt& r) const throw () {
        return TGreater<int>::operator()(l, r);
    }
};

void TListTest::TestQuickSort() {
    TMyList l(1000);
    size_t c = 0;

    l.QuickSort(TIntGreater());

    UNIT_ASSERT_EQUAL(l.Size(), 1000);

    for (TMyList::TIterator it = l.Begin(); it != l.End(); ++it) {
        UNIT_ASSERT_EQUAL(*it, int(1000 - c++));
    }
}

void TListTest::TestSize() {
    TMyList l(1024);

    UNIT_ASSERT_EQUAL(l.Size(), 1024);
}

void TListTest::TestIterate() {
    TMyList l(1000);
    size_t c = 0;

    for (TMyList::TIterator it = l.Begin(); it != l.End(); ++it) {
        ++c;

        UNIT_ASSERT_EQUAL(*it, (int)c);
    }

    UNIT_ASSERT_EQUAL(c, 1000);
}

void TListTest::TestRIterate() {
    TMyList l(1000);
    size_t c = 1000;

    for (TMyList::TReverseIterator it = l.RBegin(); it != l.REnd(); ++it) {
        UNIT_ASSERT_EQUAL(*it, (int)c--);
    }

    UNIT_ASSERT_EQUAL(c, 0);
}

class TSum {
    public:
        inline TSum(size_t& sum)
            : Sum_(sum)
        {
        }

        inline void operator() (const TInt* v) throw () {
            Sum_ += *v;
        }

    private:
        size_t& Sum_;
};

class TSumWithDelete {
    public:
        inline TSumWithDelete(size_t& sum)
            : Sum_(sum)
        {
        }

        inline void operator() (TInt* v) throw () {
            if (*v % 2) {
                Sum_ += *v;
            } else {
                delete v;
            }
        }

    private:
        size_t& Sum_;
};

void TListTest::TestForEach() {
    TMyList l(1000);
    size_t sum = 0;
    TSum functor(sum);

    l.ForEach(functor);

    UNIT_ASSERT_EQUAL(sum, 1000 * 1001 / 2);
}

void TListTest::TestForEachWithDelete() {
    TMyList l(1000);
    size_t sum = 0;
    TSumWithDelete functor(sum);

    l.ForEach(functor);

    UNIT_ASSERT_EQUAL(sum, 500 * 500 /*== n * (x + y * (n - 1) / 2), x == 1, y == 2*/);
}

static void CheckIterationAfterCut(const TMyList& l, const TMyList& l2, size_t N, size_t M) {
    size_t c = 0;
    for (TMyList::TConstIterator it = l.Begin(); it != l.End(); ++it) {
        ++c;

        UNIT_ASSERT_EQUAL(*it, (int)c);
    }

    UNIT_ASSERT_EQUAL(c, M);

    for (TMyList::TConstIterator it = l2.Begin(); it != l2.End(); ++it) {
        ++c;

        UNIT_ASSERT_EQUAL(*it, (int)c);
    }

    UNIT_ASSERT_EQUAL(c, N);

    for (TMyList::TConstIterator it = l2.End(); it != l2.Begin(); --c) {
        --it;

        UNIT_ASSERT_EQUAL(*it, (int)c);
    }

    UNIT_ASSERT_EQUAL(c, M);

    for (TMyList::TConstIterator it = l.End(); it != l.Begin(); --c) {
        --it;

        UNIT_ASSERT_EQUAL(*it, (int)c);
    }

    UNIT_ASSERT_EQUAL(c, 0);
}

static void TestCutFront(int N, int M) {
    TMyList l(N);
    TMyList l2(0);

    TMyList::TIterator it = l.Begin();
    for (int i = 0; i < M; ++i) {
        ++it;
    }

    TMyList::Cut(l.Begin(), it, l2.End());
    CheckIterationAfterCut(l2, l, N, M);
}

static void TestCutBack(int N, int M) {
    TMyList l(N);
    TMyList l2(0);

    TMyList::TIterator it = l.Begin();
    for (int i = 0; i < M; ++i) {
        ++it;
    }

    TMyList::Cut(it, l.End(), l2.End());
    CheckIterationAfterCut(l, l2, N, M);
}

void TListTest::TestCut() {
    TestCutFront(1000, 500);
    TestCutBack(1000, 500);
    TestCutFront(1, 0);
    TestCutBack(1, 0);
    TestCutFront(1, 1);
    TestCutBack(1, 1);
    TestCutFront(2, 0);
    TestCutBack(2, 0);
    TestCutFront(2, 1);
    TestCutBack(2, 1);
    TestCutFront(2, 2);
    TestCutBack(2, 2);
}

static void CheckIterationAfterAppend(const TMyList& l, size_t N, size_t M) {
    TMyList::TConstIterator it = l.Begin();

    for (size_t i = 1; i <= N; ++i, ++it) {
        UNIT_ASSERT_EQUAL((int)i, *it);
    }

    for (size_t i = 1; i <= M; ++i, ++it) {
        UNIT_ASSERT_EQUAL((int)i, *it);
    }

    UNIT_ASSERT_EQUAL(it, l.End());
}

static void TestAppend(int N, int M) {
    TMyList l(N);
    TMyList l2(M);
    l.Append(l2);

    UNIT_ASSERT(l2.Empty());
    CheckIterationAfterAppend(l, N, M);
}

void TListTest::TestAppend() {
    ::TestAppend(500, 500);
    ::TestAppend(0, 0);
    ::TestAppend(1, 0);
    ::TestAppend(0, 1);
    ::TestAppend(1, 1);
}
