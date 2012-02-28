#include "static_hash.h"
#include "static_hash_map.h"

#include <library/unittest/registar.h>

#include <util/system/defaults.h>
#include <util/folder/dirut.h>

SIMPLE_UNIT_TEST_SUITE(TStaticHashTest) {
    SIMPLE_UNIT_TEST(TestMap) {
        yhash<ui32, ui64> map;
        map[0] = 0;
        map[2] = 20;
        map[8] = 80;

        Stroka filename = Stroka(GetSystemTempDir()) + "/TStaticHashTest-TestMap";
        SaveHashToFile(map, ~filename);

        sthash_mapped<ui32, ui64, hash<ui32> > mapped(~filename, true);

        UNIT_ASSERT_VALUES_EQUAL(3u, mapped->size());
        UNIT_ASSERT_VALUES_EQUAL(0u, mapped->find(0).Value());
        UNIT_ASSERT_VALUES_EQUAL(20u, mapped->find(2).Value());
        UNIT_ASSERT_VALUES_EQUAL(80u, mapped->find(8).Value());

        unlink(~filename);
    }
}
