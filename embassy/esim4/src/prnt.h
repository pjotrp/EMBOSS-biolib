#ifndef SIM_PRNT_H
#define SIM_PRNT_H
/* $Id: prnt.h,v 1.2 2004/08/05 16:02:32 rice Exp $ */

typedef unsigned int edit_op_t; /* 32 bits */

#ifdef EMBASSY
#include "ajax.h"
void print_align_header(AjPSeq  seq1, AjPSeq  seq2, argv_scores_t *ds);
#else  /* EMBASSY */
void print_align_header(SEQ *seq1, SEQ *seq2, argv_scores_t *ds);
#endif /* EMBASSY */
void print_align(int score, sim4_uchar *seq1, sim4_uchar *seq2,
		 int beg1, int end1, int beg2, int end2, int *S);


#endif
