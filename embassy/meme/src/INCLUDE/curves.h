/***********************************************************************
*                                                                      *
*       MEME++                                                         *
*       Copyright 1995, The Regents of the University of California    *
*       Author: Timothy L. Bailey                                      *
*                                                                      *
***********************************************************************/

/* curves.h */

#ifndef CURVES_H
#define CURVES_H

#define 	MAXPARMS	10

/* the type of the curve function */
typedef char (*FTYPE( 	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode));

extern FTYPE *get_curve_type(
  int mode,				/* type of curve */
  int *nparms				/* number of parameters */
);

extern char *negexp( 	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode);

extern char *logcurve(	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode);

extern char *logcurve2(	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode);

extern char *logexp( 	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode);

extern char *loglog(	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode);

extern char *negexp2( 	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode);

extern char *invsqrt(	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode);

extern char *logcurve3(	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode);

extern char *logcurve4(	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode);

extern char *gcurve(	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode);

extern char *rootlog(	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode);

extern char *gsdcurve(	float	X,	float	A[],
		float *YFit,	float	dYdA[],
		int mode);
#endif

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/curves.h,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
