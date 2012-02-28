#include "uri.h"
#include <util/http_url.h>

#include <library/unittest/registar.h>

#include <util/string/htmlentity.h>
#include <util/string/quote.h>
#include <util/system/maxlen.h>

namespace NUri {

struct TTest
{
    TStringBuf Val;
    TParseFlags Flags;
    TState::EParsed State;
    TStringBuf Scheme;
    TStringBuf User;
    TStringBuf Pass;
    TStringBuf Host;
    ui16 Port;
    TStringBuf Path;
    TStringBuf Query;
    TStringBuf Frag;
};

#define URL_MSG(url1, url2, cmp) \
    (Stroka("[") + url1.PrintS() + ("] " cmp " [") + url2.PrintS() + "]")
#define URL_EQ(url1, url2) \
   UNIT_ASSERT_EQUAL_C(url, url2, URL_MSG(url1, url2, "!="))
#define URL_NEQ(url1, url2) \
   UNIT_ASSERT_UNEQUAL_C(url, url2, URL_MSG(url1, url2, "=="))

#define CMP_FLD(fld) \
UNIT_ASSERT_VALUES_EQUAL(url.GetField(TField::Field ## fld), test.fld)

#define CMP_URL(url, test) \
do { \
    CMP_FLD(Scheme); \
    CMP_FLD(User); \
    CMP_FLD(Pass); \
    CMP_FLD(Host); \
    UNIT_ASSERT_VALUES_EQUAL(url.GetPort(), test.Port); \
    CMP_FLD(Path); \
    CMP_FLD(Query); \
    CMP_FLD(Frag); \
} \
while (false)

#define URL_TEST(url, test) \
do { \
    TState::EParsed st = url.ParseUri(test.Val, test.Flags, 0); \
    UNIT_ASSERT_VALUES_EQUAL(st, test.State); \
    CMP_URL(url, test); \
    TUri _url; \
    const Stroka urlstr = url.PrintS(); \
    TState::EParsed st2 = _url.ParseUri(urlstr, test.Flags, 0); \
    UNIT_ASSERT_VALUES_EQUAL(st2, test.State); \
    const Stroka urlstr2 = _url.PrintS(); \
    UNIT_ASSERT_VALUES_EQUAL(urlstr, urlstr2); \
    URL_EQ(url, _url); \
    CMP_URL(_url, test); \
} \
while (false)


SIMPLE_UNIT_TEST_SUITE(URLTest)
{

static const char *urls[] = {
    "http://a/b/c/d;p?q#r",
    "g",             "http://a/b/c/g",
    "./g",           "http://a/b/c/g",
    "g/",            "http://a/b/c/g/",
    "/g",            "http://a/g",
    "//g",           "http://g/",
    "?y",            "http://a/b/c/d;p?y",
    "g?y",           "http://a/b/c/g?y",
    "#s",            "http://a/b/c/d;p?q#s",
    "g#s",           "http://a/b/c/g#s",
    "g?y#s",         "http://a/b/c/g?y#s",
    ";x",            "http://a/b/c/;x",
    "g;x",           "http://a/b/c/g;x",
    "g;x?y#s",       "http://a/b/c/g;x?y#s",
    ".",             "http://a/b/c/",
    "./",            "http://a/b/c/",
    "./.",           "http://a/b/c/",
    "././",          "http://a/b/c/",
    "././.",         "http://a/b/c/",
    "..",            "http://a/b/",
    "../",           "http://a/b/",
    "../.",          "http://a/b/",
    "../g",          "http://a/b/g",
    "../..",         "http://a/",
    "../../",        "http://a/",
    "../../.",       "http://a/",
    "../../g",       "http://a/g",
    "../../../g",    "http://a/g",
    "../../../../g", "http://a/g",
    "/./g",          "http://a/g",
    "g.",            "http://a/b/c/g.",
    ".g",            "http://a/b/c/.g",
    "g..",           "http://a/b/c/g..",
    "..g",           "http://a/b/c/..g",
    "./../g",        "http://a/b/g",
    "./g/.",         "http://a/b/c/g/",
    "g/./h",         "http://a/b/c/g/h",
    "g/../h",        "http://a/b/c/h",
    "g;x=1/./y",     "http://a/b/c/g;x=1/y",
    "g;x=1/../y",    "http://a/b/c/y",
    "g?y/./x",       "http://a/b/c/g?y/./x",
    "g?y/../x",      "http://a/b/c/g?y/../x",
    "g#s/./x",       "http://a/b/c/g#s/./x",
    "g#s/../x",      "http://a/b/c/g#s/../x",
    "?",             "http://a/b/c/d;p?",
    "/?",            "http://a/?",
    "x?",            "http://a/b/c/x?",
    "x%20y",         "http://a/b/c/x%20y",
    "%20y",          "http://a/b/c/%20y",
    // "%2zy",          "http://a/b/c/%2zy",
    0
};

SIMPLE_UNIT_TEST(test_httpURL)
{
    TUri rel, base, abs;
    TState::EParsed er = base.Parse(urls[0]);
    UNIT_ASSERT_VALUES_EQUAL(er, TState::ParsedOK);
    UNIT_ASSERT(base.IsValidAbs());
    UNIT_ASSERT_VALUES_EQUAL(base.PrintS(), urls[0]);

    Stroka errbuf;
    TStringOutput out(errbuf);
    const long mflag = TFeature::FeaturesAll;
    for (int i = 1; urls[i]; i += 2) {
        er = rel.Parse(urls[i]);
        UNIT_ASSERT_VALUES_EQUAL(er, TState::ParsedOK);
        rel.Merge(base);
        UNIT_ASSERT_VALUES_EQUAL_C(rel.PrintS(), urls[i+1], urls[i]);

        // try the same thing differently
        er = rel.Parse(urls[i], mflag, urls[0]);
        UNIT_ASSERT_VALUES_EQUAL(er, TState::ParsedOK);
        UNIT_ASSERT_VALUES_EQUAL_C(rel.PrintS(), urls[i+1], urls[i]);

        // lastly...
        er = abs.Parse(urls[i + 1], mflag);
        UNIT_ASSERT_VALUES_EQUAL(er, TState::ParsedOK);
        errbuf.clear();
        out << '[' << rel.PrintS()
            << "] != [" << abs.PrintS() << ']';
        UNIT_ASSERT_EQUAL_C(rel, abs, errbuf);
    }
}

SIMPLE_UNIT_TEST(test_Schemes)
{
    TUri url;
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("www.ya.ru/index.html"), TState::ParsedOK);
    UNIT_ASSERT_EQUAL(url.GetScheme(), TScheme::SchemeEmpty);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("http://www.ya.ru"), TState::ParsedOK);
    UNIT_ASSERT_EQUAL(url.GetScheme(), TScheme::SchemeHTTP);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("https://www.ya.ru"), TState::ParsedBadScheme);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("https://www.ya.ru", TFeature::FeaturesDefault | TFeature::FeatureSchemeKnown), TState::ParsedOK);
    UNIT_ASSERT_EQUAL(url.GetScheme(), TScheme::SchemeHTTPS);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("httpwhatever://www.ya.ru"), TState::ParsedBadScheme);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("httpwhatever://www.ya.ru", TFeature::FeaturesDefault | TFeature::FeatureSchemeFlexible), TState::ParsedOK);
    UNIT_ASSERT_EQUAL(url.GetScheme(), TScheme::SchemeUnknown);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("httpswhatever://www.ya.ru"), TState::ParsedBadScheme);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("httpswhatever://www.ya.ru", TFeature::FeaturesDefault | TFeature::FeatureSchemeFlexible), TState::ParsedOK);
    UNIT_ASSERT_EQUAL(url.GetScheme(), TScheme::SchemeUnknown);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("ftp://www.ya.ru"), TState::ParsedBadScheme);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("ftp://www.ya.ru", TFeature::FeaturesDefault | TFeature::FeatureSchemeFlexible), TState::ParsedOK);
    UNIT_ASSERT_EQUAL(url.GetScheme(), TScheme::SchemeFTP);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("httpsssss://www.ya.ru"), TState::ParsedBadScheme);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("httpsssss://www.ya.ru", TFeature::FeaturesDefault | TFeature::FeatureSchemeFlexible), TState::ParsedOK);
    UNIT_ASSERT_EQUAL(url.GetScheme(), TScheme::SchemeUnknown);
}

struct Link4Norm {
    const char* const base;
    const char* const link;
    const char* const result;
    TUri::TLinkType ltype;
};

static const Link4Norm link4Norm[] = {
    {"http://www.alltest.ru/all.php?a=aberporth", "http://www.alltest.ru/all.php?a=domestic jobs", "", TUri::LinkIsBad},
    {"http://www.alltest.ru/all.php?a=aberporth", "http://www.alltest.ru/all.php?a=domestic%20jobs", "http://www.alltest.ru/all.php?a=domestic%20jobs", TUri::LinkIsLocal},
    {"http://president.rf/%D0%BD%D0%BE%D0%B2%D0%BE%D1%81%D1%82%D0%B8", "http://president.rf/%D0%BD%D0%BE%D0%B2%D0%BE%D1%81%D1%82%D0%B8/1024", "http://president.rf/%D0%BD%D0%BE%D0%B2%D0%BE%D1%81%D1%82%D0%B8/1024", TUri::LinkIsLocal},
    {NULL, NULL, NULL, TUri::LinkIsBad},
};

SIMPLE_UNIT_TEST(test_httpURLNormalize)
{
    TUri normalizedLink;

    for (int i = 0; link4Norm[i].link; i++) {
        TUri base;
        TState::EParsed er = base.Parse(link4Norm[i].base);
        UNIT_ASSERT_VALUES_EQUAL(er, TState::ParsedOK);
        TUri::TLinkType ltype = normalizedLink.Normalize(base, link4Norm[i].link);
        UNIT_ASSERT_VALUES_EQUAL(ltype, link4Norm[i].ltype);
        Stroka s = normalizedLink.PrintS();
        UNIT_ASSERT_VALUES_EQUAL(s, link4Norm[i].result);
    }
}

static const char *urlsWithMultiplySlash[] = {
    "http://a/http://b", "http://a/http://b",
    "http://a/https://b", "http://a/https://b",
    "http://a/b://c", "http://a/b:/c",
    "http://a/b//c", "http://a/b/c",
    NULL, NULL
};

SIMPLE_UNIT_TEST(test_httpURLPathOperation)
{
    char copyUrl[URL_MAXLEN];
    for (int i = 0;urlsWithMultiplySlash[i];i += 2) {
        const TStringBuf url(urlsWithMultiplySlash[i]);
        memcpy(copyUrl, url.data(), url.length());
        char* p = copyUrl;
        char* e = copyUrl + url.length();
        TUri::PathOperation(p, e, 1);
        UNIT_ASSERT_VALUES_EQUAL(TStringBuf(p, e), urlsWithMultiplySlash[i + 1]);
    }
}

static const char* hostsForCheckHost[] = {
    "simplehost.ru",
    "third_level.host.ru",
    "second_level.ru",
    NULL
};

static TState::EParsed answersForCheckHost[] = {
    TState::ParsedOK,
    TState::ParsedOK,
    TState::ParsedBadHost
};

SIMPLE_UNIT_TEST(test_httpURLCheckHost)
{
    for (size_t index = 0;hostsForCheckHost[index];++index) {
        TState::EParsed state = TUri::CheckHost(hostsForCheckHost[index]);
        UNIT_ASSERT_VALUES_EQUAL(state, answersForCheckHost[index]);
    }
}

SIMPLE_UNIT_TEST(test_httpURLSet)
{

    // set port
    {
        TUri parsedUrl;
        parsedUrl.Parse("http://www.host.com/script.cgi?param1=value1&param2=value2");
        parsedUrl.FldMemSet(TField::FieldPort, "8080");
        UNIT_ASSERT_VALUES_EQUAL(parsedUrl.GetPort(), 8080);
        UNIT_ASSERT_VALUES_EQUAL(parsedUrl.PrintS(), "http://www.host.com:8080/script.cgi?param1=value1&param2=value2");
    }

    // clear port
    {
        TUri parsedUrl;
        parsedUrl.Parse("http://www.host.com:8080/script.cgi?param1=value1&param2=value2");
        parsedUrl.FldMemSet(TField::FieldPort, NULL);
        UNIT_ASSERT_VALUES_EQUAL(parsedUrl.GetPort(), 80);
        UNIT_ASSERT_VALUES_EQUAL(parsedUrl.PrintS(), "http://www.host.com/script.cgi?param1=value1&param2=value2");
    }

    // change scheme with default port
    {
        TUri parsedUrl;
        parsedUrl.Parse("http://www.host.com/script.cgi?param1=value1&param2=value2");
        parsedUrl.FldMemSet(TField::FieldScheme, "https");
        UNIT_ASSERT_VALUES_EQUAL(parsedUrl.GetPort(), 443);
        UNIT_ASSERT_VALUES_EQUAL(parsedUrl.PrintS(), "https://www.host.com/script.cgi?param1=value1&param2=value2");
    }

    // change scheme with non-default port
    {
        TUri parsedUrl;
        parsedUrl.Parse("http://www.host.com:8080/script.cgi?param1=value1&param2=value2");
        parsedUrl.FldMemSet(TField::FieldScheme, "https");
        UNIT_ASSERT_VALUES_EQUAL(parsedUrl.GetPort(), 8080);
        UNIT_ASSERT_VALUES_EQUAL(parsedUrl.PrintS(), "https://www.host.com:8080/script.cgi?param1=value1&param2=value2");
    }
}

SIMPLE_UNIT_TEST(test_httpURLAuth)
{

    {
        TUri parsedUrl;
        TState::EParsed st = parsedUrl.Parse("http://@www.host.com/path", TFeature::FeaturesRobot);
        UNIT_ASSERT_VALUES_EQUAL(st, TState::ParsedBadAuth);
    }

    {
        TUri parsedUrl;
        TState::EParsed st = parsedUrl.Parse("http://loginwithnopass@www.host.com/path", TFeature::FeatureAuthSupported);
        UNIT_ASSERT_VALUES_EQUAL(st, TState::ParsedOK);
        UNIT_ASSERT_EQUAL(parsedUrl.GetField(TField::FieldHost), "www.host.com");
        UNIT_ASSERT_EQUAL(parsedUrl.GetField(TField::FieldUser), "loginwithnopass");
        UNIT_ASSERT_EQUAL(parsedUrl.GetField(TField::FieldPass), "");
    }

    {
        TUri parsedUrl;
        TState::EParsed st = parsedUrl.Parse("http://login:pass@www.host.com/path", TFeature::FeatureAuthSupported);
        UNIT_ASSERT_VALUES_EQUAL(st, TState::ParsedOK);
        UNIT_ASSERT_EQUAL(parsedUrl.GetField(TField::FieldHost), "www.host.com");
        UNIT_ASSERT_EQUAL(parsedUrl.GetField(TField::FieldUser), "login");
        UNIT_ASSERT_EQUAL(parsedUrl.GetField(TField::FieldPass), "pass");
    }
}

SIMPLE_UNIT_TEST(test01)
{
    TTest test = {
        "user:pass@host:8080"
        , TFeature::FeaturesAll
        , TState::ParsedOpaque
        , "user", "", "", "", 0, "", "", "" };
    TUri url;
    URL_TEST(url, test);
}

SIMPLE_UNIT_TEST(test02)
{
    TTest test = {
        "http://host"
        , TFeature::FeaturesAll
        , TState::ParsedOK
        , "http", "", "", "host", 80, "/", "", "" };
    TUri url;
    URL_TEST(url, test);
}

SIMPLE_UNIT_TEST(test03)
{
    TTest test = {
        "https://host"
        , TFeature::FeatureSchemeFlexible | TFeature::FeatureAllowHostIDN
        , TState::ParsedOK
        , "https", "", "", "host", 443, "/", "", "" };
    TUri url;
    URL_TEST(url, test);
}

SIMPLE_UNIT_TEST(test04)
{
    TTest test = {
        "user:pass@host:8080"
        , TFeature::FeaturesAll | TFeature::FeatureHierURI
        , TState::ParsedOK
        , "", "user", "pass", "host", 8080, "/", "", "" };
    TUri url;
    URL_TEST(url, test);
    TUri url2(url);
    CMP_URL(url2, test);
    URL_EQ(url, url2);
}

SIMPLE_UNIT_TEST(test05)
{
    TTest test = {
        "host:8080"
        , TFeature::FeaturesAll | TFeature::FeatureHierURI
        , TState::ParsedOK
        , "", "", "", "host", 8080, "/", "", "" };
    TUri url;
    URL_TEST(url, test);
}

SIMPLE_UNIT_TEST(test06)
{
    TTest test = {
        "user:pass@host?q"
        , TFeature::FeaturesAll | TFeature::FeatureHierURI
        , TState::ParsedOK
        , "", "user", "pass", "host", 0, "/", "q", "" };
    TUri url;
    URL_TEST(url, test);
    url.FldMemSet(TField::FieldScheme, "https");
    UNIT_ASSERT(!url.FldIsDirty());
    UNIT_ASSERT_VALUES_EQUAL(url.GetField(TField::FieldScheme), "https");
    UNIT_ASSERT_VALUES_EQUAL(url.GetPort(), 443);

    // test copying
    TUri url2(url);
    // make sure strings are equal...
    UNIT_ASSERT_VALUES_EQUAL(
        url.GetField(TField::FieldUser),
        url2.GetField(TField::FieldUser));
    // ... and memory locations are the same
    UNIT_ASSERT_EQUAL(
        url.GetField(TField::FieldUser),
        url2.GetField(TField::FieldUser));
    // and urls compare the same
    URL_EQ(url, url2);

    // cause a dirty field
    url.FldMemSet(TField::FieldUser, "use"); // it is now shorter
    UNIT_ASSERT(!url.FldIsDirty());
    url.FldMemSet(TField::FieldUser, STRINGBUF("user"));
    UNIT_ASSERT(url.FldIsDirty());

    // copy again
    url2 = url;
    UNIT_ASSERT(url.FldIsDirty());
    UNIT_ASSERT(!url2.FldIsDirty());
    URL_EQ(url, url2);
    // make sure strings are equal...
    UNIT_ASSERT_VALUES_EQUAL(
        url.GetField(TField::FieldUser),
        url2.GetField(TField::FieldUser));
    // ... but memory locations are different
    UNIT_ASSERT_UNEQUAL(
        url.GetField(TField::FieldUser).data(),
        url2.GetField(TField::FieldUser).data());
    URL_EQ(url, url2);

    // make query empty
    url.FldMemSet(TField::FieldQuery, "");
    url2 = url;
    URL_EQ(url, url2);
    // set query to null value (should clear it)
    url2.FldMemSet(TField::FieldQuery, TStringBuf());
    // make sure they are no longer equal
    URL_NEQ(url, url2);
    // reset query
    url.FldClr(TField::FieldQuery);
    // equal again
    URL_EQ(url, url2);
    // reset port and set the other to default
    url.FldClr(TField::FieldPort);
    url2.FldMemSet(TField::FieldPort, "443");
    URL_EQ(url, url2);
}

SIMPLE_UNIT_TEST(test07)
{
    TTest test = {
        "http://host/path//"
        , TFeature::FeaturesAll | TFeature::FeatureHierURI
        , TState::ParsedOK
        , "http", "", "", "host", 80, "/path/", "", "" };
    TUri url;
    URL_TEST(url, test);
    url.FldMemSet(TField::FieldScheme, "HTTPs");
    UNIT_ASSERT_EQUAL(TScheme::SchemeHTTPS, url.GetScheme());
    UNIT_ASSERT_EQUAL("https", url.GetField(TField::FieldScheme));
    url.FldMemSet(TField::FieldScheme, "HtTP");
    UNIT_ASSERT_EQUAL(TScheme::SchemeHTTP, url.GetScheme());
    UNIT_ASSERT_EQUAL("http", url.GetField(TField::FieldScheme));
}

SIMPLE_UNIT_TEST(test08)
{
    {
        TTest test = {
            "host:/path/.path/."
            , TFeature::FeaturesAll | TFeature::FeatureHierURI
            , TState::ParsedOK
            , "", "", "", "host", 0, "/path/.path/", "", "" };
        TUri url;
        URL_TEST(url, test);
    }
    {
        TTest test = {
            "./path"
            , TFeature::FeaturesAll
            , TState::ParsedOK
            , "", "", "", "", 0, "path", "", "" };
        TUri url;
        URL_TEST(url, test);
    }
    {
        TTest test = {
            "/../path"
            , TFeature::FeaturesAll
            , TState::ParsedOK
            , "", "", "", "", 0, "/path", "", "" };
        TUri url;
        URL_TEST(url, test);
    }
}

SIMPLE_UNIT_TEST(test09)
{
    TTest test = {
        "host:"
        , TFeature::FeaturesAll | TFeature::FeatureHierURI
        , TState::ParsedOK
        , "", "", "", "host", 0, "/", "", "" };
    TUri url;
    URL_TEST(url, test);
}


SIMPLE_UNIT_TEST(test10)
{
    // make sure everything in escaped properly, including boundary cases
    {
        Stroka host = "президент.рф";
        const Stroka urlstr = Stroka::Join("http://", host, "/");
        UrlEscape(host);
        TTest test = {
            urlstr
            , TFeature::FeatureEncodeExtendedASCII | TFeature::FeaturesDefault
            , TState::ParsedBadHost
            , "http", "", "", host, 80, "/", "", "" };
        TUri url;
        URL_TEST(url, test);
    }

    {
        Stroka host = "Фilip.ru";
        const Stroka urlstr = Stroka::Join("http://", host);
        UrlEscape(host);
        TTest test = {
            urlstr
            , TFeature::FeatureEncodeExtendedASCII | TFeature::FeaturesDefault
            , TState::ParsedBadHost
            , "http", "", "", host, 80, "/", "", "" };
        TUri url;
        URL_TEST(url, test);
    }

    {
        Stroka host = "Filip%90.rЯ";
        const Stroka urlstr = Stroka::Join(host, ":8080");
        UrlEscape(host);
        TTest test = {
            urlstr
            , TFeature::FeatureEncodeExtendedASCII
                | TFeature::FeatureDecodeAllowed
                | TFeature::FeaturesDefault
                | TFeature::FeatureHierURI
            , TState::ParsedBadHost
            , "", "", "", host, 8080, "/", "", "" };
        TUri url;
        URL_TEST(url, test);
    }
}


SIMPLE_UNIT_TEST(test11)
{
    {
        TTest test = {
            "HtTp://HoSt/%50aTh/?Query#Frag"
            , TFeature::FeaturesAll | TFeature::FeatureHierURI
            , TState::ParsedOK
            , "http", "", "", "host", 80, "/PaTh/", "Query", "Frag" };
        TUri url;
        URL_TEST(url, test);
    }

    {
        TTest test = {
            "HtTp://HoSt/%50a%54h/?Query#Frag"
            , TParseFlags(
                TFeature::FeaturesAll | TFeature::FeatureHierURI
                , TFeature::FeatureToLower
            )
            , TState::ParsedOK
            , "http", "", "", "host", 80, "/path/", "query", "frag"
        };
        TUri url;
        URL_TEST(url, test);
    }
}


SIMPLE_UNIT_TEST(test12)
{
    // test characters which are not always safe
    {
#define RAW "/:"
#define DEC "%2F:"
#define ENC "%2F%3A"
        TTest test = {
            "http://" ENC ":" ENC "@host/" ENC "?" ENC "#" ENC
            , TFeature::FeaturesAll
            , TState::ParsedOK
            , "http", RAW, RAW, "host", 80, "/" DEC, RAW, RAW };
        TUri url;
        URL_TEST(url, test);
        UNIT_ASSERT_VALUES_EQUAL(url.PrintS()
            , "http://" ENC ":" ENC "@host/" DEC "?" RAW "#" RAW);
#undef RAW
#undef DEC
#undef ENC
    }
    {
#define RAW "?@"
#define DEC "%3F@"
#define ENC "%3F%40"
        TTest test = {
            "http://" ENC ":" ENC "@host/" ENC "?" ENC "#" ENC
            , TFeature::FeaturesAll
            , TState::ParsedOK
            , "http", RAW, RAW, "host", 80, "/" DEC, RAW, RAW };
        TUri url;
        URL_TEST(url, test);
        UNIT_ASSERT_VALUES_EQUAL(url.PrintS()
            , "http://" ENC ":" ENC "@host/" DEC "?" RAW "#" RAW);
#undef RAW
#undef DEC
#undef ENC
    }
    {
#define RAW "%&;="
#define DEC "%25&;="
#define ENC "%25%26%3B%3D"
        TTest test = {
            "http://" ENC ":" ENC "@host/" ENC "?" ENC "#" ENC
            , TFeature::FeaturesAll
            , TState::ParsedOK
            , "http", RAW, RAW, "host", 80, "/" ENC, ENC, ENC };
        TUri url;
        URL_TEST(url, test);
        UNIT_ASSERT_VALUES_EQUAL(url.PrintS()
            , "http://" ENC ":" ENC "@host/" ENC "?" ENC "#" ENC);
#undef RAW
#undef DEC
#undef ENC
    }
    {
#define RAW "!$'()*,"
#define DEC "!$%27()*,"
#define ENC "%21%24%27%28%29%2A%2C"
        TTest test = {
            "http://" ENC ":" ENC "@host/" ENC "?" ENC "#" ENC
            , TFeature::FeaturesAll
            , TState::ParsedOK
            , "http", RAW, RAW, "host", 80, "/" ENC, DEC, DEC };
        TUri url;
        URL_TEST(url, test);
        UNIT_ASSERT_VALUES_EQUAL(url.PrintS()
            , "http://" ENC ":" ENC "@host/" ENC "?" DEC "#" DEC);
#undef RAW
#undef DEC
#undef ENC
    }
}

static const TStringBuf NonRfcUrls[] = {
"http://deshevle.ru/price/price=&SrchTp=1&clID=24&BL=SrchTp=0|clID=24&frmID=75&SortBy=P&PreSort=&NmDir=0&VndDir=0&PrDir=0&SPP=44",
"http://secure.rollerwarehouse.com/skates/aggressive/skates/c/11[03]/tx/$$$+11[03][a-z]",
"http://secure.rollerwarehouse.com/skates/aggressive/skates/tx/$$$+110[a-z]",
"http://translate.google.com/translate_t?langpair=en|ru",
"http://www.garnier.com.ru/_ru/_ru/our_products/products_trade.aspx?tpcode=OUR_PRODUCTS^PRD_BODYCARE^EXTRA_SKIN^EXTRA_SKIN_BENEFITS",
"http://www.km.ru/magazin/view_print.asp?id={1846295A-223B-41DC-9F51-90D5D6236C49}",
"http://www.manutd.com/default.sps?pagegid={78F24B85-702C-4DC8-A5D4-2F67252C28AA}&itype=12977&pagebuildpageid=2716&bg=1",
"http://www.pokupay.ru/price/price=&SrchTp=1&clID=24&BL=SrchTp=0|clID=24&frmID=75&SPP=35&SortBy=N&PreSort=V&NmDir=0&VndDir=1&PrDir=0",
"http://www.rodnoyspb.ru/rest/plager/page[0].html",
"http://www.trinity.by/?section_id=46,47,48&cat=1&filters[]=2^_^Sony",
NULL
};

SIMPLE_UNIT_TEST(test_NonRfcUrls)
{
    TUri url;
    const long flags = TFeature::FeaturesRobot;
    for (size_t i = 0; ; ++i)
    {
        const TStringBuf &buf = NonRfcUrls[i];
        if (!buf.IsInited())
            break;
        UNIT_ASSERT_VALUES_EQUAL(TState::ParsedOK, url.Parse(buf, flags));
    }
}

}

SIMPLE_UNIT_TEST_SUITE(TInvertDomainTest)
{
    SIMPLE_UNIT_TEST(TestInvert)
    {
        Stroka a;
        UNIT_ASSERT_EQUAL(InvertDomain(a), "");
        Stroka aa(".:/foo");
        UNIT_ASSERT_EQUAL(InvertDomain(aa), ".:/foo");
        Stroka aaa("/foo.bar:");
        UNIT_ASSERT_EQUAL(InvertDomain(aaa), "/foo.bar:");
        Stroka b("ru");
        UNIT_ASSERT_EQUAL(InvertDomain(b), "ru");
        Stroka c(".ru");
        UNIT_ASSERT_EQUAL(InvertDomain(c), "ru.");
        Stroka d("ru.");
        UNIT_ASSERT_EQUAL(InvertDomain(d), ".ru");
        Stroka e("www.yandex.ru:80/yandsearch?text=foo");
        UNIT_ASSERT_EQUAL(InvertDomain(e), "ru.yandex.www:80/yandsearch?text=foo");
        Stroka f("www.yandex.ru:80/yandsearch?text=foo");
        InvertDomain(f.begin(), f.begin() + 10);
        UNIT_ASSERT_EQUAL(f, "yandex.www.ru:80/yandsearch?text=foo");
        Stroka g("https://www.yandex.ru:80//");
        UNIT_ASSERT_EQUAL(InvertDomain(g), "https://ru.yandex.www:80//");
        Stroka h("www.yandex.ru:8080/redir.pl?url=https://google.com/");
        UNIT_ASSERT_EQUAL(InvertDomain(h), "ru.yandex.www:8080/redir.pl?url=https://google.com/");
    }
}

}
