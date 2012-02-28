#include <util/system/defaults.h>

#if defined(_win_)
    #define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
    #define NOMINMAX
    #include <windows.h>
#endif

#include <util/stream/file.h>

#include "fs.h"
#include "oldfile.h"

int NFs::Remove(const char* name) {
    struct stat sb;

    if (int result = lstat(name, &sb))
        return result;

    if (!S_ISDIR(sb.st_mode))
        return ::remove(name);

    return ::rmdir(name);
}

int NFs::Rename(const char* oldName, const char* newName) {
#if defined(_win_)
    return MoveFileEx(oldName, newName, MOVEFILE_REPLACE_EXISTING) == 0;
#else
    return ::rename(oldName, newName);
#endif
}

void NFs::HardLinkOrCopy(const char* from, const char* to) {
    bool done = false;
#if defined(_win_)
    done = (CreateHardLink(to, from, 0) != 0);
#elif defined(_unix_)
    done = (link(from, to) == 0);
#endif
    if (!done)
        Copy(from, to);
}

void NFs::Cat(const char* dstName, const char* srcName) {
    TFileInput src(srcName);
    TFileOutput dst(TFile(dstName, ForAppend | WrOnly | Seq));

    TransferData(&src, &dst);
}

void NFs::Copy(const char* oldName, const char* newName) {
    TFileInput src(oldName);
    TFileOutput dst(TFile(newName, CreateAlways | WrOnly | Seq));

    TransferData(&src, &dst);
}
