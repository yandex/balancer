#include "glue.h"

#include <cstdio>

#include <util/memory/addstorage.h>

class TStreamBufAdaptor::TImpl: public TAdditionalStorage<TImpl> {
    public:
        inline TImpl(TInputStream* istream, size_t ibuflen, TOutputStream* ostream, size_t obuflen)
            : InputStream_(istream)
            , IBufLen_(ibuflen)
            , OutputStream_(ostream)
            , OBufLen_(obuflen)
        {
        }

        inline ~TImpl() throw () {
        }

        inline char* InputBuffer() const throw () {
            return (char*)AdditionalData();
        }

        inline size_t InputBufferLength() const throw () {
            return IBufLen_;
        }

        inline char* OutputBuffer() const throw () {
            return InputBuffer() + InputBufferLength();
        }

        inline size_t OutputBufferLength() const throw () {
            return OBufLen_;
        }

        static inline TImpl* Construct(TInputStream* istream, size_t ibuflen, TOutputStream* ostream, size_t obuflen) {
            return new (ibuflen + obuflen + 1) TImpl(istream, ibuflen, ostream, obuflen);
        }

        inline void OverFlow(size_t len, int c) {
            char* buf = OutputBuffer();
            buf[len] = (char)c;
            OutputStream_->Write(buf, len + 1);
        }

        inline bool HaveOutStream() const throw () {
            return (bool)OutputStream_;
        }

        inline size_t UnderFlow() {
            if (!InputStream_) {
                return 0;
            }

            return InputStream_->Read(InputBuffer(), InputBufferLength());
        }

        inline void Sync(size_t len) {
            OutputStream_->Write(OutputBuffer(), len);
        }

    private:
        TInputStream* InputStream_;
        const size_t IBufLen_;
        TOutputStream* OutputStream_;
        const size_t OBufLen_;
};

TStreamBufAdaptor::TStreamBufAdaptor(TInputStream* istream, size_t ibuflen)
    : Impl_(TImpl::Construct(istream, ibuflen, 0, 0))
{
}

TStreamBufAdaptor::TStreamBufAdaptor(TOutputStream* ostream, size_t obuflen)
    : Impl_(TImpl::Construct(0, 0, ostream, obuflen))
{
    SetOutBuf();
}

TStreamBufAdaptor::TStreamBufAdaptor(TInputStream* istream, size_t ibuflen,
    TOutputStream* ostream, size_t obuflen)
    : Impl_(TImpl::Construct(istream, ibuflen, ostream, obuflen))
{
    SetOutBuf();
}

TStreamBufAdaptor::~TStreamBufAdaptor() throw () {
    try {
        sync();
    } catch (...) {
    }
}

void TStreamBufAdaptor::SetOutBuf() {
    assert(Impl_->HaveOutStream());

    char* buf = Impl_->OutputBuffer();
    const size_t len = Impl_->OutputBufferLength();

    setp(buf, buf + len);
}

size_t TStreamBufAdaptor::InOutBuffer() const throw () {
    assert(Impl_->HaveOutStream());

    return pptr() - pbase();
}

int TStreamBufAdaptor::overflow(int c) {
    if (!Impl_->HaveOutStream()) {
        return EOF;
    }

    Impl_->OverFlow(InOutBuffer(), c);
    SetOutBuf();

    return !EOF;
}

int TStreamBufAdaptor::underflow() {
    const size_t readed = Impl_->UnderFlow();

    {
        if (!readed) {
            return EOF;
        }

        char* buf = Impl_->InputBuffer();

        setg(buf, buf, buf + readed);
    }

    return (unsigned char)(*gptr());
}

int TStreamBufAdaptor::sync() {
    if (!Impl_->HaveOutStream()) {
        return -1;
    }

    Impl_->Sync(InOutBuffer());
    SetOutBuf();

    return 0;
}
