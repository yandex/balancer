#include "strbuf.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(TStrBufTest) {
    SIMPLE_UNIT_TEST(TestConstructors) {
        TStringBuf str("qwerty");

        UNIT_ASSERT_EQUAL(*~str, 'q');
        UNIT_ASSERT_EQUAL(+str, 6);

        TStringBuf str1(STRINGBUF("qwe\0rty"));
        TStringBuf str2(str1.data());
        UNIT_ASSERT_VALUES_UNEQUAL(str1, str2);
    }

    SIMPLE_UNIT_TEST(TestAfter) {
        TStringBuf str("qwerty");

        UNIT_ASSERT_EQUAL(str.After('w'), TStringBuf("erty"));
        UNIT_ASSERT_EQUAL(str.After('x'), TStringBuf("qwerty"));
        UNIT_ASSERT_EQUAL(str.After('y'), TStringBuf());
    }

    SIMPLE_UNIT_TEST(TestBefore) {
        TStringBuf str("qwerty");

        UNIT_ASSERT_EQUAL(str.Before('w'), TStringBuf("q"));
        UNIT_ASSERT_EQUAL(str.Before('x'), TStringBuf("qwerty"));
        UNIT_ASSERT_EQUAL(str.Before('y'), TStringBuf("qwert"));
        UNIT_ASSERT_EQUAL(str.Before('q'), TStringBuf());
    }

    SIMPLE_UNIT_TEST(TestEmpty) {
        UNIT_ASSERT(TStringBuf().Empty());
        UNIT_ASSERT(!TStringBuf("q").Empty());
    }

    SIMPLE_UNIT_TEST(TestShift) {
        TStringBuf qw("qwerty");
        TStringBuf str;

        str = qw;
        str.Chop(10);
        UNIT_ASSERT(str.Empty());

        str = qw;
        UNIT_ASSERT_EQUAL(str.SubStr(1), str + 1);
        UNIT_ASSERT_EQUAL(str + 2, TStringBuf("erty"));
        UNIT_ASSERT_EQUAL(str += 3, qw.SubStr(3));
        str.Chop(1);
        UNIT_ASSERT_EQUAL(str, TStringBuf("rt"));
    }

    SIMPLE_UNIT_TEST(TestSplit) {
        TStringBuf qw("qwerty");
        TStringBuf lt, rt;

        rt = qw;
        lt = rt.NextTok('r');
        UNIT_ASSERT_EQUAL(lt, TStringBuf("qwe"));
        UNIT_ASSERT_EQUAL(rt, TStringBuf("ty"));

        lt = qw;
        rt = lt.SplitOff('r');
        UNIT_ASSERT_EQUAL(lt, TStringBuf("qwe"));
        UNIT_ASSERT_EQUAL(rt, TStringBuf("ty"));
    }

    SIMPLE_UNIT_TEST(TestRFind) {
        TStringBuf buf1 = STRINGBUF("123123456");
        UNIT_ASSERT_EQUAL(buf1.rfind('3'), 5);
        UNIT_ASSERT_EQUAL(buf1.rfind('4'), 6);
        UNIT_ASSERT_EQUAL(buf1.rfind('7'), TStringBuf::npos);

        TStringBuf buf2;
        UNIT_ASSERT_EQUAL(buf2.rfind('3'), TStringBuf::npos);

        TStringBuf buf3 = TStringBuf("123123456", 6);
        UNIT_ASSERT_EQUAL(buf3.rfind('3'), 5);
        UNIT_ASSERT_EQUAL(buf3.rfind('4'), TStringBuf::npos);
        UNIT_ASSERT_EQUAL(buf3.rfind('7'), TStringBuf::npos);

        TStringBuf buf4 = TStringBuf("123123456", 5);
        UNIT_ASSERT_EQUAL(buf4.rfind('3'), 2);
    }

    SIMPLE_UNIT_TEST(TestRNextTok) {
        TStringBuf buf1("a.b.c");
        UNIT_ASSERT_EQUAL(buf1.RNextTok('.'), TStringBuf("c"));
        UNIT_ASSERT_EQUAL(buf1, TStringBuf("a.b"));

        TStringBuf buf2("a");
        UNIT_ASSERT_EQUAL(buf2.RNextTok('.'), TStringBuf("a"));
        UNIT_ASSERT_EQUAL(buf2, TStringBuf());
    }

    SIMPLE_UNIT_TEST(TestRSplitOff) {
        TStringBuf buf1("a.b.c");
        UNIT_ASSERT_EQUAL(buf1.RSplitOff('.'), TStringBuf("a.b"));
        UNIT_ASSERT_EQUAL(buf1, TStringBuf("c"));

        TStringBuf buf2("a");
        UNIT_ASSERT_EQUAL(buf2.RSplitOff('.'), TStringBuf());
        UNIT_ASSERT_EQUAL(buf2, TStringBuf("a"));
    }

}
