/* read_data_file.h */

#ifndef READ_DATA_FILE_H
#define READ_DATA_FILE_H

extern DATASET *read_seq_file(
  AjPSeqall seqa,		/* name of file to open */
  char *alpha,			/* alphabet used in sequences */
  BOOLEAN use_comp,		/* use complementary strands, too */
  double seqfrac		/* fraction of input sequences to use */
);

extern BOOLEAN read_sequence(
  AjPSeqall seqa,               /* file containing sequences */
  char **sample_name,           /* unique identifier of sequence */
  char **sample_de,             /* descriptor of sequence */
  char **sequence,              /* protein or DNA letters */
  int *length                   /* length of sequence */
);

#endif
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/read_seq_file.h,v 1.1 2000/11/05 21:47:56 ajb Exp $ */
