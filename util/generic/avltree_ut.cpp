#include <library/unittest/registar.h>

#include <util/generic/avltree.h>

class TAvlTreeTest: public TTestBase {
        UNIT_TEST_SUITE(TAvlTreeTest);
            UNIT_TEST(TestLowerBound);
        UNIT_TEST_SUITE_END();
    private:
        void TestLowerBound();

        class TIt;
        struct TItCompare {
            static inline bool Compare(const TIt& l, const TIt& r) throw ();
        };

        class TIt : public TAvlTreeItem<TIt, TItCompare> {
        public:
            TIt(int val = 0)
                : Val(val) {}

            int Val;
        };

        typedef TAvlTree<TIt, TItCompare> TIts;

};

inline bool TAvlTreeTest::TItCompare::Compare(const TIt& l, const TIt& r) throw () {
    return l.Val < r.Val;
}

UNIT_TEST_SUITE_REGISTRATION(TAvlTreeTest);

void TAvlTreeTest::TestLowerBound() {
    TIts its;
    TIt it1(5);
    TIt it2(2);
    TIt it3(10);
    TIt it4(879);
    TIt it5(1);
    TIt it6(52);
    TIt it7(4);
    TIt it8(5);
    its.Insert(&it1);
    its.Insert(&it2);
    its.Insert(&it3);
    its.Insert(&it4);
    its.Insert(&it5);
    its.Insert(&it6);
    its.Insert(&it7);
    its.Insert(&it8);

    TIt it_zero(0);
    TIt it_large(1000);
    UNIT_ASSERT_EQUAL(its.LowerBound(&it3), &it3);
    UNIT_ASSERT_EQUAL(its.LowerBound(&it_zero), &it5);
    UNIT_ASSERT_EQUAL(its.LowerBound(&it_large), 0);
}

