#include "ylimits.h"

#include <library/unittest/registar.h>

#include <util/string/cast.h>
#include <util/system/valgrind.h>

class TLimitTest: public TTestBase {
        UNIT_TEST_SUITE(TLimitTest);
            UNIT_TEST(TestLimits);
            UNIT_TEST(TestNan);
        UNIT_TEST_SUITE_END();

    protected:
        void TestLimits();
        void TestNan();
};

UNIT_TEST_SUITE_REGISTRATION(TLimitTest);

#define CHECK_COND(X) UNIT_ASSERT(X)

static inline bool ValidSignInfo(bool, bool) {
    return true;
}

template <class T>
static inline bool ValidSignInfo(bool limitIsSigned, const T&) {
    return limitIsSigned && IsNegative(T(-1)) || !limitIsSigned && !IsNegative(T(-1));
}

template <class T>
static inline bool TestIntegralLimits(const T&, bool unknownSign = true, bool isSigned = true) {
    typedef TNumericLimits<T> lim;

    CHECK_COND(lim::is_specialized);
    CHECK_COND(lim::is_integer);
    CHECK_COND(lim::min() < lim::max());
    CHECK_COND((unknownSign && ((lim::is_signed && (lim::min() != 0)) || (!lim::is_signed && (lim::min() == 0)))) ||
        (!unknownSign && ((lim::is_signed && isSigned) || (!lim::is_signed && !isSigned))));

    T min = Min();
    UNIT_ASSERT_EQUAL(lim::min(), min);
    T max = Max();
    UNIT_ASSERT_EQUAL(lim::max(), max);

    if (unknownSign) {
        CHECK_COND(ValidSignInfo(lim::is_signed, T()));
    }

    return true;
}

template <class T>
static inline bool TestSignedIntegralLimits(const T& val) {
    return TestIntegralLimits(val, false, true);
}

template <class T>
static inline bool TestUnsignedIntegralLimits(const T& val) {
    return TestIntegralLimits(val, false, false);
}

template <class T>
static inline bool TestFloatLimits(const T&) {
    typedef TNumericLimits<T> lim;

    CHECK_COND(lim::is_specialized);
    CHECK_COND(!lim::is_modulo);
    CHECK_COND(!lim::is_integer);
    CHECK_COND(lim::is_signed);

    CHECK_COND(lim::max() > 1000);
    CHECK_COND(lim::min() > 0);
    CHECK_COND(lim::min() < 0.001);
    CHECK_COND(lim::epsilon() > 0);

    if (lim::is_iec559) {
        CHECK_COND(lim::has_infinity);
        CHECK_COND(lim::has_quiet_NaN);
        CHECK_COND(lim::has_signaling_NaN);
    }

    if (lim::has_infinity) {
        const T infinity = lim::infinity();

        CHECK_COND(infinity > lim::max());
        CHECK_COND(-infinity < -lim::max());
    }

    return true;
}

template <class T>
static inline bool TestNan(const T&) {
    typedef TNumericLimits<T> lim;

    if (lim::has_quiet_NaN) {
        const T qnan = lim::quiet_NaN();

        CHECK_COND(!(qnan == 42));
        CHECK_COND(!(qnan == qnan));
        CHECK_COND(qnan != 42);
        CHECK_COND(qnan != qnan);
    }

    return true;
}

void TLimitTest::TestLimits() {
    UNIT_ASSERT(TestIntegralLimits(bool()));
    UNIT_ASSERT(TestIntegralLimits(char()));
    typedef signed char signed_char;
    UNIT_ASSERT(TestSignedIntegralLimits(signed_char()));
    typedef unsigned char unsigned_char;
    UNIT_ASSERT(TestUnsignedIntegralLimits(unsigned_char()));
    UNIT_ASSERT(TestSignedIntegralLimits(short()));
    typedef unsigned short unsigned_short;
    UNIT_ASSERT(TestUnsignedIntegralLimits(unsigned_short()));
    UNIT_ASSERT(TestSignedIntegralLimits(int()));
    typedef unsigned int unsigned_int;
    UNIT_ASSERT(TestUnsignedIntegralLimits(unsigned_int()));
    UNIT_ASSERT(TestSignedIntegralLimits(long()));
    typedef unsigned long unsigned_long;
    UNIT_ASSERT(TestUnsignedIntegralLimits(unsigned_long()));
    typedef long long long_long;
    UNIT_ASSERT(TestSignedIntegralLimits(long_long()));
    typedef unsigned long long unsigned_long_long;
    UNIT_ASSERT(TestUnsignedIntegralLimits(unsigned_long_long()));
    UNIT_ASSERT(TestFloatLimits(float()));
    UNIT_ASSERT(TestFloatLimits(double()));
    typedef long double long_double;
    UNIT_ASSERT(RUNNING_ON_VALGRIND || TestFloatLimits(long_double()));
}

void TLimitTest::TestNan() {
    UNIT_ASSERT(::TestNan(float()));
    UNIT_ASSERT(::TestNan(double()));
    typedef long double long_double;
    UNIT_ASSERT(::TestNan(long_double()));
}
