/************************************************************
 * HMMER - Biological sequence analysis with profile HMMs
 * Copyright (C) 1992-1998 Washington University School of Medicine
 * All Rights Reserved
 * 
 *     This source code is distributed under the terms of the
 *     GNU General Public License. See the files COPYING and LICENSE
 *     for details.
 ************************************************************/

/* hmmindex.c
 * SRE, Wed Aug  5 11:05:03 1998 [St. Louis]
 * 
 * Create a GSI index file for an HMM database.
 * 
 * RCS $Id: ehmmindex.c,v 1.2 2004/06/14 14:43:30 rice Exp $
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


struct gsikey_s
{
  char        key[GSI_KEYSIZE];
  int         filenum;
  long        offset;
};
#define KEYBLOCK 100		/* malloc blocks of keys */




static int gsikey_compare(const void *el1, const void *el2)
{
    struct gsikey_s *key1;
    struct gsikey_s *key2;
    key1 = (struct gsikey_s *) el1;
    key2 = (struct gsikey_s *) el2;

    return strcmp(key1->key, key2->key);
}




int main(int argc, char **argv)
{
    const char    *hmmfile;		/* HMM file to open                */
    char    *hmmtail;			/* HMMfile without directory path  */
    char    *gsifile;			/* GSI file to write               */
    HMMFILE *hmmfp;			/* opened hmm file pointer         */
    FILE    *outfp;			/* open gsifile for writing        */
    struct plan7_s     *hmm;		/* a hidden Markov model           */
    int     idx, nhmm;			/* counter over HMMs               */
    int     nkeys;			/* number of keys                  */
    long    offset;			/* offset in HMM file              */
    struct gsikey_s *keylist;		/* list of keys                    */
    char    fname[GSI_KEYSIZE];

    AjPFile inf=NULL;
    AjPStr  instr=NULL;
  
#ifdef MEMDEBUG
    unsigned long histid1, histid2, orig_size, current_size;
    orig_size = malloc_inuse(&histid1);
    fprintf(stderr, "[... memory debugging is ON ...]\n");
#endif

    /***********************************************
     * Parse the command line
     ***********************************************/

    ajNamInit("emboss");
    ajAcdInitP("ehmmindex",argc,argv,"HMMER");

    inf = ajAcdGetInfile("infile");

    instr   = ajStrNewC((char *)ajFileName(inf));
    hmmfile = ajStrStr(instr);
    ajFileClose(&inf);
    

    /***********************************************
     * Open our i/o file pointers, make sure all is well
     ***********************************************/

    if ((hmmfp = HMMFileOpen(hmmfile, NULL)) == NULL)
	ajFatal("failed to open HMM file %s for reading.", hmmfile);
    if (hmmfp->gsi != NULL)
	ajFatal("GSI index already exists for %s. Please delete it first.",
		hmmfile);
  
    gsifile = MallocOrDie(strlen(hmmfile) + 5);
    strcpy(gsifile, hmmfile);
    strcat(gsifile, ".gsi");
    if (FileExists(gsifile))
	ajFatal("GSI file %s already exists; please delete it first",
		gsifile);	/* shouldn't happen */
    if ((outfp = fopen(gsifile, "wb")) == NULL)
	ajFatal("GSI file %s couldn't be opened for writing", gsifile); 

    keylist = MallocOrDie(sizeof(struct gsikey_s) * KEYBLOCK);

    /*********************************************** 
     * Show the banner
     ***********************************************/
/*
    Banner(stdout, banner);
    printf("HMM file:                 %s\n", hmmfile);
    printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - "
	   "- - -\n\n");
*/
    /***********************************************
     * Get offsets and names for every model; store in keylist
     ***********************************************/
/*
    printf("Determining offsets for %s, please be patient...\n", hmmfile);
*/
    offset = ftell(hmmfp->f);
    nhmm = nkeys = 0;
    while (HMMFileRead(hmmfp, &hmm)) 
    {	
	if (hmm == NULL) 
	    ajFatal("HMM file %s may be corrupt or in incorrect format; "
		    "parse failed", hmmfile);

	if (strlen(hmm->name) >= GSI_KEYSIZE )
	    ajWarn("HMM %s name is too long to index in a GSI file; "
		   "truncating it.", hmm->name);
	/* record name of HMM as a retrieval key */
	if (strlen(hmm->name) >= GSI_KEYSIZE) 
	    ajWarn("HMM name %s is too long to be indexed: must be < %d\n",
		   GSI_KEYSIZE);
	strncpy(keylist[nkeys].key, hmm->name, GSI_KEYSIZE-1);
	keylist[nkeys].key[GSI_KEYSIZE-1] = '\0';
	keylist[nkeys].filenum = 1;
	keylist[nkeys].offset  = offset;
	nkeys++;
	if (nkeys % KEYBLOCK == 0)	
	    keylist = ReallocOrDie(keylist, sizeof(struct gsikey_s) *
				   (nkeys + KEYBLOCK));

	/* record accession of HMM as a retrieval key */
	if (hmm->flags & PLAN7_ACC)
	{
	    if (strlen(hmm->acc) >= GSI_KEYSIZE) 
		ajWarn("HMM accession %s is too long to be indexed: must "
		       "be < %d\n", GSI_KEYSIZE);
	    strncpy(keylist[nkeys].key, hmm->acc, GSI_KEYSIZE-1);
	    keylist[nkeys].key[GSI_KEYSIZE-1] = '\0';
	    keylist[nkeys].filenum = 1;
	    keylist[nkeys].offset  = offset;
	    nkeys++;
	    if (nkeys % KEYBLOCK == 0)	
		keylist = ReallocOrDie(keylist, sizeof(struct gsikey_s) *
				       (nkeys + KEYBLOCK));
	}

	offset = ftell(hmmfp->f);
	nhmm++;
	FreePlan7(hmm);
    }
    HMMFileClose(hmmfp);

    /***********************************************
     * Sort the keylist
     ***********************************************/
/*
    printf("Sorting keys... \n");
*/
    qsort((void *) keylist, nkeys, sizeof(struct gsikey_s), gsikey_compare);
/*    SQD_DPRINTF1(("(OK, done with qsort)\n"));  */

    /***********************************************
     * Output the GSI file
     ***********************************************/

    hmmtail = FileTail(hmmfile, FALSE);
    if (strlen(hmmtail) >= GSI_KEYSIZE)
    {
	ajWarn("HMM file name length is >= %d char. Truncating.", GSI_KEYSIZE);
	hmmtail[GSI_KEYSIZE-1] = '\0';
    }
    strcpy(fname, hmmtail);

    GSIWriteHeader(outfp, 1, nkeys);
    /* this line is unused, so doesn't matter */
    GSIWriteFileRecord(outfp, fname, 1, 0);
    for (idx = 0; idx < nkeys; idx++)
	GSIWriteKeyRecord(outfp, keylist[idx].key, keylist[idx].filenum,
			  keylist[idx].offset);
/*
    printf("Complete.\n");
    printf("GSI %s indexes %d keys (names+accessions) for %d HMMs in %s.\n", 
	   gsifile, nkeys, nhmm, hmmfile);
*/
    /***********************************************
     * Exit
     ***********************************************/

    free(hmmtail);
    free(gsifile);
    free(keylist);
    if (fclose(outfp) != 0) PANIC;
    SqdClean();

#ifdef MEMDEBUG
    current_size = malloc_inuse(&histid2);
    if (current_size != orig_size)
	malloc_list(2, histid1, histid2);
    else
	fprintf(stderr, "[No memory leaks]\n");
#endif

    ajStrDel(&instr);

    ajExit();
    return 0;
}
