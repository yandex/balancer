#include "yexception.h"

static inline void Throw1DontMove() {
    ythrow yexception() << "blabla"; //dont move this line
}

static inline void Throw2DontMove() {
    ythrow yexception() << 1 << " qw " << 12.1; //dont move this line
}

#include <library/unittest/registar.h>

#include <util/stream/ios.h>

#include "yexception_ut.h"

#if defined (_MSC_VER)
#   pragma warning(disable:4702) /*unreachable code*/
#endif

static void CallbackFun(int i) {
    throw i;
}

static TOutputStream* OUTS = 0;

class TExceptionTest: public TTestBase {
        UNIT_TEST_SUITE(TExceptionTest);
            UNIT_TEST_EXCEPTION(TestException, yexception)
            UNIT_TEST_EXCEPTION(TestLineInfo, yexception)
            UNIT_TEST(TestFormat1)
            UNIT_TEST(TestRaise1)
            UNIT_TEST(TestVirtuality)
            UNIT_TEST(TestVirtualInheritance)
            UNIT_TEST(TestMixedCode)
        UNIT_TEST_SUITE_END();
    private:
        inline void TestVirtualInheritance() {
            TStringStream ss;

            OUTS = &ss;

            class TA {
                public:
                    inline TA() {
                        *OUTS << "A";
                    }
            };

            class TB {
                public:
                    inline TB() {
                        *OUTS << "B";
                    }
            };

            class TC: public virtual TB, public virtual TA {
                public:
                    inline TC() {
                        *OUTS << "C";
                    }
            };

            class TD: public virtual TA {
                public:
                    inline TD() {
                        *OUTS << "D";
                    }
            };

            class TE: public TC, public TD {
                public:
                    inline TE() {
                        *OUTS << "E";
                    }
            };

            TE e;

            UNIT_ASSERT_EQUAL(ss.Str(), "BACDE");
        }

        inline void TestVirtuality() {
            try {
                ythrow TFileError() << "1";
                UNIT_ASSERT(false);
            } catch (const TIoException&) {
            } catch (...) {
                UNIT_ASSERT(false);
            }

            try {
                ythrow TFileError() << 1;
                UNIT_ASSERT(false);
            } catch (const TSystemError&) {
            } catch (...) {
                UNIT_ASSERT(false);
            }

            try {
                ythrow TFileError() << '1';
                UNIT_ASSERT(false);
            } catch (const yexception&) {
            } catch (...) {
                UNIT_ASSERT(false);
            }

            try {
                ythrow TFileError() << 1.0;
                UNIT_ASSERT(false);
            } catch (const TFileError&) {
            } catch (...) {
                UNIT_ASSERT(false);
            }
        }

        inline void TestFormat1() {
            try {
                throw yexception() << 1 << " qw " << 12.1;
                UNIT_ASSERT(false);
            } catch (...) {
                const Stroka err = CurrentExceptionMessage();

                UNIT_ASSERT_VALUES_EQUAL(err, "1 qw 12.1");
            }
        }

        inline void TestRaise1() {
            try {
                Throw2DontMove();
                UNIT_ASSERT(false);
            } catch (...) {
                Stroka err = CurrentExceptionMessage();
                char *ptr = err.begin();
                while ((ptr = strchr(ptr, '\\')) != 0)
                    *ptr = '/';
                UNIT_ASSERT_VALUES_EQUAL(err, "util/generic/yexception_ut.cpp:8: 1 qw 12.1");
            }
        }

        inline void TestException() {
            ythrow yexception() << "blablabla";
        }

        inline void TestLineInfo() {
            try {
                Throw1DontMove();
            } catch (...) {
                UNIT_ASSERT_VALUES_EQUAL(CurrentExceptionMessage(), "yexception_ut.cpp:4: blabla");

                throw;
            }
        }

        //! tests propagation of an exception through C code
        //! @note on some platforms, for example GCC on 32-bit Linux without -fexceptions option,
        //!       throwing an exception from a C++ callback through C code aborts program
        inline void TestMixedCode() {
            const int N = 26082009;
            try {
                TestCallback(&CallbackFun, N);
                UNIT_ASSERT(false);
            } catch (int i) {
                UNIT_ASSERT_VALUES_EQUAL(i, N);
            }
        }
};

UNIT_TEST_SUITE_REGISTRATION(TExceptionTest);
