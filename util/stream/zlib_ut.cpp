#include "zlib.h"

#include <library/unittest/registar.h>

#include <util/stream/file.h>
#include <util/system/tempfile.h>

#define ZDATA "./zdata"

static const Stroka data = "8s7d5vc6s5vc67sa4c65ascx6asd4xcv76adsfxv76s";

SIMPLE_UNIT_TEST_SUITE(TZLibTest) {
    SIMPLE_UNIT_TEST(Compress) {
        TFileOutput o(ZDATA);
        TZLibCompress c(&o, ZLib::ZLib);

        c.Write(~data, +data);
        c.Finish();
        o.Finish();
    }

    SIMPLE_UNIT_TEST(Decompress) {
        TTempFile tmpFile(ZDATA);

        {
            TFileInput i(ZDATA);
            TZLibDecompress d(&i);

            UNIT_ASSERT_EQUAL(d.ReadLine(), data);
        }
    }
}
