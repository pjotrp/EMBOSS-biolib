/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/

#include "meme.h"

#define WORST_LSIG 0.0

#define check_sig(p)							\
  {									\
    model->pal = p;							\
    calc_like(model, dataset);	/* get the significance */	 	\
    /*printf("w = %d, log(sig) = %g %g %*.*s\n", model->w[c],model->sig,\
      model->e_ll, model->w[c], model->w[c], model->cons+col);*/	\
    if ((model->sig < best_sig)						\
      || (best_sig == WORST_LSIG && model->e_ll > best_e_ll)) {		\
      best_col = col;							\
      best_sig = model->sig;						\
      best_e_ll = model->e_ll;						\
      best_w = model->w[c];						\
      best_lambda = model->lambda[c];					\
      best_pal = p;							\
    }									\
  }

/**********************************************************************/
/*
	shorten

	Find the optimum shortened version of a motif.
	Re-runs EM on shortened version of the motif.

*/
/**********************************************************************/

extern void shorten(
  MODEL *model,				/* the model */
  DATASET *dataset,			/* the dataset */
  PRIORS *priors,			/* the priors */
  int c,				/* component of model to shorten */
  int min_w,				/* minimum width of motif allowed */
  int maxiter,				/* maximum number iterations for EM */
  double distance			/* stopping criterion */
)
{
  int i, j, w;
  double old_lambda = model->lambda[c];	/* current motif lambda */
  int old_w = model->w[c];		/* current motif width */
  int best_w;				/* best width */
  int best_col;				/* best starting column */
  double best_lambda;			/* best value of lambda */
  BOOLEAN best_pal;			/* best is a palindrome */
  double best_sig;			/* best value of lratio significance */
  double best_e_ll;			/* best expected log likelihood */
  THETA theta = model->theta[c];	/* motif theta */
  THETA theta0 = model->theta[0];	/* background theta */
  THETA theta_pal;			/* scratch for making palindrome */
  THETA obs = model->obs[c];		/* motif theta */
  THETA obs0 = model->obs[0];		/* background theta */
  THETA obs_pal;			/* scratch for making palindrome */
  int alength = dataset->alength;	/* alphabet length */
/* tlb 5-12-97; wgt_total_res */
  int wgt_total_res = dataset->wgt_total_res;	/* total size of dataset */
  double *res_freq = dataset->res_freq; /* average letter frequencies */
  double nsites = nsites(dataset, old_w, old_lambda);
  double back[MAXALPH];			/* background counts */

  /* create a scratch theta array for making palindromes */
  create_2array(theta_pal, double, MAXSITE, alength);
  create_2array(obs_pal, double, MAXSITE, alength);

  /* save the counts for the background */
  print_once("Fixed bug in shorten: adjust background correctly\n");
  for (j=0; j<alength; j++) back[j] = wgt_total_res * res_freq[j];

  /* initialize search stuff */
  best_sig = WORST_LSIG;		/* worst log(significance) */
  best_e_ll = LITTLE;
  best_col = 0;
  best_pal = FALSE;
  best_w = min_w;
  best_lambda = nsites/ps(dataset, min_w);

  /* find the best start column in motif for each width <= old_w */
  for (w=min_w; w <= old_w; w++) {
    int col;					/* current column */

    /* set new width */
    model->w[c] = w;

    for (col = 0; col <= old_w - w; col++) {

      model->theta[c] = theta + col;		/* shift theta */
      model->obs[c] = obs + col;		/* shift theta */
      model->lambda[c] = nsites/ps(dataset, w);	/* adjust lambda to new w */
   
      /* calculate new background frequencies */
      for (j = 0; j<alength; j++) theta0(0,j) = obs0(0,j) = back[j];/* counts */
      for (i = col; i<col+w; i++) {
        for (j = 0; j<alength; j++) { 
          /* subtract motif counts from background counts */ 
          theta0(0,j) -= nsites * theta(i,j);
          obs0(0,j) -= nsites * obs(i,j);
        }
      }
      /* normalize background theta0 */
      for (j = 0; j<alength; j++) {
        print_once("5/16/95 yet another fix in shorten to avoid / by 0\n");
        if (theta0(0,j) <= 0 ||			/* priors screw up things */
           (wgt_total_res - (nsites * w)) <= 0) { 	/* avoid divide by 0 */
          theta0(0,j) = res_freq[j];
          obs0(0,j) = res_freq[j];
        } else {				/* things are ok */
          theta0(0,j) /= wgt_total_res - (nsites * w);
          obs0(0,j) /= wgt_total_res - (nsites * w);
        }
      }

      /* check this start and width */

      if (!dataset->pal) {			/* palindromes not allowed */ 
        check_sig(FALSE);			
      } else { 					/* palindromes allowed */
        if (dataset->pal == 1) {		/* non-palindromes allowed */
          check_sig(FALSE);			/* motif significant? */
        } 
	palindrome(theta+col, theta_pal, w, alength);
	palindrome(obs+col, obs_pal, w, alength); 
	model->theta[c] = theta_pal;		
	model->obs[c] = obs_pal;	
	model->lambda[c] = nsites/ps(dataset, w);
	check_sig(TRUE);			/* palindrome significant? */
      } 
    } /* start column loop */
  } /* width loop */

  /* free the scratch space */
  free_2array(theta_pal, MAXSITE);
  free_2array(obs_pal, MAXSITE);

  /* shift the motif */
  if (best_col != 0) {
    for (i = 0; i < best_w; i++) { 
      swap(theta[i], theta[best_col + i], double *);
      swap(obs[i], obs[best_col + i], double *);
    }
  }

  /* shorten motif */
  model->w[c] = best_w;			/* shorten the motif */
  model->pal = best_pal;		/* set palindrome flag? */
  model->theta[c] = theta;		/* restore theta */
  model->obs[c] = obs;			/* restore theta */
  model->lambda[c] = best_lambda;	/* restore lambda */

  /* palindromize theta if the best motif was a palindrome */
  if (best_pal) {
    palindrome(theta, theta, best_w, alength); /* palidromize */
    palindrome(obs, obs, best_w, alength); /* palidromize */
  }

  /* run EM on shortened / palindromized motif */
  em(model, dataset, priors, maxiter, distance);

}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/shorten.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
