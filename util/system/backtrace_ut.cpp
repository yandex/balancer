#include "backtrace.h"

#include <library/unittest/registar.h>

#include <util/stream/ios.h>

typedef int (*PFunc)(void**, size_t);

int Dbg1(void** buf, size_t len) {
    volatile int ret = (int)BackTrace(buf, len);
    return ret;
}

int Dbg2(void** buf, size_t len) {
    volatile int ret = (int)BackTrace(buf, len);
    return ret;
}

class TBackTraceTest : public TTestBase {
    UNIT_TEST_SUITE(TBackTraceTest);
        UNIT_TEST(TestBackTrace)
        UNIT_TEST(TestPrintBackTrace)
    UNIT_TEST_SUITE_END();

    void SomeMethod() {
        TStringStream out;

        FormatBackTrace(&out);

        UNIT_ASSERT(out.empty() || out.find("FormatBackTrace") != Stroka::npos);
    }

    void TestPrintBackTrace() {
        SomeMethod();
    }

    void TestBackTrace() {
        //PrintBackTrace();
        void* buf1[100];
        size_t ret1;

        void* buf2[100];
        size_t ret2;

        volatile PFunc func = &Dbg1;
        ret1 = (*func)(buf1, 100);
        func = &Dbg2;
        ret2 = (*func)(buf2, 100);

        UNIT_ASSERT_EQUAL(ret1, ret2);

        for (size_t i = 0; i < ret1; ++i) {
            if (buf1[i] != buf2[i]) {
                UNIT_ASSERT(ui64(buf1[i]) < ui64((void*)&Dbg1) + 100);
                UNIT_ASSERT(ui64(buf1[i]) > ui64((void*)&Dbg1) - 100);
                break;
            }
        }

    }
};

UNIT_TEST_SUITE_REGISTRATION(TBackTraceTest);
