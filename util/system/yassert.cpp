#include "yassert.h"

#include <stdio.h>
#include <stdarg.h>

#include <util/stream/str.h>
#include <util/system/guard.h>
#include <util/string/util.h>
#include <util/stream/output.h>
#include <util/system/spinlock.h>
#include <util/system/backtrace.h>
#include <util/generic/stroka.h>
#include <util/generic/singleton.h>

struct TPanicLockHolder {
    TAdaptiveLock Lock_;
};

void Panic(const char *file, int line, const char *function, const char *expr, const char *format, ...) {
    // We care of panic of first failed thread only
    // Otherwise stderr could contain multiple messages and stack traces shuffled
    TGuard<TAdaptiveLock> guard(Singleton<TPanicLockHolder>()->Lock_);

    Stroka errorMsg;
    va_list args;
    va_start(args, format);
    vsprintf(errorMsg, format, args);
    va_end(args);

    Stroka r;
    TStringOutput o(r);
    o << "VERIFY failed: " << errorMsg << Endl;
    o << "  " << file << ":" << line << Endl;
    o << "  " << function << "(): requirement " << expr << " failed" << Endl;
    Cerr << r;
    PrintBackTrace();
    abort();
}

