#include "uri.h"
#include <util/http_url.h>

#include <library/unittest/registar.h>

#include <util/string/htmlentity.h>
#include <util/system/maxlen.h>

namespace NUri {

SIMPLE_UNIT_TEST_SUITE(URLTestRU)
{

SIMPLE_UNIT_TEST(test_httpURL2)
{
    TUri url;
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("g:h"), TState::ParsedBadScheme);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("http:g"), TState::ParsedBadFormat);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("/../g"), TState::ParsedBadPath);
    const char* const UpCaseUrl = "http://www.TEST.Ru:80/InDex.html";
    UNIT_ASSERT_VALUES_EQUAL(url.Parse(UpCaseUrl), TState::ParsedOK);
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(), "http://www.TEST.Ru/InDex.html");
    UNIT_ASSERT_VALUES_EQUAL(url.Parse(UpCaseUrl, TFeature::FeaturesDefault|TFeature::FeatureToLower), TState::ParsedOK);
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(), "http://www.test.ru/InDex.html");
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(TField::FlagScheme), "http:");
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(TField::FlagScheme|TField::FlagHost), "http://www.test.ru");
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(TField::FlagHost), "www.test.ru");
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(TField::FlagHost|TField::FlagPath), "www.test.ru/InDex.html");
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(TField::FlagQuery), "");
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("http://www.TEST.Ru:90/InDex.html"), TState::ParsedOK);
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(TField::FlagHostPort|TField::FlagPath), "www.TEST.Ru:90/InDex.html");
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("www.ya.ru/index.html"), TState::ParsedOK);
    UNIT_ASSERT(!url.IsValidAbs());
    UNIT_ASSERT(url.IsNull(TField::FlagHost));
    UNIT_ASSERT(!url.IsNull(TField::FlagPath));
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(TField::FlagPath), "www.ya.ru/index.html");

    UNIT_ASSERT_VALUES_EQUAL(url.Parse("www.TEST.Ru/ÔÅÓÔ\\'\".html?ÔÅÓÔ\\'\"=ÔÅÓÔ+\\'\"%10"), TState::ParsedOK);
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(), "www.TEST.Ru/ÔÅÓÔ\\'\".html?ÔÅÓÔ\\'\"=ÔÅÓÔ+\\'\"%10");

    UNIT_ASSERT_VALUES_EQUAL(url.Parse("www.TEST.Ru/ÔÅÓÔ\\'\".html?ÔÅÓÔ\\'\"=ÔÅÓÔ+\\'\"%10",
        TFeature::FeaturesDefault | TFeature::FeatureEncodeExtendedASCII), TState::ParsedOK);
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(),
       "www.TEST.Ru/%D4%C5%D3%D4\\'\".html?%D4%C5%D3%D4\\'\"=%D4%C5%D3%D4+\\'\"%10");

    UNIT_ASSERT_VALUES_EQUAL(url.Parse("www.TEST.Ru/ÔÅÓÔ\\'\".html?ÔÅÓÔ\\'\"=ÔÅÓÔ+\\'\"%10",
       TFeature::FeaturesDefault | TFeature::FeatureEncodeForSQL), TState::ParsedOK);
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(), "www.TEST.Ru/ÔÅÓÔ%5C%27%22.html?ÔÅÓÔ%5C%27%22=ÔÅÓÔ+%5C%27%22%10");

    UNIT_ASSERT_VALUES_EQUAL(url.Parse("q/%33%26%13%2f%2b%30%20",
       TFeature::FeaturesDefault | TFeature::FeatureDecodeStandard), TState::ParsedOK);
    UNIT_ASSERT_VALUES_EQUAL(url.PrintS(), "q/3%26%13/%2B0%20");

    UNIT_ASSERT_VALUES_EQUAL(url.Parse("http://www.prime-tass.ru/news/0/{656F5BAE-6677-4762-9BED-9E3B77E72055}.uif"),
                             TState::ParsedOK);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("//server/path"), TState::ParsedOK);
    UNIT_ASSERT_VALUES_EQUAL(url.Parse("//server/path", TFeature::FeaturesRobot), TState::ParsedBadFormat);
}

static const char* const links[] = {
"viewforum.php?f=1&amp;sid=b4568481b67b1d7683bea78634b2e240", "viewforum.php?f=1&sid=b4568481b67b1d7683bea78634b2e240",
"./viewtopic.php?p=74&amp;sid=6#p74", "./viewtopic.php?p=74&sid=6#p74",
"viewtopic.php?p=9313&amp;sid=8#9313", "viewtopic.php?p=9313&sid=8#9313",
"profile.php?mode=viewprofile&u=-1#drafts&amp;sid=a6e5989cee27adb5996bfff044af04ca", "profile.php?mode=viewprofile&u=-1#drafts&sid=a6e5989cee27adb5996bfff044af04ca",

"images\nil.jpg", "images%0Ail.jpg",
"http://caedebaturque.termez.su\r\n/?article=218", "http://caedebaturque.termez.su%0D%0A/?article=218",

"javascript:window.external.AddFavorite(\'http://www.humor.look.ru/\',\'úÌÏÂÎÙÅ äÅÄÙ íÏÒÏÚÙ!!!\')", "javascript:window.external.AddFavorite(\'http://www.humor.look.ru/\',\'%FA%CC%CF%C2%CE%D9%C5%20%E4%C5%C4%D9%20%ED%CF%D2%CF%DA%D9!!!\')",
"search.php?search_author=%CB%FE%E4%EC%E8%EB%E0+%C3%F3%F1%E5%E2%E0&amp;showresults=posts&amp;sid=8", "search.php?search_author=%CB%FE%E4%EC%E8%EB%E0+%C3%F3%F1%E5%E2%E0&showresults=posts&sid=8",
"/Search/author/?q=Øòðèáåëü Õ.Â.", "/Search/author/?q=%D8%F2%F0%E8%E1%E5%EB%FC%20%D5.%C2.",
"javascript:ins(\'ÃÎÐØÎÊ\')", "javascript:ins(\'%C3%CE%D0%D8%CE%CA\')",
"?l=ÿ", "?l=%FF",
"content.php?id=3392&theme=Öåíà", "content.php?id=3392&theme=%D6%E5%ED%E0",
"/a-mp3/stype-1/?search=Ð", "/a-mp3/stype-1/?search=%D0%90",
"/a-mp3/stype-1/?search=Ð‘", "/a-mp3/stype-1/?search=%D0%91",
"/a-mp3/stype-1/?search=Ð’", "/a-mp3/stype-1/?search=%D0%92",
"/a-mp3/stype-1/?search=Ð“", "/a-mp3/stype-1/?search=%D0%93",
"/a-mp3/stype-1/?search=Ð”", "/a-mp3/stype-1/?search=%D0%94",
"/a-mp3/stype-1/?search=Ð•", "/a-mp3/stype-1/?search=%D0%95",
"/a-mp3/stype-1/?search=Ð–", "/a-mp3/stype-1/?search=%D0%96",
"/a-mp3/stype-1/?search=Ð—", "/a-mp3/stype-1/?search=%D0%97",
"/a-mp3/stype-1/?search=Ð˜", "/a-mp3/stype-1/?search=%D0%98",
"/a-mp3/stype-1/?search=Ð™", "/a-mp3/stype-1/?search=%D0%99",
"/a-mp3/stype-1/?search=Ðš", "/a-mp3/stype-1/?search=%D0%9A",
"/a-mp3/stype-1/?search=Ð›", "/a-mp3/stype-1/?search=%D0%9B",
"/a-mp3/stype-1/?search=Ðœ", "/a-mp3/stype-1/?search=%D0%9C",
"/a-mp3/stype-1/?search=Ð", "/a-mp3/stype-1/?search=%D0%9D",
"/a-mp3/stype-1/?search=Ðž", "/a-mp3/stype-1/?search=%D0%9E",
"/a-mp3/stype-1/?search=ÐŸ", "/a-mp3/stype-1/?search=%D0%9F",
"/a-mp3/stype-1/?search=Ð", "/a-mp3/stype-1/?search=%D0",
"/a-mp3/stype-1/?search=Ð¡", "/a-mp3/stype-1/?search=%D0%A1",
"/a-mp3/stype-1/?search=Ð¢", "/a-mp3/stype-1/?search=%D0%A2",
"/a-mp3/stype-1/?search=Ð£", "/a-mp3/stype-1/?search=%D0%A3",
"/a-mp3/stype-1/?search=Ð¤", "/a-mp3/stype-1/?search=%D0%A4",
"/a-mp3/stype-1/?search=Ð¥", "/a-mp3/stype-1/?search=%D0%A5",
"/a-mp3/stype-1/?search=Ð¦", "/a-mp3/stype-1/?search=%D0%A6",
"/a-mp3/stype-1/?search=Ð§", "/a-mp3/stype-1/?search=%D0%A7",
"/a-mp3/stype-1/?search=Ð¨", "/a-mp3/stype-1/?search=%D0%A8",
"/a-mp3/stype-1/?search=Ð©", "/a-mp3/stype-1/?search=%D0%A9",
"/a-mp3/stype-1/?search=Ð«", "/a-mp3/stype-1/?search=%D0%AB",
"/a-mp3/stype-1/?search=Ð­", "/a-mp3/stype-1/?search=%D0%AD",
"/a-mp3/stype-1/?search=Ð®", "/a-mp3/stype-1/?search=%D0%AE",
"/a-mp3/stype-1/?search=Ð¯", "/a-mp3/stype-1/?search=%D0%AF",

"javascript:emoticon(\":&#39;(\")", "javascript:emoticon(\":\'(\")",
"javascript:emoticon(\'&gt;:o\')", "javascript:emoticon(\'>:o\')",
"javascript:emoticon(\']:-&gt;\')", "javascript:emoticon(\']:->\')",
"javascript:emoticon(\':-&#33;\')", "javascript:emoticon(\':-!\')",
"javascript:emoticon(\'@}-&gt;--\')", "javascript:emoticon(\'@}->--\')",
"http&#58;//www.is-ufa.ru/price2/price_IS.rar", "http://www.is-ufa.ru/price2/price_IS.rar",
"&#109;&#97;&#105;&#108;&#116;&#111;&#58;&#105;&#110;&#102;&#111;&#64;&#101;&#116;&#101;&#109;&#46;&#100;&#101;", "mailto:info@etem.de",
"&quot;http://www.fubix.ru&quot;", "\"http://www.fubix.ru\"",
"mailto:&#107;&#97;&#109;&#112;&#97;&#64;&#117;&#107;&#114;&#46;&#110;&#101;&#116;?subject=Àðàáñêèé ÿçûê", "mailto:kampa@ukr.net?subject=%C0%F0%E0%E1%F1%EA%E8%E9%20%FF%E7%FB%EA",
NULL
};

SIMPLE_UNIT_TEST(testHtLinkDecode)
{
    char decodedlink[URL_MAXLEN + 10];
    for (int i = 0; links[i]; i += 2) {
        UNIT_ASSERT(HtLinkDecode(links[i], decodedlink, sizeof(decodedlink)));
        UNIT_ASSERT(strcmp(decodedlink, links[i+1]) == 0);
    }
}

}

}
