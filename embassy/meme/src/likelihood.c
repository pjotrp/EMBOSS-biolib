/**********************************************************************/
/*
	calc_like

	Calculate the (expected) likelihood, relative entropy and
	likelihood ratio test significance of the model.
        Sets model-> e_ll_0, e_ll, rel, and sig.

	The likelihood is actually the expected value of the
	likelihood function over the conditional density of
	the missing data given the data and the model.

*/
/**********************************************************************/

#ifdef lrt_standalone
#define DEFINE_GLOBALS
#endif
#include "meme.h"

static double lrsig(
  double ll0,					/* likelihood of null model */
  double ll,					/* likelihood of model */
  double nu  					/* difference in free params */
);

extern void calc_like(
  MODEL *model,			/* the model */
  DATASET *dataset  		/* the dataset */
)
{
  int c, i, j;
  double entropy[MAXG];		/* entropy/col of each component of model */
  double rentropy[MAXG];	/* information content of each component */
  double sentropy;		/* entropy due to different strands */
  MTYPE mtype = model->mtype;			/* type of model */
  double *sigma = model->sigma;			/* probability diff strands */
  int nc = model->c;				/* number of components */
  int alength = dataset->alength;		/* length of alphabet */
  double h0 = dataset->adj_entropy;		/* dataset entropy */
  double n = dataset->n_samples;		/* number of sequences */
  /* tlb 5-9-97; wgt_total_res */
  double N = dataset->wgt_total_res;		/* weighted size of dataset */
  double *freq = dataset->res_freq;		/* average letter freqs */
  double w;					/* width of widest motif */
  int ind_cols = 0;				/* # indep columns in model*/

  /* initialize entropies */
  for (c=0; c<MAXG; c++) entropy[c] = rentropy[c] = 0.0;

  /* calculate the entropy, ic and #indep cols of each motif and background */
  for (c = 0; c < nc; c++) {			/* component of model */
    THETA theta = model->theta[c];		/* theta of component */
    THETA obs = model->obs[c];			/* observed frequencies */
     
    w = model->w[c];				/* width of component */
    ind_cols += ind_cols(model, c);		/* indep cols of component */

    /* entropies */
    for (i = 0; i < w; i++) {			/* position in site */
      for (j = 0; j < alength; j++) {		/* letter */
        double p = theta(i, j);			/* probability in model */
        double f = obs(i, j);			/* observed frequencies */
        double p0 = freq[j];			/* overall frequency */
        entropy[c] += f * LOG2(p);		/* entropy */
        rentropy[c] += f * LOG2(p0);		/* relative entropy */
      }
    }

    entropy[c] /= w;				/* entropy per column */
    rentropy[c] /= w;				/* entropy per column */

    /* information content per column of motif */
    model->rel[c] = entropy[c] - rentropy[c];
  }

  /* calculate the entropy due to different strands */
  sentropy = 0;
  for (i=0; i<3; i++) if (model->d[i]) sentropy += sigma[i] * LOG2(sigma[i]);

  /* calculate the expeceted log likelihood of the model and null model */

  if (mtype == Oops || mtype == Zoops) {
    /* use component 1 for motif */
    double lambda = model->lambda[1];		/* lambda of component 1 */
    double w = model->w[1];			/* width of component 1 */
    double nsites = nsites(dataset, w, lambda);	/* est # sites in data */
    double Q = MIN(nsites, n);			/* correct numerical errors */
    double gamma = MIN(1.0, nsites/n);		/* fraction of seqs with site */

    {
      if (mtype == Oops) print_once("Using zoops likelihood for oops model\n");
      /* calculate the expected log likelihood of null model */
      model->e_ll_0 = N * h0;
      /* calculate expected log likelihood of the model */
      model->e_ll = 
	((N - Q*w) * entropy[0] ) +
	Q * w * entropy[1] + 
	((n - Q) * LOG2(1 - gamma)) + 
	(Q * LOG2(lambda)) +
        (Q * sentropy);
    }
  } else if (mtype == Tcm) { 
    double N_p;					/* effective size of dataset */
    double lambda_0;
    double e_ll;
    w = model->w[1];

    N_p = N/w;
    print_once("***calc_like: using N_p N/w\n");

    /* calculate expected log likelihood of the model */
    model->e_ll = 0;				/* model e_ll */
    lambda_0 = 1;				/* background lambda */
    for (c=1; c<nc; c++) { 
      double lambda = model->lambda[c];

      lambda = nsites(dataset, w, lambda) * w / N;
      print_once("***calc_like: using lambda = nsites * w /N\n"); 

      e_ll = lambda * (LOG2(lambda) + w * entropy[c]);
      model->e_ll += e_ll;
      lambda_0 -= lambda;			/* component 0 lambda */
      /*printf("w %f c %d lambda %f e_ll %f\n", w, c, lambda, e_ll);*/
    }
    e_ll = lambda_0 * (LOG2(lambda_0) + w * entropy[0]);
    /*printf("w %f c %d lambda %f e_ll %f\n", w, 0, lambda_0, e_ll);*/
    model->e_ll += e_ll;
    model->e_ll *= N_p;			/* expected log likelihood */
    model->e_ll_0 = N_p * w * h0;
    /*printf("w %2f e_ll0 %f e_ll %f ", w, model->e_ll_0, model->e_ll);*/

  } else { 
    printf("calc_like: unknown model type\n"); 
    exit(1);
  }

  /* Calculate likelihood ratio significance level of model vs. null model.  */
  {
    double ll0 = model->e_ll_0;	/* log likelihood of null model */
    double ll = model->e_ll;		/* log likelihood of model */
    int alength = dataset->effective_alength;	/* length of alphabet */
    double nu = (alength-1) * (ind_cols - 1);	/* delta degrees of freedom */
    print_once("lrsig: Using 1 columns in null model\n");

    model->lrt = lrsig(ll0, ll, nu);
    model->root = model->lrt/(nu-1.0);
    model->bon = log10(nu-1.0) + model->lrt;
    if (model->bon >= -2.0) {			/* \nu \alpha >= .01 */
      model->bon = log10(1.0 - pow(1 - exp10(model->lrt), (nu-1.0)));
    }
    switch(dataset->adj) {
    case None: model->sig = model->lrt; break; 
    case Bon: model->sig = model->bon; break;
    case Root: model->sig = model->root; break;
    }
  }
}

/**********************************************************************/
/*
	lrsig

	Compute log10 significance level of likelihood ratio test.
*/
/**********************************************************************/
static double lrsig(
  double ll0,					/* likelihood of null model */
  double ll,					/* likelihood of model */
  double nu  					/* difference in free params */
)
{
  double x;					/* likelihood ratio */
  double log10_alpha;

  if (ll <= ll0) return 0;			/* not significant */
 
  x = 2 * log(2.0) * (ll - ll0); 		/* likelihood ratio */
  log10_alpha = chiq(x, nu);
  return (log10_alpha);
}

#ifdef lrt_standalone
main( int argc, char **argv)
{
  double ll0 = atof(argv[1]);
  double ll = atof(argv[2]);
  double nu = atof(argv[3]);
  double alpha = .005;
  init_log();

  printf("LRT = %g\n", exp10(lrsig(ll0, ll, nu)));
}
#endif
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/likelihood.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
