#include "utf.h"
#include "codepage.h"

EUTF8Detect UTF8Detect(const char* s, size_t len) {
    const unsigned char *s0 = (const unsigned char *)s;
    const unsigned char *send = s0 + len;
    wchar32 rune;
    size_t rune_len;
    EUTF8Detect res = ASCII;

    while (s0 < send) {
        RECODE_RESULT rr = utf8_read_rune(rune, rune_len, s0, send);

        if (rr != RECODE_OK) {
            return NotUTF8;
        }

        if (rune_len > 1) {
            res = UTF8;
        }

        s0 += rune_len;
    }

    return res;
}
