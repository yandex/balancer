#include "pcdata.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(TPcdata) {
    SIMPLE_UNIT_TEST(TestStress) {
        {
            ui64 key = 0x000017C0B76C4E87ull;
            Stroka res = EncodeHtmlPcdata(TStringBuf((const char*)&key, sizeof(key)));
        }

        for (size_t i = 0; i < 1000; ++i) {
            const Stroka s = NUnitTest::RandomString(i, i);

            UNIT_ASSERT_VALUES_EQUAL(DecodeHtmlPcdata(EncodeHtmlPcdata(s)), s);
        }
    }

    SIMPLE_UNIT_TEST(Test1) {
        const Stroka tests[] = {
            "qw&qw",
            "&<",
            ">&qw",
            "\'&aaa"
        };

        for (size_t i = 0; i < ARRAY_SIZE(tests); ++i) {
            const Stroka s = tests[i];

            UNIT_ASSERT_VALUES_EQUAL(DecodeHtmlPcdata(EncodeHtmlPcdata(s)), s);
        }
    }

    SIMPLE_UNIT_TEST(Test2) {
        UNIT_ASSERT_VALUES_EQUAL(EncodeHtmlPcdata("&qqq"), "&amp;qqq");
    }
}
