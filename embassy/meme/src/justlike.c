/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* justlike.c */
/*	
	Set the z-matrix to 1 for each motif start; to 0 otherwise.
	Motifs have been read in from a .motif file.
*/

#include "meme.h"

/**********************************************************************/
/*
	like_e_step

*/
/**********************************************************************/
extern double like_e_step(
  MODEL *model,                 /* the model */
  DATASET *dataset              /* the dataset */
)
{
  int i, j, k;
  int n_samples = dataset->n_samples;
  SAMPLE **samples = dataset->samples;
  int nsites = 0;				/* number of sites read in */
  BOOLEAN *dir = model->d;			/* strand direction */
  int imotif = model->imotif;			/* motif number */
  MOTIF motif = dataset->motifs[imotif-1];	/* known motif */
  int w = motif.width;				/* width of known motif */

  /* set all z's to 0 except motif starts */
  for (i=0; i < n_samples; i++) {		/* sequence */
    SAMPLE *s = samples[i];
    char *sample_name = s->sample_name;
    int lseq = s->length;
    for (j=0; j <= lseq - w; j++) {		/* offset */
      if (hash_lookup(sample_name, j+1, motif.ht)) {
        s->z[1][j] = 1;				/* motif start */
        nsites++;
      } else {
        s->z[1][j] = 0;				/* not motif start */
      }
    }
    for (k=0; k<3; k++) {
      if (!dir[k]) {
	s->dz[k] = 0;
      } else {
	s->dz[k] = 1;
      }
    }
  }

  /* calculate lambda for motif */
  model->lambda[1] = (double) nsites/ps(dataset, w); 

  return 0; 
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/justlike.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
