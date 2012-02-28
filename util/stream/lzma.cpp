#include <util/system/context.h>
#include <util/memory/gc.h>
#include <util/generic/cast.h>

extern "C" {
    #include <contrib/libs/lzmasdk/LzmaEnc.h>
    #include <contrib/libs/lzmasdk/LzmaDec.h>
}

#include "lzma.h"

static const Stroka BAD_ALLOC("bad alloc");

template <class T>
class TInverseFilter {
        class TTrampoLine: public ITrampoLine {
            public:
                inline TTrampoLine(TInverseFilter* parent)
                    : Parent_(parent)
                {
                }

                virtual void DoRun() {
                    Parent_->RunFilter();
                }

            private:
                TInverseFilter* Parent_;
        };

        class TInput: public TInputStream {
            public:
                inline TInput(TInverseFilter* parent)
                    : Parent_(parent)
                {
                }

                virtual ~TInput() throw () {
                }

                virtual size_t DoRead(void* ptr, size_t len) {
                    return Parent_->ReadImpl(ptr, len);
                }

            private:
                TInverseFilter* Parent_;
        };

        class TOutput: public TOutputStream {
            public:
                inline TOutput(TInverseFilter* parent)
                    : Parent_(parent)
                {
                }

                virtual ~TOutput() throw () {
                }

                virtual void DoWrite(const void* ptr, size_t len) {
                    Parent_->WriteImpl(ptr, len);
                }

            private:
                TInverseFilter* Parent_;
        };

    public:
        inline TInverseFilter(TOutputStream* slave, T* filter)
            : Slave_(slave)
            , Filter_(filter)
            , TrampoLine_(this)
            , FilterCtx_(FilterClosure())
            , Finished_(false)
            , In_(0, 0)
        {
        }

        virtual ~TInverseFilter() throw () {
            if (!UncaughtException()) {
                try {
                    Finish();
                } catch (...) {
                }
            } else {
                //rely on gc
            }
        }

        inline void Write(const void* ptr, size_t len) {
            In_.Reset(ptr, len);

            while (In_.Avail()) {
                SwitchTo();
            }
        }

        inline void Finish() {
            if (!Finished_) {
                Finished_ = true;
                SwitchTo();
            }
        }

    private:
        inline void RunFilter() {
            try {
                TInput in(this);
                TOutput out(this);

                (*Filter_)(&in, &out);
            } catch (...) {
                try {
                    ShitHappen_ = CurrentExceptionMessage();
                } catch (...) {
                    ShitHappen_ = BAD_ALLOC;
                }
            }

            SwitchFrom();
        }

        inline TContClosure FilterClosure() {
            TContClosure ret = {
                &TrampoLine_,
                TMemRange(Stack_, sizeof(Stack_))
            };

            return ret;
        }

        inline size_t ReadImpl(void* ptr, size_t len) {
            while (!Finished_) {
                const size_t ret = In_.Read(ptr, len);

                if (ret) {
                    return ret;
                }

                SwitchFrom();
            }

            return 0;
        }

        inline void WriteImpl(const void* ptr, size_t len) {
            YASSERT(!Out_.Avail());

            Out_.Reset(ptr, len);

            while (Out_.Avail()) {
                SwitchFrom();
            }
        }

        inline void FlushImpl() {
            if (Out_.Avail()) {
                TransferData(CheckedCast<IZeroCopyInput*>(&Out_), Slave_);
                Out_.Reset(0, 0);
            }
        }

        inline void SwitchTo() {
            CurrentCtx_.SwitchTo(&FilterCtx_);

            if (!!ShitHappen_) {
                Finished_ = true;
                ythrow yexception() << "shit happen in subfilter(" << ShitHappen_ << ")";
            }

            FlushImpl();
        }

        inline void SwitchFrom() {
            FilterCtx_.SwitchTo(&CurrentCtx_);
        }

    private:
        TOutputStream* Slave_;
        T* Filter_;
        TTrampoLine TrampoLine_;
        char Stack_[16 * 1024];
        char Fake_[1 * 1024];
        TContMachineContext FilterCtx_;
        TContMachineContext CurrentCtx_;
        bool Finished_;
        TMemoryInput In_;
        TMemoryInput Out_;
        Stroka ShitHappen_;
};

class TLzma {
    public:
        class TLzmaInput: public ISeqInStream {
            public:
                inline TLzmaInput(TInputStream* slave)
                    : Slave_(slave)
                {
                    Read = ReadFunc;
                }

            private:
                static inline SRes ReadFunc(void* p, void* ptr, size_t* len) {
                    *len = static_cast<TLzmaInput*>((ISeqInStream*)p)->Slave_->Read(ptr, *len);

                    return SZ_OK;
                }

            private:
                TInputStream* Slave_;
        };

        class TLzmaOutput: public ISeqOutStream {
            public:
                inline TLzmaOutput(TOutputStream* slave)
                    : Slave_(slave)
                {
                    Write = WriteFunc;
                }

            private:
                static inline size_t WriteFunc(void* p, const void* ptr, size_t len) {
                    static_cast<TLzmaOutput*>((ISeqOutStream*)p)->Slave_->Write(ptr, len);

                    return len;
                }

            private:
                TOutputStream* Slave_;
        };

        class TAlloc: public ISzAlloc {
            public:
                inline TAlloc() {
                    Alloc = AllocFunc;
                    Free = FreeFunc;
                }

            private:
                static void* AllocFunc(void* t, size_t len) {
                    return static_cast<TAlloc*>(((ISzAlloc*)t))->Gc_.Allocate(len);
                }

                static void FreeFunc(void* t, void* p) {
                    static_cast<TAlloc*>(((ISzAlloc*)t))->Gc_.Deallocate(p);
                }

            private:
                TMemoryGc Gc_;
        };

        inline ISzAlloc* Alloc() throw () {
            return &Alloc_;
        }

        static inline void Check(SRes r) {
            if (r != SZ_OK) {
                ythrow yexception() << "lzma error(" << r << ")";
            }
        }

    private:
        TAlloc Alloc_;
};

class TLzmaCompressBase: public TLzma {
    public:
        inline TLzmaCompressBase(size_t level)
            : H_(LzmaEnc_Create(Alloc()))
        {
            if (!H_) {
                ythrow yexception() << "can not init lzma engine";
            }

            LzmaEncProps_Init(&Props_);

            Props_.level = level;
            Props_.dictSize = 0;
            Props_.lc = -1;
            Props_.lp = -1;
            Props_.pb = -1;
            Props_.fb = -1;
            Props_.numThreads = -1;
            Props_.writeEndMark = 1;

            Check(LzmaEnc_SetProps(H_, &Props_));
            size_t bufLen = sizeof(PropsBuf_);
            Zero(PropsBuf_);
            Check(LzmaEnc_WriteProperties(H_, PropsBuf_, &bufLen));
        }

        inline ~TLzmaCompressBase() throw () {
            LzmaEnc_Destroy(H_, Alloc(), Alloc());
        }

        inline void operator() (TInputStream* in, TOutputStream* out) {
            TLzmaInput input(in);
            TLzmaOutput output(out);

            out->Write(PropsBuf_, sizeof(PropsBuf_));

            Check(LzmaEnc_Encode(H_, &output, &input, 0, Alloc(), Alloc()));
        }

    private:
        CLzmaEncHandle H_;
        CLzmaEncProps Props_;
        Byte PropsBuf_[LZMA_PROPS_SIZE];
};

class TLzmaCompress::TImpl: public TLzmaCompressBase
                          , public TInverseFilter<TLzmaCompressBase> {
    public:
        inline TImpl(TOutputStream* slave, size_t level)
            : TLzmaCompressBase(level)
            , TInverseFilter<TLzmaCompressBase>(slave, this)
        {
        }
};

class TLzmaDecompress::TImpl: public TLzma {
    public:
        inline TImpl(TInputStream* slave)
            : Slave_(slave)
            , InPos_(0)
            , InSize_(0)
        {
            LzmaDec_Construct(&H_);

            Byte buf[LZMA_PROPS_SIZE];

            Slave_->Load(buf, sizeof(buf));

            Check(LzmaDec_Allocate(&H_, buf, sizeof(buf), Alloc()));
            LzmaDec_Init(&H_);
        }

        inline ~TImpl() throw () {
            LzmaDec_Free(&H_, Alloc());
        }

        inline size_t Read(void* ptr, size_t len) {
            size_t ret = 0;

            do {
                ret = Uncompressed_.Read(ptr, len);
            } while (!ret && Uncompress());

            return ret;
        }

    private:
        inline bool Uncompress() {
            if ((InPos_ == InSize_) && !Fill()) {
                return false;
            }

            Uncompressed_.Reset(UncompressedBuf_, DecompressPortion());

            return Uncompressed_.Avail();
        }

        inline size_t DecompressPortion() {
            size_t bufLen = sizeof(UncompressedBuf_);
            size_t availLen = InSize_ - InPos_;
            ELzmaStatus status;

            Check(LzmaDec_DecodeToBuf(&H_, (Byte*)UncompressedBuf_, &bufLen, (Byte*)In_ + InPos_, &availLen, LZMA_FINISH_ANY, &status));

            InPos_ += availLen;

            return bufLen;
        }

        inline bool Fill() {
            InSize_ = Slave_->Read(In_, sizeof(In_));
            InPos_ = 0;

            return InSize_;
        }

    private:
        CLzmaDec H_;
        TInputStream* Slave_;
        TMemoryInput Uncompressed_;
        char UncompressedBuf_[4096];
        char In_[4096];
        size_t InPos_;
        size_t InSize_;
};

TLzmaCompress::TLzmaCompress(TOutputStream* slave, size_t level)
    : Impl_(new TImpl(slave, level))
{
}

TLzmaCompress::~TLzmaCompress() throw () {
}

void TLzmaCompress::DoWrite(const void* buf, size_t len) {
    if (!Impl_) {
        ythrow yexception() << "can not write to finished lzma stream";
    }

    Impl_->Write(buf, len);
}

void TLzmaCompress::DoFinish() {
    THolder<TImpl> impl(Impl_.Release());

    if (!!impl) {
        impl->Finish();
    }
}

TLzmaDecompress::TLzmaDecompress(TInputStream* slave)
    : Impl_(new TImpl(slave))
{
}

TLzmaDecompress::~TLzmaDecompress() throw () {
}

size_t TLzmaDecompress::DoRead(void* buf, size_t len) {
    return Impl_->Read(buf, len);
}
