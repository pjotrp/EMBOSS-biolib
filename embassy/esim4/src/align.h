#ifndef SCRIPTLIB_H
#define SCRIPTLIB_H
/* $Id: align.h,v 1.2 2004/08/05 16:02:32 rice Exp $ */
extern void align_path(int,int,int,int,int,edit_script**,edit_script**);
extern int  align_get_dist(int, int, int, int, int);
extern void Condense_script(edit_script *);
extern void Condense_both_Ends(edit_script **, edit_script **, edit_script **);
extern void S2A(edit_script *, int *, int);
extern void align_reverse(int *);
extern void IDISPLAY(sim4_uchar *, sim4_uchar *, int, int, int *, int, int,int,
		     Exon *);
extern void Free_script(edit_script *);
extern void Flip_script(struct edit_script **);

#ifdef AUXUTILS
extern void Reverse_script(edit_script *);
extern void Print_script(edit_script *head, int M, int N);
#endif

#endif /* SCRIPTLIB_H */
