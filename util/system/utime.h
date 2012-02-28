#pragma once

#include "defaults.h"

#ifdef _MSC_VER
#   include <sys/utime.h>
#else
#   include <sys/types.h>
#   include <utime.h>
#endif

inline int TouchFile(const char* filePath) {
    return utime(filePath, NULL);
}

inline int SetModTime(const char* filePath, time_t modtime, time_t actime) {
    struct utimbuf buf;
    buf.modtime = modtime;
    buf.actime = actime;
    return utime(filePath, &buf);
}
