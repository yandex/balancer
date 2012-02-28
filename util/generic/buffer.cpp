#include "ymath.h"
#include "stroka.h"
#include "buffer.h"

#include <util/memory/alloc.h>

TBuffer::TBuffer(size_t len)
    : Data_(0)
    , Len_(0)
    , Pos_(0)
{
    Reserve(len);
}

TBuffer::TBuffer(const Stroka& s)
    : Data_(0)
    , Len_(0)
    , Pos_(0)
{
    Append(~s, +s + 1);
}

void TBuffer::DoReserve(size_t realLen) {
    const size_t len = FastClp2(realLen);

    YASSERT(len > Len_);

    Data_ = (char*)y_reallocate(Data_, len);
    Len_ = len;
}

TBuffer::~TBuffer() throw () {
    y_deallocate(Data_);
}

void TBuffer::AsString(Stroka& s) {
    s.assign(Data(), Size());
    Clear();
}
