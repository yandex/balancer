#include "bitmap.h"

#include <library/unittest/registar.h>

#define INIT_BITMAP(bitmap, bits) \
    for (size_t i = 0; i < sizeof(bits) / sizeof(size_t); ++i) { \
        bitmap.Set(bits[i]); \
    }

#define CHECK_BITMAP(bitmap, bits) \
    { \
        size_t cur = 0, end = sizeof(bits) / sizeof(size_t); \
        for (size_t i = 0; i < bitmap.Size(); ++i) { \
            if (cur < end && bits[cur] == i) { \
                UNIT_ASSERT_EQUAL(bitmap.Get(i), true); \
                ++cur; \
            } else { \
                UNIT_ASSERT_EQUAL(bitmap.Get(i), false); \
            } \
        } \
    }

#define CHECK_BITMAP_WITH_TAIL(bitmap, bits) \
    { \
        size_t cur = 0, end = sizeof(bits) / sizeof(size_t); \
        for (size_t i = 0; i < bitmap.Size(); ++i) { \
            if (cur < end) { \
                if (bits[cur] == i) { \
                    UNIT_ASSERT_EQUAL(bitmap.Get(i), true); \
                    ++cur; \
                } else { \
                    UNIT_ASSERT_EQUAL(bitmap.Get(i), false); \
                } \
            } else { \
                UNIT_ASSERT_EQUAL(bitmap.Get(i), true); \
            } \
        } \
    }

class TBitMapTest: public TTestBase {
    UNIT_TEST_SUITE(TBitMapTest);
        UNIT_TEST(TestBitMap)
        UNIT_TEST(TestDynBitMap)
        UNIT_TEST(TestAndFixed)
        UNIT_TEST(TestAndDyn)
        UNIT_TEST(TestOrFixed)
        UNIT_TEST(TestOrDyn)
        UNIT_TEST(TestCopy)
        UNIT_TEST(TestOps)
        UNIT_TEST(TestShiftFixed)
        UNIT_TEST(TestShiftDyn)
        UNIT_TEST(TestShiftExpansion)
        UNIT_TEST(TestFixedSanity)
    UNIT_TEST_SUITE_END();
private:
    void TestBitMap() {
        TBitMap<101> bitmap;

        UNIT_ASSERT_EQUAL(bitmap.Size(), 101);
        UNIT_ASSERT_EQUAL(bitmap.Count(), 0);
        UNIT_ASSERT_EQUAL(bitmap.FirstNonZeroBit(), 101);

        size_t initBits[] = {0, 50, 100, 45};
        INIT_BITMAP(bitmap, initBits);
        bitmap.Reset(45);

        UNIT_ASSERT_EQUAL(bitmap.FirstNonZeroBit(), 0);
        size_t setBits[] = {0, 50, 100};
        CHECK_BITMAP(bitmap, setBits);

        for (size_t i = 0; i < bitmap.Size(); ++i)
            UNIT_ASSERT_EQUAL(bitmap.Get(i), bitmap.Test(i));

        UNIT_ASSERT_EQUAL(bitmap.Count(), 3);

        bitmap.Reset(0);

        UNIT_ASSERT_EQUAL(bitmap.FirstNonZeroBit(), 50);

        bitmap.Clear();

        UNIT_ASSERT_EQUAL(bitmap.Count(), 0);
        UNIT_ASSERT_EQUAL(bitmap.Empty(), true);
    }

    void TestDynBitMap() {
        TDynBitMap bitmap;

        UNIT_ASSERT_EQUAL(bitmap.Size(), 64); // Initial capacity
        UNIT_ASSERT_EQUAL(bitmap.Count(), 0);
        UNIT_ASSERT_EQUAL(bitmap.FirstNonZeroBit(), 64);

        size_t initBits[] = {0, 50, 100, 45};
        INIT_BITMAP(bitmap, initBits);
        bitmap.Reset(45);

        UNIT_ASSERT_EQUAL(bitmap.Size(), 128);

        UNIT_ASSERT_EQUAL(bitmap.FirstNonZeroBit(), 0);
        size_t setBits[] = {0, 50, 100};
        CHECK_BITMAP(bitmap, setBits);

        for (size_t i = 0; i < bitmap.Size(); ++i)
            UNIT_ASSERT_EQUAL(bitmap.Get(i), bitmap.Test(i));

        UNIT_ASSERT_EQUAL(bitmap.Count(), 3);

        bitmap.Reset(0);

        UNIT_ASSERT_EQUAL(bitmap.FirstNonZeroBit(), 50);

        bitmap.Clear();

        UNIT_ASSERT_EQUAL(bitmap.Count(), 0);
        UNIT_ASSERT_EQUAL(bitmap.Empty(), true);
    }

    void TestAndFixed() {
        TestAndImpl< TBitMap<101> >();
    }

    void TestAndDyn() {
        TestAndImpl<TDynBitMap>();
    }

    template <class TBitMapImpl>
    void TestAndImpl() {
        TBitMapImpl bitmap1;
        TBitMapImpl bitmap2;

        size_t initBits1[] = {10, 20, 50, 100};
        size_t initBits2[] = {10, 11, 22, 50};

        INIT_BITMAP(bitmap1, initBits1);
        INIT_BITMAP(bitmap2, initBits2);

        bitmap1.And(bitmap2);

        UNIT_ASSERT_EQUAL(bitmap1.Count(), 2);
        UNIT_ASSERT_EQUAL(bitmap2.Count(), 4);

        size_t resBits[] = {10, 50};

        CHECK_BITMAP(bitmap1, resBits);
        CHECK_BITMAP(bitmap2, initBits2);
    }

    void TestOrFixed() {
        TestOrImpl< TBitMap<77> >();
    }

    void TestOrDyn() {
        TestOrImpl<TDynBitMap>();
    }


    template <class TBitMapImpl>
    void TestOrImpl() {
        TBitMapImpl bitmap1;
        TBitMapImpl bitmap2;

        size_t initBits1[] = {0, 10, 11, 76};
        size_t initBits2[] = {1, 11, 22, 50};

        INIT_BITMAP(bitmap1, initBits1);
        INIT_BITMAP(bitmap2, initBits2);

        bitmap1.Or(bitmap2);

        UNIT_ASSERT_EQUAL(bitmap1.Count(), 7);
        UNIT_ASSERT_EQUAL(bitmap2.Count(), 4);

        size_t resBits[] = {0, 1, 10, 11, 22, 50, 76};

        CHECK_BITMAP(bitmap1, resBits);
        CHECK_BITMAP(bitmap2, initBits2);
   }

    void TestCopy() {
        TBitMap<101> bitmap1;
        size_t initBits[] = {0, 10, 11, 76, 100};

        INIT_BITMAP(bitmap1, initBits);

        TDynBitMap bitmap2(bitmap1);
        CHECK_BITMAP(bitmap2, initBits);

        TBitMap<101> bitmap3(bitmap1);
        CHECK_BITMAP(bitmap3, initBits);

        TBitMap<127> bitmap4(bitmap1);
        CHECK_BITMAP(bitmap4, initBits);

        TDynBitMap bitmap5;
        bitmap5 = bitmap1;
        CHECK_BITMAP(bitmap5, initBits);

        TBitMap<101> bitmap6;
        bitmap6 = bitmap1;
        CHECK_BITMAP(bitmap6, initBits);

        TBitMap<127> bitmap7;
        bitmap7 = bitmap1;
        CHECK_BITMAP(bitmap7, initBits);

        TBitMap<101> bitmap8;
        DoSwap(bitmap8, bitmap6);
        CHECK_BITMAP(bitmap8, initBits);
        UNIT_ASSERT_EQUAL(bitmap6.Empty(), true);

        TDynBitMap bitmap9;
        DoSwap(bitmap9, bitmap5);
        CHECK_BITMAP(bitmap9, initBits);
        UNIT_ASSERT_EQUAL(bitmap5.Empty(), true);
    }

    void TestOps() {
        TBitMap<16> bitmap1;
        TBitMap<12> bitmap2;
        size_t initBits1[] = {0, 3, 7, 11};
        size_t initBits2[] = {1, 3, 6, 7, 11};
        INIT_BITMAP(bitmap1, initBits1);
        INIT_BITMAP(bitmap2, initBits2);

        bitmap1.Or(3).And(bitmap2).Flip();

        size_t resBits[] = {0, 2, 4, 5, 6, 8, 9, 10, 12};
        CHECK_BITMAP_WITH_TAIL(bitmap1, resBits);

        TDynBitMap bitmap3;
        INIT_BITMAP(bitmap3, initBits1);

        bitmap3.Or(3).And(bitmap2).Flip();

        CHECK_BITMAP_WITH_TAIL(bitmap3, resBits);

        bitmap3.Clear();
        INIT_BITMAP(bitmap3, initBits1);

        TDynBitMap bitmap4 = ~((bitmap3 | 3 ) & bitmap2);
        CHECK_BITMAP_WITH_TAIL(bitmap4, resBits);
    }

    void TestShiftFixed() {
        size_t initBits[] = {0, 3, 7, 11};

        TBitMap<128> bitmap1;

        INIT_BITMAP(bitmap1, initBits);
        bitmap1 <<= 62;
        size_t resBits1[] = {62, 65, 69, 73};
        CHECK_BITMAP(bitmap1, resBits1);
        bitmap1 >>= 62;
        CHECK_BITMAP(bitmap1, initBits);

        bitmap1.Clear();
        INIT_BITMAP(bitmap1, initBits);
        bitmap1 <<= 120;
        size_t resBits2[] = {120, 123, 127};
        CHECK_BITMAP(bitmap1, resBits2);
        bitmap1 >>= 120;
        size_t resBits3[] = {0, 3, 7};
        CHECK_BITMAP(bitmap1, resBits3);

        bitmap1.Clear();
        INIT_BITMAP(bitmap1, initBits);
        bitmap1 <<= 128;
        UNIT_ASSERT_EQUAL(bitmap1.Empty(), true);

        bitmap1.Clear();
        INIT_BITMAP(bitmap1, initBits);
        bitmap1 <<= 120;
        bitmap1 >>= 128;
        UNIT_ASSERT_EQUAL(bitmap1.Empty(), true);

        bitmap1.Clear();
        INIT_BITMAP(bitmap1, initBits);
        bitmap1 <<= 140;
        UNIT_ASSERT_EQUAL(bitmap1.Empty(), true);

        bitmap1.Clear();
        INIT_BITMAP(bitmap1, initBits);
        bitmap1 <<= 62;
        bitmap1 >>= 140;
        UNIT_ASSERT_EQUAL(bitmap1.Empty(), true);
    }

    void TestShiftDyn() {
        size_t initBits[] = {0, 3, 7, 11};

        TDynBitMap bitmap1;

        INIT_BITMAP(bitmap1, initBits);
        bitmap1 <<= 62;
        size_t resBits1[] = {62, 65, 69, 73};
        CHECK_BITMAP(bitmap1, resBits1);
        bitmap1 >>= 62;
        CHECK_BITMAP(bitmap1, initBits);

        bitmap1.Clear();
        INIT_BITMAP(bitmap1, initBits);
        bitmap1 <<= 120;
        size_t resBits2[] = {120, 123, 127, 131};
        CHECK_BITMAP(bitmap1, resBits2);
        bitmap1 >>= 120;
        CHECK_BITMAP(bitmap1, initBits);

        bitmap1.Clear();
        INIT_BITMAP(bitmap1, initBits);
        bitmap1 <<= 128;
        size_t resBits3[] = {128, 131, 135, 139};
        CHECK_BITMAP(bitmap1, resBits3);

        bitmap1.Clear();
        INIT_BITMAP(bitmap1, initBits);
        bitmap1 <<= 120;
        bitmap1 >>= 128;
        size_t resBits4[] = {3};
        CHECK_BITMAP(bitmap1, resBits4);

        bitmap1.Clear();
        INIT_BITMAP(bitmap1, initBits);
        bitmap1 <<= 62;
        bitmap1 >>= 140;
        UNIT_ASSERT_EQUAL(bitmap1.Empty(), true);
    }

    // Test that we don't expand bitmap in LShift when high-order bits are zero
    void TestShiftExpansion() {

        UNIT_ASSERT_EQUAL(TDynBitMap().LShift(1).Size(), 64);
        UNIT_ASSERT_EQUAL(TDynBitMap().LShift(65).Size(), 64);
        UNIT_ASSERT_EQUAL(TDynBitMap().LShift(128).Size(), 64);

        TDynBitMap bitmap;
        bitmap.Set(62).LShift(1);
        UNIT_ASSERT_EQUAL(bitmap, TDynBitMap().Set(63));
        UNIT_ASSERT_EQUAL(bitmap.Size(), 64);

        // Expand explicitly
        bitmap.Set(65);
        UNIT_ASSERT_EQUAL(bitmap.Size(), 128);

        bitmap.Clear().Set(0).LShift(1);
        UNIT_ASSERT_EQUAL(bitmap, TDynBitMap().Set(1));
        UNIT_ASSERT_EQUAL(bitmap.Size(), 128);

        bitmap.Clear().Set(63).LShift(1);
        UNIT_ASSERT_EQUAL(bitmap, TDynBitMap().Set(64));
        UNIT_ASSERT_EQUAL(bitmap.Size(), 128);

        bitmap.Clear().Set(63).LShift(64);
        UNIT_ASSERT_EQUAL(bitmap, TDynBitMap().Set(127));
        UNIT_ASSERT_EQUAL(bitmap.Size(), 128);

        bitmap.Clear().Set(62).LShift(129);
        UNIT_ASSERT_EQUAL(bitmap, TDynBitMap().Set(191));
        UNIT_ASSERT_EQUAL(bitmap.Size(), 192);
    }

    void TestFixedSanity() {
        // test extra-bit cleanup
        UNIT_ASSERT_EQUAL(TBitMap<33>().Set(0).LShift(34).RShift(34).Empty(), true);
        UNIT_ASSERT_EQUAL(TBitMap<88>().Set(0).Set(1).Set(2).LShift(90).RShift(90).Empty(), true);
        UNIT_ASSERT_EQUAL(TBitMap<88>().Flip().RShift(88).Empty(), true);
        UNIT_ASSERT_EQUAL(TBitMap<64>().Flip().LShift(2).RShift(2).Count(), 62);
        UNIT_ASSERT_EQUAL(TBitMap<67>().Flip().LShift(2).RShift(2).Count(), 65);
        UNIT_ASSERT_EQUAL(TBitMap<128>().Flip().LShift(2).RShift(2).Count(), 126);
        UNIT_ASSERT_EQUAL(TBitMap<130>().Flip().LShift(2).RShift(2).Count(), 128);
        UNIT_ASSERT_EQUAL(TBitMap<130>(TDynBitMap().Set(131)).Empty(), true);
        UNIT_ASSERT_EQUAL(TBitMap<33>().Or(TBitMap<40>().Set(39)).Empty(), true);
        UNIT_ASSERT_EQUAL(TBitMap<33>().Xor(TBitMap<40>().Set(39)).Empty(), true);
    }
};

UNIT_TEST_SUITE_REGISTRATION(TBitMapTest);
