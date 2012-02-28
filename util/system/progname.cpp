#include "execpath.h"
#include "progname.h"

#include <util/folder/dirut.h>
#include <util/generic/singleton.h>

static const char *Argv0;

struct TProgramNameHolder {
    inline TProgramNameHolder()
        : ProgName(GetFileNameComponent(Argv0 ? Argv0 : ~GetExecPath()))
    {
    }

    Stroka ProgName;
};

const Stroka& GetProgramName() {
    return Singleton<TProgramNameHolder>()->ProgName;
}

void SetProgramName(const char *argv0) {
    Argv0 = argv0;
}
