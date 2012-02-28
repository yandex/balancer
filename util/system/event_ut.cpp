#include "event.h"
#include "atomic.h"

#include <library/unittest/registar.h>

#include <util/thread/queue.h>
#include <util/stream/output.h>

struct TEventTest : public TTestBase {
    UNIT_TEST_SUITE(TEventTest);
        UNIT_TEST(TestEvents)
    UNIT_TEST_SUITE_END();

    struct TSharedData {
        TSharedData()
            : Counter(0)
            , failed(false)
        {
        }

        TAtomic Counter;
        Event event;
        bool failed;
    };

    struct TThreadTask : public IObjectInQueue {
        public:
            TThreadTask(TSharedData& data, size_t id)
                : Data_(data)
                , Id_(id)
            {
            }

#define FAIL_ASSERT(cond) \
    if (!(cond)) { \
        Data_.failed = true; \
    }
            void Process(void*) {
                THolder<TThreadTask> This(this);

                if (Id_ == 0) {
                    usleep(100);
                    FAIL_ASSERT(Data_.Counter == 0);
                    Data_.event.Signal();
                } else {
                    while (!Data_.event.WaitT(TDuration::Seconds(100))) {
                    }
                    AtomicAdd(Data_.Counter, Id_);
                }
            }
#undef FAIL_ASSERT

        private:
            TSharedData& Data_;
            size_t Id_;

    };

    private:
        void TestEvents() {
            Q_.Start(5);
            for (size_t i = 0; i < 5; ++i) {
                UNIT_ASSERT(Q_.Add(new TThreadTask(Data_, i)));
            }
            Q_.Stop();
            UNIT_ASSERT(Data_.Counter == 10);
            UNIT_ASSERT(!Data_.failed);
        }

    private:
        TSharedData Data_;
        TMtpQueue Q_;
};

UNIT_TEST_SUITE_REGISTRATION(TEventTest);
