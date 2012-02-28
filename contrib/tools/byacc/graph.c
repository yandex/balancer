/* $Id: graph.c,v 1.1 2007-11-29 19:12:16 pg Exp $ */

#include "defs.h"

static void graph_state(int stateno);
static void graph_LA(int ruleno);

static unsigned int larno;

void graph(void)
{
    register int i;
    register int j;
    register shifts *sp;
    register int sn;
    register int as;

    if (!gflag)
	return;

    for (i = 0; i < nstates; ++i)
    {
	closure(state_table[i]->items, state_table[i]->nitems);
	graph_state(i);
    }

    fprintf(graph_file, "\n\n");
    for (i = 0; i < nstates; ++i)
    {

	sp = shift_table[i];
	if (sp)
	    for (j = 0; j < sp->nshifts; ++j)
	    {
		sn = sp->shift[j];
		as = accessing_symbol[sn];
		fprintf(graph_file,
			"\tq%d -> q%d [label=\"%s\"];\n",
			i, sn, symbol_pname[as]);
	    }
    }

    fprintf(graph_file, "}\n");

    for (i = 0; i < nsyms; ++i)
	FREE(symbol_pname[i]);
    FREE(symbol_pname);
}

static void graph_state(int stateno)
{
    register short *isp;
    register int rule;
    register short *sp;
    register short *sp1;

    larno = lookaheads[stateno];
    fprintf(graph_file, "\n\tq%d [label=\"%d:\\l", stateno, stateno);

    for (isp = itemset; isp < itemsetend; isp++)
    {
	sp1 = sp = ritem + *isp;

	while (*sp >= 0)
	    ++sp;
	rule = -(*sp);
	fprintf(graph_file, "  %s -> ", symbol_pname[rlhs[rule]]);

	for (sp = ritem + rrhs[rule]; sp < sp1; sp++)
	    fprintf(graph_file, "%s ", symbol_pname[*sp]);

	putc('.', graph_file);

	while (*sp >= 0)
	{
	    fprintf(graph_file, " %s", symbol_pname[*sp]);
	    sp++;
	}

	if (*sp1 < 0)
	    graph_LA(-*sp1);

	fprintf(graph_file, "\\l");
    }
    fprintf(graph_file, "\"];");
}

static void graph_LA(int ruleno)
{
    register int i;
    register int tokensetsize;
    register unsigned *rowp;

    tokensetsize = WORDSIZE(ntokens);

    if (ruleno == LAruleno[larno])
    {
	rowp = LA + larno * tokensetsize;

	fprintf(graph_file, " { ");
	for (i = ntokens - 1; i >= 0; i--)
	{
	    if (BIT(rowp, i))
		fprintf(graph_file, "%s ", symbol_pname[i]);
	}
	fprintf(graph_file, "}");
	++larno;
    }
}
