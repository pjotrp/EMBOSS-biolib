/****************************************************************************
**
** @source AJAX low-level functions for handling protein structural data.  
** For use with the Atom, Chain and Pdb objects.
** Also for Hetent, Het, Vdwres, Vdwall, Cmap and Pdbtosp objects.
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
** All constructors return a pointer to a new instance. It is the 
** responsibility of the user to first destroy any previous instance. The 
** target pointer does not need to be initialised to NULL, but it is good 
** programming practice to do so anyway.
**
****************************************************************************/

/* @func ajAtomNew *******************************************************
**
** Atom object constructor.
** This is normally called by the ajChainNew function.
**
** @return [AjPAtom] Pointer to an atom object
** @@
****************************************************************************/

AjPAtom ajAtomNew(void)
{
    AjPAtom ret = NULL;

    AJNEW0(ret);
    
    ret->Id3   = ajStrNew();
    ret->Atm   = ajStrNew();
    ret->Pdb   = ajStrNew();
    ret->eId   = ajStrNew();

    ret->Id1   = '.';
    ret->eType = '.';
    ajStrAssC(&ret->eId, ".");
    ret->eStrideType = '.';
    

    return ret;
}





/* @func ajChainNew *********************************************************
**
** Chain object constructor. 
** This is normally called by the ajPdbNew function
**
** @return [AjPChain] Pointer to a chain object
** @@
****************************************************************************/

AjPChain ajChainNew(void)
{
    AjPChain ret = NULL;
  
    AJNEW0(ret);

    ret->Seq    = ajStrNewC("");
    ret->Atoms  = ajListNew();

    return ret;
}





/* @func ajPdbNew ***********************************************************
**
** Pdb object constructor. Fore-knowledge of the number of chains 
** is required. This is normally called by the functions that read PDB 
** files or clean coordinate files (see embpdb.c & embpdbraw.c).
**
** @param [r] chains [ajint] Number of chains in this pdb file
**
** @return [AjPPdb] Pointer to a pdb object
** @@
****************************************************************************/

AjPPdb ajPdbNew(ajint n)
{
    AjPPdb ret = NULL;
    ajint i;
    
    AJNEW0(ret);
  

    ret->Pdb    = ajStrNew();
    ret->Compnd = ajStrNew();
    ret->Source = ajStrNew();
    ret->Groups = ajListNew();
    ret->Water  = ajListNew();
    ret->gpid   = ajChararrNew();
    

    if(n)
    {	
	AJCNEW0(ret->Chains,n);
	for(i=0;i<n;++i)
	    ret->Chains[i] = ajChainNew();
    }

    return ret;
}


/* @func ajHetentNew ********************************************************
**
** Hetent object constructor. 
**
** @return [AjPHetent] Pointer to a Hetent object
** @@
****************************************************************************/

AjPHetent ajHetentNew(void)
{
    AjPHetent ret = NULL;
    
    AJNEW0(ret);
    
    /* Create strings */
    ret->abv = ajStrNew();
    ret->syn = ajStrNew();
    ret->ful = ajStrNew();
    
    return ret;
}





/* @func ajHetNew ***********************************************************
**
** Het object constructor. 
**
** @param [r] n [ajint] Number of entries in dictionary.
** 
** @return [AjPHet] Pointer to a Het object
** @@
****************************************************************************/

AjPHet ajHetNew(ajint n)
{
    ajint i    = 0;
    AjPHet ret = NULL;
    
    AJNEW0(ret);

    if(n)
    {
	ret->n = n;
	AJCNEW0(ret->entries, n);
	for(i=0;i<n;i++)
	    ret->entries[i]=ajHetentNew();
    }
    else
    {
	ret->n = 0;
	ret->entries = NULL;
    }
    

    return ret;
}





/* @func ajVdwallNew ********************************************************
**
** Vdwall object constructor. This is normally called by the embVdwallReadNew
** function. Fore-knowledge of the number of residues is required.
**
** @param  [r] n [ajint]  Number of residues
**
** @return [AjPVdwall] Pointer to a Vdwall object
** @@
****************************************************************************/

AjPVdwall ajVdwallNew(ajint n)
{
    AjPVdwall ret = NULL;
    
    AJNEW0(ret);

    ret->N = n;

    if(n)
	AJCNEW0(ret->Res, n);


    return ret;
}





/* @func ajVdwresNew ********************************************************
**
** Vdwres object constructor. This is normally called by the embVdwallReadNew
** function. Fore-knowledge of the number of atoms is required.
**
** @param  [r] n [ajint]  Number of atoms
**
** @return [AjPVdwres] Pointer to a Vdwres object
** @@
****************************************************************************/

AjPVdwres  ajVdwresNew(ajint n)
{
    ajint x;
    AjPVdwres ret = NULL;
    
    AJNEW0(ret);

    ret->Id3 = ajStrNew();    
    ret->N   = n;

    if(n)
    {
	AJCNEW0(ret->Atm, n);
	for(x=0;x<n;++x)
	    ret->Atm[x]=ajStrNew();

	AJCNEW0(ret->Rad, n);
    }

    return ret;
}





/* @func ajCmapNew **********************************************************
**
** Cmap object constructor. This is normally called by the embCmapReadNew
** function. Fore-knowledge of the dimension (number of residues) for the 
** contact map is required.
**
** @param  [r] n [ajint]   Dimenion of contact map
**
** @return [AjPCmap] Pointer to a Cmap object
** 
** @@
****************************************************************************/

AjPCmap ajCmapNew(ajint n)
{
    AjPCmap ret = NULL;
    ajint z = 0;
    

    AJNEW0(ret);

    ret->Id  = ajStrNew();    	
    ret->Seq = ajStrNew();
    

    if(n)
    {
	/* Create the SQUARE contact map */
	ret->Mat = ajInt2dNewL((ajint)n);
	for(z=0;z<n;++z)
	    ajInt2dPut(&ret->Mat, z, n-1, (ajint) 0);
    }

    ret->Dim  = n;
    ret->Ncon = 0;

    return ret;
}





/* @func ajPdbtospNew *******************************************************
**
** Pdbtosp object constructor. Fore-knowledge of the number of entries is 
** required. This is normally called by the embPdbtospReadCNew / 
** embPdbtospReadNew functions.
**
** @param [r] n [ajint] Number of entries
**
** @return [AjPPdbtosp] Pointer to a Pdbtosp object
** @@
****************************************************************************/

AjPPdbtosp ajPdbtospNew(ajint n)
{

    AjPPdbtosp ret = NULL;
    ajint i;

    AJNEW0(ret);

    ret->Pdb  = ajStrNew();

    if(n)
    {
	AJCNEW0(ret->Acc,n);
	AJCNEW0(ret->Spr,n);
	for(i=0; i<n; i++)
	{
	    ret->Acc[i]=ajStrNew();
	    ret->Spr[i]=ajStrNew();
	}
    }

    ret->n = n;

    return ret;
}





/* ======================================================================= */
/* =========================== destructors =============================== */
/* ======================================================================= */

/* @section Destructors *****************************************************
**
** All destructor functions receive the address of the instance to be
** deleted.  The original pointer is set to NULL so is ready for re-use.
**
****************************************************************************/


/* @func ajAtomDel **********************************************************
**
** Destructor for atom object.
**
** @param [w] ptr [AjPAtom*] Atom object pointer
**
** @return [void]
** @@
****************************************************************************/

void ajAtomDel(AjPAtom *ptr)
{
    AjPAtom pthis;

    pthis = *ptr;

    if(!ptr || !pthis)
	return;

    ajStrDel(&pthis->Id3);
    ajStrDel(&pthis->Atm);
    ajStrDel(&pthis->Pdb);
    ajStrDel(&pthis->eId);

    AJFREE(pthis);
    (*ptr) = NULL;

    return;
}





/* @func ajChainDel *********************************************************
**
** Destructor for chain object.
**
** @param [w] ptr [AjPChain*] Chain object pointer
**
** @return [void]
** @@
****************************************************************************/

void ajChainDel(AjPChain *ptr)
{
    AjPChain pthis;
    AjPAtom atm = NULL;

    pthis = *ptr;

    if(!ptr || !pthis)
	return;
    
    while(ajListPop(pthis->Atoms,(void **)&atm))
	ajAtomDel(&atm);

    ajStrDel(&pthis->Seq);
    ajListDel(&pthis->Atoms);

    AJFREE(pthis);
    (*ptr) = NULL;

    return;
}





/* @func ajPdbDel ***********************************************************
**
** Destructor for pdb object.
**
** @param [w] ptr [AjPPdb*] Pdb object pointer
**
** @return [void]
** @@
****************************************************************************/

void ajPdbDel(AjPPdb *ptr)
{
    AjPPdb pthis;
    AjPAtom atm = NULL;

    ajint nc = 0;
    ajint i  = 0;

    pthis = *ptr;

    if(!pthis || !ptr)
	return;
    
    nc = pthis->Nchn;

    ajStrDel(&pthis->Pdb);
    ajStrDel(&pthis->Compnd);
    ajStrDel(&pthis->Source);

    ajChararrDel(&pthis->gpid);
    
    
    while(ajListPop(pthis->Water,(void **)&atm))
	ajAtomDel(&atm);
    ajListDel(&pthis->Water);

    while(ajListPop(pthis->Groups,(void **)&atm))
	ajAtomDel(&atm);
    ajListDel(&pthis->Groups);
    
    
    for(i=0;i<nc;++i)
	ajChainDel(&pthis->Chains[i]);
    AJFREE(pthis->Chains);

    AJFREE(pthis);
    (*ptr) = NULL;

    return;
}





/* @func ajHetentDel ********************************************************
**
** Destructor for Hetent object.
**
** @param [w] ptr [AjPHetent*] Hetent object pointer
**
** @return [void]
** @@
****************************************************************************/

void ajHetentDel(AjPHetent *ptr)
{
    /* Check arg's */
    if(*ptr==NULL)
	return;

   
    if((*ptr)->abv)
	ajStrDel( &((*ptr)->abv)); 

    if((*ptr)->syn)
	ajStrDel( &((*ptr)->syn)); 

    if((*ptr)->ful)
	ajStrDel( &((*ptr)->ful)); 

    AJFREE(*ptr);
    *ptr = NULL;
    
    return;
}





/* @func ajHetDel ***********************************************************
**
** Destructor for Het object.
**
** @param [w] ptr [AjPHet*] Het object pointer
**
** @return [void]
** @@
****************************************************************************/

void ajHetDel(AjPHet *ptr)
{
    ajint i = 0;
    
    /* Check arg's */
    if(*ptr==NULL)
	return;

    if((*ptr)->entries)
    {
        for(i=0;i<(*ptr)->n;i++)
	{
	    if((*ptr)->entries[i])
		ajHetentDel(&((*ptr)->entries[i]));
	}	
	
	AJFREE((*ptr)->entries);
    }
    AJFREE(*ptr);
    *ptr = NULL;

    return;
}




/* @func ajVdwallDel ********************************************************
**
** Destructor for Vdwall object.
**
** @param [w] ptr [AjPVdwall*] Vdwall object pointer
**
** @return [void]
** @@
****************************************************************************/

void ajVdwallDel(AjPVdwall *ptr)
{
    ajint x = 0;
    
    for(x=0;x<(*ptr)->N; ++x)
	ajVdwresDel(&(*ptr)->Res[x]);
    
    AJFREE((*ptr)->Res);
    AJFREE(*ptr);    
    *ptr = NULL;

    return;
}	





/* @func ajVdwresDel ********************************************************
**
** Destructor for Vdwres object.
**
** @param [w] ptr [AjPVdwres*] Vdwres object pointer
**
** @return [void]
** @@
****************************************************************************/

void ajVdwresDel(AjPVdwres *ptr)
{
    ajint x = 0;
    
    ajStrDel(&(*ptr)->Id3);
    
    for(x=0;x<(*ptr)->N; ++x)
	ajStrDel(&(*ptr)->Atm[x]);
    
    AJFREE((*ptr)->Atm);
    AJFREE((*ptr)->Rad);
    AJFREE(*ptr);    
    *ptr = NULL;

    return;
}	





/* @func ajCmapDel **********************************************************
**
** Destructor for Cmap object.
**
** @param [w] ptr [AjPCmap*] Cmap object pointer
**
** @return [void]
** @@
****************************************************************************/

void ajCmapDel(AjPCmap *ptr)
{
    if((*ptr)->Id)
	ajStrDel(&(*ptr)->Id);

    if((*ptr)->Seq)
	ajStrDel(&(*ptr)->Seq);

    if((*ptr)->Mat)
	ajInt2dDel(&(*ptr)->Mat);

    if((*ptr))
	AJFREE(*ptr);    

    *ptr = NULL;

    return;
}	





/* @func ajPdbtospDel *******************************************************
**
** Destructor for Pdbtosp object.
**
** @param [w] ptr [AjPPdbtosp*] Pdbtosp object pointer
**
** @return [void]
** @@
****************************************************************************/

void ajPdbtospDel(AjPPdbtosp *ptr)
{
    AjPPdbtosp pthis;
    ajint i;

    pthis = *ptr;

    if(!pthis || !ptr)
	return;

    ajStrDel(&pthis->Pdb);

    if(pthis->n)
    {
	for(i=0; i<pthis->n; i++)
	{
	    ajStrDel(&pthis->Acc[i]);
	    ajStrDel(&pthis->Spr[i]);
	}
	AJFREE(pthis->Acc);
	AJFREE(pthis->Spr);
    }

    AJFREE(pthis);
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

/* @func ajAtomCopy *********************************************************
**
** Copies the data from an Atom object to an Atom object; the new object
** is created if needed. 
** 
** IMPORTANT - THIS DOES NOT COPY THE eNum & eType ELEMENTS, WHICH ARE SET 
** TO ZERO and '.' INSTEAD.
** 
** @param [w] to   [AjPAtom*]  Atom object pointer
** @param [r] from [AjPAtom]   Atom object pointer
**
** @return [AjBool] True on success
** @@
****************************************************************************/

AjBool ajAtomCopy(AjPAtom *to, AjPAtom from)
{
    if(!to)
    {
	ajWarn("Bad arg (NULL) passed to ajAtomCopy");
	return ajFalse;
    }

    if(!(*to))
	*to = ajAtomNew();

    (*to)->Mod   = from->Mod;
    (*to)->Chn   = from->Chn;    
    (*to)->Gpn   = from->Gpn;
    (*to)->Type  = from->Type;
    (*to)->Idx   = from->Idx;
    ajStrAssS(&((*to)->Pdb), from->Pdb);
    (*to)->Id1   = from->Id1;
    ajStrAssS(&((*to)->Id3), from->Id3);
    ajStrAssS(&((*to)->Atm), from->Atm);
    (*to)->X     = from->X;
    (*to)->Y     = from->Y;
    (*to)->Z     = from->Z;
    (*to)->O     = from->O;
    (*to)->B     = from->B;

    (*to)->eNum  = from->eNum; 
    ajStrAssS(&((*to)->eId), from->eId);
    (*to)->eType = from->eType;
    (*to)->eClass= from->eClass; 

    (*to)->eStrideNum  = from->eStrideNum; 
    (*to)->eStrideType = from->eStrideType; 

    (*to)->all_abs  = from->all_abs; 
    (*to)->all_rel  = from->all_rel; 
    (*to)->side_abs = from->side_abs; 
    (*to)->side_rel = from->side_rel; 
    (*to)->main_abs = from->main_abs;     
    (*to)->main_rel = from->main_rel; 
    (*to)->npol_abs = from->npol_abs; 
    (*to)->npol_rel = from->npol_rel; 
    (*to)->pol_abs  = from->pol_abs; 
    (*to)->pol_rel  = from->pol_rel; 


    return ajTrue;
}





/* @func ajAtomListCopy *****************************************************
**
** Read a list of Atom structures and writes a copy of the list.  The 
** data are copied and the list is created if necessary.
** 
** @param [r] from     [AjPList]   List of Atom objects to read
** @param [w] to       [AjPList *] List of Atom objects to write
**
** @return [AjBool] True if list was copied ok.
** @@
****************************************************************************/

AjBool ajAtomListCopy(AjPList *to, AjPList from)
{
/* AjPList ret  = NULL; */
   AjIList iter = NULL;
   AjPAtom hit  = NULL;
   AjPAtom new  = NULL;

   /* Check arg's */
   if(!from || !to)
   {
       ajWarn("Bad arg's passed to ajAtomListCopy\n");
       return ajFalse;
   }
    
   /* Allocate the new list, if necessary */
   if(!(*to))
       *to=ajListNew();
    
   /* Initialise the iterator */
   iter=ajListIter(from);
    
   /* Iterate through the list of Atom objects */
   while((hit=(AjPAtom)ajListIterNext(iter)))
   {
       new=ajAtomNew();
	
       ajAtomCopy(&new, hit);

       /* Push scophit onto list */
       ajListPushApp(*to, new);
   }


   ajListIterFree(iter);
   return ajTrue; 
}





/* @func ajPdbCopy **********************************************************
**
** Copies data from one Pdb object to another; the new object is 
** always created. 
** 
** 
** @param [w] to   [AjPPdb*] Pdb object pointer
** @param [r] from [AjPPdb]  Pdb object pointer
**
** @return [AjBool] True on success
** @@
****************************************************************************/

AjBool ajPdbCopy(AjPPdb *to, AjPPdb from)
{
    ajint x = 0;
    

    if(!from)
    {
	ajWarn("NULL arg passed to ajPdbCopy");
	return ajFalse;
    }

    if((*to))
    {
	ajWarn("Pointer passed to ajPdbCopy should be NULL but isn't !");
	return ajFalse;
    }	


    /* Copy data in Pdb object */
    (*to) = ajPdbNew(from->Nchn);
    ajStrAssS(&((*to)->Pdb), from->Pdb);
    ajStrAssS(&((*to)->Compnd), from->Compnd);
    ajStrAssS(&((*to)->Source), from->Source);
    (*to)->Method = from->Method;
    (*to)->Reso   = from->Reso;
    (*to)->Nmod   = from->Nmod;    
    (*to)->Nchn   = from->Nchn;    
    (*to)->Ngp    = from->Ngp;

    for(x=0;x<from->Ngp;x++)
	ajChararrPut(&((*to)->gpid), x, ajChararrGet(from->gpid, x));
       
    ajListDel(&((*to)->Groups));
    ajListDel(&((*to)->Water));

/*    (*to)->Groups = ajAtomListCopy(from->Groups);
    (*to)->Water  = ajAtomListCopy(from->Water); */

    if(!ajAtomListCopy(&(*to)->Groups, from->Groups))
	ajFatal("Error copying Groups list");
    
    if(!ajAtomListCopy(&(*to)->Water, from->Water))
	ajFatal("Error copying Water list");
    
    /* Copy data in Chain objects */
    for(x=0;x<from->Nchn;x++)
    {
	ajListDel(&((*to)->Chains[x]->Atoms));
	
	(*to)->Chains[x]->Id         = from->Chains[x]->Id;
	(*to)->Chains[x]->Nres       = from->Chains[x]->Nres;
	(*to)->Chains[x]->Nlig       = from->Chains[x]->Nlig;
	(*to)->Chains[x]->numHelices = from->Chains[x]->numHelices;
	(*to)->Chains[x]->numStrands = from->Chains[x]->numStrands;
	ajStrAssS(&((*to)->Chains[x]->Seq), from->Chains[x]->Seq);
/*	(*to)->Chains[x]->Atoms = ajAtomListCopy(from->Chains[x]->Atoms); */
	if(!ajAtomListCopy(&(*to)->Chains[x]->Atoms, from->Chains[x]->Atoms))
	    ajFatal("Error copying Atoms list");	    
    }
    

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





/* ======================================================================= */
/* ============================== Casts ===================================*/
/* ======================================================================= */

/* @section Casts ***********************************************************
**
** These functions examine the contents of an instance and return some
** derived information. Some of them provide access to the internal
** components of an instance. They are provided for programming convenience
** but should be used with caution.
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

/* @func ajPdbChnidToNum ****************************************************
**
** Finds the chain number for a given chain identifier in a pdb structure
**
** @param [r] id  [char]    Chain identifier
** @param [r] pdb [AjPPdb]  Pdb object
** @param [w] chn [ajint *] Chain number
**
** @return [AjBool] True on succcess
** @@
****************************************************************************/

AjBool ajPdbChnidToNum(char id, AjPPdb pdb, ajint *chn)
{
    ajint a;
    
    for(a=0;a<pdb->Nchn;a++)
    {
	if(toupper((int)pdb->Chains[a]->Id) == toupper((int)id))
	{
	    *chn=a+1;
	    return ajTrue;
	}

	/*
	** Cope with chain id's of ' ' (which might be given as '.' in 
	**the Pdb object)
	*/
	if((id==' ')&&(pdb->Chains[a]->Id=='.'))
	{
	    *chn=a+1;
	    return ajTrue;
	}
    }
    
    /*
    ** A '.' may be given as the id for domains comprising more than one
    ** chain
    */
    if(id=='.')
    {
	*chn=1;
	return ajTrue;
    }
	
    return ajFalse;
}





/* @func ajPdbtospArrFindPdbid **********************************************
**
** Performs a binary search for a PDB code over an array of Pdbtosp
** structures (which of course must first have been sorted). This is a 
** case-insensitive search.
**
** @param [r] id  [AjPStr]      Search term
** @param [r] arr [AjPPdbtosp*] Array of AjOPdbtosp objects
** @param [r] siz [ajint]       Size of array
**
** @return [ajint] Index of first Pdbtosp object found with an PDB code
** matching id, or -1 if id is not found.
** @@
****************************************************************************/

ajint ajPdbtospArrFindPdbid(AjPStr id, AjPPdbtosp *arr, ajint siz)
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
	    h=m-1;
        else if(c>0) 
	    l=m+1;
        else 
	    return m;
    }

    return -1;
}





/* ======================================================================= */
/* ========================== Input & Output ============================= */
/* ======================================================================= */

/* @section Input & output **************************************************
**
** These functions are used for formatted input and output to file.    
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

