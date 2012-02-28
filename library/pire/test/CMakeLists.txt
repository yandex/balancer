PROGRAM(pire_test)

SET(PIRETESTSDIR ${ARCADIA_ROOT}/contrib/libs/pire/tests)
SET(PIREDIR ${ARCADIA_ROOT}/contrib/libs/pire/pire)

CXXFLAGS(-I${PIRETESTSDIR} -I${PIREDIR} -DPIRE_NO_CONFIG)

PEERDIR(library/pire)

SRCDIR(
    library/unittest
    ${PIRETESTSDIR}
)

ADD_CUSTOM_COMMAND(
    OUTPUT ${BINDIR}/inline_ut.cpp
    COMMAND ${ARCADIA_ROOT}/library/pire/inline/pire_inline -o "${BINDIR}/inline_ut.cpp" "${PIRETESTSDIR}/inline_ut.cpp"
    MAIN_DEPENDENCY "${PIRETESTSDIR}/inline_ut.cpp"
    COMMENT "Embedding regexps into ${BINDIR}/inline_ut.cpp"
    DEPENDS ${ARCADIA_ROOT}/library/pire/inline/pire_inline
)

SRCS(
    utmain.cpp
    pire_ut.cpp
    capture_ut.cpp
    count_ut.cpp
    ${BINDIR}/inline_ut.cpp
    easy_ut.cpp
    regexp_ut.cpp
)

END()

