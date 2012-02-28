LIBRARY()

ENABLE(NOUTIL)

PEERDIR(
    contrib/libs/lzmasdk
    contrib/libs/zlib
    contrib/libs/lz4
    contrib/libs/snappy
    util/network
)

IF (NOT WIN32)
    SET(WERROR yes)
ENDIF (NOT WIN32)

SRCS(
    buffer.cpp
    buffered.cpp
    bzip2.cpp
    chunk.cpp
    debug.cpp
    file.cpp
    glue.cpp
    helpers.cpp
    http.cpp
    input.cpp
    lz.cpp
    lzma.cpp
    mem.cpp
    multi.cpp
    null.cpp
    output.cpp
    pipe.cpp
    str.cpp
    tee.cpp
    zerocopy.cpp
    zlib.cpp
    printf.cpp
    format.cpp
    
    aligned.h
    base.h
    ios.h
    length.h
    tempbuf.h
    tokenizer.h
    walk.h
)

END()
