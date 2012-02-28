#include <library/unittest/registar.h>

#include <map>

#include <util/generic/hash.h>
#include <util/generic/strkey.h>

SIMPLE_UNIT_TEST_SUITE(TStrKeyTests)
{
    SIMPLE_UNIT_TEST(TestPersistence)
    {
        char buf1[256];
        char buf2[256];

        const TStringBuf key1 = STRINGBUF("key1");
        const TStringBuf key2 = STRINGBUF("key2");

        // +1 to make sure it remains null-terminated
        memcpy(buf1, ~key1, +key1 + 1);
        memcpy(buf2, ~key2, +key2 + 1);

        yhash_map<TStringKey, Stroka> map1;
        NStl::map<TStringKey, Stroka> map2;

        map1[buf1] = "1";
        map2[buf1] = "1";
        map1[buf2] = "2";
        map2[buf2] = "2";

        UNIT_ASSERT_EQUAL(map1[key1], "1");
        UNIT_ASSERT_EQUAL(map2[key1], "1");
        UNIT_ASSERT_EQUAL(map1[key2], "2");
        UNIT_ASSERT_EQUAL(map2[key2], "2");

        memcpy(buf1, ~key2, +key2 + 1); // overwrite buf1
        memset(buf2, 0, 256); // reset buf2

        // make sure the values haven't changed
        UNIT_ASSERT_EQUAL(map1[key1], "1");
        UNIT_ASSERT_EQUAL(map2[key1], "1");
        UNIT_ASSERT_EQUAL(map1[key2], "2");
        UNIT_ASSERT_EQUAL(map2[key2], "2");
    }
}
