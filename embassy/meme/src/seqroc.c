/*
#define debug 
*/
/**********************************************************************/
/*
  seqroc <logodds> <dataset> <motif> <alphabet> <min_roc> [<plot>] 

	<logodds>	file with log-odds matrices
	<dataset>	file of sequences
	<motif>		file of sequence, "TNP?" pairs of family (Prosite tags)
			(see script getnonneg2)
	<alphabet>	alphabet of sequences
	<min_roc>	print "none" in pass summary if best ROC below <min_roc>
        [<plot>]	file to receive data for plotting 
	[-opt]		print recall and precision for recall = 1 threshold
			  best is best precision given recall = 1
	[-T <t>]	threshold to use

	Prints the sequence-level
	ROC, precision and recall for best match to a known
	concept where best is MAX(ROC), or, in case of ties, 
	MAX(recall+precision).

	Compute:
		ROC - receiver operator characteristic

		\integral_0^1 tpp d(fpp)

		where
			tpp = true positive proportion
			    = tp/(tp + fn)
			fpp = false positive proportion
			    = fp/(fp + tn)

		recall =	tp/(tp + fn) given <thresh>
		precision =	tp/(tp + fp) given <thresh>

	The logodds matrix contains:
		[
		<w> <thresh> <sig> <e_ll> <ic> <sites> <pal>
		<column 1 of motif log-odds matrix> 
		<column 2 of motif log-odds matrix> 
		...
		<column w of motif log-odds matrix> 
		] ...
*/
/**********************************************************************/

#define DEFINE_GLOBALS
#include "user.h"
#include "meme.h"
#include "general.h"

#define NBINS 1000

#define save_best \
      { \
	motif[imotif].roc = roc; \
	motif[imotif].pass = pass; \
	motif[imotif].recall = recall; \
	motif[imotif].precision = precision; \
	motif[imotif].pal = lo->pal; \
	motif[imotif].like = lo->e_ll; \
	motif[imotif].sig = lo->sig; \
	motif[imotif].ic = lo->ic; \
	motif[imotif].sites = lo->sites; \
	motif[imotif].w = lo->w; \
        motif[imotif].min_thresh = min_thresh; \
        motif[imotif].max_thresh = max_thresh; \
      }
#define check_best(roc1, roc2, rec1, rec2, pre1, pre2, opt) \
  (!opt && (roc1 > roc2 || (roc1 == roc2 && (rec1+pre1) > (rec2+pre2)))) \
  || (opt && (pre1 > pre2))

static void calc_roc (
  FILE *fdata,			/* opened dataset file */
  char *motif_file,		/* name of .motifs file */
  int nlo,			/* number of motifs in log-odds array */
  LO *los[],			/* log-odds matrix array */
  FILE *fplot,			/* file to receive plot points */
  double min_roc,		/* threshold for "none" to be printed */
  BOOLEAN opt			/* pick threshold to optimize precision */ 
);

extern int main (int argc, char *argv[])
{
  int i;
  char *nlogodds = NULL;
  char *dataset = NULL; 
  char *motif_file = NULL;
  char *alphabet = NULL;
  char *plot = NULL;
  double min_roc = 0;
  FILE *fdata;
  FILE *fplot = NULL;
  LO *los[MAXLO+1];			/* array of logodds matrices */
  int nmotifs;				/* number of motifs found by meme */
  BOOLEAN opt = FALSE;			/* choose threshold optimize precision*/
  double thresh = LITTLE;		/* threshold to use for all motifs */
  double  f[MAXASCII];			/* array of null letter probabilities */
  int range = -1;			/* do not change range of logodds */
  double scale;				/* scale factor */
  double offset;			/* offset factor */


#ifndef lint
  i = 1;
  DO_STANDARD_COMMAND_LINE(1,
    NON_SWITCH(1, <datafile>\n,
      switch (i++) {
        case 1: nlogodds = _OPTION_; break;
        case 2: dataset = _OPTION_; break;
        case 3: motif_file = _OPTION_; break;
        case 4: alphabet = _OPTION_; break;
        case 5: min_roc = atof(_OPTION_); break;
        case 6: plot = _OPTION_; break;
        default: COMMAND_LINE_ERROR;
      });
    FLAG_OPTN(1, opt, \tprint recall and precision for recall=1 threshhold,
      opt = TRUE)
    DATA_OPTN(1, T, <t>, \tthreshold to use, thresh = atof(_OPTION_))
  );
#endif

  /* open the datafile */
  fdata = fopen(dataset, "r");
  if (fdata == NULL) {
    printf("Cannot open file '%s'\n", dataset);
    exit(1);
  }

  /* open the plot file */
  if (argc >= 6) fplot = fopen(plot, "w");

  /* read in log-odds matrices */
  nmotifs = read_log_odds(FALSE, nlogodds, alphabet, range, los, f);

  /* use thresh if specified */
  if (thresh > LITTLE) {
    for (i=0; i<nmotifs; i++) {
      los[i]->thresh = thresh;
    }
  }

  /* calculate ROC for each motif in log-odds file */
  calc_roc(fdata, motif_file, nmotifs, los, fplot, min_roc, opt);

  return 0;
}

  
/**********************************************************************/
/*
	calc_roc

*/
/**********************************************************************/
static void calc_roc (
  FILE *fdata,			/* opened dataset file */
  char *motif_file,             /* name of .motifs file */
  int nlo,			/* number of motifs in log-odds array */
  LO *los[],			/* log-odds matrix array */
  FILE *fplot,			/* file to receive plot points */
  double min_roc,		/* threshold for "none" to be printed */
  BOOLEAN opt			/* pick threshold to optimize precision */ 
)
{
  int i, k;
  int pass;
  int nmotifs;			/* number of known motifs */
  int imotif;
  double recall, precision;
  double best_roc, best_recall, best_precision;
  char *best_name = "none";		/* best for found motif */
  MOTIF motif[NMOTIFS];			/* info about each motif */
  int maxpos;
  double *plotx, *ploty, *best_plotx, *best_ploty;
  int pos_bins[NMOTIFS][NBINS];		/* bins for calculating roc */
  int neg_bins[NMOTIFS][NBINS];		/* bins for calculating roc */
  int tp_thresh[NMOTIFS];		/* true pos for threshold */
  int fp_thresh[NMOTIFS];		/* false pos for threshold */
  int kw = 0;				/* width of widest known motif */
  DATASET dataset;			/* the dataset */

  /* read in the motifs, storing them as hash tables */
  nmotifs = read_motifs (fdata, motif_file, motif, 1, &dataset);

  /* set up to save stuff for plotting later */
  maxpos = 0;
  for (i=0; i<nmotifs; i++) maxpos = MAX(maxpos, motif[i].pos);
  plotx = (double *) malloc ((maxpos + 2) * sizeof(double));
  ploty = (double *) malloc ((maxpos + 2) * sizeof(double));
  best_plotx = (double *) malloc ((maxpos + 2) * sizeof(double));
  best_ploty = (double *) malloc ((maxpos + 2) * sizeof(double));

  printf("Summary for each pass, sequence-level:\n");
  printf(" pass      sig     e_ll IC/col  sites width motif         ROC recall precis  thr\n");
  printf(" ----      ---     ---- ------  ----- ----- -----         --- ------ ------  ---\n");

  /* loop over each of the discovered motifs, reading them
	in, scoring the dataset and computing ROC
  */
  for (pass=1; pass<=nlo; pass++) {
    LO *lo = los[pass-1];		/* current log-odds structure */
    int seq, off;
    double min, max;			/* score limit values */
    double bin_size;			/* width of bin */
    double best_pi = 0;			/* number of points plotted */
    double avg_score;			/* average score per column */

    /* skip degenerate motifs; too few sites to be real */
    if (lo->sites <= 1.0) continue;

    /* figure out the size of the bins */
    min_max(lo->logodds, lo->w, lo->alen, &min, &max);
    avg_score = (((lo->alen-1) * min) + max)/(lo->alen * lo->w);
    bin_size = (max - min)/(NBINS - 1);
#define FIND_BIN(x)	(MAX(0, (int) floor((x - min)/bin_size)))
#define THRESH(bin)	(min + bin * bin_size)

    /* calculate the ROC for this motif against the known motif */

    best_roc = -1;
    best_recall = best_precision = 0;

    /* initialize the bins holding counts of pos and neg samples 
	and the numbers of true and false positives */
    for (imotif=0; imotif<nmotifs; imotif++) {
      kw = MAX(kw, motif[imotif].width);		/* widest known motif */
      tp_thresh[imotif] = fp_thresh[imotif] = 0;
      for (k=0; k<NBINS; k++) {
        pos_bins[imotif][k] = neg_bins[imotif][k] = 0;
      }
    }

    /* loop over sequences in dataset; fill in the known motif X bins arrays */
    for (seq=0; seq < dataset.n_samples; seq++) {
      char *sample_name = dataset.samples[seq]->sample_name;
      char *sequence = dataset.samples[seq]->res;	/* integer encoded */
      int length = dataset.samples[seq]->length;
      int bin;
      int w = lo->w;			/* width of meme motif */
      double best_score = LITTLE;	/* best score for this sequence */

      /* skip sequences labeled col=0 for all motifs
         in .tag file; "?/P" in Prosite 
      */
      for (imotif=0; imotif<nmotifs; imotif++) {
        if (!hash_lookup(sample_name, 0, motif[imotif].ht)) break;
      }
      if (imotif == nmotifs) continue;		/* don't score this seq */

      /* calculate scores for this sequence; save the best score */
      for (off=0; off <= length-w; off++) {	/* site start */
	double score = 0;
        int col;

        /* score for this position */
	for (col=off, k=0; k<w; col++, k++) {/* position in sequence */
	  score += lo->logodds(k, (int) sequence[col]);
	}
	best_score = MAX(score, best_score);
      }	/* offset */

      /* put sample score in appropriate bins */
      bin = FIND_BIN(best_score);
      if (bin < 0 || bin >= NBINS) 
        printf("bug: bin=%d NBINS=%d score %f min %f max %f bin_size %f\n",	
	  bin, NBINS, best_score, min, max, bin_size);
      for (imotif=0; imotif<nmotifs; imotif++) {
	/* classify sample according to known motif and threshold */
	/* if col=1, it is "T" or "N" in prosite */
	if (hash_lookup(sample_name, 1, motif[imotif].ht)) {
	  /* positive */
	  pos_bins[imotif][bin]++;
	  if (best_score > lo->thresh) tp_thresh[imotif]++;
        } else if (hash_lookup(sample_name, 0, motif[imotif].ht)) {
          /* skip for this motif */
	} else {
	  /* negative */
	  neg_bins[imotif][bin]++;
	  if (best_score > lo->thresh) fp_thresh[imotif]++;
	}

      } /* known motif */

    } /* sequence */

    /* loop over known motifs */
    for (imotif=0; imotif<nmotifs; imotif++) {
      double newfpp;			/* new false pos proportion */
      double newtpp = 0;
      int bin;
      int pi;
      /* clear all the counts */
      int tp = 0;		/* count of true positives */
      int fp = 0;		/* count of false positives */
      double tpp = 0;		/* true positive proportion */
      double fpp = 0;		/* false positive proportion */
      double roc = 0;		/* receiver operating characteristic */
      int pos = motif[imotif].pos;	/* tp + fn */
      int neg = 0;			/* tn + fp */
      double min_thresh=LITTLE, max_thresh=LITTLE;

      /* calculate total negatives tn + fp */
      for (bin=NBINS-1; bin>0; bin--) {
        neg += neg_bins[imotif][bin];
	/*printf("p=%3d n=%3d fp=%3d\n", 
          pos_bins[imotif][bin], neg_bins[imotif][bin], neg);*/
      }

      for (bin=0; bin<NBINS; bin++) 

      /* plot first point */
      pi = 0;				/* number of points plotted */
      if (fplot) {plotx[pi] = 0; ploty[pi++] = 0;}

      /* integrate ROC */
      for (bin=NBINS-1; bin>=0; bin--) {
	/* update ROC if new positive found */
	if (pos_bins[imotif][bin] > 0) {
	  tp += pos_bins[imotif][bin];
	  fp += neg_bins[imotif][bin];
	  /* trapezoidal rule : (y2 + y1)/2 dx */
          newtpp = (double) tp / pos;
	  newfpp = (double) fp / neg;
	  roc += .5 * (newtpp + tpp) * (newfpp - fpp);	
	  if (fplot) {plotx[pi] = newfpp; ploty[pi++] = newtpp;}
	  tpp = newtpp;
	  fpp = newfpp;
	} else {
	  fp += neg_bins[imotif][bin];
	}
      } /* integrate */
      /* add in last term */
      /* trapezoidal rule : (y2 + y1)/2 dx */
      if (newtpp != 1.0) 
	printf("newtpp < 1 for motif %d. newtpp = %g tp %d pos %d\n", 
	  imotif, newtpp, tp, pos);
      newfpp = 1.0;
      roc += .5 * (newtpp + tpp) * (newfpp - fpp);	
      if (fplot) {plotx[pi] = newfpp; ploty[pi++] = newtpp;}

      /* save the best known motif so far */
      recall = (double) tp_thresh[imotif]/pos;
      precision = (fp_thresh[imotif] == 0) ? 1 :
	(double) tp_thresh[imotif] /
	  (tp_thresh[imotif] + fp_thresh[imotif]);

      /* overall best match for this discovered motif */
      /*if (roc > best_roc) { */
      if (check_best(roc, best_roc, recall, 
        best_recall, precision, best_precision, opt)) {
	best_roc = roc;
	best_name = motif[imotif].name;
	best_recall = recall;
	best_precision = precision;
	{double *tmp = best_plotx; best_plotx = plotx; plotx = tmp;}
	{double *tmp = best_ploty; best_ploty = ploty; ploty = tmp;}
	best_pi = pi;
      }

      /* best match for this known motif; save the motif record */
      if (roc > motif[imotif].roc)  save_best;
{
int tp = tp_thresh[imotif];
int fp = fp_thresh[imotif];
int fn = pos-tp;
int tn = neg-fp;
printf("TP= %d FN= %d FP= %d TN= %d\n", tp, fn, fp, tn);
}

    } /* known motif */

    /* print the results */
    printf ("%5d %8.1e %c%7.0f %6.3f %6.1f %5d", pass, lo->sig, 
      lo->pal ? 'P' : ' ', lo->e_ll, lo->ic, lo->sites, lo->w);

    if (best_roc >= min_roc) {
      printf (" %-7.7s %9.5f %6.3f %6.3f %4.1f\n", 
	best_name, best_roc,
	best_recall, best_precision, lo->thresh);
    } else {
      printf (" %-5.5s\n", "none");
    }

    /* plot the results */
    if (fplot) {
      fprintf(fplot, "pass %d motif %s ROC %9.5f \n",pass, best_name, best_roc);
      for (i=0; i<best_pi; i++) { 
	fprintf(fplot, "%f %f\n", best_plotx[i], best_ploty[i]);
      }
    }
  } /* meme motif */

  if (nmotifs > 0) {
    double total_roc = 0;
    double total_recall = 0;
    double total_precision = 0;

    printf ("\nSummary for known motifs, sequence-level:\n");
    printf ("motif         ROC recall precis   pass      sig     e_ll IC/col  sites width\n");
    printf ("-----         --- ------ ------   ----      ---     ---- ------  ----- -----\n");
    for (i=0; i<nmotifs; i++) {
      if (motif[i].roc > min_roc) {
	printf("%-7.7s %9.5f %6.3f %6.3f   %4d %8.1e %c%7.0f %6.3f %6.1f %5d\n",
	  motif[i].name, motif[i].roc,
	  motif[i].recall, motif[i].precision,
	  motif[i].pass, motif[i].sig, motif[i].pal ? 'P' : ' ',
          motif[i].like, motif[i].ic, motif[i].sites,
             motif[i].w);
	total_roc += motif[i].roc;
	total_recall += motif[i].recall;
	total_precision += motif[i].precision;
      }
    }
    printf("\nAverage (ROC recall precision) %8.6f %8.6f %8.6f\n", 
      total_roc/nmotifs, total_recall/nmotifs, total_precision/nmotifs);
  }
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/seqroc.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
