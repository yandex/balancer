#include "tempbuf.h"

#include <library/unittest/registar.h>

class TTempBufTest: public TTestBase {
        UNIT_TEST_SUITE(TTempBufTest);
            UNIT_TEST(TestCreate);
            UNIT_TEST(TestOps);
            UNIT_TEST(TestAppend);
        UNIT_TEST_SUITE_END();
    public:
        void TestCreate();
        void TestOps();

        void TestAppend() {
            TTempBuf tmp;

            tmp.Append("a", 1);
            tmp.Append("bc", 2);
            tmp.Append("def", 3);

            UNIT_ASSERT_EQUAL(tmp.Filled(), 6);
            UNIT_ASSERT_EQUAL(Stroka(tmp.Data(), tmp.Filled()), "abcdef");
        }
};

UNIT_TEST_SUITE_REGISTRATION(TTempBufTest);

void TTempBufTest::TestCreate() {
    const size_t num = 1000000;
    size_t tmp = 0;
    const size_t len = 4096;

    for (size_t i = 0; i < num; ++i) {
        TTempBuf buf(len);

        tmp += (size_t)buf.Data();
    }

    UNIT_ASSERT(tmp != 0);
}

void TTempBufTest::TestOps() {
    TTempBuf tmp(201);

    tmp.Proceed(100);

    UNIT_ASSERT_EQUAL(tmp.Current() - tmp.Data(), 100);
    UNIT_ASSERT(tmp.Left() >= 101);
    UNIT_ASSERT(tmp.Size() >= 201);
    UNIT_ASSERT_EQUAL(tmp.Filled(), 100);

    tmp.Reset();

    UNIT_ASSERT_EQUAL(tmp.Current(), tmp.Data());
    UNIT_ASSERT(tmp.Left() >= 201);
    UNIT_ASSERT(tmp.Size() >= 201);
    UNIT_ASSERT_EQUAL(tmp.Filled(), 0);
}
