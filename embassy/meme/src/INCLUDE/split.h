/* split.h */

#ifndef SPLIT_H
#  define SPLIT_H 

#include <macros.h>

/* longest line allowd in in file */
#define MAXLINE 10000

extern FILE *split(
  int nmode,		/* 0 - open; 1 - print */
  FILE *infile,
  int n
);

#endif
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/INCLUDE/split.h,v 1.1 2000/11/05 21:47:56 ajb Exp $ */
