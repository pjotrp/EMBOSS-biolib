/***********************************************************************
*                                                                      *
*       MEME++                                                         *
*       Copyright 1995, The Regents of the University of California    *
*       Author: Timothy L. Bailey                                      *
*                                                                      *
***********************************************************************/
 
/* mrq.h */
 
#ifndef REGRESS_H 
#define REGRESS_H

extern double regress(
  int n,                        /* number of points */
  double *x,                    /* x values */
  double *y,                    /* y values */
  double *m,                    /* slope */
  double *b                     /* y intercept */
);

#endif
 
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/regress.h,v 1.1 2000/11/05 21:47:56 ajb Exp $ */
