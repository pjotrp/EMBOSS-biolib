/***********************************************************************
*								       *
*	MEME++							       *
*	Copyright 1996, The Regents of the University of California    *
*	Author: Bill Grundy					       *
*								       *
***********************************************************************/
/* message.h */

#ifndef MESSAGE_H
#  define MESSAGE_H

#include <meme.h> /* We need the defn of S_POINT. */
/***************************************************************************/
/* Type declarations */
/***************************************************************************/
/* To do a reduction, we first pass small packets around containing
   the discriminating information. */
typedef struct reduction_packet {
  double data;
  double ID; /* Use a double so the MPI type handle is simple. */
} REDUCE_PACKET;

typedef struct s_point_packet {
  double score;
  double iseq; /* Use a double so the MPI type handle is simple. */
  double ioff; /* Use a double so the MPI type handle is simple. */
} S_POINT_PACKET;

/* This structure stores each processor's starting and ending points
   in the search for starting points. */
typedef struct seq_params {
  int start_seq;
  int start_off;
  int end_seq;
  int end_off;
} SEQ_PARAMS;

/***************************************************************************/
/* Global variables */
/***************************************************************************/

EXTERN SEQ_PARAMS *start_n_end;

/***************************************************************************/
/* Function prototypes */
/***************************************************************************/

extern void store_consensus(
  MODEL *model,
  CANDIDATE *candidates
);

extern void print_model(
   char *label,
   MODEL *model
);

extern void reduce_across_models(
   MODEL *model,
   int alength
);

extern void balance_loop1(
   SAMPLE **samples,
   SEQ_PARAMS *start_n_end,
   int n_samples
);

extern void reduce_across_s_points(
  S_POINT *s_points,
  SAMPLE **samples,
  int n_nsites0,
  int n_starts
);			  

#endif
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/message.h,v 1.1 2000/11/05 21:47:56 ajb Exp $ */
