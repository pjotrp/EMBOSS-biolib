/* @source scopseqs application
**
** Adds pdb and swissprot sequence records to a scop classification file.
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
**
**
**
**
******************************************************************************
**IMPORTANT NOTE      IMPORTANT NOTE      IMPORTANT NOTE        IMPORTANT NOTE     
******************************************************************************
**
** Mon May 20 11:43:39 BST 2002
**
** The following documentation is out-of-date and should be disregarded.  It 
** will be updated shortly. 
** 
******************************************************************************
**IMPORTANT NOTE      IMPORTANT NOTE      IMPORTANT NOTE        IMPORTANT NOTE     
******************************************************************************
**
**
**
**
**
** Operation
** 
** scopseqs parses the embl-like format scop classification file (Figure 1) generated by 
** the EMBOSS application scopparse, and writes in the same format a file to
** which domain sequence information derived from structure and sequence databases
** has been added (Figure 2). The format of the embl-like format scop classification file is 
** explained in the scopparse documentation - with the new records added by
** scopseqs described below.  Domain sequences are extracted from the clean domain 
** coordinate files generated by the EMBOSS application domainer, and from the 
** swissprot database.
**
** In order to find the start and end of a domain in the swissprot sequence, 
** scopseqs aligns the domain sequence from the clean coordinate file to the 
** full length protein sequence from swissprot.  Alignment is performed first by
** string handling and if that fails, by using the EMBOSS implementation of the 
** Needleman and Wunsch global alignment algorithm.  Gap insertion and extension 
** penalties of XXX and the XXX residue substitution matrix are used in the alignments.
**
**
** Figure 1  Excerpt from embl-like format scop classification file (input)
**
**  ID   D3SDHA_
**  XX
**  EN   3SDH
**  XX
**  OS   Ark clam (Scapharca inaequivalvis)
**  XX
**  CL   All alpha proteins
**  XX
**  FO   Globin-like
**  XX
**  SF   Globin-like
**  XX
**  FA   Globins
**  XX
**  DO   Hemoglobin I
**  XX
**  NC   1
**  XX
**  CN   [1]
**  XX
**  CH   a CHAIN; . START; . END;
**  //
**
**
** Figure 2  Excerpt from embl-like format scop classification file (output)
**
**  ID   D3SDHA_
**  XX
**  EN   3SDH
**  XX
**  OS   Ark clam (Scapharca inaequivalvis)
**  XX
**  CL   All alpha proteins
**  XX
**  FO   Globin-like
**  XX
**  SF   Globin-like
**  XX
**  FA   Globins
**  XX
**  DO   Hemoglobin I
**  XX
**  NC   1
**  XX
**  CN   [1]
**  XX
**  CH   a CHAIN; . START; . END;
**  XX 
**  DS   SEQUENCE   78 AA;  15127 MW;  5EC7DB1E CRC32; 
**       VLSPADKTNV KAAWGKVGAH AGEYGAEALE RMFLSFPTTK TYFPHFDLSH GSAQVKGHGK 
**       KVADALTNAV AHVDDMPNA  
**  XX
**  AC   P00432
**  XX
**  RA   2 START; 79 END; 
**  XX 
**  SQ   SEQUENCE   78 AA;  15127 MW;  5EC7DB1E CRC32; 
**       VLSPADKTNV KAAWGKVGAH AGEYGAEALE RMFLSFPTTK TYFPHFDLSH GSAQVKGHGK 
**       KVADALTNAV AHVDDMPNA
**  XX
**  //
**
** Important Note
**
*******************************************************************************/




#include "emboss.h"


static AjBool scopseqs_AlignDomain(AjPSeq sp_seq, AjPSeq pdb_seq, 
				AjPMatrixf matrix, float gapopen, 
				float gapextend, ajint* start, 
				ajint* end);

static AjBool scopseqs_FindDomainLimits(AjPSeq sp_seq, AjPSeq pdb_seq, 
				     AjPStr m, AjPStr n, ajint start1, 
				     ajint start2, float gapopen, 
				     float gapextend, float score, 
				     AjPMatrixf matrix, ajint begina, 
				     ajint beginb, ajint* start, ajint* end);


/* @prog scopseqs ************************************************************
**
** Test
**
******************************************************************************/

int main(int argc, char **argv)
{
    ajint      start      = 0;    /* Start of the domain in the swissprot 
				     sequence */
    ajint      end        = 0;    /* End of the domain inthe swissprot 
				     sequence */
    
    float      gapopen;		  /* Gap insertion penalty*/
    float      gapextend;	  /* Gap extension penalty*/
 
    AjPStr     msg        = NULL; /* String used for messages */
    AjPStr     dpdb_path  = NULL; /* Path of dpdb (clean domain) files */
    AjPStr     dpdb_name  = NULL; /* Name of dpdb (clean domain) file */
    AjPStr     dpdb_extn  = NULL; /* Extension of dpdb (clean domain) file */
    AjPStr     dpdb_seq   = NULL; /* Holds the clean pdb sequence */
    AjPStr     acc        = NULL; /* A string to hold an accession number */
    AjPStr     db         = NULL; /* The name of the sequence database to 
				     retrieve the sequences */
    AjPStr     sp_dom_seq = NULL; /* The swissprot domain sequence */

    AjPFile    dpdb_inf   = NULL; /* File pointer for clean domain files */
    AjPFile    scop_inf   = NULL; /* File pointer to Escop.dat */
    AjPFile    pdbtosp_inf= NULL; /* File pointer to Epdbtosp.dat */
    AjPFile    scop_outf  = NULL; /* File pointer for scop output file */
    AjPFile    errf       = NULL; /* Output log file */
    
    AjPList    list       = NULL; /* A list to hold pdbtosp structures */
    
    AjPSeq     sp_seq     = NULL; /* Sequence object to hold the retrieved 
				     swissprot sequence */
    AjPSeq     pdb_seq    = NULL; /* Sequence object to hold the retrieved 
				     dpdb sequence */
     
    AjPMatrixf matrix;		  /* Substitution matrix*/
    AjPScop    scop       = NULL; /* Pointer to scop structure */
    AjPPdb     pdb        = NULL; /* Pointer to pdb structure */

    
    
    /* Initialise strings, lists,  etc */
    msg        = ajStrNew();
    dpdb_path  = ajStrNew();
    dpdb_name  = ajStrNew();
    dpdb_extn  = ajStrNew();
    dpdb_seq   = ajStrNew();
    acc        = ajStrNew();
    db         = ajStrNew();
    sp_dom_seq = ajStrNew();

    list       = ajListNew();


    /* Read data from acd */
    embInit("scopseqs",argc,argv);

    scop_inf    = ajAcdGetInfile("scopin");
    pdbtosp_inf = ajAcdGetInfile("pdbtosp");
    scop_outf   = ajAcdGetOutfile("scopout");
    dpdb_path   = ajAcdGetString("dpdb");
    dpdb_extn   = ajAcdGetString("extn");
    matrix      = ajAcdGetMatrixf("datafile");
    gapopen     = ajAcdGetFloat("gapopen");
    gapextend   = ajAcdGetFloat("gapextend");
    errf        = ajAcdGetOutfile("errf");


    /* Check directory*/
    if(!ajFileDir(&dpdb_path))
	ajFatal("Could not open dpdb directory");



    /* set up the pdbtosp object list */
    ajXyzPdbtospReadAll(pdbtosp_inf,&list);
   
    /* Start of main application loop */
    while((ajXyzScopReadC(scop_inf, "*", &scop)))
    {
	ajStrAssS(&dpdb_name,scop->Entry);
	ajStrToLower(&dpdb_name);
	ajStrApp(&dpdb_name,dpdb_extn);

	ajFmtPrintF(errf, "//\n%-15S\n", scop->Entry);  
	

	if(!(dpdb_inf=ajFileNewDF(dpdb_path, dpdb_name)))
	{
	    ajFmtPrintF(errf, "%-15s\n", "FILE_OPEN");  
	    ajFmtPrintS(&msg, "Could not open dpdb file %S", dpdb_name);
	    ajWarn(ajStrStr(msg));
	    ajXyzScopDel(&scop);
	    continue;
	}
	
	/*Read the coordinate file for the domain*/
	if(!ajXyzCpdbRead(dpdb_inf, &pdb))
	{
	    ajFmtPrintF(errf, "%-15s\n", "FILE_READ");  
	    ajFmtPrintS(&msg, "Error reading dpdb file %S", dpdb_name);
	    ajWarn(ajStrStr(msg));
	    ajFileClose(&dpdb_inf);
	    ajXyzScopDel(&scop);
	    ajXyzPdbDel(&pdb);
	    continue;
	}
		
	/* get and assign the pdb sequence */
	ajStrAssS(&dpdb_seq, pdb->Chains[0]->Seq);
	
	/* create a pdb sequence object */
	pdb_seq = ajSeqNew();
	ajStrAssS(&pdb_seq->Name,dpdb_name);
	ajStrAssS(&pdb_seq->Seq,dpdb_seq);
	
	/* add pdb sequence to the scop structure */
	ajStrAssS(&scop->SeqPdb, dpdb_seq);

	/* given a pdb code get the accession number an list of pdbtoscop 
	   structures */
	if(!ajXyzPdbToAcc(scop->Pdb,&acc,list))
	{
	    /*JISON added tidy up and log file code*/
	    ajWarn("No accession number found for domain %S\n",dpdb_name);
	    ajFmtPrintF(errf, "%-15s\n", "NO_ACCESSION_NUMBER");  
	    ajXyzScopWrite(scop_outf,scop);
	    ajFileClose(&dpdb_inf);
	    ajXyzScopDel(&scop);
	    ajXyzPdbDel(&pdb);
	    ajSeqDel(&pdb_seq);
	    continue;
	}

	/* add the accession number to the scop structure */
	ajStrAssS(&scop->Acc,acc);


	/* get the swissprot sequence */
	sp_seq = ajSeqNew();

	ajStrAssC(&db, "swissprot-acc:");
	ajStrApp(&db, acc);

	if(!(ajSeqGetFromUsa(db,ajTrue,&sp_seq)))
        {
            ajFmtPrintF(errf, "%-15s\n", "ENTRY_NOT_FOUND_IN_SW");
            ajXyzScopWrite(scop_outf,scop);
	    ajFileClose(&dpdb_inf);
	    ajXyzScopDel(&scop);
	    ajXyzPdbDel(&pdb);
	    ajSeqDel(&pdb_seq);
	    ajSeqDel(&sp_seq);
            continue;
        }
        else
            ajStrAssS(&scop->Spr,sp_seq->Name);

	ajStrAssS(&scop->Spr,sp_seq->Name);
	
	/***************************************************************************
	 * find the swissprot sequence segment that is the same as the pdb sequence.
	 ***************************************************************************/
	
	/* first look for identical match */
	if((ajStrFind(sp_seq->Seq,dpdb_seq))>=0)
	{
	    start = ajStrFind(sp_seq->Seq,dpdb_seq);
	    end = start + dpdb_seq->Len - 1;
	    scop->Startd = start+1;
	    scop->Endd = end+1;
	}	
	
	/* Carry out Needle_Wunch pairwise alignment to find the 
	   location of domain */
	else if (scopseqs_AlignDomain(sp_seq, pdb_seq, matrix, 
				   gapopen, gapextend, &start, &end))
	{
	    ajFmtPrintF(errf, "%-15s\n", "ALIGNMENT_NECESSARY"); 

	    scop->Startd = (start+1);
	    scop->Endd = end;
	}
	
	else
	    ajFatal("Fatal failure in scopseqs_AlignDomain\n");

	/* get the domain sequence from the swissprot sequence trim it and write 
	   to scop structure */
	ajStrAssSub(&sp_dom_seq, sp_seq->Seq, start, end);
	ajStrAssS(&scop->SeqSpr,sp_dom_seq);

	/*write out the scop structure */
	ajXyzScopWrite(scop_outf,scop);

	/* clean up */
	start = 0;
	end   = 0;
	
	ajFileClose(&dpdb_inf);
	ajSeqDel(&sp_seq);
	ajSeqDel(&pdb_seq);
	/*JISON*/
	ajXyzPdbDel(&pdb);
	ajXyzScopDel(&scop);
    }
    

    
    /* Tidy up*/
    ajMatrixfDel(&matrix);
    ajStrDel(&msg);
    ajStrDel(&dpdb_path);
    ajStrDel(&dpdb_name);
    ajStrDel(&dpdb_extn);
    ajStrDel(&dpdb_seq);
    ajStrDel(&acc);
    ajFileClose(&errf);   
    ajFileClose(&scop_inf);
    ajFileClose(&pdbtosp_inf);
    ajFileClose(&scop_outf);	    
    

    /* Bye Bye */
    ajExit();
    return 0;
}

/* @funcstatic scopseqs_AlignDomain ***********************************************
**
** Align the pdb sequence to the full length swissprot sequence using Needleman 
** and Wunsch (adapted to find location (start & end) of pdb sequence in swissprot
** sequence).
**
** @param [r] sp_seq    [AjPSeq]     Swissprot sequence object 
** @param [r] pdb_seq   [AjPSeq]     Pdb sequence object 
** @param [r] matrix    [AjPMatrixf] Residue substitution matrix
** @param [r] gapopen   [float]      Gap insertion penalty
** @param [r] gapextend [float]      Gap extension penalty
** @param [w] start     [int *]      Start of domain in swissprot sequence
** @param [w] end       [int *]      End of domain in swissprot sequence
**
** @return [AjBool] ajTrue on success
** @@
*******************************************************************************/
static AjBool scopseqs_AlignDomain(AjPSeq sp_seq, AjPSeq pdb_seq, AjPMatrixf matrix, 
				float gapopen, float gapextend, ajint* start, 
				ajint* end)
{

    ajint         start1  = 0;	  /*Start of seq 1, passed as arg but not used*/
    ajint         start2  = 0;	  /*Start of seq 2, passed as arg but not used*/
    ajint         maxarr  = 300;  /*Initial size for matrix*/
    ajint         len;
    ajint         lena;
    ajint         lenb;
    ajint        *compass;

    ajint        begina;
    ajint        beginb;

    char         *p;
    char         *q;

    float        **sub;
    float        id       = 0.;	  /*Passed as arg but not used here*/
    float        sim      = 0.;	
    float        idx      = 0.;	  /*Passed as arg but not used here*/
    float        simx     = 0.;	  /*Passed as arg but not used here*/
    float        *path;
    float        score;    

    AjPStr       m        = NULL; /*Passed as arg but not used here*/
    AjPStr       n        = NULL; /*Passed as arg but not used here*/

    AjPSeqCvt   cvt       = 0;
    AjBool      show      = ajFalse; /*Passed as arg but not used here*/


    /* Check args */
    if(!sp_seq || !pdb_seq || !matrix || !start || !end)
    {
	ajWarn("Bad args passed to scopseqs_AlignDomain");
	return ajFalse;
    }
    
    
    /*Intitialise some variables*/
    AJCNEW(path, maxarr);
    AJCNEW(compass, maxarr);

    m = ajStrNew();    
    n = ajStrNew();    

    gapopen   = ajRoundF(gapopen,8);
    gapextend = ajRoundF(gapextend,8);

    sub = ajMatrixfArray(matrix);
    cvt = ajMatrixfCvt(matrix);


    begina  = ajSeqBegin(sp_seq)+ajSeqOffset(sp_seq);

    lena = ajSeqLen(sp_seq);
    ajSeqTrim(pdb_seq);
    lenb = ajSeqLen(pdb_seq);
    len  = ajSeqLen(sp_seq)*ajSeqLen(pdb_seq);
    
    if(len>maxarr)
    {
	AJCRESIZE(path,len);
	AJCRESIZE(compass,len);
	maxarr=len;
    }
    
    beginb  = ajSeqBegin(pdb_seq)+ajSeqOffset(pdb_seq);

    p = ajSeqChar(sp_seq); 
    q = ajSeqChar(pdb_seq); 
    
    ajStrAssC(&m,"");
    ajStrAssC(&n,"");
    
    
    /* Call alignment functions */
    embAlignPathCalc(p,q,lena,lenb, gapopen,gapextend,path,sub,cvt,
		     compass,show);

    score =  embAlignScoreNWMatrix(path,sp_seq,pdb_seq,sub,cvt,lena, 
				   lenb,gapopen,compass,gapextend,
				   &start1,&start2);

    embAlignWalkNWMatrix(path,sp_seq,pdb_seq,&m,&n,lena,lenb,&start1,
			 &start2,gapopen,gapextend,cvt,compass,sub);

    embAlignCalcSimilarity(m,n,sub,cvt,lena,lenb,&id,&sim,&idx, &simx);

    /* calculate the start and end of the alignment relative to the 
       swissprot numbering */
    scopseqs_FindDomainLimits(sp_seq, pdb_seq, m, n, start1, start2,	
			   gapopen, gapextend, score, matrix, begina,
			   beginb, start, end);
   
    
    
    /* Tidy up */
    AJFREE(compass);
    AJFREE(path);
    ajStrDel(&m);
    ajStrDel(&n);
    
    /* Bye Bye */
    return ajTrue;
}    


/* @funcstatic scopseqs_FindDomainLimits **************************************
**
** Calculates the start and end of the alignment relative to the swissprot 
** numbering.
**
** @param [r] sp_seq    [AjPSeq] Completefirst sequence
** @param [r] pdb_seq   [AjPSeq] Complete second sequence
** @param [r] m         [AjPStr] Walk alignment for first sequence
** @param [r] n         [AjPStr] Walk alignment for second sequence
** @param [r] start1    [ajint]  Start of alignment in first sequence
** @param [r] start2    [ajint]  Start of alignment in second sequence
** @param [r] gapopen   [float]  Gap open penalty to report
** @param [r] gapextend [float]  Gap extend penalty to report
** @param [r] score     [float]  Alignment score from AlignScoreX
** @param [r] matrix    [AjPMatrixf] Floating point matrix
** @param [r] begina    [ajint]  first sequence offset
** @param [r] beginb    [ajint]  Second sequence offset
** @param [w] start     [ajint*] The start of the alignment (domain) in the 
**				 swissprot sequence.
** @param [w] end       [ajint*] The end of the alignment (domain) in the 
**			         swissprot sequence.
** @return [AjBool]
*******************************************************************************/

static AjBool scopseqs_FindDomainLimits(AjPSeq sp_seq, AjPSeq pdb_seq,AjPStr m, 
				     AjPStr n,	ajint start1, ajint start2, 
				     float gapopen, float gapextend, float score, 
				     AjPMatrixf matrix, ajint begina, 
				     ajint beginb, ajint* start, ajint* end)
{ 
    AjPSeq res1       = NULL;
    AjPSeq res2       = NULL;

    ajint end1;
    ajint end2;

    AjPStr fa         = NULL;
    AjPStr fb         = NULL;
    AjPSeqset seqset  = NULL;

    ajint maxlen;
    ajint i;
    ajint alen;
    ajint blen;
    ajint apos;
    ajint bpos;
    ajint nc;

    char* a;
    char* b;

    ajint no_gaps_sp  = 0;   /* the number of gaps in the swissprot sequence */
    ajint no_gaps_pdb = 0;   /* the number of gaps in the pdb sequence */
    
    ajDebug("embAlignReportGlobal %d %d\n", start1, start2);
    ajDebug("  sp_seq: '%S' \n", ajSeqStr(sp_seq));
    ajDebug("  pdb_seq: '%S' \n", ajSeqStr(pdb_seq));
    ajDebug("  alim: '%S' \n", m);
    ajDebug("  alin: '%S' \n", n);

    maxlen = AJMAX(ajSeqLen(sp_seq), ajSeqLen(pdb_seq));

    seqset = ajSeqsetNew();
    res1 = ajSeqNew();
    res2 = ajSeqNew();

    ajSeqAssName (res1, ajSeqGetName(sp_seq));
    ajSeqAssName (res2, ajSeqGetName(pdb_seq));
    ajSeqAssUsa (res1, ajSeqGetUsa(sp_seq));
    ajSeqAssUsa (res2, ajSeqGetUsa(pdb_seq));

    a = ajSeqChar(sp_seq);
    b = ajSeqChar(pdb_seq);
    
    /* generate the full aligned sequences */
    ajStrModL (&fa, maxlen);
    ajStrModL (&fb, maxlen);

    /* pad the start of either sequence */
    if(start1>start2)
    {
	for(i=0;i<start1;++i)
	{
	    (void) ajStrAppK(&fa,a[i]);
	}
	nc=start1-start2;
	for(i=0;i<nc;++i) (void) ajStrAppK(&fb,' ');
	for(++nc;i<start1;++i) (void) ajStrAppK(&fb,b[i-nc]);
	*start = start1;
	
    }
    else if(start2>start1)
    {
	for(i=0;i<start2;++i)
	{
	    (void) ajStrAppK(&fb,b[i]);
	}
	nc=start2-start1;
	for(i=0;i<nc;++i) (void) ajStrAppK(&fa,' ');
	for(++nc;i<start2;++i) (void) ajStrAppK(&fa,a[i-nc]);
	*start = 0;
    }

    apos = start1 + ajStrLen(m) - ajSeqGapCountS(m);
    bpos = start2 + ajStrLen(n) - ajSeqGapCountS(n);

    ajStrApp(&fa, m);
    ajStrApp(&fb, n);

    alen=ajSeqLen(sp_seq) - apos;
    blen=ajSeqLen(pdb_seq) - bpos;

    ajDebug("alen: %d blen: %d apos: %d bpos: %d\n", alen, blen, apos, bpos);

    if(alen>blen)
    {
	(void) ajStrAppC(&fa,&a[apos]);
	for(i=0;i<blen;++i)
	{
	    (void) ajStrAppK(&fb,b[bpos+i]);
	}	
	nc=alen-blen;
	for(i=0;i<nc;++i)
	{
	    (void) ajStrAppC(&fb," ");
	}
    }	
    
    else if(blen>alen)
    {
	(void) ajStrAppC(&fb,&b[bpos]);
	for(i=0;i<alen;++i)
	{
	    (void) ajStrAppK(&fa,a[apos+i]);
	}
	nc=blen-alen;
	for(i=0;i<nc;++i)
	{
	    (void) ajStrAppC(&fa," ");
	}
    }

    /* same length, just copy */
    else			
    {
	(void) ajStrAppC(&fa,&a[apos]);
	(void) ajStrAppC(&fb,&b[bpos]);
    }	

    ajDebug("  res1: %5d '%S' \n", ajStrLen(fa), fa);
    ajDebug("  res2: %5d '%S' \n", ajStrLen(fb), fb);
    ajSeqAssSeq (res1, fa);
    ajSeqAssSeq (res2, fb);

    ajSeqsetFromPair (seqset, res1, res2);

    end1 = start1 - ajStrCountK(m, '-') + ajStrLen(m);
    end2 = start2 - ajStrCountK(n, '-') + ajStrLen(n);

    /********************************************************************
     ** Find the domain boundries form the sequence alignment
     *******************************************************************/

    no_gaps_sp  = (alen + (end1 - ajStrLen(sp_seq->Seq)));
    no_gaps_pdb = (blen + (end2 - ajStrLen(pdb_seq->Seq)));
    
    *end = (((*start + (end2-start2)) - no_gaps_sp));
   

    ajStrDel(&fa);
    ajStrDel(&fb);
    ajSeqsetDel(&seqset);
    ajSeqDel(&res1);
    ajSeqDel(&res2);

    return ajTrue;
}








