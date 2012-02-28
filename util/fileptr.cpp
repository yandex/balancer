#include "fgood.h"
#include <system/file.h>

#include <sys/stat.h>
#include <sys/types.h>

#if defined(_win_)
# include <io.h>
#endif

STATIC_ASSERT(sSet == SEEK_SET && sCur == SEEK_CUR && sEnd == SEEK_END);

i64 TFILEPtr::length() const {
    int fd = fileno(m_file);
#if defined (_win32_) && defined(_MSC_VER)
    i64 rv = _filelengthi64(fd);
    if (rv == -1)
        ythrow yexception() << "filelength " <<  ~Name << ": " << LastSystemErrorText();
    return rv;
#else
    struct stat statbuf;
    if (::fstat(fd, &statbuf) != 0)
        ythrow yexception() << "fstat " <<  ~Name << ": " << LastSystemErrorText();
    return statbuf.st_size;
#endif
}
