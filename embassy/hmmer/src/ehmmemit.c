/************************************************************
 * HMMER - Biological sequence analysis with profile HMMs
 * Copyright (C) 1992-1998 Washington University School of Medicine
 * All Rights Reserved
 * 
 *     This source code is distributed under the terms of the
 *     GNU General Public License. See the files COPYING and LICENSE
 *     for details.
 ************************************************************/

/* hmmemit.c
 * SRE, Sun Mar  8 14:11:24 1998 [St. Louis]
 * 
 * main() for generating sequences from an HMM
 * RCS $Id: ehmmemit.c,v 1.2 2004/06/14 14:43:30 rice Exp $
 *
 * Modified for EMBOSS by Alan Bleasby (ISMB 2001)
 */

#include "ajax.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "structs.h"		/* data structures, macros, #define's   */
#include "config.h"		/* compile-time configuration constants */
#include "funcs.h"		/* function declarations                */
#include "globals.h"		/* alphabet global variables            */
#include "squid.h"		/* general sequence analysis library    */

#ifdef MEMDEBUG
#include "dbmalloc.h"
#endif


int main(int argc, char **argv) 
{
    const char      *hmmfile;	/* file to read HMMs from                  */
    FILE            *fp;	/* output file handle                      */
    HMMFILE         *hmmfp;	/* opened hmmfile for reading              */
    struct plan7_s  *hmm;	/* HMM to generate from                    */
    int              L;		/* length of a sequence                    */
    int              i;		/* counter over sequences                  */

    char            *ofile;	/* output sequence file                    */
    int              nseq;	/* number of seqs to sample                */
    int              seed;	/* random number generator seed            */
    int              be_quiet;	/* TRUE to silence header/footer           */
    int              do_alignment; /* TRUE to output in aligned format     */ 
    int              do_consensus; /* TRUE to do a single consensus seq    */

    AjBool ajselex;
    AjBool ajcons;
    AjPFile inf=NULL;
    AjPFile outf=NULL;
    AjPStr  instr=NULL;
    AjPStr  outstr=NULL;
  

#ifdef MEMDEBUG
    unsigned long histid1, histid2, orig_size, current_size;
    orig_size = malloc_inuse(&histid1);
    fprintf(stderr, "[... memory debugging is ON ...]\n");
#endif

    /*********************************************** 
     * Parse command line
     ***********************************************/

    nseq         = 10;

    be_quiet     = FALSE;
    do_alignment = FALSE;  
    do_consensus = FALSE;
    ofile        = NULL;

    ajNamInit("emboss");
    ajAcdInitP("ehmmemit",argc,argv,"HMMER");

    ajselex = ajAcdGetBool("selex");
    ajcons  = ajAcdGetBool("consensus");
    nseq    = ajAcdGetInt("number");
    seed    = ajAcdGetInt("seed");
    inf     = ajAcdGetInfile("infile");
    outf    = ajAcdGetOutfile("outfile");
  
    if(!seed)
	seed = time ((time_t *) NULL);

    if(ajselex)
	do_alignment=TRUE;
    else
	do_alignment=FALSE;
  
    if(ajcons)
	do_consensus=TRUE;
    else
	do_consensus=FALSE;

    instr  = ajStrNewC((char *)ajFileName(inf));
    outstr = ajStrNewC((char *)ajFileName(outf));

    hmmfile = ajStrStr(instr);

    sre_srandom(seed);

    if (do_alignment && do_consensus)
	ajFatal("Sorry, -selex and -consensus are incompatible.\n"); 
    if (nseq != 10 && do_consensus)
	ajWarn("-consensus overrides -number (# of sampled seqs)");

    /*********************************************** 
     * Open HMM file (might be in HMMERDB or current directory).
     * Read a single HMM from it.
     ***********************************************/

    if ((hmmfp = HMMFileOpen(hmmfile, "HMMERDB")) == NULL)
	ajFatal("Failed to open HMM file %s\n", hmmfile);
    if (!HMMFileRead(hmmfp, &hmm)) 
	ajFatal("Failed to read any HMMs from %s\n", hmmfile);
    HMMFileClose(hmmfp);
    if (hmm == NULL) 
	ajFatal("HMM file %s corrupt or in incorrect format? Parse failed",
		hmmfile);

    /* Configure the HMM to shut off N,J,C emission: so we
     * do a simple single pass through the model.
     */
    Plan7NakedConfig(hmm);
    Plan7Renormalize(hmm);

    /*********************************************** 
     * Open the output file, or stdout
     ***********************************************/ 

    fp = ajFileFp(outf);
  
 
    /*********************************************** 
     * Show the options banner
     ***********************************************/
    be_quiet=TRUE;
    if (! be_quiet) 
    {
	printf("HMM file:             %s\n", hmmfile);
	if (! do_consensus)
	{
	    printf("Number of seqs:       %d\n", nseq);
	    printf("Random seed:          %d\n", seed);
	}
	printf("- - - - - - - - - - - - - - - - - - - - - - - - - "
	       "- - - - - - -\n\n");
    }

    /*********************************************** 
     * Do the work.
     * If we're generating an alignment, we have to collect
     * all our traces, then output. If we're generating unaligned
     * sequences, we can emit one at a time.
     ***********************************************/

    if (do_consensus) 
    {
	char    *seq;
	SQINFO   sqinfo;	/* info about sequence (name/desc)        */

	EmitConsensusSequence(hmm, &seq, NULL, &L, NULL);
	strcpy(sqinfo.name, "consensus");
	sqinfo.len = L;
	sqinfo.flags = SQINFO_NAME | SQINFO_LEN;

	WriteSeq(fp, kPearson, seq, &sqinfo);
	free(seq);
    }
    else if (do_alignment)
    {
	struct p7trace_s **tr;
	char           **dsq;
	SQINFO          *sqinfo;
	char           **aseq;
	AINFO            ainfo;
	float           *wgt;

	dsq    = MallocOrDie(sizeof(char *)             * nseq);
	tr     = MallocOrDie(sizeof(struct p7trace_s *) * nseq);
	sqinfo = MallocOrDie(sizeof(SQINFO)             * nseq);
	wgt    = MallocOrDie(sizeof(float)              * nseq);
	FSet(wgt, nseq, 1.0);

	for (i = 0; i < nseq; i++)
	{
	    EmitSequence(hmm, &(dsq[i]), &L, &(tr[i]));
	    sprintf(sqinfo[i].name, "seq%d", i+1);
	    sqinfo[i].len   = L;
	    sqinfo[i].flags = SQINFO_NAME | SQINFO_LEN;
	}

	P7Traces2Alignment(dsq, sqinfo, wgt, nseq, hmm->M, tr, FALSE, 
			   &aseq, &ainfo);

	/* Output the alignment */
	WriteSELEX(fp, aseq, &ainfo, 50);
	if (ofile != NULL && !be_quiet)
	    printf("Alignment saved in file %s\n", ofile);

	/* Free memory
	 */
	for (i = 0; i < nseq; i++) 
	{
	    P7FreeTrace(tr[i]);
	    free(dsq[i]);
	}
	FreeAlignment(aseq, &ainfo);
	free(sqinfo);
	free(dsq);
	free(wgt);
	free(tr);
    }
    else				/* unaligned sequence output */
    {
	struct p7trace_s *tr;
	char             *dsq;
	char             *seq;
	SQINFO            sqinfo;

	for (i = 0; i < nseq; i++)
	{
	    EmitSequence(hmm, &dsq, &L, &tr);
	    sprintf(sqinfo.name, "seq%d", i+1);
	    sqinfo.len   = L;
	    sqinfo.flags = SQINFO_NAME | SQINFO_LEN;

	    seq = DedigitizeSequence(dsq, L);

	    WriteSeq(fp, kPearson, seq, &sqinfo);
	  
	    P7FreeTrace(tr);
	    free(dsq);
	    free(seq);
	}
    }

    ajFileClose(&outf);
  
    FreePlan7(hmm);
    SqdClean();

#ifdef MEMDEBUG
    current_size = malloc_inuse(&histid2);
    if (current_size != orig_size)
	malloc_list(2, histid1, histid2);
    else
	fprintf(stderr, "[No memory leaks.]\n");
#endif


    ajStrDel(&instr);
    ajStrDel(&outstr);

    ajExit();
    return 0;
}
