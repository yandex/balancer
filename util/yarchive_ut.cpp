#include "yarchive.h"

#include <library/unittest/registar.h>

#include <util/string/cast.h>
#include <util/stream/file.h>
#include <util/system/tempfile.h>

class TArchiveTest: public TTestBase {
        UNIT_TEST_SUITE(TArchiveTest)
            UNIT_TEST(TestCreate);
            UNIT_TEST(TestRead);
        UNIT_TEST_SUITE_END();
    private:
        void TestCreate();
        void TestRead();
};

UNIT_TEST_SUITE_REGISTRATION(TArchiveTest);

#define ARCHIVE "./test.ar"

void TArchiveTest::TestCreate() {
    TBufferedFileOutput out(ARCHIVE);
    TArchiveWriter w(&out);

    for (size_t i = 0; i < 1000; ++i) {
        const Stroka path = "/" + ToString(i);
        const Stroka data = "data" + ToString(i * 1000) + "dataend";
        TStringInput si(data);

        w.Add(path, &si);
    }

    w.Finish();
    out.Finish();
}

void TArchiveTest::TestRead() {
    TTempFile tmpFile(ARCHIVE);
    TBlob blob = TBlob::FromFileSingleThreaded(ARCHIVE);
    TArchiveReader r(blob);

    UNIT_ASSERT_EQUAL(r.Count(), 1000);

    for (size_t i = 0; i < 1000; ++i) {
        const Stroka key = "/" + ToString(i);
        TAutoPtr<TInputStream> is = r.ObjectByKey(key);
        const Stroka data = is->ReadAll();

        UNIT_ASSERT_EQUAL(data, "data" + ToString(i * 1000) + "dataend");
    }
}
