LIBRARY()

ENABLE(NOUTIL)
ENABLE(NO_COMPILER_WARNINGS)

SRCS(
    randtable.c
    crctable.c
    compress.c
    bzlib.c
    decompress.c
    blocksort.c
    huffman.c
)

END()
