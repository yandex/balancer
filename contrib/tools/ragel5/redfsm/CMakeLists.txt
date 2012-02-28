LIBRARY()

ENABLE(NOUTIL)
ENABLE(NO_COMPILER_WARNINGS)
ENABLE(NO_SHARED_LIB)

SRCDIR(
	contrib/tools/ragel5/aapl
)

PEERDIR(
    ADDINCL contrib/tools/ragel5/common
)

SRCS(
	gendata.cpp
	redfsm.cpp
	xmlparse.cpp
	xmlscan.cpp
	xmltags.cpp
)

END()
