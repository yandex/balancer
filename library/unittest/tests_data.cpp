#include <util/folder/dirut.h>
#include "tests_data.h"

#ifdef _win_
const char* DIR_SEPARATORS = "/\\";
#else
const char* DIR_SEPARATORS = "/";
#endif

Stroka GetArcadiaTestsData() {
    const char* workDir = getcwd(NULL, 0);
    if (!workDir)
        return "";

    Stroka path(workDir); free((void*)workDir);
    while (!path.empty()) {
        Stroka dataDir = path + "/arcadia_tests_data";
        if (isdir(dataDir.c_str()) == 0)
            return dataDir;

        size_t pos = path.find_last_of(DIR_SEPARATORS);
        if (pos == Stroka::npos)
            pos = 0;
        path.erase(pos);
    }

    return "";
}
