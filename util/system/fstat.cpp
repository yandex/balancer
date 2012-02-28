#include "file.h"
#include <util/datetime/systime.h>

#include <sys/stat.h>
#include <sys/types.h>

TFileStat::TFileStat() :
          Mode()
        , Uid()
        , Gid()
        , NLinks()
        , Size()
        , ATime()
        , MTime()
        , CTime()
{
}

void MakeStat(TFileStat& st, FHANDLE f) {
#if defined(_unix_)
    struct stat fs;

    if (fstat(f, &fs) < 0)
        memset(&fs, 0, sizeof(fs));
    st.Mode = fs.st_mode;
    st.NLinks = fs.st_nlink;
    st.Uid = fs.st_uid;
    st.Gid = fs.st_gid;
    st.Size = fs.st_size;
    st.ATime = fs.st_atime;
    st.MTime = fs.st_mtime;
    st.CTime = fs.st_ctime;
#elif defined(_win_)
    BY_HANDLE_FILE_INFORMATION info;
    if (!GetFileInformationByHandle(f, &info)) {
        memset(&st, 0, sizeof(st));
        return;
    }

    timeval tv;
    FileTimeToTimeval(&info.ftCreationTime, &tv);
    st.CTime = tv.tv_sec;
    FileTimeToTimeval(&info.ftLastAccessTime, &tv);
    st.ATime = tv.tv_sec;
    FileTimeToTimeval(&info.ftLastWriteTime, &tv);
    st.MTime = tv.tv_sec;
    st.NLinks = info.nNumberOfLinks;
    st.Mode = 0;
    st.Uid = 0;
    st.Gid = 0;
    st.Size = ((ui64)info.nFileSizeHigh << 32) | info.nFileSizeLow;
#endif
}

TFileStat::TFileStat(const TFile& f) {
    MakeStat(*this, f.GetHandle());
}

TFileStat::TFileStat(const TFileHandle& f) {
    MakeStat(*this, f);
}

TFileStat::TFileStat(const Stroka& f) {
    MakeStat(*this, TFileHandle(f, RdOnly));
}
