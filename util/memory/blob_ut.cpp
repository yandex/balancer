#include "blob.h"

#include <library/unittest/registar.h>

#include <util/stream/ios.h>

class TBlobTest: public TTestBase {
        UNIT_TEST_SUITE(TBlobTest);
            UNIT_TEST(TestSubBlob);
            UNIT_TEST(TestFromStream);
            UNIT_TEST(TestFromString);
        UNIT_TEST_SUITE_END();
    public:
        inline void TestSubBlob() {
            TBlob child;
            const char* p = NULL;

            {
                TBlob parent = TBlob::CopySingleThreaded("0123456789", 10);
                UNIT_ASSERT_EQUAL(parent.Length(), 10);
                p = parent.AsCharPtr();
                UNIT_ASSERT_EQUAL(memcmp(p, "0123456789", 10), 0);
                child = parent.SubBlob(2, 5);
            } // Don't worry about parent

            UNIT_ASSERT_EQUAL(child.Length(), 3);
            UNIT_ASSERT_EQUAL(memcmp(child.AsCharPtr(), "234", 3), 0);
            UNIT_ASSERT_EQUAL(p + 2, child.AsCharPtr());
        }

        inline void TestFromStream() {
            Stroka s("sjklfgsdyutfuyas54fa78s5f89a6df790asdf7");
            TMemoryInput mi(~s, +s);
            TBlob b = TBlob::FromStreamSingleThreaded(mi);

            UNIT_ASSERT_EQUAL(Stroka((const char*)b.Data(), b.Length()), s);
        }

        inline void TestFromString() {
            Stroka s("dsfkjhgsadftusadtf");
            TBlob b(TBlob::FromString(s));

            UNIT_ASSERT_EQUAL(Stroka((const char*)b.Data(), b.Size()), s);
        }
};

UNIT_TEST_SUITE_REGISTRATION(TBlobTest);
