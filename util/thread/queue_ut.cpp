#include "queue.h"

#include <library/unittest/registar.h>

#include <util/stream/ios.h>
#include <util/random/randcpp.h>
#include <util/system/spinlock.h>

class TMtpQueueTest: public TTestBase {
        UNIT_TEST_SUITE(TMtpQueueTest);
            UNIT_TEST(TestTMtpQueue)
            //UNIT_TEST(TestTAdaptiveMtpQueue)
        UNIT_TEST_SUITE_END();
    private:
        TSpinLock lock;
        long r;

        struct TTask: public IObjectInQueue {
            TMtpQueueTest* Test;
            long Value;

            TTask(TMtpQueueTest* test, int value)
                : Test(test)
                , Value(value)
            {
            }

            void Process(void*) {
                TAutoPtr<TTask> This(this);

                TGuard<TSpinLock> guard(Test->lock);
                Test->r ^= Value;
            }
        };

        inline void TestAnyQueue(IMtpQueue* queue) {
            TRand rand;
            const size_t cnt = 1000;

            rand.srandom(17);
            r = 0;
            for (size_t i = 0; i < cnt; ++i) {
                r ^= rand.random();
            }

            queue->Start(10, cnt);

            rand.srandom(17);
            for (size_t i = 0; i < cnt; ++i) {
                UNIT_ASSERT(queue->Add(new TTask(this, rand.random())));
            }

            queue->Stop();

            UNIT_ASSERT_EQUAL(0, r);
        }

        inline void TestTMtpQueue() {
            TMtpQueue q;

            TestAnyQueue(&q);
        }

        inline void TestTAdaptiveMtpQueue() {
            TAdaptiveMtpQueue q;

            TestAnyQueue(&q);
        }
};

UNIT_TEST_SUITE_REGISTRATION(TMtpQueueTest);
