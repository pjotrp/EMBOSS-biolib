/* motifs.h */

#ifndef MOTIFS_H
#  define MOTIFS_H

extern int read_motifs (
  FILE *fdata,                          /* opened dataset file */
  char *filename,                       /* motif file */
  MOTIF motif[NMOTIFS],                 /* motif info */
  BOOLEAN save_dataset,                 /* return dataset in memory */
  DATASET *dataset                      /* integer-encoded dataset */
);

#endif
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/motifs.h,v 1.1 2000/11/05 21:47:56 ajb Exp $ */
