#ifndef SPLICE_H
#define SPLICE_H

/* "$Id: splice.h,v 1.1 2002/02/25 09:33:50 hgmp Exp $"; */

extern signal_t gt;
extern signal_t ct;
extern signal_t ag;
extern signal_t ac;


splice_t *new_splice(char,int,int,int,int,int,splice_t *);
void splice(uchar *in_seqx, int ls, int us, int le, int ue, 
            uchar *in_seqy, int ys, int ye, splice_t **g, splice_t **c, int ori);

#endif         /* SPLICE_H */ 
