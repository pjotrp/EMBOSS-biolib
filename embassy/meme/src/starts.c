/***********************************************************************
*                                                                      *
*       MEME++                                                         *
*       Copyright 1994, The Regents of the University of California    *
*       Author: Timothy L. Bailey                                      *
*                                                                      *
***********************************************************************/
/* starts.c */
/*
	Routines to set up an array of starting points for EM.
*/

#include "meme.h"

/* 
  Compute maximum and minimum number of sites to try.  If no max or min
  was input, they are computed as:
    min = sqrt(number of sequences)
    max = MAX(number of sequences, number of possible sites/(width+1)).
  M = max_ or min_nsites0, N = n_samples, W = width, D = dataset 
*/
#define MIN_NSITES(M, N)       ( (M) ? (M) : MAX(2, sqrt((double)(N))) )
#define MAX_NSITES(M, N, W, D) ( (M) ? (M) : MAX((N), ps((D),(W))/((W)+1.0)) )

static int get_nsites0 (
  int w,				/* width of motif */
  double min_nsites,			/* minimum nsites0 */ 
  double max_nsites,			/* maximum nsites0 */
  S_POINT *s_points			/* array of starting points */
);

/**********************************************************************/
/*
       	get_starts

	Create a list of starting points for EM. 

	If sample_prob > 0, samples subsequences in the input
	dataset for good starting points.  
	Otherwise, e_cons is specified as the starting point.

	w is sampled in geometric progression by x w_factor;
	nsites is sampled in a geometric progression by x 2. 

	Returns number of starting points in list.

*/
/**********************************************************************/

extern S_POINT *get_starts(
  DATASET *dataset,		/* the dataset */
  MODEL *model,                 /* the model */
  THETA map,			/* letter by frequency mapping matrix */
  double sample_prob,		/* sampling probability for subsequences */
  char *e_cons,			/* encoded consensus sequence */
  int min_w,			/* minimum width of motif */
  int max_w,			/* maximum width of motif */
  double w_factor,		/* factor between sampled widths */
  double min_nsites,		/* minimum occurrences of motif */
  double max_nsites,		/* minimum occurrences of motif */
  int *n_starts			/* number of starting points */
)
{
  int i, j;
  S_POINT *s_points = NULL;	/* array of starting points */
  int n = 0;			/* number of starting points in array */
  int w;			/* width of motif */
  MTYPE mtype = model->mtype;	/* type of model */
  int n_samples = dataset->n_samples;	/* number of sequences in dataset */
  double loop1_time = 0.0;      /* CPU time in 1st parallelized loop */

  /* 
    try all values of w up to max_w in geometric progression 
  */
  for (w = min_w; w <= max_w; w = MIN(max_w, (int) (w * w_factor) ) ) {
    int min_n = MIN_NSITES(min_nsites, n_samples);
    int max_n = MAX_NSITES(max_nsites, n_samples, w, dataset);
    int n_nsites0 = LOG2(max_n)+1;	/* upper bound on # of nsites0 for w */

    /* get list of nsites0 for this width and append to s_points list */
    Resize(s_points, n+n_nsites0, S_POINT);
    n_nsites0 = get_nsites0(w, min_n, max_n, s_points+n);

    /* fill in the starting points with the subsequence to use */
    if (!e_cons && sample_prob != 0) {	/* sample subsequences for starts */
      n_nsites0 = subseq7(mtype, map, dataset, w, n_nsites0, sample_prob,
		  s_points+n, &loop1_time);
    } else {				/* don't sample subsequences */
      for (i=n; i<n+n_nsites0; i++) {
        s_points[i].e_cons0 = e_cons;		/* use the input consensus */
        s_points[i].score = BIG;		/* tag as good starting point */
      }
    }

    /* set up human readable consensus sequence for starting point */
    for (i=n; i<n+n_nsites0; i++) {
      char *e_cons0 = s_points[i].e_cons0;
      for (j=0; j<w; j++)
        s_points[i].cons0[j] = (e_cons0 ? unhash(e_cons0[j]) : 'x');
      s_points[i].cons0[j] = 0;
      if (PRINT_STARTS) {
	ajFmtPrintF(outf,"s=%d, score=%6.0f, w0=%3d, nsites0=%5.0f, cons=%s\n",
	  i, s_points[i].score, s_points[i].w0, 
	  s_points[i].nsites0, s_points[i].cons0);
      }
    }

    /* update length of starting point list */
    n += n_nsites0;

    /* end of w loop; tricky so max_w will be used */
    if (w == max_w) break;
  }

  if ((TIMER == 1) || (TIMER == 3)) {
#ifdef PARALLEL
    fprintf(stderr, "%2d: ", mpMyID());
#endif
    fprintf(stderr, "%5.2f s. in loop 1\n", loop1_time);
  }

  *n_starts = n;			/* number of new starts */
  return s_points;
} /* get_starts */

/**********************************************************************/
/*	
	get_nsites0

	Get a list of the values to try for nsites0 and
	put them in the s_point array.   The array is set
	up with each entry with score LITTLE.

	List is geometric, increasing by factor of 2 from
	min_nsites to max_nsites.
	
	Returns the size of the added list.
*/
/**********************************************************************/
static int get_nsites0 (
  int w,				/* width of motif */
  double min_nsites,			/* minimum nsites0 */ 
  double max_nsites,			/* maximum nsites0 */
  S_POINT *s_points			/* array of starting points */
)
{
  double nsites;			/* number of sites */
  int n;				/* number of nsites for this w */

  /* initialize the starting points, making sure everything is initalized
     so that MPI won't barf
  */
  for (n=0, nsites=min_nsites; nsites < 2*max_nsites; n++, nsites*=2) {
    s_points[n].score = LITTLE;
    s_points[n].iseq = 0;
    s_points[n].ioff = 0;
    s_points[n].w0 = w;
    s_points[n].nsites0 = nsites<max_nsites ? nsites : max_nsites;
    s_points[n].e_cons0 = NULL;
    s_points[n].cons0 = NULL; 
    Resize(s_points[n].cons0, w+1, char);
    s_points[n].cons0[0] = '\0';
  }

  return n;				/* number of starts for w */
} /* get_nsites0 */
