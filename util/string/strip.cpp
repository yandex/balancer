#include "strip.h"
#include "ascii.h"

bool collapse(const Stroka &from, Stroka &to, size_t maxlen) {
    unsigned start;
    unsigned len;
    unsigned del = 0;
    bool changed = false;
    to = from;
    if (maxlen == 0)
        maxlen = to.length();
    else
        maxlen = Min(maxlen, to.length());
    for (start = 0; start < maxlen; ++start) {
        for (len = 0; start + len < maxlen; ++len) {
            if (!IsAsciiSpace(to[start + len]))
                break;
        }
        if (len > 1 || (len == 1 && to[start] != ' ')) {
            del += (len - 1);
            changed = true;
            to.replace(start, len, " ");
        }
    }
    return changed;
}

void collapse_text(const Stroka &from, Stroka &to, size_t maxlen) {
    collapse(from, to, maxlen);
    strip(to, to);
    if (+to >= maxlen) {
        to.remove(maxlen - 5); // " ..."
        to.reverse();
        size_t pos = to.find_first_of(" .,;");
        if (pos != stroka::npos && pos < 32)
            to.remove(0, pos + 1);
        to.reverse();
        to.append(" ...");
    }
}
