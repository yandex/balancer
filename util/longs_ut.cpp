#include "longs.h"

#include <library/unittest/registar.h>

#include <util/digest/crc.h>
#include <util/string/util.h>
#include <util/stream/ios.h>

class TLongsTest: public TTestBase {
        UNIT_TEST_SUITE(TLongsTest);
            UNIT_TEST(TestLongs)
        UNIT_TEST_SUITE_END();
    private:
        void TestLongs() {
            i16 x16 = 40;
            i64 x64 = 40;
            i64 y64;
            Stroka s;

            s += Sprintf("x16=0x%x\n", (int)x16);
            s += Sprintf("LO_8(x16)=0x%x HI_8(x16)=0x%x\n\n", (int)LO_8(x16), (int)HI_8(x16));

            char buf[100];
            memset(buf, 0, 100);
            char *p = buf;
            int l = out_long(x64, buf);
            s += Sprintf("x64=0x%" PRIi64 "\n", x64);
#ifdef GCC_IS_NOT_BUGGY
            s += Sprintf("LO_32(x64)=0x%" PRIu32 " HI_32(x64)=0x%" PRIu32 "\n", (ui32)LO_32(x64), (ui32)HI_32(x64));
#else
            {
                ui32a loa = LO_32(x64);
                ui32a hia = HI_32(x64);
                ui32 lo = loa;
                ui32 hi = hia;
                s += Sprintf("LO_32(x64)=0x%" PRIu32 " HI_32(x64)=0x%" PRIu32 "\n", lo, hi);
            }
#endif
            s += Sprintf("buf=%s, l=%d: ", buf, l);
            for (int i = 0; i < l; i++) {
                s += Sprintf("0x%02x ", buf[i]);
            }
            s += Sprintf("\n");

            p = buf;
            in_long(y64, p);
            s += Sprintf("x=0x%" PRIi64 " y=0x%" PRIi64 "\n", x64, y64);
            if (x64 != y64) {
                s += Sprintf("Error: y64 != x64\n");
            } else {
                s += Sprintf("OK\n");
            }

            UNIT_ASSERT_EQUAL(Crc<ui64>(~s, +s), 7251624297500315779ULL);
        }
};

UNIT_TEST_SUITE_REGISTRATION(TLongsTest);
