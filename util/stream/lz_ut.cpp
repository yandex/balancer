#include "lz.h"

#include <library/unittest/registar.h>

#include <util/stream/file.h>
#include <util/system/tempfile.h>

#define LDATA "./ldata"

static const Stroka data = "aa aaa aa aaa aa aaa bb bbb bb bbb bb bbb";

template <class C>
static inline void TestCompress() {
    TBufferedFileOutput o(LDATA);
    C c(&o, 1024);

    Stroka d = data;

    for (size_t i = 0; i < 10; ++i) {
        c.Write(~d, +d);
        c << Endl;
        d = d + d;
    }

    c.Finish();
    o.Finish();
}

template <class D>
static inline void TestDecompress() {
    TTempFile tmpFile(LDATA);

    {
        TBufferedFileInput i(LDATA);
        D ld(&i);

        Stroka d = data;

        for (size_t i = 0; i < 10; ++i) {
            UNIT_ASSERT_EQUAL(ld.ReadLine(), d);

            d = d + d;
        }
    }
}

SIMPLE_UNIT_TEST_SUITE(TLzTest) {
    SIMPLE_UNIT_TEST(TestLzo) {
        TestCompress<TLzoCompress>();
        TestDecompress<TLzoDecompress>();
    }

    SIMPLE_UNIT_TEST(TestLzf) {
        TestCompress<TLzfCompress>();
        TestDecompress<TLzfDecompress>();
    }

    SIMPLE_UNIT_TEST(TestLzq) {
        TestCompress<TLzqCompress>();
        TestDecompress<TLzqDecompress>();
    }

    SIMPLE_UNIT_TEST(TestLz4) {
        TestCompress<TLz4Compress>();
        TestDecompress<TLz4Decompress>();
    }

    SIMPLE_UNIT_TEST(TestSnappy) {
        TestCompress<TSnappyCompress>();
        TestDecompress<TSnappyDecompress>();
    }
}
