#ifndef SIM_PRNT_H
#define SIM_PRNT_H
/* $Id: prnt.h,v 1.1 2002/02/25 09:33:50 hgmp Exp $ */

typedef unsigned int edit_op_t; /* 32 bits */

#ifdef EMBASSY
#include "ajax.h"
void print_align_header(AjPSeq  seq1, AjPSeq  seq2, argv_scores_t *ds);
#else  /* EMBASSY */
void print_align_header(SEQ *seq1, SEQ *seq2, argv_scores_t *ds);
#endif /* EMBASSY */
void print_align(int score, uchar *seq1, uchar *seq2, int beg1, int end1, int beg2, int end2, int *S);


#endif
