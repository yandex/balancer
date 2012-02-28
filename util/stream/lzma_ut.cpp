#include "lzma.h"

#include <library/unittest/registar.h>

#include <util/digest/crc.h>
#include <util/random/randcpp.h>

class TLzmaTest: public TTestBase {
        UNIT_TEST_SUITE(TLzmaTest);
            UNIT_TEST(Test1)
            UNIT_TEST(Test2)
        UNIT_TEST_SUITE_END();
    private:
        inline Stroka GenData() {
            Stroka data;
            TRand rnd;

            for (size_t i = 0; i < 50000; ++i) {
                const char ch = rnd.random() % 256;
                const size_t len = rnd.random() % 10;

                data += Stroka(len, ch);
            }

            return data;
        }

        inline void Test2() {
            class TExcOutput: public TOutputStream {
                public:
                    virtual ~TExcOutput() throw () {
                    }

                    virtual void DoWrite(const void*, size_t) {
                        throw 12345;
                    }
            };

            Stroka data(GenData());
            TMemoryInput mi(~data, +data);
            TExcOutput out;

            try {
                TLzmaCompress c(&out);

                TransferData(static_cast<IZeroCopyInput*>(&mi), &c);
            } catch (int i) {
                UNIT_ASSERT_EQUAL(i, 12345);
            }
        }

        inline void Test1() {
            Stroka data(GenData());
            Stroka data1;
            Stroka res;

            {
                TMemoryInput mi(~data, +data);
                TStringOutput so(res);
                TLzmaCompress c(&so);

                TransferData(static_cast<IZeroCopyInput*>(&mi), &c);

                c.Finish();
            }

            {
                TMemoryInput mi(~res, +res);
                TStringOutput so(data1);
                TLzmaDecompress d(&mi);

                TransferData(&d, &so);
            }

            UNIT_ASSERT_EQUAL(data, data1);
            UNIT_ASSERT_EQUAL(Crc<ui64>(~data, +data), ULL(17580906285496112424));
        }
};

UNIT_TEST_SUITE_REGISTRATION(TLzmaTest);
