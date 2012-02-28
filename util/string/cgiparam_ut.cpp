#include "cgiparam.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(TCgiParametersTest) {
    SIMPLE_UNIT_TEST(Test1) {
        TCgiParameters C;
        C.Scan("aaa=b%62b&ccc=ddd&ag0=");
        UNIT_ASSERT_EQUAL(C.Get("aaa") == "bbb", true);
        UNIT_ASSERT_EQUAL(C.NumOfValues("ag0") == 1, true);
        UNIT_ASSERT(C.Has("ccc", "ddd"));
        UNIT_ASSERT(C.Has("ag0", ""));
        UNIT_ASSERT(!C.Has("a", "bbb"));
        UNIT_ASSERT(!C.Has("aaa", "bb"));
    }

    SIMPLE_UNIT_TEST(Test2) {
        const Stroka parsee("=000&aaa=bbb&ag0=&ccc=ddd");
        TCgiParameters c;
        c.Scan(parsee);

        UNIT_ASSERT_VALUES_EQUAL(c.Print(), parsee);
    }

    SIMPLE_UNIT_TEST(Test3) {
        const Stroka parsee("aaa=bbb&ag0=&ccc=ddd");
        TCgiParameters c;
        c.Scan(parsee);

        c.InsertEscaped("d", "xxx");

        UNIT_ASSERT_VALUES_EQUAL(c.Print(), parsee + "&d=xxx");
    }

    SIMPLE_UNIT_TEST(Test4) {
        TCgiParameters c;
        c.ScanAddAll("qw");

        UNIT_ASSERT_VALUES_EQUAL(c.size(), 1u);
        UNIT_ASSERT(c.Get("qw").empty());
    }

    SIMPLE_UNIT_TEST(Test5) {
        TCgiParameters c;
        c.ScanAddAll("qw&");

        UNIT_ASSERT_VALUES_EQUAL(c.size(), 1u);
        UNIT_ASSERT(c.Get("qw").empty());
    }

    SIMPLE_UNIT_TEST(Test6) {
        TCgiParameters c;
        c.ScanAddAll("qw=1&x");

        UNIT_ASSERT_VALUES_EQUAL(c.size(), 2u);
        UNIT_ASSERT_VALUES_EQUAL(c.Get("qw"), "1");
        UNIT_ASSERT(c.Get("x").empty());
    }

    SIMPLE_UNIT_TEST(Test7) {
        TCgiParameters c;
        c.ScanAddAll("ccc=1&aaa=1&ccc=3&bbb&ccc=2");

        UNIT_ASSERT_VALUES_EQUAL(c.Print(), "aaa=1&bbb=&ccc=1&ccc=3&ccc=2");
    }

    SIMPLE_UNIT_TEST(Test8) {
        TCgiParameters c;
        c.ScanAddAll("par=1&aaa=1&par=2&bbb&par=3");
        c.EraseAll("par");

        UNIT_ASSERT_VALUES_EQUAL(c.Print(), "aaa=1&bbb=");
    }

    SIMPLE_UNIT_TEST(Test9) {
        TCgiParameters c;
        c.ScanAddAll("par=1&aaa=1&par=2&bbb&par=3");

        UNIT_ASSERT_VALUES_EQUAL(c.NumOfValues("par"), 3u);
    }

    SIMPLE_UNIT_TEST(Test10) {
        TCgiParameters c;

        c.ScanAuto("f=1&t=%D1%EB%EE%ED&p=0");
        UNIT_ASSERT_VALUES_EQUAL(c.Print(), "f=1&p=0&t=%D0%A1%D0%BB%D0%BE%D0%BD");

        c.ScanAuto("t=\xD0\xB3\xD0\xB8");
        UNIT_ASSERT_VALUES_EQUAL(c.Print(), "t=%D0%B3%D0%B8");
    }

    SIMPLE_UNIT_TEST(Test11) {
        TCgiParameters c("f=1&t=%84R%84%7C%84%80%84%7E&reqenc=SHIFT_JIS&p=0");
        UNIT_ASSERT_VALUES_EQUAL(c.Get("t"), "\x84R\x84\x7C\x84\x80\x84\x7E");

        c.ScanAuto("f=1&t=%84R%84%7C%84%80%84%7E&p=0", CODES_SHIFT_JIS);
        UNIT_ASSERT_VALUES_EQUAL(c.Get("t"), "\xD0\xA1\xD0\xBB\xD0\xBE\xD0\xBD");

        c.ScanAuto("f=1&t=%84R%84%7C%84%80%84%7E&reqenc=SHIFT_JIS&p=0");
        UNIT_ASSERT_VALUES_EQUAL(c.Get("t"), "\xD0\xA1\xD0\xBB\xD0\xBE\xD0\xBD");

        c.ScanAuto("f=1&t=%E4R%F4f%DC%B4%80%80d%7Ef&reqenc=SHIFT_JIS&p=0");
        UNIT_ASSERT_VALUES_EQUAL(c.Get("t"), "\xE8\x86\xA4\xEE\x8C\x96\xEF\xBE\x9C\xEF\xBD\264d\xE2\x80\276f");
    }

    SIMPLE_UNIT_TEST(TestEmpty) {
        UNIT_ASSERT(TCgiParameters().Print().empty());
    }

    SIMPLE_UNIT_TEST(Test12) {
        TCgiParameters c;

        c.ScanAuto("foo=1&foo=2");
        c.JoinUnescaped("foo", ";", "0");

        UNIT_ASSERT_VALUES_EQUAL(c.Print(), "foo=1;2;0");
    }

}
