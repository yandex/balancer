LIBRARY()

SET(PIREDIR ${ARCADIA_ROOT}/contrib/libs/pire/pire)

CFLAGS(-DPIRE_NO_CONFIG)

SRCDIR(
    ${PIREDIR}
)

##
## Evil hack
##

ADD_CUSTOM_COMMAND(
    OUTPUT ${BINDIR}/re_parser.cpp ${BINDIR}/re_parser.h
    COMMAND ${BYACC} ${BYACC_FLAGS} "${BINDIR}/re_parser.cpp" "${PIREDIR}/re_parser.ypp"
    MAIN_DEPENDENCY "${PIREDIR}/re_parser.ypp"
    COMMENT "Building ${BINDIR}/re_parser.cpp, ${BINDIR}/re_parser.h with byacc"
    DEPENDS byacc
)

SRCS(
    pire.h
    regexp.h

    classes.cpp
    encoding.cpp
    fsm.cpp
    scanner_io.cpp
    easy.cpp
    
    scanners/null.cpp

    extra/capture.cpp
    extra/count.cpp
    extra/glyphs.cpp

    re_lexer.cpp
    ${BINDIR}/re_parser.cpp
    extraencodings.cpp
)

SET_SOURCE_FILES_PROPERTIES(re_lexer.cpp
    PROPERTIES OBJECT_DEPENDS ${BINDIR}/re_parser.h
)

END()
