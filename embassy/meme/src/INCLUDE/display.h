/* display.h */

#ifndef DISPLAY_H
#  define DISPLAY_H

extern void print_results(
  DATASET *dataset,		/* the dataset */
  DATASET *neg_dataset,		/* negative examples */
  MODEL *model,			/* the model */
  MODEL *neg_model, 		/* negative model */
  int max_w,			/* maximum width tried */
  P_POINT *p_point,		/* previous components starts */
  CANDIDATE *candidates 	/* candidate models found */
);
extern void print_theta(
  int format,		/* 1 = floating point
              		   2 = integer */ 
  THETA theta,		/* theta */
  int w,		/* width of motif */
  char *str_space,	/* space for printing strand direction */
  DATASET *dataset,	/* the dataset */ 
  FILE *outfile	 	/* NULL stdout; otherwise, print to split file */
);
extern void print_zij(
  DATASET *dataset,			/* the dataset */
  MODEL *model				/* the model */
);
extern void print_wij(
  DATASET *dataset		/* the dataset */
);
extern char *get_consensus(
  THETA theta,			/* motif theta */
  int w,			/* width of motif */
  DATASET *dataset,		/* the dataset */
  int N,			/* number of letters for each position */
  double min_prob		/* minimum cumulative prob for N letters */
);
void print_dataset_summary (
  char *datafile,                       /* name of the training set file */
  char *alphabet,                       /* alphabet of sequences */
  DATASET *dataset,                     /* structure containing sequences */
  char *negfile                         /* name of negative example file */
);
void print_env(
  FILE *debug_file
);

void print_meme_doc();

#endif

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/display.h,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
