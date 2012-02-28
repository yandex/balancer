#include "guard.h"

#include <library/unittest/registar.h>

#include <util/thread/queue.h>

struct TTestGuard : public TTestBase {
    UNIT_TEST_SUITE(TTestGuard);
        UNIT_TEST(TestGuard)
    UNIT_TEST_SUITE_END();

    struct TGuardChecker {
        TGuardChecker()
            : guarded(false)
        {
        }

        void Acquire() {
            guarded = true;
        }
        void Release() {
            guarded = false;
        }

        bool guarded;
    };

    void TestGuard() {
        TGuardChecker checker;

        UNIT_ASSERT(!checker.guarded);
        {
            TGuard<TGuardChecker> guard(checker);
            UNIT_ASSERT(checker.guarded);
        }
        UNIT_ASSERT(!checker.guarded);
    }
};

UNIT_TEST_SUITE_REGISTRATION(TTestGuard)
