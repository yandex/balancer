LIBRARY(yutil)

ENABLE(NOUTIL)

PEERDIR(
    contrib/libs/libbz2
    contrib/libs/libiconv
    contrib/libs/fastlz
    contrib/libs/minilzo
    contrib/libs/quicklz
    contrib/libs/zlib

    util/charset
    util/config
    util/datetime
    util/digest
    util/folder
    util/generic
    util/memory
    util/network
    util/random
    util/stream
    util/string
    util/system
    util/thread
    util/sorter
    util/logger
    util/green
    util/uri
)

IF (NOT WIN32)
    SET(WERROR yes)
ENDIF (NOT WIN32)

IF (SUN)
    SET_APPEND(PICFLAGS -fPIC)
ENDIF (SUN)

SRCS(
    fileptr.cpp
    httpcodes.cpp
    str_hash.cpp
    longs.cpp
    ffb.cpp
    mbitmap.cpp
    ysafeptr.cpp
    yarchive.cpp
    ysaveload.cpp
    httpdate.cpp
    array2d.h
    array2d_writer.h
    atomizer.h
    autoarray.h
    cont_init.h
    fgood.h
    fput.h
    httpreqdata.h
    save_stl.h
    spars_ar.h
    static_hash.h
    static_hash_map.h
    str_map.h
    str_stl.h
)

END()
