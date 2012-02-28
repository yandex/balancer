LIBRARY()

ENABLE(NOUTIL)
ENABLE(NO_COMPILER_WARNINGS)

SRCS(
    genaliases.c
    genaliases2.c
    #genflags.c
    #gentranslit.c
    iconv.c
    #relocatable.c
)

END()
