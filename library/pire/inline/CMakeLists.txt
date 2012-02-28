PROGRAM(pire_inline)

SET(PIREDIR ${ARCADIA_ROOT}/contrib/libs/pire/pire)

CXXFLAGS(-I${PIREDIR})
CXXFLAGS(-I${ARCADIA_ROOT}/library/pire)
CXXFLAGS(-DPIRE_NO_CONFIG)


PEERDIR( 
    util
    library/pire
)

SRCDIR(
    ${PIREDIR}
)

SRCS(
    inline.lpp
)

END()
