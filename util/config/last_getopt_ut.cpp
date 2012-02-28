#include "last_getopt.h"

#include <library/unittest/registar.h>

using namespace NLastGetopt;

yvector<const char*> V(const char* t1) {
    yvector<const char*> v;
    v.push_back(t1);
    return v;
}

yvector<const char*> V(const char* t1, const char* t2) {
    yvector<const char*> v;
    v.push_back(t1);
    v.push_back(t2);
    return v;
}

yvector<const char*> V(const char* t1, const char* t2, const char* t3) {
    yvector<const char*> v;
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    return v;
}

yvector<const char*> V(const char* t1, const char* t2, const char* t3, const char* t4) {
    yvector<const char*> v;
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    v.push_back(t4);
    return v;
}

yvector<const char*> V(const char* t1, const char* t2, const char* t3, const char* t4, const char* t5) {
    yvector<const char*> v;
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    v.push_back(t4);
    v.push_back(t5);
    return v;
}

yvector<const char*> V(const char* t1, const char* t2, const char* t3, const char* t4, const char* t5, const char* t6) {
    yvector<const char*> v;
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    v.push_back(t4);
    v.push_back(t5);
    v.push_back(t6);
    return v;
}

yvector<const char*> V(const char* t1, const char* t2, const char* t3, const char* t4, const char* t5, const char* t6, const char* t7) {
    yvector<const char*> v;
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    v.push_back(t4);
    v.push_back(t5);
    v.push_back(t6);
    v.push_back(t7);
    return v;
}

yvector<const char*> V(const char* t1, const char* t2, const char* t3, const char* t4, const char* t5, const char* t6, const char* t7, const char* t8) {
    yvector<const char*> v;
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    v.push_back(t4);
    v.push_back(t5);
    v.push_back(t6);
    v.push_back(t7);
    v.push_back(t8);
    return v;
}

struct TOptsParserTester {
    TOpts Opts_;
    yvector<const char*> Argv_;

    THolder<TOptsParser> Parser_;

    void Initialize() {
        if (!Parser_)
            Parser_.Reset(new TOptsParser(&Opts_, (int)+Argv_, ~Argv_));
    }

    void Accept() {
        Initialize();
        UNIT_ASSERT(Parser_->Next());
    }

    void AcceptOption() {
        Accept();
        UNIT_ASSERT(!!Parser_->CurOpt());
    }

    void AcceptOption(char c) {
        AcceptOption();
        UNIT_ASSERT(Parser_->CurOpt()->CharIs(c));
    }

    void AcceptOption(const Stroka& optName) {
        AcceptOption();
        UNIT_ASSERT(Parser_->CurOpt()->NameIs(optName));
    }

    template <typename TOpt>
    void AcceptOptionWithValue(TOpt optName, const Stroka& value) {
        AcceptOption(optName);
        UNIT_ASSERT_VALUES_EQUAL_C(value, Parser_->CurValStr(), "; option " << optName);
    }

    template <typename TOpt>
    void AcceptOptionWithoutValue(TOpt optName) {
        AcceptOption(optName);
        UNIT_ASSERT_C(!Parser_->CurVal(), ": opt " << optName << " must have no param");
    }

    void AcceptFreeArgInOrder(const Stroka& expected) {
        Accept();
        UNIT_ASSERT(!Parser_->CurOpt());
        UNIT_ASSERT_VALUES_EQUAL(expected, Parser_->CurValStr());
    }

    size_t Pos_;

    void AcceptEndOfOptions() {
        Initialize();
        UNIT_ASSERT(!Parser_->Next());
        Pos_ = Parser_->Pos_;

        // pos must not be changed after last meaningful invocation of Next()
        UNIT_ASSERT(!Parser_->Next());
        UNIT_ASSERT_VALUES_EQUAL(Pos_, Parser_->Pos_);
        UNIT_ASSERT(!Parser_->Next());
        UNIT_ASSERT_VALUES_EQUAL(Pos_, Parser_->Pos_);
    }

    void AcceptError() {
        Initialize();
        try {
            Parser_->Next();
            UNIT_FAIL("expecting exception");
        } catch (const TUsageException&) {
            // expecting
        }
    }

    void AcceptUnexpectedOption() {
        Initialize();
        size_t pos = Parser_->Pos_;
        size_t sop = Parser_->Sop_;
        AcceptError();
        UNIT_ASSERT_VALUES_EQUAL(pos, Parser_->Pos_);
        UNIT_ASSERT_VALUES_EQUAL(sop, Parser_->Sop_);
    }

    void AcceptFreeArg(const Stroka& expected) {
        UNIT_ASSERT(Pos_ < Parser_->Argc_);
        UNIT_ASSERT_VALUES_EQUAL(expected, Parser_->Argv_[Pos_]);
        ++Pos_;
    }

    void AcceptEndOfFreeArgs() {
        UNIT_ASSERT_VALUES_EQUAL(Argv_.size(), Pos_);
    }
};

SIMPLE_UNIT_TEST_SUITE(TLastGetoptTests) {
    SIMPLE_UNIT_TEST(TestEqual) {
        TOpts opts;
        opts.AddLongOption("from");
        opts.AddLongOption("to");
        TOptsParseResultException r(&opts, V("copy", "--from=/", "--to=/etc"));

        UNIT_ASSERT_VALUES_EQUAL("copy", r.GetProgramName());
        UNIT_ASSERT_VALUES_EQUAL("/", r.Get("from"));
        UNIT_ASSERT_VALUES_EQUAL("/etc", r.Get("to"));
        UNIT_ASSERT_VALUES_EQUAL("/etc", r.GetOrElse("to", "trash"));
        UNIT_ASSERT(r.Has("from"));
        UNIT_ASSERT(r.Has("to"));
        try {
            r.Get("left");
            UNIT_FAIL("expecting exception");
        } catch (const TException&) {
            // expected
        }
    }

    SIMPLE_UNIT_TEST(TestCharOptions) {
        TOpts opts;
        opts.AddCharOption('R', NO_ARGUMENT);
        opts.AddCharOption('l', NO_ARGUMENT);
        opts.AddCharOption('h', NO_ARGUMENT);
        TOptsParseResultException r(&opts, V("cp", "/etc", "-Rl", "/tmp/etc"));
        UNIT_ASSERT(r.Has('R'));
        UNIT_ASSERT(r.Has('l'));
        UNIT_ASSERT(!r.Has('h'));

        UNIT_ASSERT_VALUES_EQUAL(2u, r.GetFreeArgs().size());
        UNIT_ASSERT_VALUES_EQUAL("/etc", r.GetFreeArgs()[0]);
        UNIT_ASSERT_VALUES_EQUAL("/tmp/etc", r.GetFreeArgs()[1]);
    }

    SIMPLE_UNIT_TEST(TestCharOptionsRequiredOptional) {
        TOpts opts;
        opts.AddCharOption('d', REQUIRED_ARGUMENT);
        opts.AddCharOption('e', REQUIRED_ARGUMENT);
        opts.AddCharOption('x', REQUIRED_ARGUMENT);
        opts.AddCharOption('y', REQUIRED_ARGUMENT);
        opts.AddCharOption('l', NO_ARGUMENT);
        TOptsParseResultException r(&opts, V("cmd", "-ld11", "-e", "22", "-lllx33", "-y", "44"));
        UNIT_ASSERT_VALUES_EQUAL("11", r.Get('d'));
        UNIT_ASSERT_VALUES_EQUAL("22", r.Get('e'));
        UNIT_ASSERT_VALUES_EQUAL("33", r.Get('x'));
        UNIT_ASSERT_VALUES_EQUAL("44", r.Get('y'));
    }

    SIMPLE_UNIT_TEST(TestReturnInOrder) {
        TOptsParserTester tester;
        tester.Opts_.AddLongOption('v', "value");
        tester.Opts_.ArgPermutation_ = RETURN_IN_ORDER;

        tester.Argv_.push_back("cmd");
        tester.Argv_.push_back("--value=11");
        tester.Argv_.push_back("xx");
        tester.Argv_.push_back("-v12");
        tester.Argv_.push_back("yy");
        tester.Argv_.push_back("--");
        tester.Argv_.push_back("-v13");
        tester.Argv_.push_back("--");

        tester.AcceptOptionWithValue("value", "11");
        tester.AcceptFreeArgInOrder("xx");
        tester.AcceptOptionWithValue('v', "12");
        tester.AcceptFreeArgInOrder("yy");
        tester.AcceptFreeArgInOrder("-v13");
        tester.AcceptFreeArgInOrder("--");
        tester.AcceptEndOfOptions();
        tester.AcceptEndOfFreeArgs();
    }

    SIMPLE_UNIT_TEST(TestRequireOrder) {
        TOptsParserTester tester;
        tester.Opts_.ArgPermutation_ = REQUIRE_ORDER;
        tester.Opts_.AddLongOption('v', "value");

        tester.Argv_.push_back("cmd");
        tester.Argv_.push_back("--value=11");
        tester.Argv_.push_back("xx");
        tester.Argv_.push_back("-v12");
        tester.Argv_.push_back("yy");

        tester.AcceptOptionWithValue("value", "11");
        tester.AcceptEndOfOptions();

        tester.AcceptFreeArg("xx");
        tester.AcceptFreeArg("-v12");
        tester.AcceptFreeArg("yy");
        tester.AcceptEndOfFreeArgs();
    }

    SIMPLE_UNIT_TEST(TestPlusForLongOption) {
        TOptsParserTester tester;
        tester.Opts_.AddLongOption('v', "value");
        tester.Opts_.AllowPlusForLong_ = true;
        tester.Argv_.push_back("cmd");
        tester.Argv_.push_back("+value=11");
        tester.Argv_.push_back("xx");
        tester.Argv_.push_back("-v12");
        tester.Argv_.push_back("yy");

        tester.AcceptOptionWithValue("value", "11");
        tester.AcceptOptionWithValue("value", "12");
        tester.AcceptEndOfOptions();

        tester.AcceptFreeArg("xx");
        tester.AcceptFreeArg("yy");
        tester.AcceptEndOfFreeArgs();
    }

    SIMPLE_UNIT_TEST(TestBug1) {
        TOptsParserTester tester;
        tester.Opts_.AddCharOptions("A:b:cd:");

        tester.Argv_.push_back("cmd");
        tester.Argv_.push_back("-A");
        tester.Argv_.push_back("aaaa");
        tester.Argv_.push_back("zz");
        tester.Argv_.push_back("-c");
        tester.Argv_.push_back("-d8");
        tester.Argv_.push_back("ww");

        tester.AcceptOptionWithValue('A', "aaaa");
        tester.AcceptOptionWithoutValue('c');
        tester.AcceptOptionWithValue('d', "8");
        tester.AcceptEndOfOptions();

        tester.AcceptFreeArg("zz");
        tester.AcceptFreeArg("ww");
        tester.AcceptEndOfFreeArgs();
    }

    SIMPLE_UNIT_TEST(TestPermuteComplex) {
        TOptsParserTester tester;

        tester.Opts_.AddCharOption('x').NoArgument();
        tester.Opts_.AddCharOption('y').RequiredArgument();
        tester.Opts_.AddCharOption('z').NoArgument();
        tester.Opts_.AddCharOption('w').RequiredArgument();
        tester.Opts_.ArgPermutation_ = PERMUTE;

        tester.Argv_.push_back("cmd");
        tester.Argv_.push_back("-x");
        tester.Argv_.push_back("-y");
        tester.Argv_.push_back("val");
        tester.Argv_.push_back("freearg1");
        tester.Argv_.push_back("-zw");
        tester.Argv_.push_back("val2");
        tester.Argv_.push_back("freearg2");

        tester.AcceptOptionWithoutValue('x');
        tester.AcceptOptionWithValue('y', "val");
        tester.AcceptOptionWithoutValue('z');
        tester.AcceptOptionWithValue('w', "val2");
        tester.AcceptEndOfOptions();
        tester.AcceptFreeArg("freearg1");
        tester.AcceptFreeArg("freearg2");
        tester.AcceptEndOfFreeArgs();
    }

    SIMPLE_UNIT_TEST(TestFinalDashDash) {
        TOptsParserTester tester;
        tester.Opts_.AddLongOption("size");

        tester.Argv_.push_back("cmd");
        tester.Argv_.push_back("--");

        tester.AcceptEndOfOptions();
        tester.AcceptEndOfFreeArgs();
    }

    SIMPLE_UNIT_TEST(TestDashDashAfterDashDash) {
        TOptsParserTester tester;
        tester.Opts_.AddLongOption("size");

        tester.Argv_.push_back("cmd");
        tester.Argv_.push_back("--");
        tester.Argv_.push_back("--");
        tester.Argv_.push_back("--");

        tester.AcceptEndOfOptions();
        tester.AcceptFreeArg("--");
        tester.AcceptFreeArg("--");
        tester.AcceptEndOfFreeArgs();
    }

    SIMPLE_UNIT_TEST(TestUnexpectedUnknownOption) {
        TOptsParserTester tester;

        tester.Argv_.push_back("cmd");
        tester.Argv_.push_back("-x");

        tester.AcceptUnexpectedOption();
    }

    SIMPLE_UNIT_TEST(TestPositionWhenNoArgs) {
        TOptsParserTester tester;

        tester.Argv_.push_back("cmd");

        tester.Opts_.AddCharOption('c');

        tester.AcceptEndOfOptions();

        UNIT_ASSERT_VALUES_EQUAL(1u, tester.Parser_->Pos_);
    }

    SIMPLE_UNIT_TEST(TestExpectedUnknownCharOption) {
        TOptsParserTester tester;

        tester.Argv_.push_back("cmd");
        tester.Argv_.push_back("-x");
        tester.Argv_.push_back("-y");
        tester.Argv_.push_back("val");
        tester.Argv_.push_back("freearg1");
        tester.Argv_.push_back("-zw");
        tester.Argv_.push_back("val2");
        tester.Argv_.push_back("freearg2");

        tester.Opts_.AllowUnknownCharOptions_ = true;

        tester.AcceptOptionWithoutValue('x');
        tester.AcceptOptionWithValue('y', "val");
        tester.AcceptOptionWithoutValue('z');
        tester.AcceptOptionWithValue('w', "val2");
        tester.AcceptEndOfOptions();
        tester.AcceptFreeArg("freearg1");
        tester.AcceptFreeArg("freearg2");
        tester.AcceptEndOfFreeArgs();
    }

#if 0
    SIMPLE_UNIT_TEST(TestRequiredParams) {
        TOptsParserTester tester;

        tester.Argv_.push_back("cmd");
        tester.Argv_.push_back("--port=1231");
        tester.Argv_.push_back("asas");

        tester.Opts_.AddLongOption("port");
        tester.Opts_.AddLongOption("home").Required();

        tester.AcceptOptionWithValue("port", "1231");
        tester.AcceptError();
    }
#endif

    SIMPLE_UNIT_TEST(TestStoreResult) {
        TOpts opts;
        Stroka data;
        int number;
        opts.AddLongOption('d', "data").StoreResult(&data);
        opts.AddLongOption('n', "number").StoreResult(&number);
        TOptsParseResultException r(&opts, V("cmd", "--data=jjhh", "-n", "11"));
        UNIT_ASSERT_VALUES_EQUAL("jjhh", data);
        UNIT_ASSERT_VALUES_EQUAL(11, number);
    }

    SIMPLE_UNIT_TEST(TestStoreValue) {
        int a = 0, b = 0;
        size_t c = 0;
        EHasArg e = NO_ARGUMENT;

        TOpts opts;
        opts.AddLongOption('a', "alpha").NoArgument().StoreValue(&a, 42);
        opts.AddLongOption('b', "beta").NoArgument().StoreValue(&b, 24);
        opts.AddLongOption('e', "enum").NoArgument().StoreValue(&e, REQUIRED_ARGUMENT).StoreValue(&c, 12345);

        TOptsParseResultException r(&opts, V("cmd", "-a", "-e"));

        UNIT_ASSERT_VALUES_EQUAL(42, a);
        UNIT_ASSERT_VALUES_EQUAL(0, b);
        UNIT_ASSERT(e == REQUIRED_ARGUMENT);
        UNIT_ASSERT_VALUES_EQUAL(12345u, c);
    }

    SIMPLE_UNIT_TEST(TestDefaultValue) {
        TOpts opts;
        opts.AddLongOption("path").DefaultValue("/etc");
        TOptsParseResultException r(&opts, V("cmd", "dfdf"));
        UNIT_ASSERT_VALUES_EQUAL("/etc", r.Get("path"));
    }

    SIMPLE_UNIT_TEST(TestSplitValue) {
        TOpts opts;
        yvector<Stroka> vals;
        opts.AddLongOption('s', "split")
            .Handler(new TOptSplitHandler<Stroka>(&vals, ','));
        TOptsParseResultException r(&opts, V("prog", "--split=a,b,c"));
        UNIT_ASSERT_EQUAL(vals.ysize(), 3);
        UNIT_ASSERT_EQUAL(vals[0], "a");
        UNIT_ASSERT_EQUAL(vals[1], "b");
        UNIT_ASSERT_EQUAL(vals[2], "c");
    }

    SIMPLE_UNIT_TEST(TestParseArgs) {
        TOpts o("AbCx:y:z::");
        UNIT_ASSERT_EQUAL(o.GetCharOption('A').HasArg_, NO_ARGUMENT);
        UNIT_ASSERT_EQUAL(o.GetCharOption('b').HasArg_, NO_ARGUMENT);
        UNIT_ASSERT_EQUAL(o.GetCharOption('C').HasArg_, NO_ARGUMENT);
        UNIT_ASSERT_EQUAL(o.GetCharOption('x').HasArg_, REQUIRED_ARGUMENT);
        UNIT_ASSERT_EQUAL(o.GetCharOption('y').HasArg_, REQUIRED_ARGUMENT);
        UNIT_ASSERT_EQUAL(o.GetCharOption('z').HasArg_, OPTIONAL_ARGUMENT);
    }

    SIMPLE_UNIT_TEST(TestRequiredOpts) {
        TOpts opts;
        TOpt& opt_d = opts.AddCharOption('d');

        // test 'not required'
        // makes sure that the problem will only be in 'required'
        TOptsParseResultException r1(&opts, V("cmd"));

        // test 'required'
        opt_d.Required();
        bool ok = false;
        try {
            TOptsParseResultException r2(&opts, V("cmd"));
        } catch (const TUsageException & /*exc*/) {
            ok = true;
        }
        UNIT_ASSERT_C(ok, "required option wasn't detected");

        TOptsParseResultException r3(&opts, V("cmd", "-d11"));
        UNIT_ASSERT_VALUES_EQUAL("11", r3.Get('d'));
    }

    class HandlerStoreTrue
    {
        bool *Flag;
    public:
        HandlerStoreTrue(bool *flag)
            : Flag(flag)
        {}
        void operator()()
        {
            *Flag = true;
        }
    };
    SIMPLE_UNIT_TEST(TestHandlers) {
        TOpts opts;
        bool flag = false;
        opts.AddLongOption("flag").Handler0(HandlerStoreTrue(&flag)).NoArgument();
        TOptsParseResultException r(&opts, V("cmd", "--flag"));
        UNIT_ASSERT(flag);
    }

}
