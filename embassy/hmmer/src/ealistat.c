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
 * RCS $Id: ealistat.c,v 1.1 2001/07/29 14:13:49 ajb Exp $
 * 
 * Look at an alignment file, determine some simple statistics.
 * Modified for EMBOSS by Alan Bleasby (ISMB 2001)
 */

#include "ajax.h"
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "globals.h"
#include "squid.h"


#ifdef MEMDEBUG
#include "dbmalloc.h"
#endif


int main(int argc, char **argv)
{
    char     *seqfile;			/* name of aligned sequence file */
    int       fmt;			/* format of seqfile             */
    char    **aseq;			/* aligned sequences             */
    AINFO     ainfo;			/* info about sequences          */
    int       nres;			/* number of residues */
    float  **imx;			/* identity matrix               */
    int       i,j;
    int       small, large;	
    int       bestj=0., worstj=0.;
    float     sum, best, worst;
    float     worst_worst=0., worst_best=0., best_best=0.;
    float     avgid;
    int       nsample;
    AjPFile inf=NULL;
    AjPStr  infname=NULL;
    AjPFile outf=NULL;
    AjBool  ajadd;
    AjBool  ajfast;
    int    allreport;
    int    do_fast;

#ifdef MEMDEBUG
    unsigned long histid1, histid2, orig_size, current_size;
#endif

    /***********************************************
     * Parse command line
     ***********************************************/

    allreport = FALSE;
    do_fast   = FALSE;

    ajNamInit("emboss");
    ajAcdInitP("ealistat",argc,argv,"HMMER");

    inf = ajAcdGetInfile("infile");
    outf = ajAcdGetOutfile("outfile");
    ajadd = ajAcdGetBool("additional");
    ajfast = ajAcdGetBool("fast");
  
    if(ajadd)
	allreport=TRUE;
    else
	allreport=FALSE;
    if(ajfast)
	do_fast=TRUE;
    else
	do_fast=FALSE;
  
    infname = ajStrNewC((char *)ajFileName(inf));
    seqfile = ajStrStr(infname);
    ajFileClose(&inf);

#ifdef MEMDEBUG
    orig_size = malloc_size(&histid1);
#endif

    if (do_fast && allreport)
	ajFatal("Verbose report (-a) is incompatible with fast sampling (-f)");

    /***********************************************
     * Read the file.
     ***********************************************/

    ajFmtPrintF(outf,"alistat %s, %s\n\n", squid_version, squid_date);

    if (! SeqfileFormat(seqfile, &fmt, NULL))
	switch (squid_errno)
	{
	case SQERR_NOFILE: 
	    ajFatal("Alignment file %s could not be opened for reading",
		    seqfile);
	    /*FALLTHRU*/
	case SQERR_FORMAT: 
	default:           
	    ajFatal("Failed to determine format of sequence file %s", seqfile);
	}
    if (! ReadAlignment(seqfile, fmt, &aseq, &ainfo))
	ajFatal("Failed to read aligned sequence file %s", seqfile);
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
	avgid = AlignmentIdentityBySampling(aseq, ainfo.alen, ainfo.nseq,
					    nsample);
    }
    else
    {
	/* In a full report, for each sequence, find the best relative,
	 * and the worst relative.
	 * For overall statistics, save the worst best
	 * (most distant single seq)
	 * and the best best (most closely related pair)
	 * and the worst worst (most distantly related pair)
	 * and yes, I know it's confusing.
	 */

	MakeIdentityMx(aseq, ainfo.nseq, &imx);
	if (allreport)
	{
	    ajFmtPrintF(outf,"  %-15s %5s %7s %-15s %7s %-15s\n",
			"NAME", "LEN", "HIGH ID", "(TO)", "LOW ID", "(TO)");
	    ajFmtPrintF(outf,"  --------------- ----- ------- -------------"
			"-- ------- ---------------\n");
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
	    {				/* closest seq to this one = best */
		if (i != j && imx[i][j] > best)
		{
		    best  = imx[i][j];
		    bestj = j;
		}
		if (imx[i][j] < worst)
		{
		    worst = imx[i][j];
		    worstj = j;
		}
	    }

	    if (allreport) 
		ajFmtPrintF(outf,"* %-15s %5d %7.1f %-15s %7.1f %-15s\n",
			    ainfo.sqinfo[i].name, ainfo.sqinfo[i].len,
			    best * 100.,  ainfo.sqinfo[bestj].name,
			    worst * 100., ainfo.sqinfo[worstj].name);
	  
	    if (best > best_best)
		best_best = best;
	    if (best < worst_best)
		worst_best = best;
	    if (worst < worst_worst)
		worst_worst = worst;
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
    ajFmtPrintF(outf,"Format:              %s\n",     SeqFormatString(fmt));
    ajFmtPrintF(outf,"Number of sequences: %d\n",     ainfo.nseq);
    ajFmtPrintF(outf,"Total # residues:    %d\n",     nres);
    ajFmtPrintF(outf,"Smallest:            %d\n",     small);
    ajFmtPrintF(outf,"Largest:             %d\n",     large);
    ajFmtPrintF(outf,"Average length:      %.1f\n",
		(float) nres / (float) ainfo.nseq);
    ajFmtPrintF(outf,"Alignment length:    %d\n",     ainfo.alen);
    ajFmtPrintF(outf,"Average identity:    %.0f%%\n", 100.*avgid);
    if (! do_fast) {
	ajFmtPrintF(outf,"Most related pair:   %.0f%%\n", 100.*best_best);
	ajFmtPrintF(outf,"Most unrelated pair: %.0f%%\n", 100.*worst_worst);
	ajFmtPrintF(outf,"Most distant seq:    %.0f%%\n", 100.*worst_best);
    }
    ajFmtPrintF(outf,"\n");

    FreeAlignment(aseq, &ainfo);

#ifdef MEMDEBUG
    current_size = malloc_size(&histid2);
  
    if (current_size != orig_size)
	malloc_list(2, histid1, histid2);
    else
	fprintf(stderr, "[No memory leaks]\n");
#endif

    ajStrDel(&infname);
    ajFileClose(&outf);

    ajExit();
    return 0;
}
