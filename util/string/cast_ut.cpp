#include "cast.h"

#include <library/unittest/registar.h>

#include <util/charset/wide.h>

#define test1(t, v)\
    F<t>().R1(v);

#define test2(t, v)\
    F<t>().R2(v);

#define EPS 10E-7

#define HEX_MACROS_MAP(mac, type, val) mac(type, val, 2) mac(type, val, 3) mac(type, val, 4) \
    mac(type, val, 5) mac(type, val, 6) mac(type, val, 7) mac(type, val, 8) \
    mac(type, val, 9) mac(type, val, 10) mac(type, val, 11) mac(type, val, 12) \
    mac(type, val, 13) mac(type, val, 14) mac(type, val, 15) mac(type, val, 16)

#define R1_HEX_CHECK(type, val, base) UNIT_ASSERT_EQUAL((IntFromStringForCheck<base>(IntToString<base>(val))), val);
#define R2_HEX_CHECK(type, val, base) UNIT_ASSERT_EXCEPTION((IntFromString<type, base>(IntToString<base>(val))), yexception);


template <class A>
struct TRet {
    template <int base>
    inline A IntFromStringForCheck(const Stroka& str) {
        return IntFromString<A, base>(str);
    }

    template <class B>
    inline void R1(B v) {
        UNIT_ASSERT_EQUAL(FromString<A>(ToString(v)),v);
        HEX_MACROS_MAP(R1_HEX_CHECK, A, v);
    }

    template <class B>
    inline void R2(B v) {
        UNIT_ASSERT_EXCEPTION(FromString<A>(ToString(v)), yexception);
        HEX_MACROS_MAP(R2_HEX_CHECK, A, v);
    }
};

template <>
struct TRet<bool> {
    template <class B>
    inline void R1(B v) {
        UNIT_ASSERT_EQUAL(FromString<bool>(ToString(v)),v);
    }

    template <class B>
    inline void R2(B v) {
        UNIT_ASSERT_EXCEPTION(FromString<bool>(ToString(v)), yexception);
    }
};

template <class A>
inline TRet<A> F() {
    return TRet<A>();
};


class TCastTest: public TTestBase  {
        template <class A>
        inline TRet<A> F() {
            return TRet<A>();
        };

        template <class TFloat>
        struct TFromStringCaller {
            TFromStringCaller(const char* str) {
                FromString<TFloat>(str);
            }
            void operator ()() {}
        };

        template <class TFloat>
        TFromStringCaller<TFloat> TestFloatExc(const char* str) {
            return TFromStringCaller<TFloat>(str);
        }

    private:
        UNIT_TEST_SUITE(TCastTest);
            UNIT_TEST(TestToFrom);
            //UNIT_TEST(TestFloats);
            UNIT_TEST(TestReadFloats);
            UNIT_TEST(TestStrToD);
            UNIT_TEST(TestLiteral);
            UNIT_TEST(TestVolatile);
            UNIT_TEST(TestAutoDetectType);


#if 0
            test1(long long int, LLONG_MAX)
            test1(long long int, LLONG_MIN)
            test1(long long int, LLONG_MAX - 1)
            test1(long long int, LLONG_MIN + 1)
#endif
        UNIT_TEST_SUITE_END();

    private:
        inline void TestToFrom() {
            test1(bool, true);
            test1(bool, false);
            test2(bool, "");
            test2(bool, "a");

            test1(int, -1)
            test2(unsigned int, -1)
            test1(short int, -1)
            test2(unsigned short int, -1)
            test1(long int, -1)
            test2(unsigned long int, -1)
            test1(int, INT_MAX)
            test1(int, INT_MIN)
            test1(int, INT_MAX - 1)
            test1(int, INT_MIN + 1)
            test2(int, (long long int)INT_MAX + 1)
            test2(int, (long long int)INT_MIN - 1)
            test1(unsigned int, UINT_MAX)
            test1(unsigned int, UINT_MAX - 1)
            test2(unsigned int, (long long int)UINT_MAX + 1)
            test1(short int, SHRT_MAX)
            test1(short int, SHRT_MIN)
            test1(short int, SHRT_MAX - 1)
            test1(short int, SHRT_MIN + 1)
            test2(short int, (long long int)SHRT_MAX + 1)
            test2(short int, (long long int)SHRT_MIN - 1)
            test1(unsigned short int, USHRT_MAX)
            test1(unsigned short int, USHRT_MAX - 1)
            test2(unsigned short int, (long long int)USHRT_MAX + 1)
            test1(long int, LONG_MAX)
            test1(long int, LONG_MIN)
            test1(long int, LONG_MAX - 1)
            test1(long int, LONG_MIN + 1)
        }

        inline void TestVolatile() {
            UNIT_ASSERT_EQUAL(ToString((volatile int)1), "1");
        }

        inline void TestStrToD() {
            UNIT_ASSERT_DOUBLES_EQUAL(StrToD("1.1", 0), 1.1, EPS);
            UNIT_ASSERT_DOUBLES_EQUAL(StrToD("1.12345678", 0), 1.12345678, EPS);
            UNIT_ASSERT_DOUBLES_EQUAL(StrToD("10E-5", 0), 10E-5, EPS);
            UNIT_ASSERT_DOUBLES_EQUAL(StrToD("1.1E+5", 0), 1.1E+5, EPS);

            char* ret = 0;

            UNIT_ASSERT_DOUBLES_EQUAL(StrToD("1.1y", &ret), 1.1, EPS);
            UNIT_ASSERT_EQUAL(*ret, 'y');
            UNIT_ASSERT_DOUBLES_EQUAL(StrToD("1.12345678z", &ret), 1.12345678, EPS);
            UNIT_ASSERT_EQUAL(*ret, 'z');
            UNIT_ASSERT_DOUBLES_EQUAL(StrToD("10E-5y", &ret), 10E-5, EPS);
            UNIT_ASSERT_EQUAL(*ret, 'y');
            UNIT_ASSERT_DOUBLES_EQUAL(StrToD("1.1E+5z", &ret), 1.1E+5, EPS);
            UNIT_ASSERT_EQUAL(*ret, 'z');
        }

        inline void TestFloats() {
            UNIT_ASSERT_EQUAL(ToString<float>((float)1.0 / 10000), "0.0001");
        }

        inline void TestReadFloats() {
            UNIT_ASSERT_DOUBLES_EQUAL(FromString<float>("0.0001"), 0.0001, EPS);
            UNIT_ASSERT_DOUBLES_EQUAL(FromString<double>("0.0001"), 0.0001, EPS);
            UNIT_ASSERT_DOUBLES_EQUAL(FromString<long double>("0.0001"), 0.0001, EPS);
            UNIT_ASSERT_DOUBLES_EQUAL(FromString<float>("10E-5"), 10E-5, EPS);
            UNIT_ASSERT_DOUBLES_EQUAL(FromString<double>("1.0001E5"), 1.0001E5, EPS);
            UNIT_ASSERT_DOUBLES_EQUAL(FromString<long double>("1.0001e5"), 1.0001e5, EPS);
            UNIT_ASSERT_DOUBLES_EQUAL(FromString<long double>(".0001e5"), .0001e5, EPS);
            UNIT_TEST_EXCEPTION(TestFloatExc<float>("a10E-5"), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<float>("10 "), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<float>("10\t"), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<float>("10E"), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<float>("10.E"), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<float>("..0"), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<double>("1.00.01"), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<double>("1.0001E5b"), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<double>("1.0001s"), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<double>("1..01"), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<long double>(".1.00"), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<long double>("1.00."), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<long double>("1.0001e5-"), yexception);
            UNIT_TEST_EXCEPTION(TestFloatExc<long double>("10e 2"), yexception);
        }

        inline void TestLiteral() {
            UNIT_ASSERT_EQUAL(ToString("abc"), Stroka("abc"));
        }

        inline void TestAutoDetectType() {
            UNIT_ASSERT_DOUBLES_EQUAL((float)FromString("0.0001"), 0.0001, EPS);
            UNIT_ASSERT_DOUBLES_EQUAL((double)FromString("0.0015", sizeof("0.0015") - 2), 0.001, EPS);
            UNIT_ASSERT_DOUBLES_EQUAL((long double)FromString(TStringBuf("0.0001")), 0.0001, EPS);
            UNIT_ASSERT_DOUBLES_EQUAL((float)FromString(Stroka("10E-5")), 10E-5, EPS);
            UNIT_ASSERT_EQUAL((bool)FromString("da"), true);
            UNIT_ASSERT_EQUAL((bool)FromString("no"), false);
            UNIT_ASSERT_EQUAL((short)FromString(UTF8ToWide("9000")), 9000);
            UNIT_ASSERT_EQUAL((int)FromString(~UTF8ToWide("-+100500")), -100500);
            UNIT_ASSERT_EQUAL((unsigned long long)FromString(TWtringBuf(~UTF8ToWide("42"), 1)), 4);
            int integer = FromString("125");
            wchar16 wideCharacter = FromString(UTF8ToWide("125"));
            UNIT_ASSERT_EQUAL(integer, wideCharacter);
        }
};

UNIT_TEST_SUITE_REGISTRATION(TCastTest);
