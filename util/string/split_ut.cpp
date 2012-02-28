#include "split.h"
#include "vector.h"

#include <library/unittest/registar.h>

#include <util/stream/ios.h>
#include <util/charset/wide.h>
#include <util/datetime/cputimer.h>

template <typename T>
static inline void OldSplit(char *pszBuf, T* pRes) {
    pRes->resize(0);
    pRes->push_back(pszBuf);
    for (char* pszData = pszBuf; *pszData; ++pszData) {
        if (*pszData == '\t') {
            *pszData = 0;
            pRes->push_back(pszData + 1);
        }
    }
}

template <class T1, class T2>
inline void Cmp(const T1& t1, const T2& t2) {
    try {
        UNIT_ASSERT_EQUAL(+t1, +t2);
    } catch (...) {
        Print(t1);
        Cerr << "---------------" << Endl;
        Print(t2);

        throw;
    }

    for (size_t i = 0; i < +t1; ++i) {
        try {
            UNIT_ASSERT_EQUAL(t1[i], t2[i]);
        } catch (...) {
            Cerr << "(" << t1[i] << ")->(" << t2[i] << ")" << Endl;

            throw;
        }
    }
}

template <class T>
inline void Print(const T& t) {
    for (size_t i = 0; i < +t; ++i) {
        Cerr << t[i] << Endl;
    }
}

template <typename TResult, typename I, typename TDelimiter>
void TestDelimiterOnString(TResult& good, I* str, const TDelimiter& delim) {
    TResult test;
    TContainerConsumer<TResult> consumer(&test);
    SplitString(str, delim, consumer);
    Cmp(good, test);
    UNIT_ASSERT_EQUAL(good, test);
}

template <typename TResult, typename I, typename TDelimiter>
void TestDelimiterOnRange(TResult& good, I* b, I* e, const TDelimiter& delim) {
    TResult test;
    TContainerConsumer<TResult> consumer(&test);
    SplitString(b, e, delim, consumer);
    Cmp(good, test);
    UNIT_ASSERT_EQUAL(good, test);
}

template <typename TConsumer, typename TResult, typename I>
void TestConsumerOnString(TResult& good, I* str, I* d) {
    TResult test;
    TContainerConsumer<TResult> consumer(&test);
    TConsumer tested(&consumer);
    TCharDelimiter<const I> delim(*d);
    SplitString(str, delim, tested);
    Cmp(good, test);
    UNIT_ASSERT_EQUAL(good, test);
}

template <typename TConsumer, typename TResult, typename I>
void TestConsumerOnRange(TResult& good, I* b, I* e, I* d) {
    TResult test;
    TContainerConsumer<TResult> consumer(&test);
    TConsumer tested(&consumer);
    TCharDelimiter<const I> delim(*d);
    SplitString(b, e, delim, tested);
    Cmp(good, test);
    UNIT_ASSERT_EQUAL(good, test);
}

typedef TContainerConsumer<yvector<Stroka> > TStrokaConsumer;

void TestLimitingConsumerOnString(yvector<Stroka>& good, const char* str, const char* d, size_t n, const char* last) {
    yvector<Stroka> test;
    TStrokaConsumer consumer(&test);
    TLimitingConsumer<TStrokaConsumer, const char> limits(n, &consumer);
    TCharDelimiter<const char> delim(*d);
    SplitString(str, delim, limits);
    Cmp(good, test);
    UNIT_ASSERT_EQUAL(good, test);
    UNIT_ASSERT_EQUAL(Stroka(limits.Last), Stroka(last)); // Quite unobvious behaviour. Why the last token is not added to slave consumer?
}

void TestLimitingConsumerOnRange(yvector<Stroka>& good, const char* b, const char* e, const char* d, size_t n, const char* last) {
    yvector<Stroka> test;
    TStrokaConsumer consumer(&test);
    TLimitingConsumer<TStrokaConsumer, const char> limits(n, &consumer);
    TCharDelimiter<const char> delim(*d);
    SplitString(b, e, delim, limits);
    Cmp(good, test);
    UNIT_ASSERT_EQUAL(good, test);
    UNIT_ASSERT_EQUAL(Stroka(limits.Last), Stroka(last));
}

inline void Benchmark() {
    yvector<const char*> v, v1, v2;
    Stroka test("q\tqw\tqwe\tqwer\tdjfgjfgsdghf\t11111111111111111111111111111111\t2222222222222");
    char buf[1000];

    {
        TFuncTimer timer("copy");

        for (size_t i = 0; i < 10000000; ++i) {
            test.copy(buf, 1000);
        }
    }

    {
        TFuncTimer timer("old");

        for (size_t i = 0; i < 10000000; ++i) {
            test.copy(buf, 1000);
            OldSplit(buf, &v);
        }
    }

    v1 = v;

    {
        TFuncTimer timer("new");

        for (size_t i = 0; i < 10000000; ++i) {
            test.copy(buf, 1000);
            Split(buf, &v);
        }
    }

    v2 = v;

    UNIT_ASSERT_EQUAL(v1, v2);
}

SIMPLE_UNIT_TEST_SUITE(SplitStringTest) {
    SIMPLE_UNIT_TEST(TestCharSingleDelimiter) {
        Stroka data("qw ab  qwabcab");
        Stroka canonic[] = {"qw", "ab", "", "qwabcab"};
        yvector<Stroka> good(canonic, canonic + 4);
        TCharDelimiter<const char> delim(' ');

        TestDelimiterOnString(good, ~data, delim);
        TestDelimiterOnRange(good, data.c_str(), data.end(), delim);
    }

    SIMPLE_UNIT_TEST(TestWideSingleDelimiter) {
        Wtroka data(CharToWide("qw ab  qwabcab"));
        Wtroka canonic[] = {CharToWide("qw"), CharToWide("ab"), Wtroka(), CharToWide("qwabcab")};
        yvector<Wtroka> good(canonic, canonic + 4);
        TCharDelimiter<const TChar> delim(' ');

        TestDelimiterOnString(good, ~data, delim);
        TestDelimiterOnRange(good, data.c_str(), data.end(), delim);
    }

    SIMPLE_UNIT_TEST(TestCharSkipEmty) {
        Stroka data("qw ab  qwabcab ");
        Stroka canonic[] = {"qw", "ab", "qwabcab"};
        yvector<Stroka> good(canonic, canonic + 3);

        TestConsumerOnString<TSkipEmptyTokens<TStrokaConsumer> >(good, ~data, " ");
        TestConsumerOnRange<TSkipEmptyTokens<TStrokaConsumer> >(good, data.c_str(), data.end(), " ");
    }

    SIMPLE_UNIT_TEST(TestCharKeepDelimiters) {
        Stroka data("qw ab  qwabcab ");
        Stroka canonic[] = {"qw", " ", "ab", " ", "", " ", "qwabcab", " ", ""};
        yvector<Stroka> good(canonic, canonic + 9);

        TestConsumerOnString<TKeepDelimiters<TStrokaConsumer> >(good, ~data, " ");
        TestConsumerOnRange<TKeepDelimiters<TStrokaConsumer> >(good, data.c_str(), data.end(), " ");
    }

    SIMPLE_UNIT_TEST(TestCharLimit) {
        Stroka data("qw ab  qwabcab ");
        Stroka canonic[] = {"qw", "ab"};
        yvector<Stroka> good(canonic, canonic + 2);

        TestLimitingConsumerOnString(good, ~data, " ", 3, " qwabcab ");
        TestLimitingConsumerOnRange(good, data.c_str(), data.end(), " ", 3, " qwabcab ");
    }

    SIMPLE_UNIT_TEST(TestCharStringDelimiter) {
        Stroka data("qw ab qwababcab");
        Stroka canonic[] = {"qw ", " qw", "", "c", ""};
        yvector<Stroka> good(canonic, canonic + 5);
        TStringDelimiter<const char> delim("ab");

        TestDelimiterOnString(good, ~data, delim);
        TestDelimiterOnRange(good, data.c_str(), data.end(), delim);
    }

    SIMPLE_UNIT_TEST(TestWideStringDelimiter) {
        Wtroka data(CharToWide("qw ab qwababcab"));
        Wtroka canonic[] = {CharToWide("qw "), CharToWide(" qw"), Wtroka(), CharToWide("c"), Wtroka()};
        yvector<Wtroka> good(canonic, canonic + 5);
        Wtroka wideDelim(CharToWide("ab"));
        TStringDelimiter<const TChar> delim(~wideDelim);

        TestDelimiterOnString(good, ~data, delim);
        TestDelimiterOnRange(good, data.c_str(), data.end(), delim);
    }

    SIMPLE_UNIT_TEST(TestCharSetDelimiter) {
        Stroka data("qw ab qwababccab");
        Stroka canonic[] = {"q", " ab q", "abab", "", "ab"};
        yvector<Stroka> good(canonic, canonic + 5);
        TSetDelimiter<const char> delim("wc");

        TestDelimiterOnString(good, ~data, delim);
        TestDelimiterOnRange(good, data.c_str(), data.end(), delim);
    }

    SIMPLE_UNIT_TEST(TestWideSetDelimiter) {
        Wtroka data(CharToWide("qw ab qwababccab"));
        Wtroka canonic[] = {CharToWide("q"), CharToWide(" ab q"), CharToWide("abab"), Wtroka(), CharToWide("ab")};
        yvector<Wtroka> good(canonic, canonic + 5);
        Wtroka wideDelim(CharToWide("wc"));
        TSetDelimiter<const TChar> delim(~wideDelim);

        TestDelimiterOnString(good, ~data, delim);
    }

    SIMPLE_UNIT_TEST(TestWideSetDelimiterRange) {
        Wtroka data(CharToWide("qw ab qwababccab"));
        Wtroka canonic[] = {CharToWide("q"), CharToWide(" ab q"), CharToWide("abab"), Wtroka(), CharToWide("ab")};
        yvector<Wtroka> good(1);
        Wtroka wideDelim(CharToWide("wc"));
        TSetDelimiter<const TChar> delim(~wideDelim);

        yvector<Wtroka> test;
        TContainerConsumer<yvector<Wtroka> > consumer(&test);
        SplitString(data.c_str(), data.c_str(), delim, consumer); // Empty string is still inserted into consumer
        Cmp(good, test);

        good.assign(canonic, canonic + 4);
        good.push_back(Wtroka());
        test.clear();
        SplitString(data.c_str(), data.end() - 2, delim, consumer);
        Cmp(good, test);
    }

    //~ SIMPLE_UNIT_TEST(TestBenchmark) {
        //~ Benchmark();
    //~ }
}
