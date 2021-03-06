#include "bzip2.h"

#include <contrib/libs/libbz2/bzlib.h>

#include <util/memory/addstorage.h>

class TBZipDecompress::TImpl: public TAdditionalStorage<TImpl> {
    public:
        inline TImpl(TInputStream* input)
            : Stream_(input)
        {
            Zero(BzStream_);
            Init();
        }

        inline ~TImpl() throw () {
            Clear();
        }

        inline void Init() {
            if (ArcBZ2_bzDecompressInit(&BzStream_, 0, 0) != BZ_OK) {
                ythrow TBZipDecompressError() << "can not init bzip engine";
            }
        }

        inline void Clear() throw () {
            ArcBZ2_bzDecompressEnd(&BzStream_);
        }

        inline size_t Read(void* buf, size_t size) {
            BzStream_.next_out = (char*)buf;
            BzStream_.avail_out = size;

            while (true) {
                if (BzStream_.avail_in == 0) {
                    if (FillInputBuffer() == 0) {
                        return 0;
                    }
                }

                switch (ArcBZ2_bzDecompress(&BzStream_)) {
                    case BZ_STREAM_END: {
                        Clear();
                        Init();
                    }

                    case BZ_OK: {
                        const size_t processed = size - BzStream_.avail_out;

                        if (processed) {
                            return processed;
                        }

                        break;
                    }

                    default:
                        ythrow TBZipDecompressError() << "bzip error";
                }
            }
        }

        inline size_t FillInputBuffer() {
            BzStream_.next_in = (char*)AdditionalData();
            BzStream_.avail_in = Stream_->Read(BzStream_.next_in, AdditionalDataLength());

            return BzStream_.avail_in;
        }

    private:
        TInputStream* Stream_;
        bz_stream BzStream_;
};

TBZipDecompress::TBZipDecompress(TInputStream* input, size_t bufLen)
    : Impl_(new (bufLen) TImpl(input))
{
}

TBZipDecompress::~TBZipDecompress() throw () {
}

size_t TBZipDecompress::DoRead(void* buf, size_t size) {
    return Impl_->Read(buf, size);
}

class TBZipCompress::TImpl: public TAdditionalStorage<TImpl> {
    public:
        inline TImpl(TOutputStream* stream, size_t level)
            : Stream_(stream)
        {
            Zero(BzStream_);

            if (ArcBZ2_bzCompressInit(&BzStream_, level, 0, 0) != BZ_OK) {
                ythrow TBZipCompressError() << "can not init bzip engine";
            }

            BzStream_.next_out = TmpBuf();
            BzStream_.avail_out = TmpBufLen();
        }

        inline ~TImpl() throw () {
            ArcBZ2_bzCompressEnd(&BzStream_);
        }

        inline void Write(const void* buf, size_t size) {
            BzStream_.next_in = (char*)buf;
            BzStream_.avail_in = size;

            while (BzStream_.avail_in) {
                const int ret = ArcBZ2_bzCompress(&BzStream_, BZ_RUN);

                switch (ret) {
                    case BZ_RUN_OK:
                        continue;

                    case BZ_PARAM_ERROR:
                    case BZ_OUTBUFF_FULL:
                        Stream_->Write(TmpBuf(), TmpBufLen() - BzStream_.avail_out);
                        BzStream_.next_out = TmpBuf();
                        BzStream_.avail_out = TmpBufLen();

                        break;

                    default:
                        ythrow TBZipCompressError() << "bzip error(" << ret << ", " << BzStream_.avail_out << ")";
                }
            }
        }

        inline void Flush() {
            /*
             * TODO ?
             */
        }

        inline void Finish() {
            int ret = ArcBZ2_bzCompress(&BzStream_, BZ_FINISH);

            while (ret != BZ_STREAM_END) {
                Stream_->Write(TmpBuf(), TmpBufLen() - BzStream_.avail_out);
                BzStream_.next_out = TmpBuf();
                BzStream_.avail_out = TmpBufLen();

                ret = ArcBZ2_bzCompress(&BzStream_, BZ_FINISH);
            }

            Stream_->Write(TmpBuf(), TmpBufLen() - BzStream_.avail_out);
        }

    private:
        inline char* TmpBuf() throw () {
            return (char*)AdditionalData();
        }

        inline size_t TmpBufLen() const throw () {
            return AdditionalDataLength();
        }

    private:
        TOutputStream* Stream_;
        bz_stream BzStream_;
};

TBZipCompress::TBZipCompress(TOutputStream* out, size_t compressionLevel, size_t bufLen)
    : Impl_(new (bufLen) TImpl(out, compressionLevel))
{
}

TBZipCompress::~TBZipCompress() throw () {
    try {
        Finish();
    } catch (...) {
    }
}

void TBZipCompress::DoWrite(const void* buf, size_t size) {
    if (!Impl_) {
        ythrow TBZipCompressError() << "can not write to finished bzip stream";
    }

    Impl_->Write(buf, size);
}

void TBZipCompress::DoFlush() {
    if (!!Impl_) {
        Impl_->Flush();
    }
}

void TBZipCompress::DoFinish() {
    THolder<TImpl> impl(Impl_.Release());

    if (!!impl) {
        impl->Finish();
    }
}
