#include "yexception.h"

#include <stlport/string>

#include <stdexcept>

#include <util/folder/dirut.h>

Stroka CurrentExceptionMessage() {
    try {
        throw;
    } catch (const std::exception& e) {
        return e.what();
    } catch (...) {
    }

    return "unknown error";
}

bool UncaughtException() {
    return std::uncaught_exception();
}

void ThrowBadAlloc() {
    throw std::bad_alloc();
}

void ThrowLengthError(const char* descr) {
    throw std::length_error(descr);
}

void ThrowRangeError(const char* descr) {
    throw std::out_of_range(descr);
}

static bool CmpStrIgnoringSlashes(const char *s1, const char *s2)
{
    for(; (*s1 == '/' && *s2) || *s1 == *s2; ++s1, ++s2)
        if(! *s1)
            return true;
    return false;
}

static size_t GetFilePathOffsetInArcadia() {
    const char* f1 = __FILE__;
    const char* f2 = "util/generic/yexception.cpp";
    size_t l1 = strlen(f1);
    size_t l2 = strlen(f2);
    YASSERT(l1 >= l2);

    size_t off = l1 - l2;
    YASSERT(CmpStrIgnoringSlashes(f2, f1 + off));
    return off;
}

static TStringBuf GetFilePathInArcadia(const TStringBuf& f) {
    static const size_t off = GetFilePathOffsetInArcadia();

    return f.SubStr(off);
}

void TSystemError::Init() {
    yexception& exc = *this;

    exc << STRINGBUF("(");
    exc << TStringBuf(LastSystemErrorText(Status()));
    exc << STRINGBUF(") ");
}

template <>
void Out<NPrivateException::TLineInfo>(TOutputStream& o, const NPrivateException::TLineInfo& t) {
    o << GetFilePathInArcadia(t.File) << ":" << t.Line << ": ";
}

static inline const char* ZeroTerminate(TTempBuf& buf) {
    char* end = (char*)buf.Current();

    if (!buf.Left()) {
        --end;
    }

    *end = 0;

    return buf.Data();
}

const char* NPrivateException::yexception::what() const throw () {
    return ZeroTerminate(Buf_);
}
