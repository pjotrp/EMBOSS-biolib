/****************************************************************************
** 
** @source AJAX low-level functions for handling protein domain data.  
** For use with the Scop and Cath objects.
**
** @author: Copyright (C) 2004 Jon Ison (jison@hgmp.mrc.ac.uk)
** @version 1.0 
** @@
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
** 
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
****************************************************************************/





/* ======================================================================= */
/* ============================ include files ============================ */
/* ======================================================================= */

#include "ajax.h"





/* ======================================================================= */
/* ============================ private data ============================= */
/* ======================================================================= */





/* ======================================================================= */
/* ================= Prototypes for private functions ==================== */
/* ======================================================================= */




/* ======================================================================= */
/* ========================== private functions ========================== */
/* ======================================================================= */





/* ======================================================================= */
/* =========================== constructors ============================== */
/* ======================================================================= */

/* @section Constructors ****************************************************
**
** These constructors return a pointer to a new instance of an object.
**
****************************************************************************/

/* @func ajCathNew **********************************************************
**
** Cath object constructor. Fore-knowledge of the number of chain segments
** the domain is comprised of is required. 
**
** @param [r] n [ajint] No. of chain segments
** 
** @return [AjPCath] Pointer to a Cath object
**
****************************************************************************/

AjPCath ajCathNew(ajint n)
{
    AjPCath ret = NULL;
    ajint x = 0;
    
    AJNEW0(ret);

    
    ret->DomainID     = ajStrNew();
    ret->Pdb          = ajStrNew();  
    ret->Class        = ajStrNew();  
    ret->Architecture = ajStrNew();  
    ret->Topology     = ajStrNew();  
    ret->Superfamily  = ajStrNew();  

    
    if(n > 0)
    {
	AJCNEW0(ret->Start, n);
	AJCNEW0(ret->End, n);
	
	for(x=0; x<n; x++)
	{
	    ret->Start[x] = ajStrNew();
	    ret->End[x]   = ajStrNew();
	}
    }
    
    ret->NSegment = n;
    
    return ret;
}





/* @func ajScopNew **********************************************************
**
** Scop object constructor. Fore-knowledge of the number of chains is 
** required. This is normally called by the embScopReadCNew / embScopReadNew
** functions.
**
** @param [r] chains [ajint] Number of chains
**
** @return [AjPScop] Pointer to a Scop object
** @@
****************************************************************************/

AjPScop ajScopNew(ajint chains)
{

    AjPScop ret = NULL;
    ajint i;

    AJNEW0(ret);

    ret->Entry       = ajStrNew();
    ret->Pdb         = ajStrNew();
    ret->Class       = ajStrNew();
    ret->Fold        = ajStrNew();
    ret->Superfamily = ajStrNew();
    ret->Family      = ajStrNew();  
    ret->Domain      = ajStrNew();  
    ret->Source      = ajStrNew();
    ret->Acc         = ajStrNew();
    ret->Spr         = ajStrNew();
    ret->SeqPdb      = ajStrNew();
    ret->SeqSpr      = ajStrNew();

    if(chains)
    {
	ret->Chain=ajCharNewL(chains);
	AJCNEW0(ret->Start,chains);
	AJCNEW0(ret->End,chains);
	for(i=0; i<chains; i++)
	{
	    ret->Start[i]=ajStrNew();
	    ret->End[i]=ajStrNew();
	}
    }

    ret->N = chains;

    return ret;
}





/* ======================================================================= */
/* =========================== destructors =============================== */
/* ======================================================================= */

/* @section Structure Destructors *******************************************
**
** These destructors functions receive the address of the instance to be
** deleted.  The original pointer is set to NULL so is ready for re-use.
**
****************************************************************************/

/* @func ajScopDel **********************************************************
**
** Destructor for scop object.
**
** @param [w] ptr [AjPScop*] Scop object pointer
**
** @return [void]
** @@
****************************************************************************/

void ajScopDel(AjPScop *ptr)
{
    AjPScop pthis;
    ajint i;

    pthis = *ptr;

    if(!pthis || !ptr)
	return;

    ajStrDel(&pthis->Entry);
    ajStrDel(&pthis->Pdb);
    ajStrDel(&pthis->Class);
    ajStrDel(&pthis->Fold);
    ajStrDel(&pthis->Superfamily);
    ajStrDel(&pthis->Family);
    ajStrDel(&pthis->Domain);
    ajStrDel(&pthis->Source);
    ajStrDel(&pthis->Acc);
    ajStrDel(&pthis->Spr);
    ajStrDel(&pthis->SeqPdb);
    ajStrDel(&pthis->SeqSpr);


    if(pthis->N)
    {
	for(i=0; i<pthis->N; i++)
	{
	    ajStrDel(&pthis->Start[i]);
	    ajStrDel(&pthis->End[i]);
	}
	AJFREE(pthis->Start);
	AJFREE(pthis->End);
	AJFREE(pthis->Chain);
    }

    AJFREE(pthis);
    (*ptr) = NULL;

    return;
}





/* @func ajCathDel **********************************************************
**
** Destructor for Cath object. Fore-knowledge of the number of chain segments
** domain is comprised of is required. 
**
** @param [w] ptr [AjPCath *] Cath object pointer
**
** @return [void]
**
****************************************************************************/ 

void ajCathDel(AjPCath *ptr)
{
    AjPCath pthis;
    ajint x;

    pthis = *ptr;
    
    if(!pthis || !ptr)     /* (pthis==NULL)||(ptr==NULL) */
	return;
  
    ajStrDel(&pthis->DomainID);     
    ajStrDel(&pthis->Pdb);         
    ajStrDel(&pthis->Class);        
    ajStrDel(&pthis->Architecture); 
    ajStrDel(&pthis->Topology);      
    ajStrDel(&pthis->Superfamily);   

    if(pthis->NSegment)
    {
	for(x=0; x<pthis->NSegment; x++)
	{
	    ajStrDel(&pthis->Start[x]);
	    ajStrDel(&pthis->End[x]);    
	}
    
	AJFREE(pthis->Start);
	AJFREE(pthis->End);
    }
    AJFREE(pthis);

    pthis  = NULL;
    
    (*ptr) = NULL;
    
    return;
} 
 




/* ======================================================================= */
/* ============================ Assignments ============================== */
/* ======================================================================= */

/* @section Assignments *****************************************************
**
** These functions overwrite the instance provided as the first argument
** A NULL value is always acceptable so these functions are often used to
** create a new instance by assignment.
**
****************************************************************************/

/* @func ajScopCopy *********************************************************
**
** Copies the contents from one Scop object to another.
**
** @param [w] to   [AjPScop*] Scop object pointer 
** @param [r] from [AjPScop]  Scop object 
**
** @return [AjBool] True if copy was successful.
** @@
****************************************************************************/

AjBool ajScopCopy(AjPScop *to, AjPScop from)
{
    ajint x = 0;
    
    /* Check args */
    if(!from)
	return ajFalse;

    if(!(*to))
	(*to) = ajScopNew(from->N);
    

    ajStrAssS(&(*to)->Entry, from->Entry);
    ajStrAssS(&(*to)->Pdb, from->Pdb);
    ajStrAssS(&(*to)->Class, from->Class);
    ajStrAssS(&(*to)->Fold, from->Fold);
    ajStrAssS(&(*to)->Superfamily, from->Superfamily);
    ajStrAssS(&(*to)->Family, from->Family);
    ajStrAssS(&(*to)->Domain, from->Domain);
    ajStrAssS(&(*to)->Source, from->Source);

    for(x=0; x<from->N; x++)
    {
	(*to)->Chain[x] = from->Chain[x];
	ajStrAssS(&(*to)->Start[x], from->Start[x]);	
	ajStrAssS(&(*to)->End[x], from->End[x]);	
    }
    
    ajStrAssS(&(*to)->Acc, from->Acc);
    ajStrAssS(&(*to)->Spr, from->Spr);
    ajStrAssS(&(*to)->SeqPdb, from->SeqPdb);
    ajStrAssS(&(*to)->SeqSpr, from->SeqSpr);
    (*to)->Startd = from->Startd;
    (*to)->Endd   = from->Endd;

    (*to)->Sunid_Class = from->Sunid_Class;
    (*to)->Sunid_Fold  = from->Sunid_Fold;
    (*to)->Sunid_Superfamily = from->Sunid_Superfamily;
    (*to)->Sunid_Family = from->Sunid_Family;
    (*to)->Sunid_Domain = from->Sunid_Domain;
    (*to)->Sunid_Source = from->Sunid_Source;
    (*to)->Sunid_Domdat = from->Sunid_Domdat;

    return ajTrue;
}





/* ======================================================================= */
/* ============================= Modifiers =============================== */
/* ======================================================================= */

/* @section Modifiers *******************************************************
**
** These functions use the contents of an instance and update them.
**
****************************************************************************/





/* ======================================================================= */
/* ========================== Operators ===================================*/
/* ======================================================================= */

/* @section Operators *******************************************************
**
** These functions use the contents of an instance but do not make any 
** changes.
**
****************************************************************************/





/* @func ajScopMatchSunid ***************************************************
**
** Function to sort Scop object by Sunid_Family.
**
** @param [r] entry1  [const void*] Pointer to Scop object 1
** @param [r] entry2  [const void*] Pointer to Scop object 2
**
** @return [ajint] -1 if Start1 should sort before Start2, +1 if the Start2 
** should sort first. 0 if they are identical.
** @@
****************************************************************************/
ajint ajScopMatchSunid(const void *entry1, const void *entry2)
{
    AjPScop p = NULL;
    AjPScop q = NULL;

    p = (*(AjPScop*)entry1);
    q = (*(AjPScop*)entry2);
   

    if(p->Sunid_Family < q->Sunid_Family)
        return -1;
    else if(p->Sunid_Family == q->Sunid_Family)
        return 0;

    return 1;
}





/* @func ajScopMatchScopid **************************************************
**
** Function to sort Scop object by Entry element. 
**
** @param [r] hit1  [const void*] Pointer to Scop object 1
** @param [r] hit2  [const void*] Pointer to Scop object 2
**
** @return [ajint] -1 if Entry1 should sort before Entry2, +1 if the Entry2 
** should sort first. 0 if they are identical in length and content. 
** @@
****************************************************************************/

ajint ajScopMatchScopid(const void *hit1, const void *hit2)
{
    AjPScop p = NULL;
    AjPScop q = NULL;

    p = (*(AjPScop*)hit1);
    q = (*(AjPScop*)hit2);
    
    return ajStrCmpO(p->Entry, q->Entry);
}





/* @func ajScopMatchPdbId ***************************************************
**
** Function to sort Scop object by Pdb element. 
**
** @param [r] hit1  [const void*] Pointer to Scop object 1
** @param [r] hit2  [const void*] Pointer to Scop object 2
**
** @return [ajint] -1 if Pdb1 should sort before Pdb2, +1 if the Pdb2 
** should sort first. 0 if they are identical in length and content. 
** @@
****************************************************************************/

ajint ajScopMatchPdbId(const void *hit1, const void *hit2)
{
    AjPScop p = NULL;
    AjPScop q = NULL;

    p = (*(AjPScop*)hit1);
    q = (*(AjPScop*)hit2);
    
    return ajStrCmpO(p->Pdb, q->Pdb);
}





/* @func ajCathMatchPdbid ***************************************************
**
** Function to sort Cath object by Pdb element. 
**
** @param [r] hit1  [const void*] Pointer to Cath object 1
** @param [r] hit2  [const void*] Pointer to Cath object 2
**
** @return [ajint] -1 if Pdb1 should sort before Pdb2, +1 if the Pdb2 
** should sort first. 0 if they are identical in length and content. 
** @@
****************************************************************************/

ajint ajCathMatchPdbid(const void *hit1, const void *hit2)
{
    AjPCath p = NULL;
    AjPCath q = NULL;

    p = (*(AjPCath*)hit1);
    q = (*(AjPCath*)hit2);
    
    return ajStrCmpO(p->Pdb, q->Pdb);
}





/* ======================================================================= */
/* ============================== Casts ===================================*/
/* ======================================================================= */

/* @section Casts ***********************************************************
**
** These functions examine the contents of an instance and return some
** derived information. Some of them provide access to the internal
** components of an instance. 
**
****************************************************************************/





/* ======================================================================= */
/* =========================== Reporters ==================================*/
/* ======================================================================= */

/* @section Reporters *******************************************************
**
** These functions return the contents of an instance but do not make any 
** changes.
**
****************************************************************************/

/* @func ajScopArrFindScopid ************************************************
**
** Performs a binary search for a SCOP domain id over an array of Scop
** structures (which of course must first have been sorted). This is a 
** case-insensitive search.
**
** @param [r] id  [AjPStr]      Search term
** @param [r] arr [AjPScop*]    Array of AjPScop objects
** @param [r] siz [ajint]       Size of array
**
** @return [ajint] Index of first Scop object found with a SCOP domain id
** matching id, or -1 if id is not found.
** @@
****************************************************************************/
ajint ajScopArrFindScopid(AjPStr id, AjPScop *arr, ajint siz)

{
    int l;
    int m;
    int h;
    int c;


    l = 0;
    h = siz-1;
    while(l<=h)
    {
        m = (l+h)>>1;

        if((c = ajStrCmpCase(id, arr[m]->Entry)) < 0) 
	    h = m-1;
        else if(c>0) 
	    l = m+1;
        else 
	    return m;
    }

    return -1;
}





/* @func ajScopArrFindSunid *************************************************
**
** Performs a binary search for a SCOP sunid over an array of Scop
** objects (which of course must first have been sorted). This is a 
** case-insensitive search.
**
** @param [r] id  [ajint]       Search term
** @param [r] arr [AjPScop*]    Array of Scop objects
** @param [r] siz [ajint]       Size of array
**
** @return [ajint] Index of first Scop object found with an PDB code
** matching id, or -1 if id is not found.
** @@
****************************************************************************/

ajint ajScopArrFindSunid(ajint id, AjPScop *arr, ajint siz)
{
    int l;
    int m;
    int h;

    l = 0;
    h = siz-1;
    while(l<=h)
    {
        m=(l+h)>>1;
        
        if(id < arr[m]->Sunid_Family)
            h=m-1;
        else if(id > arr[m]->Sunid_Family)
            l=m+1;
        else 
            return m;
    }

    return -1;
}





/* @func ajScopArrFindPdbid *************************************************
**
** Performs a binary search for a SCOP domain id over an array of Scop
** objects (which of course must first have been sorted). This is a 
** case-insensitive search.
**
** @param [r] id  [AjPStr]      Search term
** @param [r] arr [AjPScop*]    Array of AjPScop objects
** @param [r] siz [ajint]       Size of array
**
** @return [ajint] Index of first Scop object found with a PDB code
** matching id, or -1 if id is not found.
** @@
****************************************************************************/

ajint ajScopArrFindPdbid(AjPStr id, AjPScop *arr, ajint siz)
{
    int l;
    int m;
    int h;
    int c;


    l = 0;
    h = siz-1;
    while(l<=h)
    {
        m = (l+h)>>1;

        if((c=ajStrCmpCase(id, arr[m]->Pdb)) < 0) 
	    h = m-1;
        else if(c>0) 
	    l = m+1;
        else 
	    return m;
    }

    return -1;
}





/* @func ajCathArrFindPdbid ************************************************
**
** Performs a binary search for a CATH domain id over an array of Cath
** structures (which of course must first have been sorted). This is a 
** case-insensitive search.
**
** @param [r] id  [AjPStr]      Search term
** @param [r] arr [AjPCath*]    Array of AjPCath objects
** @param [r] siz [ajint]       Size of array
**
** @return [ajint] Index of first Cath object found with a PDB code
** matching id, or -1 if id is not found.
** @@
****************************************************************************/

ajint ajCathArrFindPdbid(AjPStr id, AjPCath *arr, ajint siz)
{
    int l;
    int m;
    int h;
    int c;


    l = 0;
    h = siz-1;
    while(l<=h)
    {
        m=(l+h)>>1;

        if((c=ajStrCmpCase(id, arr[m]->Pdb)) < 0) 
	    h = m-1;
        else if(c>0) 
	    l = m+1;
        else 
	    return m;
    }

    return -1;
}





/* ======================================================================= */
/* ========================== Input & Output ============================= */
/* ======================================================================= */

/* @section Input and Output ************************************************
**
** These functions use the contents of an instance but do not make any 
** changes.
**
****************************************************************************/





/* ======================================================================= */
/* ======================== Miscellaneous =================================*/
/* ======================================================================= */

/* @section Miscellaneous ***************************************************
**
** These functions may have diverse functions that do not fit into the other
** categories. 
**
****************************************************************************/



