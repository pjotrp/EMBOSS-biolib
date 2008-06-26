/* @source ssematch application
**
** Searches a DCF file (domain classification file) for secondary structure 
** matches.
** 
** @author: Copyright (C) Amanda O'Reilly
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
*******************************************************************************
** 
**  SSEMATCH documentation
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
**  
****************************************************************************/





#include "emboss.h"





/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static ajint   ssematch_CompScoreInv(const void *scop1, 
				     const void *scop2);

static AjBool         ssematch_NWScore(AjPScop temp_scop, 
				AjPSeq pseq,
				ajint mode,
				AjPMatrixf matrix,
				float gapopen,
				float gapextend);

static AjPSeq         ssematch_convertbases(AjPStr qse);





/* @prog ssematch ***********************************************************
**
** Searches a DCF file (domain classification file) for secondary structure 
** matches.
**
****************************************************************************/
int main(int argc, char **argv)
{
    /* Variables declarations */
    AjPFile dcfin        = NULL; /* Domain classification file */
    AjPFile ssin         = NULL; /* Secondary structure input file*/
    AjPMatrixf matrix    = NULL; /* Substitution matrix */
    AjPFile out_ss       = NULL; /* For ss top matches*/
    AjPFile out_se       = NULL; /* For se top matches*/
    AjPFile outfile      = NULL; /* Output file*/
    AjPFile logf         = NULL; /* Log file */
    float gapopen_sss    = 0.0;  /* Gap insertion penalty */
    float gapopen_sse    = 0.0;
    float gapopen        = 0.0;
    float gapextend_sss  = 0.0;  /* Gap extension penalty */
    float gapextend_sse  = 0.0;
    float gapextend      = 0.0;
    ajint max_hits       = 0;    /* number of top alignments to display*/
    ajint mode           = 0;
    ajint x              = 0;

    AjPScop temp_scop    = NULL; /* scop object pointer*/

    AjPList  scop_list   = NULL; /* list of scop objects for entire domain
				    classification file */
     
    AjIList       iter   = NULL;
    AjPStr        msg    = NULL; /* Pointer to String used for messages */
    AjPStr        line   = NULL;
   
    AjPStr    qse        = NULL; /* query secondary structure elements*/
    AjPStr    qss        = NULL; /* query secondary structure (by residue)*/
    AjPSeq    q3se       = NULL; /* query secondary structure elements, 3-letter 
				    code*/
    AjPSeq    q3ss       = NULL; /* query secondary structure 
				    (by residue), 3-letter code*/
    AjPSeq    query      = NULL;
    




    /* Read data from acd */
    embInitP("ssematch",argc,argv,"DOMAINATRIX");
    dcfin       = ajAcdGetInfile("dcfinfile");
    ssin       = ajAcdGetInfile("ssinfile");
    max_hits      = ajAcdGetInt("maxhits");
    matrix        = ajAcdGetMatrixf("datafile");
    gapopen_sss   = ajAcdGetFloat("gapopenr");
    gapextend_sss = ajAcdGetFloat("gapextendr");
    gapopen_sse   = ajAcdGetFloat("gapopene");
    gapextend_sse = ajAcdGetFloat("gapextende");
    out_ss        = ajAcdGetOutfile("outssfile");
    out_se        = ajAcdGetOutfile("outsefile");
    logf       = ajAcdGetOutfile("logfile");






    /* Create list of scop objects for entire input domain classification file. */
    scop_list  = ajListNew();
    while((temp_scop = (ajScopReadCNew(dcfin, "*"))))
        ajListPushAppend(scop_list,temp_scop);


    /* Error handing if domain classification file was empty. */
    if(!(ajListGetLength(scop_list)))
    {
      
        ajWarn("Empty list from scop input file\n");
	ajFileClose(&dcfin);
	ajFileClose(&ssin);
	ajMatrixfDel(&matrix);
       	ajFileClose(&out_ss);
	ajFileClose(&out_se);
       	ajFileClose(&logf);
	while(ajListPop(scop_list, (void *) &temp_scop))
	    ajScopDel(&temp_scop);
	ajListFree(&scop_list);
        ajListIterDel(&iter);    
	
	ajExit();
	return 1;
    }

    /* Error handling in case of empty query file. */
    if(ssin == NULL)   
    {
        ajWarn("Empty secondary structure query file\n");
	ajFileClose(&dcfin);
	ajFileClose(&ssin);
	ajMatrixfDel(&matrix);
       	ajFileClose(&out_ss);
	ajFileClose(&out_se);
       	ajFileClose(&logf);
	while(ajListPop(scop_list, (void *) &temp_scop))
	    ajScopDel(&temp_scop);
	ajListFree(&scop_list);
        ajListIterDel(&iter);    
	
	ajExit();
	return 1; 
    }      
    
    /* Assign sequences in query file to sequence objects. */
    qse = ajStrNew();
    qss = ajStrNew();
    
    while(ajReadlineTrim(ssin,&line))
    {
        /* SE string */
        if(ajStrPrefixC(line,"SE"))
	{
	    ajFmtScanS(line, "%*s %S", &qse);
            /* Convert this string to 3-letter code & then convert to AjPSeq 
	       object. */
            q3se = ssematch_convertbases(qse);
        }
        /* SS string */
	else if(ajStrPrefixC(line,"SS"))
	{
            while((ajReadlineTrim(ssin,&line)) && !ajStrPrefixC(line,"XX"))
                ajStrAppendS(&qss,line);
            ajStrRemoveWhite(&qss);

            /* Convert this string to 3-letter code & then to AjPSeq object. */
            q3ss = ssematch_convertbases(qss);
      	}
    }



    /* For se & then for ss, modes 0 & 1. */
    for(mode = 0; mode <= 1; mode++)
    {
        /* Assign arguments for alignment function. */
        if (mode == 0) 
        {
            query = q3se;
            gapopen = gapopen_sse; 
            gapextend = gapextend_sse;
            outfile =  out_se;
        } 
        else if(mode == 1)
        {
            query = q3ss;  
            gapopen = gapopen_sss; 
            gapextend = gapextend_sss;
            outfile =  out_ss;   
        }


        /* Iterate through list of scop objects & calculate alignment scores. */
        iter=ajListIterNew(scop_list);
        while((temp_scop=(AjPScop)ajListIterGet(iter)))
        {
            /* The function extracts the se (mode 0) or ss (mode 1) subject 
	       sequences from the scop object, performs a Needleman-Wunsch 
	       global alignment with the query sequence & allocates the score 
	       to the Score element of the scop object*/ 

	    if(!(ssematch_NWScore(temp_scop , query, mode, matrix, gapopen, gapextend)))

	    {
                ajFmtPrintF(logf, "%-15s\n", "ALIGNMENT");
                ajFmtPrintF(logf, "Could not align sequence in scop domain %S\n ", 
			    temp_scop->Entry); 
	        ajFmtPrintS(&msg, "Could not align sequence in scop domain %S\n ", 
			    temp_scop->Entry);
	        ajWarn(ajStrGetPtr(msg));
                continue;
	    }
	}


        ajListIterDel(&iter);
      	temp_scop    = NULL;


        /* Sort list of Scop objects by Score */
        ajListSort(scop_list, ssematch_CompScoreInv);
	
	
        iter=ajListIterNew(scop_list);
        /* Write top-scoring hits to outfile. */
        for(x=0; x < max_hits; x++ )
	{
            temp_scop=(AjPScop)ajListIterGet(iter);
 
	    /* Print score to output file. */
	    ajFmtPrintF(outfile, "XX   ALIGNMENT SCORE %.3f\nXX\n", temp_scop->Score);
	    
	    /* Could also write alignment - later modification. */
	    if(!ajScopWrite(outfile, temp_scop))
		ajFatal("Could not write output file %S\n", outfile);

        
        }

	ajListIterDel(&iter); 
     	temp_scop    = NULL;
    }


      
    /* Memoryt management. */
    ajFileClose(&dcfin);
    ajFileClose(&ssin);
    ajMatrixfDel(&matrix);
    ajFileClose(&out_ss);
    ajFileClose(&out_se);
    ajFileClose(&logf);
    while(ajListPop(scop_list, (void *) &temp_scop))
	ajScopDel(&temp_scop);
    ajListFree(&scop_list);
    ajStrDel(&msg);
    ajStrDel(&line);
    ajStrDel(&qse); 
    ajStrDel(&qss);
    ajSeqDel(&q3se);
    ajSeqDel(&q3ss);
    

    ajExit();
    return 0;
}
    






/* @funcstatic ssematch_CompScoreInv ******************************************
**
** Function to sort AjOScop objects by Score element.
**
** @param [r] scop1  [const void*] Pointer to AjOScop object 1
** @param [r] scop2  [const void*] Pointer to AjOScop object 2
**
** @return [ajint] +1 if Score1 should sort before Score2, -1 if the Score2
** should sort first. 0 if they are identical in value.
** @@
******************************************************************************/


static ajint ssematch_CompScoreInv(const void *scop1, const void *scop2)
{
    AjPScop p  = NULL;
    AjPScop q  = NULL;

    p = (*  (AjPScop*)scop1);
    q = (*  (AjPScop*)scop2);

    if(p->Score < q->Score)
	return 1;
    else if(p->Score == q->Score)
	return 0;
    else
	return -1;
}






/* @funcstatic ssematch_NWScore ***********************************************
**
** Calculate the similarity between query sequence and subject sequence.
**
** @param [r] temp_scop  [AjPScop]    Scop object
** @param [r] pseq       [AjPSeq]     Query sequence
** @param [r] mode       [int]        Mode 
** @param [r] matrix     [AjPMatrixf] Substitution matrix
** @param [r] gapopen    [float]      Gap insertion penalty
** @param [r] gapextend  [float]      Gap extension penalty
**
** @return               [AjBool]  True on success
** @@
******************************************************************************/

static AjBool  ssematch_NWScore(AjPScop temp_scop, 
				AjPSeq pseq, 
				ajint mode, 
				AjPMatrixf matrix,
				float gapopen, 
				float gapextend)


{
    ajint         start1  =0;	/* Start of seq 1, passed as arg but not used.*/
    ajint         start2  =0;	/* Start of seq 2, passed as arg but not used.*/
    ajint         maxarr  =300;	/* Initial size for matrix.                   */
    ajint         len;

    ajint        *compass;

    const char       *p;        /* Query sequence.                            */
    const char       *q;        /* Subject sequence from scop object.         */

    float     **sub;
    float       id       =0.;	/* Passed as arg but not used here.           */
    float       sim      =0.;	
    float       idx      =0.;	/* Passed as arg but not used here.           */
    float       simx     =0.;	/* Passed as arg but not used here.           */
    float      *path;

    AjPStr      pstr = NULL;	/*  m walk alignment for first sequence 
				    Passed as arg but not used here.          */
    AjPStr      qstr = NULL;	/*  n walk alignment for second sequence 
				    Passed as arg but not used here.          */

    AjPSeq      qseq = NULL;    /* Subject sequence.                          */

   
    ajint lenp;                 /* Length of query sequence.                  */
    ajint lenq;                 /* Length of subject sequence.                */
    

    AjPSeqCvt   cvt  = 0;
    AjBool      show = ajFalse; /*Passed as arg but not used here.            */






    AJCNEW(path, maxarr);
    AJCNEW(compass, maxarr);
    pstr = ajStrNew();    
    qstr = ajStrNew();    
    gapopen   = ajRoundF(gapopen,8);
    gapextend = ajRoundF(gapextend,8);
    sub = ajMatrixfArray(matrix);
    cvt = ajMatrixfCvt(matrix);


    
    
    /* Extract subject sequence from scop object, convert to 3 letter code. */
    if (mode == 0)
        qseq = ssematch_convertbases(temp_scop->Sse);
    else if (mode == 1)
        qseq = ssematch_convertbases(temp_scop->Sss);


    lenp = ajSeqGetLen(pseq); /* Length of query sequence.   */
    lenq = ajSeqGetLen(qseq); /* Length of subject sequence. */
   

   


    /* Start of main application loop */
    /* Intitialise variables for use by alignment functions*/	    
    len = (lenp * lenq);

    if(len>maxarr)
    {
	AJCRESIZE(path,len);
	AJCRESIZE(compass,len);
	maxarr=len;
    }

    p = ajSeqGetSeqC(pseq); 
    q = ajSeqGetSeqC(qseq); 

    ajStrAssignC(&pstr,"");
    ajStrAssignC(&qstr,"");


    /* Check that no sequence length is 0. */
    if((lenp == 0)||(lenq == 0))
    {
       	AJFREE(compass);
	AJFREE(path);
	ajStrDel(&pstr);
	ajStrDel(&qstr);
    }


    /* Call alignment functions. */
    embAlignPathCalc(p,q,lenp,lenq, gapopen,
		     gapextend,path,sub,cvt,compass,show);

    /*embAlignScoreNWMatrix(path,compass,gapopen,gapextend,
                          pseq, qseq,
			  lenp,lenq,sub,cvt,
			  &start1,&start2);*/

    embAlignWalkNWMatrix(path,pseq,qseq,&pstr,&qstr,
			 lenp,lenq,&start1,&start2,
                         gapopen,gapextend,compass);

    embAlignCalcSimilarity(pstr,qstr,sub,cvt,lenp,
			   lenq,&id,&sim,&idx, &simx);


    /* Assign score. */
    
    temp_scop->Score = sim;
    
  



    /* Tidy up */
    AJFREE(compass);
    AJFREE(path);
    ajStrDel(&pstr);
    ajStrDel(&qstr);
    ajSeqDel(&qseq); 

    
    /* Bye Bye */
    return ajTrue;
}    






/* @funcstatic ssematch_convertbases ******************************************
**
** Convert AjPStr of stride-assigned secondary structures to AjPSeq of 
** secondary structures in 3-state assignment.
**
** @param [r]  qs  [AjPstr]  String
** 
** @return [w] q3s [AjPSeq] Sequence
**
** @@
******************************************************************************/
static AjPSeq ssematch_convertbases(AjPStr qs)
{
    AjIStr     iter = NULL;
    char       base;    
    AjPSeq     tmp_seq   = NULL;   
    AjPStr     tmp_str   = NULL;   
    AjPStr name_str = NULL;

    /* Iterate through letters of string*/
    /* StrIterGetK:returns Current text string within iterator, char* */
    /* helices H, G, I -> G & I are changed to H 
       extended conformation -> E stays as E
       bridge B/b, turn T, coil C -> all changed to L */



    iter    = ajStrIterNew(qs);
    tmp_str = ajStrNew();

    do
    {
	base = ajStrIterGetK(iter);
	    
	if((base == 'G') || (base == 'I'))
	    base = 'H';
	else if((base == 'B') || (base == 'b') || (base == 'T')	
		|| (base == 'C'))
	    base = 'L';

	ajStrAppendK(&tmp_str, base);
    }
    while(ajStrIterNext(iter));
    

    ajStrAssignC(&name_str, "convertbases");
    tmp_seq = ajSeqNewNameS(tmp_str, name_str);


    /* Tidy up */
    ajStrIterDel(&iter);
    ajStrDel(&tmp_str);
    ajStrDel(&name_str);

    return tmp_seq;
}



