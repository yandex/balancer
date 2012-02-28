#include "encodexml.h"

#include <library/unittest/registar.h>

SIMPLE_UNIT_TEST_SUITE(TEncodeXml) {
    SIMPLE_UNIT_TEST(TestEncodeXML) {
        UNIT_ASSERT_EQUAL(EncodeXML("It's a \"test\" & <TEST>") == "It&#39;s a &quot;test&quot; &amp; &lt;TEST&gt;", true);
    }
}
