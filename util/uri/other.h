#pragma once

#include <util/generic/stroka.h>

// Some functions for inverted url representation
// No scheme cut-off, no 80th port normalization

void TrspChars(char *s);
void UnTrspChars(char *s);
void TrspChars(char *s, size_t l);
void UnTrspChars(char *s, size_t l);
void TrspChars(const char *s, char *d);
void UnTrspChars(const char *s, char *d);

void InvertDomain(char* begin, char* end);

inline Stroka& InvertDomain(Stroka& url) {
    InvertDomain(url.begin(), url.begin() + +url);
    return url;
}

inline Stroka& InvertUrl(Stroka& url) {
    size_t s = url.find('/');
    url.to_lower(0, s);
    InvertDomain(url);
    TrspChars(url.begin());
    return url;
}

inline Stroka& RevertUrl(Stroka& url) {
    UnTrspChars(url.begin());
    InvertDomain(url);
    return url;
}
