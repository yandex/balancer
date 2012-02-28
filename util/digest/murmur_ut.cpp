#include "murmur.h"

#include <library/unittest/registar.h>

#include <util/stream/ios.h>

class TMurmurHashTest: public TTestBase {
        UNIT_TEST_SUITE(TMurmurHashTest);
            UNIT_TEST(TestHash32)
            UNIT_TEST(TestHash64)
        UNIT_TEST_SUITE_END();
    private:
        inline void TestHash32() {
            ui8 buf[256];

            for (size_t i = 0; i < 256; ++i) {
                buf[i] = i;
            }

            Test<ui32>(buf, 256, 2373126550UL);
            Test<ui32>(buf, 255, 3301607533UL);
            Test<ui32>(buf, 254, 2547410121UL);
            Test<ui32>(buf, 253, 80030810UL);
        }

        inline void TestHash64() {
            ui8 buf[256];

            for (size_t i = 0; i < 256; ++i) {
                buf[i] = i;
            }

            Test<ui64>(buf, 256, ULL(12604435678857905857));
            Test<ui64>(buf, 255, ULL(1708835094528446095));
            Test<ui64>(buf, 254, ULL(5077937678736514994));
            Test<ui64>(buf, 253, ULL(11553864555081396353));
        }

    private:
        template <class T>
        inline void Test(const void* data, size_t len, T expected) {
            const T res = MurmurHash<T>(data, len);

            try {
                UNIT_ASSERT_EQUAL(res, expected);
            } catch (...) {
                Cerr << res << Endl;

                throw;
            }
        }
};

UNIT_TEST_SUITE_REGISTRATION(TMurmurHashTest);
