/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1994, The Regents of the University of California    *
*	Author: Timothy L. Bailey				       *
*								       *
***********************************************************************/
/* 
	histogram.c
*/
#include "macros.h"
#include "histogram.h"

/* macros */
#define get_data(I, N, F, A, X) \
	(A == NULL ? (fscanf(F, "%lf", &X) == 1) : \
          (I < N ? (X=A[I++], TRUE) : FALSE))

/* local functions */

#ifdef HIST_SO
#include "general.h"
/**********************************************************************/
/*
  histogram [-f file] [-nb n_bins] [-nt n_ticks] [-bs bar_symbol]

	Plot histograms of lists of numbers
	Each stream of numbers should be preceded with the line
	# <min> <max>
	where <min> and <max> will be the minimum and maximum on
	the x-axis of the histogram.  
	If <min> = <max>, the
	data will be stored and the actual min, max, avg and std
	computed and used in making the histogram.
	Otherwise, the numbers are read but not stored.

	Each histogram is plotted vertically with each bar occupying
	a line written to standard output.  All samples are put in
	the bin whose center they are closest to.

        file		- name of file to read; default = standard input

	n_bins		- number of bins on the X-axis (which
			  is plotted vertically); default = N_BINS 
	n_ticks		- maximum height of a bin, ie, the resolution
			  of the Y-axis (which is plotted horizontally);
			  default = N_TICKS 
	bar_symbol	- the symbol to print when drawing the bars;
			  default = BAR_SYMBOL
*/
/**********************************************************************/
main(int argc, char *argv[])
{
  /* set up defaults */
  char *fname = NULL;
  int n_bins = N_BINS;
  int n_ticks = N_TICKS;
  char bar_symbol = BAR_SYMBOL;

  /* get the command line arguments */
  DO_STANDARD_COMMAND_LINE(0, 
    DATA_OPTN(1, f, <file>, \tname of file with values, fname = _OPTION_);
    DATA_OPTN(1, nb, <nb>, \tnumber of X bins, n_bins = atoi(_OPTION_));
    DATA_OPTN(1, nt, <nt>, \tnumber of Y ticks, n_ticks = atoi(_OPTION_)); 
    DATA_OPTN(1, bs, <bs>, \tcharacter to use in bars, 
      bar_symbol = _OPTION_[0]);
  );

  /* call the histogram plotting routine */
  file_histogram(fname, n_bins, n_ticks, bar_symbol);
}
#endif

/**********************************************************************/
/*
	file_histogram

	Print histograms for data in a file.
	The first line of each set of values should be
		# <min> <max>
	If <min> == <max>, the data will be read in and
	the actual min, max, avg and std computed and used.
	Otherwise, the data is not stored and avg and std
	are not computed.
*/
/**********************************************************************/
extern void file_histogram(
  char *fname,		/* name of file; NULL means stdin */
  int n_bins,		/* number of bins (X-axis, plotted vertically) */
  int n_ticks,		/* number of ticks (Y-axis, plotted horizontally) */
  char bar_symbol	/* symbol to build bars from */
)
{
  FILE *file;		/* file containing values to plot */
  int pass;
  double min, max;

  /* open the file */
  if (fname == NULL) {
    file = stdin;	/* no filename given */
  } else {
    file = fopen(fname, "r");
    if (file == NULL) {
      printf("Couldn't open file %s \n", fname);
      exit(1);
    }
  }

  /* plot histograms for each set of values */
  pass = 0;
  while (fscanf(file, "# %lf %lf", &min, &max) == 2) {
    int n = 0;
    double *data = NULL;
    printf("Histogram %3d\n-------------\n", ++pass);
    if (min == max) {
      /* read in the data from the file */
      double x;
      while (fscanf(file, "%lf", &x) == 1) {
	if ((n % CHUNK) == 0) Resize(data, (n+CHUNK) * sizeof(double), double);
	data[n++] = x;
      }
    }
    histogram(file, data, n, min, max, n_bins, n_ticks, bar_symbol);
  }
}

/**********************************************************************/
/*
        histogram

	Given a list of numbers, plot a histogram of their
	values.  
	If an array is given, the 5 standard deviation
	point is also plotted.  Otherwise, 
        the values are read from the file
	and plotted without using a local array.
	Print their average and standard deviation as well.

	The histogram is plotted vertically.
*/
/**********************************************************************/

extern void histogram(
  FILE *file,		/* file containing values to plot */
  double *data_array,	/* array containing values to plot */
  int n,		/* size of array */
  double min,		/* minimum value on X-axis; ignored if array */
  double max,		/* maximum value on X-axis; ignored if array */
  int n_bins,		/* number of bins (X-axis, plotted vertically) */
  int n_ticks,		/* number of ticks (Y-axis, plotted horizontally) */
  char bar_symbol	/* symbol to build bars from */
)
{
  double bin_size, avg, sd;
  int *bins = (int *) malloc(n_bins * sizeof(int));
  BOOLEAN stream = (data_array == NULL);

  init_histogram(stream, data_array, n,
    bins, n_bins, &bin_size, &min, &max);
  fill_histogram(file, data_array, n, min, bin_size, bins, n_bins);
  finish_histogram(min, n_bins, bins, bin_size, n_ticks, bar_symbol, &avg, &sd);

  myfree(bins);
}

/**********************************************************************/
/*
	init_histogram

	Set up the environment for printing a histogram.
	Returns bin_size, min, max and clears the bins.
*/
/**********************************************************************/
extern void init_histogram(
  BOOLEAN stream,	/* reading from a file */
  double *data_array,	/* array containing values to plot */
  int n,		/* size of array */
  int *bins,		/* bins of histogram */
  int n_bins,		/* number of bins (X-axis, plotted vertically) */
  double *bin_size,	/* size of a bin */
  double *minimum,	/* minimum value on X-axis; ignored if array */
  double *maximum 	/* maximum value on X-axis; ignored if array */
)
{
  int i;
  double min = *minimum;
  double max = *maximum;
  double bs, z;

  /* compute average and standard deviation and get
     maximum and minimum values for computing bin size
  */
  if (!stream) {
    double avg, std, var;
    double sum = 0.0;
    max = LITTLE;
    min = BIG;
    /* average */
    for (i=0; i<n; i++) {
      double x = data_array[i];
      sum += x;
      max = MAX(max, x);
      min = MIN(min, x);
    }
    avg = sum/n;
    /* standard deviation */
    sum = 0.0;
    for (i=0; i<n; i++) {
      double d = avg - data_array[i];
      sum += d * d;
    }
    var = sum/(n - 1);
    std = sqrt(var);

    printf("Max X = %g\n", max);
    printf("Min X = %g\n", min);
    printf("Avg X = %g\n", avg);
    printf("Std X = %g\n", std);
  }

  /* calculate the appropriate width of a bin */
  bs = (max - min)/(n_bins);		/* width of a bin on X-axis */
  if (bs == 0) bs = 1;			/* prevent size 0 bin */
  /* bin_size = (1, 2, 5) * 10^x */
  z = floor(log10(bs));
  z = exp10(z);
  if (bs <= z) {
    bs = z;
  } else if (bs <= z * 2) {
    bs = z * 2;
  } else if (bs <= z * 5) {
    bs = z * 5;
  } else if (bs <= z * 10) {
    bs = z * 10;
  }

  /* round min to an even multiple of bin_size */
  i = (int) (min/bs);
  min = i * bs;
  /*printf("min %f max %f bin_size %f\n", min, max, bs); */

  /* clear the bins */
  for (i=0; i<n_bins; i++) bins[i] = 0;

  /* return the values */
  *bin_size = bs;
  *maximum = max;
  *minimum = min;
}

/**********************************************************************/
/*
	fill_histogram

	Put the values in the bins.  The values in bin i
	range from  (min + i*bs - bs) <= x < (min + i*bs + bs).
*/
/**********************************************************************/
extern void fill_histogram(
  FILE *file,		/* file containing values to plot */
  double *data_array,	/* array containing values to plot */
  int n,		/* size of array */
  double min,		/* minimum value on X-axis */
  double bin_size,	/* size of a bin */
  int *bins,		/* bins of histogram */
  int n_bins		/* number of bins (X-axis, plotted vertically) */
)
{
  int i, j;
  double x;

  /* put samples in bins */
  i = 0;
  while (get_data(i, n, file, data_array, x)) {
    j = (int) ((x - min + bin_size/2)/bin_size);	/* x in bin j */
    if (j >= n_bins) j = n_bins - 1;
    if (j < 0) j = 0;
    ++bins[j];
  }
}

/**********************************************************************/
/*
	finish_histogram

	Print out the histogram prepared with init_ and fill_histogram.
	The labels of the bins are the centers of the bins; ie, values
	in the bin range from (label - bs) <= x < (label + bs).
	Returns the sample mean and standard deviation of the data.

*/
/**********************************************************************/
extern void finish_histogram(
  double min,		/* minimum value on X-axis; ignored if array */
  int n_bins,		/* number of bins (X-axis, plotted vertically) */
  int *bins,		/* bins of histogram */
  double bin_size,	/* size of a bin */
  int n_ticks,		/* number of ticks (Y-axis, plotted horizontally) */
  char bar_symbol,	/* symbol to build bars from */
  double *avg,		/* mean of data */
  double *sd		/* standard deviation of data */
)
{
  int i, j, k, x_prec, sigma1, sigma2;
  double x, xp;
  double tick_size = 1; /* size of each division on y-axis */
  int first_non_empty_bin;
  int last_non_empty_bin;
  double mean, std, count;

  /* number of digits after the decimal on x-axis labels */
  x_prec = (int) ceil(-1 * log10(bin_size));
  x_prec = MAX(x_prec, 0);

  /* start the histogram */

  /* print y-axis labels "0	10	20 ... " */
  for (i=0; i<80; i++) putchar('='); putchar('\n');
  printf("%19.19s ", "");
  for (i=0; i<=n_ticks; i+=10) printf("%-10d", j= (int)(i*tick_size));
  putchar('\n');

  /* print y-axis labels "+	+ 	+ ... " */
  printf("%8.8s (%9.9s)", "center", "count  ");
  for (i=0; i<=n_ticks; i+=10) printf("+         ");
  putchar('\n');
  for (i=0; i<80; i++) putchar('='); putchar('\n');

  /* find the first/last non-empty bin */
  for (i=n_bins-1; i>=0 && bins[i] == 0; i--);
  first_non_empty_bin = i;
  for (i=0; i<n_bins && bins[i] == 0; i++);
  last_non_empty_bin = i;

  /* estimate the mean and standard deviation of the histogram */
  mean = 0;
  count = 0;
  for (i=last_non_empty_bin; i <= first_non_empty_bin; i++) {
    x = min + (i * bin_size);			/* X axis value */
    mean += bins[i] * x;
    count += bins[i];
  }
  mean /= count;
  std = 0;
  for (i=last_non_empty_bin; i <= first_non_empty_bin; i++) {
    double diff;
    x = min + (i * bin_size);			/* X axis value */
    diff = mean - x;
    std += bins[i] * diff * diff;
  }
  std /= count - 1;
  std = sqrt(std);
    
  /* plot the histogram */
  for (i=first_non_empty_bin; i>=last_non_empty_bin; i--) {	/* plot bins */
    j = (int) (bins[i]/tick_size);		/* number of ticks in bin */
    /* print start of bin */
    x = min + (i * bin_size);			/* X axis value */
    xp = min + ((i+1) * bin_size);		/* previous X axis value */

    /* print levels above 1 sigma above mean */
    sigma1 = (int) ((int) (xp - mean) / std);
    sigma2 = (int) ((int) (x - mean) / std);
    if (sigma1 > 0) {
      if (sigma1 != sigma2) { 
        printf("----------------------------------------- %2d sigma ", sigma1);
        printf("-----------------------------\n");
      }
    }

    printf("%8.*f (%9d)|", x_prec, x, bins[i]);
    for (k=0; k<j; k++) {
      if (k >= n_ticks) {
        putchar('~');
        break;
      } else {
        putchar(bar_symbol);	/* print horizontal bar */
      }
    }
    putchar('\n');
  }

  /* finish the histogram */

  /* print y-axis labels "+	+ 	+ ... " */
  for (i=0; i<80; i++) putchar('='); putchar('\n');
  printf("%19.19s ", "");
  for (i=0; i<=n_ticks; i+=10) printf("+         ");
  putchar('\n');

  /* print y-axis labels "0	10	20 ... " */
  printf("%8.8s (%9.9s)", "center", "count  ");
  for (i=0; i<=n_ticks; i+=10) printf("%-10d", j= (int)(i*tick_size));
  putchar('\n');
  for (i=0; i<80; i++) putchar('='); putchar('\n');
  putchar('\n');

  /* return mean and standard deviation */
  *avg = mean;
  *sd = std;
}

/**********************************************************************/
/*
	finish_blastogram

	Print out the histogram prepared with init_ and fill_histogram
	in blast format.
	The labels of the bins are the bottoms (minima) of the bins. 
	Returns the sample mean and standard deviation of the data.

*/
/**********************************************************************/
extern void finish_blastogram(
  double min,		/* minimum value on X-axis; ignored if array */
  int n_bins,		/* number of bins (X-axis, plotted vertically) */
  int *bins,		/* bins of histogram */
  double bin_size,	/* size of a bin */
  int n_ticks,		/* number of ticks (Y-axis, plotted horizontally) */
  char bar_symbol,	/* symbol to build bars from */
  double *avg,		/* mean of data */
  double *sd		/* standard deviation of data */
)
{
  int i, j, k, x_prec, sigma1, sigma2;
  double x, xp;
  int tick_size; 		/* size of each division on y-axis */
  int first_non_empty_bin;
  int last_non_empty_bin;
  double mean, std, count, max;
  int *cum = (int *) malloc((n_bins+1) * sizeof(int));

  /* number of digits after the decimal on x-axis labels */
  x_prec = (int) ceil(-1 * log10(bin_size));

  /* find the first/last non-empty bin */
  for (i=n_bins-1; i>=0 && bins[i] == 0; i--);
  first_non_empty_bin = i;
  for (i=0; i<n_bins && bins[i] == 0; i++);
  last_non_empty_bin = i;

  /* estimate the mean and standard deviation of the histogram */
  mean = 0;
  count = 0;
  max = 0;
  cum[first_non_empty_bin+1] = 0;
  for (i=first_non_empty_bin; i >= last_non_empty_bin; i--) {
    x = min + (i * bin_size);			/* X axis value */
    mean += bins[i] * x;
    count += bins[i];
    max = MAX(max, bins[i]);
    cum[i] = cum[i+1] + bins[i];
  }
  mean /= count;
  std = 0;
  for (i=last_non_empty_bin; i <= first_non_empty_bin; i++) {
    double diff;
    x = min + (i * bin_size);			/* X axis value */
    diff = mean - x;
    std += bins[i] * diff * diff;
  }
  std /= count - 1;
  std = sqrt(std);
  tick_size = (int) ceil(max/n_ticks);			/* size of a tick */
  printf("Histogram units:   = %d sequences   : fewer than %d sequences\n\n",
    tick_size, tick_size);
    
  /* print y-axis labels */
  printf("%6.6s %7.7s %6.6s\n", "min", "cumul", "count");

  /* plot the histogram */
  for (i=last_non_empty_bin; i<=first_non_empty_bin; i++) {	/* plot bins */

    /* print levels above above mean */
    xp = min + ((i-1) * bin_size) - bin_size/2.0;	/* top previous bin */

    sigma1 = (int) ((int) (xp - mean) / std);
    x = min + (i * bin_size) - bin_size/2.0;		/* bottom of bin */
    sigma2 = (int) ((int) (x - mean) / std);

    /* print  center, cumulative total, and count in bin */
    printf("%6.*f %7d %6d|", x_prec, x, cum[i], bins[i]);

    /* print bar for bin */
    j = (int) (bins[i]/tick_size);		/* number of ticks in bin */
    if (j == 0 && bins[i] > 0) {
      putchar(':');
    } else {
      for (k=0; k<j; k++) putchar(bar_symbol);	/* print horizontal bar */
    }
    putchar('\n');
  }

  /* return mean and standard deviation */
  *avg = mean;
  *sd = std;
}

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/histogram.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
