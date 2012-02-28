#include "sorter.h"

#include <library/unittest/registar.h>

#include <util/digest/md5.h>
#include <util/generic/static_assert.h>

struct stru48 {
    char data[6];
    stru48() {}
    stru48(ui64 src) { memcpy(data, &src, 6); }
    bool operator <(const stru48 &w) const { return memcmp(data, w.data, 6) < 0; }
    inline stru48& operator= (const stru48& r) throw () {
        if (this != &r) {
            memcpy(data, r.data, sizeof(data));
        }

        return *this;
    }
};

struct stru96 {
    ui32 data[3];
    stru96() {}
    stru96(ui64 src) { memcpy(data, &src, 8); data[2] = data[0] + data[1]; }
    bool operator <(const stru96 &w) const { return memcmp(data, w.data, 12) < 0; }
};

using namespace NSorter;

template<bool Compressed>
class TSorterTestBase: public TTestBase
{
public:
    template <class T, class TSortMode>
    Stroka SortRandomData() {
        ui64 mydata_seed = 0;
        typedef NSorter::TSorter<T, TSortMode> TSorter;
        TSorter srt(100000, 0, Compressed);
        srt.SetFileNameCallback(new NSorter::TPortionFileNameCallback2("unittest"), 0);
        for (int n = 0; n < 345678; n++) {
            srt.PushBack((T)mydata_seed);
            mydata_seed = (mydata_seed + 5678) * 6151;
        }
        for (int n = 0; n < 123456; n++)
            srt.PushBack((T)mydata_seed);
        typename TSortMode::TIterator i(22 * 1024);
        srt.Close(i);
        MD5 md5;
        for (; !i.Finished(); ++i)
            md5.Update((ui8*)&*i, sizeof(T));
        char out_buf[26];
        md5.End_b64(out_buf);
        return out_buf;
    }
    void testSort32() {
        Stroka res = SortRandomData<ui32, TSimpleSort<ui32> >();
        //UNIT_ASSERT_EQUAL(res == "", true);
#ifdef _little_endian_
        UNIT_ASSERT_EQUAL(Stroka("hQdVu3H5d78LikP6tosBbA=="), res);
#else
        UNIT_ASSERT_EQUAL(Stroka(""), res); // to do!
#endif
    }
    void testSortUniq32() {
        Stroka res = SortRandomData<ui32, TSmartUniq<ui32> >();
#ifdef _little_endian_
        UNIT_ASSERT_EQUAL(Stroka("fKgxZLzIPaaMUkCq5GQu9g=="), res);
#else
        UNIT_ASSERT_EQUAL(Stroka(""), res); // to do!
#endif
    }
    void testSort48() {
        Stroka res = SortRandomData<stru48, TSimpleSort<stru48> >();
#ifdef _little_endian_
        UNIT_ASSERT_EQUAL(Stroka("6FN+n+n7JKdHQ+wKHem27w=="), res);
#else
        UNIT_ASSERT_EQUAL(Stroka(""), res); // to do!
#endif
    }
    void testSort64() {
#ifndef _win32_ // этот тест не работает на _win32_ по неизвестным причинам
        Stroka res = SortRandomData<ui64, TSimpleSort<ui64> >();
#ifdef _little_endian_
        UNIT_ASSERT_EQUAL(Stroka("i6bGEpE0qOjs9N9OoKp/hQ=="), res);
#else
        UNIT_ASSERT_EQUAL(Stroka(""), res); // to do!
#endif
#endif
    }
    void testSort96() {
        Stroka res = SortRandomData<stru96, TSimpleSort<stru96> >();
#ifdef _little_endian_
        UNIT_ASSERT_EQUAL(Stroka("4v/t41l4lCK00OlCMGX4UA=="), res);
#else
        UNIT_ASSERT_EQUAL(Stroka(""), res); // to do!
#endif
    }
};

class TSorterTest : public TSorterTestBase<false> {
    UNIT_TEST_SUITE(SorterTest);
        UNIT_TEST(testSort32);
        UNIT_TEST(testSortUniq32);
        UNIT_TEST(testSort48);
        UNIT_TEST(testSort64);
        UNIT_TEST(testSort96);
    UNIT_TEST_SUITE_END();
};
UNIT_TEST_SUITE_REGISTRATION(TSorterTest);

class TCompressedSorterTest : public TSorterTestBase<true> {
    UNIT_TEST_SUITE(CompressedSorterTest);
        UNIT_TEST(testSort32);
        UNIT_TEST(testSort48);
        UNIT_TEST(testSort64);
        UNIT_TEST(testSort96);
    UNIT_TEST_SUITE_END();
};
UNIT_TEST_SUITE_REGISTRATION(TCompressedSorterTest);

