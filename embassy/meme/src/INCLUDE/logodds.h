/* logodds.h */

#ifndef LOGODDS_H
#  define LOGODDS_H

#include <user.h>
#include <hash_alph.h>

/*
  LOGODDS matrices
*/
/* number of log-odds matrices allowed */
#define MAXLO MAXG
/* 
  single-letter logodds matrix 
*/
#define logodds(a, b)   logodds[(a)][(b)]
#define logodds1(a, b)   logodds1[(a)][(b)]
#define logodds2(a, b)   logodds2[(a)][(b)]
typedef double LOGODDSB;			/* base type of LOGODDS */
typedef LOGODDSB **LOGODDS;			/* [pos][chash(letter)] */

/* 
  two-letter logodds matrix 
*/
typedef double LOGODDS2B;			/* base type of LOGODDS2 */
typedef LOGODDS2B **LOGODDS2;	/* [pos][dhash(chash(l1),chash(l2),alen)] */

/* macro to convert from scaled score back to bit-score */
#define scaled_to_bit(x,w,scale,offset) (((x)/(scale)) + ((w)*(offset)))
#define bit_to_scaled(x,w,scale,offset) (NINT(((x) - ((w)*offset)) * (scale)))

/*
  macro to compute the three class bit log-odds score from the positive and
  negative motif scaled/offset scores:
	pos		positive motif scaled log-odds score:log_2(Pr(P)/Pr(B))'
	neg		negative motif scaled log-odds score: log_2(Pr(P)/Pr(N)'
	lo		pointer to logodds structure for motif
*/
#define score3class(pos, neg, lo) \
  ( -LOG_SUM( \
    -((scaled_to_bit(pos, lo->w, lo->scale, lo->offset) \
       * (Log2)) - (lo->ln_lambda1)), \
    -((scaled_to_bit(neg, lo->w, lo->scalen, lo->offsetn) \
       * (Log2)) - (lo->ln_lambda2))  \
  ) / (Log2) )

/* structure used by read_log_odds */
typedef struct lo {
  BOOLEAN pair;         /* pair of motifs: m_ij/b_j followed by m_ij/n_ij */
  int name;		/* name of motif */
  int w;		/* width of motif */
  int ws;		/* width of motif in sequence */
  double thresh;	/* threshold for scores */
  double sig;		/* significance level of motif */
  double e_ll;		/* expected log likelihood of motif */
  double ic;		/* information content of motif */
  double sites;		/* number of occurrences of motif in dataset */
  int alen;		/* length of alphabet */
  BOOLEAN dna;		/* motif is DNA if TRUE */
  BOOLEAN pal;		/* motif is DNA palindrome if TRUE */
  BOOLEAN d[4];		/* strand directions to use; see MODEL in meme.h */
  double lambda;	/* lambda for motif */
  double L;             /* average length of sequences */
  char *best_seq;	/* best possible matching sequence */
  LOGODDS logodds;	/* log-odds matrix */
  LOGODDS2 logodds2;	/* two-letter log-odds matrix */
  double *corr;		/* correlations with lower-numbered motifs */
  double scale;		/* scale factor (positive motif) for converting 2 bits*/
  double offset;	/* offset (positive motif) for converting 2 bits */
  double scalen;	/* scale factor (negative motif) for converting 2 bits*/
  double offsetn;	/* offset (negative motif) for converting 2 bits*/
  double scale3;	/* scale factor for converting 3-class score 2 bits */
  double offset3;	/* offset factor for converting 3-class score 2 bits */
  double ln_lambda1;	/* log( Pr(B)/Pr(P) ) */
  double ln_lambda2;	/* log( Pr(N)/Pr(P) ) */
} LO;

extern int read_log_odds(
  BOOLEAN translate_dna,/* DNA sequences and protein motifs */
  char *filename,	/* file name (output of make_logodds) */
  char *alphabet,	/* alphabet of log-odds matrices */
  int range, 		/* scale entries in logodds matrices to [0..range] */
  LO *los[MAXLO+1],	/* log-odds structures */
  double *f 		/* null letter frequencies for alphabet (pointer) */
);

extern void min_max(
  LOGODDS logodds,		/* log-odds matrix */
  int w,                        /* width of motif */
  int a,                        /* length of alphabet */
  double *minimum,              /* minimum score */
  double *maximum 		/* minimum score */
);

extern void motif_corr(
  int nmotifs,			/* number of motifs */
  LO *los[]			/* array of logodds structures */
);

extern void shuffle_cols(
  LO *los[],		/* array of pointers to log-odds matrices */
  int nmotifs 		/* number of log-odds matrices in los */
);

#endif

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/logodds.h,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
