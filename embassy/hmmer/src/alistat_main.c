/*****************************************************************
 * HMMER - Biological sequence analysis with profile HMMs
 * Copyright (C) 1992-1998 Washington University School of Medicine
 * All Rights Reserved
 * 
 *     This source code is distributed under the terms of the
 *     GNU General Public License. See the files COPYING and LICENSE
 *     for details.
 *****************************************************************/

/* alistat_main.c
 * Fri Jan 27 10:41:41 1995
 * RCS $Id: alistat_main.c,v 1.1 2001/07/29 14:13:48 ajb Exp $
 * 
 * Look at an alignment file, determine some simple statistics.
 */

#include <stdio.h>
#include <string.h>
#include "squid.h"

struct opt_s OPTIONS[] = {
  { "-a", TRUE, sqdARG_NONE },     /* report per-seq, not just summary */  
  { "-f", TRUE, sqdARG_NONE },     /* fast: do average id by sampling  */
  { "-h", TRUE, sqdARG_NONE },     /* help                             */
};
#define NOPTIONS (sizeof(OPTIONS) / sizeof(struct opt_s))


#ifdef MEMDEBUG
#include "dbmalloc.h"
#endif

char usage[]  = "Usage: alistat [-options] <alignment file>\n\
  Verify a multiple sequence alignment file, do some simple statistics.\n\
  Available options:\n\
  -a    : report per-sequence info, not just a summary\n\
  -f    : fast: estimate average %id by sampling (not compatible with -a)\n\
  -h    : help; display usage and version\n";

int
main(int argc, char **argv)
{
  char     *seqfile;            /* name of aligned sequence file */
  int       fmt;		/* format of seqfile             */
  char    **aseq;		/* aligned sequences             */
  AINFO     ainfo;		/* info about sequences          */
  int       nres;		/* number of residues */
  float  **imx;                /* identity matrix               */
  int       i,j;
  int       small, large;	
  int       bestj, worstj;
  float     sum, best, worst;
  float     worst_worst, worst_best, best_best;
  float     avgid;
  int       nsample;

  int    allreport;
  int    do_fast;

  char  *optname;
  char  *optarg;
  int    optind;

#ifdef MEMDEBUG
  unsigned long histid1, histid2, orig_size, current_size;
#endif

  /***********************************************
   * Parse command line
   ***********************************************/

  allreport = FALSE;
  do_fast   = FALSE;
  while (Getopt(argc, argv, OPTIONS, NOPTIONS, usage, 
		&optind, &optname, &optarg))
    {
      if      (strcmp(optname, "-a") == 0) { allreport = TRUE; }
      else if (strcmp(optname, "-f") == 0) { do_fast   = TRUE; }
      else if (strcmp(optname, "-h") == 0) {
        printf("alistat %s, %s\n%s\n", squid_version, squid_date, usage);
        exit(EXIT_SUCCESS);
      }
    }

  if (argc - optind != 1) Die("%s\n", usage);

  seqfile = argv[optind];

#ifdef MEMDEBUG
  orig_size = malloc_size(&histid1);
#endif

  if (do_fast && allreport)
    Die("Verbose report (-a) is incompatible with fast sampling (-f)");

  /***********************************************
   * Read the file.
   ***********************************************/

  printf("alistat %s, %s\n\n", squid_version, squid_date);

  if (! SeqfileFormat(seqfile, &fmt, NULL))
    switch (squid_errno) {
    case SQERR_NOFILE: 
      Die("Alignment file %s could not be opened for reading", seqfile);
    /*FALLTHRU*/
    case SQERR_FORMAT: 
    default:           
      Die("Failed to determine format of sequence file %s", seqfile);
    }
  if (! ReadAlignment(seqfile, fmt, &aseq, &ainfo))
    Die("Failed to read aligned sequence file %s", seqfile);
  for (i = 0; i < ainfo.nseq; i++) s2upper(aseq[i]);
  

  /* Statistics we always collect
   */
  nres = 0;
  small = large = -1;
  for (i = 0; i < ainfo.nseq; i++)
    {
      nres += ainfo.sqinfo[i].len;
      if (small == -1 || ainfo.sqinfo[i].len < small) 
	small = ainfo.sqinfo[i].len;
      if (large == -1 || ainfo.sqinfo[i].len > large) 
	large = ainfo.sqinfo[i].len;
    }

  /* Statistics we have to be careful about
   * collecting, because of time constraints on NxN operations
   */
  if (do_fast)
    {
      nsample = 1000;
      avgid = AlignmentIdentityBySampling(aseq, ainfo.alen, ainfo.nseq, nsample);
    }
  else
    {
      /* In a full report, for each sequence, find the best relative, and the worst relative.
       * For overall statistics, save the worst best (most distant single seq)
       * and the best best (most closely related pair)
       * and the worst worst (most distantly related pair)
       * and yes, I know it's confusing.
       */

      MakeIdentityMx(aseq, ainfo.nseq, &imx);
      if (allreport) {
	printf("  %-15s %5s %7s %-15s %7s %-15s\n",
	       "NAME", "LEN", "HIGH ID", "(TO)", "LOW ID", "(TO)");
	printf("  --------------- ----- ------- --------------- ------- ---------------\n");
      }

      sum = 0.0;
      worst_best  = 1.0;
      best_best   = 0.0;
      worst_worst = 1.0;
      for (i = 0; i < ainfo.nseq; i++)
	{
	  worst = 1.0;
	  best  = 0.0;
	  for (j = 0; j < ainfo.nseq; j++)
	    {			/* closest seq to this one = best */
	      if (i != j && imx[i][j] > best) { best  = imx[i][j]; bestj = j; }
	      if (imx[i][j] < worst)          { worst = imx[i][j]; worstj = j; }
	    }

	  if (allreport) 
	    printf("* %-15s %5d %7.1f %-15s %7.1f %-15s\n",
		   ainfo.sqinfo[i].name, ainfo.sqinfo[i].len,
		   best * 100.,  ainfo.sqinfo[bestj].name,
		   worst * 100., ainfo.sqinfo[worstj].name);
	  
	  if (best > best_best)    best_best = best;
	  if (best < worst_best)   worst_best = best;
	  if (worst < worst_worst) worst_worst = worst;
	  for (j = 0; j < i; j++)
	    sum += imx[i][j];

	}
      avgid = sum / (float) (ainfo.nseq * (ainfo.nseq-1)/2.0);
      if (allreport) puts("");
      FMX2Free(imx);
    }

  /* Print output. 
   * Some fields aren't available if -f (fast) was chosen.
   */
  printf("Format:              %s\n",     SeqFormatString(fmt));
  printf("Number of sequences: %d\n",     ainfo.nseq);
  printf("Total # residues:    %d\n",     nres);
  printf("Smallest:            %d\n",     small);
  printf("Largest:             %d\n",     large);
  printf("Average length:      %.1f\n",   (float) nres / (float) ainfo.nseq);
  printf("Alignment length:    %d\n",     ainfo.alen);
  printf("Average identity:    %.0f%%\n", 100.*avgid);
  if (! do_fast) {
    printf("Most related pair:   %.0f%%\n", 100.*best_best);
    printf("Most unrelated pair: %.0f%%\n", 100.*worst_worst);
    printf("Most distant seq:    %.0f%%\n", 100.*worst_best);
  }
  puts("");  

  FreeAlignment(aseq, &ainfo);

#ifdef MEMDEBUG
  current_size = malloc_size(&histid2);
  
  if (current_size != orig_size)
    malloc_list(2, histid1, histid2);
  else
    fprintf(stderr, "[No memory leaks]\n");
#endif
    
  return 0;
}
