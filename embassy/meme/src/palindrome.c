/* palindrome.c */

/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/

#include "meme.h"

#define theta_pal(i,j) theta_ref(theta_pal, i, j)

/***********************************************************************/
/*
	palindrome

	Apply the DNA palindrome constraints to a motif.
	Puts constrained motif in theta_pal.
*/
/***********************************************************************/
extern void palindrome(
  THETA theta, 			/* motif */
  THETA theta_pal,		/* constrained motif */
  int w, 			/* width */
  int alength			/* length of alphabet */
)
{
  int i, j;

  if (alength != 4) {
    printf("Non-DNA dataset used with palindrome switch!\n");
    exit(1);
  }
  for (i=0; i<=w/2; i++) {		/* create the palindrome */
    for (j=0; j<alength; j++) {
      int ii = w-i-1;
      int jj = dna_comp(j);
      theta_pal(i,j) = theta_pal(ii,jj) = (theta(i,j) + theta(ii,jj))/2;
    }
  }
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/palindrome.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
