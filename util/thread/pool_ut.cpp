#include "pool.h"
#include "queue.h"

#include <library/unittest/registar.h>

class TThrPoolTest: public TTestBase {
        UNIT_TEST_SUITE(TThrTest);
            UNIT_TEST(TestSystemPool)
            UNIT_TEST(TestAdaptivePool)
        UNIT_TEST_SUITE_END();

        struct TRunAble: public IThreadPool::IThreadAble {
            inline TRunAble()
                : done(false)
            {
            }

            virtual ~TRunAble() throw () {
            }

            virtual void DoExecute() {
                done = true;
            }

            bool done;
        };

    private:
        inline void TestSystemPool() {
            TRunAble r;

            {
                TAutoPtr<IThreadPool::IThread> thr = SystemThreadPool()->Run(&r);

                thr->Join();
            }

            UNIT_ASSERT_EQUAL(r.done, true);
        }

        inline void TestAdaptivePool() {
            TRunAble r;

            {
                TAdaptiveMtpQueue pool;

                pool.Start(0);

                TAutoPtr<IThreadPool::IThread> thr = pool.Run(&r);

                thr->Join();
            }

            UNIT_ASSERT_EQUAL(r.done, true);
        }
};

UNIT_TEST_SUITE_REGISTRATION(TThrPoolTest);
