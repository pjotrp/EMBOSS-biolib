/*****************************************************************
 * HMMER - Biological sequence analysis with profile HMMs
 * Copyright (C) 1992-1998 Washington University School of Medicine
 * All Rights Reserved
 * 
 *     This source code is distributed under the terms of the
 *     GNU General Public License. See the files COPYING and LICENSE
 *     for details.
 *****************************************************************/

/* main.c, Fri Dec 25 14:22:17 1992, SRE
 * 
 * getseq -- a program to extract subsequences from a sequence database 
 * RCS $Id: getseq_main.c,v 1.1 2001/07/29 14:13:49 ajb Exp $
 */

#include <stdio.h>
#include <string.h>
#include "squid.h"

#ifdef MEMDEBUG
#include "dbmalloc.h"
#endif

struct opt_s OPTIONS[] = {
  { "-a", TRUE, sqdARG_NONE   },
  { "-d", TRUE, sqdARG_STRING },
  { "-f", TRUE, sqdARG_INT    },
  { "-h", TRUE, sqdARG_NONE   },
  { "-o", TRUE, sqdARG_STRING },
  { "-r", TRUE, sqdARG_STRING },
  { "-t", TRUE, sqdARG_INT    },
  { "-D", TRUE, sqdARG_STRING },
  { "-F", TRUE, sqdARG_STRING },
};
#define NOPTIONS (sizeof(OPTIONS) / sizeof(struct opt_s))

static char usage[] = "\
Usage: getseq [-options] <seqname>\n\
   Get a sequence from a database.\n\
   Available options:\n\
   -a            : name is an accession number, not a key\n\
   -d <seqfile>  : get sequence from <seqfile>\n\
   -D <database> : instead, get sequence from main database\n\
   -h            : help; print version and usage info\n\
   -r <newname>  : rename the fragment <newname>\n\
   -f <from>     : from which residue (1..N)\n\
   -t <to>       : to which residue (1..N)\n\
   -o <outfile>  : direct output to <outfile>\n\
   -F <format>   : use output format of <format>; see below for\n\
                   list. Default is original format of database.\n\
\n\
  Available output formats are:\n\
    embl\n\
    fasta\n\
    genbank\n\
    gcg\n\
    strider\n\
    zuker\n\
    ig\n\
    pir\n\
    squid\n\
    raw\n\n\
  Available databases are: (if $env variables are set correctly)\n\
    -Dsw      $SWDIR   SwissProt\n\
    -Dpir     $PIRDIR  PIR\n\
    -Dem      $EMBLDIR EMBL\n\
    -Dgb      $GBDIR   GenBank\n\
    -Dwp      $WORMDIR WormPep\n\
    -Dowl     $OWLDIR  OWL\n";

struct seqfmt_s {  char *formatname; int fmt; } seqfmt[] =
{
  { "embl",    kEMBL    },
  { "fasta",   kPearson },
  { "genbank", kGenBank },
  { "gcg",     kGCG     },
  { "strider", kStrider },
  { "zuker",   kZuker   },
  { "ig",      kIG      },
  { "pir",     kPIR     },
  { "squid",   kSquid   },
  { "raw",     kRaw     },
};
#define NUMFORMATS  (sizeof(seqfmt) / sizeof(struct seqfmt_s))

/* dbenv maps command line database selection to an environment
 * variable, from which the database directory is obtained.
 */
struct dbenv_s { 
  char *dbname;		/* name of database, as used on command line */
  char *gsiname;        /* name of GSI index file to look for        */
  char *envname;        /* environment var to get directory path from*/
  char *entryend;       /* string signifying end of entry            */
  int   addend;		/* TRUE if entryend line is part of entry    */
} dbenv[] =
{
  { "sw",  "swiss.gsi",  "SWDIR",   "//",  TRUE},
  { "pir", "pir.gsi",    "PIRDIR",  "///", TRUE},
  { "em",  "embl.gsi",   "EMBLDIR", "//",  TRUE},
  { "gb",  "genbank.gsi","GBDIR",   "//",  TRUE},
  { "wp",  "wormpep.gsi","WORMDIR", ">",   FALSE},  
  { "owl", "owl.gsi",    "OWLDIR",  ">",   FALSE}, /* use FASTA OWL version */
};
#define NUMDBS  (sizeof(dbenv) / sizeof(struct dbenv_s))

int
main(int argc, char **argv)
{
  char  *dbname;                /* master database to search */
  char  *seqfile;               /* name of sequence file to read */
  char  *gsifile;		/* name of GSI index file (if one exists) */
  SQFILE *seqfp;                /* pointer to open sequence file */
  char  *getname;               /* name of sequence to get from */
  int    from;			/* starting residue, 1..N */
  int    to;			/* ending residue, 1..N   */
  char  *outfile;               /* name of file to put output to */
  FILE  *outfp;                 /* file pointer to put output to */
  int    format;		/* format of seqfile */
  int    outfmt;		/* output format     */
  char  *seq;                   /* current working sequence */
  SQINFO sqinfo;
  char  *frag;                  /* extracted subsequence */
  int    source_start;		/* start of seq on original source 1..N */
  int    source_stop;           /* end of seq on original source 1..N   */
  int    source_orient;		/* sign of parent: -1 revcomp, +1 normal*/
  int    i;
  char  *ss;			/* secondary structure representation */
  GSIFILE  *gsi;                /* open GSI index file */
  long   gsi_offset;		/* disk offset for locating sequence */
  
  char *rename;	        	/* new name to give fragment */
  int   reverse_complement;	/* do we have to reverse complement? */
  int   getall;
  char *outformat;		/* output format string */
  int   by_accession;		/* TRUE if name is accession number not key */
  int   used_gsi;		/* TRUE if GSI file was used (don't scan) */
  int   dbidx;

  char *optname;
  char *optarg;
  int   optind;

#ifdef MEMDEBUG
  unsigned long histid1, histid2, orig_size, current_size;
  orig_size = malloc_size(&histid1);
#endif


  /***********************************************
   * Parse the command line
   ***********************************************/

				/* initializations and defaults */
  reverse_complement = 0;
  getall  = TRUE;
  dbname  = NULL;
  dbidx   = -1;
  seqfile = NULL;
  from    = -1;
  to      = -1;			/* flag that says do the whole thing */
  outfile = NULL;
  getname = NULL;
  rename  = NULL;
  outformat = NULL;
  by_accession = FALSE;
  used_gsi     = FALSE;

  while (Getopt(argc, argv, OPTIONS, NOPTIONS, usage,
                &optind, &optname, &optarg))
    {
      if      (strcmp(optname, "-a") == 0) { by_accession = TRUE;   }
      else if (strcmp(optname, "-d") == 0) { seqfile      = optarg; }
      else if (strcmp(optname, "-f") == 0) { 
	from = atoi(optarg); getall = FALSE;
      }
      else if (strcmp(optname, "-t") == 0) {
	to = atoi(optarg); getall = FALSE;
      }
      else if (strcmp(optname, "-r") == 0) { rename    = optarg; }
      else if (strcmp(optname, "-o") == 0) { outfile   = optarg; }
      else if (strcmp(optname, "-D") == 0) { dbname    = optarg; }
      else if (strcmp(optname, "-F") == 0) { outformat = optarg; }
      else if (strcmp(optname, "-h") == 0) {
	printf("getseq %s, %s\n%s\n", squid_version, squid_date, usage);
	exit(EXIT_SUCCESS);
      }
    }

  if (argc - optind != 1) Die("%s\n", usage); 
  getname = argv[optind];

  /***********************************************
   * Get name of file to look through, and disk offset,
   * using GSI file if one exists. Three possibilities:
   *    1) Look in main DB, which has GSI index in the directory
   *    2) Look in a file, which has associated GSI index
   *    3) Look in an unindexed file
   ***********************************************/

  gsi_offset = 0;
  if (dbname != NULL && seqfile != NULL)
    Die("Can't get seq from *both* a database %s and a file %s\n%s", 
	dbname, seqfile, usage);
  if (dbname == NULL && seqfile == NULL)
    {				/* try to guess SwissProt */
      if (strchr(getname, '_') != NULL) dbname = Strdup("sw");
      else 
	Die("You have to specify either a database or a seqfile\n%s", usage);
    }

  if (dbname != NULL)		/* Main database. GSI index mandatory. */
    {
      char *dbdir;
      char *dbfile;
				/* find which db this is */
      for (dbidx = 0; dbidx < NUMDBS; dbidx++)
	if (strcmp(dbenv[dbidx].dbname, dbname) == 0)
	  break;
      if (dbidx == NUMDBS)
	Die("No such main database %s\n%s", dbname, usage);
      
				/* get directory name */
      if ((dbdir = getenv(dbenv[dbidx].envname)) == NULL)
	Die("Environment variable %s is not set.\n%s",
	    dbenv[dbidx].envname, usage);
				/* open gsi file */
      gsifile = (char *) MallocOrDie
	((strlen(dbdir) + strlen(dbenv[dbidx].gsiname) + 2) * sizeof(char));
      sprintf(gsifile, "%s/%s", dbdir, dbenv[dbidx].gsiname);
      if ((gsi = GSIOpen(gsifile)) == NULL)
	Die("Failed to open GSI index file %s in directory %s\n%s",
	    dbenv[dbidx].gsiname, dbdir, usage);
				/* get seqfile name and offset */
      dbfile = (char *) MallocOrDie (GSI_KEYSIZE);
      if (! GSIGetOffset(gsi, getname, dbfile, &format, &gsi_offset))
	Die("Failed to find key %s in GSI file %s", getname, gsifile);
      free(gsifile);
      GSIClose(gsi);
				/* set up proper seqfile, with path */
      seqfile = (char *) MallocOrDie 
	((strlen(dbdir) + strlen(dbfile) + 2) * sizeof(char));
      sprintf(seqfile, "%s/%s", dbdir, dbfile);
      free(dbfile);
      used_gsi = TRUE;
    }
  else				/* Sequence file. GSI index optional. */
    {
      char  newseqfile[GSI_KEYSIZE];

      gsifile = (char *) MallocOrDie ((strlen(seqfile) + 5) * sizeof(char));
      sprintf(gsifile, "%s.gsi", seqfile);
      if ((gsi = GSIOpen(gsifile)) != NULL)
	{
	  if (! GSIGetOffset(gsi, getname, newseqfile, &format, &gsi_offset))
	    Die("Failed to find key %s in GSI file %s", getname, gsifile);
	  GSIClose(gsi);
	  used_gsi = TRUE;
	}
      free(gsifile);
    }
  
  /***********************************************
   * Open database file
   ***********************************************/

  /* If we used a GSI index, we already know the file format.
   */
  if (! used_gsi && ! SeqfileFormat(seqfile, &format, NULL))
    {
      switch (squid_errno) {
      case SQERR_NOFILE: 
	Die("Sequence file %s could not be opened for reading", seqfile);
      /*FALLTHRU*/
      case SQERR_FORMAT: 
      default:           
	Die("Failed to determine format of sequence file %s", seqfile);
      }
    }
  if ((seqfp = SeqfileOpen(seqfile, format, NULL)) == NULL)
    Die("Failed to open sequence database file %s\n%s\n", seqfile, usage);

  SeqfilePosition(seqfp, gsi_offset);

  /***********************************************
   * Open output file
   ***********************************************/
  
  outfmt = format;
  if (outformat != NULL)
    {
      for (i = 0; i < NUMFORMATS; i++)
	if (strcasecmp(outformat, seqfmt[i].formatname) == 0)
	  { outfmt = seqfmt[i].fmt; break; }
      if (outfmt == kUnknown)
	Die("Unknown output format %s\n%s", outformat, usage);
    }

				/* open output file for writing;
				   use stdout by default */
  if (outfile == NULL)
    outfp = stdout;
  else if ((outfp = fopen(outfile, "w")) == NULL)
    Die("cannot open %s for output\n", outfile);


  /***********************************************
   * Main loop
   ***********************************************/

  /* If this is a simple fetch of the complete sequence
   * in native format, and we've been positioned in the file
   * by a GSI index file, we can just read right from the file.
   */
  if (getall && used_gsi && outfmt == format && dbname != NULL)
    {
      char buffer[256];
      int  endlen;

      if (dbidx == -1) Die("That's weird. No database index available.");
      endlen = strlen(dbenv[dbidx].entryend);
      fputs(seqfp->sbuffer, outfp); /* always do first line */
      fputs("\n", outfp);
      while (fgets(buffer, 256, seqfp->f) != NULL)
	{
	  if (strncmp(buffer, dbenv[dbidx].entryend, endlen) == 0)
	    {
	      if (dbenv[dbidx].addend) fputs(buffer, outfp);
	      break;
	    }
	  fputs(buffer, outfp);
	}
    }
  else				/* else, the hard way with ReadSeq */
    {
      seq    = NULL;
      frag   = NULL;
      while (ReadSeq(seqfp, format, &seq, &sqinfo))
	{
	  if (used_gsi)		/* GSI file puts us right on our seq. */
	    break;
	  else if (by_accession && 
		   (sqinfo.flags & SQINFO_ACC) &&
		   strcmp(sqinfo.acc, getname) == 0)
	    break;
	  else if (strcmp(sqinfo.name, getname) == 0)
	    break;

	  FreeSequence(seq, &sqinfo);
	  seq = NULL;
	}
      
      if (seq == NULL) 
	Die("failed to extract the subsequence %s\n%s", getname, usage);
      
      if (getall)
	{
	  from = 1;
	  to   = sqinfo.len;
	}
      else if (from == -1) from = 1;
      else if (to   == -1) to   = sqinfo.len;
      
      if (to > sqinfo.len || from > sqinfo.len)
	Warn("Extracting beyond the length of the sequence");
      if (to < 1 || from < 1)
	Warn("Extracting beyond the beginning of the sequence");
      
      /* check for reverse complement */
      if (to != -1 && from > to)
	{
	  int   swapfoo;	/* temp variable for swapping coords */

	  reverse_complement   = TRUE;
	  swapfoo = from; from = to; to = swapfoo;
	}
      if (to > sqinfo.len) to = sqinfo.len;
      if (from < 1)      from = 1;
      
      if ((frag = (char *) calloc (to-from+2, sizeof(char))) == NULL)
	Die("memory error\n");
      
      if (strncpy(frag, seq+from-1, to-from+1) == NULL)
	Die("strncpy() failed\n"); 
      
      if (sqinfo.flags & SQINFO_SS)
	{
	  if ((ss = (char	*) calloc (to-from+2, sizeof(char))) == NULL)
	    Die("memory error\n");
	  if (strncpy(ss, sqinfo.ss+from-1, to-from+1) == NULL)
	    Die("strncpy() failed\n"); 
	  free(sqinfo.ss);
	  sqinfo.ss = ss;
	}
      
      if (reverse_complement)
	{
	  char			*revfrag;		/* temp variable for reverse complement */
	  int   swapfoo;	/* temp variable for swapping coords back */

	  if ((revfrag = calloc ( to-from+2, sizeof(char))) == NULL)
	    Die("memory failure\n"); 
	  revcomp(revfrag, frag);
	  free(frag);
	  frag = revfrag;
	  swapfoo = from; from = to; to = swapfoo;

	  /* reverse complement nullifies secondary structure */
	  if (sqinfo.flags & SQINFO_SS)
	    { free(sqinfo.ss); sqinfo.flags &= ~SQINFO_SS; }
	}
      
      if (! (sqinfo.flags & SQINFO_ID))
	SetSeqinfoString(&sqinfo, sqinfo.name, SQINFO_ID);
      
      if (! (sqinfo.flags & SQINFO_OLEN))
	{ sqinfo.olen = sqinfo.len; sqinfo.flags |= SQINFO_OLEN; }
      
      sqinfo.len = (to > from) ? to-from+1 : from-to+1;
      sqinfo.flags |= SQINFO_LEN;
      
      if (rename != NULL)
	SetSeqinfoString(&sqinfo, rename, SQINFO_NAME);
      
      source_start = (sqinfo.flags & SQINFO_START) ? sqinfo.start : 1;
      source_stop  = (sqinfo.flags & SQINFO_STOP)  ? sqinfo.stop  : sqinfo.len;
      source_orient= (source_stop > source_start)  ? 1 : -1;
      
      sqinfo.start = source_start + (from- 1) * source_orient;
      sqinfo.stop  = source_start + (to  - 1) * source_orient;
      sqinfo.flags |= SQINFO_START | SQINFO_STOP;
      
      WriteSeq(outfp, outfmt, frag, &sqinfo);
      free(frag);
      FreeSequence(seq, &sqinfo);
    }

  if (outfile != NULL)
    printf("Fragment written to file %s\n", outfile);

  SeqfileClose(seqfp);
  fclose(outfp);

#ifdef MEMDEBUG
  current_size = malloc_size(&histid2);
  
  if (current_size != orig_size)
    malloc_list(2, histid1, histid2);
  else
    fprintf(stderr, "[No memory leaks]\n");
#endif

  return(0);
}
