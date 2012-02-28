#include "pool.h"

#include <library/unittest/registar.h>

#include <util/stream/ios.h>

class TMemPoolTest: public TTestBase {
        UNIT_TEST_SUITE(TMemPoolTest);
            UNIT_TEST(TestMemPool)
        UNIT_TEST_SUITE_END();
    private:
        inline void TestMemPool() {
            class TAlloc: public TDefaultAllocator {
                public:
                    inline TAlloc()
                        : Alloced_(0)
                        , Released_(0)
                        , Allocs_(0)
                        , Frees_(0)
                    {
                    }

                    virtual TBlock Allocate(size_t len) {
                        Check();

                        Alloced_ += len;
                        ++Allocs_;

                        return TDefaultAllocator::Allocate(len);
                    }

                    virtual void Release(const TBlock& block) {
                        Released_ += block.Len;
                        ++Frees_;

                        Check();

                        TDefaultAllocator::Release(block);
                    }

                    inline void CheckAtEnd() {
                        UNIT_ASSERT_EQUAL(Alloced_, Released_);
                        UNIT_ASSERT_EQUAL(Allocs_, Frees_);
                    }

                private:
                    inline void Check() {
                        UNIT_ASSERT(Alloced_ >= Released_);
                        UNIT_ASSERT(Allocs_ >= Frees_);
                    }

                private:
                    size_t Alloced_;
                    size_t Released_;
                    size_t Allocs_;
                    size_t Frees_;
            } alloc;

            {
                TMemoryPool pool(123, TMemoryPool::TExpGrow::Instance(), &alloc);

                for (size_t i = 0; i < 1000; ++i) {
                    UNIT_ASSERT(pool.Allocate(i));
                }
            }

            alloc.CheckAtEnd();
        }
};

UNIT_TEST_SUITE_REGISTRATION(TMemPoolTest);
