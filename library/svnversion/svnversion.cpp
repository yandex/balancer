#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "svnversion_data.h"

extern "C" const char* GetProgramSvnVersion() {
#if defined(REVISION)
    // for package systems generating from svn export but providing REVISION macro
#   define REVISION2(x) #x
#   define REVISION3(x) REVISION2(x)
#   define REVISION4 REVISION3(REVISION)
#   define REVISIONINFO "r"REVISION4
#   if defined(PROGRAM_VERSION)
        return PROGRAM_VERSION "\n\n" REVISIONINFO;
#   else
        return REVISIONINFO" "__DATE__" "__TIME__;
#   endif
#elif defined(PROGRAM_VERSION)
    return PROGRAM_VERSION;
#else
    return "No program version found";
#endif
}

extern "C" const char* GetArcadiaSourcePath() {
#if defined(ARCADIA_SOURCE_PATH)
    return ARCADIA_SOURCE_PATH;
#else
    return "";
#endif
}

extern "C" int GetProgramSvnRevision() {
#if defined(ARCADIA_SOURCE_REVISION)
    return atoi(ARCADIA_SOURCE_REVISION);
#else
    return 0;
#endif
}

extern "C" void PrintSvnVersionAndExit(int argc, char *argv[]) {
    if (argc == 2 && !strcmp(argv[1], "--version")) {
        puts(GetProgramSvnVersion());
        exit(0);
    }
}
