#ifndef prolog_h_as78d5f67sdf
#define prolog_h_as78d5f67sdf

#include "table.h"

#define RENAME(a)              DO_RENAME(QLZ_YVERSION, COMPRESSION_LEVEL, STREAMING_MODE, a)
#define DO_RENAME(a, b, c, d)  DO_RENAME2(a, b, c, d)
#define DO_RENAME2(a, b, c, d) yqlz_ ## a ## _ ## b ## _ ## c ## _ ## d

#endif

#define qlz_decompress        RENAME(decompress)
#define qlz_compress          RENAME(compress)
#define qlz_size_decompressed RENAME(size_decompressed)
#define qlz_size_compressed   RENAME(size_compressed)
#define qlz_get_setting       RENAME(get_setting)
#define qlz_table             RENAME(table)
#define qlz_compress_core     RENAME(compress_core)
#define qlz_decompress_core   RENAME(decompress_core)
#define hash_func             RENAME(hash_func)
#define fast_read             RENAME(fast_read)
#define fast_write            RENAME(fast_write)
