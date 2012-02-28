#include "smallobj.h"

#include <library/unittest/registar.h>

#include <util/generic/hash_set.h>

class TSmallObjAllocTest: public TTestBase {
        struct TClass: public TObjectFromPool<TClass> {
            char buf[125];
        };

        struct TErrClass: public TObjectFromPool<TErrClass>  {
            inline TErrClass(bool t) {
                if (t) {
                    throw 1;
                }
            }
        };

        UNIT_TEST_SUITE(TSmallObjAllocTest);
            UNIT_TEST(TestAllocate)
            UNIT_TEST(TestError)
        UNIT_TEST_SUITE_END();
    private:
        inline void TestError() {
            TErrClass::TPool pool(TDefaultAllocator::Instance());
            TErrClass* f = new (&pool) TErrClass(false);

            delete f;

            bool wasError = false;

            try {
                new (&pool) TErrClass(true);
            } catch (...) {
                wasError = true;
            }

            UNIT_ASSERT(wasError);
            UNIT_ASSERT_EQUAL(f, new (&pool) TErrClass(false));
        }

        inline void TestAllocate() {
            TClass::TPool pool(TDefaultAllocator::Instance());
            yhash_set<TClass*> alloced;

            for (size_t i = 0; i < 10000; ++i) {
                TClass* c = new (&pool) TClass;

                UNIT_ASSERT_EQUAL(alloced.find(c), alloced.end());
                alloced.insert(c);
            }

            for (yhash_set<TClass*>::iterator it = alloced.begin(); it != alloced.end(); ++it) {
                delete *it;
            }

            for (size_t i = 0; i < 10000; ++i) {
                TClass* c = new (&pool) TClass;

                UNIT_ASSERT(alloced.find(c) != alloced.end());
            }

            UNIT_ASSERT_EQUAL(alloced.find(new (&pool) TClass), alloced.end());
        }
};

UNIT_TEST_SUITE_REGISTRATION(TSmallObjAllocTest);
