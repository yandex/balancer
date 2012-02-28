#pragma once

#include <util/system/defaults.h>

#include <cmath>
#include <cstdio>

#include <util/generic/ptr.h>
#include <util/generic/vector.h>
#include <util/generic/pair.h>
#include <util/generic/set.h>
#include <util/string/cast.h>
#include <util/generic/intrlist.h>
#include <util/generic/yexception.h>

namespace NUnitTest {
class ITestSuiteProcessor {
    public:
        struct TUnit {
            const char* name;
        };

        struct TTest {
            const TUnit* unit;
            const char* name;
        };

        struct TError {
            const TTest* test;
            const char* msg;
        };

        struct TSuccess {
            const TTest* test;
        };

        inline ITestSuiteProcessor() throw ()
            : Ok_(0)
            , Err_(0)
            , CurOk_(0)
            , CurErr_(0)
        {
        }

        virtual ~ITestSuiteProcessor() throw () {
        }

        inline void Start() {
            OnStart();
        }

        inline void End() {
            OnEnd();
        }

        inline void UnitStart(const TUnit& unit) {
            CurOk_ = 0;
            CurErr_ = 0;

            OnUnitStart(&unit);
        }

        inline void UnitStop(const TUnit& unit) {
            Ok_ += CurOk_;
            Err_ += CurErr_;

            OnUnitStop(&unit);
        }

        inline void Error(const TError& descr) {
            ++CurErr_;

            OnError(&descr);
        }

        inline void Success(const TSuccess& descr) {
            ++CurOk_;

            OnSuccess(&descr);
        }

        inline unsigned GoodTests() const throw () {
            return Ok_;
        }

        inline unsigned FailTests() const throw () {
            return Err_;
        }

        inline unsigned GoodTestsInCurrentUnit() const throw () {
            return CurOk_;
        }

        inline unsigned FailTestsInCurrentUnit() const throw () {
            return CurErr_;
        }

        virtual bool CheckAccess(const char* /*name*/) {
            return true;
        }

    private:
        virtual void OnStart() {
        }

        virtual void OnEnd() {
        }

        virtual void OnUnitStart(const TUnit* /*unit*/) {
        }

        virtual void OnUnitStop(const TUnit* /*unit*/) {
        }

        virtual void OnError(const TError* /*error*/) {
        }

        virtual void OnSuccess(const TSuccess* /*success*/) {
        }

    private:
        unsigned Ok_;
        unsigned Err_;

        unsigned CurOk_;
        unsigned CurErr_;
};

class TTestBase;
class TTestFactory;

class ITestBaseFactory: public TIntrusiveListItem<ITestBaseFactory> {
    public:
        inline ITestBaseFactory() {
            Register();
        }

        virtual ~ITestBaseFactory() throw () {
        }

        virtual const char* Name() const throw () = 0;
        virtual TTestBase* ConstructTest() = 0;

    private:
        inline void Register() throw ();
};

class TTestBase {
        friend class TTestFactory;
    public:
        inline TTestBase() throw ()
            : Parent_(0)
        {
        }

        virtual ~TTestBase() {
        }

        virtual const char* Name() const throw () = 0;
        virtual void Execute() = 0;

        virtual void SetUp() {
        }

        virtual void TearDown() {
        }

    protected:
        inline void Error(const char* func, const char* msg) {
            const ITestSuiteProcessor::TUnit unit = {Name()};
            const ITestSuiteProcessor::TTest test = {&unit, func};
            const ITestSuiteProcessor::TError err = {&test, msg};

            Processor()->Error(err);
        }

        inline void Success(const char* func) {
            const ITestSuiteProcessor::TUnit unit = {Name()};
            const ITestSuiteProcessor::TTest test = {&unit, func};
            const ITestSuiteProcessor::TSuccess success = {&test};

            Processor()->Success(success);
        }

        inline void AtStart() {
            const ITestSuiteProcessor::TUnit unit = {Name()};

            Processor()->UnitStart(unit);
        }

        inline void AtEnd() {
            const ITestSuiteProcessor::TUnit unit = {Name()};

            Processor()->UnitStop(unit);
        }

        class TCleanUp {
            public:
                inline TCleanUp(TTestBase* base)
                    : Base_(base)
                {
                    Base_->BeforeTest();
                }

                inline ~TCleanUp() {
                    try {
                        Base_->AfterTest();
                    } catch (...) {
                    }
                }

            private:
                TTestBase* Base_;
        };

        inline void BeforeTest() {
            SetUp();
        }

        inline void AfterTest() {
            TearDown();
        }

    private:
        inline ITestSuiteProcessor* Processor() const throw ();

    private:
        TTestFactory* Parent_;
};

#define UNIT_TEST_SUITE(N) \
    public:\
    static const char* StaticName() throw () {\
        return (#N);\
    }\
    private:\
    virtual const char* Name() const throw () {\
        return this->StaticName();\
    }\
    \
    virtual void Execute() {\
        this->AtStart();

#ifndef UT_SKIP_EXCEPTIONS
    #define CATCH_REACTION(FN, e) this->Error(FN, e.what())
#else
    #define CATCH_REACTION(FN, e) throw
#endif

#define UNIT_TEST(F)\
    try {\
        {\
            TCleanUp cleaner(this);\
            this->F();\
        }\
        this->Success((#F));\
    } catch (const std::exception& e) {\
        CATCH_REACTION((#F), e);\
    } catch (...) {\
        this->Error((#F), "non-std exception!");\
    }

#define UNIT_TEST_EXCEPTION(F, E)\
    try {\
        {\
            TCleanUp cleaner(this);\
            this->F();\
        }\
        this->Error((#F), "exception expected");\
    } catch (const E& e) {\
        Stroka err;\
        extern bool CheckExceptionMessage(const char*, Stroka&);\
        if (!CheckExceptionMessage(e.what(), err))\
            this->Error((#F), err.c_str());\
        this->Success((#F));\
    } catch (const std::exception& e) {\
        this->Error((#F), e.what());\
    } catch (...) {\
        this->Error((#F), "non-std exception!");\
    }


#define UNIT_TEST_SUITE_END()\
        this->AtEnd();\
    }\
    public:\
    /*for ; after macros*/ void sub##F()

#define UNIT_FAIL(M)\
    ythrow yexception() << "forced failure(" << (M) << ")"

//doubles
#define UNIT_ASSERT_DOUBLES_EQUAL_C(E, A, D, C)\
    if (::fabs((E) - (A)) > (D)) {\
        ythrow yexception() << "assertion failed(fabs(" << (long double)(E) << " - " << (long double)(A) << ") > " << (long double)(D) << ") " << C; \
    }
#define UNIT_ASSERT_DOUBLES_EQUAL(E, A, D) UNIT_ASSERT_DOUBLES_EQUAL_C(E, A, D, "")

//strings
#define UNIT_ASSERT_STRINGS_EQUAL_C(A, B, C) do {\
        THolder<char, TFree> _a(strdup(A));\
        THolder<char, TFree> _b(strdup(B));\
        if(::strcmp(_a.Get(), _b.Get()) != 0) {\
            ythrow yexception() << "strings equal assertion failed('" << _a.Get() << "' != '" << _b.Get() << "') " << C; \
        }\
    } while (false)
#define UNIT_ASSERT_STRINGS_EQUAL(A, B) UNIT_ASSERT_STRINGS_EQUAL_C(A, B, "")

//bool
#define UNIT_ASSERT_C(A, C) \
    if (!(A)) {\
        ythrow yexception() << "assertion failed(" << (#A) << ") " << C; \
    }
#define UNIT_ASSERT(A) UNIT_ASSERT_C(A, "")

//general
#define UNIT_ASSERT_EQUAL_C(A, B, C) \
    if (!((A) == (B))) {\
        ythrow yexception() << "equal assertion failed(" << (#A) << " == " << (#B) << ") " << C; \
    }
#define UNIT_ASSERT_EQUAL(A, B) UNIT_ASSERT_EQUAL_C(A, B, "")

#define UNIT_ASSERT_UNEQUAL_C(A, B, C) \
    if ((A) == (B)) {\
        ythrow yexception() << "unequal assertion failed(" << (#A) << " != " << (#B) << ") " << C; \
    }
#define UNIT_ASSERT_UNEQUAL(A, B) UNIT_ASSERT_UNEQUAL_C(A, B, "")

#define UNIT_ASSERT_EXCEPTION(A, E)                                     \
    do {                                                                \
        bool thrown = false;                                            \
        try {                                                           \
            A;                                                          \
        } catch (const E&) {                                            \
            thrown = true;                                              \
        }                                                               \
        if (!thrown)                                                    \
            ythrow yexception() << "exception assertion failed(" << (#A) << " doesn't throw " << (#E) << ")"; \
    } while(0)

#define UNIT_ASSERT_NO_EXCEPTION(A)                                     \
    do {                                                                \
        try {                                                           \
            A;                                                          \
        } catch (...) {                                                 \
            ythrow yexception() << "exception-free assertion failed(" << (#A) << " throws)"; \
        }                                                               \
    } while(0)

namespace NPrivate {
    template <typename T, typename U>
    static inline bool CompareEqual(const T& a, const U& b) {
        return a == b;
    }
    static inline bool CompareEqual(const char *a, const char *b) {
        return 0 == strcmp(a, b);
    }
    // helper method to avoid double evaluation of A and B expressions in UNIT_ASSERT_VALUES_EQUAL_C
    template <typename T, typename U>
    static inline bool CompareAndMakeStrings(const T& a, const U& b, Stroka& as, Stroka& bs, bool want) {
        const bool have = CompareEqual(a, b);
        if (want == have)
            return true;
        as = ::ToString(a);
        bs = ::ToString(b);
        return false;
    }
}

//values
#define UNIT_ASSERT_VALUES_EQUAL_IMPL(A, B, C, EQflag, EQstr, NEQstr) \
    do { \
        Stroka as; \
        Stroka bs; \
        if (!::NUnitTest::NPrivate::CompareAndMakeStrings(A, B, as, bs, EQflag)) { \
            ythrow yexception() << "assertion (" << (#A) << " " EQstr " " << (#B) \
                << ") failed('" << as << "' " NEQstr " '" << bs << "') " << C; \
        } \
    } while (0)

#define UNIT_ASSERT_VALUES_EQUAL_C(A, B, C) \
    UNIT_ASSERT_VALUES_EQUAL_IMPL(A, B, C, true, "==", "!=")
#define UNIT_ASSERT_VALUES_UNEQUAL_C(A, B, C) \
    UNIT_ASSERT_VALUES_EQUAL_IMPL(A, B, C, false, "!=", "==")

#define UNIT_ASSERT_VALUES_EQUAL(A, B) UNIT_ASSERT_VALUES_EQUAL_C(A, B, "")
#define UNIT_ASSERT_VALUES_UNEQUAL(A, B) UNIT_ASSERT_VALUES_UNEQUAL_C(A, B, "")


class TTestFactory {
        friend class TTestBase;
        friend class ITestBaseFactory;

        /*
        * by default do nothing
        */
        class TCommonProcessor: public ITestSuiteProcessor {
            public:
                inline TCommonProcessor() throw () {
                }

                virtual ~TCommonProcessor() throw () {
                }
        };

        struct TCmp {
            template <class T>
            inline bool operator() (const T& l, const T& r) const throw () {
                return stricmp(Fix(l.Name()), Fix(r.Name())) < 0;
            }

            static inline const char* Fix(const char* n) throw () {
                if (*n == 'T') {
                    return n + 1;
                }

                return n;
            }
        };

    public:
        static inline TTestFactory& Instance() {
            static TCommonProcessor p;
            static TTestFactory f(&p);

            return f;
        }

        inline unsigned Execute() {
            Items_.QuickSort(TCmp());
            Processor_->Start();

            yset<Stroka> types;
            while (!Items_.Empty()) {
                ITestBaseFactory* factory = Items_.PopFront();

                if (!Processor_->CheckAccess(factory->Name())) {
                    continue;
                }

                THolder<TTestBase> test(factory->ConstructTest());

#ifdef _unix_ // on Windows RTTI causes memory leaks
                Stroka type = typeid(*test.Get()).name();
                if (types.insert(type).second == false) {
                    warnx("Duplicate test found: %s", type.c_str());
                    return 1;
                }
#endif // _unix_

                test->Parent_ = this;

#ifndef UT_SKIP_EXCEPTIONS
                try {
#endif
                    test->Execute();
#ifndef UT_SKIP_EXCEPTIONS
                } catch (...) {
                }
#endif
            }

            Processor_->End();

            return Processor_->FailTests();
        }

        inline void SetProcessor(ITestSuiteProcessor* processor) {
            Processor_ = processor;
        }

    private:
        inline void Register(ITestBaseFactory* b) throw () {
            Items_.PushBack(b);
        }

        inline ITestSuiteProcessor* Processor() const throw () {
            return Processor_;
        }

    private:
        inline TTestFactory(ITestSuiteProcessor* processor)
            : Processor_(processor)
        {
        }

        inline ~TTestFactory() throw () {
        }

    private:
        TIntrusiveList<ITestBaseFactory> Items_;
        ITestSuiteProcessor* Processor_;
};

inline ITestSuiteProcessor* TTestBase::Processor() const throw () {
    return Parent_->Processor();
}

inline void ITestBaseFactory::Register() throw () {
    TTestFactory::Instance().Register(this);
}

template <class T>
class TTestBaseFactory: public ITestBaseFactory {
    public:
        virtual ~TTestBaseFactory() throw () {
        }

        virtual TTestBase* ConstructTest() {
            return new T;
        }

        virtual const char* Name() const throw () {
            return T::StaticName();
        }
};

#define UNIT_TEST_SUITE_REGISTRATION(T)\
    static NUnitTest::TTestBaseFactory< T > JOIN(test, __LINE__);

#define SIMPLE_UNIT_TEST_SUITE(N) \
    namespace NTestSuite ## N { \
        class TCurrentTest: public TTestBase { \
        private: \
            typedef yvector< TPair<const char*, void (*)()> > TTests; \
            \
            static TTests& Tests() { \
                static TTests tests; \
                return tests; \
            } \
            \
        public: \
            static const char* StaticName() { \
                return #N; \
            } \
            virtual const char* Name() const throw() { \
                return StaticName(); \
            } \
            \
            static void AddTest(const char* name, void (*body)()) { \
                Tests().push_back(MakePair(name, body)); \
            } \
            \
            virtual void Execute() { \
                this->AtStart(); \
                for (TTests::iterator i = Tests().begin(), ie = Tests().end(); i != ie; ++i) { \
                    try { \
                        { \
                            TCleanUp cleaner(this); \
                            (i->second)(); \
                        } \
                        this->Success(i->first); \
                    } catch (const std::exception& e) { \
                        CATCH_REACTION(i->first, e); \
                    } catch (...) {\
                        this->Error(i->first, "non-std exception!");\
                    } \
                } \
                this->AtEnd(); \
            } \
        }; \
        UNIT_TEST_SUITE_REGISTRATION(TCurrentTest) \
    } \
    namespace NTestSuite ## N

#define SIMPLE_UNIT_TEST(N) \
    void N(); \
    struct TTestRegistration ## N { \
        TTestRegistration ## N ()\
        { \
            TCurrentTest::AddTest(#N, &N); \
        } \
    }; \
    static TTestRegistration ## N testRegistration ## N; \
    void N()

#define SIMPLE_UNIT_TEST_SUITE_IMPLEMENTATION(N) \
    namespace NTestSuite ## N

#define SIMPLE_UNIT_TEST_DECLARE(N) \
    void N()

#define SIMPLE_UNIT_TEST_FRIEND(N,T) \
    friend void NTestSuite ## N ::T()

Stroka RandomString(size_t len, ui32 seed = 0);
}

using NUnitTest::TTestBase;
