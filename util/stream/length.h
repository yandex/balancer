#pragma once

#include "input.h"

#include <util/generic/utility.h>

class TLengthLimitedInput: public TInputStream {
    public:
        inline TLengthLimitedInput(TInputStream* slave, ui64 length) throw ()
            : Slave_(slave)
            , Length_(length)
        {
        }

        virtual ~TLengthLimitedInput() throw () {
        }

        inline ui64 Left() const throw () {
            return Length_;
        }

    private:
        inline size_t DoRead(void* buf, size_t len) {
            const size_t toRead = (size_t)Min<ui64>(Length_, len);
            const size_t ret = Slave_->Read(buf, toRead);

            Length_ -= ret;

            return ret;
        }

        inline size_t DoSkip(size_t len) {
            len = (size_t)Min<ui64>((ui64)len, Length_);
            len = Slave_->Skip(len);
            Length_ -= len;
            return len;
        }

    private:
        TInputStream* Slave_;
        ui64 Length_;
};
