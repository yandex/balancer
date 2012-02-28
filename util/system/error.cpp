#include "tls.h"
#include "error.h"

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <util/string/util.h>
#include <util/generic/strfcpy.h>

#if defined (_win_)
    #include <util/network/socket.h>

    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX

    #include <windows.h>
#elif defined(_unix_)
    #include <unistd.h>
#endif

void ClearLastSystemError() {
#if defined(_win_)
    SetLastError(0);
#else
    errno = 0;
#endif
}

int LastSystemError() {
#if defined(_win_)
    int ret = GetLastError();

    if (ret)
        return ret;

    ret = WSAGetLastError();

    if (ret)
        return ret;
    // when descriptors number are over maximum, errno set in this variable
    ret = *(_errno());
    return ret;


#else
    return errno;
#endif
}

#if defined(_win_)
    struct TErrString {
        inline TErrString() throw () {
            data[0] = 0;
        }

        static inline TErrString& Instance() throw () {
            STATIC_THREAD(TErrString) errText;

            return errText;
        }

        char data[1024];
    };
#endif

const char* LastSystemErrorText(int code) {
#if defined(_win_)
    TErrString& text(TErrString::Instance());
    LastSystemErrorText(text.data, sizeof(text.data), code);

    return text.data;
#else
    return strerror(code);
#endif
}

#ifdef _win_
static char* Strip(char* s) {
    size_t len = strlen(s);
    const char* ptr = s;
    Strip(ptr, len);
    if (ptr != s)
        memmove(s, ptr, len);
    s[len] = 0;
    return s;
}
#endif // _win_

void LastSystemErrorText(char* str, size_t size, int code) {
#if defined(_win_)
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, code, 0, str, DWORD(size), 0);
    Strip(str);
#elif defined (_sun_)
    strfcpy(str, strerror(code), size);
#else
    strerror_r(code, str, size);
#endif
}
