/* @source seqalign application
**
** Reads a DAF file (domain alignment file) and a DHF (domain hits file) and 
** writes a DAF file extended with the hits.
** 
** @author: Copyright (C) Ranjeeva Ranasinghe (rranasin@hgmp.mrc.ac.uk)
** @author: Copyright (C) Jon Ison (jison@hgmp.mrc.ac.uk)
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
**  SEQALIGN Documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**  
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**  
**  Email jison@rfcgr.mrc.ac.uk.
**  
**  NOTES
**  seqalign writes a temporary clustalw input file in fasta format (via a call 
**  to embDmxScopalgWriteFasta) rather than clustal format. This is to ensure
**  it will work on input alignments that have been hand-edited by Jalview.
**  (Jalview writes a deviant "clustal" format file that causes clustalw to fail).
**  
****************************************************************************/





#include "emboss.h"





/* @prog seqalign *************************************************************
**
** Reads a DAF file (domain alignment file) and a DHF (domain hits file) and 
** writes a DAF file extended with the hits.
**
******************************************************************************/
int main(int argc, char **argv)
{
    AjPList  inseqs    = NULL;  /* Directory of input sequences. Is made to 
				   point to inseqsdhf or inseqsdaf. */
    AjPList  inseqsdhf = NULL;  /* Directory of DAF file (domain alignment 
				   file) or singlet  sequence files (input).   */
    AjPList  inseqsdaf = NULL;  /* Directory of DAF file (domain alignment 
				   file) or singlet  sequence files (input).   */

    AjPStr  inname     = NULL;  /* Full name of the current DAF or singlet file*/
    AjPFile inf        = NULL;  /* DAF or singlet file (input)                 */


    AjPDir dhfin       = NULL;  /* Directory of domain hits files (input).     */
    AjPFile hitsinf    = NULL;  /* Current domain hits file.                   */
    AjPStr hitsname    = NULL;  /* Name of current domain hits file.           */

    AjPDir  dafout     = NULL;  /* Directory of domain alignment files (output)*/

    AjPStr  tmp_name   = NULL;  /* Random name for temp. files.                */

    AjPStr  clustin1   = NULL;  /* Name of clustalw input alignment file.      */
    AjPFile clustinf1  = NULL;  /* Clustalw input alignment file (CLUSTAL format) */
    AjPStr  clustin2   = NULL;  /* Name of clustalw input sequence file.       */
    AjPFile clustinf2  = NULL;  /* Clustalw input sequence file                */

    AjPStr  clustout   = NULL;  /* Name of clustalw file that will be created, 
				   will be reformated into domain alignment 
				   format                                      */
    AjPStr  clustdnd   = NULL;  /* the name of the Clustal tree file (delete)  */

    AjPFile    logf    = NULL;  /* Log file pointer                            */

    ajint x=0;                  /* Loop counter                                */
    AjPStr  cmd        = NULL;  /* Executable command                          */
   
    AjPFile alg_in     = NULL;  /* the alignment file for reading              */
    AjPFile alg_out    = NULL;  /* the alignment file for writing              */
    AjPStr outname     = NULL;  /* Name of domain alignment file (output).     */

    AjPStr  line       = NULL;
    
    AjPScopalg scopalign = NULL; /* Scopalg object for input alignmennt 
				    (domain alignment file)                    */
    AjPSeqset seqset_a   = NULL; /* Seqset for input alignment                 */
    AjPSeqin  seqin_a    = NULL; /* Seqin for input alignment                  */

    AjPScophit hit_sing  = NULL; /* Hit for input hit sequences (domain hits 
				    file)                                      */
    AjPSeq    seq_sing   = NULL; /* Seq for input singlet sequence             */
    AjPSeqin  seqin_sing = NULL; /* Seqin for input singlet sequence           */

    AjPHitlist hitlist_h = NULL; /* Hitlist for input hit sequences (domain 
				    hits file)                                 */
    AjPSeqset seqset_h   = NULL; /* Seqset for input hit sequences             */
    AjPSeqin  seqin_h    = NULL; /* Seqin for input hit sequences              */

    AjPStr      *amode   = NULL; /* Mode of operation from acd                 */
    ajint     amoden     = 0;    /* Program mode, 1: MODE_STAMP, 
				    2: MODE_TCOFFEE (not yet implemented)      */

    AjPStr   *mode       = NULL; /* 
				  ** Mode of operation from ACD: 
				  ** 1: Single sequences,
				  ** 2: Sequence sets
				  */
    ajint      modei     = 0;    /* ACD mode as int                            */

    AjPStr    tmp1      = NULL;
    AjPStr    tmp2      = NULL;
    
    AjBool    ok = ajFalse;
    AjBool    ok2 = ajFalse;
    AjBool    forcetype = ajFalse; /* ACD variable */
          

    /* Memory allocation */
    line        = ajStrNew();
    inname  = ajStrNew();
    hitsname  = ajStrNew();
    cmd         = ajStrNew();
    tmp_name    = ajStrNew();
    clustin1    = ajStrNew();
    clustin2    = ajStrNew();
    clustout    = ajStrNew();
    clustdnd    = ajStrNew();
    tmp1        = ajStrNew();
    tmp2        = ajStrNew();
  

    /* ACD processing */
    ajNamInit("emboss");
    ajAcdInitP("seqalign",argc,argv,"DOMALIGN");

    inseqsdhf = ajAcdGetDirlist("dhfinpath");
    inseqsdaf = ajAcdGetDirlist("dafinpath");
    dhfin     = ajAcdGetDirectory("dhfindir");
    dafout    = ajAcdGetOutdir("dafoutdir");
    logf      = ajAcdGetOutfile("logfile");    
    amode     = ajAcdGetList("amode");  
    mode      = ajAcdGetList("mode");    
    forcetype = ajAcdGetBool("forcetype");
    

    /* Convert the selected amode to an integer */
    if(!(ajStrToInt(amode[0], &amoden)))
	ajFatal("Could not parse ACD node option");

    modei      = (ajint) ajStrChar(*mode,0)-48;



    /* Create name for temp. clustalw input files. */
    ajRandomSeed();
    ajStrAssC(&tmp_name,ajFileTempName(NULL));


    /* Read each domain alignment file */
    if(modei==1)
	inseqs = inseqsdhf;
    else if(modei==2)
    	inseqs = inseqsdaf;
    else
	ajFatal("Unrecognised mode");
    
   
    while(ajListPop(inseqs,(void **)&inname))
    {
	ajFmtPrint("Processing %S\n", inname);
	ajFmtPrintF(logf, "//\n%S\n", inname);
	

	/* Open domain alignment file */
	if((inf = ajFileNewIn(inname)) == NULL)
	{
	    ajWarn("Could not open seed alignment file %S", inname);
	    ajFmtPrintF(logf, "//\nCould not open seed alignment file %S\n", inname);
	    continue;
	}



	/* Create file name for clustal .dnd tree file (deleted later) and 
	   clustal output file. */
	ajStrAssS(&clustdnd,tmp_name);
	ajStrAppC(&clustdnd,".dnd");
	ajStrAssS(&clustout,tmp_name);
	ajStrAppC(&clustout,".out");
	    

	/* Input sequences are seed alignment. */
	if(modei==2) 
	{
	    /* Read the domain seed alignment file. */
	    ok = ajFalse;
	    if(!ajDmxScopalgRead(inf,&scopalign))
	    {
		ajWarn("ajDmxScopalgRead call failed in seqsearch_psialigned");
	
		/* Read sequence set instead. */ 
		seqset_a = ajSeqsetNew();
		seqin_a  = ajSeqinNew();
		/* Set the filename via the USA. ajSeqsetRead interprets it to
		   find the filename. */
		ajSeqinUsa(&seqin_a, inname);
	
		if(!(ajSeqsetRead(seqset_a, seqin_a)))
		    ajFatal("SeqsetRead failed in seqsearch_psialigned");

		if(ajSeqsetSize(seqset_a))
		    ok = ajTrue;
	    }
	    else
		if(scopalign->N)
		    ok = ajTrue;
	    ajFileClose(&inf);
	    
	    
	    if(!ok)
	    {		
		ajWarn("Empty alignment in %S\n", inname);
		ajFmtPrintF(logf, "Empty alignment in %S\n", inname);
		if(scopalign)
		    ajDmxScopalgDel(&scopalign);
		if(seqset_a)
		    ajSeqsetDel(&seqset_a);
		if(seqin_a)
		    ajSeqinDel(&seqin_a);
		continue;
	    }		
	    
	    
	    /* Create clustal input alignment in CLUSTAL format. */
	    ajStrAssS(&clustin1,tmp_name);
	    ajStrAppC(&clustin1,".aln");
	    if((clustinf1 = ajFileNewOut(clustin1))==NULL)
		ajFatal("Could not open %S for writing\n", clustin1);
	    if(scopalign)
		ajDmxScopalgWriteFasta(scopalign,clustinf1);  
	    else
	    {
		for(x=0;x<ajSeqsetSize(seqset_a);++x)
		    ajFmtPrintF(clustinf1,">%S_%d\n%s\n",
				ajSeqsetName(seqset_a, x), x, 
				ajSeqsetSeq(seqset_a, x));
		ajFmtPrintF(clustinf1,"\n");
	    }
	    ajFileClose(&clustinf1);
	}
	/* Input sequences are singlets. */
	else if(modei==1) 
	{	
	    ok = ajFalse;
	    
	    if(!((hit_sing = ajDmxScophitReadFasta(inf))))
	    {
		ajWarn("embHitReadFasta call failed in seqsearch_psialigned for %F", inf);
	
		/* Read sequence set instead. */ 
		seq_sing    = ajSeqNew();
		seqin_sing  = ajSeqinNew();
		/* Set the filename via the USA. ajSeqsetRead interprets it to 
		   find the filename. */
		ajSeqinUsa(&seqin_sing, inname);
	
		if(!(ajSeqRead(seq_sing, seqin_sing)))
		    ajFatal("SeqRead failed in seqsearch_psialigned");
		if(MAJSTRLEN(ajSeqStr(seq_sing)))
		    ok = ajTrue;
	    }
	    else
		if(MAJSTRLEN(hit_sing->Seq))
		    ok = ajTrue;
	    ajFileClose(&inf);
	    
	    
	    if(!ok)
	    {		
		ajWarn("Empty singlet sequence in %S\n", inname);
		ajFmtPrintF(logf, "Empty singlet sequence for %S\n", inname);
		if(hit_sing)
		    ajDmxScophitDel(&hit_sing);
		if(seqset_a)
		    ajSeqDel(&seq_sing);
		if(seqin_a)
		    ajSeqinDel(&seqin_sing);
		continue;
	    }		
	}
	else
	    ajFatal("Unrecognised mode in SEQALIGN");
		
	    
	    
	/* Open domain hits file (input). */
	ajStrAssS(&hitsname, inname);
	ajFileDirExtnTrim(&hitsname);
	ajStrInsert(&hitsname, 0, ajDirName(dhfin));
	ajStrAppC(&hitsname, ".");
	ajStrApp(&hitsname, ajDirExt(dhfin));
	
	if(!(hitsinf = ajFileNewIn(hitsname)))
	{
	    ajWarn("Could not open domain hits file %S", hitsname);
	    ajFmtPrintF(logf, "Could not open domain hits file %S", hitsname);
	    if(scopalign)
		ajDmxScopalgDel(&scopalign);
	    if(seqset_a)
		ajSeqsetDel(&seqset_a);
	    if(seqin_a)
		ajSeqinDel(&seqin_a);
	    if(hit_sing)
		ajDmxScophitDel(&hit_sing);

	    continue;
	}
	else
	{
	    if(!ajFileReadLine(hitsinf, &line))
	    {	    
		ajWarn("Empty domain hits file %S", hitsname);
		ajFmtPrintF(logf, "Empty domain hits file %S", hitsname);
		if(scopalign)
		    ajDmxScopalgDel(&scopalign);
		if(seqset_a)
		    ajSeqsetDel(&seqset_a);
		if(seqin_a)
		    ajSeqinDel(&seqin_a);
		if(hit_sing)
		    ajDmxScophitDel(&hit_sing);
		continue;
	    }
	    else	/* rewind file */
		ajFileSeek(hitsinf, 0, 0);
	}
	

	
	/* Create Hitlist from domain hits file. */
	ok2=ajFalse;
	if((!(hitlist_h = embHitlistReadFasta(hitsinf))))
	{
	    /* Read sequence set instead. */ 
	    seqset_h = ajSeqsetNew();
	    seqin_h  = ajSeqinNew();
	    /* Set the filename via the USA. ajSeqsetRead interprets it to
	       find the filename. */
	    ajSeqinUsa(&seqin_h, hitsname);
	    
	    if(!(ajSeqsetRead(seqset_h, seqin_h)))
		ajFatal("SeqsetRead failed in seqsearch_psialigned");
	    
	    if(ajSeqsetSize(seqset_h) >= 1)
		ok2 = ajTrue;
	}
	else 
	    if((hitlist_h->N >= 1))
		ok2 = ajTrue;
	

	if(!ok2)
	{
	    ajWarn("Not enough sequences found in domain hits file");
	    ajFmtPrintF(logf, "Not enough sequences found in domain hits file");
	    if(scopalign)
		ajDmxScopalgDel(&scopalign);
	    if(seqset_a)
		ajSeqsetDel(&seqset_a);
	    if(seqin_a)
		ajSeqinDel(&seqin_a);
	    if(hitsinf)
		ajFileClose(&hitsinf);
	    if(hit_sing)
		ajDmxScophitDel(&hit_sing);
	    continue;
	}
	    

	/* Create file of clustal input sequences. */
	ajStrAssS(&clustin2,tmp_name);
	ajStrAppC(&clustin2,".seqs");
	clustinf2 = ajFileNewOut(clustin2);


	/* ajFmtPrint("hit_sing->Dom: %S\nhit_sing->Acc: %S\n", 
		   hit_sing->Dom, 	hit_sing->Acc); */
	


	/* Singlet sequence input rather than seed alignment. Add this 
	   sequence to the file of hits.  */
	if(modei==1) 
	{
	    if(hit_sing)
	    {
		
		if((MAJSTRLEN(hit_sing->Dom)))
		    /* The start and end may just be zero for domain sequences but 
		       include them for consistency with the other inputs */
		    ajFmtPrintF(clustinf2,">%S_%d_%d\n",
				hit_sing->Dom,
				hit_sing->Start,
				hit_sing->End);
		else if((MAJSTRLEN(hit_sing->Acc)))
		    ajFmtPrintF(clustinf2,">%S_%d_%d\n",
				hit_sing->Acc,
				hit_sing->Start,
				hit_sing->End);
		else	
		    ajFmtPrintF(clustinf2,">._%d_%d\n",
				hit_sing->Start,
				hit_sing->End);

		ajFmtPrintF(clustinf2,"%S\n",hit_sing->Seq);
	    }
	    else
	    {
		/* The '0' is just for consistency with the other inputs. */
		ajFmtPrintF(clustinf2,">%s_0\n",
			    ajSeqName(seq_sing));
		ajFmtPrintF(clustinf2,"%S\n", ajSeqStr(seq_sing));
	    }
	}
	

	if(hitlist_h)
	{
	    for(x=0;x<hitlist_h->N;x++)
	    {
		if((MAJSTRLEN(hitlist_h->hits[x]->Dom)))
		    ajFmtPrintF(clustinf2,">%S_%d_%d\n",
				hitlist_h->hits[x]->Dom,
				hitlist_h->hits[x]->Start,
				hitlist_h->hits[x]->End);
		else if((MAJSTRLEN(hitlist_h->hits[x]->Acc)))
		    ajFmtPrintF(clustinf2,">%S_%d_%d\n",
				hitlist_h->hits[x]->Acc,
				hitlist_h->hits[x]->Start,
				hitlist_h->hits[x]->End);
		else
		    ajFmtPrintF(clustinf2,">._%d_%d\n",
				hitlist_h->hits[x]->Start,
				hitlist_h->hits[x]->End);

		ajFmtPrintF(clustinf2,"%S\n",hitlist_h->hits[x]->Seq);
	    }
	}
	else
	{
	    for(x=0;x<ajSeqsetSize(seqset_h);++x)
	    {
		ajFmtPrintF(clustinf2,">%S_%d\n",
			    ajSeqsetName(seqset_h, x), x);
		ajFmtPrintF(clustinf2,"%S\n", ajSeqsetSeq(seqset_h, x));
	    }	
	}
	
	ajFileClose(&clustinf2);
	embHitlistDel(&hitlist_h);


	/* Call clustalw. */
	if(modei==2)
	{
	    /* Alignment is available: do profile to sequence mode. */
	    ajFmtPrintS(&cmd,"clustalw -type=protein -profile1=%S -sequences"
			" -profile2=%S -MATRIX=BLOSUM -GAPOPEN=10"
			" -GAPEXT=0.5 -outfile=%S\n",
			clustin1,clustin2,clustout);
	}	
	/* Such cases will now no longer occur ... but keep here for time
	   being. */
	else if(modei==1)
	{
	    /* Alignment is NOT available: multiple sequence mode. */
	    ajFmtPrintS(&cmd,"clustalw -infile=%S -align"
			" -MATRIX=BLOSUM -GAPOPEN=10"
			" -GAPEXT=0.5 -outfile=%S\n",
			clustin2,clustout);
	}	
	ajFmtPrint("\n%S\n", cmd);
	system(ajStrStr(cmd));
	    
	    
	/* Reformat output file into domain alignment format. */
	if((alg_in = ajFileNewIn(clustout))==NULL)
	    ajFatal("Could not read clustal output file %S", clustout);


    
	/* Open domain alignment file (output). */
	ajStrAssS(&outname, inname);
	ajFileDirExtnTrim(&outname);
	ajStrInsert(&outname, 0, ajDirName(dafout));
	ajStrAppC(&outname, ".");
	ajStrApp(&outname, ajDirExt(dafout));

	if((alg_out = ajFileNewOut(outname))==NULL)
	    ajFatal("Could not write clustal output file");


	/* Then write domain classification data. */
	if((modei==2))
	{
	    if(scopalign)
	    {
		if(scopalign->Type == ajSCOP)
		    ajFmtPrintF(alg_out,"# TY   SCOP\n# XX\n");
		else
		    ajFmtPrintF(alg_out,"# TY   CATH\n# XX\n");
		ajFmtPrintF(alg_out,"# CL   %S\n# XX\n",scopalign->Class);
		ajFmtPrintF(alg_out,"# FO   %S\n# XX\n",scopalign->Fold);
		ajFmtPrintF(alg_out,"# SF   %S\n# XX\n",scopalign->Superfamily);
		ajFmtPrintF(alg_out,"# FA   %S\n# XX\n",scopalign->Family);
		ajFmtPrintF(alg_out,"# SI   %d\n# XX\n",scopalign->Sunid_Family);
	    }
	    else if(forcetype)
		ajFmtPrintF(alg_out,"# TY   SCOP\n# XX\n");
	}
	else if(modei==1)
	{
	    if(hit_sing)
	    {
		if(hit_sing->Type == ajSCOP)
		    ajFmtPrintF(alg_out,"# TY   SCOP\n# XX\n");
		else
		    ajFmtPrintF(alg_out,"# TY   CATH\n# XX\n");
		ajFmtPrintF(alg_out,"# CL   %S\n# XX\n",hit_sing->Class);
		ajFmtPrintF(alg_out,"# FO   %S\n# XX\n",hit_sing->Fold);
		ajFmtPrintF(alg_out,"# SF   %S\n# XX\n",hit_sing->Superfamily);
		ajFmtPrintF(alg_out,"# FA   %S\n# XX\n",hit_sing->Family);
		ajFmtPrintF(alg_out,"# SI   %d\n# XX\n",hit_sing->Sunid_Family);
	    }
	    else if(forcetype)
		ajFmtPrintF(alg_out,"# TY   SCOP\n# XX\n");
	}	

	/* Then parse the clustal file and write the alignment. */
	while(ajFileReadLine(alg_in,&line))
	{
	    if(ajStrPrefixC(line, "CLUSTAL"))
		continue;
	    if(MAJSTRLEN(line)==0)
		continue;
	    if(line->Ptr[0]==' ')
		ajFmtPrintF(alg_out, "\n");
	    else
	    {
		/* Add bogus index numbers (of 0) to conform to EMBOSS simple
		   / srs format. */
		ajFmtScanS(line, "%S %S", &tmp1, &tmp2);
		if(MAJSTRLEN(tmp1)>25)
		    ajFatal("Code longer than permissible width in seqalign");
		ajFmtPrintF(alg_out, "%-25S 0 %S 0\n", tmp1, tmp2);
	    }
	    
	}
	
	ajFileClose(&alg_in);
	ajFileClose(&alg_out);
	if(hitsinf)
	    ajFileClose(&hitsinf);

	/* Clean up directory. */
	if(modei==2)
	    if(scopalign->N !=0)
		ajSysUnlink(clustin1);
	
	ajSysUnlink(clustin2);
	ajSysUnlink(clustout);
	ajSysUnlink(clustdnd);  
	
	
	/* Free memory. */
	if(scopalign)
	    ajDmxScopalgDel(&scopalign);
	if(seqset_a)
	    ajSeqsetDel(&seqset_a);
	if(seqin_a)
	    ajSeqinDel(&seqin_a);


	if(hit_sing) 
	    ajDmxScophitDel(&hit_sing);
	
	if(seq_sing)
	    ajSeqDel(&seq_sing);
	if(seqin_sing)
	    ajSeqinDel(&seqin_sing);


	ajStrDel(&inname);
    }


    /* Memory management. */    
    ajStrDel(&hitsname);
    ajStrDel(&line);
    ajStrDel(&cmd);
    ajListDel(&inseqs);
    ajDirDel(&dafout);
    ajStrDel(&tmp_name);
    ajStrDel(&clustin1);
    ajStrDel(&clustin2);
    ajStrDel(&clustout);
    ajStrDel(&clustdnd);
    ajFileClose(&logf);
    ajStrDel(&amode[0]);
    AJFREE(amode);
    ajStrDel(&mode[0]);
    AJFREE(mode);
    ajStrDel(&tmp1);
    ajStrDel(&tmp2);

    ajExit();
    return 0;
    
}







