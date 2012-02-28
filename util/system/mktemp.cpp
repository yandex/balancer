#include "mktemp.h"

#include <cerrno>
#include <cstring>

#include <util/stream/file.h>
#include <util/generic/yexception.h>

#ifdef _win32_
#   define WIN32_LEAN_AND_MEAN
#   define NOMINMAX
#   include <windows.h>
#   include <io.h>
#else
#   include <unistd.h>
#   include <stdlib.h>
#endif

extern "C" int mkstemps(char* path, int slen);

#ifdef _win32_
static char* WinMkTemp(const char* wrkDir, const char* prefix) {
    char* fname = new char[MAX_PATH];

    if (GetTempFileName(wrkDir, (prefix) ? (prefix) : "yand", 0, fname))
        return fname;
    else {
        delete [] fname;
        return NULL;
    }
}
#endif

// Create temporary file with "tmp" extension
static char* makeTempName(const char* wrkDir, const char* prefix) {
    char* buf = 0;

#ifndef _win32_
//    templName = "prefixXXXXX.tmp";
    int buflen = 20;

    if (wrkDir && *wrkDir)
        buflen += strlen(wrkDir) + 1; // +1 -- for '/' after dir name
    else
        buflen += strlen(P_tmpdir) + 1;

    if (prefix)
        buflen += strlen(prefix);
    buf = new char[buflen + 1];

    if (wrkDir && *wrkDir)
        strcpy(buf, wrkDir);
    else
        strcpy(buf, P_tmpdir);

    if (buf[strlen(buf)-1] != '/')
        strcat(buf, "/");

    if (prefix)
       strcat(buf, prefix);

    strcat(buf, "XXXXXX.tmp");

    int fd = mkstemps(buf, 4);
    if (fd < 0) {
        delete [] buf;
        buf = 0;
    } else {
        close(fd);
    }
#else
    const int TmpDirSize = 1024;
    char TmpDir[TmpDirSize];
    const char* pDir = 0;

    if (wrkDir && *wrkDir)
        pDir = wrkDir;
    else if (GetTempPath(TmpDirSize, TmpDir))
        pDir = TmpDir;

    if ((buf = WinMkTemp(pDir, prefix)) == 0) {
    }
#endif
    return buf;
}

Stroka MakeTempName(const char* wrkDir, const char* prefix) {
    TArrayHolder<char> ret(makeTempName(wrkDir, prefix));

    if (!ret) {
        ythrow yexception() << "can not create temp name(" << wrkDir << ", " << prefix << ")";
    }

    return ret.Get();
}
