/***********************************************************************
*                                                                      *
*       MEME++                                                         *
*       Copyright 1994, The Regents of the University of California    *
*       Author: Timothy L. Bailey                                      *
*                                                                      *
***********************************************************************/
/* banner.c */
/*
        Print name, version, date, reference.
*/

#include "meme.h"
#include "projrel.h"


void PSTARRS()
{
    int i;

    for(i=0;i<PAGEWIDTH;++i)
	ajFmtPrintF(outf,"*");
    ajFmtPrintF(outf,"\n");
    return;
}




extern void banner(
  char *program			/* name of program */
) 
{
  int i = strlen(ARCHIVE_DATE) - 9;

  /* announce the program */
  PSTARRS(); ajFmtPrintF(outf,"MEME - Motif discovery tool\n"); PSTARRS();
  ajFmtPrintF(outf,"MEME version %s (Release date: %*.*s)\n\n"
"For further information on how to interpret these results or to get\n"
"a copy of the MEME software please access http://www.sdsc.edu/MEME.\n"
"\n"
"This file may be used as input to the MAST algorithm for searching\n"
"sequence databases for matches to groups of motifs.  MAST is available\n"
"for interactive use and downloading at http://www.sdsc.edu/MEME.\n",
    VERSION, i, i, ARCHIVE_DATE+7);
  PSTARRS();

  /* print reference citation */
  ajFmtPrintF(outf,"\n\n");
  PSTARRS(); ajFmtPrintF(outf,"REFERENCE\n"); PSTARRS();
  ajFmtPrintF(outf,
"If you use this program in your research, please cite:\n"
"\n"
"Timothy L. Bailey and Charles Elkan,\n"
"\"Fitting a mixture model by expectation maximization to discover\n"
"motifs in biopolymers\", Proceedings of the Second International\n"
"Conference on Intelligent Systems for Molecular Biology, pp. 28-36,\n"
"AAAI Press, Menlo Park, California, 1994.\n"
  );
  PSTARRS();
}

/* $Header: /home/repository/emboss/emboss/emboss/embassy/meme/src/banner.c,v 1.1 2000/11/05 21:47:55 ajb Exp $ */
