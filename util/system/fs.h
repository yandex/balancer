#pragma once

namespace NFs {
    int Remove(const char* name);
    int Rename(const char* oldName, const char* newName);
    void HardLinkOrCopy(const char* oldName, const char* newName);
    void Cat(const char* dstName, const char* srcName);
    void Copy(const char* oldName, const char* newName);
}
