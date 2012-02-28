#include "escape.h"

#include <library/unittest/registar.h>

#include <util/generic/stroka.h>
#include <util/charset/wide.h>

typedef TFixedString<char> TFixedStringA;
#define S(s) TFixedStringA(s, sizeof(s) - 1)
static const TFixedStringA CommonTestData[] = {
    // Should be valid UTF-8.
    S("http://ya.ru/"), S("http://ya.ru/"),
    S("http://ya.ru/\\x17\\n"), S("http://ya.ru/\x17\n"),

    S("http://ya.ru/\\0"), S("http://ya.ru/\0"),
    S("http://ya.ru/\\0\\0"), S("http://ya.ru/\0\0"),
    S("http://ya.ru/\\0\\0000"), S("http://ya.ru/\0\0" "0"),
    S("http://ya.ru/\\0\\0001"), S("http://ya.ru/\0\x00" "1"),

    S("\\2\\4\\00678"), S("\2\4\6" "78"),
    S("\\2\\4\\689"), S("\2\4\689"),

    S("\\\"Hello\\\", Alice said."), S("\"Hello\", Alice said."),
    S("Slash\\\\dash!"), S("Slash\\dash!"),
    S("There\\nare\\r\\nnewlines."), S("There\nare\r\nnewlines."),
    S("There\\tare\\ttabs."), S("There\tare\ttabs.")
};
#undef S

SIMPLE_UNIT_TEST_SUITE(TEscapeCTest) {
    SIMPLE_UNIT_TEST(TestStrokaEscapeC) {
        for (size_t i = 0; i < ARRAY_SIZE(CommonTestData); i += 2) {
            Stroka expected(CommonTestData[i].Start, CommonTestData[i].Length);
            Stroka source(CommonTestData[i + 1].Start, CommonTestData[i + 1].Length);
            Stroka actual(EscapeC(source));
            Stroka actual2(UnescapeC(expected));

            UNIT_ASSERT_VALUES_EQUAL(expected, actual);
            UNIT_ASSERT_VALUES_EQUAL(source, actual2);
        }

        UNIT_ASSERT_VALUES_EQUAL("http://ya.ru/\\x17\\n\\xAB", EscapeC(Stroka("http://ya.ru/\x17\n\xab")));
        UNIT_ASSERT_VALUES_EQUAL("http://ya.ru/\x17\n\xab", UnescapeC(Stroka("http://ya.ru/\\x17\\n\\xAB")));
        UNIT_ASSERT_VALUES_EQUAL("h", EscapeC('h'));
        UNIT_ASSERT_VALUES_EQUAL("h", UnescapeC(Stroka("h")));
        UNIT_ASSERT_VALUES_EQUAL("\\xFF", EscapeC('\xFF'));
        UNIT_ASSERT_VALUES_EQUAL("\xFF", UnescapeC(Stroka("\\xFF")));

        UNIT_ASSERT_VALUES_EQUAL("\\377f", EscapeC(Stroka("\xff" "f")));
        UNIT_ASSERT_VALUES_EQUAL("\xff" "f", UnescapeC(Stroka("\\377f")));
        UNIT_ASSERT_VALUES_EQUAL("\\xFFg", EscapeC(Stroka("\xff" "g")));
        UNIT_ASSERT_VALUES_EQUAL("\xff" "g", UnescapeC(Stroka("\\xFFg")));
        UNIT_ASSERT_VALUES_EQUAL("\xEA\x9A\x96", UnescapeC(Stroka("\\uA696")));
        UNIT_ASSERT_VALUES_EQUAL("Странный компроматтест", UnescapeC(Stroka("\\u0421\\u0442\\u0440\\u0430\\u043d\\u043d\\u044b\\u0439 \\u043a\\u043e\\u043c\\u043f\\u0440\\u043e\\u043c\\u0430\\u0442тест")));
    }

    SIMPLE_UNIT_TEST(TestWtrokaEscapeC) {
        for (size_t i = 0; i < ARRAY_SIZE(CommonTestData); i += 2) {
            Wtroka expected(UTF8ToWide(CommonTestData[i].Start, CommonTestData[i].Length));
            Wtroka source(UTF8ToWide(CommonTestData[i + 1].Start, CommonTestData[i + 1].Length));
            Wtroka actual(EscapeC(source));
            Wtroka actual2(UnescapeC(expected));

            UNIT_ASSERT_VALUES_EQUAL(expected, actual);
            UNIT_ASSERT_VALUES_EQUAL(source, actual2);
        }

        UNIT_ASSERT_VALUES_EQUAL(CharToWide("http://ya.ru/\\x17\\n\\u1234"), EscapeC(UTF8ToWide("http://ya.ru/\x17\n") + wchar16(0x1234)));
        UNIT_ASSERT_VALUES_EQUAL(CharToWide("h"), EscapeC(wchar16('h')));
        UNIT_ASSERT_VALUES_EQUAL(CharToWide("\\xFF"), EscapeC(wchar16(255)));
    }
}

