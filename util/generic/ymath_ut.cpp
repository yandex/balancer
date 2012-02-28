#include "ymath.h"

#include <limits>

#include <library/unittest/registar.h>

#include <util/stream/ios.h>
#include <util/datetime/cputimer.h>

template <class T>
static inline T SlowClp2(T t) throw () {
    if (t) {
        T ret = 1;

        while (ret < t) {
            ret *= 2;
        }

        return ret;
    }

    return 0;
}

class TMathTest: public TTestBase {
        UNIT_TEST_SUITE(TMathTest);
            UNIT_TEST(TestClp2)
            UNIT_TEST(TestClpSimple)
            UNIT_TEST(TestSqr)
            UNIT_TEST(TestLog2)
            UNIT_TEST(TestLog2f)
            UNIT_TEST(ValueBitCount)
            UNIT_TEST(TestErf);
        UNIT_TEST_SUITE_END();
    private:
        void TestClp2();
        void TestSqr();
        void TestErf();

        inline void TestClpSimple() {
            UNIT_ASSERT_EQUAL(FastClp2<ui32>(12), 16);
            UNIT_ASSERT_EQUAL(FastClp2<ui16>(11), 16);
            UNIT_ASSERT_EQUAL(FastClp2<ui8>(10), 16);
        }

        inline void TestLog2() {
            UNIT_ASSERT_DOUBLES_EQUAL(Log2(2.0), 1.0, 1e-10);
        }

        inline void TestLog2f() {
            UNIT_ASSERT_DOUBLES_EQUAL(Log2f(2.0), 1.0f, 1e-7f);
        }

        inline void ValueBitCount() {
            UNIT_ASSERT_VALUES_EQUAL(GetValueBitCount(1), 1u);
            UNIT_ASSERT_VALUES_EQUAL(GetValueBitCount(2), 2u);
            UNIT_ASSERT_VALUES_EQUAL(GetValueBitCount(3), 2u);
            UNIT_ASSERT_VALUES_EQUAL(GetValueBitCount(257), 9u);
        }
};

UNIT_TEST_SUITE_REGISTRATION(TMathTest);

void TMathTest::TestSqr() {
    UNIT_ASSERT_EQUAL(Sqr(2), 4);
    UNIT_ASSERT_EQUAL(Sqr(2.0), 4.0);
}

void TMathTest::TestClp2() {
    for (ui8 i = 0; i < 127; ++i) {
        UNIT_ASSERT_EQUAL(SlowClp2(i), FastClp2(i));
    }

    for (ui16 i = 0; i < 255; ++i) {
        UNIT_ASSERT_EQUAL(SlowClp2(i), FastClp2(i));
    }

    for (ui32 i = 0; i < 255; ++i) {
        UNIT_ASSERT_EQUAL(SlowClp2(i), FastClp2(i));
    }

    for (ui64 i = 0; i < 255; ++i) {
        UNIT_ASSERT_EQUAL(SlowClp2(i), FastClp2(i));
    }

    if (0) {
        {
            TFuncTimer timer("fast");
            size_t ret = 0;

            for (size_t i = 0; i < 10000000; ++i) {
                ret += FastClp2(i);
            }

            Cerr << ret << Endl;
        }

        {
            TFuncTimer timer("slow");
            size_t ret = 0;

            for (size_t i = 0; i < 10000000; ++i) {
                ret += SlowClp2(i);
            }

            Cerr << ret << Endl;
        }
    }
}

void TMathTest::TestErf() {
    static const double a = -5.0;
    static const double b =  5.0;
    static const int n = 50;
    static const double step = (b - a) / n;

    static const double values[n + 1] = {
        -1.0000000, -1.0000000, -1.0000000, -1.0000000, -1.0000000,
        -1.0000000, -0.9999999, -0.9999996, -0.9999985, -0.9999940,
        -0.9999779, -0.9999250, -0.9997640, -0.9993115, -0.9981372,
        -0.9953223, -0.9890905, -0.9763484, -0.9522851, -0.9103140,
        -0.8427008, -0.7421010, -0.6038561, -0.4283924, -0.2227026,
         0.0000000,
         0.2227026,  0.4283924,  0.6038561,  0.7421010,  0.8427008,
         0.9103140,  0.9522851,  0.9763484,  0.9890905,  0.9953223,
         0.9981372,  0.9993115,  0.9997640,  0.9999250,  0.9999779,
         0.9999940,  0.9999985,  0.9999996,  0.9999999,  1.0000000,
         1.0000000,  1.0000000,  1.0000000,  1.0000000,  1.0000000
    };

    double x = a;
    for (int i = 0; i <= n; ++i, x += step) {
        double f = Erf(x);
        UNIT_ASSERT_DOUBLES_EQUAL(f, values[i], 1e-7);
    }
}
