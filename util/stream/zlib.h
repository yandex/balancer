#pragma once

#include "ios.h"

#include <util/system/defaults.h>
#include <util/generic/ptr.h>
#include <util/generic/yexception.h>

/// @addtogroup Streams_Archs
/// @{

struct TZLibError: public yexception {
};

struct TZLibCompressorError: public TZLibError {
};

struct TZLibDecompressorError: public TZLibError {
};

#define ZLIB_BUF_LEN (8 * 1024)

namespace ZLib {
    enum StreamType {
        //for decompress only
        Auto = 0,
        //for compress && decompress
        ZLib = 1,
        GZip = 2,
        Raw  = 3
    };
}

class TZLibDecompress: public TInputStream {
    public:
        TZLibDecompress(TInputStream* input, ZLib::StreamType type = ZLib::Auto, size_t buflen = ZLIB_BUF_LEN);
        TZLibDecompress(IZeroCopyInput* input, ZLib::StreamType type = ZLib::Auto);
        virtual ~TZLibDecompress() throw ();

    private:
        virtual size_t DoRead(void* buf, size_t size);

    public:
        class TImpl;
        THolder<TImpl> Impl_;
};

class TZLibCompress: public TOutputStream {
    public:
        TZLibCompress(TOutputStream* out,
            ZLib::StreamType type = ZLib::ZLib,
            size_t compression_level = 6,
            size_t buflen = ZLIB_BUF_LEN);
        virtual ~TZLibCompress() throw ();

    private:
        virtual void DoWrite(const void* buf, size_t size);
        virtual void DoFlush();
        virtual void DoFinish();

    public:
        class TImpl;
        THolder<TImpl> Impl_;
};

/// buffered version, supports efficient ReadLine calls and similar "reading in small pieces" usage patterns
class TBufferedZLibDecompress: public TBuffered<TZLibDecompress> {
    public:
        template <class T>
        inline TBufferedZLibDecompress(T* in, ZLib::StreamType type = ZLib::Auto, size_t buf = 1 << 13)
            : TBuffered<TZLibDecompress>(buf, in, type)
        {
        }

        virtual ~TBufferedZLibDecompress() throw () {
        }
};

typedef TBufferedZLibDecompress TZDecompress;

/// @}

/*
 * undef some shit...
 */
#undef ZLIB_BUF_LEN
