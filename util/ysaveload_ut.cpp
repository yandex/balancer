#include "ysaveload.h"

#include <library/unittest/registar.h>

#include <util/memory/pool.h>
#include <util/stream/buffer.h>
#include <util/generic/map.h>
#include <util/generic/set.h>
#include <util/generic/hash.h>
#include <util/generic/deque.h>
#include <util/generic/stroka.h>
#include <util/generic/vector.h>
#include <util/generic/buffer.h>
#include <util/generic/hash_set.h>
#include <util/memory/blob.h>

static inline char* AllocateFromPool(TMemoryPool& pool, size_t len) {
    return (char*)pool.Allocate(len);
}

class TSaveLoadTest: public TTestBase {
        UNIT_TEST_SUITE(TSaveLoadTest);
            UNIT_TEST(TestSave)
            UNIT_TEST(TestLoad)
        UNIT_TEST_SUITE_END();

        struct TSaveHelper {
            inline void Save(TOutputStream* o) const {
                o->Write("qwerty", 7);
            }

            inline void Load(TInputStream* i) {
                char buf[7];

                UNIT_ASSERT_EQUAL(i->Load(buf, 7), 7);
                UNIT_ASSERT_EQUAL(strcmp(buf, "qwerty"), 0);
            }
        };

    private:
        inline void TestSave() {
            Save(&S_, (ui8)1);
            Save(&S_, (ui16)2);
            Save(&S_, (ui32)3);
            Save(&S_, (ui64)4);

            {
                yvector<ui16> vec;

                vec.push_back((ui16)1);
                vec.push_back((ui16)2);
                vec.push_back((ui16)4);

                Save(&S_, vec);
            }

            {
                ymap<ui16, ui32> map;

                map[(ui16)1] = 2;
                map[(ui16)2] = 3;
                map[(ui16)3] = 4;

                Save(&S_, map);
            }

            {
                TSaveHelper helper;

                Save(&S_, helper);
            }

            {
                Stroka val("123456");

                Save(&S_, val);
            }

            {
                TBuffer buf;

                buf.Append("asdf", 4);
                Save(&S_, buf);
            }

            {
                yvector<const char*> vec;

                vec.push_back("1");
                vec.push_back("123");
                vec.push_back("4567");

                Save(&S_, vec);
            }

            {
                ydeque<ui16> deq;

                deq.push_back(1);
                deq.push_back(2);
                deq.push_back(4);
                deq.push_back(5);

                Save(&S_, deq);
            }
        }

        inline void TestLoad() {
            {
                ui8 val;

                Load(&S_, val);
                UNIT_ASSERT_EQUAL(val, 1);
            }

            {
                ui16 val;

                Load(&S_, val);
                UNIT_ASSERT_EQUAL(val, 2);
            }

            {
                ui32 val;

                Load(&S_, val);
                UNIT_ASSERT_EQUAL(val, 3);
            }

            {
                ui64 val;

                Load(&S_, val);
                UNIT_ASSERT_EQUAL(val, 4);
            }

            {
                yvector<ui16> vec;

                Load(&S_, vec);
                UNIT_ASSERT_EQUAL(+vec, 3);
                UNIT_ASSERT_EQUAL(vec[0], 1);
                UNIT_ASSERT_EQUAL(vec[1], 2);
                UNIT_ASSERT_EQUAL(vec[2], 4);
            }

            {
                ymap<ui16, ui32> map;

                Load(&S_, map);
                UNIT_ASSERT_EQUAL(map.size(), 3);
                UNIT_ASSERT_EQUAL(map[(ui16)1], 2);
                UNIT_ASSERT_EQUAL(map[(ui16)2], 3);
                UNIT_ASSERT_EQUAL(map[(ui16)3], 4);
            }

            {
                TSaveHelper helper;

                Load(&S_, helper);
            }

            {
                Stroka val;

                Load(&S_, val);
                UNIT_ASSERT_EQUAL(val, "123456");
            }

            {
                TBuffer buf;

                Load(&S_, buf);
                UNIT_ASSERT_EQUAL(+buf, 4);
                UNIT_ASSERT_EQUAL(memcmp(~buf, "asdf", 4), 0);
            }

            {
                yvector<const char*> vec;
                TMemoryPool pool(1024);

                Load(&S_, vec, pool);

                UNIT_ASSERT_EQUAL(+vec, 3);
                UNIT_ASSERT_EQUAL(vec[0], Stroka("1"));
                UNIT_ASSERT_EQUAL(vec[1], Stroka("123"));
                UNIT_ASSERT_EQUAL(vec[2], Stroka("4567"));
            }

            {
                ydeque<ui16> deq;

                Load(&S_, deq);

                UNIT_ASSERT_EQUAL(+deq, 4);
                UNIT_ASSERT_EQUAL(deq[0], 1);
                UNIT_ASSERT_EQUAL(deq[1], 2);
                UNIT_ASSERT_EQUAL(deq[2], 4);
                UNIT_ASSERT_EQUAL(deq[3], 5);
            }
        }

    private:
        TBufferStream S_;
};

UNIT_TEST_SUITE_REGISTRATION(TSaveLoadTest);
