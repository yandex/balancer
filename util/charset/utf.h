#pragma once

#include <util/generic/strbuf.h>

enum EUTF8Detect {
      NotUTF8
    , UTF8
    , ASCII
};

EUTF8Detect UTF8Detect(const char* s, size_t len);

inline EUTF8Detect UTF8Detect(const TStringBuf& input) {
    return UTF8Detect(~input, +input);
}

inline bool IsUtf(const char* input, size_t len) {
    return UTF8Detect(input, len) != NotUTF8;
}

inline bool IsUtf(const TStringBuf& input) {
    return IsUtf(~input, +input);
}
