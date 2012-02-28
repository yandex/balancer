PROGRAM()

ENABLE(NO_COMPILER_WARNINGS)

PEERDIR(
    contrib/libs/lua
)

SRCDIR(
    contrib/libs/lua/lua-5.1.4/src
)

SRCS(
    lua.cpp
)
SET(IDE_FOLDER "_Builders")

END()
