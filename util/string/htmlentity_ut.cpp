#include <library/unittest/registar.h>

#include <util/string/htmlentity.h>
#include <util/charset/wide.h>

class THtmlEntityTest : public TTestBase {
private:
    UNIT_TEST_SUITE(THtmlEntityTest);
    UNIT_TEST(Test);
    UNIT_TEST(TestMarkupSafety);
    UNIT_TEST(TestUnknownPlane);
    UNIT_TEST_SUITE_END();

public:
    void Test();
    void TestMarkupSafety();
    void TestUnknownPlane();
};

UNIT_TEST_SUITE_REGISTRATION(THtmlEntityTest);

Stroka HtEntDecodeToUtf(ECharset fromCharset, const char* str, size_t strLen) {
    TCharTemp wide(strLen * 2); // if we have 1-byte charset that's all mapped to pairs
    size_t outLen = HtEntDecodeToChar(fromCharset, str, strLen, wide.Data());
    return WideToUTF8(wide.Data(), outLen);
}

void THtmlEntityTest::Test() {
    const char* BROKEN_RUNE_UTF = "ÔøΩ"; // ef bf bd
    struct {
        const char* Html;
        const char* Str;
    }
    tests[] = {
        // positive
        { "&lt;&apos;&amp;&quot;&gt;", "<'&\">" },
        { "&lt;hello&gt;", "<hello>" },
        { "&#65;", "A" },
        { "&#65&#66", "AB" },
        { "&#x41;", "A" },
        { "&#x41&#x42", "AB" },
        { "&#X41;", "A" },
        { "&#4f;", "\x04" "f;" },
        // negative
        { "&#xZ;", "&#xZ;" },
        { "&#Z;", "&#Z;" },
        { "&fuflo;", "&fuflo;" },
        { "&verylongentity", "&verylongentity" },
        { "&123;", "&123;" },
        // number termination
        { "&#65", "A" },
        { "&#65a", "Aa" },
        { "&#x65a", "Ÿö" },
        { "&-1;", "&-1;" },
        // long numbers
        { "&#xfffffffffffffffff;", BROKEN_RUNE_UTF },
        { "&#xffffffff;", BROKEN_RUNE_UTF },
        { "&#000000000000000000;", BROKEN_RUNE_UTF },
        { "&#000065;", "A" },
        { "&#000000000000000065;", "A" },
        { "&#000000000000000000000000000000000000000000000000000000000000000000000000000065q", "Aq" },
        // hex
        { "&#x;",       "&#x;" },
        { "&#x9;",      "\x9" },
        { "&#x99",      "‚Ñ¢" },
        { "&#x999",     "‡¶ô" },
        { "&#x9999",    "È¶ô" },
        { "&#x99999",   "Úô¶ô" },
        { "&#x999999",  BROKEN_RUNE_UTF },
        { "&#x9999999",  BROKEN_RUNE_UTF },
        { "&#x99999999",  BROKEN_RUNE_UTF },
        { "&#x999999999",  BROKEN_RUNE_UTF },
        { "&#x9999999999",  BROKEN_RUNE_UTF },
        { "&#x99999999999",  BROKEN_RUNE_UTF },
        // dec
        { "&#;",        "&#;" },
        { "&#9",        "\x9" },
        { "&#99",        "c" },
        { "&#999",       "œß" },
        { "&#9999",      "‚úè" },
        { "&#99999",     "òöü" },
        { "&#999999",    "Û¥àø" },
        { "&#9999999",   BROKEN_RUNE_UTF },
        { "&#99999999",   BROKEN_RUNE_UTF },
        { "&#999999999",   BROKEN_RUNE_UTF },
        { "&#9999999999",   BROKEN_RUNE_UTF },
        { "&#99999999999",   BROKEN_RUNE_UTF },
        { "&#999999999999",   BROKEN_RUNE_UTF },
        { "&#9999999999999",   BROKEN_RUNE_UTF },
        { "&#99999999999999",   BROKEN_RUNE_UTF },
    };

    TStringStream diff;
    for (int i = 0; i < (int)ARRAY_SIZE(tests); ++i) {
        const char* html = tests[i].Html;
        const char* ok = tests[i].Str;

        Stroka result = HtEntDecodeToUtf(CODES_UTF8, html, strlen(html));

        if (result != ok)
            diff << html << " -> " << result << " (expected: " << ok << ")\n";
    }
    if (!diff.empty())
        UNIT_FAIL("\n" + diff.Str());

    Stroka r2 = HtEntDecodeToUtf(CODES_EUC_JP, "≠µ°•ÀﬁŒ„", 8);
    UNIT_ASSERT(+r2 <= 8 * 6);
}

void THtmlEntityTest::TestMarkupSafety() {
    // To safely parse html we need to be sure what this symbols always remain at their positions in all encodings
    Stroka symbols = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789<>\"' \t\n\r -=+_?&;!";
    Wtroka wideSymbols = CharToWide(symbols);

    for (int enc = 0; enc < CODES_MAX; ++enc) {
        ECharset code = static_cast<ECharset>(enc);
        if (code == CODES_UNKNOWNPLANE || code == CODES_TDS565 || code == CODES_UTF_16BE || code == CODES_UTF_16LE)
            // UNKNOWNPLANE is special single byte encoding what maps all symbols to unknown plane
            // TDS565 is some stupid turkmenian encoding %/
            continue;

        UNIT_ASSERT_VALUES_EQUAL(CharToWide(symbols, code), wideSymbols);
    }
}

void THtmlEntityTest::TestUnknownPlane() {
    // CODES_UNKNOWNPLANE must map all symbols except zero to unknown plane
    const CodePage& page = *CodePageByCharset(CODES_UNKNOWNPLANE);

    UNIT_ASSERT_VALUES_EQUAL(page.unicode[0], 0u);

    for (ui64 i = 1; i < 256; ++i) {
        UNIT_ASSERT_VALUES_EQUAL(0xf000 + i, page.unicode[i]);
    }
}
