LIBRARY(uri)

ENABLE(NOUTIL)

PEERDIR(
    util/charset
    util/generic
    util/memory
    util/stream
    util/string
    util/system
)

IF (NOT WIN32)
    SET(WERROR yes)
ENDIF (NOT WIN32)

IF (SUN)
    SET_APPEND(PICFLAGS -fPIC)
ENDIF (SUN)

SRCS(
    assign.cpp
    common.cpp
    encode.cpp
    other.cpp
    parse.cpp
    uri.cpp
)

SRCS(
    encode.rl
    parse.rl
)

set_source_files_properties(encode.rl PROPERTIES
    RL6 TRUE
    GENERATE_SOURCE encode.rl.gen.cpp
)
set_source_files_properties(parse.rl PROPERTIES
    RL6 TRUE
    GENERATE_SOURCE parse.rl.gen.cpp
)

END()
