/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* tcm.c */
/*	
	EM algorithm.

	Two component mixture model. 
*/
	
#include "meme.h"

/**********************************************************************/
/*
	tcm_e_step	

	Do the E step of EM.

	Estimate the expectation of model 1 for each position in the data.
	In other words, calculate E[z_ij] in z.

	Updates z.  

	Returns log pr(X | theta, lambda).

	Note: clobbers theta.

	Time: O(n_samples*lseq*w)
*/
/**********************************************************************/
double tcm_e_step(
  MODEL *model,			/* the model */
  DATASET *dataset 		/* the dataset */
)
{
  int i, j, k, ii;
  THETA theta0 = model->theta[0];	/* background theta */
  THETA theta1 = model->theta[1];	/* motif theta */
  int w = model->w[1];			/* motif width */
  double lambda = model->lambda[1];	/* lambda of tcm model */
  double log_lambda = LOG(lambda);	
  double log_1mlambda = LOG(1-lambda);
  int alength = dataset->alength;
  int n_samples = dataset->n_samples;
  double logpX;				/* log likelihood */
  /*double scale = (float) dataset->total_res / (w * ps(dataset,w)); */

  /* E step */

  convert_theta_to_log(model, alength);

  /* calculate all the posterior offset probabilities */
  logpX = 0;
  for (i=0; i < n_samples; i++) {		/* sequence */
    SAMPLE *s = dataset->samples[i];
    int lseq = s->length;
    char *res = s->res;				/* integer sequence */
    double *not_o = s->not_o;			/* not overlapp a prv site */
    double *z1i = s->z[1];			/* motif z_i */
    /*double *z0i = s->z[0];*/			/* background z_i */

    /*
	Estimate E(z_i1 | X, theta, lambda)
	as in ar85 page 69.
    */
    for (j=0; j <= lseq - w; j++) {		/* site start */
      double log_pXitheta0 = log_1mlambda;	/* log Pr(X_i | theta0) */
      double log_pXitheta1 = log_lambda;	/* log Pr(X_i | theta1) */
      double log_pXitheta;			/* log Pr(X_i | theta) */

      for (k=j, ii=0; k < j+w; k++, ii++) {	/* position in sequence */
        int r = res[k];
        log_pXitheta0 += logtheta0(0, r);	/* background model */
        log_pXitheta1 += logtheta1(ii, r);	/* motif model */
      }
      log_pXitheta = LOG_SUM(log_pXitheta0, log_pXitheta1);
      logpX += log_pXitheta;

      /* calculate E[z_ij] = 
         P(Z=1 | X, theta, v) = P(Z=1 | X, theta, v_ij=1) P(v_ij=1) */
      z1i[j] = exp(log_pXitheta1 - log_pXitheta) * not_o[j];
      /*z0i[j] = 1 - z1i[j];*/
    }
    for (j=lseq-w+1; j < lseq; j++) {		/* tail of sequence */
      z1i[j] = 0;
    }
    s->dz[0] = 1;				/* fix later */
    s->dz[1] = 0;				/* fix later */
    s->dz[2] = 0;				/* fix later */
    s->dz[3] = 0;				/* fix later */
  }

  /* normalize so no window of size w has z_i1 which sum to
	greater than 1.0 for first component
  */
  (void) smooth(w, model, dataset);

  /*return (scale * logpX/log(2.0));*/
  return (logpX/log(2.0));
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/tcm.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
