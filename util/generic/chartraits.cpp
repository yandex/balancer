#include "chartraits.h"

#include <util/string/strspn.h>

static inline const char* FindChr(const char* s, char c, size_t len) throw () {
    const char* ret = TCharTraits<char>::Find(s, c, len);

    return ret ? ret : (s + len);
}

const char* FastFindFirstOf(const char* s, size_t len, const char* set, size_t setlen) {
    switch (setlen) {
        case 0:
            return s + len;

        case 1:
            return FindChr(s, *set, len);

        default:
            return TCompactStrSpn(set, set + setlen).FindFirstOf(s, s + len);
    }
}

const char* FastFindFirstNotOf(const char* s, size_t len, const char* set, size_t setlen) {
    return TCompactStrSpn(set, set + setlen).FindFirstNotOf(s, s + len);
}

