/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* oops.c */
/*	

	EM algorithm.

	OOPSA model:	1 occurrence per sequence; all directions

*/

#include "meme.h"
#include "logs.h"

/**********************************************************************/
/*
  m_step 

	Do the M step of EM.
	Calculate the expected residue frequencies using the
	posterior probabilities of the site starts for each
	sequence.

	Time: O(n_samples*lseq*lseq)
*/
/**********************************************************************/
void m_step(
  MODEL *model,			/* the model */
  DATASET *dataset,		/* the dataset */
  PRIORS *priors,		/* the priors */
  int iter			/* iteration number */
)
{
  int i, j, k, p, ii, c;
  int nc = model->c;			/* number of components of model */
  MTYPE mtype = model->mtype;		/* type of model */
  THETA theta0 = model->theta[0];	/* component 0 */
  THETA theta1 = model->theta[1];	/* component 1 */
  int w = model->w[1];                  /* width of motif */
  BOOLEAN *dir = model->d;		/* used strands */
  int alength = dataset->alength;	/* length of alphabet */
  /* tlb 5-9-97; wgt_total_res */
  int wgt_total_res = dataset->wgt_total_res;	/* weighted size of dataset */
  int n_samples = dataset->n_samples;	/* number of sequences in dataset */
  double *t = dataset->res_freq;	/* frequencies of letters in dataset */
  SAMPLE **samples = dataset->samples;	/* samples */
  double q=0;				/* $Q$; sum of motif expectations */
  double dz_sum[4]={0,0,0,0};		/* \sum_{k=0}^3 dz_ik */
  PTYPE ptype = priors->ptype;		/* type of prior */
  PriorLib *plib = priors->plib;	/* library for mixture priors */
  double total, total_obs;		/* scratch */

  iter++;				/* avoid compiler complaints */

  /* M step */

  /* initialize the frequency matrix to zero */
  for (c=0; c<nc; c++) {		/* component */
    int w = model->w[c];		/* width of component */
    THETA theta = model->theta[c];	/* theta of component */
    for (i=0; i<w; i++) {
      for (j=0; j < alength; j++) {
        theta(i, j) = 0;
      }
    }
  }

  /* calculate the expected frequencies of residues in the 
	different site positions 
  */
  /* get the expected COUNTS, $c_k$ */
  for (i=0; i < n_samples; i++) { 		/* sequence */
    SAMPLE *s = samples[i];			/* array of samples */
    int lseq = s->length;			/* length this seq */
    char *res = s->res;				/* integer-coded sequence */
    char *resc = s->resc;			/* integer-coded complement */
    double *zi = s->z[1];			/* z_i */
    double *dz = s->dz;				/* prob of diff directions */
    double qi = 0;				/* $q_i$; sum of zi */
    int r;					/* integer encoded letter */
    int last_j = lseq - w + 1;			/* last site start */
    double sw = s->sw;				/* sequence weight */

    for (j=0; j < last_j; j++) {		/* site start */
      int last_p = j+w; 			/* last position in site */
      char *eseq;				/* integer encoded sequence */

      qi += zi[j];				/* $Q_i$; sum of z_ij */

      for (k=0; k<=3; k++) {			/* for each strand */
        double z;				/* zij * dz[k] */
        double omz;				/* weight for background */

        if (!dir[k]) continue;			/* skip unused strands */

        dz_sum[k] += dz[k];			/* sum of dz_ik */

        z = zi[j] * dz[k] * sw;			/* weight for motif  */
        omz = sw - z;				/* weight for background */

        eseq = (k<2) ? res : resc;		/* choose the strand */

        /* calculate: E[theta | X, z, dz] 
        */
        if (k==0 || k==3) {			/* left to right strands */
          for (p=j, ii=0; p<last_p; p++, ii++) {
            int r = eseq[p];
	    if (mtype == Tcm) theta0(0, r) += omz;
	    theta1(ii, r) += z;
          }
        } else {				/* right to left strands */
          for (p=j, ii=w-1; p<last_p; p++, ii--) {
            int r = eseq[p];
	    if (mtype == Tcm) theta0(0, r) += omz;
	    theta1(ii, r) += z;
          }
        }
      }
    }

    /* $c_0$ calculation for Oops and Zoops models */
    if (mtype == Oops || mtype == Zoops) {
      for (r=0; r<alength; r++) {
        /* tlb 5-9-97; wgt_total_res */
	theta0(0, r) = wgt_total_res * t[r];	/* $\vt$ */
	for (k=0; k<w; k++) {
          theta0(0, r) -= theta1(k, r);		/* $\vt - \sum_{k=1}^W \vc_k$ */
        }
        theta0(0, r) = MAX(theta0(0, r), 0.001);/* avoid roundoff errors */ 
      }
    }

    q += qi;					/* $Q$; sum of q_i */
  }

  /* M step for theta: convert COUNTS $c_k$ to FREQUENCIES $f_k$ 
     and use frequencies as estimate for theta--- $p_k^{(t+1)} = f_k$ */
  print_once("m-step: keeping track of observed frequencies separately\n"); 
  for (c=0; c<nc; c++) {			/* component */
    int w = model->w[c];			/* width of component */
    THETA theta = model->theta[c];		/* theta of component */
    THETA obs = model->obs[c];			/* observed frequencies */
    double *p_count = priors->prior_count;	/* pseudo counts */

    /* detect error in lambda; should never be 0.0 */
    if (model->lambda[c]==0) 
      printf("lambda[%d]=%g w = %d\n", c, model->lambda[c], model->w[1]);

    /* convert counts to frequencies and regularize using pseudo-counts */
    for (i=0; i<w; i++) {
      total = total_obs = 0.0;		/* total count for position i */
      /* get the pseudo counts if using mixture priors */
      print_once("Regularizing background to avoid divide by zero\n"); 
      if (ptype == Dmix || ptype == Mega || ptype == MegaP) 
        mixture_regularizer(theta[i], plib, p_count);
      for (j=0; j < alength; j++) {
	total_obs += obs(i, j) = theta(i, j);	/* copy theta to obs */
        total += theta(i, j) += p_count[j];	/* pseudo count */
      }
      for (j=0; j < alength; j++) {
	theta(i, j) /= total;			/* normalize to probability */ 
	obs(i, j) /= total_obs;			/* normalize to frequencies */
      }
    }

    /* palindrome: enforce constraints */
    if (model->pal) {
      palindrome(theta, theta, w, alength);
      palindrome(obs, obs, w, alength);
    }
  }


  /* M step for lambda */
  model->lambda[1] = q / ps(dataset, w);
  model->lambda[0] = 1 - model->lambda[1];

  /* M step for sigma */
  total = 0;
  for (k=0; k<=3; k++) total += dz_sum[k];
  for (k=0; k<=3; k++) model->sigma[k] = dz_sum[k]/total;
}

/**********************************************************************/
/*
	e_step

	Do the E step of EM. 

	OOPS model.

	Updates z array.

	Returns log Pr(X | theta).

	Note: Clobbers theta.

	Time: O(n_samples*lseq*lseq)

	See notes 9/13/94
*/
/**********************************************************************/
double e_step(
  MODEL *model,                 /* the model */
  DATASET *dataset              /* the dataset */
)
{
  int i, j, k, p, ii;
  THETA theta0 = model->theta[0];       /* background theta */
  THETA theta1 = model->theta[1];       /* motif theta */
  int w = model->w[1];                  /* width of motif */
  int alength = dataset->alength;
  int n_samples = dataset->n_samples;
  SAMPLE **samples = dataset->samples;
  double q;	
  double logpX;				/* Pr(X | \theta) */
  /* pXtheta[i] = Pr(X_i | \theta) */
  double *log_pXtheta = (double *) malloc(n_samples * sizeof(double));
  BOOLEAN *dir = model->d;
  double log_sigma[4];			/* Pr(dz_ik=1 | theta) */

  /* E step */

  convert_theta_to_log(model, alength);
  for (k=0; k<=3; k++) log_sigma[k] = LOG(model->sigma[k]);

  /* calculate all the posterior offset probabilities */
  logpX = 0.0;				/* prob X given theta */
  q = LITTLE;				/* sum_{i=1}^n Pr(X_i | \theta) */
  for (i=0; i < n_samples; i++){	/* sequence */
    SAMPLE *s = samples[i]; 
    int lseq = s->length;		/* length of the sequence */
    char *res = s->res;			/* integer-coded sequence */
    char *resc = s->resc;		/* integer-coded complement sequence */
    int m = lseq - w + 1;		/* number of possible sites */
    double log_gamma = -LOG((double)m);/* log(\gamma) */
    double *zi = s->z[1];		/* z_i */ 
    double log_pXitheta[4];		/* Pr(X_i | dz_ij=1, \theta) */
    if (m == 0) printf("booboo!\n");

    /* initialize log probabilities to -infinity */
    log_pXtheta[i] = LITTLE;		/* Pr(X_i | \theta) */
    log_pXitheta[0] =log_pXitheta[1]= log_pXitheta[2]= log_pXitheta[3]= LITTLE;

    /* calculate P(X_i | z_ij=1, theta) */
    for (j=0; j < m; j++) {		/* site start */
      double log_pXijktheta;		/* Pr(X_i | z_ij=1, d_ik=1, \theta) */ 
      int first_p = j;			/* start of site */
      int last_p = j+w;			/* end of site */
      double z = LITTLE;		/* zij */
      char *eseq;			/* integer-coded sequence */

      /* calculate: 
        pXijktheta 	= Pr(X_i | z_ij=1, dz_ik=1, \theta)
        pXitheta[k]	= Pr(X_i | dz_ik=1, \theta)
        zi[j]   	= Pr(X_i | z_ij=1, \theta) \gamma_i
      */
      for (k=0; k<=3; k++) {		/* for each strand */
        if (!dir[k]) continue; 		/* skip unused strands */

        eseq = (k<2) ? res : resc;	/* choose the strand */

        log_pXijktheta = 0;		/* Pr(X_i | z_ij=1, d_ik=1, \theta) */ 

        /* calculate probability of positions before the site */
        for (p=0; p<first_p; p++) {
          log_pXijktheta += logtheta0(0, eseq[p]);
        }
        /* calculate probability of positions after the site */
        for (p=last_p; p<lseq; p++) {
          log_pXijktheta += logtheta0(0, eseq[p]);
        }
	/* calculate the probability of positions in the site */
        if (k==0 || k==3) {			/* left to right strands */
	  for (p=j, ii=0; p<last_p; p++, ii++) {
            log_pXijktheta += logtheta1(ii, eseq[p]);
          }
        } else {				/* right to left strands */
	  for (p=j, ii=w-1; p<last_p; p++, ii--) {
            log_pXijktheta += logtheta1(ii, eseq[p]);
          }
        }

	/* z = Pr(X_i | z_ij=1, \theta) =
	    \sum_{k=0}^3 Pr(X_i | z_ij=1, dz_ik=1, \theta) \sigma_k 
	*/
	z = LOG_SUM(z, log_pXijktheta + log_sigma[k]);

	/* Pr(X_i | dz_ik=1, \theta) = 
	     \sum_{j=0}^m \sum_{k=0}^3 Pr(X_i | z_ij=1, dz_ik=1,\theta) \gamma_i 
	*/
	log_pXitheta[k] = LOG_SUM(log_pXitheta[k], log_pXijktheta);
      }  

      /* (unnormalized) zi[j] = Pr(X_i | z_ij=1, \theta) \gamma_i =
	  \gamma_i (\sum_{k=0}^3 Pr(X_i | z_ij=1, dz_ik=1, \theta) \sigma_k)
      */
      zi[j] = z + log_gamma;

    }

    /* calculate:
      pXtheta[i] = Pr(X_i | \theta) = 
	 \sum_{j=0}^m Pr(X_i | z_ij=1, \theta) \gamma_i 
    */
    for (k=0; k<=3; k++) {
      if (!dir[k]) continue; 			/* skip unused strands */
      log_pXitheta[k] += log_gamma;		/* finish calc of this */
      log_pXtheta[i] = LOG_SUM(log_pXtheta[i], log_pXitheta[k] + log_sigma[k]);
    }

    /* calculate: 
      dz[k] = Pr(dz_ik=1 | X_i, \theta) = 
        (Pr(X_i | dz_ik=1, \theta) \sigma_k) / Pr(X_i | \theta)
    */
    for (k=0; k<=3; k++) {
      if (!dir[k]) {
        s->dz[k] = 0;
      } else {
        s->dz[k] = exp(log_pXitheta[k] + log_sigma[k] - log_pXtheta[i]);
      }
    }

    /* calculate: q = \sum_{i=1}^n Pr(X_i | \theta) */
   
    q = LOG_SUM(q, log_pXtheta[i]);

    /* logpX = \sum_{i=1}^n Pr(X_i | \theta) */
    logpX += log_pXtheta[i];
  }

  /* finish calculation of \z_ij */
  /* P(Z=1 | X, \theta, V) \approx P(Z=1 | X, \theta) P(v_ij = 1) */
  for (i=0; i < n_samples; i++){		/* sequence */
    SAMPLE *s = samples[i]; 
    int lseq = s->length;			/* L */
    double *z1i = s->z[1];			/* motif z_i */
    double *not_o = s->not_o;			/* Pr(v_ij = 1) */
    double t;
    t = log_pXtheta[i];				/* log(sum_j (z_ij)) */

    for (j=0; j <= lseq - w; j++) {		/* site */
      z1i[j] = exp(z1i[j] - t) * not_o[j];	/* normalize; de-log */	
    }
    for (j=lseq-w+1; j < lseq; j++) {		/* tail of sequence */
      z1i[j] = 0;
    }

  }

  myfree(log_pXtheta);

  return logpX/log(2.0);
}

/**********************************************************************/
/*
	convert_theta_to_log

	Convert theta_0, ..., theta_{g-1} to log() in place.
*/
/**********************************************************************/
extern void convert_theta_to_log(
  MODEL *model,				/* the model */
  int alength				/* length of alphabet */
)
{
  int c, i, j;
  int nc = model->c;			/* number of components in model */

  /* convert theta to log(theta) */
  for (c=0; c<nc; c++) {			/* component */
    THETA theta = model->theta[c];		/* theta */
    int w = model->w[c];			/* width */
    for (i=0; i < w; i++) {                     /* position */
      for (j=0; j < alength; j++) {		/* letter */
        theta(i, j) = LOGL(theta(i,j));
      }
    }
  }
  print_once("Using LOGL in convert_theta_to_log\n");
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/oops.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
