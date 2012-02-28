#include "ios.h"
#include "http.h"
#include "file.h"
#include "zlib.h"
#include "chunk.h"
#include <library/unittest/registar.h>

#include <util/network/socket.h>
#include <util/string/printf.h>

class THttpTest: public TTestBase {
        UNIT_TEST_SUITE(THttpTest);
            UNIT_TEST(TestHttpInput);
            UNIT_TEST(TestMinRequest);
            UNIT_TEST(TestOutputFlush);
            UNIT_TEST(TestOutputPostFlush);
            UNIT_TEST(TestOutputFinish);
            UNIT_TEST(TestMultilineHeaders);
            UNIT_TEST(TestRebuildStreamOnPost);
        UNIT_TEST_SUITE_END();
    public:
        void TestHttpInput();
        void TestMinRequest();
        void TestOutputFlush();
        void TestOutputPostFlush();
        void TestRebuildStreamOnPost();
        void TestOutputFinish();
        void TestMultilineHeaders();
};

UNIT_TEST_SUITE_REGISTRATION(THttpTest);

void THttpTest::TestHttpInput() {
    TNetworkAddress addr("yandex.ru", 80);
    TSocket s(addr);

    //TDebugOutput dbg;
    TNullOutput dbg;

    {
        TSocketOutput so(s);
        TTeeOutput out(&so, &dbg);
        THttpOutput output(&out);

        output.EnableKeepAlive(true);
        output.EnableCompression(true);

        Stroka r;
        r += "GET / HTTP/1.1";
        r += "\r\n";
        r += "Host: yandex.ru";
        r += "\r\n";
        r += "\r\n";

        output.Write(~r, +r);
        output.Finish();
    }

    {
        TSocketInput si(s);
        THttpInput input(&si);
        unsigned http_code = ParseHttpRetCode(input.FirstLine());
        UNIT_ASSERT_VALUES_EQUAL(http_code, 301u);

        TransferData(&input, &dbg);
    }

    // Test low level
    UNIT_ASSERT_VALUES_EQUAL(ParseHttpRetCode("HTTP/1.1 301"), 301u);
}

void THttpTest::TestMinRequest() {
    TNetworkAddress addr("www.yandex.ru", 80);
    TSocket s(addr);
    TNullOutput dbg;

    SendMinimalHttpRequest(s, "www.yandex.ru", "/");

    TSocketInput si(s);
    THttpInput input(&si);
    unsigned http_code = ParseHttpRetCode(input.FirstLine());
    UNIT_ASSERT_VALUES_EQUAL(http_code, 200u);

    TransferData(&input, &dbg);
}

void THttpTest::TestOutputFlush() {
    Stroka str;
    TStringOutput strOut(str);
    TBufferedOutput bufOut(&strOut, 8192);
    THttpOutput httpOut(&bufOut);

    httpOut.EnableKeepAlive(true);
    httpOut.EnableCompression(true);

    const char *header = "GET / HTTP/1.1\r\nHost: yandex.ru\r\n\r\n";
    httpOut << header;

    unsigned curLen = +str;
    const char *body = "<html>Hello</html>";
    httpOut << body;
    UNIT_ASSERT_VALUES_EQUAL(curLen, +str);
    httpOut.Flush();
    UNIT_ASSERT_VALUES_EQUAL(curLen + strlen(body), +str);
}

void THttpTest::TestOutputPostFlush() {
    Stroka str;
    Stroka checkStr;
    TStringOutput strOut(str);
    TStringOutput checkOut(checkStr);
    TBufferedOutput bufOut(&strOut, 8192);
    TTeeOutput teeOut(&bufOut, &checkOut);
    THttpOutput httpOut(&teeOut);

    httpOut.EnableKeepAlive(true);
    httpOut.EnableCompression(true);

    const char *header = "POST / HTTP/1.1\r\nHost: yandex.ru\r\n\r\n";
    httpOut << header;

    UNIT_ASSERT_VALUES_EQUAL(+str, 0);

    const char *body = "<html>Hello</html>";
    httpOut << body;
    UNIT_ASSERT_VALUES_EQUAL(+str, 0);

    httpOut.Flush();
    UNIT_ASSERT_VALUES_EQUAL(+checkStr, +str);
}

class TOutputProxy : public TOutputStream {
    public:
        TOutputProxy(TOutputStream* slave)
            : Slave_(slave)
        {
        }

        void Reset(TOutputStream* slave) {
            Slave_ = slave;
        }

        virtual void DoWrite(const void* buf, size_t len) {
            Slave_->Write(buf, len);
        }

        virtual void DoFlush() {
            Slave_->Flush();
        }

        virtual void DoFinish() {
            Slave_->Finish();
        }

    private:
        TOutputStream* Slave_;
};

void THttpTest::TestRebuildStreamOnPost() {
    Stroka str;
    Stroka bodyStr;
    TStringOutput strOut(str);
    TStringOutput bodyOut(bodyStr);

    {
        TBufferedOutput bufOut(&strOut, 8192);

        TChunkedOutput chunkOut(&bodyOut);
        TZLibCompress comprOut(&chunkOut, ZLib::GZip);

        THttpOutput httpOut(&bufOut);

        httpOut.EnableKeepAlive(true);
        httpOut.EnableCompression(true);

        httpOut << "POST / HTTP/1.1\r\n";
        httpOut << "Host: yandex.ru\r\n";
        httpOut << "Content-Encoding: gzip\r\n";
        httpOut << "\r\n";

        UNIT_ASSERT_VALUES_EQUAL(+str, 0);

        const char *body = "<html>Hello</html>";
        httpOut << body;
        comprOut << body;

        httpOut.Flush();
    }

    UNIT_ASSERT(bodyStr.is_suffix(str));
}

void THttpTest::TestOutputFinish() {
    Stroka str;
    TStringOutput strOut(str);
    TBufferedOutput bufOut(&strOut, 8192);
    THttpOutput httpOut(&bufOut);

    httpOut.EnableKeepAlive(true);
    httpOut.EnableCompression(true);

    const char *header = "GET / HTTP/1.1\r\nHost: yandex.ru\r\n\r\n";
    httpOut << header;

    unsigned curLen = +str;
    const char *body = "<html>Hello</html>";
    httpOut << body;
    UNIT_ASSERT_VALUES_EQUAL(curLen, +str);
    httpOut.Finish();
    UNIT_ASSERT_VALUES_EQUAL(curLen + strlen(body), +str);
}

void THttpTest::TestMultilineHeaders() {
    const char *headerLine0 = "HTTP/1.1 200 OK";
    const char *headerLine1 = "Content-Language: en";
    const char *headerLine2 = "Vary: Accept-Encoding, ";
    const char *headerLine3 = "\tAccept-Language";
    const char *headerLine4 = "Content-Length: 18";

    Stroka endLine("\r\n");
    Stroka r;
    r += headerLine0 + endLine;
    r += headerLine1 + endLine;
    r += headerLine2 + endLine;
    r += headerLine3 + endLine;
    r += headerLine4 + endLine + endLine;
    r += "<html>Hello</html>";
    TStringInput stringInput(r);
    THttpInput input(&stringInput);

    const THttpHeaders& httpHeaders = input.Headers();
    UNIT_ASSERT_VALUES_EQUAL(httpHeaders.Count(), 3u);

    THttpHeaders::TConstIterator it = httpHeaders.Begin();
    UNIT_ASSERT_VALUES_EQUAL(it->ToString(), Stroka(headerLine1));
    UNIT_ASSERT_VALUES_EQUAL((++it)->ToString(), Stroka::Join(headerLine2, headerLine3));
    UNIT_ASSERT_VALUES_EQUAL((++it)->ToString(), Stroka(headerLine4));
}
