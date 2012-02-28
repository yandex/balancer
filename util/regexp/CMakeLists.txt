LIBRARY()

PEERDIR(
    contrib/libs/pcre
)

SRCS(
    glob_compat.h
    regexp.cpp
)

IF (WIN32)
    SRCS(
        glob.cpp
    )
ENDIF (WIN32)

END()
