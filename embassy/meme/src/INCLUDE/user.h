/* user.h */
/*
	User settable parameters
*/

#ifndef user_h
#define user_h

#define MSN	 24 		/* maximum length of sample name */
#define MAXALPH  27		/* maximum length of alphabet */
#define MAXG 101		/* maximum number of motifs + 1 */
#define MAXSITE 300             /* maximum length of a site; */
#define MAXSEQS 200000		/* maximum number of sequences */
#define MINSITES 1.5		/* minimum number of sites in valid motif */

#define MINCONS 0.2		/* Display 'X' as consensus if no letter f > */ 

/* minimum allowable motif width before shortening; 
   never make less than 2 or will crash! */
#define MIN_W 8
/* maximum allowable length before shortening */
#define MAX_W 57

#define MNAME 20		/* names of known motifs */
#define NMOTIFS MAXG		/* maximum number of known motifs */

#endif
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/user.h,v 1.1 2000/11/05 21:47:56 ajb Exp $ */
