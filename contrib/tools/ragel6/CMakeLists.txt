PROGRAM(ragel6)

ENABLE(NOUTIL)
ENABLE(NO_COMPILER_WARNINGS)
ENABLE(NO_OPTIMIZE)

SRCDIR(
	contrib/tools/ragel5/aapl
)

SRCS(
    cdcodegen.cpp
    cdftable.cpp
    cdtable.cpp
    csfgoto.cpp
    csipgoto.cpp
    fsmap.cpp
    fsmmin.cpp
    javacodegen.cpp
    rbxgoto.cpp
    rubycodegen.cpp
    rubytable.cpp
    cdfflat.cpp
    cdgoto.cpp
    common.cpp
    csflat.cpp
    cssplit.cpp
    fsmattach.cpp
    fsmstate.cpp
    main.cpp
    redfsm.cpp
    rubyfflat.cpp
    xmlcodegen.cpp
    cdfgoto.cpp
    cdipgoto.cpp
    cscodegen.cpp
    csftable.cpp
    cstable.cpp
    fsmbase.cpp
    gendata.cpp
    parsedata.cpp
    rlparse.cpp
    rubyflat.cpp
    cdflat.cpp
    cdsplit.cpp
    csfflat.cpp
    csgoto.cpp
    dotcodegen.cpp
    fsmgraph.cpp
    inputdata.cpp
    parsetree.cpp
    rlscan.cpp
    rubyftable.cpp
)

ENABLE(USE_LF_ALLOCATOR)

END()
