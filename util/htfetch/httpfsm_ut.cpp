#include "httpfsm.h"

#include <library/unittest/registar.h>

class THttpHeaderParserTestSuite: public TTestBase {
    UNIT_TEST_SUITE(THttpHeaderParserTestSuite);
        UNIT_TEST(TestRequestHeader);
        UNIT_TEST(TestProxyRequestHeader);
        UNIT_TEST(TestIncorrectRequestHeader);
        UNIT_TEST(TestLastModified);
        UNIT_TEST(TestLastModifiedCorrupted);
        UNIT_TEST(TestResponseHeaderOnRequest);
        UNIT_TEST(TestRequestHeaderOnResponse);
    UNIT_TEST_SUITE_END();
private:
    THttpHeaderParser *httpHeaderParser;
private:
    void TestStart();
    void TestFinish();
public:
    void TestRequestHeader();
    void TestProxyRequestHeader();
    void TestIncorrectRequestHeader();
    void TestLastModified();
    void TestLastModifiedCorrupted();
    void TestResponseHeaderOnRequest();
    void TestRequestHeaderOnResponse();
};

void THttpHeaderParserTestSuite::TestStart() {
    httpHeaderParser = new THttpHeaderParser();
}

void THttpHeaderParserTestSuite::TestFinish() {
    delete httpHeaderParser;
}

void THttpHeaderParserTestSuite::TestRequestHeader() {
    TestStart();
    THttpRequestHeader httpRequestHeader;
    httpHeaderParser->Init(&httpRequestHeader);
    const char *request = "GET /search?q=hi HTTP/1.1\r\n"
        "Host: www.google.ru:8080\r\n\r\n";
    i32 result = httpHeaderParser->Execute(request, strlen(request));
    UNIT_ASSERT_EQUAL(result, 2);
    UNIT_ASSERT_EQUAL(httpRequestHeader.http_method, HTTP_METHOD_GET);
    UNIT_ASSERT_EQUAL(strcmp(httpRequestHeader.host, "www.google.ru:8080"), 0);
    UNIT_ASSERT_EQUAL(strcmp(httpRequestHeader.request_uri, "/search?q=hi"), 0);
    UNIT_ASSERT(httpRequestHeader.GetUrl() ==
            "http://www.google.ru:8080/search?q=hi");
    UNIT_ASSERT_EQUAL(httpHeaderParser->lastchar - request + 1,
            (i32) strlen(request));
    UNIT_ASSERT_EQUAL(httpRequestHeader.x_yandex_response_timeout,
            DEFAULT_RESPONSE_TIMEOUT);
    UNIT_ASSERT_EQUAL(httpRequestHeader.x_yandex_request_priority,
            DEFAULT_REQUEST_PRIORITY);
    UNIT_ASSERT_EQUAL(strcmp(httpRequestHeader.x_yandex_sourcename, ""), 0);
    TestFinish();
    UNIT_ASSERT_EQUAL(httpRequestHeader.max_age, DEFAULT_MAX_AGE);
}

void THttpHeaderParserTestSuite::TestProxyRequestHeader() {
    TestStart();
    THttpRequestHeader httpRequestHeader;
    httpHeaderParser->Init(&httpRequestHeader);
    const char *request =
        "GET http://www.google.ru:8080/search?q=hi HTTP/1.1\r\n"
        "X-Yandex-Response-Timeout: 1000\r\n"
        "X-Yandex-Request-Priority: 2\r\n"
        "X-Yandex-Sourcename: orange\r\n"
        "Cache-control: max-age=100\r\n"
        "If-Modified-Since: Sat, 29 Oct 1994 19:43:31 GMT\r\n"
        "User-Agent: Yandex/1.01.001 (compatible; Win16; I)\r\n"
        "From: webadmin@yandex.ru\r\n\r\n";
    i32 result = httpHeaderParser->Execute(request, strlen(request));
    UNIT_ASSERT_EQUAL(result, 2);
    UNIT_ASSERT_EQUAL(httpRequestHeader.http_method, HTTP_METHOD_GET);
    UNIT_ASSERT_EQUAL(httpRequestHeader.x_yandex_response_timeout, 1000);
    UNIT_ASSERT_EQUAL(httpRequestHeader.x_yandex_request_priority, 2);
    UNIT_ASSERT_EQUAL(strcmp(httpRequestHeader.x_yandex_sourcename, "orange"),
            0);
    UNIT_ASSERT_EQUAL(httpRequestHeader.max_age, 100);
    UNIT_ASSERT_VALUES_EQUAL(httpRequestHeader.if_modified_since,
            TInstant::ParseIso8601("1994-10-29 19:43:31Z").TimeT());
    UNIT_ASSERT_EQUAL(
            strcmp(httpRequestHeader.request_uri,
                "http://www.google.ru:8080/search?q=hi"), 0);
    UNIT_ASSERT(httpRequestHeader.GetUrl() ==
            "http://www.google.ru:8080/search?q=hi");
    UNIT_ASSERT_EQUAL(strcmp(httpRequestHeader.host, ""), 0);
    UNIT_ASSERT_EQUAL(strcmp(httpRequestHeader.from, "webadmin@yandex.ru"), 0);
    UNIT_ASSERT_EQUAL(strcmp(httpRequestHeader.user_agent,
                "Yandex/1.01.001 (compatible; Win16; I)"), 0);
    UNIT_ASSERT_EQUAL(httpHeaderParser->lastchar - request + 1,
            (i32) strlen(request));
    TestFinish();
}

void THttpHeaderParserTestSuite::TestIncorrectRequestHeader() {
    TestStart();
    THttpRequestHeader httpRequestHeader;
    httpHeaderParser->Init(&httpRequestHeader);
    const char *request = "GET /search?q=hi HTP/1.1\r\n"
        "Host: www.google.ru:8080\r\n\r\n";
    i32 result = httpHeaderParser->Execute(request, strlen(request));
    UNIT_ASSERT(result != 2);
    TestFinish();
}

void THttpHeaderParserTestSuite::TestLastModified() {
    TestStart();
    THttpHeader h;
    UNIT_ASSERT(0 == httpHeaderParser->Init(&h));
    const char* headers =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Last-Modified: Thu, 13 Aug 2009 14:27:08 GMT\r\n\r\n";
    UNIT_ASSERT(2 == httpHeaderParser->Execute(headers, strlen(headers)));
    UNIT_ASSERT_VALUES_EQUAL(
            TInstant::ParseIso8601("2009-08-13 14:27:08Z").TimeT(),
            h.http_time);
    TestFinish();
}

void THttpHeaderParserTestSuite::TestLastModifiedCorrupted() {
    TestStart();
    THttpHeader h;
    UNIT_ASSERT(0 == httpHeaderParser->Init(&h));
    const char* headers =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Last-Modified: Thu, 13 Aug 2009 14:\r\n\r\n";
    UNIT_ASSERT(2 == httpHeaderParser->Execute(headers, strlen(headers)));
    UNIT_ASSERT(h.http_time < 0); // XXX: don't understand what is the proper value
    TestFinish();
}

void THttpHeaderParserTestSuite::TestResponseHeaderOnRequest() {
    TestStart();
    THttpHeader httpHeader;
    httpHeaderParser->Init(&httpHeader);
    const char *request = "GET /search?q=hi HTP/1.1\r\n"
        "Host: www.google.ru:8080\r\n\r\n";
    i32 result = httpHeaderParser->Execute(request, strlen(request));
    UNIT_ASSERT(result != 2);
    TestFinish();
}

void THttpHeaderParserTestSuite::TestRequestHeaderOnResponse() {
    TestStart();
    THttpRequestHeader httpRequestHeader;
    httpHeaderParser->Init(&httpRequestHeader);
    const char *response = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Last-Modified: Thu, 13 Aug 2009 14:\r\n\r\n";
    i32 result = httpHeaderParser->Execute(response, strlen(response));
    UNIT_ASSERT(result != 2);
    TestFinish();
}

UNIT_TEST_SUITE_REGISTRATION(THttpHeaderParserTestSuite);

