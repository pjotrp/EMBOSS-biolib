/*#define debug */
/**********************************************************************/
/*
  roc <logodds> <dataset> <motif> <alphabet> <min_roc> [<plot>] 

	<logodds>	file with log-odds matrices
	<dataset>	file of sequences
	<motif>		file of seq, offset pairs of positive examples
	<alphabet>	alphabet of sequences
	<min_roc>	print "none" in pass summary if best ROC below <min_roc>
        [<plot>]	file to receive data for plotting 

	Prints the ROC, precision and recall for best match to a known
	concept where best is MAX(roc).

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

#define NBINS 100
#define RANGE (MAX(motif[imotif].width-1,lo->w-1))
#define NSHIFTS (2*MAXSITE+1)

static void calc_roc (
  FILE *fdata,			/* opened dataset file */
  char *motif_file,		/* name of .motifs file */
  int nlo,			/* number of motifs in log-odds array */
  LO *los[],			/* log-odds matrix array */
  FILE *fplot,			/* file to receive plot points */
  double min_roc		/* threshold for "none" to be printed */
);

extern int main (int argc, char *argv[])
{
  char *nlogodds = argv[1];
  char *dataset = argv[2];
  char *motif_file = argv[3];
  char *alphabet = argv[4];
  double min_roc = atof(argv[5]);
  char *plot = argv[6];
  FILE *fdata;
  FILE *fplot = NULL;
  LO *los[MAXLO+1];			/* array of logodds matrices */
  int nmotifs;				/* number of motifs found by meme */
  double f[MAXASCII];			/* array of null letter probabilities */
  int range = 100;			/* do not change range of logodds */
  double scale;				/* scale factor */
  double offset;			/* offset factor */


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

  /* calculate ROC for each motif in log-odds file */
  calc_roc(fdata, motif_file, nmotifs, los, fplot, min_roc);

  return 0;
}

/**********************************************************************/
/*
	calc_roc

*/
/**********************************************************************/
static void calc_roc (
  FILE *fdata,			/* opened dataset file */
  char *motif_file,		/* name of .motifs file */
  int nlo,			/* number of motifs in log-odds array */
  LO *los[],			/* log-odds matrix array */
  FILE *fplot,			/* file to receive plot points */
  double min_roc		/* threshold for "none" to be printed */
)
{
  int i, j, k;
  int pass;
  int nmotifs;				/* number of known motifs */
  int imotif;
  int shift, best_shift;
  double recall, precision;
  double best_roc, best_recall, best_precision;
  char *best_name = NULL;		/* best for found motif */
  MOTIF motif[NMOTIFS];			/* info about each motif */
  int nscores;				/* total number of scores computed */
  int maxpos;
  double *plotx, *ploty, *best_plotx, *best_ploty;
  int pos_bins[NMOTIFS][NSHIFTS][NBINS];/* bins for calculating roc */
  int neg_bins[NMOTIFS][NSHIFTS][NBINS];/* bins for calculating roc */
  int tp_thresh[NMOTIFS][NSHIFTS];	/* true pos for threshold */
  int fp_thresh[NMOTIFS][NSHIFTS];	/* false pos for threshold */
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

  printf("Summary for each pass, site-level:\n");
  printf(" pass      sig     e_ll IC/col  sites width motif   shift      ROC recall precis\n");
  printf(" ----      ---     ---- ------  ----- ----- -----   -----      --- ------ ------\n");

  /* loop over each of the discovered motifs, reading them
	in, scoring the dataset and computing ROC
  */
  for (pass=1; pass<=nlo; pass++) {
    LO *lo = los[pass-1];		/* current log-odds structure */
    int seq, off;
    double min, max;			/* score limit values */
    double bin_size;			/* width of bin */
    double best_pi = 0;			/* number of points plotted */
    int max_w = lo->w;			/* largest motif width */
    double avg_score;			/* average score per column */

    /* skip degenerate motifs; too few sites to be real */
    /*if (lo->sites <= 1.0) continue;*/

    /* figure out the size of the bins */
    min_max(lo->logodds, lo->w, lo->alen, &min, &max);
    avg_score = (((lo->alen-1) * min) + max)/(lo->alen * lo->w);
    /* printf("average column score = %f\n", avg_score); */
    bin_size = (max - min)/(NBINS - 1);
#define FIND_BIN(x)	(MAX(0, (int) floor((x - min)/bin_size)))

    /* calculate the ROC for this motif against the known motifs 
       allowing different shifts */

    best_roc = 0;
    best_shift = 0;
    best_recall = best_precision = 0;

    /* initialize the bins holding counts of pos and neg samples 
	and the numbers of true and false positives */
    for (imotif=0; imotif<nmotifs; imotif++) {
      kw = MAX(kw, motif[imotif].width);		/* widest known motif */
      for (j=0; j<=2*RANGE; j++) {
        tp_thresh[imotif][j] = fp_thresh[imotif][j] = 0;
        for (k=0; k<NBINS; k++) {
	  pos_bins[imotif][j][k] = neg_bins[imotif][j][k] = 0;
        }
      }
    }

    /* loop over sequences in dataset; fill in the known motif X bins arrays */
    nscores = 0;
    for (seq=0; seq < dataset.n_samples; seq++) {
      char *sample_name = dataset.samples[seq]->sample_name;
      char *sequence = dataset.samples[seq]->res;       /* integer encoded */
      int length = dataset.samples[seq]->length;
      int bin;

      /* calculate scores for this sequence */
      for (off=-max_w; off < length+kw; off++) {	/* site start */
	double score = 0.0;
        int col;

        /* score for this position; allow for overhang-- set col score = avg */
	for (col=off, k=0; k < lo->w; col++, k++) {/* position in sequence */
          if (col < 0 || col >= length) {
            score += avg_score;
          } else {
            score += lo->logodds(k, (int)sequence[col]);
          }
	}
	/*if (score > lo->thresh) printf("seq %d off %d score %f\n", seq, off, score); */
        nscores++;

	/* put sample score in appropriate bins */
        bin = FIND_BIN(score);
        if (bin < 0 || bin >= NBINS) printf("bug: bin=%d NBINS=%d\n",	
		bin, NBINS);
        for (imotif=0; imotif<nmotifs; imotif++) {

	  /* try different shifts of this motif */
  	  /*for (shift=-RANGE; shift < RANGE; shift++) {*/
  	  for (shift=-max_w+1; shift < motif[imotif].width; shift++) {
	    int ioff = off - shift;			/* shift the site */
	    /* legal shift? */
/*
	    if (ioff < 0 || ioff > length-motif[imotif].width) continue;
*/
	    if (ioff < 0 || ioff >= length) continue;
            /* classify sample according to known motif and threshold */
	    if (hash_lookup(sample_name, ioff+1, motif[imotif].ht)) {
	      /* positive */
              pos_bins[imotif][shift+RANGE][bin]++;
              if (score > lo->thresh) tp_thresh[imotif][shift+RANGE]++;
            } else {
              /* negative */
              neg_bins[imotif][shift+RANGE][bin]++;
              if (score > lo->thresh) fp_thresh[imotif][shift+RANGE]++;
            }
          } /* shift */
        } /* known motif */

      }	/* offset */

    } /* sequence */

    /*printf("\n");*/

    /* loop over known motifs */
    for (imotif=0; imotif<nmotifs; imotif++) {
      double newfpp;			/* new false pos proportion */
      double newtpp = 0;

      /* try different shifts of this motif */
      /*for (shift=-RANGE; shift <= RANGE; shift++) {*/
      for (shift=-max_w+1; shift < motif[imotif].width; shift++) {
        int bin;
        int pi;
        /* clear all the counts */
        int tp = 0;		/* count of true positives */
        int fp = 0;		/* count of false positives */
        double tpp = 0;		/* true positive proportion */
        double fpp = 0;		/* false positive proportion */
        double roc = 0;		/* receiver operating characteristic */
        int n = 0;		/* tn + fp */

        /* calculate total negatives tn + fp */
        for (bin=0; bin<NBINS; bin++) n += neg_bins[imotif][shift+RANGE][bin];

        /* plot first point */
        pi = 0;				/* number of points plotted */
        if (fplot) {plotx[pi] = 0; ploty[pi++] = 0;}

        /* integrate ROC */
	for (bin=NBINS-1; bin>=0; bin--) {
	  /* update ROC if new positive found */
	  if (pos_bins[imotif][shift+RANGE][bin] > 0) {
	    tp += pos_bins[imotif][shift+RANGE][bin];
	    fp += neg_bins[imotif][shift+RANGE][bin];
	    /* trapezoidal rule : (y2 - y1)/2 dx */
	    newtpp = (double) tp / motif[imotif].pos;
	    newfpp = (double) fp / n;
	    roc += .5 * (newtpp + tpp) * (newfpp - fpp);	
	    if (fplot) {plotx[pi] = newfpp; ploty[pi++] = newtpp;}
	    tpp = newtpp;
	    fpp = newfpp;
	  } else {
	    fp += neg_bins[imotif][shift+RANGE][bin];
	  }
	} /* integrate */
        /*printf("shift %d tp = %d fp = %d pos = %d\n", 
          shift, tp, fp, motif[imotif].pos); */
	/* add in last term */
	/* trapezoidal rule : (y2 - y1)/2 dx */
	/*newtpp = 1.0;*/
        if (newtpp != 1.0) 
          printf("newtpp < 1 for motif %d shift %d. tp %d pos %d\n", 
            imotif, shift, tp, motif[imotif].pos);
	newfpp = 1.0;
	roc += .5 * (newtpp + tpp) * (newfpp - fpp);	
	if (fplot) {plotx[pi] = newfpp; ploty[pi++] = newtpp;}

	/* save the best known motif/shift so far */
	recall = (double) tp_thresh[imotif][shift+RANGE]/motif[imotif].pos;
	precision = (tp_thresh[imotif][shift+RANGE] == 0) ? 0 :
          (double) tp_thresh[imotif][shift+RANGE] /
	    (tp_thresh[imotif][shift+RANGE] + fp_thresh[imotif][shift+RANGE]);
#ifdef debug
        printf("motif=%d, shift=%d,tpt=%d fpt=%d pos=%d recall=%4.3f precision=%4.3f roc=%6.4f\n", 
	  imotif, shift,
          tp_thresh[imotif][shift+RANGE],
          fp_thresh[imotif][shift+RANGE],
          motif[imotif].pos,
          recall, precision, roc); 
#endif
	/* overall best match for this discovered motif */
	if (roc > best_roc) { 
	  best_roc = roc;
	  best_shift = shift;
	  best_name = motif[imotif].name;
	  best_recall = recall;
	  best_precision = precision;
          {double *tmp = best_plotx; best_plotx = plotx; plotx = tmp;}
          {double *tmp = best_ploty; best_ploty = ploty; ploty = tmp;}
          best_pi = pi;
        }
        /* best match for this known motif */
        if (roc > motif[imotif].roc) {
          motif[imotif].roc = roc;
          motif[imotif].shift = shift;
          motif[imotif].pass = pass;
          motif[imotif].recall = recall;
          motif[imotif].precision = precision;
          motif[imotif].pal = lo->pal;
          motif[imotif].like = lo->e_ll;
          motif[imotif].sig = lo->sig;
          motif[imotif].ic = lo->ic;
          motif[imotif].sites = lo->sites;
          motif[imotif].w = lo->w;
        }

      } /* shift */

    } /* known motif */

    /* print the results */
    printf ("%5d %8.1e %c%7.0f %6.3f %6.1f %5d", pass, lo->sig, 
      lo->pal ? 'P' : ' ', lo->e_ll, lo->ic, lo->sites, lo->w);

    if (best_roc >= min_roc) {
      printf (" %-7.7s %5d %8.4f %6.3f %6.3f\n", 
        best_name, best_shift, best_roc,
        best_recall, best_precision);
    } else {
      printf (" %-5.5s\n", "none");
    }

    /* plot the results */
    if (fplot) {
      fprintf(fplot, "pass %d motif %s ROC %8.4f \n",pass, best_name, best_roc);
      for (i=0; i<best_pi; i++) { 
        fprintf(fplot, "%f %f\n", best_plotx[i], best_ploty[i]);
      }
    }

    rewind(fdata);
  } /* found motif */ 

  if (nmotifs > 0) {
    double total_roc = 0;
    double total_recall = 0;
    double total_precision = 0;

    printf ("\nSummary for known motifs, site-level:\n");
    printf ("motif  shift      ROC recall precis   pass      sig     e_ll IC/col  sites width\n");
    printf ("-----  -----      --- ------ ------   ----      ---     ---- ------  ----- -----\n");
    for (i=0; i<nmotifs; i++) {
      if (motif[i].roc > min_roc) {
	printf("%-7.7s %4d %8.4f %6.3f %6.3f   %4d %8.1e %c%7.0f %6.3f %6.1f %5d\n",
	  motif[i].name, motif[i].shift, motif[i].roc,
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
