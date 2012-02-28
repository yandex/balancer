#include "null.h"
#include "multi.h"

TMultiInput::TMultiInput(TInputStream* f, TInputStream* s) throw ()
    : C_(f)
    , N_(s)
{
}

TMultiInput::~TMultiInput() throw () {
}

size_t TMultiInput::DoRead(void* buf, size_t len) {
    const size_t ret = C_->Read(buf, len);

    if (ret) {
        return ret;
    }

    C_ = N_;
    N_ = &Cnull;

    return C_->Read(buf, len);
}
