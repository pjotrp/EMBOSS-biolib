/***********************************************************************
*                                                                      *
*       MEME++                                                         *
*       Copyright 1994, The Regents of the University of California    *
*       Author: Timothy L. Bailey                                      *
*                                                                      *
***********************************************************************/
/* split.c */

#include "split.h"
#include "meme.h"
/**********************************************************************/
/*
        split 

	Print a file in stripes columns wide to standard output.
*/
/**********************************************************************/
extern FILE *split(
  int mode,		/* 0 - open; 1 - print */
  FILE *infile,
  int n
)
{
  int i, c;
  char s[MAXLINE+1];
  int page_start = 0;			/* start char of current page */
  int max_line = 1;

  if (mode == 0) {
    return tmpfile();
  } else {
    while (max_line > page_start) {
      rewind(infile);			/* rewind the file */
      c = 0;
      while (c != EOF) { 
	/* get the next line */
	for (i=0; (c = fgetc(infile)) != '\n' && c != EOF; i++) {
          if (i >= MAXLINE) break; 	/* line too long; truncate */
	  s[i] = c;
	}
        /* print the current swath of the line */
	if (i==0 && c == EOF) break;	/* no line and EOF */
        s[i] = '\0';			/* make a string for printf */	
	if (i > page_start) {
	 ajFmtPrintF(outf,"%-.*s\n", n, s+page_start);
	} else {
	  ajFmtPrintF(outf,"\n");
	}
	max_line = MAX(i, max_line);
      }
      page_start += n;
    }
    fclose(infile);
  }
  return NULL;
}
/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/split.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
