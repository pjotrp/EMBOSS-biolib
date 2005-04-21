/* @source seqsort application
**
** Reads multiple files of hits and writes (i) a scop families file and (ii) 
** a scop ambiguities file.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
** 02111-1307, USA.
**
*******************************************************************************
** 
**  SEQSORT Documentation
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
**  
******************************************************************************/






#include "emboss.h"





/******************************************************************************
**
** PROTOTYPES  
**
******************************************************************************/
static AjBool seqsort_HitlistToThreeScophits(AjPList in, 
				      AjPList *fam,
				      AjPList *sfam,
				      AjPList *fold);

static AjBool seqsort_HitlistPriorityLow(AjPHitlist *list);

static AjBool seqsort_HitlistPriorityHigh(AjPHitlist *list);

static AjBool seqsort_WriteOutputFiles(AjPFile fptr_fam,
				       AjPFile fptr_amb, 
				       AjBool dofam,
				       AjBool doamb,
				       AjPList famlist, 
				       AjPList supfamlist, 
				       AjPList foldlist,
				       AjPDir outdir);

static AjBool seqsort_PsiblastHitSort(AjPList* famlist,
				      AjPList* supfamlist, 
				      AjPList* foldlist, 
				      ajint sig_overlap, 
				      AjPStr path,
				      AjPStr extn);

static AjPList seqsort_HitlistListRead(AjPStr path,
				       AjPStr extn);



void seqsort_unused(void);



/* @prog seqsort **************************************************************
**
** Reads DHF files (domain hits files) of database hits (sequences) and 
** removes hits of ambiguous classification.
**
******************************************************************************/
int main(int argc, char **argv)
{
    ajint   sig_overlap  = 0;       /* The minimum overlaping residues 
				       required for merging of two hits.     */
    AjPDir  dhfin      = NULL;      /* Directory of domain hits files - input*/
    AjPDir  dhfout     = NULL;      /* Directory of domain hits files - input*/
    AjPStr  psipath    = NULL;      /* the name of the directory where 
				       psiblasts results are kept.           */
    AjPStr  psiextn    = NULL;      /* The psiblasts file extension.         */
    

   
    AjPList famlist    = NULL;      /* List of family members.               */
    AjPList supfamlist = NULL;      /* List of superfamily members.          */	
    AjPList foldlist   = NULL;      /* List of fold members.                 */
    
    AjPFile ambigf     = NULL;      /* Domain ambiguities output file.       */
    AjPFile hitsf      = NULL;      /* Domain families output file.          */
    
    AjBool  dofamilies = ajFalse;   /* True if domain families file is 
				       written.                              */
    AjBool  doambiguities= ajFalse; /* True if domain ambiguities file 
				       is written.                           */
    AjPScophit  tmp    = NULL;      /* Temp. pointer for freeing famlist, 
				       supfamlist, foldlist.                 */
    



    /* ACD processing */
    ajNamInit("emboss");
    ajAcdInitP("seqsort",argc,argv,"DOMSEARCH");
    sig_overlap    = ajAcdGetInt("overlap");
    ambigf         = ajAcdGetOutfile("ambigfile");
    hitsf          = ajAcdGetOutfile("hitsfile");
    dofamilies     = ajAcdGetToggle("dofamilies");
    doambiguities  = ajAcdGetToggle("doambiguities");
    dhfin          = ajAcdGetDirectory("dhfindir");
    dhfout         = ajAcdGetOutdir("dhfoutdir");


    /* Memory allocation & initialisation */
    famlist    = ajListNew();
    supfamlist = ajListNew();
    foldlist   = ajListNew();
    psipath    = ajStrNew();
    psiextn    = ajStrNew();
    
    
    ajStrAssS(&psipath, ajDirName(dhfin));
    ajStrAssS(&psiextn, ajDirExt(dhfin));



    /* Process results of psiblast searches for scop families */ 
    if(!seqsort_PsiblastHitSort(&famlist,&supfamlist,&foldlist, sig_overlap,
				 psipath,psiextn))
	ajFatal("seqsort_PsiblastHitSort failed");
    
    seqsort_WriteOutputFiles(hitsf, ambigf, dofamilies, doambiguities, 
			     famlist, supfamlist, foldlist, dhfout);



    /* Memory management. */
    ajFileClose(&ambigf);
    ajFileClose(&hitsf);

    while(ajListPop(famlist,(void**)&tmp))
	ajDmxScophitDel(&tmp);
    ajListDel(&famlist);

    while(ajListPop(supfamlist,(void**)&tmp))
	ajDmxScophitDel(&tmp);
    ajListDel(&supfamlist);

    while(ajListPop(foldlist,(void**)&tmp))
	ajDmxScophitDel(&tmp);
    ajListDel(&foldlist);

    ajStrDel(&psipath);
    ajStrDel(&psiextn);
    ajDirDel(&dhfin);
    ajDirDel(&dhfout);
    

    ajExit();

    return 0;
}

/* @funcstatic  seqsort_HitlistListRead  ************************************
**
** Reads DHF files (domain hits files) with a given file extension from a 
** specified directory.
** A list of Hitlist structures is allocated and returned.
**
** @param [r] path       [AjPStr]    File path of hits files.
** @param [r] extn       [AjPStr]    File extension of hits files.
** 
** @return [AjPList] which is a list of Hitlist structures.
** @@
*****************************************************************************/

static AjPList seqsort_HitlistListRead(AjPStr path, 
				       AjPStr extn)
{
    AjPStr tmp         = NULL;	 /* Temporary string.                       */
    AjPStr filename    = NULL;	 /* Name of file containing the results of a 
				    psiblasts run.                          */
    AjPStr logf        = NULL;   /* Log file pointer.                       */
     
    AjPList list       = NULL;   /* List to hold the file names.            */
    AjPList tmplist    = NULL;	 /* Temporary list.                         */

    AjPHitlist hitlist = NULL;   /* Hitlist for reading a DHF file.         */
    AjPFile  inf       = NULL;   /* DHF file.                               */
    
    
    tmp      = ajStrNew();
    filename = ajStrNew();
    logf     = ajStrNew();

    list     = ajListNew();
    tmplist  = ajListNew();
    

    
    /* Check directories. */

    if(!path || !extn)
	ajFatal("Bad arg's passed to seqsort_HitlistListRead\n");
    
    
    /* Create list of files in the path. */
    ajStrAssC(&tmp, "*");  		
	
    if((ajStrChar(extn, 0)=='.')) 	
	ajStrApp(&tmp, extn);    
					
 
    /* This picks up situations where the user has specified an extension 
       without a ".". */
    else
    {
	ajStrAppC(&tmp, ".");       	
	ajStrApp(&tmp, extn);       	
    }	

    /* All files containing hits will be in a list. */
    ajFileScan(path, tmp, &list, ajFalse, ajFalse, NULL, NULL, ajFalse, NULL);    

    
    /* Read each psiblast file and create a list of Scophit structures. */
    while(ajListPop(list,(void **)&filename))
    {
	if((inf = ajFileNewIn(filename)) == NULL)
	{
	  ajWarn("Could not open for reading\n");
	  ajFmtPrintS(&logf,"WARN  Could not open for reading %S\n",filename);	
	  continue;	    
	}	
	
	/* Read each psblast file into a Hitlist structure. */
	hitlist = embHitlistReadFasta(inf);    
	ajListPushApp(tmplist, hitlist);        
	ajStrDel(&filename);
	ajFileClose(&inf); 
    }
    
    /* Memory management. */
    ajStrDel(&tmp);
    ajStrDel(&filename);
    ajStrDel(&logf);
    ajListDel(&list);

    
    return tmplist;
}





/* @funcstatic  seqsort_PsiblastHitSort  **************************************
**
** Processes the results of a seqsearch run as held in three lists of Scophit 
** objects (for families, superfamilies and folds). The lists are modified.
**
** @param [w] famlist    [AjPList *] Families list
** @param [w] supfamlist [AjPList *] Superfamilies list
** @param [w] foldlist   [AjPList *] Folds list
** @param [r] sig_overlap[ajint ]    The minimum overlaping residues 
**                		     required for merging of two hits
** @param [r] path       [AjPStr]    File path of hits files.
** @param [r] extn       [AjPStr]    File extension of hits files.
** 
** @return [AjBool] ajTrue if the list has been sorted, ajFalse otherwise.
** @@
******************************************************************************/

static AjBool seqsort_PsiblastHitSort(AjPList* famlist,
				      AjPList* supfamlist, 
				      AjPList* foldlist, 
				      ajint sig_overlap, 
				      AjPStr path,
				      AjPStr extn)
{

    AjPList hitslist   = NULL;		/* List of hitlist structures. */
    AjIList iter       = NULL;		/* List iterator for family list. */
  
    AjPScophit  hit    = NULL;		/* A given hit in the list. */
    AjPScophit  nexthit= NULL;		/* The next hit in the list. */

    AjPHitlist  tmp    = NULL;          /* Pointer for freeing hitslist. */
    
   
    /* Check args. */
    if((!(*famlist)) || (!(*supfamlist)) || (!(*foldlist)) || (!path) || 
       (!extn))
	return ajFalse;
    

    /***********************************************************************/
    /** FIGURE A.2 Preparation of list for processing results of PSIBLAST **/
    /** searches for SCOP families.                                       **/
    /***********************************************************************/       
 
    /* Read the files containing the psiblasts hits and construct a list 
       of hitlist structures. */ 
    hitslist  =  seqsort_HitlistListRead(path,extn);

    /* Convert the hitslist to a single list of Scophit structures (families 
       list). */
    embDmxHitlistToScophits(hitslist,famlist);


    
    /* DIAGNOSTIC       
    ajFmtPrint("\n\n\nContents of FAMILIES list\n");
    iter= ajListIter(*famlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    ajFmtPrint("\nContents of SUPERFAMILIES list\n");
    iter= ajListIter(*supfamlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    ajFmtPrint("\nContents of FOLDS list\n");
    iter= ajListIter(*foldlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    */

    
    /* DIAGNOSTIC 
    printf("\n\nIDENTIFYING MEMBERS OF FAMILIES (merge overlapping hits)\n"); 
    */

    /***********************************************************************/
    /** FIGURE A.3 Identify members of families (merge overlapping hits)  **/
    /***********************************************************************/

    iter=ajListIter(*famlist); 

    /* Sort list, first by Family, then by Accession number, and finally by 
       Start. */
    ajListSort3(*famlist, ajDmxScophitCompSunid, ajDmxScophitCompAcc, 
		ajDmxScophitCompStart);

        
    /* Get the first node in the list, only once. */
    hit = (AjPScophit)ajListIterNext(iter);
  
    /* Loop while we can get another hit. */
    while((nexthit=(AjPScophit)ajListIterNext(iter)))
    {
	/* Check if the accession numbers are the same and if there is 
	   significant overlap. */


	/* DIAGNOSTIC  
	   ajFmtPrint("Comparing %S (%d-%d, %B %B) to %S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2, 
		   nexthit->Acc, nexthit->Start, nexthit->End, nexthit->Target, 
		   nexthit->Target2);*/
	

	if(embDmxScophitsOverlapAcc(hit,nexthit,sig_overlap))
	{
	    /* Are the families identical? */
	    if( hit->Sunid_Family == nexthit->Sunid_Family)
	    {
		/* Insert merged hit to the same list and target other 2 for 
		   removal. */
		embDmxScophitMergeInsertThis(*famlist,hit,nexthit,iter);
		hit = (AjPScophit)ajListIterNext(iter);
		continue;
	    }
	    else
	    {
		/* DIAGNOSTIC 
		ajFmtPrint("--> Overlap but families different\n");
		*/

		/* Move one node along. */
		hit = nexthit;
	    }
	}	

	/* would mean that the two hits were distinct and should be left 
	   in the list. */
	else
	{
	    /* DIAGNOSTIC 
	    ajFmtPrint("--> Hits are distinct !\n");
	    */

	    /* Move one node along. */
	    hit = nexthit;
	}
    }
    ajListIterFree(&iter);



    /* The end of the list been reached. 
       Delete hits in the list that are targetted for removal. */
    ajListGarbageCollect(*famlist, ajDmxScophitDelWrap, 
			 (int(*)(const void *)) ajDmxScophitCheckTarget);
    


    /* Sort list, first by accession number,then by start and finally by family. */
    ajListSort3(*famlist, ajDmxScophitCompAcc, ajDmxScophitCompStart, 
		ajDmxScophitCompSunid); 




    /* DIAGNOSTIC
    ajFmtPrint("\n\n\nContents of FAMILIES list\n");
    iter= ajListIter(*famlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    ajFmtPrint("\nContents of SUPERFAMILIES list\n");
    iter= ajListIter(*supfamlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    ajFmtPrint("\nContents of FOLDS list\n");
    iter= ajListIter(*foldlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    */



    /***********************************************************************/
    /** FIGURE A.4 Identify members of families  (remove superfamily and  **/
    /** fold members from family list).                                   **/
    /***********************************************************************/      
    

    iter= ajListIter(*famlist);
    
    /* Get the first node in the list, only once. */
    hit = (AjPScophit)ajListIterNext(iter); 
    while((nexthit=(AjPScophit)ajListIterNext(iter)))
    {
	/* Check if the accession numbers are the same and if there is significant 
	   overlap. */

	/* DIAGNOSTIC 
	   ajFmtPrint("Comparing %S (%d-%d, %B %B) to %S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2, 
		   nexthit->Acc, nexthit->Start, nexthit->End, nexthit->Target, 
		   nexthit->Target2);
		   */

	if(embDmxScophitsOverlapAcc(hit,nexthit,sig_overlap))
	{
	    /* If Target2 is set for either hit then target both for removal. */
	    if(hit->Target2 || nexthit->Target2)
	    {
		ajDmxScophitTarget(&hit);
		ajDmxScophitTarget(&nexthit);
	    }
	    
	    /* Are the families identical? */

	    /* 
	     ** Overlaps between hits with same family will already have been 
	     ** merged.  If they are detected now it is because we no longer 
	     ** do garbage collection immediately before this code block. Such
	     ** cases should be ignored, otherwise errors will result.
	     */


	    /* Are the superfamilies identical? */
	    if(ajStrMatch(hit->Superfamily,nexthit->Superfamily)&&
		    !(ajStrMatch(hit->Family,nexthit->Family)))
	    {
		/* DIAGNOSTIC 
		ajFmtPrint("--> Overlap & superfamilies identical! Merged hit "
		"goes to sfam list, both are targetted\n");
		*/

		/* Target both hits for removal.  Place a hit corresponding 
		   to the merging of the two hits into the supfamlist. */
		embDmxScophitMergeInsertOther(*supfamlist,hit,nexthit);

		
		/* Also place the merged hit into the family list so it can be 
		   considered for overlaps, but target it for removal. */
		
		embDmxScophitMergeInsertThisTarget(*famlist,hit,nexthit,iter);
		hit = (AjPScophit)ajListIterNext(iter);
	    }	
	    /* Are the folds identical? */
               else if(ajStrMatch(hit->Fold,nexthit->Fold)&&
		    !(ajStrMatch(hit->Superfamily,nexthit->Superfamily))&&
		    !(ajStrMatch(hit->Family,nexthit->Family)))
	    {
		/* DIAGNOSTIC 
		ajFmtPrint("--> Overlap & folds identical! Merged hit goes to "
		"fold list, both are targetted\n");
		*/

		/* Target both hits for removal.  Place a hit corresponding 
		   to the merging of the two hits into the foldlist. */
		embDmxScophitMergeInsertOther(*foldlist,hit,nexthit);    


		/* Also place the merged hit into the family list so it can be 
		   considered for overlaps, but target it for removal. */
		
		embDmxScophitMergeInsertThisTarget(*famlist,hit,nexthit,iter);
		hit = (AjPScophit)ajListIterNext(iter);
		continue;
	    }
	    else
	    {
		/* DIAGNOSTIC 
		ajFmtPrint("--> Overlap & folds different! Both are targetted\n");
		*/

		/* Target both hits for removal.. */
		ajDmxScophitTarget(&hit);
		ajDmxScophitTarget(&nexthit);


		/* Also place a merged hit into the family, superfamily and 
		   fold list so it can be considered for overlaps, but 
		   target it for removal. */
		
		embDmxScophitMergeInsertThisTargetBoth(*famlist,hit,nexthit,iter);
		embDmxScophitMergeInsertOtherTargetBoth(*supfamlist,hit,nexthit);
		embDmxScophitMergeInsertOtherTargetBoth(*foldlist,hit,nexthit);
		
		hit = (AjPScophit)ajListIterNext(iter);
		continue;
	    }
	}
	else
	{
	    /* DIAGNOSTIC 
	    ajFmtPrint("--> Hits are distinct !\n");
	    */

	    /* Move one node along. */
	    hit=nexthit;
	}
    }
    
    
    /* The end of the list been reached. 
       Delete hits in the list that are targetted for removal. */
    ajListGarbageCollect(*famlist, ajDmxScophitDelWrap, 
			 (int(*)(const void *)) ajDmxScophitCheckTarget);



    /* Sort list, first by Family, then by accession number, and finally by 
       Start. */
    ajListSort3(*famlist, ajDmxScophitCompSunid, ajDmxScophitCompAcc, 
		ajDmxScophitCompStart);

    
    ajListIterFree(&iter);



    /* DIAGNOSTIC 
    ajFmtPrint("\n\n\nContents of FAMILIES list\n");
    iter= ajListIter(*famlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    ajFmtPrint("\nContents of SUPERFAMILIES list\n");
    iter= ajListIter(*supfamlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    ajFmtPrint("\nContents of FOLDS list\n");
    iter= ajListIter(*foldlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    */


	/* DIAGNOSTIC 
    printf("\n\nIDENTIFYING MEMBERS OF SUPERFAMILIES (merge overlapping hits)\n");
    */

    /**************************************************************************/
    /** FIGURE A.5 Identify members of superfamilies (merge overlapping hits) */
    /**************************************************************************/

    iter= ajListIter(*supfamlist);
    
    /* Sort list, first by superfamily, then by accession number and finally by
       Start. */
    ajListSort3(*supfamlist, ajDmxScophitCompSfam, ajDmxScophitCompAcc, 
		ajDmxScophitCompStart);
    
    /* Get the first node in the list, only once. */ 
    hit = (AjPScophit)ajListIterNext(iter);	                
    while((nexthit=(AjPScophit)ajListIterNext(iter)))
    {   	   
	/* Check if the accession numbers are the same and if there is 
	   significant overlap.. */


	/* DIAGNOSTIC 
	   ajFmtPrint("Comparing %S (%d-%d, %B %B) to %S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2, 
		   nexthit->Acc, nexthit->Start, nexthit->End, nexthit->Target,
		   nexthit->Target2);
		   */

	if(embDmxScophitsOverlapAcc(hit,nexthit,sig_overlap))
	{
	    /* Are the superfamilies identical? */
	    if(ajStrMatch(hit->Superfamily,nexthit->Superfamily))
	    {
		/* DIAGNOSTIC 
		ajFmtPrint("--> Overlap & superfamilies identical! "
		"Replacing both with merged hit\n");
		*/

		/* Merge the two hits and then write back to the same list. */
		embDmxScophitMergeInsertThis(*supfamlist,hit,nexthit, iter);
		hit = (AjPScophit)ajListIterNext(iter);
	    }
	    else
	    {
		/* DIAGNOSTIC 
		ajFmtPrint("--> Overlap but superfamilies different\n");
		*/

		/* Move one node along. */
		hit = nexthit;
	    }	
	}
	/* Would mean that the two hits were distinct and should be left 
	   in the list. */
	else
	{
	    /* DIAGNOSTIC 
	    ajFmtPrint("--> Hits are distinct !\n");
	    */

	    /* Move one node along. */
	    hit = nexthit;
	}
    }

    /* The end of the list been reached. */

    /* Sort list , first by accession number,then by start and finally by 
       superfamily. */
    ajListSort3(*supfamlist, ajDmxScophitCompAcc, ajDmxScophitCompStart,        	
		ajDmxScophitCompSfam);
    
    ajListIterFree(&iter);


    /* DIAGNOSTIC 

    ajFmtPrint("\n\n\nContents of FAMILIES list\n");
    iter= ajListIter(*famlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    ajFmtPrint("\nContents of SUPERFAMILIES list\n");
    iter= ajListIter(*supfamlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    ajFmtPrint("\nContents of FOLDS list\n");
    iter= ajListIter(*foldlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    */

    
    /***************************************************************/
    /** FIGURE A.6 Identify members of superfamilies (remove fold **/
    /** members from the superfamilies list.                      **/
    /***************************************************************/
    
    /* DIAGNOSTIC 
    printf("\n\nIDENTIFYING MEMBERS OF SUPERFAMILIES (remove fold members "
    "from superfamily list)\n");
    */

    iter= ajListIter(*supfamlist);
    
    /* Get the first node in the list, only once. */
    hit = (AjPScophit)ajListIterNext(iter);                     
    while((nexthit=(AjPScophit)ajListIterNext(iter)))
    {
	/* Check if the accession numbers are the same and if there is 
	   significant overlap. */
	/* DIAGNOSTIC 

	ajFmtPrint("Comparing %S (%d-%d, %B %B) to %S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2, 
		   nexthit->Acc, nexthit->Start, nexthit->End, nexthit->Target,
		   nexthit->Target2);
		   */

	if(embDmxScophitsOverlapAcc(hit,nexthit,sig_overlap))
	{
	    /* If Target2 is set for either hit then target both for removal. */
	    if(hit->Target2 || nexthit->Target2)
	    {
		ajDmxScophitTarget(&hit);
		ajDmxScophitTarget(&nexthit);
	    }


	    /* Are the super-families identical ? */
	    
	    /*
	     ** Overlaps between hits with same superfamily will already have been 
	     ** merged.  If they are detected now it is because we no longer 
	     ** do garbage collection immediately before this code block. Such
	     ** cases should be ignored, otherwise errors will result. 
	     */

	    /* Are the folds identical? */
	    if(ajStrMatch(hit->Fold,nexthit->Fold)&&
			 !(ajStrMatch(hit->Superfamily,nexthit->Superfamily))&&
			 !(ajStrMatch(hit->Family,nexthit->Family)))
	    {
		/* DIAGNOSTIC 
		ajFmtPrint("--> Overlap & folds identical! Merged hit goes to"
		" fold list, both are targetted\n");
		*/

		/* target both hits for removal. Place a hit corresponding 
		   to the merging of the 
		   two hits into the folds list. */
		embDmxScophitMergeInsertOther(*foldlist,hit,nexthit);


		/* Also place the merged hit into the supfam list so it can be 
		   considered for overlaps, but target it for removal. */
		
		embDmxScophitMergeInsertThisTarget(*supfamlist,hit,nexthit,iter);
		hit = (AjPScophit)ajListIterNext(iter);
		continue;
	    }
	    else
	    {
		/* DIAGNOSTIC 
		ajFmtPrint("--> Overlap & folds different! Both are targetted\n");
		*/

		/* Target both hits for removal. */
		ajDmxScophitTarget(&hit);
		ajDmxScophitTarget(&nexthit);

		
		/* Also place a merged hit into the supfam and fold list so it 
		   can be considered for overlaps, but target it for removal. */
		
		embDmxScophitMergeInsertThisTargetBoth(*supfamlist,hit,nexthit,iter);
		embDmxScophitMergeInsertOtherTargetBoth(*foldlist,hit,nexthit);
		hit = (AjPScophit)ajListIterNext(iter);
		continue;
	    }
	}
	else
	{
	    /* DIAGNOSTIC 
	    ajFmtPrint("--> Hits are distinct !\n");
	    */

	    /* Move one node along. */
	    hit = nexthit;
	}
    }
    
    /* The end of the list has been reached. */
    /* Delete hits in the list that are targeted for removal. */
    ajListGarbageCollect(*supfamlist, ajDmxScophitDelWrap, 
			 (int(*)(const void *)) ajDmxScophitCheckTarget);

    /* Sort list, first by superfamily, then by accession number, and finally
       by Start. */
    ajListSort3(*supfamlist, ajDmxScophitCompSfam, ajDmxScophitCompAcc, 
		ajDmxScophitCompStart);
    
    ajListIterFree(&iter);



    /* DIAGNOSTIC 

    ajFmtPrint("\n\n\nContents of FAMILIES list\n");
    iter= ajListIter(*famlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    ajFmtPrint("\nContents of SUPERFAMILIES list\n");
    iter= ajListIter(*supfamlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    ajFmtPrint("\nContents of FOLDS list\n");
    iter= ajListIter(*foldlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    */

    
    /*************************************************************/
    /** FIGURE A.7 Identify members of folds (merge overlapping **/
    /** hits and remove hits of  unknown classification).       **/ 
    /*************************************************************/

    /* DIAGNOSTIC 
    printf("\n\nIDENTIFYING MEMBERS OF FOLDS (merge overlapping hits)\n");
    */

    iter= ajListIter(*foldlist);
    
    /* Sort list, first by fold, then by accession number and finally by the 
       Start. */
    ajListSort3(*foldlist, ajDmxScophitCompFold, ajDmxScophitCompAcc, 
		ajDmxScophitCompStart);
    
    /* Get the first node in the list, only once. */ 
    hit = (AjPScophit)ajListIterNext(iter);	
    while((nexthit=(AjPScophit)ajListIterNext(iter)))
    {
	/* Check if the accession numbers are the same and if there is 
	   significant overlap. */
	if(embDmxScophitsOverlapAcc(hit,nexthit,sig_overlap))
	{
	    /* If Target2 is set for either hit then target both for removal. */
	    if(hit->Target2 || nexthit->Target2)
	    {
		ajDmxScophitTarget(&hit);
		ajDmxScophitTarget(&nexthit);
	    }

	    /* Are the folds identical - folds must be specified !*/
	    if( (MAJSTRLEN(hit->Fold)) && (MAJSTRLEN(nexthit->Fold)) 
	       && (ajStrMatch(hit->Fold,nexthit->Fold)))
	    {
		/* Target both hits for removal. Place a hit corresponding 
		   to the merging of the two hits into the folds (same) list. */
		embDmxScophitMergeInsertThis(*foldlist,hit,nexthit, iter);
		hit = (AjPScophit)ajListIterNext(iter);
	    }
	    else
	    {
		/* DIAGNOSTIC 
		ajFmtPrint("--> Overlap & folds different! Both are targetted\n");
		*/

		/* Target both hits for removal. */
		ajDmxScophitTarget(&hit);
		ajDmxScophitTarget(&nexthit);


		/* Also place a merged hit into the fold list so it can be 
		   considered for overlaps, but target it for removal. */
		
		embDmxScophitMergeInsertThisTargetBoth(*foldlist,hit,nexthit,iter);
		hit = (AjPScophit)ajListIterNext(iter);
		continue;
	    }
	}
	else
	    /* Move one node along. */
	    hit = nexthit;
    }	
    
    /* The end of the list has been reached. */
    /* Delete hits in the list that are targeted for removal. */
    ajListGarbageCollect(*foldlist, ajDmxScophitDelWrap, 
			 (int(*)(const void *)) ajDmxScophitCheckTarget);
    
    ajListSort3(*foldlist, ajDmxScophitCompSfam, ajDmxScophitCompAcc, 
		ajDmxScophitCompStart);
    
    ajListIterFree(&iter);

    
    /* DIAGNOSTIC 
    ajFmtPrint("\n\n\nContents of FAMILIES list\n");
    iter= ajListIter(*famlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    ajFmtPrint("\nContents of SUPERFAMILIES list\n");
    iter= ajListIter(*supfamlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    ajFmtPrint("\nContents of FOLDS list\n");
    iter= ajListIter(*foldlist);
    while((hit=(AjPScophit)ajListIterNext(iter)))
	ajFmtPrint("%S (%d-%d, %B %B)\n", 
		   hit->Acc, hit->Start, hit->End, hit->Target, hit->Target2);
    ajListIterFree(&iter);
    */

    while(ajListPop(hitslist,(void**)&tmp))
	embHitlistDel(&tmp);

    ajListDel(&hitslist);
    
    return ajTrue;
}




/* @funcstatic  seqsort_WriteOutputFiles **************************************
**
** Writes the contents of three lists of Scophit objects (for families, 
** superfamilies and folds) to file.  A domain ambiguities file (containing all 
** the hits that could NOT be uniquely assigned to a single family) and a 
** domain families file (containing only hits that were uniquely assigned to a 
** single family) are written, if specified by user.
**
** @param [w] fptr_fam   [AjPStr ]   Name of domain families file
** @param [w] fptr_amb   [AjPStr ]   Name of domain ambiguities file
** @param [r] dofam      [AjBool ]   Write domain ambiguities file (or not)
** @param [r] doamb      [AjBool ]   Write domain families file (or not)
** @param [r] famlist    [AjPList *] Families list.
** @param [r] supfamlist [AjPList *] Superfamilies list.
** @param [r] foldlist   [AjPList *] Folds list.
** @param [r] outdir     [AjPDir]    Output directory. 
** 
** @return [AjBool] ajTrue if the files were written, ajFalse otherwise.
** @@
******************************************************************************/
static AjBool seqsort_WriteOutputFiles(AjPFile fptr_fam,
				       AjPFile fptr_amb, 
				       AjBool dofam, 
				       AjBool doamb,
				       AjPList famlist, 
				       AjPList supfamlist, 
				       AjPList foldlist, 
				       AjPDir outdir)

{
    AjPHitlist hitlist = NULL;
    AjIList    iter    = NULL;
    AjPFile    outf    = NULL;
    AjPStr     temp    = NULL;

    /* Check args. */
    if(!famlist && !supfamlist && !foldlist)
	return ajFalse;
    


    if(doamb)
    {
	while((embDmxScophitsToHitlist(foldlist, &hitlist, &iter)))
	{
	    embHitlistWriteFasta(fptr_amb, hitlist);
	    embHitlistDel(&hitlist);
	}
    }
    
    if(iter)
    {
	ajListIterFree(&iter);	
	iter=NULL;
    }	
    
    
    if(doamb)
    {
	while((embDmxScophitsToHitlist(supfamlist, &hitlist, &iter)))
	{	
	    embHitlistWriteFasta(fptr_amb, hitlist);
	    embHitlistDel(&hitlist);
	    hitlist=NULL;
	}

	if(iter)
	{
	    ajListIterFree(&iter);
	    iter=NULL;
	}
    }
    
    
    while((embDmxScophitsToHitlist(famlist, &hitlist, &iter)))
    {
	if(dofam)
	    embHitlistWriteFasta(fptr_fam, hitlist);

	ajStrFromInt(&temp, hitlist->Sunid_Family);

	if(!(outf = ajFileNewOutDir(outdir, temp)))
	    ajFatal("Could not open output file in seqsort_WriteOutputFiles");
	embHitlistWriteFasta(outf, hitlist);
	
	ajFileClose(&outf);
	embHitlistDel(&hitlist);
	hitlist=NULL;
    }   
    if(iter)
    {
	ajListIterFree(&iter);
	iter=NULL;
    }

    ajStrDel(&temp);
        
    return ajTrue;
}





/* @funcstatic seqsort_HitlistPriorityHigh*************************************
**
** Sets the Priority element of a Hitlist object to ajTrue.
**
** @param [w] list    [AjPHitlist *] Hitlist object
**
** @return [AjBool] True on success, else False
** @@
******************************************************************************/

static AjBool seqsort_HitlistPriorityHigh(AjPHitlist *list)
{
    /* Check arg's. */
    if(!(*list))
    {
	ajWarn("Bad arg's passed to seqsort_HitlistPriorityHigh\n");
	return ajFalse;
    }
    

    (*list)->Priority = ajTrue;

    return ajTrue;
}




/* @funcstatic seqsort_HitlistPriorityLow *************************************
**
** Sets the Priority element of a Hitlist object to ajFalse
**
** @param [w] list    [AjPHitlist *] Hitlist object
**
** @return [AjBool] True on success, else False
** @@
******************************************************************************/

static AjBool seqsort_HitlistPriorityLow(AjPHitlist *list)
{
    /* Check arg's. */
    if(!(*list))
    {
	ajWarn("Bad arg's passed to seqsort_HitlistPriorityHigh\n");
	return ajFalse;
    }
    

    (*list)->Priority = ajFalse;

    return ajTrue;
}




/* @funcstatic seqsort_HitlistToThreeScophits *********************************
**
** Read from a list of Hitlist structures and writes to three lists of 
** Scophit structures (for families, superfamilies and folds) depending on 
** which SCOP nodes are specified. For example, if the scop family name is
** specified in a Hitlist (Family element of the Hitlist object != NULL) then 
** the hits would be written to the families list of Scophit structures. If 
** for example the Family and Superfamily element were both == NULL then the 
** hits would be written to the folds list.
** 
** @param [r] in     [AjPList]  List of Hitlist structures
** @param [w] fam    [AjPList*] List of Scophit structures (families)
** @param [w] sfam   [AjPList*] List of Scophit structures (superfamilies)
** @param [w] fold   [AjPList*] List of Scophit structures (folds)
**
** @return [AjBool] True on success (lists were processed ok)
** @@
******************************************************************************/

static AjBool seqsort_HitlistToThreeScophits(AjPList in, 
					     AjPList *fam, 
					     AjPList *sfam,
					     AjPList *fold)
{
    AjPScophit scophit = NULL;   /* Pointer to Scophit object. */
    AjPHitlist hitlist = NULL;   /* Pointer to Hitlist object. */
    AjIList iter       = NULL;   /* List iterator.             */
    ajint x = 0;                 /* Loop counter.              */
   

    /* Check args. */
    if(!in)
    {
	ajWarn("Null arg passed to seqsort_HitlistToThreeScophits");
	return ajFalse;
    }

    /* Create list iterator and new list. */
    iter = ajListIter(in);	
    

    /* Iterate through the list of Hitlist pointers. */
    while((hitlist=(AjPHitlist)ajListIterNext(iter)))
    {
	/* Loop for each hit in hitlist structure. */
	for(x=0; x<hitlist->N; ++x)
	{
	    /* Create a new scophit structure. */
	    /* AJNEW0(scophit); */
	    scophit = ajDmxScophitNew();

	    /* Assign scop classification records from hitlist structure. */
	    scophit->Type = hitlist->Type;
	    ajStrAssS(&scophit->Class, hitlist->Class);
	    ajStrAssS(&scophit->Fold, hitlist->Fold);
	    ajStrAssS(&scophit->Superfamily, hitlist->Superfamily);
	    ajStrAssS(&scophit->Family, hitlist->Family);
	    scophit->Sunid_Family = hitlist->Sunid_Family;
	    

	    /* Assign records from hit structure. */
	    ajStrAssS(&scophit->Seq, hitlist->hits[x]->Seq);
	    ajStrAssS(&scophit->Acc, hitlist->hits[x]->Acc);
	    ajStrAssS(&scophit->Spr, hitlist->hits[x]->Spr);
	    ajStrAssS(&scophit->Model, hitlist->hits[x]->Model);
	    ajStrAssS(&scophit->Alg, hitlist->hits[x]->Alg);
	    ajStrAssS(&scophit->Group, hitlist->hits[x]->Group);
	    scophit->Start = hitlist->hits[x]->Start;
	    scophit->End = hitlist->hits[x]->End;
	    scophit->Rank = hitlist->hits[x]->Rank;
	    scophit->Score = hitlist->hits[x]->Score;
	    scophit->Eval = hitlist->hits[x]->Eval;
	    scophit->Pval = hitlist->hits[x]->Pval;
	    
	    
	    /* Push scophit onto list. */
	    if(scophit->Family)
		ajListPushApp(*fam,scophit);
	    else if(scophit->Superfamily)
		ajListPushApp(*sfam,scophit);	    
	    else if(scophit->Fold)
		ajListPushApp(*fold,scophit);	
	    else
	    {
		ajWarn("Family, superfamily and fold not specified "
		       "for hit in seqsort_HitlistToThreeScophits\n");
		ajDmxScophitDel(&scophit);
	    }
	}
    }	
    

    ajListIterFree(&iter);	

    return ajTrue;
}



/* ****************************************************************************
**
** Dummy function to prevent compiler grumbling.
** 
******************************************************************************/

void seqsort_unused(void)
{
    seqsort_HitlistToThreeScophits(NULL, NULL, NULL, NULL);
    seqsort_HitlistPriorityLow(NULL);
    seqsort_HitlistPriorityHigh(NULL);
}








