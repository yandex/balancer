LIBRARY()

ENABLE(NOUTIL)

IF (NOT WIN32)
    SET(WERROR yes)
ENDIF (NOT WIN32)

SRCS(
    base.cpp
    constants.h
    cputimer.cpp
    parser.rl
    systime.cpp
)

IF (WIN32)
    SRCS(
        strptime.cpp
    )
ENDIF (WIN32)

END()
