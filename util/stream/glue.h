#pragma once

#include "input.h"
#include "output.h"

#include <streambuf>

#include <util/generic/ptr.h>

//DEPRECATED, DONT USE IN NEW CODE
class TStreamBufAdaptor: public std::streambuf {
    public:
        TStreamBufAdaptor(TInputStream* istream, size_t ibuflen);
        TStreamBufAdaptor(TOutputStream* ostream, size_t obuflen);
        TStreamBufAdaptor(TInputStream* istream, size_t ibuflen, TOutputStream* ostream, size_t obuflen);
        virtual ~TStreamBufAdaptor() throw ();

    private:
        virtual int overflow(int c);
        virtual int underflow();
        virtual int sync();

        void SetOutBuf();
        size_t InOutBuffer() const throw ();

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};
