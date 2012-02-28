#pragma once

#include "ios.h"

#include <util/generic/ptr.h>
#include <util/generic/yexception.h>

/// All *lz* compressors compress blocks. T*Compress.Write method splits input data into blocks, compressees each block
/// and then writes each compressed block to the underlying output stream. Thus T*Compress classes are not buffered.
/// MaxBlockSize parameter specified max allowed block size.
/// see http://altdevblogaday.com/2011/04/22/survey-of-fast-compression-algorithms-part-1/ for some comparison

struct TDecompressorError: public yexception {
};

/// @addtogroup Streams_Archs
/// @{

/*
 * Lz4 compression
 * http://code.google.com/p/lz4/
 */
class TLz4Compress: public TOutputStream {
    public:
        TLz4Compress(TOutputStream* slave, ui16 maxBlockSize = 1 << 15);
        virtual ~TLz4Compress() throw ();

    private:
        virtual void DoWrite(const void* buf, size_t len);
        virtual void DoFlush();
        virtual void DoFinish();

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

class TLz4Decompress: public TInputStream {
    public:
        TLz4Decompress(TInputStream* slave);
        virtual ~TLz4Decompress() throw ();

    private:
        virtual size_t DoRead(void* buf, size_t len);

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

/*
 * Snappy compression
 * http://code.google.com/p/snappy/
 */
class TSnappyCompress: public TOutputStream {
    public:
        TSnappyCompress(TOutputStream* slave, ui16 maxBlockSize = 1 << 15);
        virtual ~TSnappyCompress() throw ();

    private:
        virtual void DoWrite(const void* buf, size_t len);
        virtual void DoFlush();
        virtual void DoFinish();

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

class TSnappyDecompress: public TInputStream {
    public:
        TSnappyDecompress(TInputStream* slave);
        virtual ~TSnappyDecompress() throw ();

    private:
        virtual size_t DoRead(void* buf, size_t len);

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

/*
 * MiniLZO compress/decompress engines
 */
class TLzoCompress: public TOutputStream {
    public:
        TLzoCompress(TOutputStream* slave, ui16 maxBlockSize = 1 << 15);
        virtual ~TLzoCompress() throw ();

    private:
        virtual void DoWrite(const void* buf, size_t len);
        virtual void DoFlush();
        virtual void DoFinish();

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

class TLzoDecompress: public TInputStream {
    public:
        TLzoDecompress(TInputStream* slave);
        virtual ~TLzoDecompress() throw ();

    private:
        virtual size_t DoRead(void* buf, size_t len);

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

/*
 * FastLZ compress/decompress engines
 */
class TLzfCompress: public TOutputStream {
    public:
        TLzfCompress(TOutputStream* slave, ui16 maxBlockSize = 1 << 15);
        virtual ~TLzfCompress() throw ();

    private:
        virtual void DoWrite(const void* buf, size_t len);
        virtual void DoFlush();
        virtual void DoFinish();

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

class TLzfDecompress: public TInputStream {
    public:
        TLzfDecompress(TInputStream* slave);
        virtual ~TLzfDecompress() throw ();

    private:
        virtual size_t DoRead(void* buf, size_t len);

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

/*
 * QuickLZ compress/decompress engines
 */
class TLzqCompress: public TOutputStream {
    public:
        enum EVersion {
            V_1_31 = 0,
            V_1_40 = 1
        };

        /*
         * streaming mode - actually, backlog size
         */
        enum EMode {
            M_0       = 0,
            M_100000  = 1,
            M_1000000 = 2
        };

        TLzqCompress(TOutputStream* slave, ui16 maxBlockSize = 1 << 15,
            EVersion ver = V_1_31,
            unsigned level = 0,
            EMode mode = M_0);
        virtual ~TLzqCompress() throw ();

    private:
        virtual void DoWrite(const void* buf, size_t len);
        virtual void DoFlush();
        virtual void DoFinish();

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

class TLzqDecompress: public TInputStream {
    public:
        TLzqDecompress(TInputStream* slave);
        virtual ~TLzqDecompress() throw ();

    private:
        virtual size_t DoRead(void* buf, size_t len);

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

/// @}
