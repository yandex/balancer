LIBRARY()

ENABLE(NO_COMPILER_WARNINGS)

ENABLE(NOUTIL)

SRCS(
    snappy.cc
    snappy-c.cc
    snappy-stubs-internal.cc
    snappy-sinksource.cc
)

END()
