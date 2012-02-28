#include "bzip2.h"

#include <library/unittest/registar.h>

#include <util/stream/file.h>
#include <util/system/tempfile.h>

#define ZDATA "./zdata"

class TBZipTest: public TTestBase {
        UNIT_TEST_SUITE(TBZipTest);
            UNIT_TEST(TestCompress);
            UNIT_TEST(TestDecompress);
            UNIT_TEST_EXCEPTION(TestCorrupted, TBZipDecompressError);
        UNIT_TEST_SUITE_END();
    public:
        void TestCompress();
        void TestDecompress();
        void TestCorrupted();
};

UNIT_TEST_SUITE_REGISTRATION(TBZipTest);

static const Stroka data = "8s7d5vc6s5vc67sa4c65ascx6asd4xcv76adsfxv76s";

void TBZipTest::TestCompress() {
    TFileOutput o(ZDATA);
    TBZipCompress c(&o);

    c.Write(~data, +data);
    c.Finish();
    o.Finish();
}

void TBZipTest::TestDecompress() {
    TTempFile tmp(ZDATA);

    {
        TFileInput i(ZDATA);
        TBZipDecompress d(&i);

        UNIT_ASSERT_EQUAL(d.ReadLine(), data);
    }
}

void TBZipTest::TestCorrupted() {
    TMemoryInput i("blablabla", 10);
    TBZipDecompress d(&i);

    d.ReadLine();
}
