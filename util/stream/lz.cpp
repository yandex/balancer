#include "lz.h"

#include <contrib/libs/fastlz/fastlz.h>
#include <contrib/libs/lz4/lz4.h>
#include <contrib/libs/quicklz/quicklz.h>
#include <contrib/libs/minilzo/minilzo.h>
#include <contrib/libs/snappy/snappy.h>

#include <util/system/yassert.h>
#include <util/system/byteorder.h>
#include <util/memory/addstorage.h>
#include <util/generic/utility.h>
#include <util/generic/singleton.h>
#include <util/generic/yexception.h>

static inline ui8 HostToLittle(ui8 t) throw () {
    return t;
}

static inline ui8 LittleToHost(ui8 t) throw () {
    return t;
}

struct TCommonData {
    static const size_t overhead = sizeof(ui16) + sizeof(ui8);
};

template <class TCompressor, class TBase>
class TCompressorBase: public TAdditionalStorage<TCompressorBase<TCompressor, TBase> >
                     , public TCompressor, public TCommonData {
    public:
        inline TCompressorBase(TOutputStream* slave, ui16 blockSize)
            : Slave_(slave)
            , BlockSize_(blockSize)
        {
            /*
             * save signature
             */
            Slave_->Write(TCompressor::signature, sizeof(TCompressor::signature) - 1);

            /*
             * save version
             */
            this->Save((ui32)1);

            /*
             * save block size
             */
            this->Save(BlockSize());
        }

        inline ~TCompressorBase() throw () {
        }

        inline void Write(const char* buf, size_t len) {
            while (len) {
                const ui16 toWrite = (ui16)Min<size_t>(len, this->BlockSize());

                this->WriteBlock(buf, toWrite);

                buf += toWrite;
                len -= toWrite;
            }
        }

        inline void Flush() {
        }

        inline void Finish() {
            this->Flush();
            this->WriteBlock(0, 0);
        }

        template <class T>
        static inline void Save(T t, TOutputStream* out) {
            t = HostToLittle(t);

            out->Write(&t, sizeof(t));
        }

        template <class T>
        inline void Save(T t) {
            Save(t, Slave_);
        }

    private:
        inline void* Block() const throw () {
            return this->AdditionalData();
        }

        inline ui16 BlockSize() const throw () {
            return BlockSize_;
        }

        inline void WriteBlock(const void* ptr, ui16 len) {
            YASSERT(len <= this->BlockSize());

            ui8 compressed = false;

            if (len) {
                const size_t out = this->Compress((const char*)ptr, len, (char*)Block());

                if (out < len || TCompressor::SaveIncompressibleChunks()) {
                    compressed = true;
                    ptr = Block();
                    len = (ui16)out;
                }
            }

            char tmp[overhead];
            TMemoryOutput header(tmp, sizeof(tmp));

            this->Save(len, &header);
            this->Save(compressed, &header);

            typedef TOutputStream::TPart TPart;
            const TPart parts[] = {
                TPart(tmp, sizeof(tmp)),
                TPart(ptr, len)
            };

            Slave_->Write(parts, sizeof(parts) / sizeof(*parts));
        }

    private:
        TOutputStream* Slave_;
        const ui16 BlockSize_;
};

template <class T>
static inline T GLoad(TInputStream* input) {
    T t;

    if (input->Load(&t, sizeof(t)) != sizeof(t)) {
        ythrow TDecompressorError() << "stream error";
    }

    return LittleToHost(t);
}

template <class TDecompressor, class TBase>
class TDecompressorBase: public TDecompressor, public TCommonData {
    public:
        inline TDecompressorBase(TInputStream* slave)
            : Slave_(slave)
            , Input_(0, 0)
            , Eof_(false)
            , Version_(this->LoadVersion())
            , BlockSize_(Load<ui16>())
            , OutBufSize_(TDecompressor::Hint(BlockSize_))
            , Tmp_(::operator new(2 * OutBufSize_))
            , In_((char*)Tmp_.Get())
            , Out_(In_ + OutBufSize_)
        {
            this->InitFromStream(Slave_);
        }

        inline ~TDecompressorBase() throw () {
        }

        inline ui32 LoadVersion() {
            this->CheckSignature();

            return GLoad<ui32>(Slave_);
        }

        inline void CheckSignature() {
            char tmp[sizeof(TDecompressor::signature) - 1];

            if (Slave_->Load(tmp, sizeof(tmp)) != sizeof(tmp)) {
                ythrow TDecompressorError() << "can not load stream signature";
            }

            if (memcmp(tmp, TDecompressor::signature, sizeof(tmp)) != 0) {
                ythrow TDecompressorError() << "incorrect signature";
            }
        }

        inline size_t Read(void* buf, size_t len) {
            size_t ret = Input_.Read(buf, len);

            if (ret) {
                return ret;
            }

            if (Eof_) {
                return 0;
            }

            this->FillNextBlock();

            ret = Input_.Read(buf, len);

            if (ret) {
                return ret;
            }

            Eof_ = true;

            return 0;
        }

        inline void FillNextBlock() {
            char tmp[overhead];

            if (Slave_->Load(tmp, sizeof(tmp)) != sizeof(tmp)) {
                ythrow TDecompressorError() << "can not read block header";
            }

            TMemoryInput header(tmp, sizeof(tmp));

            const ui16 len = GLoad<ui16>(&header);
            const ui8 compressed = GLoad<ui8>(&header);

            if (compressed > 1) {
                ythrow TDecompressorError() << "broken header";
            }

            if (Slave_->Load(In_, len) != len) {
                ythrow TDecompressorError() << "can not read data";
            }

            if (compressed) {
                const size_t ret = this->Decompress(In_, len, Out_, OutBufSize_);

                Input_.Reset(Out_, ret);
            } else {
                Input_.Reset(In_, len);
            }
        }

        template <class T>
        inline T Load() {
            return GLoad<T>(Slave_);
        }

    protected:
        TInputStream* Slave_;
        TMemoryInput Input_;
        bool Eof_;
        const ui32 Version_;
        const ui16 BlockSize_;
        const size_t OutBufSize_;
        THolder<void> Tmp_;
        char* In_;
        char* Out_;
};

#define DEF_COMPRESSOR_COMMON(rname, name)\
    rname::~rname() throw () {\
        try {\
            Finish();\
        } catch (...) {\
        }\
    }\
    \
    void rname::DoWrite(const void* buf, size_t len) {\
        if (!Impl_) {\
            ythrow yexception() << "can not write to finalized stream";\
        }\
        \
        Impl_->Write((const char*)buf, len);\
    }\
    \
    void rname::DoFlush() {\
        if (!Impl_) {\
            ythrow yexception() << "can not flush finalized stream";\
        }\
        \
        Impl_->Flush();\
    }\
    \
    void rname::DoFinish() {\
        THolder<TImpl> impl(Impl_.Release());\
        \
        if (!!impl) {\
            impl->Finish();\
        }\
    }

#define DEF_COMPRESSOR(rname, name)\
    class rname::TImpl: public TCompressorBase<name, TImpl> {\
        public:\
            inline TImpl(TOutputStream* out, ui16 blockSize)\
                : TCompressorBase<name, TImpl>(out, blockSize)\
            {\
            }\
    };\
    \
    rname::rname(TOutputStream* slave, ui16 blockSize)\
        : Impl_(new (TImpl::Hint(blockSize)) TImpl(slave, blockSize))\
    {\
    }\
    \
    DEF_COMPRESSOR_COMMON(rname, name)

#define DEF_DECOMPRESSOR(rname, name)\
    class rname::TImpl: public TDecompressorBase<name, TImpl> {\
        public:\
            inline TImpl(TInputStream* in)\
                : TDecompressorBase<name, TImpl>(in)  \
            {\
            }\
    };\
    \
    rname::rname(TInputStream* slave)\
        : Impl_(new TImpl(slave))\
    {\
    }\
    \
    rname::~rname() throw () {\
    }\
    \
    size_t rname::DoRead(void* buf, size_t len) {\
        return Impl_->Read(buf, len);\
    }

/*
 * MiniLzo
 */
class TMiniLzo {
        class TInit {
            public:
                inline TInit() {
                    if (lzo_init() != LZO_E_OK) {
                        ythrow yexception() << "can not init lzo engine";
                    }
                }
        };

    public:
        static const char signature[];

        inline TMiniLzo() {
            Singleton<TInit>();
        }

        inline ~TMiniLzo() throw () {
        }

        static inline size_t Hint(size_t len) throw () {
            return len;
        }

        static inline bool SaveIncompressibleChunks() throw () {
            return false;
        }
};

const char TMiniLzo::signature[] = "YLZO";

template <size_t N>
class TFixedArray {
    public:
        inline TFixedArray() throw () {
            memset(WorkMem_, 0, sizeof(WorkMem_));
        }

    protected:
        char WorkMem_[N];
};

class TMiniLzoCompressor: public TMiniLzo, public TFixedArray<LZO1X_MEM_COMPRESS + 1> {
    public:
        inline size_t Compress(const char* data, size_t len, char* ptr) {
            lzo_uint out = 0;
            lzo1x_1_compress((const lzo_bytep)data, len, (lzo_bytep)ptr, &out, WorkMem_);

            return out;
        }
};

class TMiniLzoDecompressor: public TMiniLzo, public TFixedArray<LZO1X_MEM_DECOMPRESS + 1> {
    public:
        inline size_t Decompress(const char* data, size_t len, char* ptr, size_t /*max*/) {
            lzo_uint ret = 0;

            lzo1x_decompress((const lzo_bytep)data, len, (lzo_bytep)ptr, &ret, WorkMem_);

            return ret;
        }

        inline void InitFromStream(TInputStream*) const throw () {
        }
};

DEF_COMPRESSOR(TLzoCompress, TMiniLzoCompressor)
DEF_DECOMPRESSOR(TLzoDecompress, TMiniLzoDecompressor)

/*
 * FastLZ
 */
class TFastLZ {
    public:
        static const char signature[];

        static inline size_t Hint(size_t len) throw () {
            return Max<size_t>((size_t)(len * 1.06), 100);
        }

        inline size_t Compress(const char* data, size_t len, char* ptr) {
            return fastlz_compress(data, len, ptr);
        }

        inline size_t Decompress(const char* data, size_t len, char* ptr, size_t max) {
            return fastlz_decompress(data, len, ptr, max);
        }

        inline void InitFromStream(TInputStream*) const throw () {
        }

        static inline bool SaveIncompressibleChunks() throw () {
            return false;
        }
};

const char TFastLZ::signature[] = "YLZF";

DEF_COMPRESSOR(TLzfCompress, TFastLZ)
DEF_DECOMPRESSOR(TLzfDecompress, TFastLZ)


/*
 * LZ4
 */
class TLZ4 {
    public:
        static const char signature[];

        static inline size_t Hint(size_t len) throw () {
            return Max<size_t>((size_t)(len * 1.06), 100);
        }

        inline size_t Compress(const char* data, size_t len, char* ptr) {
            return LZ4_compress(data, ptr, len);
        }

        inline size_t Decompress(const char* data, size_t len, char* ptr, size_t max) {
            int res = LZ4_uncompress_unknownOutputSize(data, ptr, len, max);
            if (res < 0)
                ythrow TDecompressorError();
            return res;
        }

        inline void InitFromStream(TInputStream*) const throw () {
        }

        static inline bool SaveIncompressibleChunks() throw () {
            return false;
        }
};

const char TLZ4::signature[] = "LZ.4";

DEF_COMPRESSOR(TLz4Compress, TLZ4)
DEF_DECOMPRESSOR(TLz4Decompress, TLZ4)

/*
 * Snappy
 */
class TSnappy {
    public:
        static const char signature[];

        static inline size_t Hint(size_t len) throw () {
            return Max<size_t>((size_t)(len * 1.06), 100);
        }

        inline size_t Compress(const char* data, size_t len, char* ptr) {
            size_t reslen = 0;
            snappy::RawCompress(data, len, ptr, &reslen);
            return reslen;
        }

        inline size_t Decompress(const char* data, size_t len, char* ptr, size_t) {
            size_t srclen = 0;
            if (!snappy::GetUncompressedLength(data, len, &srclen)
             || !snappy::RawUncompress(data, len, ptr))
                ythrow TDecompressorError();
            return srclen;
        }

        inline void InitFromStream(TInputStream*) const throw () {
        }

        static inline bool SaveIncompressibleChunks() throw () {
            return false;
        }
};

const char TSnappy::signature[] = "Snap";

DEF_COMPRESSOR(TSnappyCompress, TSnappy)
DEF_DECOMPRESSOR(TSnappyDecompress, TSnappy)

/*
 * QuickLZ
 */
class TQuickLZBase {
    public:
        static const char signature[];

        static inline size_t Hint(size_t len) throw () {
            return len + 500;
        }

        inline TQuickLZBase()
            : Table_(0)
        {
        }

        inline void Init(unsigned ver, unsigned lev, unsigned mod, unsigned type) {
            Table_ = LzqTable(ver, lev, mod);

            if (!Table_) {
                ythrow yexception() << "unsupported lzq stream(" <<  ver << ", " <<  lev << ", " <<  mod << ")";
            }

            const size_t size = Table_->Setting(3) + Table_->Setting(type);

            Mem_.Reset(::operator new(size));
            memset(Mem_.Get(), 0, size);
        }

        inline bool SaveIncompressibleChunks() const throw () {
            /*
             * we must save incompressible chunks as is after compressor run in streaming mode
             */

            return Table_->Setting(3);
        }

    protected:
        const TQuickLZMethods* Table_;
        THolder<void> Mem_;
};

const char TQuickLZBase::signature[] = "YLZQ";

class TQuickLZCompress: public TQuickLZBase {
    public:
        inline size_t Compress(const char* data, size_t len, char* ptr) {
            return Table_->Compress(data, ptr, len, (char*)Mem_.Get());
        }
};

class TQuickLZDecompress: public TQuickLZBase {
    public:
        inline size_t Decompress(const char* data, size_t /*len*/, char* ptr, size_t /*max*/) {
            return Table_->Decompress(data, ptr, (char*)Mem_.Get());
        }

        inline void InitFromStream(TInputStream* in) {
            const ui8 ver = ::GLoad<ui8>(in);
            const ui8 lev = ::GLoad<ui8>(in);
            const ui8 mod = ::GLoad<ui8>(in);

            Init(ver, lev, mod, 2);
        }
};

class TLzqCompress::TImpl: public TCompressorBase<TQuickLZCompress, TImpl> {
    public:
        inline TImpl(TOutputStream* out, ui16 blockSize, EVersion ver, unsigned level, EMode mode)
            : TCompressorBase<TQuickLZCompress, TImpl>(out, blockSize)
        {
            Init(ver, level, mode, 1);

            Save((ui8)ver);
            Save((ui8)level);
            Save((ui8)mode);
        }
};

TLzqCompress::TLzqCompress(TOutputStream* slave, ui16 blockSize, EVersion ver, unsigned level, EMode mode)
    : Impl_(new (TImpl::Hint(blockSize)) TImpl(slave, blockSize, ver, level, mode))
{
}

DEF_COMPRESSOR_COMMON(TLzqCompress, TQuickLZCompress)
DEF_DECOMPRESSOR(TLzqDecompress, TQuickLZDecompress)
