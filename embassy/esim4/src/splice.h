#ifndef SPLICE_H
#define SPLICE_H

/* "$Id: splice.h,v 1.2 2002/05/10 16:10:17 rice Exp $"; */

extern sim_signal_t gt;
extern sim_signal_t ct;
extern sim_signal_t ag;
extern sim_signal_t ac;


splice_t *new_splice(char,int,int,int,int,int,splice_t *);
void splice(uchar *in_seqx, int ls, int us, int le, int ue, 
            uchar *in_seqy, int ys, int ye, splice_t **g, splice_t **c, int ori);

#endif         /* SPLICE_H */ 
