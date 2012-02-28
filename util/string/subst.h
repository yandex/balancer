#pragma once

#include <util/generic/stroka.h>

/// Заменяет в строке одни подстроки на другие.
template <class Troka>
size_t SubstGlobal(Troka& s, const Troka& from, const Troka& to) {
    if (from.empty())
        return 0;

    size_t result = 0;
    for (size_t off = 0; (off = s.find(from, off)) != Troka::npos; off += to.length()) {
        s.replace(off, from.length(), to);
        ++result;
    }
    return result;
}

inline size_t SubstGlobal(Stroka& s, const Stroka& from, const Stroka& to) {
    return SubstGlobal<Stroka>(s, from, to);
}

/// Заменяет в строке одни подстроки на другие.
inline size_t SubstGlobal(Stroka& s, char from, char to) {
    size_t result = 0;
    for (char* it = s.begin(); it != s.end(); ++it) {
        if (*it == from) {
            *it = to;
            ++result;
        }
    }
    return result;
}
