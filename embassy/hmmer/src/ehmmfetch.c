/************************************************************
 * HMMER - Biological sequence analysis with profile HMMs
 * Copyright (C) 1992-1998 Washington University School of Medicine
 * All Rights Reserved
 * 
 *     This source code is distributed under the terms of the
 *     GNU General Public License. See the files COPYING and LICENSE
 *     for details.
 ************************************************************/

/* hmmfetch.c
 * SRE, Wed Aug  5 14:26:51 1998 [St. Louis]
 * 
 * Recover a specific HMM file from an HMM database, using
 * a GSI index (created with hmmindex).
 * 
 * RCS $Id: ehmmfetch.c,v 1.2 2004/06/14 14:43:30 rice Exp $
 *
 * Modified for EMBOSS by Alan Bleasby (ISMB 2001)
 */

#include "ajax.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "squid.h"
#include "config.h"
#include "structs.h"
#include "funcs.h"
#include "version.h"

#ifdef MEMDEBUG
#include "dbmalloc.h"
#endif

#include "globals.h"


int main(int argc, char **argv)
{
    const char *hmmfile;		/* HMM file to open                */
    const char *key;			/* HMM name to retrieve            */
    HMMFILE *hmmfp;			/* opened hmm file pointer         */
    struct plan7_s *hmm;		/* a hidden Markov model           */

    AjPStr ajdb;
    AjPStr ajname;
    FILE   *ajfp;
    AjPFile outf=NULL;
    
#ifdef MEMDEBUG
    unsigned long histid1, histid2, orig_size, current_size;
    orig_size = malloc_inuse(&histid1);
    fprintf(stderr, "[... memory debugging is ON ...]\n");
#endif

    /***********************************************
     * Parse the command line
     ***********************************************/

    ajNamInit("emboss");
    ajAcdInitP("ehmmfetch",argc,argv,"HMMER");

    ajdb   = ajAcdGetString("database");
    ajname = ajAcdGetString("name");
    outf   = ajAcdGetOutfile("outfile");
    
    hmmfile = ajStrStr(ajdb);
    key     = ajStrStr(ajname);
  

    /***********************************************
     * Open HMM file, make sure GSI index exists
     ***********************************************/

    if ((hmmfp = HMMFileOpen(hmmfile, "HMMERDB")) == NULL)
	ajFatal("failed to open HMM file %s for reading.", hmmfile);
    if (hmmfp->gsi == NULL)
	ajFatal("There is no GSI index for %s; you need to use "
		"hmmindex on it.", hmmfile);

    /***********************************************
     * find key in hmmfile; get HMM; show as ASCII
     ***********************************************/

    if (! HMMFilePositionByName(hmmfp, key))
	ajFatal("No such hmm %s in HMM file %s\n", key, hmmfile);
    HMMFileRead(hmmfp, &hmm); 
    if (hmm == NULL) 
	ajFatal("HMM file %s may be corrupt or in incorrect format; "
		"parse failed", hmmfile);

    ajfp = ajFileFp(outf);
    WriteAscHMM(ajfp, hmm);
    ajFileClose(&outf);
  
    FreePlan7(hmm);
    HMMFileClose(hmmfp);

    /***********************************************
     * Exit
     ***********************************************/

    SqdClean();

#ifdef MEMDEBUG
    current_size = malloc_size(&histid2);
    if (current_size != orig_size)
	malloc_list(2, histid1, histid2);
    else
	fprintf(stderr, "[No memory leaks]\n");
#endif

    ajExit();
    return 0;
}
