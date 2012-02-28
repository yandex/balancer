#pragma once

#include <util/generic/yexception.h>
#include <util/httpcodes.h>

namespace NSrvKernel {
    class THttpError: public yexception {
    public:
        inline THttpError(ui16 code)
            : Code_(code)
        {
            *this << STRINGBUF("(") << HttpCodeStr(code) << STRINGBUF(") ");
        }

        inline ui16 Code() const throw () {
            return Code_;
        }

    private:
        ui16 Code_;
    };
}
