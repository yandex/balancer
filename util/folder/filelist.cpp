#include "filelist.h"
#include "iterator.h"

#include <util/system/defaults.h>
#include <util/generic/yexception.h>
#include "dirut.h"

void TFileEntitiesList::Fill(const Stroka& dirname, TStringBuf prefix, TStringBuf suffix, int depth) {
    TDirIterator dir(dirname, TDirIterator::TOptions().SetMaxLevel(depth));

    Clear();

    int lengthDirName = dirname.length();
    while (lengthDirName > 0 && (dirname[lengthDirName - 1] == '\\' || dirname[lengthDirName - 1] == '/'))
        lengthDirName--;

    for (TDirIterator::TIterator file = dir.Begin(); file != dir.End(); ++file) {
        if (file->fts_pathlen == file->fts_namelen || file->fts_pathlen <= lengthDirName)
            continue;
        TStringBuf filename = file->fts_path + lengthDirName + 1;

        if (!+filename || !filename.has_prefix(prefix) || !filename.has_suffix(suffix))
            continue;

        if (((Mask & EM_FILES) && file->fts_info == FTS_F) || ((Mask & EM_DIRS) && file->fts_info == FTS_D)) {
            ++FileNamesSize;
            FileNames.Append(~filename, +filename + 1);
        }
    }

    Restart();
}
