PROGRAM()

ENABLE(NOUTIL)
ENABLE(NO_COMPILER_WARNINGS)
ENABLE(NO_OPTIMIZE)

SRCS(
	closure.c
	error.c
	graph.c
	lalr.c
	lr0.c
	main.c
	mkpar.c
	output.c
	reader.c
	skeleton.c
	symtab.c
	verbose.c
	warshall.c
)

END()
