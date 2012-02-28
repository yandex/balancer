PROGRAM()

ENABLE(NOUTIL)
ENABLE(NO_COMPILER_WARNINGS)
ENABLE(NO_OPTIMIZE)

SRCDIR(
	contrib/tools/ragel5/aapl
)

PEERDIR(
    ADDINCL contrib/tools/ragel5/common
    ADDINCL contrib/tools/ragel5/redfsm
)

SRCS(
	fflatcodegen.cpp
	fgotocodegen.cpp
	flatcodegen.cpp
	fsmcodegen.cpp
	ftabcodegen.cpp
	gotocodegen.cpp
	ipgotocodegen.cpp
	main.cpp
	splitcodegen.cpp
	tabcodegen.cpp
)

END()
