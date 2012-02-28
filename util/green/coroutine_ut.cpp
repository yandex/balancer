#include <library/unittest/registar.h>

#include <util/string/cast.h>

#include "impl.h"

class TCoroTest: public TTestBase {
        UNIT_TEST_SUITE(TCoroTest);
            UNIT_TEST(TestSimpleX1);
            UNIT_TEST(TestSimpleX2);
            UNIT_TEST(TestSimpleX3);
            UNIT_TEST(TestMemFun);
            UNIT_TEST(TestMutex);
            UNIT_TEST(TestCondVar);
        UNIT_TEST_SUITE_END();
    public:
        void TestSimpleX1();
        void TestSimpleX2();
        void TestSimpleX3();
        void TestMemFun();
        void TestMutex();
        void TestCondVar();
};

static int i;

static void CoRun(TCont* c, void* /*run*/) {
    while (i < 100000) {
        ++i;
        c->Yield();
    }
}

static void CoMain(TCont* c, void* /*arg*/) {
    for (volatile size_t i = 0; i < 10; ++i) {
        c->Executor()->Create(CoRun, 0, "run");
    }
}

struct TTestObject {
    TTestObject()
        : i()
        , j()
        {}

    int i, j;
    void RunTask1(TCont*) { i = 1; }
    void RunTask2(TCont*) { j = 2; }
};

void TCoroTest::TestSimpleX1() {
    i = 0;
    TContExecutor e(32000);
    e.Execute(CoMain);
    UNIT_ASSERT_EQUAL(i, 100000);
}

void TCoroTest::TestMemFun() {
    i = 0;
    TContExecutor e(32000);
    TTestObject obj;
    e.Create<TTestObject, &TTestObject::RunTask1>(&obj, "test1");
    e.Execute<TTestObject, &TTestObject::RunTask2>(&obj);
    UNIT_ASSERT_EQUAL(obj.i, 1);
    UNIT_ASSERT_EQUAL(obj.j, 2);
}

void TCoroTest::TestSimpleX2() {
    {
        i = 0;

        {
            TContExecutor e(32000);
            e.Execute(CoMain);
        }

        UNIT_ASSERT_EQUAL(i, 100000);
    }

    {
        i = 0;

        {
            TContExecutor e(32000);
            e.Execute(CoMain);
        }

        UNIT_ASSERT_EQUAL(i, 100000);
    }
}

struct TRunner {
    inline TRunner()
        : Runs(0)
    {
    }

    inline void operator() (TCont* c) {
        ++Runs;
        c->Yield();
    }

    size_t Runs;
};

void TCoroTest::TestSimpleX3() {
    TContExecutor e(32000);
    TRunner runner;

    for (volatile size_t i = 0; i < 1000; ++i) {
        e.Create(runner, "runner");
    }

    e.Execute();

    UNIT_ASSERT_EQUAL(runner.Runs, 1000);
}

static Stroka res;
static TContMutex mutex;

static void CoMutex(TCont* c, void* /*run*/) {
    {
        mutex.LockI(c);
        c->Yield();
        res += c->Name();
        mutex.UnLock();
    }

    c->Yield();

    {
        mutex.LockI(c);
        c->Yield();
        res += c->Name();
        mutex.UnLock();
    }
}

static void CoMutexTest(TCont* c, void* /*run*/) {
    c->Executor()->Create(CoMutex, 0, "1");
    c->Executor()->Create(CoMutex, 0, "2");
}

void TCoroTest::TestMutex() {
    TContExecutor e(32000);
    e.Execute(CoMutexTest);
    UNIT_ASSERT_EQUAL(res, "1212");
    res.clear();
}

static TContMutex m1;
static TContCondVar c1;

static void CoCondVar(TCont* c, void* /*run*/) {
    for (size_t i = 0; i < 3; ++i) {
        UNIT_ASSERT_EQUAL(m1.LockI(c), 0);
        UNIT_ASSERT_EQUAL(c1.WaitI(c, &m1), 0);
        res += c->Name();
        m1.UnLock();
    }
}

static void CoCondVarTest(TCont* c, void* /*run*/) {
    c->Executor()->Create(CoCondVar, 0, "1");
    c->Yield();
    c->Executor()->Create(CoCondVar, 0, "2");
    c->Yield();
    c->Executor()->Create(CoCondVar, 0, "3");
    c->Yield();
    c->Executor()->Create(CoCondVar, 0, "4");
    c->Yield();
    c->Executor()->Create(CoCondVar, 0, "5");
    c->Yield();
    c->Executor()->Create(CoCondVar, 0, "6");
    c->Yield();

    for (size_t i = 0; i < 3; ++i) {
        res += ToString((size_t)i) + "^";
        c1.BroadCast();
        c->Yield();
    }
}

void TCoroTest::TestCondVar() {
    TContExecutor e(32000);
    e.Execute(CoCondVarTest);
    UNIT_ASSERT_EQUAL(res, "0^1234561^1234562^123456");
    res.clear();
}

UNIT_TEST_SUITE_REGISTRATION(TCoroTest);
