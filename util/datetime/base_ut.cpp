#include "base.h"

#include <limits.h>

#include <library/unittest/registar.h>

#include <util/string/cast.h>
#include <util/stream/output.h>
#include <util/system/compat.h>

class TDateTimeTest : public TTestBase {
    UNIT_TEST_SUITE(TDateTimeTest);
        UNIT_TEST(TestBasicFuncs)
        UNIT_TEST(TestCompatFuncs)
        UNIT_TEST(TestSprintSscan)
        UNIT_TEST(TestNow)
        UNIT_TEST(TestStrftime)
        UNIT_TEST(TestNanoSleep)
        UNIT_TEST(TestGmTimeR)
    UNIT_TEST_SUITE_END();

    TDateTimeTest()
        : T_(987654321)
        , Date_("Thu Apr 19 04:25:21 2001\n")
        , SprintDate_("20010419")
        , SprintSecs_(15921)
    {
        Tm_.tm_sec = 21;
        Tm_.tm_min = 25;
        Tm_.tm_hour = 4;
        Tm_.tm_mday = 19;
        Tm_.tm_mon = 3;
        Tm_.tm_year = 101;
        Tm_.tm_wday = 4;
        Tm_.tm_yday = 108;
    }

    void TestBasicFuncs() {
        ui64 mlsecB = millisec();
        ui64 mcrsecB = MicroSeconds();
        struct timeval tvB;
        gettimeofday(&tvB, 0);

        usleep(100000);

        ui64 mlsecA = millisec();
        ui64 mcrsecA = MicroSeconds();
        struct timeval tvA;
        gettimeofday(&tvA, 0);

        UNIT_ASSERT(mlsecB + 90 < mlsecA);
        UNIT_ASSERT((mcrsecB + 90000 < mcrsecA));
        //UNIT_ASSERT(ToMicroSeconds(&tvB) + 90000 < ToMicroSeconds(&tvA));
        //UNIT_ASSERT(TVdiff(tvB, tvA) == long(ToMicroSeconds(&tvA) - ToMicroSeconds(&tvB)));
    }

    void TestCompatFuncs() {
        struct tm t;
        struct tm* tret;
        tret = gmtime_r(&T_, &t);
        UNIT_ASSERT(tret == &t);
        UNIT_ASSERT(Tm_.tm_sec == t.tm_sec && Tm_.tm_min == t.tm_min && Tm_.tm_hour == t.tm_hour &&
                    Tm_.tm_mday == t.tm_mday && Tm_.tm_mon == t.tm_mon && Tm_.tm_year == t.tm_year &&
                    Tm_.tm_wday == t.tm_wday && Tm_.tm_yday == t.tm_yday);

        /*
         * strptime seems to be broken on Mac OS X:
         *
         *   struct tm t;
         *   char *ret = strptime("Jul", "%b ", &t);
         *   printf("-%s-\n", ret);
         *
         * yields "- -": ret contains a pointer to a substring of the format string,
         * that should never occur: function returns either NULL or pointer to buf substring.
         *
         * So this test fails on Mac OS X.
         */

        struct tm t2;
        Zero(t2);
        char* ret = strptime(Date_, "%a %b %d %H:%M:%S %Y\n ", &t2);
        UNIT_ASSERT(ret == Date_ + strlen(Date_));
        UNIT_ASSERT(Tm_.tm_sec == t2.tm_sec && Tm_.tm_min == t2.tm_min && Tm_.tm_hour == t2.tm_hour &&
            Tm_.tm_mday == t2.tm_mday && Tm_.tm_mon == t2.tm_mon && Tm_.tm_year == t2.tm_year &&
            Tm_.tm_wday == t2.tm_wday);

        time_t t3 = timegm(&t);
        UNIT_ASSERT(t3 == T_);
    }

    void TestSprintSscan() {
        char buf[256];
        long secs;

        sprint_gm_date(buf, T_, &secs);
        UNIT_ASSERT(strcmp(buf, SprintDate_) == 0);
        UNIT_ASSERT(secs == SprintSecs_);

        struct tm t;
        bool ret = sscan_date(buf, t);
        UNIT_ASSERT(ret);
        UNIT_ASSERT(t.tm_year = Tm_.tm_year && t.tm_mon == Tm_.tm_mon && t.tm_mday == Tm_.tm_mday);

        buf[0] = 'a';
        ret = sscan_date(buf, t);
        UNIT_ASSERT(!ret);
    }

    i64 Abs(i64 a) {
        return a >= 0 ? a : -a;
    }

    void TestNow() {
        ui64 seconds = Seconds();
        ui64 milliseconds = millisec();
        ui64 microseconds = MicroSeconds();
        UNIT_ASSERT(Abs(seconds - milliseconds / 1000) <= 1);
        UNIT_ASSERT(Abs(milliseconds - microseconds / 1000) < 100);
        UNIT_ASSERT(seconds > 1243008607); // > time when test was written
    }

    void TestStrftime() {
        struct tm tm;
        Zero(tm);
        tm.tm_year = 109;
        tm.tm_mon = 4;
        tm.tm_mday = 29;
        UNIT_ASSERT_STRINGS_EQUAL("2009-05-29", Strftime("%Y-%m-%d", &tm).c_str());
    }

    void TestNanoSleep() {
        NanoSleep(0);
        NanoSleep(1);
        NanoSleep(1000);
        NanoSleep(1000000);
    }

    bool tmEq(const tm *t0, const tm *t1) {
        return t0 && t1
            && (t0->tm_sec == t1->tm_sec)
            && (t0->tm_min == t1->tm_min)
            && (t0->tm_hour == t1->tm_hour)
            && (t0->tm_mday == t1->tm_mday)
            && (t0->tm_mon == t1->tm_mon)
            && (t0->tm_year == t1->tm_year)
            && (t0->tm_wday == t1->tm_wday)
            && (t0->tm_yday == t1->tm_yday)
            && (t0->tm_isdst == t1->tm_isdst)
#ifndef _win_
            && (t0->tm_gmtoff == t1->tm_gmtoff)
            && (strcmp(t0->tm_zone, t1->tm_zone) == 0)
#endif // _win_
            ;
    }

    void TestGmTimeR() {
        time_t starttime = -(time_t)0xFFFFFFFF * 20;
        time_t finishtime = (time_t)0xFFFFFFFF * 20;
        time_t step = (time_t)0xEFFFFFFF;
        struct tm tms0, tms1;
        struct tm *ptm0, *ptm1;
        for (time_t t = starttime; t < finishtime; t += step) {
            ptm0 = GmTimeR(&t, &tms0);
            UNIT_ASSERT_EQUAL(ptm0, (t < 0 ? NULL : &tms0));

#ifdef _win_
            if (ptm0 && (tms0.tm_year + 1900 > 3000)) {
                // Windows: _MAX__TIME64_T == 23:59:59. 12/31/3000 UTC
                continue;
            }
#endif

            ptm1 = gmtime_r(&t, &tms1);
            if (ptm1 != (t < 0 ? NULL : &tms1))
                break;
            UNIT_ASSERT_EQUAL(ptm1, (t < 0 ? NULL : &tms1));

            if (ptm0) {
                UNIT_ASSERT(tmEq(ptm0, ptm1));
            }
            else {
                UNIT_ASSERT(ptm1 == NULL);
            }
        }
    }

    private:
        const time_t T_;
        const char* Date_;
        const char* SprintDate_;
        const long SprintSecs_;
        struct tm Tm_;
};

UNIT_TEST_SUITE_REGISTRATION(TDateTimeTest);

SIMPLE_UNIT_TEST_SUITE(DateTimeTest) {

    SIMPLE_UNIT_TEST(TestDurationFromFloat) {
        UNIT_ASSERT_EQUAL(TDuration::MilliSeconds(500), TDuration::Seconds(0.5));
        UNIT_ASSERT_EQUAL(TDuration::MilliSeconds(500), TDuration::Seconds(0.5f));
    }

    SIMPLE_UNIT_TEST(TestSecondsLargeValue) {
        unsigned int seconds = UINT_MAX;
        UNIT_ASSERT_VALUES_EQUAL(((ui64) seconds) * 1000000, TDuration::Seconds(seconds).MicroSeconds());
    }

    SIMPLE_UNIT_TEST(TestToString) {
#define CHECK_CONVERTIBLE(v) UNIT_ASSERT_VALUES_EQUAL(v, ToString(TDuration::Parse(v)))
#if 0

        CHECK_CONVERTIBLE("10s");
        CHECK_CONVERTIBLE("1234s");
        CHECK_CONVERTIBLE("1234ms");
        CHECK_CONVERTIBLE("12ms");
        CHECK_CONVERTIBLE("12us");
        CHECK_CONVERTIBLE("1234us");
#endif

        CHECK_CONVERTIBLE("1.000000s");
        CHECK_CONVERTIBLE("11234.000000s");
        CHECK_CONVERTIBLE("0.011122s");
        CHECK_CONVERTIBLE("33.011122s");
    }

    SIMPLE_UNIT_TEST(TestSleep) {
        // check does not throw
        Sleep(TDuration::Seconds(0));
        Sleep(TDuration::MicroSeconds(1));
        Sleep(TDuration::MilliSeconds(1));
    }

    SIMPLE_UNIT_TEST(TestInstantToString) {
        UNIT_ASSERT_VALUES_EQUAL(Stroka("2009-08-06T15:19:06.023455Z"), ToString(TInstant::Seconds(1249571946) + TDuration::MicroSeconds(23455)));
        UNIT_ASSERT_VALUES_EQUAL(Stroka("2009-08-06T15:19:06.023455Z"), (TInstant::Seconds(1249571946) + TDuration::MicroSeconds(23455)).ToString());
        UNIT_ASSERT_VALUES_EQUAL(Stroka("2009-08-06T15:19:06Z"), (TInstant::Seconds(1249571946) + TDuration::MicroSeconds(23455)).ToStringUpToSeconds());
    }
}
