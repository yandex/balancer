#include <library/unittest/registar.h>

#include <library/pire/regexp.h>

SIMPLE_UNIT_TEST_SUITE(TRegExp) {
    using namespace NRegExp;

    SIMPLE_UNIT_TEST(False) {
        UNIT_ASSERT(!TMatcher(TFsm::False()).Match("").Final());
    }

    SIMPLE_UNIT_TEST(Surround) {
        UNIT_ASSERT(TMatcher(TFsm("qw", TFsm::TOptions().SetSurround(true))).Match("aqwb").Final());
        UNIT_ASSERT(!TMatcher(TFsm("qw", TFsm::TOptions().SetSurround(false))).Match("aqwb").Final());
    }

    SIMPLE_UNIT_TEST(Case) {
        UNIT_ASSERT(TMatcher(TFsm("qw", TFsm::TOptions().SetCaseInsensitive(true))).Match("Qw").Final());
        UNIT_ASSERT(!TMatcher(TFsm("qw", TFsm::TOptions().SetCaseInsensitive(false))).Match("Qw").Final());
    }

    SIMPLE_UNIT_TEST(Glue) {
        TFsm glued =
            TFsm("qw", TFsm::TOptions().SetCaseInsensitive(true)) |
            TFsm("qw", TFsm::TOptions().SetCaseInsensitive(false)) |
            TFsm("abc", TFsm::TOptions().SetCaseInsensitive(false));
        UNIT_ASSERT(TMatcher(glued).Match("Qw").Final());
        UNIT_ASSERT(TMatcher(glued).Match("Qw").Final());
        UNIT_ASSERT(TMatcher(glued).Match("abc").Final());
        UNIT_ASSERT(!TMatcher(glued).Match("Abc").Final());
    }
}
