#include "mersenne.h"

#include <library/unittest/registar.h>

#include <util/stream/ios.h>

#define UI32(x) x ## ul

class TMersenneRndTest: public TTestBase {
        UNIT_TEST_SUITE(TMersenneRndTest);
            UNIT_TEST(Test32)
            UNIT_TEST(Test64)
        UNIT_TEST_SUITE_END();
    private:
        inline void Test32() {
            const ui32 res[] = {
                UI32(2325592414),
                UI32(482149846),
                UI32(4177211283),
                UI32(3872387439),
                UI32(1663027210),
                UI32(2005191859),
                UI32(666881213),
                UI32(3289399202),
                UI32(2514534568),
                UI32(3882134983)
            };

            Test<ui32>(res, ARRAY_SIZE(res));
        }

        inline void Test64() {
            const ui64 res[] = {
                ULL(13735441942630277712),
                ULL(10468394322237346228),
                ULL(5051557175812687784),
                ULL(8252857936377966838),
                ULL(4330799099585512958),
                ULL(327094098846779408),
                ULL(6143667654738189122),
                ULL(6387112078486713335),
                ULL(3862502196831460488),
                ULL(11231499428520958715)
            };

            Test<ui64>(res, ARRAY_SIZE(res));
        }

        template <class T>
        inline void Test(const T* res, size_t len) {
            TMersenne<T> m;

            for (size_t i = 0; i < len; ++i) {
                UNIT_ASSERT_EQUAL(m.GenRand(), res[i]);
            }
        }
};

UNIT_TEST_SUITE_REGISTRATION(TMersenneRndTest);
