/* meme.h */

#ifndef MEME_H
#define MEME_H

#include <user.h>
#include <macros.h>
#include <mtype.h>
/*#include <logs.h>*/
#include <prior.h>
#include <hash.h>
#include <gamma.h>
#include <chi.h>
#include <ajax.h>

extern AjPFile outf;
extern AjPSeqall seqall;
extern AjPSeqall seqalln;
extern AjPSeqall seqallm;
extern AjPStr datnam;

/* conventions:
	ALL_CAPS	user defined type
	Vname		enum type value
	name()		macro
*/

/* globals */
DEXTERN(int, PAGEWIDTH, 80);		/* page width for printing */
DEXTERN(BOOLEAN, VERBOSE, FALSE);	/* turn on verbose output mode */
DEXTERN(BOOLEAN, NOALIGN, FALSE);	/* print alignment */
DEXTERN(BOOLEAN, TRACE, FALSE);		/* print each start tried */
DEXTERN(BOOLEAN, PRINT_FASTA, FALSE);	/* print sites in BLOCKS format */
DEXTERN(BOOLEAN, PRINTALL, FALSE);	/* for debug */
DEXTERN(BOOLEAN, PRINT_W, FALSE);	/* print w_ij */ 
DEXTERN(BOOLEAN, PRINT_Z, FALSE);	/* print z_ij */ 
DEXTERN(BOOLEAN, PRINT_LL, FALSE);	/* print log likelihood */ 
DEXTERN(BOOLEAN, PRINT_STARTS, FALSE);	/* print starting subsequences */ 
DEXTERN(BOOLEAN, NO_STATUS, FALSE);     /* print run-time status to stderr */
DEXTERN(BOOLEAN, DOC, TRUE);		/* print documentation */
EXTERN char *OFFSET_FILE;		/* current name of offset file */
DEXTERN(int, TIMER, 0);			/* Type of timing:
					    0 : None
					    1 : Per loop
					    2 : Per start */


#ifdef PARALLEL
  DEXTERN(int, PARA, 3);		/* Type of parallelization:
                                            0 : Serial
                                            1 : Loop 1
                                            2 : Loop 2
                                            3 : Both loops */
  DEXTERN(BOOLEAN, BALANCE, FALSE);	/* Perform load-balancing, loop 1. */
#else
  DEXTERN(int, PARA, 3);		/* remove when done with parallelization */
#endif

#define dna_comp(i) comps[(int) i]
#ifdef DEFINE_GLOBALS
  char comps[] = {3,2,1,0};
  char *dir[] = {"    w53", "    w35", "    c53", "    c35"};
#else
  extern char comps[];
  extern char *dir[];
#endif

/* macro to write a line of asterisks */
#define PSTARS {int i; for (i=0;i<PAGEWIDTH;i++)printf("*");printf("\n");}

/* type of negative motifs */
typedef enum {Pair, Blend} NEGTYPE;
 
/* type of LRT adjustment */
typedef enum {None, Bon, Root} ADJTYPE;

/* type of sequence to theta mapping */
typedef enum {Uni, Pam} MAP_TYPE; 

/* type of prior */
typedef enum {Mega, MegaP, Dmix, Dirichlet, Addone} PTYPE;

/* possible sites for a dataset, width combination */
#define ps(d, w) 	((d)->total_res - ((d)->n_samples * ( (w) - 1) ) )

/* number of occurrences of a motif based on dataset, w, lambda */
#define nsites(d, w, l) ((l) * ps(d, w))

/* number of independent columns in a model component */
#define ind_cols(m, c) (m->pal ? (m->w[c] + 1)/2 : m->w[c])

/* dataset sample */
typedef struct sample {
  char *sample_name;		/* name of sample */
  int length;			/* length of sequence */
  char *res;			/* integer-coded sequence */
  char *resc;			/* integer-coded dna complement of sequence */
  double *weights;		/* Pr[pos not contained in previous site] */
  double *not_o;		/* P[no site in [x_{i,j}...x_{i,j+w}] */
  int *log_not_o;		/* log (not_o) */
  int **pY;			/* p(Y_j | theta_g, g=0,...,MAXG-1) */
  int max_j;			/* position in sequence with largest pY[1] */
  double **z;			/* E[z_ij] for components 0 thru MAXG-1 */
  double dz[4];			/* strand direction missing info */
  int *counts;			/* counts of each character */
  double sw;			/* sequence weight */
} SAMPLE;

typedef double **THETA;
#define theta_ref(a, b, c)	(a)[b][c]
#define theta(b, c)		theta_ref(theta, b, c)
#define theta0(b, c)		theta_ref(theta0, b, c)
#define theta1(b, c)		theta_ref(theta1, b, c)
#define logtheta(b, c)		theta_ref(theta, b, c)
#define logtheta0(b, c)		theta_ref(theta0, b, c)
#define logtheta1(b, c)		theta_ref(theta1, b, c)
#define obs(b, c)		theta_ref(obs, b, c)
#define obs0(b, c)		theta_ref(obs0, b, c)
#define obs1(b, c)		theta_ref(obs1, b, c)

/* a model */
typedef struct Model {
  MTYPE mtype;			/* type of model */
  int c;			/* number of components */
  int w[2];			/* widths of each component */
  BOOLEAN pal;			/* model is DNA palindrome */
  THETA theta[2];		/* theta_0, ..., theta_{c-1} */
  THETA obs[2];			/* observed frequencies */
  double lambda[2];		/* lambda_0, lambda_{1} */
  double sigma[4];		/* fraction of each strand */
  int nstrands;			/* number of different strands to use */
  BOOLEAN d[4];			/* strand directions to use
					0 : watson 5' -> 3'
					1 : watson 3' -> 5'
					2 : crick 5' -> 3'
					3 : crick 3' -> 5'
 				*/
  char cons[MAXSITE+1];		/* consensus sequence of motif */
  double rel[2];		/* relative entropy per col of motif */
  double e_ll_0;		/* expected log-likelihood of null model */
  double e_ll;			/* expected log-likelihood of model */
  double ll;			/* log-likelihood */
  double sig;			/* (adjusted) LRT significance of model */
  double lrt;			/* LRT significance of model */
  double bon;			/* Bonferonni LRT significance of model */
  double root;			/* n-th root LRT significance of model */
  double pvalue;		/* ASOP p-value of model */
  int imotif;			/* number of motif */
#ifdef PARALLEL
  char cons0[MAXSITE+1];	/* character initial consensus */
  int ID;                       /* processor id */
#endif
  int iter;			/* number of EM iterations required */
} MODEL;

/* user input/learned starting points */
typedef struct p_point {
  int c;			/* number of components */
  int w[MAXG];			/* widths for motifs */
  double nsites[MAXG];		/* nsites for motif */
  char *e_cons0[MAXG];		/* integer encoded starting subsequence */
} P_POINT;

/* starting point */
typedef struct s_point {
  double score;			/* score of starting point */
  int iseq;                     /* sequence number of starting point */
  int ioff;                     /* sequence offset of starting point */
  int w0;			/* start width for motif */
  double nsites0;		/* start nsites0 for motif */
  char *e_cons0;		/* integer encoded starting subsequence */
  char *cons0;			/* character initial consensus */
} S_POINT;

/* candidate final model */
typedef struct candidate {
  S_POINT *s_point;		/* starting point of model */
  int w;			/* final width of motif */ 
  BOOLEAN pal;			/* palindrome flag */
  double lambda;		/* final lambda for motif */
  char cons[MAXSITE+1];		/* final consensus of motif */
  double rel;			/* relative entropy per col of each motif */
  double e_ll_0;		/* expected log-likelihood of null model */
  double e_ll;			/* expected log-likelihood */
  double ll;			/* log-likelihood */
  double sig;			/* likelihood ratio test significance */
} CANDIDATE;

/* prior probabilities */
typedef struct Priors {
  PTYPE ptype;			/* type of prior to use */
  double prior_count[MAXALPH];	/* ptype = Dirichlet: pseudo counts/letter */
  PriorLib *plib;		/* ptype = Dmix, Mega, MegaP: dirichlet mix */
  PriorLib *plib0;		/* ptype = MegaP: b=0 dirichlet mixture */
} PRIORS;

/* a known motif */
typedef struct motif {
  char name[MNAME];			/* names of motif */
  int width;				/* (known) width of motif */
  int pos;				/* # positive samples this motif */
  double roc;				/* best roc for this motif */
  int shift;				/* best shift for this motif */
  int pass;				/* pass that found this motif */
  double recall;			/* best recall this motif */
  double precision;			/* best recall this motif */
  double min_thresh;			/* minimum threshold for motif */
  double max_thresh;			/* maximum threshold for motif */
  double pal;				/* motif is DNA palindrome */
  double like;				/* best likelihood this motif */
  double sig;				/* best significance this motif */
  double ic;				/* best info content this motif */
  double sites;				/* best nsites this motif */
  int w;				/* best width this motif */
  double thresh;			/* best threshold this motif */
  HASH_TABLE ht;			/* hash table of positives this motif */
} MOTIF;

/* a dataset */
typedef struct Dataset {
  int alength;			/* length of alphabet */
  int effective_alength;	/* number of non-0 letter frequencies */
  int total_res;		/* total size of dataset */
  double wgt_total_res;		/* weighted (sw*slen) total size of dataset */
  int n_samples;		/* number samples */
  SAMPLE **samples;		/* array of (pointers to) samples */
  int max_slength;		/* maximum length of sequences */
  int min_slength;		/* minimum length of sequences */
  char *alphabet;		/* alphabet */
  double *res_freq;     	/* average letter frequencies */
  double *adj_freq;		/* letter frequencies adjusted by prior */
  double *nrdb_freq;		/* non-redundant database letter frequencies */
  double entropy;		/* entropy of dataset */
  double adj_entropy;		/* entropy of dataset adjusted by prior */
  BOOLEAN pal;			/* look for palindromes */
  THETA map;			/* letter to frequency vector mapping */
  ADJTYPE adj;			/* type of LRT adjustment to use */
  MOTIF motifs[NMOTIFS];	/* known motifs in dataset */
  int nmotifs;			/* number of known motifs in dataset */
  NEGTYPE negtype;		/* how to use negative examples */
} DATASET;

/* subroutines */

extern double exp(double x);
extern int em_subseq(
  THETA map,                    /* freq x letter map */
  DATASET *dataset,             /* the dataset */
  MODEL *model,                 /* the model */
  PRIORS *priors,               /* the priors */
  int w,                        /* width of motif */
  int n_nsites0,                /* number of nsites0 values to try */
  double alpha,                 /* sampling probability */
  P_POINT *p_point,             /* starting point for previous components */
  S_POINT s_points[]            /* array of starting points: 1 per nsites0 */
);
extern int subseq7(
  MTYPE mtype,			/* type of model */
  THETA map,			/* freq x letter map */
  DATASET *dataset,		/* the dataset */
  int w,			/* width of motif */
  int n_nsites0,		/* number of nsites0 values to try */
  double alpha,			/* sampling probability */
  S_POINT s_points[], 		/* array of starting points: 1 per nsites0 */
  double *loop1_time            /* CPU time spent in first parallelized loop */
);
extern void get_not_o(
  DATASET *dataset,			/* the dataset */
  int w,				/* width of motif */
  BOOLEAN get_log			/* compute log_not_o if true */
);
extern void calc_like(
  MODEL *model,			/* the model */
  DATASET *dataset  		/* the dataset */
);
extern void shorten(
  MODEL *model,				/* the model */
  DATASET *dataset,			/* the dataset */
  PRIORS *priors,			/* the priors */
  int c,				/* component of model to shorten */
  int min_w,				/* minimum width of motif allowed */
  int maxiter,				/* maximum number iterations for EM */
  double distance			/* stopping criterion */
);
extern void init_theta(
  THETA theta,			/* theta */
  char *start,			/* integer encoded starting sequence */
  int w,			/* width of motif */
  THETA map,			/* frequency vectors for each letter */ 
  double *res_freq,		/* average letter frequencies */
  int alength			/* alphabet length */
);
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
);
extern THETA init_map(
  MAP_TYPE type,		/* type of mapping:
					Uni	- add n prior
					Pam	- pam matrix
				*/
  double scale,			/* degree of crispness, depends on type,
					Uni	- add n prior (n)
					Pam	- pam distance
				*/
  int alength			/* length of alphabet */
);
extern void copy_theta(
  THETA s,	 		/* source */
  THETA d,			/* destination */
  int w,			/* width of motif */
  int alength			/* length of alphabet */
);
extern void copy_model(
  MODEL *m1, 				/* source */
  MODEL *m2,				/* destination */
  int alength				/* length of alphabet */
);
extern void init_meme(
  int argc,		/* number of input arguments */
  char **argv,		/* input arguments */
  MODEL **model_p,      /* the model */
  MODEL **best_model_p, /* the model */
  DATASET **dataset_p,  /* the dataset */
  THETA *map_p,         /* letter to frequency vector mapping */
  PRIORS **priors_p,    /* the prior probabilities model */
  P_POINT **p_point_p,	/* previously learned starting points */
  double *min_nsites_p,	/* minimum value of nsites */
  double *max_nsites_p,	/* maximum value of nsites */
  int *min_w_p,		/* minimum width for motifs */
  int *max_w_p,		/* maximum width for motifs */
  BOOLEAN *shortn_p,	/* shorten motif flag */
  double *distance_p,   /* convergence radius */
  double *prob_p,	/* sampling probability for subsequence starts */
  int *nmotifs_p,	/* number of motifs to find */
  int *maxiter_p,	/* maximum number of iterations for EM */ 
  double *chi_alpha_p,	/* width significance level */
  FILE **debug_file_p,  /* holds debug info */
  MODEL **neg_model_p,  /* model of negative examples */
  DATASET **neg_dataset_p      /* dataset of negative examples */
);
extern MODEL *create_model(
  MTYPE mtype,				/* type of model */
  BOOLEAN *strands,			/* strands to use */
  P_POINT *p_point,			/* user input starts */
  int nmotifs,				/* number of motifs to learn */
  int alength				/* length of alphabet */
);
extern PRIORS *create_priors(
  PTYPE ptype,				/* type of prior to use */
  double beta, 				/* beta for dirichlet priors */
  DATASET *dataset,			/* the dataset */
  char *plib_name			/* name of prior library */
);
extern double min_sites(
  double nu,			/* degrees of freedom */
  double alpha,			/* significance level */
  double max_h			/* maximum entropy */
);
extern void palindrome(
  THETA theta, 			/* motif */
  THETA theta_pal,		/* constrained motif */
  int w, 			/* width */
  int alength			/* length of alphabet */
);

extern int read_motifs (
  FILE *fdata,                          /* opened dataset file */
  char *filename,                       /* motif file */
  MOTIF motif[NMOTIFS],                 /* motif info */
  BOOLEAN save_dataset,                 /* return dataset in memory */
  DATASET *dataset                      /* integer-encoded dataset */
);

#include <em.h>
#include <hash_alph.h>
#include <logodds.h>
#include <read_seq_file.h>
#include <display.h>
#include <align.h>
#include <histogram.h>
#include <clock.h>

#ifdef PARALLEL
#include <message.h>
#endif

#endif
