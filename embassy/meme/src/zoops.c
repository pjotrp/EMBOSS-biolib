/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* zoops.c */
/*	

	EM algorithm for ZOOPS model.
	Zero or one occurrence per sequence.

	The funny way of computing array subscripts is for speed.
*/

#include "meme.h"

/**********************************************************************/
/*
	zoops_e_step

	Do the E step of EM.

	Calculate the probability of the site starting in position
	k for 0 <= k <= l - j using Bayes rule for each sequence assuming
	that each sequence has zero or one occurrences of the motif.

	Updates z.

	Returns log pr(X | theta, lambda).

	Note: clobbers theta.

	Time: O(n_samples*lseq*lseq)
*/
/**********************************************************************/
double zoops_e_step(
  MODEL *model,			/* the model */
  DATASET *dataset		/* the dataset */
)
{
  int i;
  THETA theta0 = model->theta[0];       /* background theta */
  THETA theta1 = model->theta[1];       /* motif theta */
  int w = model->w[1];			/* width of motif */
  double lambda = model->lambda[1];     /* lambda of model */
  int alength = dataset->alength;
  int n_samples = dataset->n_samples;
  double gamma = MIN(1.0, (lambda * ps(dataset, w)) / n_samples);
  double log_1mgamma = LOG(1.0 - gamma);
  double logpX;				/* log prob of X given theta, gamma */

  /* E step */

  /* convert theta to log(theta) */
  convert_theta_to_log(model, alength);

  /* calculate all the posterior offset probabilities */
  logpX = 0.0;				/* log likelihood of model */
  for (i=0; i < n_samples; i++) {	/* sequence */
    int j;
    SAMPLE *s = dataset->samples[i];	/* current sample */
    int lseq = s->length;	/* length of sequence */
    char *res = s->res;                 /* integer-coded sequence */
    double *not_o = s->not_o;           /* prb would not overlap a prv site */
    int *counts = s->counts;		/* character counts of sequence */
    double *z1i = s->z[1];		/* motif z_i */
    double log_pXitheta0;		/* log Pr(X_i | theta0) */
    double log_pXitheta;		/* log prob of X_i given theta */
    double log_lambda = LOG(gamma / (lseq - w + 1));	/* exact log lambda */

    /*
	Estimate E(z_ij | X, theta, gamma); notes 8/16/94
    */

    /* calculate Pr(X_i | theta0) Pr(class = 0) */
    log_pXitheta0 = 0;
    for (j=0; j<alength; j++) {
      log_pXitheta0 += counts[j] * logtheta0(0, j);
    } 
    
    /* calculate Pr(X_i | z_ij=1, theta1, gamma) Pr(z_j = 1 | theta, gamma) and
       Pr(X_i | theta, gamma)
    */
    log_pXitheta = log_pXitheta0 + log_1mgamma;
    for (j=0; j <= lseq - w; j++) {		/* site start */
      /* log Pr(X_i | zij=1,theta1) */
      double log_pXijtheta1 = log_lambda;	
      int k, ii;
      int last = j+w;
      for (k=0; k<j; k++) { log_pXijtheta1 += logtheta0(0, (int)res[k]);}
      for (k=last; k<lseq; k++) { log_pXijtheta1 += logtheta0(0, (int)res[k]);}
      for (k=j, ii=0; k < last; k++, ii++) {	/* position in sequence */
        /* Pr(x_i | z_ij = 1, theta1) */
        log_pXijtheta1 += logtheta1(ii, (int)res[k]);
      }
      z1i[j] = log_pXijtheta1;
      log_pXitheta = LOG_SUM(log_pXitheta, z1i[j]);
    }
    logpX += log_pXitheta;			/* log likelihood of data */

    /* Calculate Pr(z_ij = 1 | X, theta, gamma, v) = 
         Pr(z_ij=1 | X, theta, gamma, v_ij=1) Pr(v_ij) 
    */
    for (j=0; j <= lseq - w; j++) {		/* site start */
      z1i[j] = exp(z1i[j] - log_pXitheta) * not_o[j];
    }
    for (j=lseq-w+1; j < lseq; j++) {		/* tail of sequence */
      z1i[j] = 0;
    }
    s->dz[0] = 1;                               /* fix later */
    s->dz[1] = 0;                               /* fix later */
    s->dz[2] = 0;                               /* fix later */
    s->dz[3] = 0;                               /* fix later */
  }

  return logpX/log(2.0);
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/zoops.c,v 1.2 2004/06/14 14:43:30 rice Exp $ */
