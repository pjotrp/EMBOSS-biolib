/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* subseq7.c */
#include "meme.h"
#include "logs.h"

/* see if support S1 is better than support S2.  The comparison is
        complicated by the fact that relative supports are positive
        and absolute supports are negative. */
#define BETTER_SUP(S1, S2) ((S2) < (S1))

/* minimum probability of NOT overlapping a previous site for starting points */
#define MIN_NOT_O .1

/* use logs and integers */

#define LOG_THETA_TYPE(V)		int *V[2][MAXSITE]
#define LOG_THETAG_TYPE(V)		int *V[MAXSITE]

typedef struct p_prob {
  int x;			/* row of score */
  int y;			/* column of score */
  int prob;			/* probability of position */
}p_prob;
typedef struct p_prob *P_PROB;

/* local functions */
static void get_pY(
  DATASET *dataset,			/* the dataset */
  LOG_THETAG_TYPE(theta_1),		/* integer log theta_1 */
  int w					/* width of motif */
);
static void next_pY(
  DATASET *dataset,			/* the dataset */
  LOG_THETAG_TYPE(theta_1),		/* integer log theta_1 */
  int w,				/* width of motif */
  int *theta_0				/* first column of previous theta_1 */
);
static int sort_max(
  MTYPE mtype,		/* the model type */
  DATASET *dataset,	/* the dataset */
  int w,		/* length of sites */ 
  P_PROB maxima 	/* array of encoded site starts of local maxima */
);
static void global_max(
  DATASET *dataset,	/* the dataset */
  int w,		/* length of sites */ 
  P_PROB maxima 	/* array of encoded site starts of local maxima */
);
static int local_max(
  DATASET *dataset,	/* the dataset */
  int w,		/* length of sites */ 
  P_PROB maxima 	/* array of encoded site starts of local maxima */
);
static int pY_compare(
  const void *v1, 
  const void *v2 
);
static void init_theta_1(
  int w,			/* width of site */
  char *res,			/* (encoded) letters of subsequence */
  LOG_THETAG_TYPE(theta_1),	/* theta_1 */
  int lmap[MAXALPH][MAXALPH]	/* matrix of frequency vectors */ 
);
static int align_top_subsequences(
  int w,				/* width of motif */
  DATASET *dataset,			/* the dataset */
  int iseq,				/* sequence number of starting point */
  int ioff,				/* sequence offset of starting point */
  char *eseq,				/* integer encoded subsequence */
  char *name,				/* name of sequence */
  int n_nsites0,			/* number of nsites0 values to try */
  int n_maxima,				/* number of local maxima */
  P_PROB maxima,			/* sorted local maxima indices */
  int flags,				/* bit flags of nsites0 to try */
  int bit_mask[],			/* bit masks */
  S_POINT s_points[]			/* array of starting points */
);

/**********************************************************************/
/*
	subseq7	

	Try subsequences as starting points and choose the
	one which yeilds the highest score.
	Score is computed by:
		1) computing p(Y | theta_1)
		2) finding the (sorted) postions of maximum pY
		3) aligning the top NSITES0 scores for each value of NSITES0
		4) computing the expected likelihood for each value of NSITES0

	The computing of p(Y | theta_1) for successive
	subsequences (values of theta_1) is optimized by
	using p_i to calculate p_{i+1}.

	Returns number of starting points updated in s_points array.

	Updates s_points, array of starting points, one
	for each value of NSITES0 tried-- finds one \theta for each
	value of nsites0 specified in the input.

	Uses globals:
*/
/**********************************************************************/

extern int subseq7(
  MTYPE mtype,			/* type of model */
  THETA map,			/* freq x letter map */
  DATASET *dataset,		/* the dataset */
  int w,			/* width of motif */
  int n_nsites0,		/* number of nsites0 values to try */
  double alpha,			/* sampling probability */
  S_POINT s_points[], 		/* array of starting points: 1 per nsites0 */
  double *loop1_time            /* CPU time spent in first parallelized loop */
)
{
  LOG_THETA_TYPE(ltheta);	/* integer encoded log theta */
  int i, j, iseq, ioff;
  double x;
  int alength = dataset->alength;	/* length of alphabet */
  int n_samples = dataset->n_samples;	/* number of samples in dataset */
  SAMPLE **samples = dataset->samples;	/* samples in dataset */
  int possible_sites = ps(dataset, w);
  int n_starts = 0;		/* number of sampled start subseq */
  int isample;			/* index to subsequences considered */
  int n_maxima;			/* number of local maxima */
  /* the local maxima positions */
  P_PROB maxima = (P_PROB) malloc(possible_sites * sizeof(p_prob));
  double avgdist[32];           /* avg distance between samples to try */
  int bit_mask[32];             /* bit masks */
  int *trylist = (int *) malloc(possible_sites * sizeof(int));
  int lmap[MAXALPH][MAXALPH];	/* consensus letter vs. log frequency matrix */
  double loop1_clock;
#ifdef PARALLEL
  int start_seq, start_off=0, end_seq, end_off=0, incr;
#endif

  /* set up the matrix of frequency vectors for each letter in the alphabet */
  for (i=0; i<alength; i++) {
    for (j=0; j<alength; j++) {
      lmap[i][j] = INT_LOG(map[j][i]);
    }
  }

  if (TRACE) printf("w= %d, possible_sites= %d\n", w, possible_sites);

  /* set up to sample various values of nsites0 */
  for (i = 0; i < n_nsites0; i++) {
    double x = s_points[i].nsites0;
    /* f = fraction of possible sites to sample*/
    double f = (alpha > 0.0 && alpha < 1.0) ? MIN(1, -log(1-alpha)/x) : 1.0;
    avgdist[i] = 1/f;				/* distance between samples */
    bit_mask[i] = 1 << n_nsites0;		/* for testing the bits */
    if (TRACE) 
     printf("Testing %g%% of subsequences as starts with nsites0=%f.\n",
       100.0*f, x);
  }
  /* set up list of words that tell which nsites0 to try for each start */
  for (i=0; i<possible_sites; i++) trylist[i] = 0;
  for (i=0; i<n_nsites0; i++) {
    for (x=0; x<possible_sites; x+=avgdist[i]) {
      trylist[(int) x] |= bit_mask[i];
    }
  }

  /* get the probability that a site starting at position x_ij would
     NOT overlap a previously found motif
  */
  get_not_o(dataset, w, TRUE);

  /* Record the current time. */
  loop1_clock = 0.0;
  if ((TIMER == 1) || (TIMER == 3))
    loop1_clock = myclock();

  /* score all the sampled positions saving the best position for
     each value of NSITES0 */
  isample = -1;
#ifdef PARALLEL

  if ((PARA == 1) || (PARA == 3)) {
    /* Retrieve the previously-calculated starting and ending points. */
    start_seq = start_n_end->start_seq;
    start_off = start_n_end->start_off;
    end_seq = start_n_end->end_seq + 1;
    end_off = start_n_end->end_off;
    incr = 1;

  } else {
    /* Don't parallelize this loop. */
    start_seq = 0;
    end_seq = n_samples;
    incr = 1;
  }

  /* Divide the various samples among processors. */
  for (iseq = start_seq; iseq < end_seq; iseq += incr) { /* sequence */
#else /* not PARALLEL */
  for (iseq = 0; iseq < n_samples; iseq++) {	/* sequence */
#endif /* PARALLEL */

    SAMPLE *s = samples[iseq];
    int lseq = s->length;
    char *res = s->res;
    char *name = s->sample_name;
    double *not_o = s->not_o;
    int max_off, init_off;

#ifdef PARALLEL
    if (mpMyID() == 0)
#endif
    if ((!NO_STATUS) && ((iseq % 5) == 0))
      fprintf(stderr, "\rstarts: w=%d, seq=%d, l=%d          ", w, iseq, lseq); 
    /* Set the appropriate starting and ending points. */
#ifdef PARALLEL
    if (((PARALLEL == 1) || (PARALLEL == 3)) && (iseq == start_seq))
      init_off = start_off;
    else
#endif
      init_off = 0;

#ifdef PARALLEL
    if (((PARALLEL == 1) || (PARALLEL == 3)) && (iseq == end_seq))
      max_off = MIN(end_off, lseq - w);
    else
#endif
      max_off = lseq - w;

    /*
      Loop over all subsequences in the current sequence testing them
      each as "starting points" (inital values) for theta
    */
    for (ioff = init_off; ioff <= max_off; ioff++) {/* subsequence */ 
      int flags = trylist[++isample];

      /* warning: always do the next step; don't ever
         "continue" or the value of pY will not be correct since
         it is computed based the previous value 
      */

      /* convert subsequence in dataset to starting point for EM */
      init_theta_1(w, res+ioff, &ltheta[1][0], lmap);

      if (ioff == init_off) { 			/* new sequence */

        /* Compute p(Y_ij | theta_1^0) */
        get_pY(dataset, &ltheta[1][0], w);

      } else {					/* same sequence */

        /* get theta[0][0]^{k-1} */
        init_theta_1(1, res+ioff-1, &ltheta[0][0], lmap);
        
        /* Compute p(Y_ij | theta_1^k) */
        next_pY(dataset, &ltheta[1][0], w, &ltheta[0][0][0]);

      }

      /* skip if no value of nsites0 to be tried for this sample */
      if (flags == 0) { continue; } 

      /* skip if there is a high probability that this subsequence
         is part of a site which has already been found 
      */
      if (not_o[ioff] < MIN_NOT_O) {continue;} 

      /* get a sorted list of the maxima of pY */
      n_maxima = sort_max(mtype, dataset, w, maxima);

      /* align the top nsites0 subsequences for each value
         of nsites0 and save the alignments with the highest likelihood 
      */
      n_starts += align_top_subsequences(w, dataset, iseq, ioff, 
        res+ioff, name, n_nsites0, n_maxima, maxima, flags, bit_mask, s_points);

    } /* subsequence */
  } /* sequence */

  if ((TIMER == 1) || (TIMER == 3))
    *loop1_time += (myclock() - loop1_clock)/1E6;

#ifdef PARALLEL
  if ((PARA == 1) || (PARA == 3))
    reduce_across_s_points(s_points, samples, n_nsites0, n_starts);
#endif /* PARALLEL */

  /* remove starting points that were not initialized from s_points */
  for (i=0; i<n_nsites0; i++) {
    if (s_points[i].score == LITTLE) {
      for (j=i; j<n_nsites0; j++) s_points[j] = s_points[j+1];
      n_nsites0--;
      i--;
      /*fprintf(stderr,"\nremoving starting points: i=%d\n", i);*/
    }
  }

  if (TRACE) printf("Tested %d possible starts...\n", n_starts);

  myfree(maxima);
  myfree(trylist);

  return n_nsites0;
}

/**********************************************************************/
/*
	get_pY

	Compute the p(Y_ij | theta_1)

*/
/**********************************************************************/
static void get_pY(
  DATASET *dataset,			/* the dataset */
  LOG_THETAG_TYPE(theta_1),		/* integer log theta_1 */
  int w					/* width of motif */
)
{
  int i, j, k;
  int n_samples = dataset->n_samples;
  SAMPLE **samples = dataset->samples;
  
  for (i=0; i < n_samples; i++) { 	/* sequence */
    SAMPLE *s = samples[i];
    int lseq = s->length;
    char *res = s->res;                 /* integer sequence */
    int *pY = s->pY[0];			/* p(Y_j | theta_1) */
    int *log_not_o = s->log_not_o;	/* prob site won't overlap prior site */
    for (j=0; j <= lseq - w; j++) {	/* site start */
      char *r = res + j;
      int p = log_not_o[j];
      for (k=0; k < w; k++) {		/* position in site */
	p += theta_1[k][(int)*r++];
      }
      pY[j] = p;
    }
    for (j=lseq-w+1; j<lseq; j++) pY[j] = 0;	/* impossible positions */
  }
}

/**********************************************************************/
/*
	next_pY

	Compute the value of p(Y_ij | theta_1^{k+1})
	from p(Y_ij | theta_1^{k} and the probability
	of first letter of Y_ij given theta_1^k,
	p(Y_ij^0 | theta_1^k).
*/
/**********************************************************************/
static void next_pY(
  DATASET *dataset,			/* the dataset */
  LOG_THETAG_TYPE(theta_1),		/* integer log theta_1 */
  int w,				/* width of motif */
  int *theta_0				/* first column of previous theta_1 */
)
{
  int i, k;
  int *theta_last = theta_1[w-1];	/* last column of theta_1 */
  int n_samples = dataset->n_samples;
  SAMPLE **samples = dataset->samples;
  
  for (i=0; i < n_samples; i++) { 	/* sequence */
    SAMPLE *s = samples[i];		/* sequence */
    int lseq = s->length;		/* length of sequence */
    char *res = s->res;                 /* integer sequence */
    int *pY = s->pY[0];			/* p(Y_j | theta_1) */
    char *r = res+lseq-1;	/* last position in sequence */
    char *r0 = res+lseq-w-1;	/* prior to start of last subsequence */
    int *log_not_o = &(s->log_not_o[lseq-w]);/* last position not_o */
    int j, p;

    /* calculate p(Y_ij | theta_1) */
    for (j=lseq-w; j > 0; j--) {
      int new = *log_not_o;
      pY[j] = pY[j-1] + theta_last[(int)*r--] - theta_0[(int)*r0--] 
        + new - *(--log_not_o);
    }

    /* calculate p(Y_i0 | theta_1) */
    p = log_not_o[0];
    r = res;
    for (k=0; k < w; k++) {     	/* position in site */
      p += theta_1[k][(int)*r++];
    }
    pY[0] = p;
  }
}

/**********************************************************************/
/*
	sort_max

	Find the maxima of pY and sort them.  Returns the length of the
	sorted list of maxima.

	For the oops an zoops models, one maximum is found per sequence.
	For the oops model, the list is not sorted, since all
	maxima will be used

	For the tcm model, all non-overlapping local maxima are found
	and sorted.
*/
/**********************************************************************/
static int sort_max(
  MTYPE mtype,		/* the model type */
  DATASET *dataset,	/* the dataset */
  int w,		/* width of sites */ 
  P_PROB maxima 	/* array of encoded site starts of local maxima */
)
{
  int n_maxima;

  if (mtype == Oops || mtype == Zoops) {
    global_max(dataset, w, maxima);
    n_maxima = dataset->n_samples;		/* one maxima per sample */
  } else {
    n_maxima = local_max(dataset, w, maxima);
  }

  if (mtype != Oops) {
    /* sort the local maxima of pY[1] */
    qsort((char *) maxima, n_maxima, sizeof(p_prob), pY_compare);
  }

  return n_maxima;
}

/**********************************************************************/
/*
	global_max	

	Find the position in each sequence with the globally maximal
	value of pY.

	Returns the updated array of maxima positions.
*/
/**********************************************************************/
static void global_max(
  DATASET *dataset,	/* the dataset */
  int w,		/* length of sites */ 
  P_PROB maxima 	/* array of encoded site starts of local maxima */
)
{
  int i;
  SAMPLE **samples = dataset->samples;		/* datset samples */
  int n_samples = dataset->n_samples;		/* number samples in dataset */

  /* find the position with maximum pY in each sequence */
  for (i=0; i<n_samples; i++) {			/* sequence */
    SAMPLE *s = samples[i];
    int lseq = s->length;
    int *pY = s->pY[0];				/* p(Y_j | theta_1) */
    int max = *pY;				/* initial maximum */
    int max_j = 0;				/* start of maximal subseq */
    int last_j = lseq-w;			/* start of last subseq */
    int j;

    for (j=0; j<=last_j; j++, pY++) {		/* subsequence */
      if (*pY > max) {				/* new maximum found */
        max = *pY;
        max_j = j;
      }
    }

    /* record the maximum for this sequence */
    maxima[i].x = i;
    maxima[i].y = max_j;
    maxima[i].prob = max ;

  }
}

/**********************************************************************/
/*
	local_max

	Find the local maxima of p(Y_ij | theta_1) 
	subject to the constraint that
	they are separated by at least w positions. 

	Returns the number of local maxima found and the
	updated array of maxima positions.
*/
/**********************************************************************/
static int local_max(
  DATASET *dataset,	/* the dataset */
  int w,		/* length of sites */ 
  P_PROB maxima 	/* array of encoded site starts of local maxima */
)
{
  int i, j, next_j, n_maxima;
  int max;
  SAMPLE **samples = dataset->samples;		/* datset samples */
  int n_samples = dataset->n_samples;		/* number samples in dataset */
  
  /* Find the non-overlapping local maxima of p(Y_ij | theta_1) */
  n_maxima = 0;
  for (i=0; i<n_samples; i++) {			/* sequence */
    SAMPLE *s = samples[i];
    int lseq = s->length;
    int *pY = s->pY[0];				/* p(Y_j | theta_1) */
    max = pY[0];
    maxima[n_maxima].x = i;			/* candidate */
    maxima[n_maxima].y = 0;		
    maxima[n_maxima].prob = max ;
    next_j = MIN(w, lseq-w);			/* next possible maximum */
    for (j=0; j<=lseq-w; j++) {			/* subsequence */
      if (j == next_j) {			/* candidate not exceeded */
        max = pY[j];
        maxima[++n_maxima].x = i;		/* save the maximum */
        maxima[n_maxima].y = j;	
        maxima[n_maxima].prob = max ;
        next_j = MIN(j + w, lseq-w);		/* next possible maximum */
      } else if (pY[j] > max) {			/* candidate exceeded */
        max = pY[j];
        maxima[n_maxima].x = i;			/* overwrite the candidate */
        maxima[n_maxima].y = j;	
        maxima[n_maxima].prob = max ;
        next_j = MIN(j + w, lseq-w);		/* next possible maximmum */
      }
    }
  }
  return n_maxima;
}

/**********************************************************************/
/*
        pY_compare

        Compare the pY of two start sequences.  Return <0 0 >0
        if the first pY is <, =, > the first pY.
*/
/**********************************************************************/
static int pY_compare(
  const void *v1, 
  const void *v2 
)
{
  int result;

  const struct p_prob * s1 = (const struct p_prob *) v1; 
  const struct p_prob * s2 = (const struct p_prob *) v2; 
  result = s2->prob - s1->prob;

  /* Impose an arbitrary total ordering so that the sorted output
     will be identical on different architectures. (Useful in debugging.) */
  if (result == 0) {
    if ((s2->x - s1->x) == 0)
      return s2->y - s1->y;
    else
      return s2->x - s1->x;
  }
  else
    return result;
}

/**********************************************************************/
/*
	init_theta_1

	Convert a subsequence to a motif.

	Uses globals:
*/
/**********************************************************************/
static void init_theta_1(
  int w,			/* width of site */
  char *res,			/* (encoded) letters of subsequence */
  LOG_THETAG_TYPE(theta_1),	/* theta_1 */
  int lmap[MAXALPH][MAXALPH] 	/* matrix of frequency vectors */ 
)
{
  int m;
  for (m=0; m < w; m++) {
    theta_1[m] = lmap[(int)res[m]];
  }
/*
  printf("%d \n", res[0]);
  for (m=0; m<alength; m++) printf("%5d ", theta_1[0][m]);
  printf("\n");
*/
  
}

/**********************************************************************/
/*
	align_top_subsequences

     	Align the top nsites0 subsequences for each value
	of nsites0 and save the alignments with the
	highest likelihood.

	Returns number of values of nsites0 tried.

	Uses globals:
		lsite
		alength
		samples
*/ 
/**********************************************************************/
static int align_top_subsequences(
  int w,				/* width of motif */
  DATASET *dataset,			/* the dataset */
  int iseq,				/* sequence number of starting point */
  int ioff,				/* sequence offset of starting point */
  char *eseq,				/* integer encoded subsequence */
  char *name,				/* name of sequence */
  int n_nsites0,			/* number of nsites0 values to try */
  int n_maxima,				/* number of local maxima */
  P_PROB maxima,			/* sorted local maxima indices */
  int flags,				/* bit flags of nsites0 to try */
  int bit_mask[],			/* bit masks */
  S_POINT s_points[]			/* array of starting points */
)
{
  int i, j, k, i_nsites0;
  int next_seq;				/* index of next subsequence to align */
  int n_starts = 0;			/* number of nsites0 tried */
  double sup;				/* score of start */
  int nsites0;				/* starting nsites rounded down */
  int alength = dataset->alength;	/* lenght of alphabet */
  /* tlb 5-9-97; wgt */
  int wgt_total_res = dataset->wgt_total_res;	/* weighted size of dataset */
  double *res_freq = dataset->res_freq;	/* \vmu */
  SAMPLE **samples = dataset->samples;	/* the sequences */
  /* tlb 5-9-97; double */
  double counts[MAXSITE+1][MAXALPH];	/* array to hold observed counts */
  double total1 = 0;			/* total count for motif col */

  /* init background counts to total counts */
  for (i=0; i < alength; i++) {
    counts[0][i] = res_freq[i] * wgt_total_res + 1;
  }

  /* init motif counts to zero */
  for (i=1; i < w+1; i++) for (j=0; j < alength; j++) counts[i][j] = 0;

  /* calculate the score of the top nsite0 probability positions */
  for (i_nsites0=0, next_seq=0; i_nsites0 < n_nsites0; i_nsites0++) {

    /* don't score this start if not tagged for this value of NSITES0 */
    if (! (flags & bit_mask[i_nsites0]) ) { continue; }

    /* don't score this start if not enough maxima found */
    nsites0 = (int) s_points[i_nsites0].nsites0;	/* round down */
    if (n_maxima < nsites0) { continue; }

    n_starts++;					/* number of nsites0 tried */

    /* Align the next highest probability sites and count the number
       of occurrences of each letter in each column of the motif
       and the background.
    */
    for (k=next_seq; k < nsites0; k++) {
      int off = maxima[k].y;
      SAMPLE *s = samples[maxima[k].x];
      double sw = s->sw;			/* sequence weight */
      char *res = s->res;                       /* integer sequence */
      int jj;
      for (j=off, jj=1; j<off+w; j++, jj++) {	/* position in sequence */
        int r = res[j];                         /* residue at position */
        counts[0][r] -= sw;			/* background model */
        counts[jj][r] += sw;			/* motif model */
      }
      total1 += sw;
    } 
    next_seq = k;				/* next subsequence to align */

    /* convert COUNTS to FREQUENCIES and calculate E[l(theta_1 | X, Z) */
    {
      double loglike0 = 0;
      double loglike1 = 0;
      double total0 = wgt_total_res - (w * total1);	/* total background */
      if (total0 <= 0) total0 = 1;		/* no bg;avoid divide by zero */

      for (i=0; i <= w; i++) {
        /* update likelihood for position i */
        for (j=0; j < alength; j++) {
          if (i==0) {
            double f = counts[i][j] / total0;
            loglike0 += f * LOGL(f);		/* background likelihood */
          } else {
            double f = counts[i][j] / total1;	/* letter frequency */
            loglike1 += f * LOGL(f);		/* motif likelihood */
          }
        }
      }
      print_once("Using LOGL in align_top_subsequences\n"); 
      sup = (total1 * loglike1) + ((wgt_total_res - (w * total1)) * loglike0);
    } 

    /* print the start sequence and other stuff */
    if (TRACE) {
      char seq[MAXSITE+1];
      r2seq(seq, eseq, w);
      printf("( %3d %3d ) ( %*.*s ) %.*s --> %s score %8.4f nsites0 %6d\n",
        iseq+1, ioff+1, MSN, MSN, name, w, seq, "NA", sup, nsites0);
    }

    /* save the best start */
    if (BETTER_SUP(sup, s_points[i_nsites0].score)) {
      /* Save the starting point and offset so we can re-calculate
	 eseq later. */
      s_points[i_nsites0].iseq = iseq;
      s_points[i_nsites0].ioff = ioff;
      s_points[i_nsites0].e_cons0 = eseq;
      s_points[i_nsites0].score = sup;
    }

  } /* nsites0 */

    return n_starts;
}
