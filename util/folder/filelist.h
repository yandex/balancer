#pragma once

#include <util/generic/buffer.h>
#include <util/generic/stroka.h>
#include <util/generic/strbuf.h>

class TFileEntitiesList {
public:
    enum EMask {
          EM_FILES = 1
        , EM_DIRS = 2
        , EM_FILES_DIRS = 3
    };

    TFileEntitiesList(EMask mask)
        : Mask(mask)
    {
        Clear();
    }

    void Clear() {
        Cur = NULL;
        FileNamesSize = CurName = 0;
        FileNames.Clear();
        FileNames.Append("", 1);
    }

    const char* Next() {
        return Cur = (CurName++ < FileNamesSize ? strchr(Cur, 0) + 1 : NULL);
    }

    size_t Size() {
        return FileNamesSize;
    }

    inline void Fill(const Stroka& dirname) {
        Fill(dirname, TStringBuf());
    }

    inline void Fill(const Stroka& dirname, TStringBuf prefix) {
        Fill(dirname, prefix, TStringBuf(), 1);
    }

    void Fill(const Stroka& dirname, TStringBuf prefix, TStringBuf suffix, int depth);

    void Restart() {
        Cur = FileNames.Data();
        CurName = 0;
    }

protected:
    TBuffer FileNames;
    size_t FileNamesSize, CurName;
    const char *Cur;
    EMask Mask;
};

class TFileList: public TFileEntitiesList {
public:
    TFileList()
        : TFileEntitiesList(TFileEntitiesList::EM_FILES)
    {
    }
};

class TDirsList: public TFileEntitiesList {
public:
    TDirsList()
        : TFileEntitiesList(TFileEntitiesList::EM_DIRS)
    {
    }
};
