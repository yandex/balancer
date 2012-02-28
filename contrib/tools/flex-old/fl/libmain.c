/* libmain - flex run-time support library "main" function */

/* $Header: /opt/vlysenkov/CVSROOT/arcadia/contrib/tools/flex-old/fl/libmain.c,v 1.2 2007-11-30 02:28:15 pg Exp $ */

extern int yylex();

int main( argc, argv )
int argc;
char *argv[];
	{
	while ( yylex() != 0 )
		;

	return 0;
	}
