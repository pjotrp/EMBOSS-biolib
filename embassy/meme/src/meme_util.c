/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1995, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* meme_util.c */

#include <meme.h>

/**********************************************************************/
/*
  copy_theta

  Copy the theta matrix to another array.

*/
/**********************************************************************/
extern void copy_theta(
  THETA s,	 		/* source */
  THETA d,			/* destination */
  int w,			/* width of motif */
  int alength			/* length of alphabet */
)
{ 
  int i, j;
  for (i = 0; i < w; i++) {		/* col in motif */
    for (j = 0; j < alength; j++) {	/* row in motif */
        theta_ref(d, i, j) = theta_ref(s, i, j);
    }
  }
}

/**********************************************************************/
/*
	get_not_o

	Calculate the probability that each possible site
     	start does not overlap a previously found site;
     	this is taken to be the minimum of all the probabilities
     	of a site starting within the possible site
*/
/**********************************************************************/
extern void get_not_o(
  DATASET *dataset,			/* the dataset */
  int w,				/* width of motif */
  BOOLEAN get_log			/* compute log_not_o if true */
)
{
  int i,j,k;
  int n_samples = dataset->n_samples;
  SAMPLE **samples = dataset->samples;

  for (i=0; i < n_samples; i++){	/* sequence */
    SAMPLE *s = samples[i];
    int lseq = s->length;
    double *weights = s->weights;       /* prb not in a previous site */
    double *not_o = s->not_o;		/* prb not overlapping a site */
    int *log_not_o = s->log_not_o;	/* prb not overlapping a site */

    for (j=0; j <= lseq - w; j++) { 	/* site start */
      not_o[j] = 1.0;			/* assume not overlapping */
      for (k=j; k < j+w; k++) { 	/* position in sequence */
        if (weights[k] < not_o[j]) not_o[j] = weights[k];
      }
      if (get_log) log_not_o[j] = INT_LOG(not_o[j]);
    }
    for (j=lseq-w+1; j < lseq ; j++) { 	/* beyond possible site starts */
      not_o[j] = 1;
      if (get_log) log_not_o[j] = 0;
    }
  }
}

/**********************************************************************/
/*
	create_model

	Create a model structure.
*/
/**********************************************************************/
extern MODEL *create_model(
  MTYPE mtype,				/* type of model */
  BOOLEAN *strands,			/* strands to use */
  P_POINT *p_point,			/* user input starts */
  int nmotifs,				/* number of motifs to learn */
  int alength				/* length of alphabet */
)
{
  int i; 
  int nstrands = 0;
  MODEL *model = (MODEL *) malloc(sizeof(MODEL));

  model->mtype = mtype;
  for (i=0; i<4; i++) {
    model->d[i] = strands[i];
    if (strands[i]) nstrands++;
  }
  model->nstrands = nstrands;

  switch (mtype) {
    case Oops:
    case Zoops:
    case Tcm:
      model->c = 2;			/* two components */
      create_2array(model->theta[0], double, 1, alength);
      create_2array(model->theta[1], double, MAXSITE, alength);
      create_2array(model->obs[0], double, 1, alength);
      create_2array(model->obs[1], double, MAXSITE, alength);
      break;
  }
  model->iter = 0;
  return model;
}

/**********************************************************************/
/*
  copy_model

  Copy a model structure.
*/
/**********************************************************************/
extern void copy_model(
  MODEL *m1, 				/* source */
  MODEL *m2,				/* destination */
  int alength				/* length of alphabet */
)
{
  int i;

  m2->mtype = m1->mtype;
  m2->c = m1->c; 
  for (i=0; i<m1->c; i++) {
    m2->w[i] = m1->w[i];
    copy_theta(m1->theta[i], m2->theta[i], m1->w[i], alength);
    copy_theta(m1->obs[i], m2->obs[i], m1->w[i], alength);
    m2->lambda[i] = m1->lambda[i];
    m2->rel[i] = m1->rel[i];
  }
  m2->pal = m1->pal;
  strcpy(m2->cons, m1->cons); 
  m2->nstrands = m1->nstrands; 
  for (i=0; i<4; i++) {
    m2->sigma[i] = m1->sigma[i];
    m2->d[i] = m1->d[i];
  }
  m2->e_ll_0 = m1->e_ll_0;
  m2->e_ll = m1->e_ll; 
  m2->ll = m1->ll; 
  m2->sig = m1->sig; 
  m2->lrt = m1->lrt; 
  m2->bon = m1->bon; 
  m2->root = m1->root; 
  m2->pvalue = m1->pvalue;
  m2->imotif = m1->imotif; 
#ifdef PARALLEL
  strcpy(m2->cons0, m1->cons0); 
  m2->ID = m1->ID; 
#endif
  m2->iter = m1->iter; 
}

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/meme_util.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
