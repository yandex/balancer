#include <library/unittest/registar.h>

#include <util/system/datetime.h>
#include <util/generic/stroka.h>
#include <util/generic/vector.h>
#include <util/generic/buffer.h>

class TBufferTest: public TTestBase {
        UNIT_TEST_SUITE(TBufferTest);
            UNIT_TEST(TestAppend);
            UNIT_TEST(TestReset);
            //UNIT_TEST(TestSpeed);
            UNIT_TEST(TestResize);
            UNIT_TEST(TestEraseBack);
        UNIT_TEST_SUITE_END();
    private:
        void TestAppend();
        void TestReset();
        void TestSpeed();
        void TestResize();
        void TestAlignUp();

        inline void TestEraseBack() {
            TBuffer buf;

            buf.Append("1234567", 7);
            buf.Reserve(1000);
            buf.Resize(6);
            buf.EraseBack(2);

            UNIT_ASSERT_EQUAL(Stroka(~buf, +buf), "1234");
        }
};

UNIT_TEST_SUITE_REGISTRATION(TBufferTest);

void TBufferTest::TestAppend() {
    const char data[] = "1234567890qwertyuiop";

    TBuffer buf(13);
    Stroka str;

    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < sizeof(data) - 1; ++j) {
            buf.Append(data, j);
            buf.Append('q');
            str.append(data, j);
            str.append('q');
        }
    }

    UNIT_ASSERT_EQUAL(Stroka(~buf, +buf), str);
}

void TBufferTest::TestReset() {
    char content[] = "some text";
    TBuffer buf;

    buf.Append(content, sizeof(content));
    buf.Clear();

    UNIT_ASSERT(buf.Capacity() != 0);

    buf.Append(content, sizeof(content));
    buf.Reset();

    UNIT_ASSERT_EQUAL(buf.Capacity(), 0);
}

void TBufferTest::TestResize() {
    char content[] = "some text";
    TBuffer buf;

    buf.Resize(10);
    UNIT_ASSERT_VALUES_EQUAL(+buf, 10u);

    buf.Resize(0);
    UNIT_ASSERT_VALUES_EQUAL(+buf, 0u);

    buf.Resize(9);
    memcpy(~buf, content, 9);
    UNIT_ASSERT_VALUES_EQUAL(Stroka(~buf, +buf), "some text");

    buf.Resize(4);
    UNIT_ASSERT_VALUES_EQUAL(Stroka(~buf, +buf), "some");
}

void TBufferTest::TestAlignUp() {
    char content[] = "some text";
    TBuffer buf;

    buf.Append(content, sizeof(content));
    buf.AlignUp(4, '!');

    UNIT_ASSERT(buf.Size() % 4 == 0);
    UNIT_ASSERT_VALUES_EQUAL(Stroka(~buf, +buf), "some text!!!");

    char addContent[] = "1234";
    buf.Append(addContent, sizeof(addContent));
    buf.AlignUp(4, 'X');
    UNIT_ASSERT(buf.Size() % 4 == 0);
    UNIT_ASSERT_VALUES_EQUAL(Stroka(~buf, +buf), "some text!!!1234");
}

void TBufferTest::TestSpeed() {
    const char data[] = "1234567890qwertyuiop";
    const size_t c = 100000;
    ui64 t1 = 0;
    ui64 t2 = 0;

    {
        TBuffer buf;

        t1 = MicroSeconds();

        for (size_t i = 0; i < c; ++i) {
            buf.Append(data, sizeof(data));
        }

        t1 = MicroSeconds() - t1;
    }

    {
        yvector<char> buf;

        t2 = MicroSeconds();

        for (size_t i = 0; i < c; ++i) {
            buf.insert(buf.end(), data, data + sizeof(data));
        }

        t2 = MicroSeconds() - t2;
    }

    //fprintf(stderr, "%u %u\n", (unsigned)t1, (unsigned)t2);

    UNIT_ASSERT(t1 < t2);
}
