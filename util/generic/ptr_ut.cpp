#include "ptr.h"

#include <library/unittest/registar.h>

#include <util/stream/ios.h>
#include <util/generic/noncopyable.h>
#include <util/generic/vector.h>

class TPtrTest: public TTestBase {
        UNIT_TEST_SUITE(TPtrTest);
            UNIT_TEST(TestSimpleIntrPtr);
            UNIT_TEST(TestHolderPtr);
            UNIT_TEST(TestTrulePtr);
            UNIT_TEST(TestLinkedPtr);
            UNIT_TEST(TestCopyPtr);
            UNIT_TEST(TestIntrPtr);
        UNIT_TEST_SUITE_END();

    private:
        void TestSimpleIntrPtr();
        void TestHolderPtr();
        void TestTrulePtr();
        void TestLinkedPtr();
        void TestCopyPtr();
        void TestIntrPtr();
};

UNIT_TEST_SUITE_REGISTRATION(TPtrTest);

static int cnt = 0;

class A: public TRefCounted<A> {
    public:
        inline A() {
            //Cerr << "A()" << Endl;
            ++cnt;
        }

        inline A(const A&)
            : TRefCounted<A>(*this)
        {
            //Cerr << "A(const A&)" << Endl;
            ++cnt;
        }

        inline ~A() {
            //Cerr << "~A()" << Endl;
            --cnt;
        }
};

static A* newA() {
    return new A();
}

/*
 * test compileability
 */
class B;
static TSimpleIntrusivePtr<B> getB() {
    throw 1;
}

void func() {
    TSimpleIntrusivePtr<B> b = getB();
}

void TPtrTest::TestSimpleIntrPtr() {
    {
        TSimpleIntrusivePtr<A> a1(newA());
        TSimpleIntrusivePtr<A> a2(newA());
        TSimpleIntrusivePtr<A> a3 = a2;

        a1 = a2;
        a2 = a3;
    }

    UNIT_ASSERT_EQUAL(cnt, 0);
}

void TPtrTest::TestLinkedPtr() {
    {
        TLinkedPtr<A> a1(newA());
        TLinkedPtr<A> a2(newA());
        TLinkedPtr<A> a3 = a2;

        a1 = a2;
        a2 = a3;
    }

    UNIT_ASSERT_EQUAL(cnt, 0);
}

void TPtrTest::TestHolderPtr() {
    {
        THolder<A> a1(newA());
        THolder<A> a2(a1.Release());
    }

    UNIT_ASSERT_EQUAL(cnt, 0);
}

void TPtrTest::TestTrulePtr() {
    {
        TAutoPtr<A> a1(newA());
        TAutoPtr<A> a2(a1);
        a1 = a2;
    }

    UNIT_ASSERT_EQUAL(cnt, 0);
}

void TPtrTest::TestCopyPtr() {
    TCopyPtr<A> a1(newA());
    {
        TCopyPtr<A> a2(newA());
        TCopyPtr<A> a3 = a2;
        UNIT_ASSERT_EQUAL(cnt, 3);

        a1 = a2;
        a2 = a3;
    }
    UNIT_ASSERT_EQUAL(cnt, 1);
    a1.Destroy();

    UNIT_ASSERT_EQUAL(cnt, 0);
}

class TOp : public TRefCounted<TOp>
          , public TNonCopyable
{
public:
    static int Cnt;
public:
    TOp() {
        ++Cnt;
    }
    virtual ~TOp(){
        --Cnt;
    }
};

int TOp::Cnt = 0;

class TOp2 : public TOp {
public:
    TIntrusivePtr<TOp> Op;
public:
    TOp2(const TIntrusivePtr<TOp>& op)
        : Op(op)
    {
        ++Cnt;
    }
    ~TOp2() {
        --Cnt;
    }
};

class TOp3 {
public:
    TIntrusivePtr<TOp2> Op2;
};

void Attach(TOp3* op3, TIntrusivePtr<TOp>* op) {
    TIntrusivePtr<TOp2> op2 = new TOp2(*op);
    op3->Op2 = op2.Get();
    *op = op2.Get();
}

void TPtrTest::TestIntrPtr() {
    {
        TIntrusivePtr<TOp> p, p2;
        TOp3 op3;
        {
            yvector<TIntrusivePtr<TOp> > f1;
            {
                yvector<TIntrusivePtr<TOp> > f2;
                f2.push_back(new TOp);
                p = new TOp;
                f2.push_back(p);
                Attach(&op3, &f2[1]);
                f1 = f2;
                UNIT_ASSERT_EQUAL(f1[0]->RefCount(), 2);
                UNIT_ASSERT_EQUAL(f1[1]->RefCount(), 3);
                UNIT_ASSERT_EQUAL(f1[1].Get(), op3.Op2.Get());
                UNIT_ASSERT_EQUAL(op3.Op2->RefCount(), 3);
                UNIT_ASSERT_EQUAL(op3.Op2->Op->RefCount(), 2);
                UNIT_ASSERT_EQUAL(TOp::Cnt, 4);
            }
            p2 = p;
        }
        UNIT_ASSERT_EQUAL(op3.Op2->RefCount(), 1);
        UNIT_ASSERT_EQUAL(op3.Op2->Op->RefCount(), 3);
        UNIT_ASSERT_EQUAL(TOp::Cnt, 3);
    }
    UNIT_ASSERT_EQUAL(TOp::Cnt, 0);
}

