#include "utility.h"

#include <library/unittest/registar.h>

class TUtilityTest: public TTestBase {
        UNIT_TEST_SUITE(TUtilityTest);
            UNIT_TEST(TestSwapPrimitive)
            UNIT_TEST(TestSwapClass)
        UNIT_TEST_SUITE_END();

        class TTest {
            public:
                inline TTest(int val)
                    : Val(val)
                {
                }

                inline void Swap(TTest& t) {
                    DoSwap(Val, t.Val);
                }

                int Val;

            private:
                TTest(const TTest&);
                TTest& operator= (const TTest&);
        };

    private:
        inline void TestSwapPrimitive() {
            int i = 0;
            int j = 1;

            DoSwap(i, j);

            UNIT_ASSERT_EQUAL(i, 1);
            UNIT_ASSERT_EQUAL(j, 0);
        }

        inline void TestSwapClass() {
            TTest i(0);
            TTest j(1);

            DoSwap(i, j);

            UNIT_ASSERT_EQUAL(i.Val, 1);
            UNIT_ASSERT_EQUAL(j.Val, 0);
        }
};

UNIT_TEST_SUITE_REGISTRATION(TUtilityTest);
