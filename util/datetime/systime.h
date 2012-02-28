#pragma once

#include <ctime>

#include <util/system/platform.h>

// timegm and gmtime_r versions that don't need access to filesystem or a big stack
time_t TimeGM(struct tm* t);
struct tm *GmTimeR(const time_t *timer, struct tm *tmbuf);

#ifdef _win_
#   define WIN32_LEAN_AND_MEAN
#   define NOMINMAX
#   include <windows.h>
#   include <winsock2.h>

    void FileTimeToTimeval(const FILETIME *ft, struct timeval *tv);

    // obtains the current time, expressed as seconds and microseconds since 00:00 UTC, January 1, 1970
    int gettimeofday(struct timeval *tp, void*);

    // thou should not mix these with non-_r functions
    tm* localtime_r(const time_t *clock, tm *result);
    tm* gmtime_r(const time_t *clock, tm *result);
    char *ctime_r(const time_t *clock, char *buf);

    inline time_t timegm(struct tm* t) {
        return TimeGM(t);
    }

    char *strptime(const char *buf, const char *fmt, struct tm *tm); // strptime.cpp
#else
#   include <sys/time.h>
#endif

