/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/**********************************************************************/
/*
	Display routines for the results of MEME
*/
/**********************************************************************/

#include "ajax.h"
#include "meme.h"
#include "split.h"

/* stepsize and size of smoothing window for get_q */
#define NSTEPS 100
/*#define WINDOW (NSTEPS/10)*/
#define WINDOW 0

/* round double to integer; round up if half way between */
/* only works for positive numbers */
#ifndef NINT
  #define NINT(x) ((int) ((x)+0.5))
#endif

/* maximum number of levels in consensus sequence */
#define MAXDEPTH ((int) (1.0/MINCONS))

/* encode and decode a (seq #, offset) pair as one integer */
/* this will bomb if seq. length > MAX_SEQ_LEN or
   if number of sequences * MAX_SEQ_LEN exceeds size of int 
*/
#define MAX_SEQ_LEN 10000
#define ENCODE(I,J) (I) * MAX_SEQ_LEN + (J);
#define DECODE(N, SEQ, OFF) {\
  (OFF) = (N) % MAX_SEQ_LEN;\
  (SEQ) = ((N) - (OFF)) / MAX_SEQ_LEN;\
}

/* distance to indent start of IC histogram, consensus and simplified motif */
#define IND  13
#define IND2 3

char *stars = NULL;

/* motif occurrence in sequence */
typedef struct {
  int seqno;		/* index in samples array */
  int pos;		/* character position of site in sequence */
  double zij;		/* value of z_ij */
  int d;		/* strand/direction of site */
} SITE;

/* record describing accuracy of motif */
typedef struct {
  double thresh;	/* optimal threshold */
  int err;		/* classification errors using thresh */
  double roc;		/* ROC */
} ACCURACY;

/* sortable sequence score record */
typedef struct {
  double score;		/* sequence score */
  int class;		/* class of sequence,  1=pos, 0=neg */
  char *id;
} SORTED_SCORE;

/* local functions */
static void print_sites(
  DATASET *dataset,			/* the dataset */
  MODEL *model,				/* the model */
  int format,				/* 0=BLOCKS; 1=FASTA */
  char *com,				/* comment to append */
  FILE *outfile				/* where to print */
);
static SITE *get_sites(
  DATASET *dataset,			/* the dataset */
  MODEL *model,				/* the model */
  int *n				/* number of sites found */
);
static void print_log_odds(
  DATASET *dataset,		/* the dataset */
  int w,			/* width of motif */
  BOOLEAN pair,			/* double matrix if true */
  LOGODDS logodds,		/* log-odds matrix */
  double bayes			/* threshold */
);
static void top_align(
  LOGODDS logodds,		/* log-odds matrix */
  int w,			/* width of motif */
  DATASET *dataset,		/* the dataset */
  MODEL *model,			/* the model */
  double threshold,		/* the bayes optimal threshold */
  FILE *outfile			/* where to print */
);
static void motif_entropy(
  THETA theta,			/* the motif */
  LOGODDS logodds,		/* motif logodds matrix */
  int w, 			/* width of motif */
  char *str_space,		/* space for printing strand direction */
  DATASET *dataset,		/* the dataset */
  FILE *outfile			/* stream for output */
);
static LOGODDS make_log_odds(
  THETA theta1,			/* motif theta */
  THETA theta0,			/* negative theta; use 0 if NULL */
  double *back,			/* background frequencies; use 0 if NULL */
  double q,			/* mixing parameter for theta0 and back */
  int w,			/* width of motif */
  int alength 			/* length of alphabet */
);
static void print_candidates(
  CANDIDATE *candidates, 	/* list of candidate models */
  DATASET *dataset,		/* the dataset */
  int max_w			/* maximum width for motifs */
);
static ACCURACY *get_thresh(
  int w,				/* width of motif */
  LOGODDS logodds1,			/* log-odds matrix: LOG2(m_ij/b_j) */
  LOGODDS logodds2,			/* log-odds matrix: LOG2(m_ij/a_ij) */
  DATASET *pos,				/* positive examples */
  DATASET *neg,				/* negative examples */
  BOOLEAN print_scores			/* print sorted scores */
);
static double score_sequence(
  char *eseq,		/* integer-coded sequence to score */
  int length,		/* length of the sequence */
  int w, 		/* width of motif */
  LOGODDS logodds1, 	/* log-odds matrix: LOG2(m_ij/b_j) */
  LOGODDS logodds2  	/* log-odds matrix: LOG2(m_ij/n_ij) */
);
static int s_compare(
  const void *v1,
  const void *v2
);
static double get_q(
  int nsteps,					/* try nsteps from 0 to 1 */
  int window,					/* smoothing window radius */
  int w,					/* width of motif */
  THETA theta,					/* motif theta */
  THETA neg_theta, 				/* anti-motif theta */
  double *back,					/* background motif */
  DATASET *dataset,				/* the dataset */
  DATASET *neg_dataset,				/* negative examples */
  char *str_space		/* space for printing strand direction */
);


/*void PSTARRS()
{
    int i;

    for(i=0;i<PAGEWIDTH;++i)
	ajFmtPrintF(outf,"*");
    ajFmtPrintF(outf,"\n");
    return;
}*/
extern void PSTARRS(void);



/**********************************************************************/
/*
	Print the results of EM
*/
/**********************************************************************/
extern void print_results(
  DATASET *dataset,		/* the dataset */
  DATASET *neg_dataset,		/* negative examples */
  MODEL *model,			/* the model */
  MODEL *neg_model, 		/* negative model */
  int max_w,			/* maximum width tried */
  P_POINT *p_point,		/* previous components starts */
  CANDIDATE *candidates		/* candidate models found */
)
{
  int i, c;
  int nc = model->c;				/* # of components in model */
  int nstrands = model->nstrands;		/* # of strands to use */
  int w = model->w[nc-1];			/* width of last component */
  double lambda = model->lambda[nc-1];		/* lambda of last component */
  double nsites = lambda * ps(dataset, w);	/* estimated number of sites */
#ifndef PARALLEL
  S_POINT *s_point = candidates[w].s_point;	/* starting point for model */
#endif
  double m1, e1, m2, e2, m3, e3;		/* for printing significance */
  char *str_space = (nstrands == 1) ? "" : "       ";
  BOOLEAN pair = neg_dataset ? neg_dataset->negtype == Pair : FALSE;
  BOOLEAN blend = neg_dataset ? neg_dataset->negtype == Blend : FALSE;

  /* print the significant models */
#ifdef DEBUG 
  print_candidates(candidates, dataset, max_w);
#else
  if (0) print_candidates(candidates, dataset, max_w);
#endif

  /* print the results for the model as a whole */
  if (pair || blend) {			/* negative motifs */
    double n_nsites = nsites(neg_dataset, w, neg_model->lambda[1]);
    ajFmtPrintF(outf, "\n\n%s\nMOTIF%3d\t\twidth = %3d\tsites = %4.1f\tnegative sites = %4.1f\n%s\n", 
      stars, model->imotif, w, nsites, n_nsites, stars);
  } else {
  ajFmtPrintF(outf,
   "\n\n%s\nMOTIF%3d\t\twidth = %3d\tsites = %4.1f\n%s\n", 
    stars, model->imotif, w, nsites, stars);
  }

  if (VERBOSE) {
    exp10_logx(model->sig, m1, e1);
    ajFmtPrintF(outf,
      "\n(best) e_ll_0 %6.0f e_ll %6.0f ll %6.0f sig %5.3fe%+04.0f iter %d\n",
      model->e_ll_0, model->e_ll, model->ll, m1, e1, model->iter);
    exp10_logx(model->lrt, m1, e1);
    exp10_logx(model->bon, m2, e2);
    exp10_logx(model->root, m3, e3);
    ajFmtPrintF(outf,
     "(best) lrt %5.3fe%+04.0f bonferroni %5.3fe%+04.0f root %5.3fe%+04.0f\n",
       m1, e1, m2, e2, m3, e3); 
  }

  /* print results for each motif */
  for (c = 1; c < nc; c++) {
    THETA theta = model->theta[c];
    THETA neg_theta = neg_model ? neg_model->theta[c] : NULL;
    double *lambda = model->lambda;
    int w = model->w[c];
    LOGODDS logodds, logodds1;
    ACCURACY *acc=NULL;
    double threshold;
    FILE *outfile;
    char *cons = model->cons;
    char *cons0;
    char local_cons0[MAXSITE + 1];
    int alength = dataset->alength;
    double *back = dataset->nrdb_freq;
    print_once("Using nrdb frequencies as background in log-odds matrix.\n");

    if (c == nc-1) {
#ifdef PARALLEL
      /* In the second parallelized loop, we don't keep track of the 
	candidates, so the consensus string is stored in the model instead. 
      */
      cons0 = model->cons0;
#else
      cons0 = s_point->cons0;
#endif
    } else {
      cons0 = local_cons0;
      for (i=0; i<w; i++) cons0[i] = unhash(p_point->e_cons0[c][i]);
      cons0[i] = 0;
    }

    if (VERBOSE) {
      ajFmtPrintF(outf,"\n(best) %s --> %s\n", cons0, cons);
      ajFmtPrintF(outf,"(best) w %3d nsites %5.1f lambda %8.7f IC/col %6.3f\n",
	w, lambda[c]*ps(dataset, w), lambda[c], model->rel[c]);
      ajFmtPrintF(outf,"\n(best) IC %6.3f\n\n", w * model->rel[c]);
    }
    if (model->pal) ajFmtPrintF(outf,"Palindrome\n\n");
 
    /* print the simplified (1-digit) motif */
    /* make the log-odds matrices */
    /* calculate the optimal threshold (min classification error or Bayes' */
    /* print the IC of the motif as a bar graph */
    if (blend) {			/* single blended motif */
      /* get mixing parameter */
      double q = get_q(NSTEPS, WINDOW, w, theta, neg_theta, back,
	dataset, neg_dataset, str_space);
      /* get threshold and performance of combined motif */
      logodds = make_log_odds(theta, neg_theta, back, q, w, alength);
      acc = get_thresh(w, logodds, NULL, dataset, neg_dataset, FALSE);
      threshold = acc->thresh;
      free(acc);
      ajFmtPrintF(outf,"\t\tPOSITIVE MOTIF\n");
      print_theta(2, theta, w, str_space, dataset, stdout);
      ajFmtPrintF(outf,"\t\tNEGATIVE MOTIF\n");
      print_theta(2, neg_theta, w, str_space, dataset, stdout);
      motif_entropy(theta, logodds, w, str_space, dataset, stdout);
    } else if (pair) {			/* pair of motifs, pos and neg */
      logodds = make_log_odds(theta, NULL, back, 0, w, alength);
      logodds1 = make_log_odds(neg_theta, NULL, back, 0, w, alength);
      threshold = LOG2((lambda[0])/lambda[c]);	/* Bayes' threshold */
      ajFmtPrintF(outf,"\t\tPOSITIVE MOTIF\n");
      print_theta(2, theta, w, str_space, dataset, stdout);
      motif_entropy(theta, logodds, w, str_space, dataset, stdout);
      ajFmtPrintF(outf,"\t\tNEGATIVE MOTIF\n");
      print_theta(2, neg_theta, w, str_space, dataset, stdout);
      motif_entropy(neg_theta, logodds1, w, str_space, dataset, stdout);
      myfree(logodds1);
      /* combine the matrices by appending P/N to P/B */
      logodds1 = make_log_odds(theta, neg_theta, NULL, 1, w, alength);
      Resize(logodds, 2*w, double *);
      for (i=0; i<w; i++) logodds[w+i] = logodds1[i];
      myfree(logodds1);
    } else {				/* no negative motifs */
      logodds = make_log_odds(theta, NULL, back, 0, w, alength);
      threshold = LOG2((lambda[0])/lambda[c]);	/* Bayes' threshold */
      print_theta(2, theta, w, str_space, dataset, stdout);
      motif_entropy(theta, logodds, w, str_space, dataset, stdout);
    }

    /* print the sites "making up" the model */
    if (pair || blend) {
      print_sites(dataset, model, PRINT_FASTA, " (positive motif)", stdout);
      print_sites(neg_dataset, neg_model, PRINT_FASTA, " (negative motif)", 
        stdout);
    } else {
      print_sites(dataset, model, PRINT_FASTA, "", stdout);
    }

    /* set up file for printing PAGEWIDTH columns wide */
    outfile = split(0, NULL, 0);

    /* print the alignment of the highest scoring sites */
    if (!NOALIGN)
      top_align(logodds, w, dataset, model, threshold, outfile);

    /* print the alignment in swaths of PAGEWIDTH char */
    (void) split(1, outfile, PAGEWIDTH);

    /* print the logodds matrix */
    print_log_odds(dataset, w, pair, logodds, threshold);

    /* print the probability matrix */
    print_theta(1, theta, w, str_space, dataset, NULL);
    if (pair || blend) print_theta(1, neg_theta, w, str_space, dataset, NULL);

    /* flush */
    fflush(stdout);

  }
}

/**********************************************************************/
/*
	print_log_odds

	Print the log-odds matrix 
*/
/**********************************************************************/
static void print_log_odds(
  DATASET *dataset,		/* the dataset */
  int w,			/* width of motif */
  BOOLEAN pair,			/* double matrix if true */
  LOGODDS logodds,		/* log-odds matrix */
  double bayes			/* threshold */
)
{
  int i, j;
  int alength = dataset->alength;	/* length of alphabet */
  int n = ps(dataset, w);		/* possible sites */
  char *type = pair ? "pair" : "";/* type of matrix */

  /* double w if printing a matrix pair */
  if (pair) w *=2;

  ajFmtPrintF(outf,"log-odds matrix: alength= %d w= %d n= %d bayes= %g %s\n", 
    alength, w, n, bayes, type);

  for (i=0; i < w; i++) {		/* site position */
    for (j=0; j < alength; j++) {	/* letter */
      ajFmtPrintF(outf,"%7.3f ", logodds(i,j));
    }
    ajFmtPrintF(outf,"\n");
  }
}

/**********************************************************************/
/*
	top_align

	Align the all sites which score over bayes-optimal threshold
	and display them. 
*/
/**********************************************************************/
static void top_align(
  LOGODDS logodds,		/* log-odds matrix */
  int w,			/* width of motif */
  DATASET *dataset,		/* the dataset */
  MODEL *model,			/* the model */
  double threshold,		/* the bayes optimal threshold */
  FILE *outfile			/* where to print */
)
{
  int i;
  int positives = 0;			/* number of scores above threshold */
  SAMPLE **samples = dataset->samples;	/* samples */
  int n_samples = dataset->n_samples;	/* number of samples in dataset */
  BOOLEAN *dir = model->d;		/* strand directions to use */
  /*int nstrands = model->nstrands;*/	/* number of strands to use */
  int imotif = model->imotif;		/* motif number */
  /*char *str_space = (nstrands == 1) ? "" : "       ";*/

  if (VERBOSE) {
    ajFmtPrintF(outf,"Bayes optimal threshold for information content scores = %8.4f\n",
      threshold);
  }

  /* calculate the IC scores of each subsequence in the dataset */
  for (i=0; i<n_samples; i++) {
    SAMPLE *s = samples[i];
    int lseq = s->length;                       /* length this seq */
    char *name = s->sample_name;		/* sample name */
    char *eseq = s->res;
    double *scores = (double *) malloc(4 * lseq * sizeof(double));

    if (i == n_samples - 1) imotif *= -1;

    positives += 
      align(imotif, logodds, -i, threshold, name, eseq, dir, lseq, w, 
        scores, outfile);

    myfree(scores);
  }
}

/**********************************************************************/
/*
	motif_entropy

	Display the relative entropy of each column of the motif
	as a bar graph.
*/
/**********************************************************************/
static void motif_entropy(
  THETA theta,			/* the motif */
  LOGODDS logodds,		/* motif logodds matrix */
  int w, 			/* width of motif */
  char *str_space,		/* space for printing strand direction */
  DATASET *dataset,		/* the dataset */
  FILE *outfile			/* stream for output */
)
{
  int i, j;
  double rel_entropy[MAXSITE];
  double level;
  double maxre;
  double min_freq;
  int alength = dataset->alength;
  double *back = dataset->nrdb_freq;
  char icstring[15];
  double ic;				/* motif information content */
  char *consensus;			/* consensus strings */
  
  /* calculate the maximum possible relative entropy */
  /* this occurs when the least probable letter occurs all the time;
     only consider frequencies which are > 0 */
  min_freq = BIG;
  for (i=1; i<alength; i++) 
    if (back[i] > 0) min_freq = MIN(min_freq, back[i]);
  maxre = LOG2(1.0/min_freq); 

  /* calculate the relative entropy of each column in theta */
  for (i=0, ic=0; i<w; i++) {			/* position */
    rel_entropy[i] = 0.0; 
    for (j=0; j<alength; j++) {		/* letter */
      double f = theta(i, j);		/* motif freq */
      rel_entropy[i] +=  f * logodds(i,j);
    }
    ic += rel_entropy[i];
  }

  /* create string containing IC */
  sprintf(icstring, "(%.1f bits)", ic); 

  /* print the relative entropy of each column as a bar graph */
  if (VERBOSE) {
/*    fprintf(outfile, "Information content of positions in motif:\n");*/
      ajFmtPrintF(outf, "Information content of positions in motif:\n");
  }
  for (i=0; i<10; i++) {
    level = maxre - (i * maxre/10.0);
/*    fprintf(outfile, (i==0 ? "%*.*s %*.1f " : "%-*.*s %*.1f "), IND, IND, 
      (i==0 ? "bits" : i==4 ? "Information" : i==5 ? "content" : 
        i==6 ? icstring : ""), IND2, level);*/
    ajFmtPrintF(outf, (i==0 ? "%*.*s %*.1f " : "%-*.*s %*.1f "), IND, IND, 
      (i==0 ? "bits" : i==4 ? "Information" : i==5 ? "content" : 
        i==6 ? icstring : ""), IND2, level);
    for (j=0; j<w; j++) if (rel_entropy[j] >= level){
/*      fputc('*', outfile);*/
	ajFmtPrintF(outf,"*");
    } else {
/*      fputc(' ', outfile);*/
	ajFmtPrintF(outf," ");
    }
/*    fputc('\n', outfile);*/
    ajFmtPrintF(outf,"\n");
  }
/*  fprintf(outfile, "%-*.*s %*.1f ", IND, IND, "", IND2,0.0);*/
  ajFmtPrintF(outf, "%-*.*s %*.1f ", IND, IND, "", IND2,0.0);
  for (i=0; i<w; i++) /*fputc('-', outfile);*/
      ajFmtPrintF(outf,"-");
  ajFmtPrintF(outf,"\n\n");
/*  fprintf(outfile, "\n\n");*/
  /* get the consensus sequences */
  consensus = get_consensus(theta, w, dataset, MAXDEPTH, MINCONS);
  for (i=0; i<MAXDEPTH && i<alength; i++) {/* print next levels of consensus */
/*    fprintf(outfile, "%-*.*s %*.0s %*.*s\n", IND, IND, 
      (i==0 ? "Multilevel" : i == 1 ? "consensus" : i == 2 ? "sequence" : ""), 
      IND2, "", w, w, consensus+(i*w));*/
    ajFmtPrintF(outf, "%-*.*s %*.0s %*.*s\n", IND, IND, 
      (i==0 ? "Multilevel" : i == 1 ? "consensus" : i == 2 ? "sequence" : ""), 
      IND2, "", w, w, consensus+(i*w));
  }
  /* free up space */
  myfree(consensus);
} /* motif entropy */

/**********************************************************************/
/*
	print_theta

		format=1		floating point; pos x letter
		format=2		1 digit; letter x pos 

	Print the probability array.
*/
/**********************************************************************/
extern void print_theta(
  int format,		/* 1 = floating point
              		   2 = integer */ 
  THETA theta,		/* theta */
  int w,		/* width of motif */
  char *str_space,	/* space for printing strand direction */
  DATASET *dataset,	/* the dataset */ 
  FILE *outfile	 	/* NULL stdout; otherwise, print to split file */
)
{
  int i, j;
  int alength = dataset->alength;
  int possible_sites = ps(dataset, w); 

  if (outfile == NULL) outfile = stdout;

  if (format == 1) {
    /* print theta: rows = position; cols = letter; floating point */
/*    fprintf(outfile,
      "\nletter-probability matrix: alength= %d w= %d n= %d",
      alength, w, possible_sites);
    fprintf(outfile, "\n");*/
    ajFmtPrintF(outf,
      "\nletter-probability matrix: alength= %d w= %d n= %d",
      alength, w, possible_sites);
    ajFmtPrintF(outf, "\n");
    for (i=0; i < w; i++) {
      for (j=0; j < alength; j++) {
/*	fprintf(outfile, "%9.6f ", theta(i, j));*/
	ajFmtPrintF(outf, "%9.6f ", theta(i, j));
      }
/*      fprintf(outfile, "\n");*/
      ajFmtPrintF(outf, "\n");
    }

  } else if (format == 2) {
    /* print theta: rows = letter; cols = position in motif; 1-digit integer */
    for (i=0; i < alength; i++) {
      /* print the letter */
/*      fprintf(outfile, "%-*.*s%*c  ", IND, IND, 
        (i==0 ? "Simplified" : i==1 ? "motif letter-" : i==2 ? "probability" :
          i==3 ? "matrix" : "" ), IND2, unhash(i));	*/
      ajFmtPrintF(outf, "%-*.*s%*c  ", IND, IND, 
        (i==0 ? "Simplified" : i==1 ? "motif letter-" : i==2 ? "probability" :
          i==3 ? "matrix" : "" ), IND2, unhash(i));
      for (j=0; j < w; j++) {
	int k = NINT(10.0 * theta(j,i));	/* round to 1 digit */
        if (k == 0) {
/*          fprintf(outfile, ":");*/		/* print 0 as colon */
          ajFmtPrintF(outf, ":");		/* print 0 as colon */
        } else {
/*          fprintf(outfile, "%1x", k);*/		/* print 1 digit */
          ajFmtPrintF(outf, "%1x", k);		/* print 1 digit */
        }

      }
/*      fprintf(outfile, "\n");*/
      ajFmtPrintF(outf, "\n");
    }
  }
  ajFmtPrintF(outf, "\n");
/*  fprintf(outfile, "\n");*/
}

/**********************************************************************/
/*
	print_zij
*/
/**********************************************************************/
extern void print_zij(
  DATASET *dataset,			/* the dataset */
  MODEL *model				/* the model */
)
{
  int i, j;
  int n_samples = dataset->n_samples;
  SAMPLE **samples = dataset->samples;

  ajFmtPrintF(outf,"z_ij: \n");
  for (i=0; i<n_samples; i++) {			/* sequence */
    int lseq = samples[i]->length;
    int w = model->w[1];
    ajFmtPrintF(outf,">%s\n", samples[i]->sample_name);
    for (j=0; j<lseq-w+1; j++) {		/* position */
      int zij = NINT(10 * samples[i]->z[1][j]);	/* round z */
      ajFmtPrintF(outf,"%1x", zij);
    }
    ajFmtPrintF(outf,"\n");
  }
  ajFmtPrintF(outf,"\n");
}

/**********************************************************************/
/*
	print_wij
*/
/**********************************************************************/
extern void print_wij(
  DATASET *dataset			/* the dataset */
)
{
  int i,j;
  int n_samples = dataset->n_samples;
  SAMPLE **samples = dataset->samples;

  ajFmtPrintF(outf,"w_ij:\n");
  for (i=0; i<n_samples; i++) {               /* sequence */
    int len = samples[i]->length;
    double *weights = samples[i]->weights;
    ajFmtPrintF(outf,">%s\n", samples[i]->sample_name);
    for (j=0; j<len; j++) {                   /* position */
      int w = NINT(10 * weights[j]);
      ajFmtPrintF(outf,"%1x", w);
    }
    ajFmtPrintF(outf,"\n");
  }
  ajFmtPrintF(outf,"\n");
}

/**********************************************************************/
/*	
	make_log_odds

	Compute the log-odds matrix from the motif, negative motif and
	background letter frequencies.
		l = log(p/(q*n+(1-q)b))
	where 	p = positive model,
		n = negative model,
		b = background model.
*/
/**********************************************************************/
static LOGODDS make_log_odds(
  THETA theta1,			/* positive motif theta */
  THETA theta0,			/* negative motif theta; use 0 if NULL */
  double *back,			/* background frequencies; use 0 if NULL */
  double q,			/* 0<=q<=1, mixing parameter */
  int w,			/* width of motif */
  int alength 			/* length of alphabet */
)
{
  int i, j;
  LOGODDS logodds = NULL;		/* the logodds matrix */

  /* 
    compute the log-odds matrix 
  */
  Resize(logodds, w, double *);		/* create rows */ 
  for (i=0; i < w; i++) {		/* site position */
    logodds[i] = NULL;			/* create columns */
    Resize(logodds[i], alength, double);

    /* calculate log-odds for this column */
    for (j=0; j < alength; j++) {	/* residue */
      double p = theta1(i,j);		/* positive motif */
      double n; 			/* negative motif */
      if (!theta0) {			/* no negative motif given */
        n = back[j];			/* background */
      } else if (!back) {		/* no background given */
          n = theta0(i,j);		/* negative motif */
      } else {				/* negative and background given */
        n = q*theta0(i,j) + (1-q)*back[j];/* blend negative & background */
      }
      if (n==0) {
        logodds(i,j) = 0;		/* letter with zero prob */
      } else {
        logodds(i,j) = LOG2(p/n);
      }
    }
  }

  return logodds;
} /* make_log_odds */

/**********************************************************************/
/*      get_consensus

        Get the consensus string from a motif.
	For each position, N consensus letters are found.
	If no letter has 
	probability > min_prob, 'x' is written for the first consensus
	letter and ' ' in the others.
        Otherwise, N letters are written in decreasing order of
	probability until one with min_prob is reached, and then ' ' is 
	written for the remaining letters.
*/
/**********************************************************************/
extern char *get_consensus(
  THETA theta,			/* motif theta */
  int w,			/* width of motif */
  DATASET *dataset,		/* the dataset */
  int N,			/* number of letters for each position */
  double min_prob		/* minimum cumulative prob for N letters */
)
{
  int i, j, n;
  int alength = dataset->alength;
  char *alphabet = dataset->alphabet;
  char *string = (char *) malloc(w * N + 1);

  for (i=0; i < w; i++) {		/* position in motif */
    int maxj[MAXDEPTH];			/* array of max indices in Theta */

    /* find N letters at position i with highest probability (in order) */
    for (n = 0; n < N; n++) {		/* current depth */	
      double max = LITTLE;		/* current max probability */
      for (j=0; j < alength; j++) {	/* letter */
	if (theta(i, j) > max) {
	  max = theta(i, j);		/* maximum probability */
	  maxj[n] = j;			/* current letter with n-th best prob */
	}
      }
      theta(i, maxj[n]) = -theta(i, maxj[n]);	/* tag this position as used */
    } 

    /* restore theta */
    for (n = 0; n < N; n++) {		/* current depth */	
      theta(i, maxj[n]) = -theta(i, maxj[n]);   /* untag */	
    }

    /* set up the consensus strings for position i */
    for (n = 0; n < N; n++) { 			/* current depth */	
      if (theta(i, maxj[n]) < min_prob) {
        string[(n*w)+i] = (n==0 ? 'x' : ' ');	/* below cutoff */
      } else { 
        string[(n*w)+i] = alphabet[maxj[n]]; 	/* set n'th consensus */
      }
    }
  }
  string[((N-1)*w)+i] = '\0';			/* terminate string */

  return string;
} /* get_consensus */

/**********************************************************************/
/*
	print_candidates

	Print the candidate models found.

*/
/**********************************************************************/
static void print_candidates(
  CANDIDATE *candidates, 	/* list of candidate models */
  DATASET *dataset,		/* the dataset */
  int max_w			/* maximum width for motifs */
)
{
  int i, w;
  int hdr = 35;
  int tail = PAGEWIDTH - hdr;
  double m, e;			/* for printing signficance */

  ajFmtPrintF(outf,"\nCandidate motifs:\n");
  ajFmtPrintF(outf,"width nsites  e_ll      signif     consensus\n");
  ajFmtPrintF(outf,"----- ------  --------- ---------- ---------\n");

  for (w=1; w<=max_w; w++) {

    if (candidates[w].sig > 1) continue;	/* skip unused candidates */

    exp10_logx(candidates[w].sig, m, e);

    ajFmtPrintF(outf,"%5d %6.1f %c%9.0f %5.3fe%+04.0f ", 
      w, 
      candidates[w].lambda * ps(dataset, w), 
      (candidates[w].pal ? 'P' : ' '),
      candidates[w].e_ll,
      m, e);
    ajFmtPrintF(outf,"%-*.*s\n", tail, tail, candidates[w].cons);
    for (i=tail; i < w; i+=tail) printf("%*.*s%-*.*s\n", 
      hdr, hdr, "", tail, tail, candidates[w].cons+i);
  }
}

/**********************************************************************/
/*
	print_dataset_summary
*/
/**********************************************************************/
void print_dataset_summary (
  char *datafile,			/* name of the training set file */
  char *alphabet,			/* alphabet of sequences */
  DATASET *dataset,			/* structure containing sequences */
  char *negfile				/* name of negative example file */
)
{
  int i, pcol;
  int w = MSN + 15;

  /* set up printing spacers */
  Resize(stars, PAGEWIDTH+1, char);
  for (i=0; i<PAGEWIDTH; i++) {
    stars[i] = '*';
  }
  stars[PAGEWIDTH] = '\0';

  /* announce the training set */
  ajFmtPrintF(outf,"%s\nTRAINING SET\n%s\n", stars, stars);

  /* print name of file and alphabet */
  ajFmtPrintF(outf,
"DATAFILE= %s (deleted by web version of MEME)\n"
"ALPHABET= %s\n", ajStrStr(datnam), alphabet
  );

  /* print name of negative dataset */
  if (negfile) ajFmtPrintF(outf,"NEGATIVES= %s\n", negfile);

  /*
    print a table of sequence lengths 
  */

  /*   print table header */
  for (pcol = w; pcol < 80; pcol += w) {
    ajFmtPrintF(outf,"%-*.*s%6s %6s%2s", MSN, MSN, 
      "Sequence name", "Weight", "Length", " ");
  }
  ajFmtPrintF(outf,"\n");
  for (pcol = w; pcol < 80; pcol += w) {
    ajFmtPrintF(outf,"%-*.*s%6s %6s%2s", MSN, MSN, 
      "-------------", "------", "------", " ");
  }
  ajFmtPrintF(outf,"\n");

  /*   print table columns */
  pcol = 0; 
  for (i=0; i<dataset->n_samples; i++) {
    SAMPLE *sample = dataset->samples[i];
    char *sample_name = sample->sample_name;
    double wgt = sample->sw;
    int length = sample->length;
    /* print the sample name and its length */
    pcol += w;          /* new line for print out? */
    if (pcol >= 80) { ajFmtPrintF(outf,"\n"); pcol = w; }
    ajFmtPrintF(outf,"%-*.*s%6.4f %6d%2s", MSN, MSN, sample_name, wgt, length, " ");
  }

  /* finish section */
  ajFmtPrintF(outf,"\n%s\n\n", stars);
}

/**********************************************************************/
/*
	print_env

	Print debug information for MEME.
*/
/**********************************************************************/
void print_env(FILE *debug_file)
{
  /* print environment */
  ajFmtPrintF(outf,"\n\n");
  PSTARRS();
  ajFmtPrintF(outf,"DEBUG INFORMATION\n");
  PSTARRS();
  split(1, debug_file, 1000);
  PSTARRS();
  ajFmtPrintF(outf,"\n");
/*  fflush(stdout);*/
}

/**********************************************************************/
/*
	print_meme_doc

	Print documentation for MEME.

	Note: file meme.doc is taken by cut-and-paste from 
	meme-output.html.
*/
/**********************************************************************/
void print_meme_doc()
{
  char *meme_directory, *name = NULL;
  int s1;
  FILE *doc;
  AjPFile inf=NULL;
  
  /* get the name of the MEME directory */
/*  meme_directory = getenv("MEME_DIRECTORY");
  if (meme_directory == NULL) {
      fprintf(stderr, "You must define environment variable MEME_DIRECTORY\n");
      exit(1);
  }*/
  
  /* make the name of the meme documentation file */
/*  s1 = strlen(meme_directory);
  Resize(name, s1 + 10, char);
  strcpy(name, meme_directory);
  strcpy(name + s1, "/meme.doc");
*/
  /* open the documentation file */
/*  doc = fopen(name, "r");
  if (doc == NULL) {
    fprintf(stderr, "Unable to open MEME's documentation file `%s'.\n", name);
    exit(1);
  }*/

  ajFileDataNewC("Ememe.dat",&inf);
  if(!inf)
      ajFatal("Unable to open MEME's documentation file Ememe.dat");

  doc = ajFileFp(inf);

  ajFmtPrintF(outf,"\n");
  PSTARRS();
  ajFmtPrintF(outf,"EXPLANATION OF RESULTS\n");
  PSTARRS();

  /* copy the documentation file to standard output */
  (void) split(1, doc, 1000);

  PSTARRS();

  ajFileClose(&inf);
}

/**********************************************************************/
/*
	score_sequence

	Compute the sequence score for a motif.

	Returns the sequence score.
*/
/**********************************************************************/
static double score_sequence(
  char *eseq,		/* integer-coded sequence to score */
  int length,		/* length of the sequence */
  int w, 		/* width of motif */
  LOGODDS logodds1, 	/* log-odds matrix: LOG2(m_ij/b_j) */
  LOGODDS logodds2  	/* log-odds matrix: LOG2(m_ij/n_ij) */
)
{
  int i, j;
  double best = LITTLE;			/* sequence score */
  double score, sc1, sc2;
  double loge2 = log(2);

  /* score the sequence with motif */
  for (i=0; i <= length - w; i++) {	/* site start */
    /* calculate score of subsequence */
    for (j=0, sc1=0, sc2=0; j<w; j++) {	/* position in sequence */
      sc1 += logodds1(j, eseq[i+j]);
      if (logodds2) sc2 += logodds2(j, eseq[i+j]);
    } /* subsequence */
    score = logodds2 ? -LOG_SUM(-sc1*loge2, -sc2*loge2)/loge2 : sc1;
    best = MAX(score, best);
  } /* site start */

  return best;

} /* score_sequence */

/**********************************************************************/
/*
	get_thresh

	Get the optimal threshold for minimizing classification error
	by classifying positive and negative data using the motif, 
	sorting and finding the minimum error.

	Returns optimal threshold, error rate and ROC.
*/
/**********************************************************************/
static ACCURACY *get_thresh(
  int w,				/* width of motif */
  LOGODDS logodds1,			/* log-odds matrix: LOG2(m_ij/b_j) */
  LOGODDS logodds2,			/* log-odds matrix: LOG2(m_ij/a_ij) */
  DATASET *pos,				/* positive examples */
  DATASET *neg,				/* negative examples */
  BOOLEAN print_scores			/* print sorted scores */
)
{
  int i, class, iseq;
  int err;				/* sum of false pos. and false neg. */
  int min_pos, max_pos;			/* best cutoff index in sorted list */
  int best_err;				/* best classification error */
  double thresh;			/* best threshold */
  SORTED_SCORE *scores=NULL;		/* array of class/score */
  int npos = pos->n_samples;
  int nneg = neg->n_samples;
  int nseqs = npos + nneg;		/* number of sequences */
  DATASET *dataset;
  /* for ROC */
  double roc;				/* receiver operating characteristic */
  double tpp, fpp;			/* true, false positive proportions */
  double tp, fp;			/* true, false positives so far */
  double newtpp, newfpp;
  ACCURACY *acc = NULL;
  double minposscore;			/* minimum score of a positive */ 
  double maxnegscore;			/* maximum score of a negative */ 

  /* allocate space for accuracy record */
  Resize(acc, 1, ACCURACY);
 
  /* allocate space for scores */
  Resize(scores, nseqs, SORTED_SCORE);

  /* score sequences */ 
  for (class=0, iseq = 0; class<2; class++) {
    if (class) dataset = pos; else dataset = neg;
    for (i=0; i<dataset->n_samples; i++, iseq++) {
      SAMPLE *s = dataset->samples[i];	/* sample */
      char *eseq = s->res;			/* integer-coded sequence */
      int length = s->length;			/* length of sequence */
      scores[iseq].class = class;
      scores[iseq].score = score_sequence(eseq, length, w, logodds1, logodds2);
      scores[iseq].id = s->sample_name;
    }
  }

  /* sort sequences by score in descending order */
  qsort(scores, nseqs, sizeof(SORTED_SCORE), s_compare);

  /* 
    get threshold with minimum classification error 
    If a range of thresholds give the same error, choose the average.
  */
  roc = tpp = fpp = tp = fp = 0 ;	/* for ROC */
  best_err = err = pos->n_samples;	/* all false negatives */
  min_pos = 0;				/* threshold must be below this */
  max_pos = 0;				/* threshold must be above this */
  minposscore = BIG;			/* smallest score of positives */
  maxnegscore = -BIG;			/* smallest score of negatives */
  for (i=0; i<nseqs; i++) {
    if (scores[i].class) {			/* positive */
      err--;					/* one fewer fn */
      tp++;
      minposscore = scores[i].score;
    } else {					/* negative */
      err++;					/* one more fp */
      fp++;
      maxnegscore = MAX(maxnegscore, scores[i].score);
    }
    if (err < best_err) { 			/* start new range */
      best_err = err;
      min_pos = max_pos = i;
    } else if (err == best_err) {		/* extend current range */
      max_pos = i;
    }
    /* ROC trapezoidal rule : (y2 - y1)/2 dx */
    newtpp = tp / npos;
    newfpp = fp / nneg;
    roc += .5 * (newtpp + tpp) * (newfpp - fpp);
    tpp = newtpp;
    fpp = newfpp;
  }
  max_pos = MIN(max_pos+1, nseqs-1);
  thresh = (scores[min_pos].score + scores[max_pos].score)/2;

  /* normalize by fpp to get ROC */
  if (fpp == 0) {
    roc = 1.0;
  } else {
    roc /= fpp;
  }
  
  /* add difference between positives and negatives if ROC is 1.0 */
  if (roc == 1.0) roc += minposscore - maxnegscore;

  /* print the sorted list */
  if (print_scores) {
    ajFmtPrintF(outf,"ROC= %f\n", roc);
    for (i=0; i<nseqs; i++) ajFmtPrintF(outf,"%-*.*s %1d %g\n", 
      MSN, MSN, scores[i].id, scores[i].class, scores[i].score); 
  }
  
  acc->thresh = thresh;
  acc->err = best_err;
  acc->roc = roc;

  return acc;
} /* get_thresh */

/**********************************************************************/
/*
        s_compare
 
        Compare two scores in descending order.  Return <0 >0
        if the first is <, > the second.  If they are equal,
        resolves ties by returning <0 if the first has smaller class.
*/
/**********************************************************************/
static int s_compare(
  const void *v1,
  const void *v2
)
{
  const SORTED_SCORE * s1 = (const SORTED_SCORE *) v1;
  const SORTED_SCORE * s2 = (const SORTED_SCORE *) v2;
  double diff = s1->score - s2->score;
  if (diff == 0) diff = (double) (s1->class - s2->class);
  return ((diff > 0) ? -1 : ( (diff < 0) ? 1 : 0) );
} /* s_compare */


/**********************************************************************/
/*
	get_q

	Get the value of q which gives the optimal ROC on the training
	set.
*/
/**********************************************************************/
static double get_q(
  int nsteps,					/* try nsteps from 0 to 1 */
  int window,					/* smoothing window radius */
  int w,					/* width of motif */
  THETA theta,					/* motif theta */
  THETA neg_theta, 				/* anti-motif theta */
  double *back,					/* background motif */
  DATASET *dataset,				/* the dataset */
  DATASET *neg_dataset,				/* negative examples */
  char *str_space 		/* space for printing strand direction */
)
{
  int i, j;
  double *roc = NULL;				/* array to hold roc(q) */
  double smooth_roc;				/* smoothed value of roc */
  double best_roc;				/* maximum roc */
  double q=0;					/* mixing parameter */
  double incr = 1.0/nsteps;			/* increment for q */
  LOGODDS logodds;				/* motif log-odds matrix */
  ACCURACY *acc;				/* get_thresh struct */
  int alength = dataset->alength;		/* length of alphabet */

  /* create ROC array */
  Resize(roc, nsteps+1, double);

  /* 
    get roc for different values of q 
  */
  for (i=0; i<=nsteps; i++) {
    q = i * incr;
    logodds = make_log_odds(theta, neg_theta, back, q, w, alength);
    acc = get_thresh(w, logodds, NULL, dataset, neg_dataset, FALSE);
    roc[i] = acc->roc;				/* save roc for this q */
    /*printf("q= %8.3f error= %5d roc= %8.5f\n", q, acc->err, acc->roc);
    motif_entropy(theta, logodds, w, str_space, dataset, consensus, stdout);
    */
    /*print_log_odds(dataset, w, logodds, acc->thresh);*/
    free(acc);					/* free up space */
  } /* get roc */

  /* 
    smooth ROC and find q that gives maximum
  */
  best_roc = 0;
  for (i=0; i<=nsteps; i++) {
    double avg = 0;
    int cnt = 0;
    for (j=MAX(0,i-window); j<=MIN(nsteps, i+window); j++) {
      avg += roc[j];
      cnt++;
    }
    smooth_roc = avg/cnt;
    if (smooth_roc > best_roc) {
      best_roc = smooth_roc;
      q = i * incr;
    }
    /*printf("q= %8.3f smoothed_roc= %8.5f\n", i*incr, smooth_roc);*/
  } /* smooth ROC and get max */

  free(roc);					/* release space */

  ajFmtPrintF(outf,"Q= %8.3f ROC= %8.3f\n", q, best_roc); 

  return q;
} 

/**********************************************************************/
/*
	get_sites

	Get the principal sites contributing to a motif.

	OOPS		position with highest z_ij in each sequence
	ZOOPS		position with highest z_ij > 0.5 in each sequence
	TCM		all positions with z_ij > 0.5

	Assumes that z_ij is set for current motif.
	Returns a list consisting of sites and sets n, the number of sites.
*/
/**********************************************************************/
static SITE *get_sites(
  DATASET *dataset,			/* the dataset */
  MODEL *model,				/* the model */
  int *n				/* number of sites found */
)
{
  int i, j, k;
  int nsites = 0;			/* number of sites */
  SITE *sites = NULL;			/* list of sites */
  int n_samples = dataset->n_samples;	/* number of sequences */
  SAMPLE **samples = dataset->samples;	/* the sequences */
  MTYPE mtype = model->mtype;		/* type of model */
  int w = model->w[1];			/* width of motif */

  if (mtype==Oops || mtype==Zoops) {	/* (Z)OOPS model */
    for (i=0; i<n_samples; i++) {	/* sequence */
      SAMPLE *sample = samples[i];	/* sample */
      int lseq = sample->length;	/* length of sequence */
      double max_zij = -1;		/* flag no z_ij found */
      int max_j = -1;			/* position of site */
      double *dz = sample->dz;		/* probability of strand directions */
      int d = 0;			/* strand direction of site */
      /* find maximum z_ij */
      for (j=0; j<lseq-w+1; j++) {	/* position */
        double zij = sample->z[1][j];	/* z_ij */
        if (zij > max_zij) {		/* bigger found */
          max_zij = zij;
          max_j = j;
        }
        if (zij > 0.5) break;		/* must be maximum if > 0.5 */
      } /* position */
      /* record site */
      if (mtype == Oops || max_zij > 0.5) {
        if (nsites % 100 == 0) Resize(sites, nsites+101, SITE);
        /* find strand direction of site */
        for (k=1; k<4; k++) {if (dz[k]>dz[d]) { d = k; }}
        sites[nsites].seqno = i;
        sites[nsites].pos = j;
        sites[nsites].zij = max_zij;
        sites[nsites++].d = d;
      }
    } /* sequence */
  } else {				/* TCM model */
    for (i=0; i<n_samples; i++) {	/* sequence */
      SAMPLE *sample = samples[i];	/* sample */
      int lseq = sample->length;	/* length of sequence */
      /* find all z_ij > 0.5 */
      for (j=0; j<lseq-w+1; j++) {	/* position */
        double zij = sample->z[1][j];	/* z_ij */
        if (zij > 0.5) { 		/* record site */
	  if (nsites % 100 == 0) Resize(sites, nsites+101, SITE);
	  sites[nsites].seqno = i;
	  sites[nsites].pos = j;
	  sites[nsites].zij = zij;
          sites[nsites++].d = 0;	/* only 1 strand with TCM */
        } /* record site */
      } /* position */
    } /* sequence */
  }
  *n = nsites;
  return sites;
} /* get_sites */

/**********************************************************************/
/*
	print_sites

	Print the sites making up the model (as determined by get_sites.)
*/
/**********************************************************************/
static void print_sites(
  DATASET *dataset,			/* the dataset */
  MODEL *model,				/* the model */
  int format,				/* 0=BLOCKS; 1=FASTA */
  char *com,				/* comment to append */
  FILE *outfile				/* where to print */
)
{
  int i, j;
  int w = model->w[1];			/* width of motif */
  int n;				/* number of sites */
  SITE *sites;				/* sites "defining" model */
  char *ftype = (format==0 ? "BLOCKS" : "FASTA");

  /* get the sites in the model */
  sites = get_sites(dataset, model, &n);

  /* print header */
/*  for (i=0; i<PAGEWIDTH; i++) fputc('-', outfile); fputc('\n', outfile);
  fprintf(outfile, "\tMotif %d in %s format%s\n", model->imotif, ftype, com);
  for (i=0; i<PAGEWIDTH; i++) fputc('-', outfile); fputc('\n', outfile);*/
  for (i=0; i<PAGEWIDTH; i++)
      ajFmtPrintF(outf,"-");
  ajFmtPrintF(outf,"\n");
  ajFmtPrintF(outf, "\tMotif %d in %s format%s\n", model->imotif, ftype, com);
  for (i=0; i<PAGEWIDTH; i++)
      ajFmtPrintF(outf,"-");
  ajFmtPrintF(outf,"\n");


  /* print the sites */
  if (format == 0)
/*      fprintf(outfile, "BL   MOTIF %d width=%d seqs=%d\n", */
      ajFmtPrintF(outf, "BL   MOTIF %d width=%d seqs=%d\n",
    model->imotif, w, dataset->n_samples);
  for (i=0; i<n; i++) {			/* print each site */
    int seqno = sites[i].seqno;		/* sequence number */
    SAMPLE *sample = dataset->samples[seqno];	/* sequence */
    char *eseq = sample->res;		/* encoded sequence */
    int pos = sites[i].pos;		/* location of site */
    double weight = sites[i].zij;	/* probability estimate */
    double d = sites[i].d;		/* strand direction */
/*    if (format == 0) {
      fprintf(outfile, "%-*.*s ( %4d) ", MSN, MSN, sample->sample_name, pos+1);
    } else {
      fprintf(outfile,">%-*.*s pos %4d\n", MSN, MSN, sample->sample_name,pos+1);
    }*/
    if (format == 0) {			/* BLOCKS format */
      ajFmtPrintF(outf, "%-*.*s ( %4d) ", MSN, MSN, sample->sample_name, pos+1);
    } else {				/* FASTA format */
      ajFmtPrintF(outf,">%-*.*s pos %4d\n", MSN, MSN, sample->sample_name,pos+1);
    }
    if (d==0 || d==3) {			/* left to right strands w53 and c35 */
      for (j=pos; j<pos+w; j++) {
/*        fputc((d==0) ? unhash(eseq[j]) : unhash(dna_comp(eseq[j])), outfile);*/
	  ajFmtPrintF(outf,"%c",(d==0) ? unhash(eseq[j]) : unhash(dna_comp(eseq[j])));
	  
      }
    } else {				/* right to left strands w35 and c53 */
      for (j=pos+w-1; j>=pos; j--) {
/*   fputc((d==1) ? unhash(eseq[j]) : unhash(dna_comp(eseq[j])), outfile);*/
ajFmtPrintF(outf,"%c",(d==1) ? unhash(eseq[j]) : unhash(dna_comp(eseq[j])));
      }
    }
    if (format == 0) {			/* BLOCKS format */
/*      fprintf(outfile, "  %g ", weight);*/
      ajFmtPrintF(outf, "  %g ", weight);
    }
/*    fputc('\n', outfile);*/
    ajFmtPrintF(outf,"\n");
  } /* print each site */
  if (format == 0) {
/*    fprintf(outfile, "//\n\n");*/
    ajFmtPrintF(outf, "//\n\n");
  } else {
    for (i=0; i<PAGEWIDTH; i++) /*fputc('-', outfile);*/
	ajFmtPrintF(outf,"-");
    ajFmtPrintF(outf,"\n\n");
/*    fprintf(outfile, "\n\n");*/
  }
  free(sites);
} /* print_sites */

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/display.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
