#include "chunkstring.h"

#include <library/unittest/registar.h>

using namespace NSrvKernel;

class TChunkStringTest: public TTestBase {
public:
    UNIT_TEST_SUITE(TChunkStringTest);
    UNIT_TEST(TestCompare);
    UNIT_TEST(TestStartsWith);
    UNIT_TEST(TestIterator);
    UNIT_TEST(TestCutFront);
    UNIT_TEST(TestCutBack);
    UNIT_TEST(TestFind);
    UNIT_TEST_SUITE_END();

private:
    void TestCompare();
    void TestStartsWith();
    void TestIterator();
    void TestCutFront();
    void TestCutBack();
    void TestFind();

    virtual size_t Next(size_t prev) const throw ();
    TChunkString MakeString(const char* string, size_t chunkLen);
};

UNIT_TEST_SUITE_REGISTRATION(TChunkStringTest);

TChunkString TChunkStringTest::MakeString(const char* string, size_t chunkLen) {
    TChunkList list;
    size_t len = TCharTraits<char>::GetLength(string);

    for (; len > chunkLen; len -= chunkLen, string += chunkLen) {
        list.Push(NewChunk(TStringBuf(string, chunkLen)));
    }
    list.Push(NewChunk(TStringBuf(string, len)));
    return TChunkString(list);
}

size_t TChunkStringTest::Next(size_t prev) const throw () {
    return (size_t)(prev * 1.62);
}

void TChunkStringTest::TestCompare() {
    for (size_t i = 1; i <= 8; ++i) {
        UNIT_ASSERT(MakeString("abcdef", i) == Stroka("abcdef"));
        UNIT_ASSERT(MakeString("abcdex", i) > Stroka("abcdef"));
        UNIT_ASSERT(MakeString("abcdefa", i) > Stroka("abcdef"));
        UNIT_ASSERT(MakeString("abcde", i) < Stroka("abcdef"));
    }

    UNIT_ASSERT(MakeString("abcdef", 1) != Stroka("axcdef"));
    UNIT_ASSERT(MakeString("abcdef", 2) != Stroka("abxdef"));
    UNIT_ASSERT(MakeString("abcdef", 4) != Stroka("abcxef"));
    UNIT_ASSERT(MakeString("abcdef", 4) != Stroka("abcdxf"));

    UNIT_ASSERT(MakeString("axcdef", 1) > Stroka("abcdef"));
    UNIT_ASSERT(MakeString("abxdef", 2) > Stroka("abcdef"));
    UNIT_ASSERT(MakeString("abcxef", 4) > Stroka("abcdef"));
    UNIT_ASSERT(MakeString("abcdxf", 6) > Stroka("abcdef"));
    UNIT_ASSERT(MakeString("abcdex", 8) > Stroka("abcdef"));
}

void TChunkStringTest::TestStartsWith() {
    for (size_t i = 1; i <= 8; ++i) {
        UNIT_ASSERT(MakeString("abcdef", i).StartsWith(Stroka("abcd")));
        UNIT_ASSERT(!MakeString("abcdef", i).StartsWith(Stroka("abxd")));
        UNIT_ASSERT(!MakeString("abcdef", i).StartsWith(Stroka("abcdefg")));
        UNIT_ASSERT(MakeString("abcdef", i).StartsWith(Stroka("abcdef")));
    }
}

void TChunkStringTest::TestIterator() {
    const char STR[] = "abcdef";
    for (size_t i = 1; i < 8; ++i) {
        TChunkString s = MakeString(STR, i);
        TChunkString::TConstIterator it = s.Begin();
        UNIT_ASSERT_EQUAL(s.Length(), sizeof(STR) - 1);
        for (size_t j = 0; j < s.Length(); ++j, ++it) {
            size_t k = s.Length() - j;
            UNIT_ASSERT_EQUAL(*it, STR[j]);
            UNIT_ASSERT_EQUAL(s.Begin() + j, it);
            UNIT_ASSERT_EQUAL(s.End() - k, it);
            UNIT_ASSERT_EQUAL((s.Begin() + j/2) + (j - j/2), it);
            UNIT_ASSERT_EQUAL((s.End() - (k - k/2)) - k/2, it);
        }
    }
}

void TChunkStringTest::TestCutFront() {
    const char STR[] = "abcdef";
    for (size_t i = 1; i < sizeof(STR); ++i) {
        for (size_t j = 0; j < sizeof(STR); ++j) {
            TChunkString s = MakeString(STR, i);
            TChunkString::TIterator iter = s.Begin();
            for (size_t k = 0; k < j; ++k) {
                ++iter;
            }
            s.CutFront(iter);
            Stroka s2(STR + j);
            UNIT_ASSERT_EQUAL(s, s2);
        }
    }
}

void TChunkStringTest::TestCutBack() {
    const char STR[] = "abcdef";
    for (size_t i = 1; i < sizeof(STR); ++i) {
        for (size_t j = 0; j < sizeof(STR); ++j) {
            TChunkString s = MakeString(STR, i);
            TChunkString::TIterator iter = s.Begin();
            for (size_t k = 0; k < j; ++k) {
                ++iter;
            }
            s.CutBack(iter);
            Stroka s2(STR, j);
            UNIT_ASSERT_EQUAL(s, s2);
        }
    }
}

void TChunkStringTest::TestFind() {
    const char STR[] = "abcdefaccd";
    for (size_t i = 1; i < sizeof(STR); ++i) {
        TChunkString s = MakeString(STR, i);
        for (size_t j = 0; j < sizeof(STR) - 1; ++j) {
            TChunkString::TConstIterator iter = s.Find(STR[j]);
            UNIT_ASSERT(iter != s.End());
            UNIT_ASSERT(*iter == STR[j]);
        }
        UNIT_ASSERT(s.Find('a', s.Find('a') + 1) != s.End());
        UNIT_ASSERT(s.Find('a', s.Find('a', s.Find('a') + 1) + 1) == s.End());
        UNIT_ASSERT(s.Find('d', s.Find('d') + 1) != s.End());
        UNIT_ASSERT(s.Find('d', s.Find('d', s.Find('d') + 1) + 1) == s.End());
    }
}
