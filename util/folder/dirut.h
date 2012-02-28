#pragma once

#include <sys/stat.h>
#include <sys/types.h>

#include <cerrno>
#include <cstdlib>

#include <util/system/defaults.h>
#include <util/generic/stroka.h>
#include <util/generic/yexception.h>

#ifdef _win32_
#   define WIN32_LEAN_AND_MEAN
#   define NOMINMAX
#   include <windows.h>
#   include <direct.h>
#   include <malloc.h>
#   include <time.h>
#   include <io.h>
#   include "dirent_win.h"

    // these live in freeBSD_mktemp.cpp
    extern "C" int mkstemps(char *path, int slen);
    char * mkdtemp(char *path);

#else
#   ifdef _sun_
#       include <alloca.h>

        char * mkdtemp(char *path);
#   endif
#   include <unistd.h>
#   include <sys/types.h>
#   include <pwd.h>
#   include <dirent.h>
#   ifndef DT_DIR
#       include <sys/stat.h>
#   endif
#endif

int isdir(const char *path);

bool IsDir(const Stroka& path);

// returns false if file exists but not accessible
bool isexist(const char *path);

class TExistenceChecker {
public:
    TExistenceChecker(bool strict = false)
        : Strict(strict)
    { }
    void SetStrict(bool strict) {
        Strict = strict;
    }
    bool IsStrict() const {
        return Strict;
    }
    const char* Check(const char* fname) const {
        if (!fname || !*fname)
            return 0;
        if (Strict)
            VERIFY(isexist(fname), "File %s doesn't exist or is not accessible", fname);
        else if (!isexist(fname))
            fname = 0;
        return fname;
    }
private:
    bool Strict;
};

void CheckIsExist(const char *path);

int mkpath(char *path, int mode = 0777);

Stroka GetHomeDir();

#   define MODE , 0777
#   define MODE0755 , 0755
#   define MODE0775 , 0775
#if defined _win32_
    typedef stroka TPath;
    inline int mkdir(const char *path, int) {
        return mkdir(path);
    }
#else
    typedef Stroka TPath;
#endif // _win32_

void MakeDirIfNotExist(const char *path, int mode = 0777);

/// Create path making parent directories as needed
void MakePathIfNotExist(const char *path, int mode = 0777);

void SlashFolderLocal(Stroka &folder);
bool correctpath(Stroka &filename);
bool resolvepath(Stroka &folder, const Stroka &home);

char GetDirectorySeparator();
const char* GetDirectorySeparatorS();

void RemoveDirWithContents(Stroka dirName);

const char* GetFileNameComponent(const char* f);

Stroka RealPath(const Stroka& path); // throws
Stroka GetCwd();

void ChDir(const Stroka& path);

const char * GetSystemTempDir();

int MakeTempDir(char path[/*FILENAME_MAX*/], const char* prefix);
//! @todo this function should be replaced with @c RemoveDirWithContents
int RemoveTempDir(const char* dirName);

int ResolvePath(const char* rel, const char* abs, char res[/*FILENAME_MAX*/], bool isdir = false);
Stroka ResolvePath(const char* path, bool isDir = false);

Stroka ResolveDir(const char* path);

Stroka GetDirName(const Stroka& path);
