PROGRAM(ragel5)

ENABLE(NOUTIL)
ENABLE(NO_COMPILER_WARNINGS)
ENABLE(NO_OPTIMIZE)

SRCDIR(
	contrib/tools/ragel5/aapl
)

PEERDIR(
    ADDINCL contrib/tools/ragel5/common
)

SRCS(
	fsmap.cpp
	fsmattach.cpp
	fsmbase.cpp
	fsmgraph.cpp
	fsmmin.cpp
	fsmstate.cpp
	main.cpp
	parsedata.cpp
	parsetree.cpp
	rlparse.cpp
	rlscan.cpp
	xmlcodegen.cpp
)

END()
