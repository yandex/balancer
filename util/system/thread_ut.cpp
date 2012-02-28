#include "thread.h"

#include <library/unittest/registar.h>

class TSysThreadTest: public TTestBase {
        UNIT_TEST_SUITE(TSysThreadTest);
            UNIT_TEST(TestThreadId)
        UNIT_TEST_SUITE_END();
    private:
        struct TIdTester {
            inline TIdTester()
                : Thr(0)
                , Cur(0)
                , Real(0)
            {
            }

            static inline void* DoRun(void* ptr) {
                ((TIdTester*)ptr)->Run();

                return 0;
            }

            inline void Run() {
                Cur = TThread::CurrentThreadId();
                Real = Thr->Id();
            }

            TThread* Thr;
            TThread::TId Cur;
            TThread::TId Real;
        };

        inline void TestThreadId() {
            TIdTester tst;
            TThread thr(tst.DoRun, &tst);

            tst.Thr = &thr;

            thr.Start();
            thr.Join();

            UNIT_ASSERT_EQUAL(tst.Cur, tst.Real);
            UNIT_ASSERT(tst.Cur != 0);
        }
};

UNIT_TEST_SUITE_REGISTRATION(TSysThreadTest);
