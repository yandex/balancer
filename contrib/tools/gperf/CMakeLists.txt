PROGRAM()

ENABLE(NOUTIL)
ENABLE(NO_COMPILER_WARNINGS)
ENABLE(NO_OPTIMIZE)
ENABLE(ADDINCLSELF)

SRCS(
    getopt.c
    getopt1.c
    getline.cpp
    hash.cpp
    keyword.cpp
    main.cpp
    options.cpp
    output.cpp
    search.cpp
    version.cpp
    hash-table.cpp
    bool-array.cpp
    input.cpp
    keyword-list.cpp
    positions.cpp
)

END()
