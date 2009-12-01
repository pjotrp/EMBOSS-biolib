/* @Source seqnr application
**
** Removes redundancy from DHF files (domain hits files) or other files of 
** sequences.
**
** @author: Copyright (C) Ranjeeva Ranasinghe 
** @author: Copyright (C) Jon Ison (jison@ebi.ac.uk)
** @@
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
*******************************************************************************
** 
**  SEQNR Documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**  
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@ebi.ac.uk.
**  
**  NOTES
**  
****************************************************************************/





#include "emboss.h"





/* @prog seqnr **************************************************************
**
** Removes redundancy from DHF files (domain hits files) or other files of 
** sequences.
**
****************************************************************************/
int main(int argc, char **argv)
{
    /* Variable declarations */
    AjPList    in        = NULL;    /* Names of domain hits files (input).   */
    AjPStr     inname    = NULL;    /* Full name of the current DHF file.    */
    AjPFile    inf       = NULL;    /* Current DHF file.                     */
    EmbPHitlist infhits   = NULL;   /* Hitlist from DHF file                 */
    AjBool     dosing    = ajFalse; /* Filter using singlet sequences.       */
    AjPDir     singlets  = NULL;    /* Singlets (input).                     */
    AjBool     dosets    = ajFalse; /* Filter using sets of sequences.       */
    AjPDir     insets    = NULL;    /* Sets (input).                         */
    AjPStr     mode      = NULL;    /* Mode of operation                     */
    ajint      moden     = 0;       /* Mode 1: single threshold for redundancy
				       removal, 2: lower and upper thresholds
				       for redundancy removal.               */
    float      thresh    = 0.0;     /* Threshold for non-redundancy.         */
    float      threshlow = 0.0;	    /* Threshold (lower limit).              */
    float      threshup  = 0.0;	    /* Threshold (upper limit).              */
    AjPMatrixf matrix    = NULL;    /* Substitution matrix.                  */
    float      gapopen   = 0.0;     /* Gap insertion penalty.                */
    float      gapextend = 0.0;     /* Gap extension penalty.                */
    AjPDirout  out       = NULL;    /* Domain hits files (output).           */
    AjPFile    outf      = NULL;    /* Current DHF file (output).            */
    AjBool     dored     = ajFalse; /* True if redundant hits are output.    */
    AjPDirout  outred    = NULL;    /* DHF files for redundant hits (output).*/
    AjPFile    redf      = NULL;    /* Current DHF file redundancy (output). */
    AjPStr     outname   = NULL;    /* Name of output file (re-used).        */
    AjPFile    logf      = NULL;    /* Log file pointer.                     */
 
    AjBool     ok        = ajFalse; /* Housekeeping.                         */
    AjPSeqset  seqset    = NULL;    /* Seqset (re-used).                     */
    AjPSeqin   seqin     = NULL;    /* Seqin (re-used).                      */
    AjPList    seq_list  = NULL;    /* Main list for redundancy removal.     */
    EmbPDmxNrseq seq_tmp = NULL;    /* Temp. pointer for making seq_list.    */
    ajint      seq_siz   = 0;       /* Size of seq_list.                     */
    AjPUint    keep      = NULL;    /* 1: Sequence in seq_list was classed as
				       non-redundant, 0: redundant.          */
    AjPUint    nokeep    = NULL;    /* Inversion of keep array.              */
    ajint      nseqnr    = 0;       /* No. non-redundant seqs. in seq_list.  */
    

    AjPStr     filtername= NULL;    /* Name of filter file (re-used).        */
    AjPFile    filterf   = NULL;    /* Current filter file.                  */
    EmbPHitlist hitlist   = NULL;   /* Hitlist from input file (re-used).    */
    AjPScopalg scopalg   = NULL;    /* Scopalg from input file.              */
    ajint      x         = 0;       /* Housekeeping.                         */
    

    


    /* Read data from acd. */
    embInitPV("seqnr",argc,argv,"DOMSEARCH",VERSION);

    in        = ajAcdGetDirlist("dhfinpath");
    dosing    = ajAcdGetToggle("dosing");
    singlets    = ajAcdGetDirectory("singletsdir");
    dosets    = ajAcdGetToggle("dosets");
    insets    = ajAcdGetDirectory("insetsdir");
    mode      = ajAcdGetListSingle("mode");  
    thresh    = ajAcdGetFloat("thresh");
    threshlow = ajAcdGetFloat("threshlow");
    threshup  = ajAcdGetFloat("threshup");
    matrix    = ajAcdGetMatrixf("matrix");
    gapopen   = ajAcdGetFloat("gapopen");
    gapextend = ajAcdGetFloat("gapextend");
    out       = ajAcdGetOutdir("dhfoutdir");
    dored     = ajAcdGetToggle("dored");
    outred    = ajAcdGetOutdir("redoutdir");
    logf      = ajAcdGetOutfile("logfile");



    /* Housekeeping. */
    filtername  = ajStrNew();
    outname     = ajStrNew();


    if(!(ajStrToInt(mode, &moden)))
	ajFatal("Could not parse ACD node option");


    
    /* Process each DHF (input) in turn. */
    while(ajListPop(in,(void **)&inname))
    {
	ajFmtPrint("Processing %S\n", inname);
	ajFmtPrintF(logf, "//\n%S\n", inname);


	seq_list    = ajListNew();
	keep        = ajUintNew();  	    
	nokeep      = ajUintNew();  	    	
	
	/**********************************/
	/*         Open DHF file          */
	/**********************************/
	if((inf = ajFileNewInNameS(inname)) == NULL)
	    ajFatal("Could not open DHF file %S", inname);

	/* Read DHF file. */
	ok = ajFalse;
	if(!(infhits = embHitlistReadFasta(inf)))
	{
	    ajWarn("embHitlistReadFasta call failed in seqnr");
	    ajFmtPrintF(logf, "embHitlistReadFasta call failed in seqnr\n");
	
	    /* Read sequence set instead. */ 
	    seqset = ajSeqsetNew();
	    seqin  = ajSeqinNew();
	    ajSeqinUsa(&seqin, inname);
	
	    if(!(ajSeqsetRead(seqset, seqin)))
		ajFatal("SeqsetRead failed in seqsearch_psialigned");

	    if(ajSeqsetGetSize(seqset))
		ok = ajTrue;
	}
	else
	    if(infhits->N)
		ok = ajTrue;

	/* Close DHF file. */
	ajFileClose(&inf);
	
	/* Process empty DHF files (should never occur). */
	if(!ok)
	{		
	    ajWarn("Empty input file %S\n", inname);
	    ajFmtPrintF(logf, "Empty input file %S\n", inname);
	    if(infhits)
		embHitlistDel(&infhits);
	    if(seqset)
		ajSeqsetDel(&seqset);
	    if(seqin)
		ajSeqinDel(&seqin);
	    continue;
	}	

	
	/* 1.  Create list of sequences from the main input directory.. */
	if(infhits)
	{
	    for(x=0; x<infhits->N; x++)
	    {
		AJNEW0(seq_tmp);
		seq_tmp->Seq = ajSeqNew();
		ajStrAssignS(&seq_tmp->Seq->Acc,infhits->hits[x]->Acc);
		ajStrAssignS(&seq_tmp->Seq->Seq,infhits->hits[x]->Seq);
		ajListPushAppend(seq_list,seq_tmp);		
	    }
	} 
	else
	{	 
	    for(x=0;x<ajSeqsetGetSize(seqset);x++)
	    {
		AJNEW0(seq_tmp);
		seq_tmp->Seq = ajSeqNew();
		ajStrAssignS(&seq_tmp->Seq->Acc, ajSeqsetGetseqAccS(seqset, x));
		ajStrAssignS(&seq_tmp->Seq->Seq, ajSeqsetGetseqSeqS(seqset, x));
		ajListPushAppend(seq_list,seq_tmp);		
	    }
	    ajSeqsetDel(&seqset);
	    ajSeqinDel(&seqin);
	}
	
    

	/**********************************/
	/*   Open singlets filter file    */
	/**********************************/
	if(dosing)
	{
	    /* Open singlets file. */
	    ajStrAssignS(&filtername, inname);
	    ajFilenameTrimPathExt(&filtername);
	    ajStrInsertS(&filtername, 0, ajDirGetPath(singlets));
	    ajStrAppendC(&filtername, ".");
	    ajStrAppendS(&filtername, ajDirGetExt(singlets));

	
	    if((filterf = ajFileNewInNameS(filtername)) == NULL)
	    {
		ajWarn("Could not open DHF file %S",
		       filtername);
		ajFmtPrint("Could not open singlets filter file %S",
			   filtername);
	    }
	    else
	    {
		/* Read DHF file. */
		ok = ajFalse;
		if(!(hitlist = embHitlistReadFasta(filterf)))
		{
		    ajWarn("embHitlistReadFasta call failed in seqnr");
		    ajFmtPrintF(logf, 
				"embHitlistReadFasta call failed in seqnr\n");
	
		    /* Read sequence set instead. */ 
		    seqset = ajSeqsetNew();
		    seqin  = ajSeqinNew();
		    ajSeqinUsa(&seqin, inname);
	
		    if(!(ajSeqsetRead(seqset, seqin)))
			ajFatal("SeqsetRead failed in seqnr");

		    if(ajSeqsetGetSize(seqset))
			ok = ajTrue;
		}
		else
		    if(hitlist->N)
			ok = ajTrue;


		/* Close DHF file. */
		ajFileClose(&filterf);

	
		/* Process empty DHF files (should never occur). */
		if(!ok)
		{		
		    ajWarn("Empty singlets filter file %S\n", filtername);
		    ajFmtPrintF(logf, "Empty singlets filter file %S\n", 
				filtername);
		    /* No continue this time. */
		}	

	
		/* 2. Add sequences from filter directories to List but mark 
		   them up (they are considered in the redundancy calculation 
		   but never appear in the output files). */
		if(hitlist)
		{
		    for(x=0; x<hitlist->N; x++)
		    {
			AJNEW0(seq_tmp);
			seq_tmp->Seq = ajSeqNew();
			seq_tmp->Garbage = ajTrue;
			ajStrAssignS(&seq_tmp->Seq->Acc,hitlist->hits[x]->Acc);
			ajStrAssignS(&seq_tmp->Seq->Seq,hitlist->hits[x]->Seq);
			ajListPushAppend(seq_list,seq_tmp);		
		    }
		    embHitlistDel(&hitlist);
		} 
		else
		{	 
		    for(x=0;x<ajSeqsetGetSize(seqset);x++)
		    {
			AJNEW0(seq_tmp);
			seq_tmp->Seq = ajSeqNew();
			seq_tmp->Garbage = ajTrue;
			ajStrAssignS(&seq_tmp->Seq->Acc,
				     ajSeqsetGetseqAccS(seqset, x));
			ajStrAssignS(&seq_tmp->Seq->Seq,
				     ajSeqsetGetseqSeqS(seqset, x));
			ajListPushAppend(seq_list,seq_tmp);		
		    }
		    ajSeqsetDel(&seqset);
		    ajSeqinDel(&seqin);
		}
	    }
	}
	
	
	
	/**********************************/
	/*      Open sets filter file     */
	/**********************************/
	if(dosets)
	{
	    /* Open sets file. */
	    ajStrAssignS(&filtername, inname);
	    ajFilenameTrimPathExt(&filtername);
	    ajStrInsertS(&filtername, 0, ajDirGetPath(insets));
	    ajStrAppendC(&filtername, ".");
	    ajStrAppendS(&filtername, ajDirGetExt(insets));

	
	    if((filterf = ajFileNewInNameS(filtername)) == NULL)
	    {
		ajWarn("Could not open DAF file %S", filtername);
		ajFmtPrint("Could not open sets filter file %S", filtername);
	    }
	    else
	    {
		/* Read DAF file. */
		ok = ajFalse;

		if(!(ajDmxScopalgRead(filterf, &scopalg)))
		{
		    ajWarn("ajDmxScopalgRead call failed in seqnr");
		    ajFmtPrintF(logf,
				"ajDmxScopalgRead call failed in seqnr\n");
	
		    /* Read sequence set instead. */ 
		    seqset = ajSeqsetNew();
		    seqin  = ajSeqinNew();
		    ajSeqinUsa(&seqin, inname);
		    
		    if(!(ajSeqsetRead(seqset, seqin)))
			ajFatal("SeqsetRead failed in seqnr");

		    if(ajSeqsetGetSize(seqset))
			ok = ajTrue;
		}
		else
		    if(scopalg->N)
			ok = ajTrue;


		/* Close DHF file. */
		ajFileClose(&filterf);

	
		/* Process empty DHF files (should never occur). */
		if(!ok)
		{		
		    ajWarn("Empty sets filter file %S\n",
			   filtername);
		    ajFmtPrintF(logf, "Empty sets filter file %S\n",
				filtername);
		    /* No continue this time. */
		}	

	
		/* 2. Add sequences from filter directories to List but mark 
		   them up (they are considered in the redundancy calculation 
		   but never appear in the output files).. */
		if(scopalg)
		{
		    for(x=0; x<scopalg->N; x++)
		    {
			AJNEW0(seq_tmp);
			seq_tmp->Seq = ajSeqNew();
			seq_tmp->Garbage = ajTrue;
			ajStrAssignS(&seq_tmp->Seq->Acc,scopalg->Codes[x]);
			ajStrAssignS(&seq_tmp->Seq->Seq,scopalg->Seqs[x]);
			/* Remove gap char's & whitespace. */
			ajStrRemoveGap(&seq_tmp->Seq->Seq);  
			ajListPushAppend(seq_list,seq_tmp);		
		    }
		    ajDmxScopalgDel(&scopalg);
		} 
		else
		{	 
		    for(x=0;x<ajSeqsetGetSize(seqset);x++)
		    {
			AJNEW0(seq_tmp);
			seq_tmp->Seq = ajSeqNew();
			seq_tmp->Garbage = ajTrue;
			ajStrAssignS(&seq_tmp->Seq->Acc,
				     ajSeqsetGetseqAccS(seqset, x));
			ajStrAssignS(&seq_tmp->Seq->Seq,
				     ajSeqsetGetseqSeqS(seqset, x));
			ajListPushAppend(seq_list,seq_tmp);		
		    }
		    ajSeqsetDel(&seqset);
		    ajSeqinDel(&seqin);
		}
	    }
	}
	
	
	/* 4. Identify redundant domains.. */
	if(moden == 1)
	{
	    if((!embDmxSeqNR(seq_list, &keep, &nseqnr, matrix, gapopen, 
			     gapextend, thresh, ajTrue)))
		ajFatal("embDmxSeqNR failure in seqnr");
	}		
	else
	{
	    if((!embDmxSeqNRRange(seq_list, &keep, &nseqnr, matrix, gapopen, 
			     gapextend, threshlow, threshup, ajTrue)))
		ajFatal("embDmxSeqNR failure in seqnr");
	}	
	seq_siz = ajListGetLength(seq_list);
	for(x=0; x<seq_siz; x++)
	    if(ajUintGet(keep, x) == 1)
		ajUintPut(&nokeep, x, 0);
	    else
		ajUintPut(&nokeep, x, 1);
	

	/* Create output files. */
	ajStrAssignS(&outname, inname);
	ajFilenameTrimPathExt(&outname);
	outf = ajFileNewOutNameDirS(outname, out);
	if(dored)
	    redf = ajFileNewOutNameDirS(outname, outred);
	

	/* 5. Write non-redundant domains to main output directory.  
	   6.  If specified, write redundant domains to output directory. */
	embHitlistWriteSubsetFasta(outf, infhits, keep);
	if(dored)
	    embHitlistWriteSubsetFasta(redf, infhits, nokeep);

	embHitlistDel(&infhits);
	ajFileClose(&outf);
	ajFileClose(&redf);
	ajStrDel(&inname);

	while(ajListPop(seq_list, (void **) &seq_tmp))
	{
	    ajSeqDel(&seq_tmp->Seq);
	    AJFREE(seq_tmp);
	}
	ajListFree(&seq_list);
	ajUintDel(&keep);	
	ajUintDel(&nokeep);
    }	    


    /* Tidy up. */
    ajListFree(&in);
    if(singlets)
	ajDirDel(&singlets);
    if(insets)
	ajDirDel(&insets);
    ajDiroutDel(&out);
    if(outred)
	ajDiroutDel(&outred);
    ajFileClose(&logf);

    ajMatrixfDel(&matrix);

    ajStrDel(&filtername);
    ajStrDel(&outname);
    ajStrDel(&mode);


    embExit();
    return 0;
}



