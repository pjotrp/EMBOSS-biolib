/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* logodds.c */
#include "meme.h"
#include "macros.h"
#include "logodds.h"


static void make_double_lo(
  LO *los[],		/* array of pointers to log-odds matrices */
  int nmotifs 		/* number of log-odds matrices in los */
); 

static void scale_lo(
  LO *los[],		/* array of pointers to log-odds matrices */
  int nmotifs,		/* number of log-odds matrices in los */
  int range  		/* set entries in matrices to [0..range] */
);

/**********************************************************************/
/*
	read_log_odds

	Read in the next log-odds matrices from a file. 
	Returns the number of matrices read.
	Updates los and f.  Sets up alphabet hash tables.

	Ambiguous letters are given logodds scores equal to the
	weighted average of the scores of the possible true letters.

	If range > 0, 
          1) scale and round the log-odds matrices so that all entries are 
             in range [0...range].
	  2) Matrix entries will be rounded to log_10(range) significant digits.
	  3) Bit score values  can be restored (with some loss of significance) 
	       score_bit = (score/scale) + (w*offset)

	See make_logodds.csh for format of logodds file.
*/
/**********************************************************************/
extern int read_log_odds(
  BOOLEAN translate_dna,/* DNA sequences and protein motifs */
  char *filename,	/* file name (output of make_logodds) */
  char *alphabet,	/* alphabet of log-odds matrices */
  int range, 		/* scale entries in logodds matrices to [0..range] */
  LO *los[MAXLO+1],	/* log-odds structures */
  double *f 		/* null letter frequencies for alphabet (pointer) */
)
{
  int i, j, k; 
  int nmotifs;			/* number of motifs */
  static FILE *fptr;		/* file pointer */
  LO *lo;			/* log odds structure pointer */
  int *p[MAXASCII];		/* alphabet permutation/substitution matrix */
  char *new_alphabet;		/* complete alphabet */
  int alen;			/* length of complete alphabet */
  double tmp; 

  /* 
    open the log-odds file 
  */
  fptr = fopen(filename, "r");
  if (fptr == NULL) {
    fprintf(stderr, "Cannot open file `%s'.\n", filename);
    exit(1);
  }

  /* 
    Set up the hashing functions for mapping between letters or codons and
    alphabet positions.
  */
  setup_hash_alph(DNAB);			/* DNAB to position hashing */
  setup_hash_alph(PROTEINB);			/* PROTEINB to position hash */

  /* 
    Determine the alphabet used by the motifs.
    Make a permutation/substitution mapping from that alphabet to one
    of the BLAST alphabets.
  */
  new_alphabet = get_alphabet(alphabet, p);
  alen = strlen(new_alphabet);

  /*
    Set up to translate DNAB to PROTEINB if requested.
  */
  if (translate_dna) {
    if (strcmp(new_alphabet, PROTEINB)) {	/* motifs must be protein */
      fprintf(stderr, "\nThe -dna switch requires protein motifs.  ");
      fprintf(stderr, "Your motif(s) use a DNA alphabet.\n");
      fclose(fptr);
      unlink(filename);
      exit(1);
    }
    setup_hash_dnab2protb();		/* DNAB to PROTEINB hashing */
  }

  /* 
    initialize a vector of null letter frequencies 
  */
  if (!strcmp(new_alphabet, DNAB)) {	/* searching DNA with DNA motifs */
    for (i=0; i<alen; i++) f[i] = ntfreq[i];
  } else {
    if (translate_dna) {		/* searching DNA with protein motifs */
      for (i=0; i<alen; i++) f[i] = frame0[i];
    } else {				/* searching protein with protein mtfs*/
      for (i=0; i<alen; i++) f[i] = nrfreq[i];
    }
  }

  /*
    read in all the log-odds matrices 
  */
  for (nmotifs=0; ; nmotifs++) {	/* number of logodds matrix */
    lo = NULL; Resize(lo, 1, LO);	/* create a log odds structure */
    los[nmotifs] = lo;			/* put pointer to it in the array */

    /*
      Too many logodds matrices?
    */
    if (nmotifs > MAXLO) {
      fprintf(stderr, 
        "Too many logodds matrices.  Recompile with larger MAXLO.\n");
      fclose(fptr);
      unlink(filename);
      exit(1);
    }

    /* 
      read in header line 
    */
    if ( (i=fscanf(fptr, "%d %lf %lf %lf %lf %lg %d %d %d %d %d %d %lf %d", 
      &(lo->w), &(lo->thresh), &(lo->sig), &(lo->e_ll), &(lo->ic), 
      &(lo->sites), &(lo->alen), &(lo->pal), 
      &(lo->d[0]), &(lo->d[1]), &(lo->d[2]), &(lo->d[3]), &(lo->lambda), 
      &(lo->pair))) != 14 ) {
      if (nmotifs == 0) {
	fprintf(stderr, "Error reading log-odds matrix file `%s'.\n", filename);
	fclose(fptr);
	unlink(filename);
	exit(1);
      } else {
        break;
      }
    }

    /* 
      create the score matrix and best matching sequence 
    */
    lo->logodds = NULL; Resize(lo->logodds, lo->w, double *);
    lo->best_seq = NULL; Resize(lo->best_seq, lo->w+1, char);
    for (i=0; i < lo->w; i++) {
      double best_score = LITTLE;			/* best score in row */
      char best_letter = 'X';				/* best letter in row */
      double *row = NULL; Resize(row, alen, double);	/* temporary row */	
      lo->logodds[i] = NULL; Resize(lo->logodds[i], lo->alen, double); /* row */
      for (j=0; j < lo->alen; j++) {			/* pos in old alph */
	if ( fscanf(fptr, "%lf", &tmp) != 1) {		/* read score */
	  if (i!=0 || j!=0) {
	    fprintf(stderr, "Error reading log-odds matrix file `%s'.\n", 
              filename);
	    fclose(fptr);
	    unlink(filename);
	    exit(1);
	  }
	}
	lo->logodds[i][j] = tmp;
      }

      /* 
        permute the columns of the row so they are in alphabetical order 
        and calculate the scores for any missing, ambiguous letters;
        determine best matching letter for row
      */
      for (j=0; j<alen; j++) {		/* position in new alphabet */
	double score = 0;
	double freq = 0;
	int old_p, new_p;
        /* no need for weighted average if only one possible letter */
        if (p[j][1] < 0) {
          row[j] = lo->logodds[i][p[j][0]];	/* substitute single score */
        } else {
          /* calculate weighted average of matching letter scores */
	  for (k=0; (old_p = p[j][k]) >= 0; k++) { /* p list */
	    new_p = hash(alphabet[old_p]);	/* new position of letter */
	    score += lo->logodds[i][old_p] * f[new_p];	/* weighted sum */
	    freq += f[new_p];			/* total frequency */
	  } /* p list */
	  row[j] = score / freq;			/* weighted average */
        }
        if (row[j] > best_score) {		/* update best score */
          best_score = row[j];			/* best score */
          best_letter = new_alphabet[j];	/* best letter */
        }
      } /* position in new alphabet */
      /*printf("\n");*/
      if (lo->logodds[i]) free(lo->logodds[i]); /* free old space */
      lo->logodds[i] = row; 	/* replace row with permuted/substituted row */
      lo->best_seq[i] = best_letter;		/* build best matching seq */
    } /* position in motif */

    lo->best_seq[i] = '\0';	/* terminate best matching sequence */
    lo->alen = alen; 		/* set length of alphabet to new alphabet's */
    lo->dna = !strcmp(new_alphabet, DNAB);	/* TRUE if DNA motif */
    lo->name = nmotifs+1;	/* name of motif */
    if (lo->pair) lo->w /= 2;	/* double matrix-- divide width in half */
    lo->ws = lo->w * (translate_dna ? 3 : 1);	/* motif width in sequence */

  } /* motif */

  /*
    Convert motif matrices to integer so entries are in [0...range].
  */
  if (range>0) scale_lo(los, nmotifs, range);

  /* 
    Create a double-letter log-odds matrix for efficiency. 
  */
  make_double_lo(los, nmotifs);

  /* 
    Set sequence alphabet to DNA if translating.
  */
  if (translate_dna) setalph(0);

  fclose(fptr);
  unlink(filename);

  return nmotifs; 
} /* read_log_odds */

/**********************************************************************/
/*
        min_max

        Find the lowest and highest scores possible with a
        given log-odds matrix.  Returns the single lowest entry in
	the log-odds matrix.
*/
/**********************************************************************/
EXTERN void min_max(
  LOGODDS logodds,		/* log-odds matrix */
  int w,                        /* width of motif */
  int a,                        /* length of alphabet */
  double *minimum,              /* minimum score */
  double *maximum 		/* minimum score */
)
{
  int i, j;
  double min_score = 0;
  double max_score = 0;

  for (i=0; i < w; i++) {
    double min = BIG;
    double max = LITTLE;
    for (j=0; j < a; j++) {
      min = MIN(min, logodds(i, j));
      max = MAX(max, logodds(i, j));
    }
    min_score += min;
    max_score += max;
  }
  *minimum = min_score;
  *maximum = max_score;
} /* min_max */

/***********************************************************************/
/*
	motif_corr

	Compute correlations between pairs of motifs.
	The correlation between two motifs is the maximum sum of 
	Pearson's correlation coefficients for aligned columns divided 
	by the width of the shorter motif.  The maximum is found by 
	trying all alignments of the two motifs. 

	The correlations are saved in lower-diagonal form in the 
	logodds array; each motif records correlations between it
	and lower-numbered motifs.
*/
/***********************************************************************/
extern void motif_corr(
  int nmotifs,			/* number of motifs */
  LO *los[]			/* array of logodds structures */
)
{
  int i, j, k, l, m, n, o;
  double *means[MAXLO];			/* means of motif columns */

  /* compute the motif column means */
  for (i=0; i<nmotifs; i++) {
    int w = los[i]->w;			/* width of motif i */
    int alen = los[i]->alen;		/* alphabet length */
    means[i] = NULL; Resize(means[i], w, double);
    for (j=0; j<w; j++) {		/* motif column */
      means[i][j] = 0;
      for (k=0; k<alen; k++) means[i][j] += los[i]->logodds(j,k);
      means[i][j] /= alen;
    }
  }

  /* compute the maximum sum of Pearson's correlation coefficient for 
     motif pairs
  */
  for (i=0; i<nmotifs; i++) {			/* "from" motif */
    int alen = los[i]->alen;		 	/* alphabet length */
    los[i]->corr = NULL; 
    Resize(los[i]->corr, nmotifs, double);	/* create correlation array */
    for (j=0; j<i; j++) {			/* "to" motif */
      double rsum_max = LITTLE;			/* max. sum of r */
      LO *lo1, *lo2;
      int w1, w2;
      double *mu1, *mu2;
      for (o=0; o<2; o++) {		/* align i to j, then j to i */
        int m1, m2;
        if (o==0) {			/* align motif i to motif j */
          m1 = i;
          m2 = j;
        } else {			/* align motif j to motif i */
          m1 = j;
          m2 = i;
        }
	lo1 = los[m1];
	lo2 = los[m2];
	w1 = lo1->w;
	w2 = lo2->w;
	mu1 = means[m1];
	mu2 = means[m2];
	for (k=0; k<w2; k++) {			/* alignment */
	  double rsum = 0;			/* sum of corr. coeffs */
	  for (l=0, m=k; l<w1 && m<w2; l++, m++) { /* column pair */
	    /* correlation of column l in motif 1 and column m in motif 2 */
	    double sum1=0, sum2=0, sum3=0;
	    double r;
	    for (n=0; n<alen; n++) { 		/* letter */
	      double a = lo1->logodds(l,n) - mu1[l];
	      double b = lo2->logodds(m,n) - mu2[m];
	      sum1 += a * b;
	      sum2 += a * a;
	      sum3 += b * b;
	    } /* letter */
	    r = sum1/sqrt(sum2*sum3);
	    rsum += r;
	  } /* column pair */
	  rsum_max = MAX(rsum_max, rsum);
	} /* alignment */
      } /* i to j, j to i */
      los[i]->corr[j] = rsum_max/MIN(los[i]->w, los[j]->w);	/* save */
    } /* to motif */
  } /* from motif */
} /* motif_corr */

/**********************************************************************/
/*
	make_double_lo

	Create a double-letter logodds matrix for efficiency.
*/
/**********************************************************************/
static void make_double_lo(
  LO *los[],		/* array of pointers to log-odds matrices */
  int nmotifs 		/* number of log-odds matrices in los */
) 
{
  int i, j, k, imotif; 

  for (imotif=0; imotif<nmotifs; imotif++) {	/* each motif */
    LO *lo = los[imotif];		/* motif */
    int w = lo->w;			/* width of motif */
    BOOLEAN pair = lo->pair;            /* double motif if true */
    int alen = lo->alen;		/* length of motif alphabet */
    int ncols = (alen+1)*(alen+1);	/* columns (letters) in double matrix */
    int nrows = (w+1)/2;		/* rows in hashed matrix */
    LOGODDS logodds = lo->logodds;	/* single-letter matrix */
    LOGODDS2 logodds2;			/* double-letter matrix */

    /* 
      Create the double-letter logodds matrix that gives scores for
      each possible pair of letters.  Alphabet length is extended
      by one for the "blank" character necessary when the sequence length
      or motif length is odd.
    */
    if (pair) nrows *= 2;		/* double number of rows if 2 motifs */
    create_2array(logodds2, LOGODDS2B, nrows, ncols);
    for (i=0; i<w; i+=2) {                      /* motif position */
      for (j=0; j<alen; j++) {                  /* letter in position+0 */
        for (k=0; k<=alen; k++) {               /* letter in position+1 */
          logodds2(i/2, dhash(j, k, alen)) =
            (LOGODDS2B) ((i==(w-1) || k==alen) ?
              logodds(i,j) : logodds(i,j)+logodds(i+1,k));
          if (pair) {
	    logodds2((nrows/2)+(i/2), dhash(j, k, alen)) =
	      (LOGODDS2B) ((i==(w-1) || k==alen) ?
		logodds(w+i,j) : logodds(w+i,j)+logodds(w+i+1,k));
          }
        } /* letter 1 */
      } /* letter 0 */
    } /* motif position */
    lo->logodds2 = logodds2;
  } /* motif */

} /* make_double_lo */

/**********************************************************************/
/*

	scale_lo

	Scale and round the log-odds matrices so that all entries are
	in range [0...range].
	Matrix entries will be rounded to log_10(range) significant digits.

	Bit score values  can be restored (with some loss of significance) by:
		score_bit = (score/scale) + (w*offset)
*/
/**********************************************************************/
static void scale_lo(
  LO *los[],		/* array of pointers to log-odds matrices */
  int nmotifs,		/* number of log-odds matrices in los */
  int range  		/* set entries in matrices to [0..range] */
)
{
  int i, j, imotif;
  BOOLEAN error;

  /*
	Compute the scale and offset factors for each motif
	and apply them to the logodds matrices to scale them
	to [0..range]
  */
  for (imotif=0, error=FALSE; imotif<nmotifs; imotif++) {
    LO *lo = los[imotif];               /* logodds structure */
    int a = lo->alen;                   /* length of alphabet */
    int w = lo->w;                      /* width of motif */
    int size = w*range+1;		/* largest possible score */
    double small = BIG;                 /* smallest entry pos logodds matrix */
    double large = -BIG;                /* largest entry pos logodds matrix */
    double smalln = BIG;                /* smallest entry neg logodds matrix */
    double largen = -BIG;               /* largest entry neg logodds matrix */

    /* find the smallest/largest entry in the logodds matrix */
    for (i=0; i < w; i++) {
      for (j=0; j < a; j++) {
        small = MIN(small, lo->logodds(i,j));
        large = MAX(large, lo->logodds(i,j));
        if (lo->pair) {                 /* negative motif */
          smalln = MIN(smalln, lo->logodds(w+i,j));
          largen = MAX(largen, lo->logodds(w+i,j));
        }
      }
    }
	
    /* skip this motif if it has no information */
    if (large==small || (lo->pair && largen==smalln)) {
      lo->scale = 0;
      continue;
    }

    /* compute scale and offset so that matrix entries in [0..range] */
    lo->scale = range/(large-small);	/* positive motif */
    lo->offset = small;
    if (lo->pair) {                     /* negative motif */
      /* set scale factor and offset for 3-class scores */
      lo->scalen = range/(largen-smalln);
      lo->offsetn = smalln;
      lo->ln_lambda1 = log(250000.0);	/* needed by score3class */
      lo->ln_lambda2 = 0;		/* needed by score3class */
      small = score3class(0, 0, lo);
      large = score3class(size-1, size-1, lo);
      lo->scale3 =  (size-1)/(large-small);
      lo->offset3 = small;
    }

    /* scale, offset and round logodds matrix entries to range [0..range] */
    for (i=0; i<w; i++) {
      for (j=0; j<a; j++) {
        lo->logodds(i,j) =
          bit_to_scaled(lo->logodds(i,j), 1, lo->scale, lo->offset);
        if (lo->pair) {                 /* negative motif */
          lo->logodds(w+i,j) =
            bit_to_scaled(lo->logodds(w+i,j), 1, lo->scalen, lo->offsetn);
        }
      }
    }

  } /* imotif */

} /* scale_lo */
  
/**********************************************************************/
/*
	shuffle_cols

	Shuffle the columns of each motif.
*/
/**********************************************************************/
extern void shuffle_cols(
  LO *los[],		/* array of pointers to log-odds matrices */
  int nmotifs 		/* number of log-odds matrices in los */
)
{
  int i, j, imotif;

  srand48(0);

  for (imotif=0; imotif<nmotifs; imotif++) {
    double tmp[MAXSITE+1][MAXALPH+1];	/* temporary logodds matrix */
    int permute[MAXSITE];		/* list to permute */
    LO *lo = los[imotif];
    int w = lo->w; 			/* width of motif */
    int a = lo->alen;			/* length of alphabet */

    /* set up permute list */
    for (i=0; i < w; i++) {		
      permute[i] = i;
    }

    /* do 50 random swaps to permute list */
    for (i=0; i < 50; i++) {		
      int c1 = (int) (w * drand48());
      int c2 = (int) (w * drand48());
      swap(permute[c1], permute[c2], int);
    }

    /* announce */
    ajFmtPrintF(outf,"Permuting columns of motif %d: ", imotif+1);
    for (i=0; i < w; i++) ajFmtPrintF(outf,"%d ", permute[i]);
    ajFmtPrintF(outf,"\n");

    /* move logodds to tmp in permuted column order */
    for (i=0; i < w; i++) {
      for (j=0; j < a; j++) {
	tmp[i][j] = lo->logodds(permute[i], j);
      }
    }

    /* copy tmp to logodds */
    for (i=0; i < w; i++) {
      for (j=0; j < a; j++) {
	lo->logodds(i,j) = tmp[i][j];
      }
    }
  }
} /* shuffle_cols */

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/logodds.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
