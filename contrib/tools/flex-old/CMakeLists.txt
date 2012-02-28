PROGRAM(flex)

ENABLE(NOUTIL)
ENABLE(NO_COMPILER_WARNINGS)
ENABLE(NO_OPTIMIZE)

PEERDIR(
	contrib/tools/flex-old/fl
)

SRCS(
	ccl.c
	dfa.c
	ecs.c
	gen.c
	main.c
	misc.c
	nfa.c
	skel.c
	sym.c
	tblcmp.c
	yylex.c
	scan.c
	parse.c
)

END()
