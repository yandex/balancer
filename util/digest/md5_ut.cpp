#include "md5.h"

#include <library/unittest/registar.h>

#include <util/system/fs.h>
#include <util/stream/file.h>

class TMD5Test: public TTestBase {
        UNIT_TEST_SUITE(TMD5Test);
            UNIT_TEST(TestMD5)
            UNIT_TEST(TestFile)
        UNIT_TEST_SUITE_END();
    private:
        inline void TestMD5() {
            // echo -n 'qwertyuiopqwertyuiopasdfghjklasdfghjkl' | md5sum
            char b[] = "qwertyuiopqwertyuiopasdfghjklasdfghjkl";

            MD5 r;
            r.Update((const unsigned char*)b, 15);
            r.Update((const unsigned char*)b + 15, strlen(b) - 15);

            char rs[33];
            Stroka s(r.End(rs));
            s.to_lower();

            UNIT_ASSERT_EQUAL(s, Stroka("3ac00dd696b966fd74deee3c35a59d8f"));
        }

        inline void TestFile() {
            Stroka s = NUnitTest::RandomString(1000000, 1);
            const Stroka tmpFile = "tmp";

            {
                TBufferedFileOutput fo(tmpFile);

                fo.Write(~s, +s);
            }

            char buf1[100];
            char buf2[100];

            UNIT_ASSERT_EQUAL(Stroka(MD5::File(~tmpFile, buf1)), Stroka(MD5::Data((const unsigned char*)~s, +s, buf2)));

            NFs::Remove(~tmpFile);
        }
};

UNIT_TEST_SUITE_REGISTRATION(TMD5Test);
