/*****************************************************************
 * HMMER - Biological sequence analysis with profile HMMs
 * Copyright (C) 1992-1998 Washington University School of Medicine
 * All Rights Reserved
 * 
 *     This source code is distributed under the terms of the
 *     GNU General Public License. See the files COPYING and LICENSE
 *     for details.
 *****************************************************************/

/* sreformat_main.c
 * Mon Sep 13 13:06:51 1993
 * 
 * sreformat - reformat sequence files.
 * renamed sreformat from reformat, Tue Jun 30 10:53:38 1998
 *
 * RCS $Id: sreformat_main.c,v 1.1 2001/07/29 14:13:49 ajb Exp $
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "squid.h"

#ifdef MEMDEBUG
#include "dbmalloc.h"
#endif

static char banner[] = "sreformat - convert between sequence formats";

static char usage[] = "\
Usage: sreformat [-options] <format> <seqfile>\n\
  Output format choices: Unaligned      Aligned\n\
                         -----------    -------\n\
                         fasta          selex\n\
                         embl           msf\n\
                         genbank        a2m\n\
                         gcg\n\
                         gcgdata\n\
                         strider\n\
                         zuker\n\
                         ig\n\
                         pir\n\
                         squid\n\
                         raw\n\n\
  Available options are:\n\
    -h : help; print brief help on version and usage\n\
    -d : force DNA alphabet for nucleic acid sequence\n\
    -r : force RNA alphabet for nucleic acid sequence\n\
    -l : force lower case\n\
    -u : force upper case\n\
";

static char experts[] = "\
  Expert options:\n\
    --pfam        : do SELEX output in PFAM style (1 line/seq)\n\
    --sam         : try to convert gaps to SAM style\n\
    --samfrac <x> : convert to SAM convention; cols w/ gapfrac > x are inserts\n\
";

static struct opt_s OPTIONS[] = {
  { "-d", TRUE, sqdARG_NONE },
  { "-h", TRUE, sqdARG_NONE },
  { "-l", TRUE, sqdARG_NONE },
  { "-r", TRUE, sqdARG_NONE },
  { "-u", TRUE, sqdARG_NONE },
  { "--pfam",    FALSE, sqdARG_NONE },
  { "--sam",     FALSE, sqdARG_NONE },
  { "--samfrac", FALSE, sqdARG_FLOAT },
};
#define NOPTIONS (sizeof(OPTIONS) / sizeof(struct opt_s))

struct seqfmt_s {  char *formatname; int fmt; } seqfmt[] =
{
  { "a2m",     kA2M     },
  { "embl",    kEMBL    },
  { "fasta",   kPearson },
  { "genbank", kGenBank },
  { "gcg",     kGCG     },
  { "gcgdata", kGCGdata },
  { "msf",     kMSF     },
  { "strider", kStrider },
  { "zuker",   kZuker   },
  { "ig",      kIG      },
  { "pir",     kPIR     },
  { "selex",   kSelex   },
  { "squid",   kSquid   },
  { "raw",     kRaw     },
};
#define NUMFORMATS  (sizeof(seqfmt) / sizeof(struct seqfmt_s))


int
main(int argc, char **argv)
{
  char     *seqfile;            /* name of sequence file */
  char     *format;
  SQFILE   *dbfp;		/* open sequence file */
  int       fmt;		/* format of seqfile  */
  int       outfmt;		/* output format */
  char     *seq;		/* sequence */
  SQINFO    sqinfo;
  int       i;

  int    force_rna;		/* TRUE to force RNA alphabet */
  int    force_dna;		/* TRUE to force DNA alphabet */
  int    force_lower;		/* TRUE to force lower case   */
  int    force_upper;		/* TRUE to force upper case   */
  int    do_pfam;		/* TRUE to make SELEX -> PFAM */
  int    samize;		/* TRUE to SAMize an A2M conversion */
  float  samfrac;		/* -1, or gap fraction for a SAM conversion */

  char *optname;                /* name of option found by Getopt()      */
  char *optarg;                 /* argument found by Getopt()            */
  int   optind;                 /* index in argv[]                       */

#ifdef MEMDEBUG
  unsigned long histid1, histid2;
  size_t orig_size, current_size;
  orig_size = malloc_inuse(&histid1);
  fprintf(stderr, "[... memory debugging is ON ...]\n");
#endif


  /***********************************************
   * Parse command line
   ***********************************************/

  force_rna   = FALSE;
  force_dna   = FALSE;
  force_upper = FALSE;
  force_lower = FALSE;
  do_pfam     = FALSE;   
  samize      = FALSE;
  samfrac     = -1.0;
  fmt         = kUnknown;

  while (Getopt(argc, argv, OPTIONS, NOPTIONS, usage,
                &optind, &optname, &optarg))  {
    if      (strcmp(optname, "-d")        == 0) force_dna   = TRUE;
    else if (strcmp(optname, "-l")        == 0) force_lower = TRUE;
    else if (strcmp(optname, "-r")        == 0) force_rna   = TRUE;
    else if (strcmp(optname, "-u")        == 0) force_upper = TRUE;
    else if (strcmp(optname, "--pfam")    == 0) do_pfam     = TRUE;
    else if (strcmp(optname, "--sam")     == 0) samize      = TRUE;
    else if (strcmp(optname, "--samfrac") == 0) samfrac     = atof(optarg);
    else if (strcmp(optname, "-h") == 0) {
      SqdBanner(stdout, banner);
      puts(usage);
      puts(experts);
      exit(0);
    }
  }

  if (argc - optind != 2)
    Die("%s\n", usage); 
  if (force_lower && force_upper)
    Die("Can't force both upper case and lower case. Stop trying to confuse me.\n%s", 
	usage);
  if (force_rna && force_dna)
    Die("Can't force both RNA and DNA. Stop trying to find bugs, you'll be sorry.\n%s", 
	usage);

  format  = argv[optind]; optind++;
  seqfile = argv[optind]; optind++;
  
  /***********************************************
   * Figure out what format we're supposed to write
   ***********************************************/

  outfmt = kUnknown;
  for (i = 0; i < NUMFORMATS; i++)
    if (strcasecmp(format, seqfmt[i].formatname) == 0)
      outfmt = seqfmt[i].fmt;
  
  if (outfmt == kUnknown)
    Die("Unknown output format %s\n%s", format, usage);

  /***********************************************
   * Reformat the file, printing to stdout.
   ***********************************************/

  if (fmt == kUnknown && ! SeqfileFormat(seqfile, &fmt, NULL))
    Die("Can't determine format of file %s\n", seqfile);

				/* Alignment to alignment conversion */
  if ((fmt == kMSF || fmt == kSelex || fmt == kClustal || fmt == kPearson) &&
      (outfmt == kMSF || outfmt == kSelex || outfmt == kA2M))
    {
      char **aseqs;
      AINFO  ainfo;

      ReadAlignment(seqfile, fmt, &aseqs, &ainfo);

      /* If asked, convert upper/lower convention and
       * gap character conventions now
       */
      if (samize)       SAMizeAlignment(aseqs, ainfo.nseq, ainfo.alen);
      if (samfrac >= 0) SAMizeAlignmentByGapFrac(aseqs, ainfo.nseq, ainfo.alen, samfrac);

      for (i = 0; i < ainfo.nseq; i++)
	{
	  if (force_dna)   ToDNA(aseqs[i]);
	  if (force_rna)   ToRNA(aseqs[i]);
	  if (force_lower) s2lower(aseqs[i]);
	  if (force_upper) s2upper(aseqs[i]);
	}
      
      switch (outfmt) {
      case kA2M:   WriteAlignedFASTA(stdout, aseqs, &ainfo); break;
      case kMSF:   WriteMSF(stdout, aseqs, &ainfo);          break;
      case kSelex: 
	if (do_pfam)
	  WriteSELEX(stdout, aseqs, &ainfo, ainfo.alen+1);
	else
	  WriteSELEX(stdout, aseqs, &ainfo, 50);
	break;
      }
      FreeAlignment(aseqs, &ainfo);
    }
  else if (outfmt == kMSF || outfmt == kSelex || outfmt == kA2M)
    {
      Die("Sorry, you can't make alignment files from unaligned files");
    }
  else
    {
      if ((dbfp = SeqfileOpen(seqfile, fmt, NULL)) == NULL)
	Die("Failed to open sequence file %s for reading", seqfile);
  
      while (ReadSeq(dbfp, fmt, &seq, &sqinfo))
	{
	  if (force_dna)   ToDNA(seq);
	  if (force_rna)   ToRNA(seq);
	  if (force_lower) s2lower(seq);
	  if (force_upper) s2upper(seq);

	  WriteSeq(stdout, outfmt, seq, &sqinfo);
	  FreeSequence(seq, &sqinfo);
	}
      SeqfileClose(dbfp);
    }

#ifdef MEMDEBUG
  malloc_chain_check(1);
  current_size = malloc_size(&histid2);
  if (current_size != orig_size) malloc_list(2, histid1, histid2);
  else fprintf(stderr, "[No memory leaks.]\n");
#endif
  return 0;
}
