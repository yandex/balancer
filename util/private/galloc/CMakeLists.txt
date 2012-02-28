LIBRARY()

ENABLE(NOUTIL)
ENABLE(NO_COMPILER_WARNINGS)

SRCDIR(
    ADDINCL util/private/jalloc
)

SRCS(
    galloc.cpp
    hack.cpp
)

END()
