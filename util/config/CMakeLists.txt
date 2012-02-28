LIBRARY()

ENABLE(WERROR)
ENABLE(NOUTIL)

SET_SVNREVISION()

SRCS(
    last_getopt.cpp
    last_getopt_support.h
    opt.cpp
    opt2.cpp
    posix_getopt.cpp
    conf.cpp
    ygetopt.cpp
)

END()
