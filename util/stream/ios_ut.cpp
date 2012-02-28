#include "ios.h"
#include "glue.h"
#include "tokenizer.h"

#include <string>
#include <iostream>

#include <library/unittest/registar.h>

#include <util/string/cast.h>
#include <util/memory/tempbuf.h>
#include <util/charset/recyr.hh>

class TStreamsTest: public TTestBase {
        UNIT_TEST_SUITE(TStreamsTest);
            UNIT_TEST(TestGenericRead);
            UNIT_TEST(TestGenericWrite);
            UNIT_TEST(TestReadLine);
            UNIT_TEST(TestMemoryStream);
            UNIT_TEST(TestStreamBufAdaptor);
            UNIT_TEST(TestBufferedIO);
            UNIT_TEST(TestBufferStream);
            UNIT_TEST(TestTokenizer);
            UNIT_TEST(TestStringStream);
            UNIT_TEST(TestWtrokaInput);
            UNIT_TEST(TestWtrokaOutput);
            UNIT_TEST(TestIStreamOperators);
        UNIT_TEST_SUITE_END();
    public:
        void TestGenericRead();
        void TestGenericWrite();
        void TestReadLine();
        void TestMemoryStream();
        void TestStreamBufAdaptor();
        void TestBufferedIO();
        void TestBufferStream();
        void TestTokenizer();
        void TestStringStream();
        void TestWtrokaInput();
        void TestWtrokaOutput();
        void TestIStreamOperators();
};

UNIT_TEST_SUITE_REGISTRATION(TStreamsTest);

void TStreamsTest::TestIStreamOperators()
{
    Stroka data ("first line\r\nsecond\t\xd1\x82\xd0\xb5\xd1\x81\xd1\x82 line\r\n 1 -4 59 4320000000009999999 c\n -1.5 1e-110");
    TStringInput si(data);

    Stroka l1;
    Stroka l2;
    Stroka l3;
    Wtroka w1;
    Stroka l4;
    ui16 i1;
    i16 i2;
    i32 i3;
    ui64 i4;
    char c1;
    unsigned char c2;
    float f1;
    double f2;

    si >> l1 >> l2 >> l3 >> w1 >> l4 >> i1 >> i2 >> i3 >> i4 >> c1 >> c2 >> f1 >> f2;

    UNIT_ASSERT_EQUAL(l1, "first");
    UNIT_ASSERT_EQUAL(l2, "line");
    {
        char buf[128];
        size_t in_readed = 0;
        size_t out_writed = 0;
        RecodeFromUnicode(CODES_WIN, w1.c_str(), buf, w1.size(), sizeof(buf) - 1, in_readed, out_writed);
        buf[out_writed] = 0;
        UNIT_ASSERT_EQUAL(strcmp(buf, "\xf2\xe5\xf1\xf2"), 0);
    }
    UNIT_ASSERT_EQUAL(l3, "second");
    UNIT_ASSERT_EQUAL(l4, "line");
    UNIT_ASSERT_EQUAL(i1, 1);
    UNIT_ASSERT_EQUAL(i2, -4);
    UNIT_ASSERT_EQUAL(i3, 59);
    UNIT_ASSERT_EQUAL(i4, 4320000000009999999ULL);
    UNIT_ASSERT_EQUAL(c1, 'c');
    UNIT_ASSERT_EQUAL(c2, '\n');
    UNIT_ASSERT_EQUAL(f1, -1.5);
    UNIT_ASSERT_EQUAL(f2, 1e-110);
}

void TStreamsTest::TestStringStream() {
    TStringStream s;

    s << "qw\r\n1234" << "\n" << 34;

    UNIT_ASSERT_EQUAL(s.ReadLine(), "qw");
    UNIT_ASSERT_EQUAL(s.ReadLine(), "1234");

    s << "\r\n" << 123.1;

    UNIT_ASSERT_EQUAL(s.ReadLine(), "34");
    UNIT_ASSERT_EQUAL(s.ReadLine(), "123.1");

    UNIT_ASSERT_EQUAL(s.Str(), "qw\r\n1234\n34\r\n123.1");
}

void TStreamsTest::TestGenericRead() {
    Stroka s("1234567890");
    TStringInput si(s);
    char buf[1024];

    UNIT_ASSERT_EQUAL(si.Read(buf, 6), 6);
    UNIT_ASSERT_EQUAL(memcmp(buf, "123456", 6), 0);
    UNIT_ASSERT_EQUAL(si.Read(buf, 6), 4);
    UNIT_ASSERT_EQUAL(memcmp(buf, "7890", 4), 0);
}

void TStreamsTest::TestGenericWrite() {
    Stroka s;
    TStringOutput so(s);

    so.Write("123456", 6);
    so.Write("7890", 4);

    UNIT_ASSERT_EQUAL(s, "1234567890");
}

void TStreamsTest::TestReadLine() {
    Stroka data("1234\r\n5678\nqw");
    TStringInput si(data);

    UNIT_ASSERT_EQUAL(si.ReadLine(), "1234");
    UNIT_ASSERT_EQUAL(si.ReadLine(), "5678");
    UNIT_ASSERT_EQUAL(si.ReadLine(), "qw");
}

void TStreamsTest::TestMemoryStream() {
    char buf[1024];
    TMemoryOutput mo(buf, sizeof(buf));
    bool ehandled = false;

    try {
        for (size_t i = 0; i < sizeof(buf) + 1; ++i) {
            mo.Write(i % 127);
        }
    } catch (...) {
        ehandled = true;
    }

    UNIT_ASSERT_EQUAL(ehandled, true);

    for (size_t i = 0; i < sizeof(buf); ++i) {
        UNIT_ASSERT_EQUAL(buf[i], (char)(i % 127));
    }
}

class TIO: public std::iostream {
    public:
        inline TIO(TStreamBufAdaptor* buf)
            : std::iostream(0)
        {
            std::istream::init(buf);
            std::ostream::init(buf);
        }
};

void TStreamsTest::TestStreamBufAdaptor() {
    Stroka in("1234\r\n123456\r\n");
    Stroka out;

    TStringInput si(in);
    TStringOutput so(out);

    {
        TStreamBufAdaptor strbuf(&si, 1024, &so, 1024);
        TIO io(&strbuf);

        std::string st;
        io >> st;
        UNIT_ASSERT_EQUAL(st, "1234");
        io >> st;
        UNIT_ASSERT_EQUAL(st, "123456");
        io << "qwerty";
        io.flush();
        io << "123456";
    }

    UNIT_ASSERT_EQUAL(out, "qwerty123456");
}

class TMyStringOutput: public TOutputStream {
    public:
        inline TMyStringOutput(Stroka& s, size_t buflen) throw ()
            : S_(s)
            , BufLen_(buflen)
        {
        }

        virtual ~TMyStringOutput() throw () {
        }

        virtual void DoWrite(const void* data, size_t len) {
            S_.Write(data, len);
            UNIT_ASSERT(len < BufLen_ || ((len % BufLen_) == 0));
        }

        virtual void DoWriteV(const TPart* p, size_t count) {
            Stroka s;

            for (size_t i = 0; i < count; ++i) {
                s.append((const char*)p[i].buf, p[i].len);
            }

            DoWrite(~s, +s);
        }

    private:
        TStringOutput S_;
        const size_t BufLen_;
};

void TStreamsTest::TestBufferedIO() {
    Stroka s;

    {
        const size_t buflen = 7;
        TBuffered<TMyStringOutput> bo(buflen, s, buflen);

        for (size_t i = 0; i < 1000; ++i) {
            Stroka str(" ");
            str += ToString(i % 10);

            bo.Write(~str, +str);
        }

        bo.Finish();
    }

    UNIT_ASSERT_EQUAL(+s, 2000);

    {
        const size_t buflen = 11;
        TBuffered<TStringInput> bi(buflen, s);

        for (size_t i = 0; i < 1000; ++i) {
            Stroka str(" ");
            str += ToString(i % 10);

            char buf[3];

            UNIT_ASSERT_EQUAL(bi.Load(buf, 2), 2);

            buf[2] = 0;

            UNIT_ASSERT_EQUAL(str, buf);
        }
    }

    s.clear();

    {
        const size_t buflen = 13;
        TBuffered<TMyStringOutput> bo(buflen, s, buflen);
        Stroka f = "1234567890";

        for (size_t i = 0; i < 10; ++i) {
            f += f;
        }

        for (size_t i = 0; i < 1000; ++i) {
            bo.Write(~f, i);
        }

        bo.Finish();
    }
}

void TStreamsTest::TestBufferStream() {
    TBufferStream stream;
    Stroka s = "test";

    stream.Write(~s, +s);
    char buf[5];
    size_t readed = stream.Read(buf, 4);
    UNIT_ASSERT_EQUAL(4, readed);
    UNIT_ASSERT_EQUAL(0, strncmp(~s, buf, 4));

    stream.Write(~s, +s);
    readed = stream.Read(buf, 2);
    UNIT_ASSERT_EQUAL(2, readed);
    UNIT_ASSERT_EQUAL(0, strncmp("te", buf, 2));

    readed = stream.Read(buf, 2);
    UNIT_ASSERT_EQUAL(2, readed);
    UNIT_ASSERT_EQUAL(0, strncmp("st", buf, 2));

    readed = stream.Read(buf, 2);
    UNIT_ASSERT_EQUAL(0, readed);
}

struct TTestEof {
    inline bool operator() (char ch) const throw () {
        return ch == '\n';
    }
};

void TStreamsTest::TestTokenizer() {
    const Stroka parts[] = {
        "qwerty",
        "1234567890"
    };

    const size_t count = sizeof(parts) / sizeof(*parts);

    Stroka s;

    for (size_t i = 0; i < count; ++i) {
        s += parts[i];
        s += "\n";
    }

    TMemoryInput mi(~s, +s);
    typedef TStreamTokenizer<TTestEof> TTokenizer;
    TTokenizer tokenizer(&mi);
    size_t cur = 0;

    for (TTokenizer::TIterator it = tokenizer.Begin(); it != tokenizer.End(); ++it) {
        UNIT_ASSERT(cur < count);
        UNIT_ASSERT_EQUAL(Stroka(it->Data(), it->Length()), parts[cur]);

        ++cur;
    }
}

#if defined (_MSC_VER)
#   pragma warning(disable:4309) /*truncation of constant value*/
#endif

namespace {

    const char Text[] = {
        // UTF8 encoded "one \ntwo\r\nthree\n\tfour\nfive\n" in russian and ...
        0xD1, 0x80, 0xD0, 0xB0, 0xD0, 0xB7, ' ', '\n',
        0xD0, 0xB4, 0xD0, 0xB2, 0xD0, 0xB0, '\r', '\n',
        0xD1, 0x82, 0xD1, 0x80, 0xD0, 0xB8, '\n',
        '\t', 0xD1, 0x87, 0xD0, 0xB5, 0xD1, 0x82, 0xD1, 0x8B, 0xD1, 0x80, 0xD0, 0xB5, '\n',
        0xD0, 0xBF, 0xD1, 0x8F, 0xD1, 0x82, 0xD1, 0x8C, '\n',
        // ... additional test cases
        '\r', '\n',
        '\n', '\r', // this char goes to the front of the next string
        'o', 'n', 'e', ' ', 't', 'w', 'o', '\n',
        '1', '2', '3', '\r', '\n',
        '\t', '\r', ' ', 0 };

    const char Expected[][20] = {
        // UTF8 encoded "one ", "two", "three", "\tfour", "five" in russian and ...
        { 0xD1, 0x80, 0xD0, 0xB0, 0xD0, 0xB7, 0x20, 0x00 },
        { 0xD0, 0xB4, 0xD0, 0xB2, 0xD0, 0xB0, 0x00 },
        { 0xD1, 0x82, 0xD1, 0x80, 0xD0, 0xB8, 0x00 },
        { 0x09, 0xD1, 0x87, 0xD0, 0xB5, 0xD1, 0x82, 0xD1, 0x8B, 0xD1, 0x80, 0xD0, 0xB5, 0x00 },
        { 0xD0, 0xBF, 0xD1, 0x8F, 0xD1, 0x82, 0xD1, 0x8C, 0x00 },
        // ... additional test cases
        { 0x00 },
        { 0x00 },
        { '\r', 'o', 'n', 'e', ' ', 't', 'w', 'o' },
        { '1', '2', '3' },
        { '\t', '\r', ' ' } };

}

void TStreamsTest::TestWtrokaInput() {

    TTempBuf buffer(sizeof(Text) * sizeof(wchar16));
    wchar16* const data = (wchar16*)buffer.Data();

    const Stroka s(Text);
    TStringInput is(s);
    Wtroka w;

    size_t i = 0;
    while (is.ReadLine(w)) {
        UNIT_ASSERT(i < sizeof(Expected) / sizeof(Expected[0]));

        size_t read = 0, written = 0;
        RecodeToUnicode(CODES_UTF8, Expected[i], data, strlen(Expected[i]), sizeof(Text) - 1, read, written);
        data[written] = 0;

        UNIT_ASSERT(w == data);
        ++i;
    }
}

void TStreamsTest::TestWtrokaOutput() {
    Stroka s;
    TStringOutput os(s);
    const size_t n = sizeof(Expected) / sizeof(Expected[0]);

    for (size_t i = 0; i < n; ++i) {
        const size_t len = strlen(Expected[i]);
        Wtroka w((int)len);
        size_t read = 0, written = 0;
        RecodeToUnicode(CODES_UTF8, Expected[i], w.begin(), len, len, read, written);
        w.remove(written);

        os << w;
        if (i == 1 || i == 5 || i == 8)
            os << '\r';
        if (i < n - 1)
            os << '\n';
    }

    UNIT_ASSERT(s == Text);
}
