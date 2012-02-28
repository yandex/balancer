#include "chunk.h"

#include <library/unittest/registar.h>

#include <util/stream/file.h>
#include <util/system/tempfile.h>

#define CDATA "./chunkedio"

class TChunkIOTest: public TTestBase {
        UNIT_TEST_SUITE(TChunkIOTest);
            UNIT_TEST(TestChunkedIo);
            UNIT_TEST(TestBinaryChunkedIo);
            UNIT_TEST(TestBadChunk);
        UNIT_TEST_SUITE_END();
    public:
        void TestChunkedIo();
        void TestBinaryChunkedIo();

        inline void TestBadChunk() {
            bool hasError = false;

            try {
                Stroka badChunk = "10\r\nqwerty";
                TMemoryInput mi(~badChunk, +badChunk);
                TChunkedInput ci(&mi);
                TransferData(&ci, &Cnull);
            } catch (...) {
                hasError = true;
            }

            UNIT_ASSERT(hasError);
        }
};

UNIT_TEST_SUITE_REGISTRATION(TChunkIOTest);

static const char test_data[] = "87s6cfbsudg cuisg s igasidftasiy tfrcua6s";

void TChunkIOTest::TestChunkedIo() {
    TTempFile tmpFile(CDATA);
    Stroka tmp;

    {
        TFileOutput fo(CDATA);
        TChunkedOutput co(&fo);

        for (size_t i = 0; i < sizeof(test_data); ++i) {
            co.Write(test_data, i);
            tmp.append(test_data, i);
        }

        co.Finish();
        fo.Finish();
    }

    {
        TFileInput fi(CDATA);
        TChunkedInput ci(&fi);
        Stroka r;

        char buf[11];
        size_t readed = 0;

        do {
            readed = ci.Read(buf, sizeof(buf));
            r.append(buf, readed);
        } while (readed);

        UNIT_ASSERT_EQUAL(r, tmp);
    }
}

void TChunkIOTest::TestBinaryChunkedIo() {
    TTempFile tmpFile(CDATA);
    Stroka tmp;

    {
        TFileOutput fo(CDATA);
        TBinaryChunkedOutput co(&fo);

        for (size_t i = 0; i < sizeof(test_data); ++i) {
            co.Write(test_data, i);
            tmp.append(test_data, i);
        }

        co.Finish();
        fo.Finish();
    }

    {
        TFileInput fi(CDATA);
        TBinaryChunkedInput ci(&fi);
        Stroka r;

        char buf[11];
        size_t readed = 0;

        do {
            readed = ci.Read(buf, sizeof(buf));
            r.append(buf, readed);
        } while (readed);

        UNIT_ASSERT_EQUAL(r, tmp);
    }
}
