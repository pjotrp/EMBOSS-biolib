/***********************************************************************
*                                                                      *
*       MEME++                                                         *
*       Copyright 1994, The Regents of the University of California    *
*       Author: Timothy L. Bailey                                      *
*                                                                      *
***********************************************************************/
/* logs.h */

#ifndef logs_h
# define logs_h

/* table of log(x) for 0 < x <= 1 */

#define log_precision 1.0e5
EXTERN double log_table[(int) log_precision + 2];	/* leave room for n+1 */

/* log lookup function; use linear interpolation */
#define LOGL_Y(x) 	((x) * log_precision)
#define LOGL_I(x)  	((int) LOGL_Y(x))
#define LOGL_LOW(x) 	(log_table[LOGL_I(x)])
#define LOGL_HI(x) 	(log_table[LOGL_I(x) + 1])
#define LOGL(x) 	(LOGL_LOW(x) + (LOGL_Y(x) - LOGL_I(x)) * \
				(LOGL_HI(x) - LOGL_LOW(x)))

/* function prototypes */

extern void init_log(void);

#endif

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/logs.h,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
