#pragma once

#include "defaults.h"

#if defined(_win_)
    #include <winerror.h>
    #if !defined(_MSC_VER) || (_MSC_VER < 1600)
        #define EISCONN WSAEISCONN
        #define EINPROGRESS WSAEINPROGRESS
        #define EALREADY WSAEALREADY
        #define EWOULDBLOCK WSAEWOULDBLOCK
        #define ETIMEDOUT WSAETIMEDOUT
        #define ENOTSOCK WSAENOTSOCK
        #define ECANCELED WSAECANCELLED
        #define ECONNRESET WSAECONNRESET
        #define EHOSTUNREACH WSAEHOSTUNREACH
        #define ECONNABORTED WSAECONNABORTED
    #endif
#endif

void ClearLastSystemError();
int LastSystemError();
void LastSystemErrorText(char* str, size_t size, int code);
const char* LastSystemErrorText(int code);

inline const char* LastSystemErrorText() {
    return LastSystemErrorText(LastSystemError());
}

inline void LastSystemErrorText(char* str, size_t size) {
    LastSystemErrorText(str, size, LastSystemError());
}
