#include "table.h"

#define STR(a) STR2(a)
#define STR2(a) STR3(a)
#define STR3(a) #a

struct TQuickLZMethods qlz_table = {
    qlz_size_decompressed,
    qlz_size_compressed,
    qlz_decompress,
    qlz_compress,
    qlz_get_setting,
    STR(QLZ_YVERSION) "-" STR(COMPRESSION_LEVEL) "-" STR(STREAMING_MODE)
};
