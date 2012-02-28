#pragma once

#include "ascii.h"

#include <util/generic/stroka.h>
#include <util/generic/strbuf.h>

template <class It>
inline void StripRangeBegin(It& b, It e) throw () {
    while (b < e && IsAsciiSpace(*b)) {
        ++b;
    }
}

template <class It>
inline void StripRangeEnd(It b, It& e) throw () {
    while (b < e && IsAsciiSpace(*(e - 1))) {
        --e;
    }
}

template <class It>
inline bool StripRange(It& b, It& e) throw () {
    const size_t oldLen = e - b;
    StripRangeBegin(b, e);
    StripRangeEnd(b, e);
    const size_t newLen = e - b;

    return newLen != oldLen;
}

template <class It>
inline bool Strip(It& b, size_t& len) throw () {
    It e = b + len;

    if (StripRange(b, e)) {
        len = e - b;

        return true;
    }

    return false;
}

template <class T>
static inline bool StripString(const T& from, T& to) {
    const typename T::TChar* b = from.begin();
    const typename T::TChar* e = from.end();

    if (StripRange(b, e)) {
        to = T(b, e);

        return true;
    }

    to = from;

    return false;
}

template <class T>
static inline T StripString(const T& from) {
    T ret;
    StripString(from, ret);
    return ret;
}

static inline bool strip(const Stroka& from, Stroka& to) {
    return StripString(from, to);
}

inline Stroka& strip(Stroka& s) {
    strip(s, s);
    return s;
}

/// Удаляет пробельные символы с краев строки.
inline Stroka strip(const Stroka& s) {
    Stroka ret = s;
    strip(ret, ret);
    return ret;
}

bool collapse(const Stroka& from, Stroka& to, size_t maxlen = 0);

inline Stroka& collapse(Stroka& s, size_t maxlen = 0) {
    collapse(s, s, maxlen);
    return s;
}

/// Заменяет последовательно идущие подряд пробельные символов одним пробелом.
inline Stroka collapse(const Stroka& s, size_t maxlen = 0) {
    Stroka ret;
    collapse(s, ret, maxlen);
    return ret;
}

void collapse_text(const Stroka& from, Stroka& to, size_t maxlen);

/// То же самое, что collapse() + обрезает строку до заданной длины.
/// @details В конец обрезанной строки вставляется многоточие.
inline void collapse_text(Stroka& s, size_t maxlen) {
    Stroka to;
    collapse_text(s, to, maxlen);
    s = to;
}
