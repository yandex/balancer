#include <library/unittest/registar.h>

#include <util/string/util.h>
#include <util/regexp/regexp.h>
#include <util/stream/output.h>

struct TRegTest {
    const char* Regexp;
    const char* Data;
    const char* Result;
    int CompileOptions;
    int RunOptions;

    TRegTest(const char* re, const char* text, const char* res, int copts = REG_EXTENDED, int ropts = 0) :
        Regexp(re),
        Data(text),
        Result(res),
        CompileOptions(copts),
        RunOptions(ropts) {}

};

const TRegTest REGTEST_DATA[] = {
    TRegTest("test", "its a test and test string.", "6 10", REG_EXTENDED, 0),
    TRegTest("test", "its a test and test string.", "6 10 15 19", REG_EXTENDED, REGEXP_GLOBAL),
    TRegTest("test|[an]{0,0}", "test and test an test string tes", "0 4 4 4 5 5 6 6 7 7 8 8 9 13 13 13 14 14 15 15 16 16 17 21 21 21 22 22 23 23 24 24 25 25 26 26 27 27 28 28 29 29 30 30 31 31 32 32", REG_EXTENDED, REGEXP_GLOBAL),
    TRegTest("test[an]{1,}", "test and test an test string tes", "NM", REG_EXTENDED, REGEXP_GLOBAL)
};

class TRegexpTest: public TTestBase {
private:
    regmatch_t Matches[NMATCHES];

private:
    UNIT_TEST_SUITE(TRegexpTest);
        UNIT_TEST(TestRe)
    UNIT_TEST_SUITE_END();

    inline void TestRe() {
        for (size_t rt = 0; rt < ARRAY_SIZE(REGTEST_DATA); ++rt) {
            memset(Matches, 0, sizeof(Matches));
            const TRegTest& reg_test = REGTEST_DATA[rt];
            Stroka result;

            TRegExBase re(reg_test.Regexp, reg_test.CompileOptions);
            if (re.Exec(reg_test.Data, Matches, reg_test.RunOptions) == 0) {
                for (int i = 0; i < NMATCHES; i++) {
                    if (Matches[i].rm_so == -1) {
                        break;
                    }
                    result.append(Sprintf("%i %i ", Matches[i].rm_so, Matches[i].rm_eo));
                }
            } else {
                result = "NM";
            }
            result = strip(result);
            UNIT_ASSERT_STRINGS_EQUAL(~result, reg_test.Result);
        }
    }
};

UNIT_TEST_SUITE_REGISTRATION(TRegexpTest);
