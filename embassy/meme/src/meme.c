/************************************************************************
*								       	*
*	MEME							       	*
*	Author: Timothy L. Bailey				       	*
*									*
*	Copyright							*
*	(1994 - 1998) The Regents of the University of California.	*
*	All Rights Reserved.						*
*									*
*	Permission to use, copy, modify, and distribute any part of 	*
*	this software for educational, research and non-profit purposes,*
*	without fee, and without a written agreement is hereby granted, *
*	provided that the above copyright notice, this paragraph and 	*
*	the following three paragraphs appear in all copies.		*
*									*
*	Those desiring to incorporate this software into commercial 	*
*	products or use for commercial purposes should contact the 	*
*	Technology Transfer Office, University of California, San Diego,*
*	9500 Gilman Drive, La Jolla, California, 92093-0910, 		*
*	Ph: (619) 534 5815.						*
*									*
*	IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO 	*
*	ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR 	*
*	CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF 	*
*	THE USE OF THIS SOFTWARE, EVEN IF THE UNIVERSITY OF CALIFORNIA 	*
*	HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 		*
*									*
*	THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE *
*	UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE 		*
*	MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.  *
*	THE UNIVERSITY OF CALIFORNIA MAKES NO REPRESENTATIONS AND 	*
*	EXTENDS NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED, *
*	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 	*
*	MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, OR THAT 	*
*	THE USE OF THE MATERIAL WILL NOT INFRINGE ANY PATENT, 		*
*	TRADEMARK OR OTHER RIGHTS.  					*
*								       	*
************************************************************************/
/* meme.c */
/*	
	meme <datafile> [options]

	Reads in a sequence file (Pearson/FASTA format).
	Runs EM algorithm with selected values of W and lambda.

	<datafile>	set of samples: [>id sequence]+
*/
#include "ajax.h"
	
#define DEFINE_GLOBALS 
#include "meme.h"
#include "logs.h"

#ifdef PARALLEL
#include <mpi.h>
#endif

AjPFile outf;
AjPSeqall seqall;
AjPSeqall seqalln;
AjPSeqall seqallm;
AjPStr  datnam;


/* external subroutines */
extern double sqrt(double x);

/* local subroutines */
static void erase(
  DATASET *dataset,		/* the dataset */
  MODEL *model,			/* the model */
  P_POINT *p_point,		/* learned components starting points */
  PRIORS *priors		/* the priors */
);
static BOOLEAN save_candidate(
  MODEL *model,				/* final model */
  DATASET *dataset,			/* the dataset */
  S_POINT *s_point,			/* starting point */
  CANDIDATE *candidates,		/* candidate for best model of width */
  double best_sig			/* best lratio significance */
);
static BOOLEAN init_model(
  P_POINT *p_point,			/* strt point for previous components */
  S_POINT *s_point,			/* the starting point */
  MODEL *model,				/* the model to intialize */
  DATASET *dataset,			/* the dataset */
  THETA map,				/* letter to freq vector map */
  int imotif				/* motif number */
);

#ifdef PARAGON
#include <mach.h>
#include <sys/types.h>
int paragon_page_faults()
{
   struct vm_statistics vm_stat;
   vm_statistics(current_task(), &vm_stat);
   return(vm_stat.pageins+vm_stat.pageouts);
 }
#endif /* PARAGON */

BOOLEAN no_print = FALSE;	/* turn off printing if parallel and not main */

/**********************************************************************/
/*
	main 
*/
/**********************************************************************/

extern int main(
  int argc,
  char *argv[]
)
{
  int i, imotif;
  DATASET *dataset;		/* the dataset */
  DATASET *neg_dataset;		/* the dataset of negative examples */
  THETA map;			/* letter to frequency map */
  MODEL *model;			/* the model */
  MODEL *neg_model;		/* the model of negatives */
  MODEL *best_model;		/* the best model found (so far) */
  PRIORS *priors;		/* the prior probability model */
  CANDIDATE candidates[MAXSITE+1];/* list of candidate models */
  P_POINT *p_point;		/* previously learned components start point */
  double nsites = 0;		/* number of occurrences of motif */
  double min_nsites;		/* minimum occurrences of motif */
  double max_nsites;		/* minimum occurrences of motif */
  int min_w;			/* minimum width of motif */
  int max_w;			/* maximum width of motif */
  BOOLEAN shortn;		/* shorten motifs flag */
  double distance;		/* convergence radius */
  double prob;			/* sampling prob for subsequence starts */
  int n_starts = 0;		/* number of starting points */
  int nmotifs;			/* number of motifs to find */
  int maxiter;			/* maximum number of iterations for EM */
  double chi_alpha;		/* width significance level */
  FILE *debug_file;		/* holds debug info */
  double loop2_time;
#ifdef PARALLEL
  int start_start, incr;
#endif
#ifdef PARAGON
  /*int num_faults = paragon_page_faults();*/
#endif /* PARAGON */




#ifdef PARALLEL
  /* Initialize MPI. */
  MPI_Init(&argc, &argv);

  /* turn off printing if parallel and not the main processor */
  no_print = (mpMyID() != 0);
#endif /* PARALLEL */

#ifdef debug_ieee
  ieee_handler_setup("common");
#endif 

  (void) myclock();		/* record CPU time */

  /* print the command line */
  if (VERBOSE) {
    argv[0] = "meme";
    for (i=0; i<argc; i++) printf("%s ", argv[i]);
    printf("\n\n");
    fflush(stdout);
  }
#ifdef UNIX
  if (!no_print && VERBOSE) system("echo ''; echo CPU: `hostname`; echo '' ");
#endif /* UNIX */

  /* initializate everything from the command line */
  init_meme(argc, argv, &model, &best_model, &dataset, &map, &priors, &p_point,
    &min_nsites, &max_nsites, &min_w, &max_w, &shortn, &distance, 
    &prob, &nmotifs, &maxiter, &chi_alpha, &debug_file, &neg_model,
    &neg_dataset);

  if (!no_print && !NO_STATUS)
    fprintf(stderr, "\nseqs=%6d, min=%4d, max=%5d, total=%9d\n",
	    dataset->n_samples, dataset->min_slength, dataset->max_slength,
	    dataset->total_res);

  /*  Find a concept and erase it loop */
  for (imotif=model->c - 1; imotif <= nmotifs; imotif++) {
    S_POINT *s_points = NULL;		/* array of starting points */
    int i_start;			/* index in s_points */
    int c = imotif;			/* index into previous starts */
    char *e_cons = p_point->e_cons0[c];	/* encoded consensus sequence */
    int best_w = 0;			/* best width */
    double best_sig;			/* best lratio test significance */
    double w_factor = sqrt(2.0);	/* factor separating sampled widths */
    int iter = 0;                       /* total number of EM iterations */

    if (!no_print & !NO_STATUS) fprintf(stderr, "\nmotif=%d\n", imotif);

    /* last component in model */
    c = model->c - 1;		

    /* known motif has been given */
    if (dataset->nmotifs > 0) {
      min_w = max_w = dataset->motifs[imotif-1].width;
      min_nsites = max_nsites = dataset->motifs[imotif-1].pos;
    }

    /* set up the array of starting points for EM */
    s_points = get_starts(dataset, model, map, prob, e_cons,
      min_w, max_w, w_factor, min_nsites, max_nsites, &n_starts);

    /* leave loop if no starts found */
    if (n_starts == 0) break;

    /* tag each candidate width as unused */
    for (i=0; i<=max_w; i++) candidates[i].sig = BIG;

    /* run EM on each start and save best final model for each final width */
    best_sig = BIG;				/* best lratio significance */

    /* Record the start time for this loop. */
    loop2_time = 0.0;
    if ((TIMER == 1) || (TIMER == 3)) {
      loop2_time = myclock()/1E6;
    }

#ifdef PARALLEL
    /* Check whether to parallelize this loop. */
    if (PARA >= 2) {
      start_start = mpMyID();
      incr = mpNodes();
      /* Make sure everybody has something to do. */
      if (start_start >= n_starts)
	start_start = n_starts-1;
    } else {
      start_start = 0;
      incr = 1;
    }
    /* Divide the various starting points among processors. */
    for (i_start=start_start; i_start < n_starts; i_start += incr) {
#else
    for (i_start=0; i_start<n_starts; i_start++) {
#endif /* PARALLEL */
      S_POINT *s_point = s_points+i_start;	/* current starting point */
#ifdef DEBUG
      double s_point_time = myclock()/1E6;
#endif /* DEBUG */

      /* initialize the model from the starting point */
      if (! init_model(p_point, s_point, model, dataset, map, imotif))
	continue;
      
      /* Count iters per loop. */
      model->iter = 0;

      /* Run EM from the starting model */
      em(model, dataset, priors, maxiter, distance);

      /* shorten the motif and check for palindromes */
      if (shortn) {		/* shorten and check palindromes */
	print_once("Fixed non-bug  passing double to int in meme->shorten\n");
	shorten(model, dataset, priors, c, 
	  (int) (model->w[c]/w_factor), maxiter, distance);
      } else if (dataset->pal) { /* don't shorten; palindrome only */
	shorten(model, dataset, priors, c, model->w[c], maxiter, distance);
      }

#ifdef DEBUG
      if (TIMER >= 2) {
#ifdef PARALLEL
	fprintf(stderr, "%2d: ", mpMyID());
#endif /* PARALLEL */
	fprintf(stderr,
		"width=%2d time=%5.2f iter=%2d s/iter=%2.4f nsites0=%2.2f\n",
		s_point->w0, (myclock()/1E6) - s_point_time,
		model->iter, (float)((myclock()/1E6) - s_point_time) /
		(float)(model->iter),  s_point->nsites0);
      }
#endif /* DEBUG */

      /* Keep track of the total number of EM iterations. */
      iter += model->iter;

      /* store model as a candidate for best model;
	 save model if best so far */
      if (save_candidate(model, dataset, s_point, candidates, best_sig)){
	best_w = model->w[c];			/* new best width */
	swap(model, best_model, MODEL *);	/* save model as best_model */
	best_sig = candidates[best_w].sig;	/* new best significance */
      }

    } /* starting point loop */ 

    /* found the best model */
    swap(model, best_model, MODEL *);

    if ((TIMER == 1) || (TIMER == 3)) {
#ifdef PARALLEL
      fprintf(stderr, "%2d: ", mpMyID());
#endif
      fprintf(stderr, "%5.2f s. in loop 2\n", (myclock()/1E6) - loop2_time);
    }

#ifdef PARALLEL
    /* Copy the consensus sequence into the model. */
    store_consensus(model, candidates);

    if (PARA >= 2) {
      /*fprintf(stderr, "\n");
	print_model("Before", model);*/
      
      /* Do the reduction. */
      reduce_across_models(model, dataset->alength);

      /*print_model("After", model);*/
    }
#endif /* PARALLEL */

    /* quit if model has too few sites */
    {
      int w = model->w[1];			/* width of motif */
      double lambda = model->lambda[1];		/* lambda of motif */
      nsites = lambda * ps(dataset, w);		/* estimated number of sites */
      if (nsites < MINSITES) break;
    }

    /* quit if model fails p-value test */
    if (model->sig > chi_alpha) break;

    /* Store the total number of EM's in the model. */
    model->iter = iter;

    /* ERASE the site and starts; use b=0 prior if MegaP */
    erase(dataset, model, p_point, priors);

    /* calculate negative model by doing one EM iteration on negative ex's */
    if (neg_model) {
      /* copy motif model to negative model */
      copy_model(model, neg_model, dataset->alength);
      /* get negative model */
      em(neg_model, neg_dataset, priors, 1, distance);
      /* ERASE the site and starts; use b=0 prior if MegaP */
      erase(neg_dataset, neg_model, p_point, priors);
    }
    
    /* print results */
    if (!no_print) print_results(dataset, neg_dataset, model, neg_model,
      max_w, p_point, candidates);
 
    /* display elapsed time */
    /*printf("\n\n\nTime %5.2f secs.\n\n", myclock()/1E6);
    fflush(stdout);*/

  } /* nmotifs loop */ 

  /* print reason for stopping */
  if (n_starts == 0) {
    ajFmtPrintF(outf,"Stopped because couldn't find any more starting points for EM.\n");
  } else if (imotif > nmotifs) {
    ajFmtPrintF(outf,"Stopped because nmotifs = %d reached.\n", nmotifs);
  } else if (nsites < MINSITES) {
    ajFmtPrintF(outf,"Stopped because next motif has fewer than %3.1f sites.\n",
      MINSITES);
  } else {
    ajFmtPrintF(outf,"Stopped because motif not significant at %f level.\n", chi_alpha);
  }
/*  fflush(stdout);*/

#ifdef UNIX
  if (!no_print) system("echo ''; echo CPU: `hostname`; echo '' ");
#endif /* UNIX */

  /* print environment information */
  print_env(debug_file);

#ifdef PARALLEL
    MPI_Finalize();
#endif

  if (!no_print && !NO_STATUS) fprintf(stderr, "\n");

  return(0);
} /* main */

/**********************************************************************/
/*
	erase

        For all models:
	  Reset the weights of the letters to probabilisticaly "erase"
	  the ones which occur in sites already found.
	  (Removed:Recalculate the entropy of the dataset.)

          tlb; 8/7/97 removed Mcm stuff 
*/
/**********************************************************************/
static void erase(
  DATASET *dataset,		/* the dataset */
  MODEL *model,			/* the model */
  P_POINT *p_point,		/* learned components starting points */
  PRIORS *priors		/* the priors */
)
{
  int i, j, k;
  int n_samples = dataset->n_samples;
  SAMPLE **samples = dataset->samples;
  int w = model->w[1];
  double *res_freq = (double *) malloc(dataset->alength * (int) sizeof(double));
  BOOLEAN use_comp = (model->d[2] | model->d[3]);	/* using complements */
  int nmotifs;				/* number of known motifs */


  /* calculate remaining (unerased) frequencies of letters */
  for (i=0; i<dataset->alength; i++) res_freq[i] = 0;

  /* run EM for 1 iteration to compute z_ij for use by erase */

  /* use b=0 if using MegaP heuristic */
  if (priors->ptype == MegaP) SWAP(PriorLib *, priors->plib, priors->plib0);

  /* if there are known motifs, don't use them in em step */
  nmotifs = dataset->nmotifs;
  dataset->nmotifs = 0;

  em(model, dataset, priors, 1, 0);

  /* restore known motifs */
  dataset->nmotifs = nmotifs;

  if (priors->ptype == MegaP) SWAP(PriorLib *, priors->plib, priors->plib0);

  /* 
     z_ij is taken as the probability of a site occurring at i,j.
     The probability of a position being in a site is taken
     as the maximum of the z_ij for sites containing (overlapping) it.
     w_ij is set to 1-max(z_ij) times its previous value which
     reflects the independence assumption among motifs.
  */

  for (i=0; i<n_samples; i++) 		{	/* sequence */
    double *weights = samples[i]->weights;	/* w_ij */
    int len = samples[i]->length;		/* seq length */
    double *z = samples[i]->z[1];		/* z_ij */
    char *res = samples[i]->res;		/* integer-coded sequence */
    char *resc = samples[i]->resc;		/* integer-coded complement */

    for (j=0; j<len; j++) {			/* position */
      double max_z = 0.0;
      /* find largest probability that site overlaps this position */
      for (k=MAX(0,j-w+1); k<=j && k<len-w+1; k++) {
	max_z = MAX(max_z, z[k]);
      }
      /* update the probability that position not in a site */
      weights[j] *= 1.0 - max_z;
      /* update the letter frequencies */
      res_freq[(int)res[j]] += weights[j]; 		/* freq of letter */
      if (use_comp) res_freq[(int)resc[j]] += weights[j];/* freq of letter */
    }
  }

  if (PRINT_W) print_wij(dataset);

  /* tlb; 8/7/97 removed Mcm stuff */
}

/**********************************************************************/
/*
	init_model

	Initialize a model from a starting point.

	Returns false if starting point was not valid.
*/
/**********************************************************************/
static BOOLEAN init_model(
  P_POINT *p_point,			/* strt point for previous components */
  S_POINT *s_point,			/* the starting point */
  MODEL *model,				/* the model to intialize */
  DATASET *dataset,			/* the dataset */
  THETA map,				/* letter to freq vector map */
  int imotif				/* motif number */
)
{
  int i, c, nc;
  int w0; 
  double lambda = 0, sigma = 1.0/model->nstrands;
  
  /* skip if no good starting points found for w0, nsites0 */
  if (s_point->score == LITTLE) { return FALSE; }

  /* initialize previously learned components of model (and background) */
  nc = model->c;				/* number components */
  for (c=0; c<nc-1; c++) {
    w0 = model->w[c] = p_point->w[c];
    init_theta(model->theta[c], p_point->e_cons0[c], w0, map, dataset->res_freq,
      dataset->alength);
    if (c > 0) 
      lambda += model->lambda[c] = p_point->nsites[c]/ps(dataset, w0);
    model->pal = 0;
  }
 
  /* initialize the new motif */
  /*printf("Starting point: %s\n", s_point->cons0);*/
  w0 = model->w[nc-1]  = s_point->w0;
  init_theta(model->theta[nc-1], s_point->e_cons0, w0, map, dataset->res_freq,
    dataset->alength);
  lambda += model->lambda[nc-1] = s_point->nsites0/ps(dataset, w0);
  model->pal = 0;

  /* initialize lambda[0] */
  model->lambda[0] = 1 - lambda;

  /* initialize sigma */
  for (i=0; i<3; i++) model->sigma[i] = sigma; 

  if (PRINTALL) {
    for (c=0; c<nc; c++) {
      ajFmtPrintF(outf,"component %2d: lambda= %8.6f\n", c, model->lambda[c]);
      print_theta(2, model->theta[c], model->w[c], "", dataset, NULL);
    }
  }

  /* init motif number */
  model->imotif = imotif;

  return TRUE;
}

/**********************************************************************/
/*
	save_candidate

	Save the starting point and part of model if it is
	most significant model of its width so far.

	Returns true if the model is the best so far.

*/ 
/**********************************************************************/
static BOOLEAN save_candidate(
  MODEL *model,				/* final model */
  DATASET *dataset,			/* the dataset */
  S_POINT *s_point,			/* starting point */
  CANDIDATE *candidates,		/* candidate for best model of width */
  double best_sig			/* best lratio significance */
)
{
  int nc = model->c;			/* number of components of model */
  int w = model->w[nc-1];		/* final motif w */
  int pal = model->pal;			/* palindrome flag */
  double lambda = model->lambda[nc-1];	/* final motif lambda */
  char *cons = model->cons;		/* final motif consensus */
  double rel = model->rel[nc-1];	/* final model rel */
  double e_ll_0  = model->e_ll_0;	/* null model e_ll */
  double e_ll = model->e_ll;		/* final model e_ll */
  double ll = model->ll;		/* final model ll */
  double sig = model->sig;		/* likelihood ratio significance */
  int w0 = s_point->w0;			/* initial w */
  double nsites0 = s_point->nsites0;	/* initial nsites */
  char *start = s_point->cons0;		/* initial consensus */

  /* print the results for this w0, nsites0 and THETA */ 
  if (PRINT_STARTS) { 
    ajFmtPrintF(outf,"\n(start) %3d %6.1f %.*s --> %s ", w0, nsites0, w0, start, cons); 
    ajFmtPrintF(outf,"IC/col %6.3f e_ll %6.0f w %3d nsites %5.1f", 
      rel, e_ll, w, lambda*ps(dataset, w)); 
    ajFmtPrintF(outf," lambda %8.7f  e_ll %6.0f  ll %6.0f sig %12.3g\n\n", 
      lambda, e_ll, ll, sig); 
  } 

  /* save the results if best so far for this width */ 
  if (sig < candidates[w].sig) {
    candidates[w].s_point = s_point;
    candidates[w].w = w;
    candidates[w].pal = pal;
    candidates[w].lambda = lambda;
    strcpy(candidates[w].cons, cons);
    candidates[w].rel = rel;
    candidates[w].e_ll_0 = e_ll_0;
    candidates[w].e_ll = e_ll;
    candidates[w].ll = ll;
    candidates[w].sig = sig;
#ifdef debug
    printf("best cons = %s null %g ll %g sig %g\n", cons, 
      e_ll_0, e_ll, sig);
#endif
  }
  return (sig < best_sig);
}

/**********************************************************************/
/*
	create_priors
*/
/**********************************************************************/
extern PRIORS *create_priors(
  PTYPE ptype,				/* type of prior to use */
  double beta, 				/* beta for dirichlet priors;
					   < 0 only returns alphabet */
  DATASET *dataset,			/* the dataset */
  char *plib_name			/* name of prior library */
)
{
  int i;
  int alength = (dataset ? dataset->alength : 0);
  double *back = (dataset ? dataset->nrdb_freq : (double *)NULL);
  PRIORS *priors = (PRIORS *) malloc(sizeof(PRIORS));

  priors->ptype = ptype;

  /* set up the prior counts */
  switch (ptype) {
    case Addone:				/* add one prior */
      for (i=0; i<alength; i++) priors->prior_count[i] = 1.0;
      break;
    case Dirichlet:				/* simple dirichlet prior */
      for (i=0; i<alength; i++) priors->prior_count[i] = beta * back[i];
      break;
    case Dmix:					/* mixture of dirichlet's */
    case Mega:					/* megaprior heuristic */
    case MegaP:					/* mod. megaprior heuristic */
    {
      priors->plib = read_PriorLib(plib_name, beta);

      /* get b=0 prior for modified mega prior heuristic */
      if (ptype == MegaP || ptype == Mega) {	/* used adj freq with Mega */
        double b = 0; 
	priors->plib0 = read_PriorLib(plib_name, b);
      }

      break;
    }
  }
  return priors;
}

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/meme.c,v 1.2 2004/06/14 14:43:30 rice Exp $ */
