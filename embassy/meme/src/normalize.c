/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
#include "meme.h"

/***********************************************************************/
/*
  smooth

  Normalize so that no local region w wide has z_ij sum of > 1.0.
  Winner-take-all: the largest value of z_ij is never reduced.

  Returns the total expected number of sites of all motifs.
*/ 
/***********************************************************************/
extern double smooth(
  int w,				/* width to smooth over */
  MODEL *model,				/* the model */
  DATASET *dataset			/* the dataset */
)
{
  int i, j, k;
  int n_samples = dataset->n_samples;
  SAMPLE **samples = dataset->samples;
  double p_sum = 0.0;

  model++;				/* avoid compiler complaints */

  for (i=0; i<n_samples; i++) {			/* sequence */
    int ioff;
    SAMPLE *sample = samples[i];		/* sample i */
    int lseq = sample->length;
    double **z= sample->z;			/* z */
    double *z1i = z[1];				/* (only) motif z */
    int max_o = lseq - w + 1;			/* largest possible offset */

    /* normalize adjacent windows of length w, then shift and repeat */
    for (ioff = 0; ioff < MIN(w, max_o); ioff+=2) {	/* window start */
      for (j=ioff; j<max_o; j += w) {		/* adjacent windows */
	double local_z = 0.0;
        double max_z = 0;			/* find largest z_ij */
        int max_k = 0;
        int last_k = MIN(j+w, max_o);
	for (k=j; k<last_k; k++) {		/* position */
          double z = z1i[k];
	  local_z += z;				/* compute local motif z sum */
	  if (z > max_z) {		
	    max_z = z;				/* largest z in window */
	    max_k = k;				/* position with largest z */
	  }
	}
	/* normalize if necessary; leave largest z in window unchanged */
	if (local_z > 1.0) {
          double scale = (1 - max_z) / (local_z - max_z);
	  for (k=j; k<last_k; k++) {		/* position */
	    if (k != max_k) {
	      z1i[k] *= scale;			/* normalize motif z */
	    } 
	  }
	}
      }
    }

    /* calculate p_sum */
    for (j=0; j < max_o; j++) {
      p_sum += z1i[j];
    }

  } /* n_samples loop */
  return p_sum;
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/normalize.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
