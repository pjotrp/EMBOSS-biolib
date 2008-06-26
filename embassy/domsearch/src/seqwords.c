/* @source seqwords application
**
** Generates DHF files (domain hits files) of database hits (sequences) 
** from Swissprot matching keywords from a keywords file.
**
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
**  SEQWORDS documentation
**  See http://wwww.emboss.org
**  
**  Please cite the authors and EMBOSS.
**
**  Rice P, Bleasby A.J.  2000.  EMBOSS - The European Molecular Biology Open 
**  Software Suite.  Trends in Genetics, 15:276-278.  
**  See also http://www.uk.embnet.org/Software/EMBOSS
**   
**  Email Jon Ison (jison@ebi.ac.uk)
**  
******************************************************************************/







#include "emboss.h"



/******************************************************************************
**
** STRUCTURE DEFINITIONS
**
******************************************************************************/

/* @data AjSTerms *************************************************************
**
** Terms object.
**
** Holds keywords for nodes in a DCF file (domain classification file).
**
** AjPTerms is implemented as a pointer to a C data structure.
**
** @alias AjSTerms
** @alias AjOTerms
**
** @attr Type         [ajint]   Type of domain, either ajSCOP (1) or ajCATH (2).
** @attr Class        [AjPStr]  Domain class name 
** @attr Architecture [AjPStr]  Domain architecture name 
** @attr Topology     [AjPStr]  Domain topology name 
** @attr Fold         [AjPStr]  Domain fold name 
** @attr Superfamily  [AjPStr]  Domain homologous superfamily name 
** @attr Family       [AjPStr]  Domain family name.
** @attr N            [ajint]   No. keywords. 
** @attr Keywords     [AjPStr*]   Array of keywords. 
**
** @@
****************************************************************************/

typedef struct AjSTerms
{
    ajint  Type;
    AjPStr Class;
    AjPStr Architecture;
    AjPStr Topology;
    AjPStr Fold;
    AjPStr Superfamily;
    AjPStr Family;
    ajint  N;		
    AjPStr *Keywords;
} AjOTerms,*AjPTerms;






/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static AjPTerms seqwords_TermsNew(void);

static AjBool   seqwords_TermsRead(AjPFile inf, 
				   AjPTerms *thys);

static void     seqwords_TermsDel(AjPTerms *pthis);

static AjBool   seqwords_keysearch(AjPFile inf,
				   AjPTerms terms,
				   EmbPHitlist *hits);






/* @prog seqwords *******************************************************
**
** Generates DHF files (domain hits files) of database hits (sequences) 
** from Swissprot matching keywords from a keywords file.
**
******************************************************************************/

int main(int argc, char **argv)
{
    AjPFile     key_inf=NULL;	/* File pointer for keywords file.           */
    AjPFile     sp_inf =NULL;	/* File pointer for swissprot database.      */
    AjPFile     outf   =NULL;	/* File pointer for output file.             */
    AjPTerms    keyptr =NULL;	/* Pointer to terms structure.               */
    EmbPHitlist  hitptr =NULL;	/* Pointer to hitlist structure.             */
    


 
   
    /* Read data from acd */
    embInitP("seqwords",argc,argv,"DOMSEARCH");

    key_inf  = ajAcdGetInfile("keyfile");
    sp_inf  = ajAcdGetInfile("spfile");
    outf =  ajAcdGetOutfile("outfile");
    

    /* Start of main application loop. */
    /* Read next list of terms from input file. */
    while((seqwords_TermsRead(key_inf, &keyptr)))
    {
	/* Rewind swissprot file pointer to the top. */
	ajFileSeek(sp_inf, 0, 0);
	

	/* Allocate memory for hitlist. */
	AJNEW0(hitptr);


	/* Do search of swissprot. */
	seqwords_keysearch(sp_inf, keyptr, &hitptr);


	/* Copy scop records from terms to hitlist structure. */
	hitptr->Type = keyptr->Type;
	ajStrAssignRef(&hitptr->Class, keyptr->Class); 
	ajStrAssignRef(&hitptr->Architecture, keyptr->Architecture); 
	ajStrAssignRef(&hitptr->Topology, keyptr->Topology); 
	ajStrAssignRef(&hitptr->Fold, keyptr->Fold); 
	ajStrAssignRef(&hitptr->Superfamily, keyptr->Superfamily);
	ajStrAssignRef(&hitptr->Family, keyptr->Family);
	

	/* Write output file. */
	embHitlistWriteFasta(outf, hitptr);


	/* Free memory for hitlist & keyptr*/
	embHitlistDel(&hitptr);
	seqwords_TermsDel(&keyptr);
    }
    seqwords_TermsDel(&keyptr);    
    
    /* Tidy up*/
    ajFileClose(&key_inf);
    ajFileClose(&sp_inf);
    ajFileClose(&outf);

    embExit();
    return 0;
}





/* @funcstatic seqwords_TermsNew *********************************************
 **
 ** Terms object constructor. This is normally called by the TermsRead
 **  function.
 **
 ** @return [AjPTerms] Pointer to a Terms object
 ** @@
 ****************************************************************************/
static AjPTerms seqwords_TermsNew(void)
{
    AjPTerms  ret =NULL;	
    

    AJNEW0(ret);
    ret->Class=ajStrNew();
    ret->Architecture=ajStrNew();
    ret->Topology=ajStrNew();
    ret->Fold=ajStrNew();
    ret->Superfamily=ajStrNew();
    ret->Family=ajStrNew();
    ret->N=0;
    ret->Keywords=NULL;
        
    return ret;
}





/* @funcstatic seqwords_TermsDel **********************************************
 **
 ** Destructor for terms object.
 **
 ** @param [w] pthis [AjPTerms*] Terms object pointer
 **
 ** @return [void]
 ** @@
 *****************************************************************************/
static void seqwords_TermsDel(AjPTerms *pthis)
{
    int x=0;				
    AjPTerms thys = *pthis;
    
    ajStrDel(&thys->Class);
    ajStrDel(&thys->Architecture);
    ajStrDel(&thys->Topology);
    ajStrDel(&thys->Fold);
    ajStrDel(&thys->Superfamily);
    ajStrDel(&thys->Family);

    
    for(x=0;x<thys->N; x++)
	ajStrDel(&thys->Keywords[x]);
    AJFREE(thys->Keywords);

    AJFREE(thys);
    
    return;
}





/* @funcstatic seqwords_TermsRead *********************************************
 **
 ** Read the next Terms object from a file in embl-like format. The search 
 ** terms are modified with a leading and trailing space.
 **
 ** @param [r] inf  [AjPFile]   Input file stream
 ** @param [w] thys [AjPTerms*] Terms object
 **
 ** @return [AjBool] True on succcess
 ** @@
 *****************************************************************************/
static AjBool seqwords_TermsRead(AjPFile inf, 
				 AjPTerms *thys)
{    
    AjPStr   line           =NULL;	/* Line of text. */
    AjPStr   temp           =NULL;
    AjPList  list_terms     =NULL;	/* List of keywords for a scop node*/
    AjBool   ok             =ajFalse;
    AjPStr   type           = NULL;


    /* Memory management */
    (*thys)=seqwords_TermsNew();
    list_terms = ajListstrNew();
    line       = ajStrNew();
    type       = ajStrNew();
    
    /* Read first line. */
    ok = ajReadlineTrim(inf,&line);


    while(ok && !ajStrPrefixC(line,"//"))
    {
	if(ajStrPrefixC(line,"XX"))
	{
	    ok = ajReadlineTrim(inf,&line);
	    continue;
	}	
	else if(ajStrPrefixC(line,"TY"))
	{
	    ajFmtScanS(line, "%*s %S", &type);
	    
	    if(ajStrMatchC(type, "SCOP"))
		(*thys)->Type = ajSCOP;
	    else if(ajStrMatchC(type, "CATH"))
		(*thys)->Type = ajCATH;
	}
	else if(ajStrPrefixC(line,"CL"))
	{
	    ajStrAssignC(&(*thys)->Class,ajStrGetPtr(line)+3);
	    ajStrRemoveWhiteExcess(&(*thys)->Class);
	}
	else if(ajStrPrefixC(line,"AR"))
	{
	    ajStrAssignC(&(*thys)->Architecture,ajStrGetPtr(line)+3);
	    ajStrRemoveWhiteExcess(&(*thys)->Architecture);
	}
	else if(ajStrPrefixC(line,"TP"))
	{
	    ajStrAssignC(&(*thys)->Topology,ajStrGetPtr(line)+3);
	    ajStrRemoveWhiteExcess(&(*thys)->Topology);
	}
	else if(ajStrPrefixC(line,"FO"))
	{
	    ajStrAssignC(&(*thys)->Fold,ajStrGetPtr(line)+3);
	    while(ajReadlineTrim(inf,&line))
	    {
		if(ajStrPrefixC(line,"XX"))
		    break;
		ajStrAppendC(&(*thys)->Fold,ajStrGetPtr(line)+3);
	    }
	    ajStrRemoveWhiteExcess(&(*thys)->Fold);
	}
	else if(ajStrPrefixC(line,"SF"))
	{
	    ajStrAssignC(&(*thys)->Superfamily,ajStrGetPtr(line)+3);
	    while(ajReadlineTrim(inf,&line))
	    {
		if(ajStrPrefixC(line,"XX"))
		    break;
		ajStrAppendC(&(*thys)->Superfamily,ajStrGetPtr(line)+3);
	    }
	    ajStrRemoveWhiteExcess(&(*thys)->Superfamily);
	}
	else if(ajStrPrefixC(line,"FA"))
	{
	    ajStrAssignC(&(*thys)->Family,ajStrGetPtr(line)+3);
	    while(ajReadlineTrim(inf,&line))
	    {
		if(ajStrPrefixC(line,"XX"))
		    break;
		ajStrAppendC(&(*thys)->Family,ajStrGetPtr(line)+3);
	    }
	    ajStrRemoveWhiteExcess(&(*thys)->Family);
	}
	else if(ajStrPrefixC(line,"TE")) 
	{
	    /* Copy and clean up term. */
	    temp    = ajStrNew();
	    ajStrAssignC(&temp,ajStrGetPtr(line)+3);
	    ajStrRemoveWhiteExcess(&temp);

	    
	    /* Append a leading and trailing space to search term*/
	    ajStrAppendK(&temp, ' ');
	    ajStrInsertC(&temp, 0, " ");

	    
	    /* Add the current term to the list. */
	    ajListstrPush(list_terms,temp);		    
	}

	ok = ajReadlineTrim(inf,&line);
    }
    if(!ok)
    {
	/* Clean up. */
	ajListstrFree(&list_terms);
	ajStrDel(&line);
	
    
	/* Return. */
	return ajFalse;
    }
        
    
    /* Convert the AjPList of terms to array of AjPSeq's. */
    if(!((*thys)->N=ajListstrToarray((AjPList)list_terms,&(*thys)->Keywords)))
	ajWarn("Zero sized list of terms passed into seqwords_TermsRead");


    /* Clean up.  Free the list (not the nodes!). */
    ajListstrFree(&list_terms);
    ajStrDel(&line);
    ajStrDel(&type);
    
    return ajTrue;
} 





/* @funcstatic seqwords_keysearch  ********************************************
**
** Search swissprot with terms structure and writes a hitlist structure
**
** @param [r] inf   [AjPFile]     File pointer to swissprot database
** @param [r] terms [AjPTerms]    Terms object pointer
** @param [w] hits  [EmbPHitlist*] Hitlist object pointer
**
** @return [AjBool] True on success
** @@
******************************************************************************/
static AjBool seqwords_keysearch(AjPFile inf, 
				 AjPTerms terms,
				 EmbPHitlist *hits)
{
    AjPStr   line           =NULL;	/* Line of text. */
    AjPStr   id             =NULL;	/* Line of text. */
    AjPStr   temp           =NULL;
    ajint    s              =0;         /* Temp. start of hit value. */
    ajint    e              =0;         /* Temp. end of hit value. */
    AjPInt   start          =NULL;      /* Array of start of hit(s). */
    AjPInt   end            =NULL;      /* Array of end of hit(s). */
    ajint    nhits          =0;         /* Number of hits. */
    ajint    x              =0;         
    AjBool   foundkw        =ajFalse;
    AjBool   foundft        =ajFalse;


    /* Check for valid args. */
    if(!inf)
	return ajFalse;
    

    

    /* Allocate strings and arrays. */
    line       = ajStrNew();
    id         = ajStrNew();
    start      = ajIntNew();
    end        = ajIntNew();



    /* Start of main loop. */
    while((ajReadlineTrim(inf,&line)))
    {
	/* Parse the AC line. */
	if(ajStrPrefixC(line,"AC"))
	{
	    /* Copy accesion number and remove the ';' from the end. */
	    ajFmtScanS(line, "%*s %S", &id);
	    ajStrExchangeCC(&id, ";", "\0");
	    
	    
	    /* Reset flags & no. hits. */
	    foundkw=ajFalse;
	    foundft=ajFalse;
	    nhits=0;
	}
	
	
	/* Search the description and keyword lines with search terms. */
	else if((ajStrPrefixC(line,"DE") || (ajStrPrefixC(line,"KW"))))
	{
	    /* 
	     ** Search terms have a leading and trailing space to prevent 
	     ** them being found as substrings within other words.  To 
	     ** catch cases where a DE or KW line begins with a search 
	     ** term, we must add a leading and trailing space to line.
	     ** We must first remove punctation from the line to be parsed.
	     */
	    ajStrExchangeSetCC(&line, ".,;:", "    ");
	    ajStrAppendK(&line, ' ');
	    ajStrInsertC(&line, 0, " ");


	    for (x = 0; x < terms->N; x++)
		/* Search term is found. */
		if((ajStrFindCaseS(line, terms->Keywords[x])!=-1))
		{	
		    foundkw=ajTrue;
		    break;
		}
	}

	
	/* Search the feature table line with search terms. */
	else if((ajStrPrefixC(line,"FT   DOMAIN")))
	{
	    /*	
	     ** Search terms have a leading and trailing space to prevent 
	     ** them being found as substrings within other words.  To 
	     ** catch cases where a FT line ends with a search 
	     ** term, we must add a  trailing space to line 
	     ** We must first remove punctation from the line to be parsed.
	     */
	    ajStrExchangeSetCC(&line, ".,;:", "    ");
	    ajStrAppendK(&line, ' ');
	    

	    for (x = 0; x < terms->N; x++)
		if((ajStrFindCaseS(line, terms->Keywords[x])!=-1))
		{
		    /* Search term is found. */
		    foundft = ajTrue;
		    nhits++;
		    
		    /* Assign start and end of hit. */
		    ajFmtScanS(line, "%*s %*s %d %d", &s, &e);


		    ajIntPut(&start, nhits-1, s);
		    ajIntPut(&end, nhits-1, e);
		    break;
		}
	}
	

	/* Parse the sequence. */
	else if((ajStrPrefixC(line,"SQ") && ((foundkw == ajTrue) ||
					     (foundft == ajTrue))))
	{
	    /* Allocate memory for temp. sequence. */
	    temp       = ajStrNew();


	    /* Read the sequence into hitlist structure. */
	    while((ajReadlineTrim(inf,&line)) && !ajStrPrefixC(line,"//"))
		/* Read sequence line into temp. */
		ajStrAppendC(&temp,ajStrGetPtr(line)+3);
 

	    /* Clean up temp. sequence. */
	    ajStrRemoveWhite(&temp);


	    /*Priority is given to domain (rather than full length) sequence.*/
	    if(foundft)
	    {
		for(x=0;x<nhits;x++)
		{
		    /* Increment counter of hits for subsequent hits*/
		    (*hits)->N++;

		    
		    /* Reallocate memory for array of hits in hitlist
                       structure. */
		    AJCRESIZE((*hits)->hits, (*hits)->N);
		    (*hits)->hits[(*hits)->N-1]=embHitNew();
		    ajStrAssignC(&(*hits)->hits[(*hits)->N-1]->Model,
				 "KEYWORD");
		    

		    /* Assign start and end of hit. */
		    (*hits)->hits[(*hits)->N-1]->Start = ajIntGet(start, x);
		    (*hits)->hits[(*hits)->N-1]->End = ajIntGet(end, x);
				

		    /* Extract sequence within specified range */
		    ajStrAssignSubS(&(*hits)->hits[(*hits)->N - 1]->Seq, temp, 
				(*hits)->hits[(*hits)->N - 1]->Start - 1, 
				(*hits)->hits[(*hits)->N - 1]->End - 1);
		    

		    /* Put id into structure */
		    ajStrAssignRef(&(*hits)->hits[(*hits)->N - 1]->Acc, id);
		}
	    }
	    else
	    {
		/* Increment counter of hits */
		(*hits)->N++;

		    
		/* Reallocate memory for array of hits in hitlist structure */
		AJCRESIZE((*hits)->hits, (*hits)->N);
		(*hits)->hits[(*hits)->N-1]=embHitNew();
		ajStrAssignC(&(*hits)->hits[(*hits)->N-1]->Model, "KEYWORD");

		/* Extract whole sequence */
		ajStrAssignRef(&(*hits)->hits[(*hits)->N - 1]->Seq, temp); 
		(*hits)->hits[(*hits)->N - 1]->Start = 1; 
		(*hits)->hits[(*hits)->N - 1]->End =
		    ajStrGetLen((*hits)->hits[(*hits)->N - 1]->Seq); 


		/* Put id into structure */
		ajStrAssignRef(&(*hits)->hits[(*hits)->N - 1]->Acc, id);
	    }

	    /* Free temp. sequence */
	    ajStrDel(&temp);
	}
    }


    /* Clean up */
    ajStrDel(&line);
    ajStrDel(&id);
    ajIntDel(&start);
    ajIntDel(&end);

    return ajTrue;
}
