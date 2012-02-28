#include "sem.h"

#include <library/unittest/registar.h>

#include <util/thread/queue.h>
#include <util/system/atomic.h>
#include <util/random/random.h>

class TSemaphoreTest : public TTestBase {
    UNIT_TEST_SUITE(TSemaphoreTest);
        UNIT_TEST(TestSemaphore)
    UNIT_TEST_SUITE_END();

    struct TSharedData {
        TSharedData()
            : Counter(0)
            , failed(false)
        {
        }

        TAtomic Counter;
        bool failed;
    };

    class TThreadTask : public IObjectInQueue {
        public:
            TThreadTask(TSharedData& data, const char* key, size_t freeCount, size_t id)
                : Data_(data)
                , Key_(key)
                , FreeCount_(freeCount)
                , Id_(id)
            {
            }

#define FAIL_ASSERT(cond) \
    if (!(cond)) { \
        Data_.failed = true; \
    }
            void Process(void*) {
                THolder<TThreadTask> This(this);

                TSemaphore semaphore(Key_, (ui32)FreeCount_);

                for (size_t i = 0; i < 10; ++i) {
                    while (!semaphore.Acquire()) {
                    }

                    AtomicIncrement(Data_.Counter);
                    FAIL_ASSERT(Data_.Counter <= long(FreeCount_));
                    usleep(1 + RandomNumber<ui32>() % 8);
                    AtomicDecrement(Data_.Counter);

                    while (!semaphore.Release()) {
                    }
                }
            }

#undef FAIL_ASSERT

            private:
                TSharedData& Data_;
                const char* Key_;
                size_t FreeCount_;
                size_t Id_;
    };

    private:
       void TestSemaphore() {
            Data_.Counter = 0;
            Q_.Start(10);
            for (size_t i = 0; i < 10; ++i) {
                UNIT_ASSERT(Q_.Add(new TThreadTask(Data_, "just key", 4, i)));
            }
            Q_.Stop();
            UNIT_ASSERT(!Data_.failed);
       }
    private:
        TSharedData Data_;
        TMtpQueue Q_;
};

UNIT_TEST_SUITE_REGISTRATION(TSemaphoreTest)
