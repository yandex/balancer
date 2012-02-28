#include "doccodes.h"

#include <library/unittest/registar.h>

#include <util/system/yassert.h>

class TDocCodesTest : public TTestBase {
private:
    UNIT_TEST_SUITE(TDocCodesTest);
        UNIT_TEST(TestNames);
    UNIT_TEST_SUITE_END();

public:
    void TestNames();
private:
    void TestName(docLanguage language, const char* name) {
        docLanguage reversed = LanguageByName(name);
        UNIT_ASSERT(language == reversed);
        docLanguage reversedStrict = LanguageByNameStrict(name);
        UNIT_ASSERT(language == reversedStrict);
    }

    void TestWrongName(const char* name) {
        docLanguage reversed = LanguageByName(name);
        UNIT_ASSERT(reversed == LANG_UNK);
        docLanguage reversedStrict = LanguageByNameStrict(name);
        UNIT_ASSERT(reversedStrict == LANG_MAX);
    }

    void TestBiblioName(docLanguage language) {
        const char* name = NameByLanguage(language);

        UNIT_ASSERT(name != NULL);
        UNIT_ASSERT(strlen(name) > 2);

        TestName(language, name);
    }

    void TestIsoName(docLanguage language) {
        const char* name = IsoNameByLanguage(language);

        UNIT_ASSERT(name != NULL);
        UNIT_ASSERT(strlen(name) == 0 || strlen(name) == 2
            || !strcmp(name, "mis") || !strncmp(name, "bas-", 4));

        if (strlen(name))
            TestName(language, name);
    }

    void TestFullName(docLanguage language) {
        const char* name = FullNameByLanguage(language);
        UNIT_ASSERT(name != NULL);
        UNIT_ASSERT(strlen(name) == 0 || strlen(name) > 3);

        if (strlen(name))
            TestName(language, name);
    }
};

UNIT_TEST_SUITE_REGISTRATION(TDocCodesTest);

void TDocCodesTest::TestNames() {
    TestWrongName(NULL);
    TestWrongName("");
    TestWrongName("A wrong language name");

    for (size_t i = 0; i != LANG_MAX; ++i) {
        docLanguage language = static_cast<docLanguage>(i);
        TestBiblioName(language);
        TestIsoName(language);
        TestFullName(language);
    }
}
