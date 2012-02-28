LIBRARY()

TOOLDIR_EX(
    tools/archiver archiver
)

ENABLE(NO_COMPILER_WARNINGS)

IF (LINUX)
    CFLAGS(-DLUA_USE_LINUX)
ELSEIF (FREEBSD)
    CFLAGS(-DLUA_USE_LINUX)
ELSEIF (DARWIN)
    CFLAGS(-DLUA_USE_MACOSX)
ELSEIF (WIN32)
    CFLAGS(-DLUA_WIN)
ELSE (LINUX)
    CFLAGS(-DLUA_USE_POSIX -DLUA_USE_DLOPEN)
ENDIF (LINUX)

SRCDIR(
    contrib/libs/lua/lua-5.1.4/src
)

ADD_CUSTOM_COMMAND(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/common.inc

    COMMAND
        ${archiver} -x -q
            "${CMAKE_CURRENT_SOURCE_DIR}/common/stdlib.lua:/common"
            > ${CMAKE_CURRENT_BINARY_DIR}/common.inc

    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/common/stdlib.lua
        archiver

    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}

    COMMENT
        "Building ${CMAKE_CURRENT_BINARY_DIR}/common.inc"

    VERBATIM
)

SRCS(
    lib.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/common.inc
)

END()
