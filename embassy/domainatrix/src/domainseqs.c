/* @source domainseqs application
**
** Adds sequence records to a DCF file (domain classification file).
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
**  DOMAINSEQS documentation
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
******************************************************************************/





#include "emboss.h"






/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static AjBool domainseqs_AlignDomain(AjPSeq sp_seq, 
				     AjPSeq pdb_seq, 
				     AjPMatrixf matrix, 
				     float gapopen, 
				     float gapextend,
				     ajint* start, 
				     ajint* end);

static AjBool domainseqs_FindDomainLimits(AjPSeq sp_seq,
					  AjPSeq pdb_seq, 
					  AjPStr m,
					  AjPStr n,
					  ajint start1, 
					  ajint start2,
					  float gapopen, 
					  float gapextend,
					  float score, 
					  AjPMatrixf matrix,
					  ajint begina, 
					  ajint beginb, 
					  ajint* start, 
					  ajint* end);





/* @prog domainseqs ***********************************************************
**
**  Adds sequence records to a DCF file (domain classification file).
**
******************************************************************************/

int main(int argc, char **argv)
{
    ajint      start      = 0;    /* Start of the domain in the swissprot 
				     sequence.                               */
    ajint      end        = 0;    /* End of the domain inthe swissprot 
				     sequence.                               */
    
    float      gapopen    = 0.0;  /* Gap insertion penalty.                  */
    float      gapextend  = 0.0;  /* Gap extension penalty.                  */
 
    AjPStr     msg        = NULL; /* String used for messages.               */
    AjPDir     dpdb_path  = NULL; /* Path of dpdb (clean domain) files.      */
    AjPStr     dpdb_name  = NULL; /* Name of dpdb (clean domain) file.       */
    AjPStr     dpdb_seq   = NULL; /* Holds the clean pdb sequence.           */
    AjPStr     acc        = NULL; /* A string to hold an accession number.   */
    AjPStr     db         = NULL; /* The name of the sequence database to 
				     retrieve the sequences.                 */
    AjPStr     sp_dom_seq = NULL; /* The swissprot domain sequence.          */

    AjPFile    dpdb_inf   = NULL; /* File pointer for clean domain files.    */
    AjPFile    domain_inf = NULL; /* File pointer to DCF file.               */
    AjPFile    pdbtosp_inf= NULL; /* File pointer to Epdbtosp.dat.           */
    AjPFile    domain_outf= NULL; /* File pointer for DCF output file.       */
    AjPFile    errf       = NULL; /* Output log file.                        */
    
    AjPList    list       = NULL; /* A list to hold pdbtosp structures.      */
    
    AjPSeq     sp_seq     = NULL; /* Sequence object to hold the retrieved 
				     swissprot sequence.                     */
    AjPSeq     pdb_seq    = NULL; /* Sequence object to hold the retrieved 
				     dpdb sequence.                          */
     
    AjPMatrixf matrix;		  /* Substitution matrix.                    */
    AjPDomain  domain     = NULL; /* Pointer to domain structure.            */
    AjPPdb     pdb        = NULL; /* Pointer to pdb structure.               */

    AjBool     getswiss   = ajFalse; /* Whether to retrieve swissprot 
				       sequences for the domains.            */
    
    ajint      type       = 0;   /* Type of domain (ajSCOP or ajCATH) in the 
				    DCF file.                                */

    
    /* Initialise strings, lists,  etc */
    msg        = ajStrNew();
    dpdb_name  = ajStrNew();
    dpdb_seq   = ajStrNew();
    acc        = ajStrNew();
    db         = ajStrNew();
    sp_dom_seq = ajStrNew();



    /* Read data from acd */
    embInitP("domainseqs",argc,argv,"DOMAINATRIX");

    domain_inf    = ajAcdGetInfile("dcfinfile");
    getswiss    = ajAcdGetToggle("getswiss");
    if(getswiss)
    {
	pdbtosp_inf = ajAcdGetInfile("pdbtospfile");
	matrix      = ajAcdGetMatrixf("datafile");
	gapopen     = ajAcdGetFloat("gapopen");
	gapextend   = ajAcdGetFloat("gapextend");
    }
    domain_outf   = ajAcdGetOutfile("dcfoutfile");
    dpdb_path   = ajAcdGetDirectory("dpdbdir");
    errf        = ajAcdGetOutfile("logfile");



    /* Set up the pdbtosp object list. */
    if(getswiss)
	list = ajPdbtospReadAllNew(pdbtosp_inf);
   
    type = ajDomainDCFType(domain_inf);

    /* Start of main application loop. */
    while((domain=(ajDomainReadCNew(domain_inf, "*", type))))
    {
	ajStrAssignS(&dpdb_name,ajDomainGetId(domain));
	ajStrFmtLower(&dpdb_name);

	ajFmtPrintF(errf, "//\n%-15S\n", ajDomainGetId(domain));  
	ajFmtPrint("//\n%-15S\n", ajDomainGetId(domain));  
	

	if(!(dpdb_inf=ajFileNewDirF(dpdb_path, dpdb_name)))
	{
	    ajStrAssignS(&msg, dpdb_name);
	    ajStrAppendC(&msg, ".");
	    ajStrAppendS(&msg,ajDirExt(dpdb_path));
	    ajFmtPrintF(errf, "%-15s\n", "FILE_OPEN");  
	    ajWarn("Could not open ccf file %S", msg);
	    ajDomainDel(&domain);
	    continue;
	}
	
	/* Read the coordinate file for the domain. */
	if((!(pdb=ajPdbReadFirstModelNew(dpdb_inf))))
	{
	    ajFmtPrintF(errf, "%-15s\n", "FILE_READ");  
	    ajFmtPrintS(&msg, "Error reading ccf file %S", dpdb_name);
	    ajWarn(ajStrGetPtr(msg));
	    ajFileClose(&dpdb_inf);
	    ajDomainDel(&domain);
	    ajPdbDel(&pdb);
	    continue;
	}
		
	/* Get and assign the pdb sequence. */
	ajStrAssignS(&dpdb_seq, pdb->Chains[0]->Seq);
	
	/* Create a pdb sequence object. */
	pdb_seq = ajSeqNew();
	ajStrAssignS(&pdb_seq->Name,dpdb_name);
	ajStrAssignS(&pdb_seq->Seq,dpdb_seq);
	
	/* Add pdb sequence to the domain structure. */
	if((domain->Type == ajSCOP))
	    ajStrAssignS(&domain->Scop->SeqPdb, dpdb_seq);
	else
	    ajStrAssignS(&domain->Cath->SeqPdb, dpdb_seq);


	
	/* If we do not need the swissprot sequence. */
	if(!getswiss)
	{
	    ajDomainWrite(domain_outf,domain); 
	    ajFileClose(&dpdb_inf);  
	    ajSeqDel(&pdb_seq);
	    ajPdbDel(&pdb);
	    ajDomainDel(&domain);
	    continue;
	}
	

	/* We only want to retrieve a swissprot sequence if the domain is not 
	   comprised of segments. */
	if(ajDomainGetN(domain) != 1)
	{
	    ajWarn("Will not retrieve swissprot sequence for segmented domain %S\n",
		   dpdb_name);
	    ajFmtPrintF(errf, "%-15s\n", "SEGMENTED_DOMAIN_NO_SP_SEQ");  
	    ajDomainWrite(domain_outf,domain);
	    ajFileClose(&dpdb_inf);
	    ajDomainDel(&domain);
	    ajPdbDel(&pdb);
	    ajSeqDel(&pdb_seq);
	    continue;
	}
	
	/* Given a pdb code get the accession number and list of pdbtoscop 
	   structures. */
	if(!embPdbidToAcc(ajDomainGetPdb(domain),&acc,list))
	{
	    ajFmtPrintF(errf, "%-15s\n", "NO_ACCESSION_NUMBER");  
	    ajDomainWrite(domain_outf,domain);
	    ajFileClose(&dpdb_inf);
	    ajDomainDel(&domain);
	    ajPdbDel(&pdb);
	    ajSeqDel(&pdb_seq);
	    continue;
	}

	/* Add the accession number to the domain structure. */
	if((domain->Type == ajSCOP))
	    ajStrAssignS(&domain->Scop->Acc,acc);
	else
	    ajStrAssignS(&domain->Cath->Acc,acc);

	/* Get the swissprot sequence. */
	sp_seq = ajSeqNew();

	ajStrAssignC(&db, "swissprot-acc:");
	ajStrAppendS(&db, acc);

	if(!(ajSeqGetFromUsa(db,ajTrue,&sp_seq)))
        {
            ajFmtPrintF(errf, "%-15s\n", "ENTRY_NOT_FOUND_IN_SW");
            ajDomainWrite(domain_outf,domain);
	    ajFileClose(&dpdb_inf);
	    ajDomainDel(&domain);
	    ajPdbDel(&pdb);
	    ajSeqDel(&pdb_seq);
	    ajSeqDel(&sp_seq);
            continue;
        }
        else
	{
	    if((domain->Type == ajSCOP))
		ajStrAssignS(&domain->Scop->Spr,sp_seq->Name);
	    else
		ajStrAssignS(&domain->Cath->Spr,sp_seq->Name);
	}
	
	if((domain->Type == ajSCOP))
	    ajStrAssignS(&domain->Scop->Spr,sp_seq->Name);
	else
	    ajStrAssignS(&domain->Cath->Spr,sp_seq->Name);

	/* Find swissprot sequence segment corresponding to pdb sequence. */
	
	/* First look for identical match. */
	if((ajStrFindS(sp_seq->Seq,dpdb_seq))>=0)
	{
	    start = ajStrFindS(sp_seq->Seq,dpdb_seq);
	    end = start + dpdb_seq->Len - 1;

	    if((domain->Type == ajSCOP))
	    {
		domain->Scop->Startd = start+1;
		domain->Scop->Endd = end+1;
	    }
	    else
	    {
		domain->Cath->Startd = start+1;
		domain->Cath->Endd = end+1;
	    }
	}	
	
	/* Carry out Needle_Wunch pairwise alignment to find the location of
	   domain. */
	else if (domainseqs_AlignDomain(sp_seq, pdb_seq, matrix, 
				   gapopen, gapextend, &start, &end))
	{
	    ajFmtPrintF(errf, "%-15s\n", "ALIGNMENT_NECESSARY"); 

	    if((domain->Type == ajSCOP))
	    {
		domain->Scop->Startd = (start+1);
		domain->Scop->Endd = end;
	    }
	    else
	    {
		domain->Cath->Startd = (start+1);
		domain->Cath->Endd = end;
	    }
	}
	
	else
	    ajFatal("Fatal failure in domainseqs_AlignDomain\n");

	/* Get the domain sequence from the swissprot sequence trim it and 
	   write to domain structure. */
	ajStrAssignSubS(&sp_dom_seq, sp_seq->Seq, start, end);

	if((domain->Type == ajSCOP))
	    ajStrAssignS(&domain->Scop->SeqSpr,sp_dom_seq);
	else
	    ajStrAssignS(&domain->Cath->SeqSpr,sp_dom_seq);

	/* write out the domain structure. */
	ajDomainWrite(domain_outf,domain);

	/* Memory management. */
	start = 0;
	end   = 0;
	
	ajFileClose(&dpdb_inf);
	ajSeqDel(&sp_seq);
	ajSeqDel(&pdb_seq);
	ajPdbDel(&pdb);
	ajDomainDel(&domain);
    }
    

    
    /* Memory management.  */
    ajDirDel(&dpdb_path);
    ajMatrixfDel(&matrix);
    ajStrDel(&msg);
    ajStrDel(&dpdb_name);
    ajStrDel(&dpdb_seq);
    ajStrDel(&acc);
    ajFileClose(&errf);   
    ajFileClose(&domain_inf);
    if(getswiss)
	ajFileClose(&pdbtosp_inf);
    ajFileClose(&domain_outf);	    
    

    ajExit();
    return 0;
}

/* @funcstatic domainseqs_AlignDomain *****************************************
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
******************************************************************************/
static AjBool domainseqs_AlignDomain(AjPSeq sp_seq,
				     AjPSeq pdb_seq, 
				     AjPMatrixf matrix, 
				     float gapopen,
				     float gapextend, 
				     ajint* start, 
				     ajint* end)
{

    ajint      start1  = 0;	    /* Start of seq 1, passed but not used. */
    ajint      start2  = 0;	    /* Start of seq 2, passed but not used. */
    ajint      maxarr  = 300;       /* Initial size for matrix. */
    ajint      len;
    ajint      lena;
    ajint      lenb;
    ajint     *compass;

    ajint      begina;
    ajint      beginb;

    const char *p;
    const char *q;

    float    **sub;
    float      id       = 0.;	    /* Passed as arg but not used here. */
    float      sim      = 0.;	
    float      idx      = 0.;	    /* Passed as arg but not used here. */
    float      simx     = 0.;	    /* Passed as arg but not used here. */
    float     *path;
    float      score;    
    
    AjPStr     m        = NULL;     /* Passed as arg but not used here. */
    AjPStr     n        = NULL;     /* Passed as arg but not used here. */

    AjPSeqCvt  cvt       = 0;
    AjBool     show      = ajFalse; /* Passed as arg but not used here. */


    /* Check args. */
    if(!sp_seq || !pdb_seq || !matrix || !start || !end)
    {
	ajWarn("Bad args passed to domainseqs_AlignDomain");
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


    begina  = ajSeqGetBegin(sp_seq)+ajSeqGetOffset(sp_seq);

    lena = ajSeqGetLen(sp_seq);
    ajSeqTrim(pdb_seq);
    lenb = ajSeqGetLen(pdb_seq);
    len  = ajSeqGetLen(sp_seq)*ajSeqGetLen(pdb_seq);
    
    if(len>maxarr)
    {
	AJCRESIZE(path,len);
	AJCRESIZE(compass,len);
	maxarr=len;
    }
    
    beginb  = ajSeqGetBegin(pdb_seq)+ajSeqGetOffset(pdb_seq);

    p = ajSeqGetSeqC(sp_seq); 
    q = ajSeqGetSeqC(pdb_seq); 
    
    ajStrAssignC(&m,"");
    ajStrAssignC(&n,"");
    
    
    /* Call alignment functions. */
    embAlignPathCalc(p,q,lena,lenb, gapopen,gapextend,path,sub,cvt,
		     compass,show);

    score =  embAlignScoreNWMatrix(path,sp_seq,pdb_seq,sub,cvt,lena, 
				   lenb,gapopen,compass,gapextend,
				   &start1,&start2);

    embAlignWalkNWMatrix(path,sp_seq,pdb_seq,&m,&n,lena,lenb,&start1,
			 &start2,gapopen,gapextend,cvt,compass,sub);

    embAlignCalcSimilarity(m,n,sub,cvt,lena,lenb,&id,&sim,&idx, &simx);

    /* Calculate the start and end of the alignment relative to the 
       swissprot numbering. */
    domainseqs_FindDomainLimits(sp_seq, pdb_seq, m, n, start1, start2,	
			   gapopen, gapextend, score, matrix, begina,
			   beginb, start, end);
   
    
    
    /* Tidy up. */
    AJFREE(compass);
    AJFREE(path);
    ajStrDel(&m);
    ajStrDel(&n);
    
    /* Bye Bye. */
    return ajTrue;
}    





/* @funcstatic domainseqs_FindDomainLimits ************************************
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
******************************************************************************/

static AjBool domainseqs_FindDomainLimits(AjPSeq sp_seq, 
					  AjPSeq pdb_seq,
					  AjPStr m, 
					  AjPStr n,
					  ajint start1, 
					  ajint start2, 
					  float gapopen, 
					  float gapextend, 
					  float score, 
					  AjPMatrixf matrix, 
					  ajint begina, 
					  ajint beginb, 
					  ajint* start, 
					  ajint* end)
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

    const char* a;
    const char* b;

    ajint no_gaps_sp  = 0;   /* The number of gaps in the swissprot sequence. */
    ajint no_gaps_pdb = 0;   /* The number of gaps in the pdb sequence. */
    
    ajDebug("embAlignReportGlobal %d %d\n", start1, start2);
    ajDebug("  sp_seq: '%S' \n", ajSeqGetSeqS(sp_seq));
    ajDebug("  pdb_seq: '%S' \n", ajSeqGetSeqS(pdb_seq));
    ajDebug("  alim: '%S' \n", m);
    ajDebug("  alin: '%S' \n", n);

    maxlen = AJMAX(ajSeqGetLen(sp_seq), ajSeqGetLen(pdb_seq));

    seqset = ajSeqsetNew();
    res1 = ajSeqNew();
    res2 = ajSeqNew();

    ajSeqAssignNameS(res1, ajSeqGetNameS(sp_seq));
    ajSeqAssignNameS(res2, ajSeqGetNameS(pdb_seq));
    ajSeqAssignUsaS(res1, ajSeqGetUsaS(sp_seq));
    ajSeqAssignUsaS(res2, ajSeqGetUsaS(pdb_seq));

    a = ajSeqGetSeqC(sp_seq);
    b = ajSeqGetSeqC(pdb_seq);
    
    /* Generate the full aligned sequences. */
    ajStrSetRes(&fa, maxlen);
    ajStrSetRes(&fb, maxlen);

    /* Pad the start of either sequence. */
    if(start1>start2)
    {
	for(i=0;i<start1;++i)
	{
	    (void) ajStrAppendK(&fa,a[i]);
	}
	nc=start1-start2;
	for(i=0;i<nc;++i) (void) ajStrAppendK(&fb,' ');
	for(++nc;i<start1;++i) (void) ajStrAppendK(&fb,b[i-nc]);
	*start = start1;
	
    }
    else if(start2>start1)
    {
	for(i=0;i<start2;++i)
	{
	    (void) ajStrAppendK(&fb,b[i]);
	}
	nc=start2-start1;
	for(i=0;i<nc;++i) (void) ajStrAppendK(&fa,' ');
	for(++nc;i<start2;++i) (void) ajStrAppendK(&fa,a[i-nc]);
	*start = 0;
    }

    apos = start1 + ajStrGetLen(m) - ajSeqGapCountS(m);
    bpos = start2 + ajStrGetLen(n) - ajSeqGapCountS(n);

    ajStrAppendS(&fa, m);
    ajStrAppendS(&fb, n);

    alen=ajSeqGetLen(sp_seq) - apos;
    blen=ajSeqGetLen(pdb_seq) - bpos;

    ajDebug("alen: %d blen: %d apos: %d bpos: %d\n", 
	    alen, blen, apos, bpos);

    if(alen>blen)
    {
	(void) ajStrAppendC(&fa,&a[apos]);
	for(i=0;i<blen;++i)
	    (void) ajStrAppendK(&fb,b[bpos+i]);
	nc=alen-blen;
	for(i=0;i<nc;++i)
	    (void) ajStrAppendC(&fb," ");
    }	
    
    else if(blen>alen)
    {
	(void) ajStrAppendC(&fb,&b[bpos]);
	for(i=0;i<alen;++i)
	    (void) ajStrAppendK(&fa,a[apos+i]);
	nc=blen-alen;
	for(i=0;i<nc;++i)
	    (void) ajStrAppendC(&fa," ");
    }

    /* Same length, just copy. */
    else			
    {
	(void) ajStrAppendC(&fa,&a[apos]);
	(void) ajStrAppendC(&fb,&b[bpos]);
    }	

    ajDebug("  res1: %5d '%S' \n", ajStrGetLen(fa), fa);
    ajDebug("  res2: %5d '%S' \n", ajStrGetLen(fb), fb);
    ajSeqAssignSeqS(res1, fa);
    ajSeqAssignSeqS(res2, fb);

    ajSeqsetFromPair (seqset, res1, res2);

    end1 = start1 - ajStrCalcCountK(m, '-') + ajStrGetLen(m);
    end2 = start2 - ajStrCalcCountK(n, '-') + ajStrGetLen(n);

    /* Find the domain boundries form the sequence alignment. */

    no_gaps_sp  = (alen + (end1 - ajStrGetLen(sp_seq->Seq)));
    no_gaps_pdb = (blen + (end2 - ajStrGetLen(pdb_seq->Seq)));
    
    *end = (((*start + (end2-start2)) - no_gaps_sp));
   

    ajStrDel(&fa);
    ajStrDel(&fb);
    ajSeqsetDel(&seqset);
    ajSeqDel(&res1);
    ajSeqDel(&res2);

    return ajTrue;
}














