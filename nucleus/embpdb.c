/****************************************************************************
** @source embpdb.c
**
** Algorithms for handling protein structural data.
** For use with the Atom, Chain and Pdb objects.
** Also for Hetent, Het, Vdwres, Vdwall, Cmap and Pdbtosp objects.
** 
** Copyright (c) 2004 Jon Ison
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
****************************************************************************/

#include "emboss.h"
#include <math.h>

/* ======================================================================= */
/* ============================ private data ============================= */
/* ======================================================================= */

#define CMAP_MODE_I   1
#define CMAP_MODE_C   2





/* ======================================================================= */
/* ================= Prototypes for private functions ==================== */
/* ======================================================================= */

static ajint  embPdbSortPdbtospPdb(const void *ptr1, const void *ptr2);





/* ======================================================================= */
/* ========================== private functions ========================== */
/* ======================================================================= */

/* @func embPdbSortPdbtospPdb ***********************************************
**
** Function to sort Pdbtosp objects by Pdb element. Usually called by 
** embPdbtospReadAllNew.
**
** @param [r] ptr1  [const void*] Pointer to AjOPdbtosp object 1
** @param [r] ptr2  [const void*] Pointer to AjOPdbtosp object 2
**
** @return [ajint] -1 if Pdb1 should sort before Pdb2,
**                 +1 if the Pdb2 should sort first. 
**                  0 if they are identical in length and content. 
** @@
****************************************************************************/

static ajint embPdbSortPdbtospPdb(const void *ptr1, const void *ptr2)
{
    AjPPdbtosp p = NULL;
    AjPPdbtosp q = NULL;

    p = (*(AjPPdbtosp*)ptr1);
    q = (*(AjPPdbtosp*)ptr2);
    
    return ajStrCmpO(p->Pdb, q->Pdb);
}





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





/* ======================================================================= */
/* =========================== destructors =============================== */
/* ======================================================================= */

/* @section Structure Destructors *******************************************
**
** All destructor functions receive the address of the instance to be
** deleted.  The original pointer is set to NULL so is ready for re-use.
**
****************************************************************************/





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





/* ======================================================================= */
/* ========================== Input & Output ============================= */
/* ======================================================================= */

/* @section Input & output **************************************************
**
** These functions are used for formatted input and output to file.    
**
****************************************************************************/

/* @func embVdwallReadNew ***************************************************
**
** Read a Vdwall object from a file in embl-like format (see documentation
** for the EMBASSY DOMAINATRIX package).
** 
** @param [r] inf     [AjPFile]  Input file stream
**
** @return [AjPVdwall] Pointer to Vdwall object.
** @@
****************************************************************************/

AjPVdwall  embVdwallReadNew(AjPFile inf)
{
    AjPVdwall ret = NULL;
    AjPStr line = NULL;   /* Line of text */
    ajint nres  = 0;      /* No. residues */
    ajint natm  = 0;      /* No. atoms */
    ajint rcnt  = 0;      /* Residue count */
    ajint acnt  = 0;      /* Atom count */
    char id1    = '\0';             /* Residue 1 char id code */
    AjPStr id3  = NULL;   /* Residue 3 char id code */
    
    
    /* Allocate strings */
    line = ajStrNew();
    id3  = ajStrNew();


    /* Start of main loop */
    while((ajFileReadLine(inf, &line)))
    {
	/* Parse NR line */
	if(ajStrPrefixC(line, "NR"))
	{	
	    ajFmtScanS(line, "%*s %d", &nres);
		
	    /* Allocate Vdwall object */
	    (ret) = ajVdwallNew(nres);
		
	}
	/* Parse residue id 3 char */
	else if(ajStrPrefixC(line, "AA"))
	{	
	    rcnt++;
	    acnt = 0;
	    ajFmtScanS(line, "%*s %S", &id3);
	}
	/* Parse residue id 1 char */
	else if(ajStrPrefixC(line, "ID"))
	    ajFmtScanS(line, "%*s %c", &id1);
	/* Parse number of atoms */
	else if(ajStrPrefixC(line, "NN"))
	{
	    ajFmtScanS(line, "%*s %d", &natm);
	    
	    /* Allocate next Vdwres object */
	    (ret)->Res[rcnt-1]=ajVdwresNew(natm);
	    
	    /* Write members of Vdwres object */
	    (ret)->Res[rcnt-1]->Id1 = id1;
	    ajStrAssS(&(ret)->Res[rcnt-1]->Id3, id3);
	    
	}
	/* Parse atom line */
	else if(ajStrPrefixC(line, "AT"))
	{
	    acnt++;
	    ajFmtScanS(line, "%*s %S %*c %f", 
		       &(ret)->Res[rcnt-1]->Atm[acnt-1], 
		       &(ret)->Res[rcnt-1]->Rad[acnt-1]);	
	}
    }	


    ajStrDel(&line);
    ajStrDel(&id3);
    
    return ret;
}





/* @func embHetReadRawNew ***************************************************
**
** Reads a dictionary of heterogen groups available at 
** http://pdb.rutgers.edu/het_dictionary.txt and writes a Het object.
**
** @param [r] inf [AjPFile]    Pointer to dictionary
**
** @return [AjPHet] True on success
** @@
****************************************************************************/

AjPHet embHetReadRawNew(AjPFile inf)
{
    AjPHet ret       = NULL;
    AjPStr line      = NULL;  /* A line from the file */
    AjPHetent entry  = NULL;  /* The current entry */
    AjPHetent tmp    = NULL;  /* Temp. pointer */
    AjPList list     = NULL;  /* List of entries */
    ajint het_cnt    = 0;     /* Count of number of HET records in file */
    ajint formul_cnt = 0;     /* Count of number of FORMUL records in file */
    

    /* Check arg's */
    if((!inf))
    {
	ajWarn("Bad args passed to embHetReadRawNew\n");
	return NULL;
    }
    
    /* Create strings etc */
    line = ajStrNew();
    list = ajListNew();

    
    /* Read lines from file */
    while(ajFileReadLine(inf, &line))
    {
	if(ajStrPrefixC(line,"HET "))
	{
	    het_cnt++;
	    
	    entry=ajHetentNew();
	    ajFmtScanS(line, "%*s %S", &entry->abv);
	}
	else if(ajStrPrefixC(line,"HETNAM"))
	{
	    ajStrAppC(&entry->ful, &line->Ptr[15]);
	}
	else if(ajStrPrefixC(line,"HETSYN"))
	{
	    ajStrAppC(&entry->syn, &line->Ptr[15]);
	}
	else if(ajStrPrefixC(line,"FORMUL"))
	{
	    formul_cnt++;

	    /* In cases where HETSYN or FORMUL were not
	       specified, assign a value of '.' */
	    if(MAJSTRLEN(entry->ful)==0)
		ajStrAssC(&entry->ful, ".");

	    if(MAJSTRLEN(entry->syn)==0)
		ajStrAssC(&entry->syn, ".");
	    

	    /* Push entry onto list */
	    ajListPush(list, (AjPHetent) entry);
	}
    }

    if(het_cnt != formul_cnt)
    {	
	while(ajListPop(list, (void **) &tmp))
	    ajHetentDel(&tmp);
	
	ajListDel(&list);	    
	ajStrDel(&line);

	ajFatal("Fatal discrepancy in count of HET and FORMUL records\n"
		"Email wawan@hgmp.mrc.ac.uk\n");
    }	
    
    ret = ajHetNew(0);
    ret->n = ajListToArray(list, (void ***) &((ret)->entries));
    
   
    ajStrDel(&line);
    ajListDel(&list);

    return ret;
}





/* @func embHetReadNew ******************************************************
**
** Read heterogen dictionary, the Het object is allocated.
** 
** @param [r] inf [AjPFile]    Pointer to Het file
** 
** @return [AjPHet] Het object.
** @@
****************************************************************************/

AjPHet  embHetReadNew(AjPFile inf)
{
    AjPHet hetdic   = NULL;
    AjPStr line     = NULL;		/* current line */
    AjPHetent entry = NULL;		/* current entry */
    AjPList list    = NULL;		/* List of entries */
    AjPStr temp     = NULL;		/* Temporary string */
  
  
    /*Check args */
    if((!inf))
    {
	ajWarn("Bad args passed to embHetReadNew\n");
	return NULL;
    }

    /* Create Het object if necessary */
    if(!(hetdic))
	hetdic = ajHetNew(0);
    
    /* Create string and list objects */
  
    line = ajStrNew();
    temp = ajStrNew();
    list = ajListNew();
  
    /* Read lines from file */
    while(ajFileReadLine(inf, &line))
    {
	if(ajStrPrefixC(line, "ID   "))
	{
	    entry=ajHetentNew();
	    ajFmtScanS(line, "%*s %S", &entry->abv);
	}	
	else if(ajStrPrefixC(line, "DE   "))
	{  	/* NEED TO ACCOUNT FOR MULTIPLE LINES */
	    ajStrAssSub(&temp, line, 5, -1);
	    if(ajStrLen(entry->ful))
		ajStrApp(&entry->ful, temp);
	    else
		ajStrAssS(&entry->ful, temp);
	}	
	else if(ajStrPrefixC(line, "SY   "))
	{
	    /*	  ajFmtScanS(line, "%*s %S", &entry->syn); */
	    ajStrAssSub(&temp, line, 5, -1);
	    if(ajStrLen(entry->syn))
		ajStrApp(&entry->syn, temp);
	    else
		ajStrAssS(&entry->syn, temp);
	}
	else if(ajStrPrefixC(line, "NN   "))
	    ajFmtScanS(line, "%*s %S", &entry->cnt);
	else if(ajStrPrefixC(line, "//"))
	    ajListPush(list, (AjPHetent) entry);
    }

    (hetdic)->n=ajListToArray(list, (void ***) &((hetdic)->entries));
  
    ajStrDel(&line);
    ajStrDel(&temp);
    ajListDel(&list);

    return hetdic;
}





/* @func embHetWrite ********************************************************
**
** Writes the contents of a Het object to file. 
**
** @param [w] outf    [AjPFile]   Output file
** @param [r] obj     [AjPHet]    Het object
** @param [r] dogrep  [AjBool]    Flag (True if we are to write <cnt>
**                                element of the Het object to file).
**
** @return [AjBool] True on success
** @@
****************************************************************************/

AjBool embHetWrite(AjPFile outf, AjPHet obj, AjBool dogrep)
{
    ajint i = 0;
    
    /* Check arg's */
    if(!outf || !obj)
	return ajFalse;
    
    
    for(i=0;i<obj->n; i++)
    {
	ajFmtPrintF(outf, "ID   %S\n", obj->entries[i]->abv);
	ajFmtPrintSplit(outf, obj->entries[i]->ful, "DE   ", 70, " \t\n\r");
	ajFmtPrintSplit(outf, obj->entries[i]->syn, "SY   ", 70, " \t\n\r");
	if(dogrep)
	    ajFmtPrintF(outf, "NN   %d\n", obj->entries[i]->cnt);
	ajFmtPrintF(outf, "//\n");
    }

    return ajTrue;
}





/* @func embCmapReadINew ****************************************************
**
** Read a Cmap object from a file in embl-like format (see documentation for 
** DOMAINATRIX "contacts" application). Takes the chain identifier as an 
** integer.
** 
** @param [r] inf     [AjPFile]  Input file stream
** @param [r] chn     [ajint]    Chain number
** @param [r] mod     [ajint]    Model number
**
** @return [AjPCmap] Pointer to new Cmap object.
** @@
****************************************************************************/

 AjPCmap  embCmapReadINew(AjPFile inf, ajint chn, ajint mod)
{
    AjPCmap ret = NULL;
    
    if(ret=embCmapReadNew(inf, CMAP_MODE_I, chn, mod))
	return NULL;

    return ret;
}





/* @func embCmapReadCNew ****************************************************
**
** Read a Cmap object from a file in embl-like format (see documentation for 
** DOMAINATRIX "contacts" application). Takes the chain identifier as a 
** character.
** 
** @param [r] inf     [AjPFile]  Input file stream
** @param [r] chn     [char]     Chain number
** @param [r] mod     [ajint]    Model number
**
** @return [AjPCmap]   Pointer to new Cmap object.
** @@
****************************************************************************/

AjPCmap embCmapReadCNew(AjPFile inf, char chn, ajint mod)
{
    AjPCmap ret = NULL;

    if(!(ret = embCmapReadNew(inf, CMAP_MODE_C, (ajint)chn, mod)))
	return NULL;

    return ret;
}





/* @func embCmapReadNew *****************************************************
**
** Read a Cmap object from a file in embl-like format (see documentation for 
** DOMAINATRIX "contacts" application). This is not usually called by the 
** user, who uses embCmapReadINew or embCmapReadCNew instead.
** 
** @param [r] inf     [AjPFile]  Input file stream.
** @param [r] mode    [ajint]    Mode, either CMAP_MODE_I (treat chn arg as  
**                               an integer) or CMAP_MODE_C (treat chn arg as
**                               a character).
** @param [r] chn     [ajint]    Chain identifier / number.
** @param [r] mod     [ajint]    Model number.
**
** @return [AjPCmap] True on success (an object read)
** @@
****************************************************************************/

AjPCmap embCmapReadNew(AjPFile inf, ajint mode, ajint chn, ajint mod)
	       
{	
    AjPCmap  ret = NULL;
    static   AjPStr line    = NULL;   /* Line of text */
    static   AjPStr temp_id = NULL;   /* Temp location for protein id */
    
    ajint    num_res = 0;       /* No. of residues in domain */	
    ajint    num_con = 0;       /* Total no. of contacts in domain */	
    ajint    x       = 0;       /* No. of first residue making contact */
    ajint    y       = 0;       /* No. of second residue making contact */
    ajint    md      = -1;      /* Model number */
    ajint    cn      = -1;      /* Chain number */
    char     chnid   = '.';     /* Temp. chain identifier */
    AjBool   idok    = ajFalse; /* If the required chain has been found */
    

    /* Check args */	
    if(!inf)
    {	
	ajWarn("Invalid args to embCmapReadNew");	
	return NULL;
    }


    /* Convert '_' chain identifiers to '.' if necessary */
    if(mode==CMAP_MODE_C)
	if(chn=='_')
	    chn='.';
    
    /* Initialise strings */
    if(!line)
    {
	line     = ajStrNew();
	temp_id  = ajStrNew();
    }
    

    /* Start of main loop */
    while((ajFileReadLine(inf, &line)))
    {
	/* Parse ID line */
	if(ajStrPrefixC(line, "ID"))
	    ajFmtScanS(line, "%*s %S", &temp_id);
	/* Parse model number */
	else if(ajStrPrefixC(line, "MO"))
	    ajFmtScanS(line, "%*s[%d]", &md);
	/* Parse chain number */
	else if(ajStrPrefixC(line, "CN"))
	    ajFmtScanS(line, "%*s[%d]", &cn);
	/* Read IN line */	    
	/* Parse number of residues and total number of contacts */
	else if((ajStrPrefixC(line, "IN")) && (md==mod))
	{
	    ajFmtScanS(line, "%*s %*s %c; %*s %d; %*s %d;", 
		       &chnid, &num_res, &num_con);
	    

	    /*
	    ** The third conditional is to capture those few
	    ** domains which are made
	    ** up from more than one chain.  For these, the
	    ** chain character passed 
	    ** in might be an A or a B (e.g. the character
	    ** extracted from the scop 
	    ** domain code) whereas the chain id given in the
	    ** contact map file will
	    ** be a '.' - because of how scopparse copes with these cases. 
	    ** (A '.' is also in the contact maps for where a chain id was not 
	    ** specified in the original pdb file)
	    */

	    if(((cn==chn)&&(mode==CMAP_MODE_I)) ||
	       ((toupper((int)chnid)==toupper(chn))&&(mode==CMAP_MODE_C))||
	       ((toupper((int)chnid)=='.') && (toupper(chn)!='.') &&
		(mode==CMAP_MODE_C)))
	    {
		idok=ajTrue;
		
		/* Allocate contact map and write values */
		(ret) = ajCmapNew(num_res);
		(ret)->Ncon = num_con;
		ajStrAssS(&(ret)->Id, temp_id);
	    }
	}
	/* Read SQ line */
	else if((ajStrPrefixC(line, "SQ")) && (md==mod) && (idok))
	{    
	    while(ajFileReadLine(inf,&line) && !ajStrPrefixC(line,"XX"))
		ajStrAppC(&(ret)->Seq,ajStrStr(line));
	    ajStrCleanWhite(&(ret)->Seq);
	}
	
	/* Read and parse residue contacts */
	else if((ajStrPrefixC(line, "SM")) && (md==mod) && (idok))
	{
	    ajFmtScanS(line, "%*s %*s %d %*c %*s %d", &x, &y);

	    /* Check residue number is in range */
	    if((x>(ret)->Dim) || (y>(ret)->Dim))
		ajFatal("Fatal attempt to write bad data in "
			"embCmapReadNew\nEmail culprit: "
			"jison@hgmp.mrc.ac.uk\n");
	    
	    /* Enter '1' in matrix to indicate contact */
	    ajInt2dPut(&(ret)->Mat, x-1, y-1, 1);
	    ajInt2dPut(&(ret)->Mat, y-1, x-1, 1);
	}
    }	
    
    return ret;	
}	





/* @func embPdbtospReadNew **************************************************
**
** Read a Pdbtosp object from a file in embl-like format (see documentation  
** for DOMAINATRIX "pdbtosp" application).
**
** @param [r] inf [AjPFile] Input file stream
** @param [r] entry [AjPStr] Pdb id
**
** @return [AjPPdbtosp] True on success
** @@
****************************************************************************/

AjPPdbtosp  embPdbtospReadNew(AjPFile inf, AjPStr entry) 
{
    AjPPdbtosp ret = NULL;
    
    ret = embPdbtospReadCNew(inf,ajStrStr(entry));

    return ret;
}





/* @func embPdbtospReadCNew *************************************************
**
** Read a Pdbtosp object from a file in embl-like format.  Memory for the
** object is allocated.
**
** @param [r] inf   [AjPFile] Input file stream
** @param [r] entry [char*]   Pdb id
**
** @return [AjPPdbtosp] True on success
** @@
****************************************************************************/

AjPPdbtosp embPdbtospReadCNew(AjPFile inf, char *entry)
{
    AjPPdbtosp ret = NULL;

    static AjPStr line   = NULL;
    static AjPStr tentry = NULL;	
    static AjPStr pdb    = NULL;	
    AjBool ok            = ajFalse;
    ajint  n             = 0;
    ajint  i             = 0;
    

    /* Only initialise strings if this is called for the first time */
    if(!line)
    {
	line    = ajStrNew();
	tentry  = ajStrNew();
	pdb     = ajStrNew();
    }


    ajStrAssC(&tentry,entry);
    ajStrToUpper(&tentry);
    
    while((ok=ajFileReadLine(inf,&line)))
    {
	if(!ajStrPrefixC(line,"EN   "))
	    continue;
	ajFmtScanS(line, "%*S %S", &pdb);
	if(ajStrMatchWild(pdb,tentry))
	    break;
    }
    if(!ok)
	return NULL;

    while(ok && !ajStrPrefixC(line,"//"))
    {
	if(ajStrPrefixC(line,"XX"))
	{
	    ok = ajFileReadLine(inf,&line);
	    continue;
	}
	else if(ajStrPrefixC(line,"NE"))
	{
	    ajFmtScanS(line, "%*S %d", &n);
	    (ret) = ajPdbtospNew(n);
	    ajStrAssS(&(ret)->Pdb, pdb);
	}
	else if(ajStrPrefixC(line,"IN"))
	{
	    ajFmtScanS(line, "%*S %S %*S %S", &(ret)->Spr[i],
		       &(ret)->Acc[i]);
	    i++;
	}
	
	ok = ajFileReadLine(inf,&line);
    }
    
    return ret;
}





/* @func embPdbtospReadAllNew ***********************************************
**
** Read all the Pdbtosp objects in a file in embl-like format (see 
** documentation for DOMAINATRIX "pdbtosp" application) and writes a list of 
** these objects. It then sorts the list by PDB id.
**
** @param [r] inf [AjPFile] Input file stream
**
** @return [AjPList] List of Pdbtosp objects.
** @@
****************************************************************************/

AjPList  embPdbtospReadAllNew(AjPFile inf)
{
    AjPList ret = NULL;
    AjPPdbtosp ptr = NULL;
    
    /* Check args and allocate list if necessary */
    if(!inf)
	return NULL;

    if(!(ret))
	ret = ajListNew();
    

    while(ptr = embPdbtospReadCNew(inf, "*"))
	ajListPush(ret, (void *) ptr);

    ajListSort(ret, embPdbSortPdbtospPdb);
    
    return ret;
}





/* @func embPdbReadNew ******************************************************
**
** Reads a clean coordinate file (see documentation for DOMAINATRIX "pdbparse" 
** application) and writes a filled Pdb object.
**
** @param [r] inf  [AjPFile] Pointer to clean coordinate file
**
** @return [AjPPdb] Pointer to Pdb object.
** @@
****************************************************************************/

AjPPdb embPdbReadNew(AjPFile inf)
{
    AjPPdb ret = NULL;
    
    ajint nmod = 0;
    ajint ncha = 0;
    ajint ngrp = 0;
    ajint nc   = 0;
    ajint mod  = 0;
    ajint chn  = 0;
    ajint gpn  = 0;

    float reso = 0.0;

    AjPStr line      = NULL;
    AjPStr token     = NULL;
    AjPStr idstr     = NULL;
    AjPStr destr     = NULL;
    AjPStr osstr     = NULL;
    AjPStr xstr      = NULL;
    AjPStrTok handle = NULL;
    
    AjPAtom atom     = NULL;

    /* Intitialise strings */
    line  = ajStrNew();
    token = ajStrNew();
    idstr = ajStrNew();
    destr = ajStrNew();
    osstr = ajStrNew();
    xstr  = ajStrNew();

    /* Start of main application loop */
    while(ajFileReadLine(inf,&line))
    {
	if(ajStrPrefixC(line,"XX"))
	    continue;

	/* Parse ID */
	if(ajStrPrefixC(line,"ID"))
	{
	    ajStrTokenAss(&handle,line," \n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&idstr,&handle,NULL);
	    continue;
	}

	
	/* Parse number of chains */
	if(ajStrPrefixC(line,"CN"))
	{
	    ajStrTokenAss(&handle,line," []\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&nc);
	    continue;
	}
	

	/* Parse description text */
	if(ajStrPrefixC(line,"DE"))
	{
	    ajStrTokenAss (&handle, line, " ");
	    ajStrToken (&token, &handle, NULL);
	    /* 'DE' */
	    ajStrToken (&token, &handle, "\n\r");
	    /* desc */
	    if (ajStrLen(destr))
	    {
		ajStrAppC (&destr, " ");
		ajStrApp (&destr, token);
	    }
	    else
		ajStrAss (&destr, token);
	    continue;
	}


	/* Parse source text */
	if(ajStrPrefixC(line,"OS"))
	{
	    ajStrTokenAss (&handle, line, " ");
	    ajStrToken (&token, &handle, NULL);
	    /* 'OS' */
	    ajStrToken (&token, &handle, "\n\r");
	    /* source */
	    if (ajStrLen(osstr))
	    {
		ajStrAppC (&osstr, " ");
		ajStrApp (&osstr, token);
	    }
	    else
		ajStrAss (&osstr, token);
	    continue;
	}
	

	/* Parse experimental line */
	if(ajStrPrefixC(line,"EX"))
	{
	    ajStrTokenAss(&handle,line," ;\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&xstr,&handle,NULL); /* method */
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL); /* reso */
	    ajStrToFloat(token,&reso);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* nmod */
	    ajStrToInt(token,&nmod);
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL); /* nchn */
	    ajStrToInt(token,&ncha);

	    ajStrToken(&token,&handle,NULL); /* nlig */
	    ajStrToInt(token,&ngrp);

	    ret = ajPdbNew(ncha);

	    ajStrAssS(&(ret)->Pdb,idstr);
	    ajStrAssS(&(ret)->Compnd,destr);
	    ajStrAssS(&(ret)->Source,osstr);
	    if(ajStrMatchC(xstr,"xray"))
		(ret)->Method = ajXRAY;
	    else
		(ret)->Method = ajNMR;

	    (ret)->Reso = reso;
	    (ret)->Nmod = nmod;
	    (ret)->Nchn = ncha;
	    (ret)->Ngp  = ngrp;
	}
	

	/* Parse information line */
	if(ajStrPrefixC(line,"IN"))
	{
	    ajStrTokenAss(&handle,line," ;\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* id value */
	    (ret)->Chains[nc-1]->Id=*ajStrStr(token);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* residues */
	    ajStrToInt(token,&(ret)->Chains[nc-1]->Nres);
	    ajStrToken(&token,&handle,NULL);
	    /* hetatm */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->Nlig);
	    /* helices */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numHelices);
	    /* strands */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numStrands);
	    /* sheets */
	    /*
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numSheets);
	    */
	    /* turns */
	    /*
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numTurns);
	    */
	    continue;
	}
  

	/* Parse sequence line */
	if(ajStrPrefixC(line,"SQ"))
	{
	    while(ajFileReadLine(inf,&line) && !ajStrPrefixC(line,"XX"))
		ajStrAppC(&(ret)->Chains[nc-1]->Seq,ajStrStr(line));
	    ajStrCleanWhite(&(ret)->Chains[nc-1]->Seq);
	    continue;
	}


	/* Parse coordinate line */
	if(ajStrPrefixC(line,"CO"))
	{
	    mod = chn = gpn = 0;
	    
	    ajStrTokenAss(&handle,line," \t\n\r");
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&mod);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&chn);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&gpn);
	    
	    /* AJNEW0(atom); */
	    atom = ajAtomNew();
	    
	    atom->Mod = mod;
	    atom->Chn = chn;
	    atom->Gpn = gpn;
	    

	    ajStrToken(&token,&handle,NULL);
	    atom->Type = *ajStrStr(token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->Idx);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Pdb,token);

	    ajStrToken(&token,&handle,NULL);
	    atom->eType = *ajStrStr(token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eNum);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->eId,token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eClass);

	    ajStrToken(&token,&handle,NULL);
	    atom->eStrideType = *ajStrStr(token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eStrideNum);

	    ajStrToken(&token,&handle,NULL);
	    atom->Id1 = *ajStrStr(token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Id3,token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Atm,token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->X);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->Y);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->Z);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->O);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->B);

	    ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->Phi);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->Psi);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->Area);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->all_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->all_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->side_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->side_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->main_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->main_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->npol_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->npol_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->pol_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->pol_rel);


	    /* Check for coordinates for water or groups that could not
	       be uniquely assigned to a chain */
	    if(chn==0)
	    {
		/* Heterogen */
		if(atom->Type == 'H')
		    ajListPushApp((ret)->Groups,(void *)atom);
		else if(atom->Type == 'W')
		    ajListPushApp((ret)->Water,(void *)atom);
		else
		    ajFatal("Unexpected parse error in embPdbRead. "
			    "Email jison@hgmp.mrc.ac.uk");
	    }
	    else
		ajListPushApp((ret)->Chains[chn-1]->Atoms,(void *)atom);
	}
    }
    /* End of main application loop */
    

    ajStrTokenClear(&handle);
    ajStrDel(&line);
    ajStrDel(&token);
    ajStrDel(&idstr);
    ajStrDel(&destr);
    ajStrDel(&osstr);
    ajStrDel(&xstr);

    return ret;
}





/* @func embPdbReadFirstModelNew ********************************************
**
** Reads a clean coordinate file file (see documentation for DOMAINATRIX 
** "pdbparse" application) and writes a filled Pdb object. Data for the first
** model only is read in.
**
** @param [r] inf  [AjPFile] Pointer to clean coordinate file
**
** @return [AjPPdb] Pointer to Pdb object.
** @@
****************************************************************************/

AjPPdb embPdbReadFirstModelNew(AjPFile inf) 
{
    AjPPdb ret = NULL;
    
    ajint nmod = 0;
    ajint ncha = 0;
    ajint ngrp = 0;
    ajint nc   = 0;
    ajint mod  = 0;
    ajint chn  = 0;
    ajint gpn  = 0;

    float reso = 0.0;

    AjPStr line      = NULL;
    AjPStr token     = NULL;
    AjPStr idstr     = NULL;
    AjPStr destr     = NULL;
    AjPStr osstr     = NULL;
    AjPStr xstr      = NULL;
    AjPStrTok handle = NULL;
    
    AjPAtom atom     = NULL;

    /* Intitialise strings */
    line  = ajStrNew();
    token = ajStrNew();
    idstr = ajStrNew();
    destr = ajStrNew();
    osstr = ajStrNew();
    xstr  = ajStrNew();

    /* Start of main application loop */
    while(ajFileReadLine(inf,&line))
    {
	if(ajStrPrefixC(line,"XX"))
	    continue;

	/* Parse ID */
	if(ajStrPrefixC(line,"ID"))
	{
	    ajStrTokenAss(&handle,line," \n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&idstr,&handle,NULL);
	    continue;
	}

	
	/* Parse number of chains */
	if(ajStrPrefixC(line,"CN"))
	{
	    ajStrTokenAss(&handle,line," []\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&nc);
	    continue;
	}
	

	/* Parse description text */
	if(ajStrPrefixC(line,"DE"))
	{
	    ajStrTokenAss (&handle, line, " ");
	    ajStrToken (&token, &handle, NULL);
	    /* 'DE' */
	    ajStrToken (&token, &handle, "\n\r");
	    /* desc */
	    if (ajStrLen(destr))
	    {
		ajStrAppC (&destr, " ");
		ajStrApp (&destr, token);
	    }
	    else
		ajStrAss (&destr, token);
	    continue;
	}


	/* Parse source text */
	if(ajStrPrefixC(line,"OS"))
	{
	    ajStrTokenAss (&handle, line, " ");
	    ajStrToken (&token, &handle, NULL);
	    /* 'OS' */
	    ajStrToken (&token, &handle, "\n\r");
	    /* source */
	    if (ajStrLen(osstr))
	    {
		ajStrAppC (&osstr, " ");
		ajStrApp (&osstr, token);
	    }
	    else
		ajStrAss (&osstr, token);
	    continue;
	}
	

	/* Parse experimental line */
	if(ajStrPrefixC(line,"EX"))
	{
	    ajStrTokenAss(&handle,line," ;\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&xstr,&handle,NULL); /* method */
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL); /* reso */
	    ajStrToFloat(token,&reso);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* nmod */
	    ajStrToInt(token,&nmod);
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL); /* ncha */
	    ajStrToInt(token,&ncha);

	    ajStrToken(&token,&handle,NULL); /* nlig */
	    ajStrToInt(token,&ngrp);

	    ret = ajPdbNew(ncha);

	    ajStrAssS(&(ret)->Pdb,idstr);
	    ajStrAssS(&(ret)->Compnd,destr);
	    ajStrAssS(&(ret)->Source,osstr);
	    if(ajStrMatchC(xstr,"xray"))
		(ret)->Method = ajXRAY;
	    else
		(ret)->Method = ajNMR;

	    (ret)->Reso = reso;
	    /* (ret)->Nmod = nmod; */

	    /*
	    ** Number of models is hard-coded to 1 as only the 
	    **  data for the first model is read in
	    */
	    (ret)->Nmod = 1;
	    (ret)->Nchn = ncha;
	    (ret)->Ngp  = ngrp;
	}
	

	/* Parse information line */
	if(ajStrPrefixC(line,"IN"))
	{
	    ajStrTokenAss(&handle,line," ;\n\t\r");
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* id value */
	    (ret)->Chains[nc-1]->Id=*ajStrStr(token);
	    ajStrToken(&token,&handle,NULL);
	    ajStrToken(&token,&handle,NULL); /* residues */
	    ajStrToInt(token,&(ret)->Chains[nc-1]->Nres);
	    ajStrToken(&token,&handle,NULL);
	    /* hetatm */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->Nlig);
	    /* helices */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numHelices);
	    /* strands */
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numStrands);
	    /* sheets */
	    /*
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numSheets);
	    */
	    /* turns */
	    /*
	    ajStrToken(&token,&handle,NULL); 
	    ajStrToInt(token,&(ret)->Chains[nc-1]->numTurns);
	    */
	    continue;
	}
  

	/* Parse sequence line */
	if(ajStrPrefixC(line,"SQ"))
	{
	    while(ajFileReadLine(inf,&line) && !ajStrPrefixC(line,"XX"))
		ajStrAppC(&(ret)->Chains[nc-1]->Seq,ajStrStr(line));
	    ajStrCleanWhite(&(ret)->Chains[nc-1]->Seq);
	    continue;
	}


	/* Parse coordinate line */
	if(ajStrPrefixC(line,"CO"))
	{
	    mod = chn = gpn = 0;
	    
	    ajStrTokenAss(&handle,line," \t\n\r");
	    ajStrToken(&token,&handle,NULL);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&mod);

	    if(mod!=1)
		break;

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&chn);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&gpn);
	    
	    /* AJNEW0(atom); */
	    atom = ajAtomNew();

	    atom->Mod = mod;
	    atom->Chn = chn;
	    atom->Gpn = gpn;
	    

	    ajStrToken(&token,&handle,NULL);
	    atom->Type = *ajStrStr(token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->Idx);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Pdb,token);

	    ajStrToken(&token,&handle,NULL);
	    atom->eType = *ajStrStr(token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eNum);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->eId,token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eClass);

	    ajStrToken(&token,&handle,NULL);
	    atom->eStrideType = *ajStrStr(token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToInt(token,&atom->eStrideNum);

	    ajStrToken(&token,&handle,NULL);
	    atom->Id1 = *ajStrStr(token);
	    
	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Id3,token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrAssS(&atom->Atm,token);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->X);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->Y);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->Z);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->O);

	    ajStrToken(&token,&handle,NULL);
	    ajStrToFloat(token,&atom->B);

	    ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->Phi);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->Psi);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->Area);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->all_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->all_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->side_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->side_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->main_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->main_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->npol_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->npol_rel);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->pol_abs);

            ajStrToken(&token,&handle,NULL);
            ajStrToFloat(token,&atom->pol_rel);

	    /* Check for coordinates for water or groups that could not
	    ** be uniquely assigned to a chain
	    */
	    if(chn==0)
	    {
		/* Heterogen */
		if(atom->Type == 'H')
		    ajListPushApp((ret)->Groups,(void *)atom);
		else if(atom->Type == 'W')
		    ajListPushApp((ret)->Water,(void *)atom);
		else
		    ajFatal("Unexpected parse error in "
			    "embPdbReadFirstModelNew. Email "
			    "jison@hgmp.mrc.ac.uk");
	    }
	    else
		ajListPushApp((ret)->Chains[chn-1]->Atoms,(void *)atom);
	}
    }
    /* End of main application loop */
    
    ajStrTokenClear(&handle);
    ajStrDel(&line);
    ajStrDel(&token);
    ajStrDel(&idstr);
    ajStrDel(&destr);
    ajStrDel(&osstr);
    ajStrDel(&xstr);

    return ret;
}





/* @func embPdbWriteAll *****************************************************
**
** Writes a clean coordinate file for a protein.
**
** @param [w] outf [AjPFile] Output file stream
** @param [r] obj  [AjPPdb]  Pdb object
**
** @return [AjBool] True on success
** @@
****************************************************************************/

AjBool embPdbWriteAll(AjPFile outf, AjPPdb obj)
{
    ajint x      = 0;
    ajint y      = 0;
    AjIList iter = NULL;
    AjPAtom tmp  = NULL;
    
    /* Write the header information */

    ajFmtPrintF(outf, "%-5s%S\n", "ID", obj->Pdb);
    ajFmtPrintF(outf, "XX\n");

    ajFmtPrintSplit(outf,obj->Compnd,"DE   ",75," \t\r\n");
    ajFmtPrintF(outf, "XX\n");

    ajFmtPrintSplit(outf,obj->Source,"OS   ",75," \t\r\n");
    ajFmtPrintF(outf, "XX\n");

    ajFmtPrintF(outf, "%-5sMETHOD ", "EX");
    if(obj->Method == ajXRAY)
	ajFmtPrintF(outf, "xray; ");	
    else
	ajFmtPrintF(outf, "nmr_or_model; ");		
    ajFmtPrintF(outf, "RESO %.2f; NMOD %d; NCHN %d; NLIG %d;\n", obj->Reso,
		obj->Nmod, obj->Nchn, obj->Ngp);


    /* Write chain-specific information */
    for(x=0;x<obj->Nchn;x++)
    { 
	ajFmtPrintF(outf, "XX\n");	
	ajFmtPrintF(outf, "%-5s[%d]\n", 
		    "CN", 
		    x+1);	
	ajFmtPrintF(outf, "XX\n");	
 
	ajFmtPrintF(outf, "%-5s", "IN");

	if(obj->Chains[x]->Id == ' ')
	    ajFmtPrintF(outf, "ID %c; ", '.');
	else
	    ajFmtPrintF(outf, "ID %c; ", obj->Chains[x]->Id);

	
	/*
	ajFmtPrintF(outf, "NR %d; NL %d; NH %d; NE %d; NS %d; NT %d;\n", 
		    obj->Chains[x]->Nres,
		    obj->Chains[x]->Nlig,
		    obj->Chains[x]->numHelices, 
		    obj->Chains[x]->numStrands, 
		    obj->Chains[x]->numSheets, 
		    obj->Chains[x]->numTurns);
		    */

	ajFmtPrintF(outf, "NRES %d; NL %d; NH %d; NE %d;\n", 
		    obj->Chains[x]->Nres,
		    obj->Chains[x]->Nlig,
		    obj->Chains[x]->numHelices, 
		    obj->Chains[x]->numStrands);

	/*
	ajFmtPrintF(outf, "%-5sID %c; NR %d; NH %d; NW %d;\n", 
		    "IN", 
		    obj->Chains[x]->Id,
		    obj->Chains[x]->Nres,
		    obj->Chains[x]->Nhet,
		    obj->Chains[x]->Nwat);
		    */
	ajFmtPrintF(outf, "XX\n");	
	ajSeqWriteXyz(outf, obj->Chains[x]->Seq, "SQ");
    }
    ajFmtPrintF(outf, "XX\n");	

    /* printf("NCHN: %d   NMOD: %d\n", obj->Nchn, obj->Nmod); */
    
    

    /* Write coordinate list */
    for(x=1;x<=obj->Nmod;x++)
    {
	for(y=0;y<obj->Nchn;y++)
	{
	    /* Print out chain-specific coordinates */
	    iter=ajListIter(obj->Chains[y]->Atoms);
	    while(ajListIterMore(iter))
	    {
		tmp=(AjPAtom)ajListIterNext(iter);
		if(tmp->Mod>x)
		    break;
		else if(tmp->Mod!=x)
		    continue;
		else	
		{
		    if(tmp->Type=='H')
			ajFmtPrintF(outf, "%-5s%-5d%-5d%-5d%-5c%-6c%-6S%-5c"
				    "%-5c%-5c%-5c%-5c%-5c%-2c"
				    "%6S    %-4S"
				    "%8.3f%9.3f%9.3f%8.2f%8.2f%8.2f%8.2f"
				    "%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
				    "%8.2f%8.2f%8.2f%8.2f\n", 
				    "CO", 
				    tmp->Mod, 
				    tmp->Chn, 
				    tmp->Gpn, 
				    tmp->Type, 
				    '.',
				    tmp->Pdb, 
				    '.', 
				    '.', 
				    '.', 
				    '.', 
				    '.', 
				    '.', 
				    tmp->Id1,
				    tmp->Id3,
				    tmp->Atm, 
				    tmp->X, 
				    tmp->Y, 
				    tmp->Z,
				    tmp->O,
				    tmp->B, 
				    tmp->Phi,
				    tmp->Psi,
				    tmp->Area, 
				    tmp->all_abs, 
				    tmp->all_rel, 
				    tmp->side_abs, 
				    tmp->side_rel, 
				    tmp->main_abs, 
				    tmp->main_rel, 
				    tmp->npol_abs, 
				    tmp->npol_rel, 
				    tmp->pol_abs, 
				    tmp->pol_rel);
		    else
		    {
			ajFmtPrintF(outf, "%-5s%-5d%-5d%-5c%-5c%-6d%-6S%-5c", 
				    "CO", 
				    tmp->Mod, 
				    tmp->Chn, 
				    '.',
				    tmp->Type, 
				    tmp->Idx, 
				    tmp->Pdb,
				    tmp->eType);
			
			if(tmp->eNum != 0)
			    ajFmtPrintF(outf, "%-5d", tmp->eNum);
			else
			    ajFmtPrintF(outf, "%-5c", '.');


			ajFmtPrintF(outf, "%-5S", tmp->eId);

			if(tmp->eType == 'H')
			    ajFmtPrintF(outf, "%-5d", tmp->eClass);
			else
			    ajFmtPrintF(outf, "%-5c", '.');


			ajFmtPrintF(outf, "%-5c", tmp->eStrideType);
			if(tmp->eStrideNum != 0)
			    ajFmtPrintF(outf, "%-5d", tmp->eStrideNum);
			else
			    ajFmtPrintF(outf, "%-5c", '.');



			ajFmtPrintF(outf, "%-2c%6S    %-4S%8.3f%9.3f%9.3f"
				    "%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
				    "%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
				    "%8.2f\n", 
				    tmp->Id1, 
				    tmp->Id3,
				    tmp->Atm, 
				    tmp->X, 
				    tmp->Y, 
				    tmp->Z,
				    tmp->O,
				    tmp->B,
				    tmp->Phi,
				    tmp->Psi,
				    tmp->Area, 
				    tmp->all_abs, 
				    tmp->all_rel, 
				    tmp->side_abs, 
				    tmp->side_rel, 
				    tmp->main_abs, 
				    tmp->main_rel, 
				    tmp->npol_abs, 
				    tmp->npol_rel, 
				    tmp->pol_abs, 
				    tmp->pol_rel);
		    }
		}
	    }
	    ajListIterFree(iter);			
	} 	

	/* Print out group-specific coordinates for this model */
	iter=ajListIter(obj->Groups);
	while(ajListIterMore(iter))
	{
	    tmp=(AjPAtom)ajListIterNext(iter);
	    if(tmp->Mod>x)
		break;
	    else if(tmp->Mod!=x)
		continue;
	    else	
	    {
		ajFmtPrintF(outf, "%-5s%-5d%-5c%-5d%-5c%-6c%-6S%-5c%-5c%-5c"
			    "%-5c%-5c%-5c%-2c"
			    "%6S    %-4S"
			    "%8.3f%9.3f%9.3f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
			    "%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f\n", 
			    "CO", 
			    tmp->Mod, 
			    '.',
			    tmp->Gpn, 
			    tmp->Type, 
			    '.', 
			    tmp->Pdb, 
			    '.', 
			    '.', 
			    '.', 
			    '.', 
			    '.', 
			    '.',
			    tmp->Id1,
			    tmp->Id3,
			    tmp->Atm, 
			    tmp->X, 
			    tmp->Y, 
			    tmp->Z,
			    tmp->O,
			    tmp->B,
			    tmp->Phi,
			    tmp->Psi,
			    tmp->Area, 
			    tmp->all_abs, 
			    tmp->all_rel, 
			    tmp->side_abs, 
			    tmp->side_rel, 
			    tmp->main_abs, 
			    tmp->main_rel, 
			    tmp->npol_abs, 
			    tmp->npol_rel, 
			    tmp->pol_abs, 
			    tmp->pol_rel);

	    }
	}
	ajListIterFree(iter);			


	/* Print out water-specific coordinates for this model */
	iter=ajListIter(obj->Water);
	while(ajListIterMore(iter))
	{
	    tmp=(AjPAtom)ajListIterNext(iter);
	    if(tmp->Mod>x)
		break;
	    else if(tmp->Mod!=x)
		continue;
	    else	
	    {
		ajFmtPrintF(outf, "%-5s%-5d%-5c%-5c%-5c%-6c%-6S%-5c%-5c"
			    "%-5c%-5c%-5c%-5c%-2c"
			    "%6S    %-4S"
			    "%8.3f%9.3f%9.3f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
			    "%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f\n",
			    "CO", 
			    tmp->Mod, 
			    '.', 
			    '.', 
			    tmp->Type, 
			    '.', 
			    tmp->Pdb, 
			    '.', 
			    '.', 
			    '.', 
			    '.',
			    '.', 
			    '.',
			    tmp->Id1,
			    tmp->Id3,
			    tmp->Atm, 
			    tmp->X, 
			    tmp->Y, 
			    tmp->Z,
			    tmp->O,
			    tmp->B, 
			    tmp->Phi,
			    tmp->Psi,
			    tmp->Area, 
			    tmp->all_abs, 
			    tmp->all_rel, 
			    tmp->side_abs, 
			    tmp->side_rel, 
			    tmp->main_abs, 
			    tmp->main_rel, 
			    tmp->npol_abs, 
			    tmp->npol_rel, 
			    tmp->pol_abs, 
			    tmp->pol_rel);
	    }
	}
	ajListIterFree(iter);			
    }
    ajFmtPrintF(outf, "//\n");    


    return ajTrue;
}





/* @func embPdbWriteDomain **************************************************
**
** Writes a clean coordinate file for a SCOP domain. Where coordinates for 
** multiple models (e.g. NMR structures) are given, data for model 1 are 
** written.
** In the clean file, the coordinates are presented as belonging to a single 
** chain regardless of how many chains the domain comprised.
** Coordinates for heterogens are NOT written to file.
**
** @param [w] outf [AjPFile] Output file stream
** @param [r] pdb  [AjPPdb]  Pdb object
** @param [r] scop [AjPScop] Scop object
** @param [w] errf [AjPFile] Output file stream for error messages
**
** @return [AjBool] True on success
** @@
** 
****************************************************************************/
AjBool embPdbWriteDomain(AjPFile outf, AjPPdb pdb,
			 AjPScop scop, AjPFile errf)
{
    /*
    ** rn_mod is a modifier to the residue number to give correct residue
    ** numbering for the domain
    */
    ajint z;
    ajint chn;
    ajint start   = 0;
    ajint end     = 0;
    ajint finalrn = 0;
    ajint rn_mod  = 0;  
    ajint last_rn = 0;  
    ajint this_rn;
    char  id;
    
    AjPStr tmpseq = NULL;   
    AjPStr seq    = NULL;   
    AjPStr tmpstr = NULL;
        
    AjBool   found_start = ajFalse;
    AjBool   found_end   = ajFalse;
    AjBool   nostart     = ajFalse;
    AjBool   noend       = ajFalse;
    AjIList  iter        = NULL;
    AjPAtom  atm         = NULL;
    AjPAtom  atm2        = NULL;
    
    /* Intitialise strings */
    seq    = ajStrNew();
    tmpseq = ajStrNew();
    tmpstr = ajStrNew();
    
    /* Check for unknown or zero-length chains */
    for(z=0;z<scop->N;z++)
	if(!ajPdbChnidToNum(scop->Chain[z], pdb, &chn))
	{
	    ajWarn("Chain incompatibility error in "
		   "embPdbWriteDomain");			
		
	    ajFmtPrintF(errf, "//\n%S\nERROR Chain incompatibility "
			"error in embPdbWriteDomain\n", scop->Entry);
	    ajStrDel(&seq);
	    ajStrDel(&tmpseq);
	    ajStrDel(&tmpstr);
	    return ajFalse;
	}
	else if(pdb->Chains[chn-1]->Nres==0)
	{		
	    ajWarn("Chain length zero");			
	    
	    ajFmtPrintF(errf, "//\n%S\nERROR Chain length zero\n", 
			scop->Entry);
	    ajStrDel(&seq);
	    ajStrDel(&tmpseq);
	    ajStrDel(&tmpstr);
	    return ajFalse;
	}
    
    
    /* Write header info. to domain coordinate file */
    ajFmtPrintF(outf, "%-5s%S\n", "ID", scop->Entry);
    ajFmtPrintF(outf, "XX\n");
    ajFmtPrintF(outf, "%-5sCo-ordinates for SCOP domain %S\n", 
		"DE", scop->Entry);
    ajFmtPrintF(outf, "XX\n");
    ajFmtPrintF(outf, "%-5sSee Escop.dat for domain classification\n", 
		"OS");
    ajFmtPrintF(outf, "XX\n");
    ajFmtPrintF(outf, "%-5sMETHOD ", "EX");
    if(pdb->Method == ajXRAY)
	ajFmtPrintF(outf, "xray; ");	
    else
	ajFmtPrintF(outf, "nmr_or_model; ");		
    ajFmtPrintF(outf, "RESO %.2f; NMOD 1; NCHN 1; NLIG 0;\n", 
		pdb->Reso);
    
    /* JCI The NCHN and NMOD are hard-coded to 1 for domain files */
    
    
    /* Start of main application loop */
    /* Print out data up to co-ordinates list */
    for(z=0;
	z<scop->N;
	z++,found_start=ajFalse, found_end=ajFalse, 
	nostart=ajFalse, noend=ajFalse, last_rn=0)
    {	
	/*
	** Unknown or Zero sized chains have already been checked for
	** so no additional checking is needed here
	*/
	ajPdbChnidToNum(scop->Chain[z], pdb, &chn);
	

	/* Initialise the iterator */
	iter=ajListIter(pdb->Chains[chn-1]->Atoms);


	/*
	** If start of domain is unspecified 
	** then assign start to first residue in chain
	*/
	if(!ajStrCmpC(scop->Start[z], "."))
	{
	    nostart     = ajTrue;
	    start       = 1;
	    found_start = ajTrue;	
	}
		


	/*
	** If end of domain is unspecified 
	** then assign end to last residue in chain
	*/
	if(!ajStrCmpC(scop->End[z], "."))
	{
	    noend = ajTrue;
	    end=pdb->Chains[chn-1]->Nres;
	    found_end=ajTrue;	
	}
		

	/* Find start and end of domains in chain */
	if(!found_start || !found_end)
	{
	    /* Iterate through the list of atoms */
	    while((atm=(AjPAtom)ajListIterNext(iter)))
	    {
		/* JCI hard-coded to work on model 1 */
		/*
		** Continue if a non-protein atom is found or break if
		** model no. !=1
		*/
		if(atm->Mod!=1 || (found_start && found_end))
		    break; 
		if(atm->Type!='P')
		    continue;


		/* if(atm->Type!='P' || atm->Mod!=1 
		   || (found_start && found_end))
		    break; */


		/* If we are onto a new residue */
		this_rn=atm->Idx;
		if(this_rn!=last_rn)
		{
		    last_rn=this_rn;

		    /*
		    ** The start position was specified, but has not 
		    ** been found yet
		    */
		    if(!found_start && !nostart)		
		    {
			ajStrAssS(&tmpstr, scop->Start[z]);
			ajStrAppK(&tmpstr, '*');
			
			/* Start position found  */
		        /*if(!ajStrCmpCase(atm->Pdb, scop->Start[z])) */
			if(ajStrMatchWild(atm->Pdb, tmpstr))
			{
			    if(!ajStrMatch(atm->Pdb, scop->Start[z]))
			    {
				ajWarn("Domain start found by wildcard "
				       "match only "
				       "in embPdbWriteDomain");
				ajFmtPrintF(errf, "//\n%S\nERROR Domain "
					    "start found "
					    "by wildcard match only in "
					    "embPdbWriteDomain\n",
					    scop->Entry);
			    }
			    
			    start=atm->Idx;
			    found_start=ajTrue;	
			}
			else	
			    continue;
		    }


		    /*
		    ** The end position was specified, but has not 
		    ** been found yet
		    */
		    if(!found_end && !noend)		
		    {
			ajStrAssS(&tmpstr, scop->End[z]);
			ajStrAppK(&tmpstr, '*');

			/* End position found */
			/*if(!ajStrCmpCase(atm->Pdb, scop->End[z])) */
			if(ajStrMatchWild(atm->Pdb, tmpstr))
			{
			    if(!ajStrMatch(atm->Pdb, scop->End[z]))
			    {
				ajWarn("Domain end found by wildcard "
				       "match only "
				       "in embPdbWriteDomain");
				ajFmtPrintF(errf, "//\n%S\nERROR Domain end "
					    "found "
					    "by wildcard match only in "
					    "embPdbWriteDomain\n",
					    scop->Entry);
			    }

			    end = atm->Idx;
			    found_end = ajTrue;       
			    break;
			}
		    }	
		}
	    }
	}
	
	

	/* Diagnostics if start position was not found */
	if(!found_start)		
	{
	    ajStrDel(&seq);
	    ajStrDel(&tmpseq);
	    ajStrDel(&tmpstr);
	    ajListIterFree(iter);	
	    ajWarn("Domain start not found in embPdbWriteDomain");
	    ajFmtPrintF(errf, "//\n%S\nERROR Domain start not found "
			"in in embPdbWriteDomain\n", scop->Entry);
	    return ajFalse;
	}
	

	/* Diagnostics if end position was not found */
	if(!found_end)		
	{
	    ajStrDel(&seq);
	    ajStrDel(&tmpseq);
	    ajStrDel(&tmpstr);
	    ajListIterFree(iter);	
	    ajWarn("Domain end not found in embPdbWriteDomain");
	    ajFmtPrintF(errf, "//\n%S\nERROR Domain end not found "
			"in embPdbWriteDomain\n", scop->Entry);
	    return ajFalse;
	}
	

	/* Write <seq> string here */
	ajStrAssSub(&tmpseq, pdb->Chains[chn-1]->Seq, start-1, end-1);
	ajStrApp(&seq, tmpseq);


	/* Free the iterator */
	ajListIterFree(iter);	
    }
    /* End of main application loop */
    
    
    /*
    ** If the domain was composed of more than once chain then a '.' is
    ** given as the chain identifier
    */
    if(scop->N > 1)
	id = '.';
    else
    {
	id = pdb->Chains[chn-1]->Id;
	if(id == ' ')
	    id = '.';
    }
    


    /* Write sequence to domain coordinate file */
    ajFmtPrintF(outf, "XX\n");	
    ajFmtPrintF(outf, "%-5s[1]\n", "CN");	
    ajFmtPrintF(outf, "XX\n");	
    /*
    ajFmtPrintF(outf, "%-5sID %c; NR %d; NL 0; NH %d; NE %d; NS %d; NT %d;\n", 
		"IN", 
		id,
		ajStrLen(seq),
		pdb->Chains[chn-1]->numHelices, 
		pdb->Chains[chn-1]->numStrands, 
		pdb->Chains[chn-1]->numSheets, 
		pdb->Chains[chn-1]->numTurns);
		*/
    ajFmtPrintF(outf, "%-5sID %c; NRES %d; NL 0; NH %d; NE %d;\n", 
		"IN", 
		id,
		ajStrLen(seq),
		pdb->Chains[chn-1]->numHelices, 
		pdb->Chains[chn-1]->numStrands);
    ajFmtPrintF(outf, "XX\n");	
    ajSeqWriteXyz(outf, seq, "SQ");
    ajFmtPrintF(outf, "XX\n");	

    
    /* Write co-ordinates list to domain coordinate file */        
    for(nostart=ajFalse, noend=ajFalse, 
	z=0; z<scop->N;
	z++,found_start=ajFalse, found_end=ajFalse)
    {
	/*
	** Unknown or Zero length chains have already been checked for
	** so no additional checking is needed here
	*/
	ajPdbChnidToNum(scop->Chain[z], pdb, &chn);
	
	/* Initialise the iterator */
	iter = ajListIter(pdb->Chains[chn-1]->Atoms);


	/* Increment res. counter from last chain if appropriate */
	if(noend)
	    rn_mod += atm2->Idx;
	else	 
	    rn_mod += finalrn;

	
	/* Check whether start and end of domain are specified */
	if(!ajStrCmpC(scop->Start[z], "."))
	    nostart = ajTrue;
	else
	    nostart=ajFalse;
	
	if(!ajStrCmpC(scop->End[z], "."))
	    noend = ajTrue;
	else 
	    noend = ajFalse;
	

	/* Iterate through the list of atoms */
	while((atm=(AjPAtom)ajListIterNext(iter)))
	{
	    /*
	    ** Continue if a non-protein atom is found or break if
	    ** model no. !=1
	    */
	    if(atm->Mod!=1)
		break;
	    if(atm->Type!='P')
		continue;


	    /*	if(atm->Mod!=1 || atm->Type!='P')
		break; */
	    
	    
	    /* The start position has not been found yet */
	    if(!found_start)
	    {
		/* Start position was specified */
		if(!nostart)
		{
		    ajStrAssS(&tmpstr, scop->Start[z]);
		    ajStrAppK(&tmpstr, '*');

		    /* Start position found */
		    /*if(!ajStrCmpCase(atm->Pdb, scop->Start[z])) */
		    if(ajStrMatchWild(atm->Pdb, tmpstr))		    
		    {
			if(!ajStrMatch(atm->Pdb, scop->Start[z]))
			{
			    ajWarn("Domain start found by wildcard match only "
				   "in embPdbWriteDomain");
			    ajFmtPrintF(errf, "//\n%S\nERROR Domain "
					"start found "
					"by wildcard match only in "
					"embPdbWriteDomain\n", scop->Entry);
			}
			    

			rn_mod -= atm->Idx-1;
			found_start = ajTrue;	
		    }
		    else	
			continue;
		}
		else	
		    found_start=ajTrue;	
	    }	

	    
	    /*
	    ** The end position was specified, but has not 
	    ** been found yet
	    */
	    if(!found_end && !noend)
	    {
		ajStrAssS(&tmpstr, scop->End[z]);
		ajStrAppK(&tmpstr, '*');

		/* End position found */
		/*if(!ajStrCmpCase(atm->Pdb, scop->End[z])) */
		if(ajStrMatchWild(atm->Pdb, tmpstr))
		{
		    if(!ajStrMatch(atm->Pdb, scop->End[z]))
		    {
			ajWarn("Domain end found by wildcard match only "
			       "in embPdbWriteDomain");
			ajFmtPrintF(errf, "//\n%S\nERROR Domain end found "
				    "by wildcard match only in "
				    "embPdbWriteDomain\n", scop->Entry);
		    }

		    found_end = ajTrue;     
		    finalrn   = atm->Idx;
		}
	    }	
	    /*
	    ** The end position was specified and has been found, and
	    ** the current atom no longer belongs to this final residue
	    */
	    else if(atm->Idx != finalrn && !noend)
		break;
	    
	    
	    /* Print out coordinate line */
	    ajFmtPrintF(outf, "%-5s%-5d%-5d%-5c%-5c%-6d%-6S%-5c",
			"CO", 
			atm->Mod,     /* It will always be 1 */
			1,	      /* JCI chn number is always given as 1 */
			'.',
			atm->Type, 
			atm->Idx+rn_mod, 
			atm->Pdb, 
			atm->eType);
	    if(atm->eNum != 0)
		ajFmtPrintF(outf, "%-5d", atm->eNum);
	    else
		ajFmtPrintF(outf, "%-5c", '.');
	    ajFmtPrintF(outf, "%-5S", atm->eId);

	    if(atm->eType == 'H')
		ajFmtPrintF(outf, "%-5d", atm->eClass);
	    else
		ajFmtPrintF(outf, "%-5c", '.');


	    ajFmtPrintF(outf, "%-5c", atm->eStrideType);
			if(atm->eStrideNum != 0)
			    ajFmtPrintF(outf, "%-5d", atm->eStrideNum);
			else
			    ajFmtPrintF(outf, "%-5c", '.');

	    ajFmtPrintF(outf, "%-2c%6S    %-4S%8.3f%9.3f%9.3f%8.2f%8.2f"
			"%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
			"%8.2f%8.2f%8.2f%8.2f\n", 
			atm->Id1, 
			atm->Id3,
			atm->Atm, 
			atm->X, 
			atm->Y, 
			atm->Z, 
			atm->O, 
			atm->B,
			atm->Phi,
			atm->Psi,
			atm->Area, 
			atm->all_abs, 
			atm->all_rel, 
			atm->side_abs, 
			atm->side_rel, 
			atm->main_abs, 
			atm->main_rel, 
			atm->npol_abs, 
			atm->npol_rel, 
			atm->pol_abs, 
			atm->pol_rel);
	    
	    
	    /* Assign pointer for this chain */
	    atm2 = atm;
	}

	ajListIterFree(iter);			
    } 	
    
    
    /* Write last line in file */
    ajFmtPrintF(outf, "//\n");    
    

    /* Tidy up */
    ajStrDel(&seq);
    ajStrDel(&tmpseq);
    ajStrDel(&tmpstr);    

    return ajTrue;
}





/* @func embPdbWriteSegment *************************************************
**
** Writes a clean coordinate file for a segment, e.g. a domain. The segment 
** corresponds to a sequence that is passed to the function.
** In the clean coordinate file, the coordinates are presented as belonging
** to a single chain.  Coordinates for heterogens are NOT written to file.
**
** @param [w] outf    [AjPFile] Output file stream
** @param [r] pdb     [AjPPdb]  Pdb object
** @param [r] segment [AjPStr]  Sequence of segment to print out.
** @param [r] chnid   [char]    Chain id of segment
** @param [r] domain  [AjPStr]  Domain code for segment
** @param [w] errf    [AjPFile] Output file stream for error messages
**
** @return [AjBool] True on success
** @@
** 
****************************************************************************/
AjBool embPdbWriteSegment(AjPFile outf, AjPPdb pdb, AjPStr segment, 
			  char chnid, AjPStr domain, AjPFile errf)
{
    ajint chn;
    ajint start     = 0;
    ajint end       = 0;
    char  id;
    
    AjIList  iter        = NULL;
    AjPAtom  atm         = NULL;
    AjPAtom  atm2        = NULL;
    AjBool   found_start = ajFalse;
    AjBool   found_end   = ajFalse;    


   
    /* Check for unknown or zero-length chain */
    if(!ajPdbChnidToNum(chnid, pdb, &chn))
    {
	ajWarn("Chain incompatibility error in "
	       "embPdbWriteSegment");			
		
	ajFmtPrintF(errf, "//\n%S\nERROR Chain incompatibility "
		    "error in embPdbWriteDomain\n", domain);
	return ajFalse;
    }
    else if(pdb->Chains[chn-1]->Nres==0)
    {		
	ajWarn("Chain length zero");			
	    
	ajFmtPrintF(errf, "//\n%S\nERROR Chain length zero\n", 
		    domain);
	return ajFalse;
    }
    
    /* Check if segment exists in this chain */
    if((start = ajStrFind(pdb->Chains[chn-1]->Seq, segment)) == -1)
    {
	ajWarn("Domain not found in embPdbWriteSegment");
	ajFmtPrintF(errf, "//\n%S\nERROR Domain not found "
		    "in embPdbWriteSegment\n", domain);
	return ajFalse;
    }
    else	
    {
	/* Residue numbers start at 1 ! */
	start++;
	end = start + MAJSTRLEN(segment) - 1;
    }  	  
    
    
    /* Write header info. to domain coordinate file */
    ajFmtPrintF(outf, "%-5s%S\n", "ID", domain);
    ajFmtPrintF(outf, "XX\n");
    ajFmtPrintF(outf, "%-5sCo-ordinates for domain %S\n", 
		"DE", domain);
    ajFmtPrintF(outf, "XX\n");
    ajFmtPrintF(outf, "%-5sDomain defined from sequence segment\n", 
		"OS");
    ajFmtPrintF(outf, "XX\n");
    ajFmtPrintF(outf, "%-5sMETHOD ", "EX");
    if(pdb->Method == ajXRAY)
	ajFmtPrintF(outf, "xray; ");	
    else
	ajFmtPrintF(outf, "nmr_or_model; ");		
    /* The NCHN and NMOD are hard-coded to 1 for domain files */
    ajFmtPrintF(outf, "RESO %.2f; NMOD 1; NCHN 1; NLIG 0;\n", 
		pdb->Reso);
    

    id = pdb->Chains[chn-1]->Id;
    if(id == ' ')
	id = '.';

    
    /* Write sequence to domain coordinate file */
    ajFmtPrintF(outf, "XX\n");	
    ajFmtPrintF(outf, "%-5s[1]\n", "CN");	
    ajFmtPrintF(outf, "XX\n");	
    
    ajFmtPrintF(outf, "%-5sID %c; NRES %d; NL 0; NH 0; NE 0;\n", 
		"IN", 
		id,
		MAJSTRLEN(segment));
    ajFmtPrintF(outf, "XX\n");	
    ajSeqWriteXyz(outf, segment, "SQ");
    ajFmtPrintF(outf, "XX\n");	
    
    
    /* Write co-ordinates list to domain coordinate file */        
    ajPdbChnidToNum(chnid, pdb, &chn);
    
    /* Initialise the iterator */
    iter = ajListIter(pdb->Chains[chn-1]->Atoms);
    
    
    /* Iterate through the list of atoms */
    while((atm=(AjPAtom)ajListIterNext(iter)))
    {
	if(atm->Mod!=1)
	    break;
	if(atm->Type!='P')
	    continue;
	if(!found_start)
	{
	    if(atm->Idx == start)
		found_start = ajTrue;	
	    else		
		continue;
	}	
	if(!found_end)
	{
	    if(atm->Idx == end)
		found_end = ajTrue;     
	}
	/*  The end position has been found, and the current atom no longer
	 ** belongs to this final residue.
	 */
	else if(atm->Idx != end && found_end)
	    break;
	    
	    
	/* Print out coordinate line */
	ajFmtPrintF(outf, "%-5s%-5d%-5d%-5c%-5c%-6d%-6S%-5c",
		    "CO", 
		    atm->Mod,		/* It will always be 1 */
		    1,			/* chn number is always given as 1 */
		    '.',
		    atm->Type, 
		    atm->Idx-start+1, 
		    atm->Pdb, 
		    atm->eType);
	if(atm->eNum != 0)
	    ajFmtPrintF(outf, "%-5d", atm->eNum);
	else
	    ajFmtPrintF(outf, "%-5c", '.');
	ajFmtPrintF(outf, "%-5S", atm->eId);

	if(atm->eType == 'H')
	    ajFmtPrintF(outf, "%-5d", atm->eClass);
	else
	    ajFmtPrintF(outf, "%-5c", '.');


	ajFmtPrintF(outf, "%-5c", atm->eStrideType);
	if(atm->eStrideNum != 0)
	    ajFmtPrintF(outf, "%-5d", atm->eStrideNum);
	else
	    ajFmtPrintF(outf, "%-5c", '.');

	ajFmtPrintF(outf, "%-2c%6S    %-4S%8.3f%9.3f%9.3f%8.2f%8.2f"
		    "%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f"
		    "%8.2f%8.2f%8.2f%8.2f\n", 
		    atm->Id1, 
		    atm->Id3,
		    atm->Atm, 
		    atm->X, 
		    atm->Y, 
		    atm->Z, 
		    atm->O, 
		    atm->B,
		    atm->Phi,
		    atm->Psi,
		    atm->Area, 
		    atm->all_abs, 
		    atm->all_rel, 
		    atm->side_abs, 
		    atm->side_rel, 
		    atm->main_abs, 
		    atm->main_rel, 
		    atm->npol_abs, 
		    atm->npol_rel, 
		    atm->pol_abs, 
		    atm->pol_rel);
	    
	    
	/* Assign pointer for this chain */
	atm2 = atm;
    }
    
    ajListIterFree(iter);			
    
    
    
    /* Write last line in file */
    ajFmtPrintF(outf, "//\n");    
    
    
    return ajTrue;
}





/* @func embAtomInContact ***************************************************
**
** Determines whether two atoms are in physical contact  
**
** @param [r] atm1   [AjPAtom]     Atom 1 object
** @param [r] atm2   [AjPAtom]     Atom 1 object
** @param [r] thresh [float]       Threshold contact distance
** @param [r] vdw    [AjPVdwall]   Vdwall object
**
** Contact between two atoms is defined as when the van der Waals surface of 
** the first atom comes within the threshold contact distance (thresh) of 
** the van der Waals surface of the second atom.
**
** @return [AjBool] True if the two atoms form contact
** @@
**
****************************************************************************/

AjBool embAtomInContact(AjPAtom atm1, AjPAtom atm2, float thresh,
			AjPVdwall vdw)
{
    float val  = 0.0;
    float val1 = 0.0;



    /* Check args */
    if(!atm1 || !atm2 || !vdw)
    {
	ajWarn("Bad args passed to embAtomInContact");
	return ajFalse;
    }
    
    
    val=((atm1->X -  atm2->X) * (atm1->X -  atm2->X)) +
	((atm1->Y -  atm2->Y) * (atm1->Y -  atm2->Y)) +
	    ((atm1->Z -  atm2->Z) * (atm1->Z -  atm2->Z));


    /*  This calculation uses square root 
    if((sqrt(val) - embVdwRad(atm1, vdw) -
	embVdwRad(atm2, vdw)) <= thresh)
	return ajTrue;
	*/


    /* Same calculation avoiding square root */
    val1 = embVdwRad(atm1, vdw) + embVdwRad(atm2, vdw) + thresh;
    
    if(val <= (val1*val1))
	return ajTrue;


    return ajFalse;
} 





/* @func embAtomDistance ****************************************************
**
** Returns the distance (Angstroms) between two atoms.
**
** @param [r] atm1   [AjPAtom]     Atom 1 object
** @param [r] atm2   [AjPAtom]     Atom 1 object
** @param [r] vdw    [AjPVdwall]   Vdwall object
**
** Returns the distance (Angstroms) between the van der Waals surface of two
** atoms.
**
** @return [float] Distance (Angstroms) between two atoms.
** @@
**
****************************************************************************/

float embAtomDistance(AjPAtom atm1, AjPAtom atm2, AjPVdwall vdw)
{
    float val  = 0.0;
    float val1 = 0.0;

    
    val=((atm1->X -  atm2->X) * (atm1->X -  atm2->X)) +
	((atm1->Y -  atm2->Y) * (atm1->Y -  atm2->Y)) +
	    ((atm1->Z -  atm2->Z) * (atm1->Z -  atm2->Z));


    /*  This calculation uses square root */
    val1= sqrt(val) - embVdwRad(atm1, vdw) - embVdwRad(atm2, vdw);
        
    return val1;
} 





/* @func embPdbAtomIndexI ***************************************************
**
** Reads a Pdb object and writes an integer array which gives the index into 
** the protein sequence for structured residues (residues for which electron
** density was determined) for a given chain. The array length is of course
** equal to the number of structured residues. 
**
** @param [r] pdb [AjPPdb] Pdb object
** @param [r] chn [ajint] Chain number
** @param [w] idx [AjPInt*] Index array
**
** @return [AjBool] True on succcess
** @@
****************************************************************************/

AjBool embPdbAtomIndexI(AjPPdb pdb, ajint chn, AjPInt *idx)
{
    AjIList  iter = NULL;
    AjPAtom  atm  = NULL;
    ajint this_rn = 0;
    ajint last_rn = -1000;
    ajint resn    = 0;     /* Sequential count of residues */
    
    
    if(!pdb || !(*idx))
    {
	ajWarn("Bad arg's passed to embPdbAtomIndexI");
	return ajFalse;
    }
    
    if((chn > pdb->Nchn) || (!pdb->Chains))
    {
	ajWarn("Bad arg's passed to embPdbAtomIndexI");
	return ajFalse;
    }
    

    /* Initialise the iterator */
    iter=ajListIter(pdb->Chains[chn-1]->Atoms);


    /* Iterate through the list of atoms */
    while((atm=(AjPAtom)ajListIterNext(iter)))
    {
	if(atm->Chn!=chn)
	    continue;
	
	/* JCI hard-coded to work on model 1 */
	/* Continue / break if a non-protein atom is found or 
	   model no. !=1 */
	if(atm->Mod!=1)
	    break;
	if(atm->Type!='P') 
	    continue;


	/* If we are onto a new residue */
	this_rn=atm->Idx;
	if(this_rn!=last_rn)
	{
	    ajIntPut(&(*idx), resn++, atm->Idx);
	    last_rn=this_rn;
	}
    }
        
    if(resn==0)
    {
	ajWarn("Chain not found in embPdbAtomIndexI");
	ajListIterFree(iter);		
	return ajFalse;
    }
    	
    ajListIterFree(iter);		

    return ajTrue;
}





/* @func embPdbAtomIndexC ***************************************************
**
** Reads a Pdb object and writes an integer array which gives the index into 
** the protein sequence for structured residues (residues for which electron
** density was determined) for a given chain.  The array length is of course 
** equal to the number of structured residues. 
**
** @param [r] pdb [AjPPdb]  Pdb object
** @param [r] chn [char]  Chain identifier
** @param [w] idx [AjPInt*] Index array
**
** @return [AjBool] True on succcess
** @@
****************************************************************************/

AjBool embPdbAtomIndexC(AjPPdb pdb, char chn, AjPInt *idx)
{
    ajint chnn;
    
    if(!ajPdbChnidToNum(chn, pdb, &chnn))
    {
	ajWarn("Chain not found in embPdbAtomIndexC");
	return ajFalse;
    }
    
    if(!embPdbAtomIndexI(pdb, chnn, idx))
	return ajFalse;

    return ajTrue;
}





/* @func embPdbAtomIndexICA *************************************************
**
** Reads a Pdb object and writes an integer array which gives the index into 
** the protein sequence for structured residues (residues for which electron
** density was determined) for a given chain, EXCLUDING those residues for 
** which CA atoms are missing. The array length is of course equal to the 
** number of structured residues. 
**
** @param [r] pdb  [AjPPdb]  Pdb object
** @param [r] chn  [ajint]   Chain number
** @param [w] idx  [AjPInt*] Index array
** @param [r] nres [ajint*]  Array length 
**
** @return [AjBool] True on succcess
** @@
****************************************************************************/

AjBool embPdbAtomIndexICA(AjPPdb pdb, ajint chn, AjPInt *idx, ajint *nres)
{
    AjIList iter  = NULL;
    AjPAtom atm   = NULL;
    ajint this_rn = 0;
    ajint last_rn = -1000;
    ajint resn    = 0;     /* Sequential count of residues */
    
    if(!pdb || !(*idx))
    {
	ajWarn("Bad arg's passed to embPdbAtomIndexICA");
	return ajFalse;
    }
    
    if((chn > pdb->Nchn) || (!pdb->Chains))
    {
	ajWarn("Bad arg's passed to embPdbAtomIndexICA");
	return ajFalse;
    }
    

    /* Initialise the iterator */
    iter=ajListIter(pdb->Chains[chn-1]->Atoms);


    /* Iterate through the list of atoms */
    while((atm=(AjPAtom)ajListIterNext(iter)))
    {
	if(atm->Chn!=chn)
	    continue;
	
	/*
	** JCI hard-coded to work on model 1
	** Continue / break if a non-protein atom is found or model no. !=1
	*/
	if(atm->Mod!=1)
	    break;
	if(atm->Type!='P') 
	    continue;

	/* If we are onto a new residue */
	this_rn=atm->Idx;
	if(this_rn!=last_rn && ajStrMatchC(atm->Atm,  "CA"))
	{
	    ajIntPut(&(*idx), resn++, atm->Idx);
	    last_rn=this_rn;
	}
    }

        
    if(resn==0)
    {
	ajWarn("Chain not found in embPdbAtomIndexICA");
	ajListIterFree(iter);		
	return ajFalse;
    }
    	
    *nres=resn;
    
    ajListIterFree(iter);		

    return ajTrue;
}





/* @func embPdbAtomIndexCCA *************************************************
**
** Reads a Pdb object and writes an integer array which gives the index into 
** the protein sequence for structured residues (residues for which electron
** density was determined) for a given chain, EXCLUDING those residues for 
** which CA atoms are missing. The array length is of course equal to the 
** number of structured residues. 
**
** @param [r] pdb [AjPPdb]  Pdb object
** @param [r] chn [char]    Chain identifier
** @param [w] idx [AjPInt*] Index array
** @param [r] nres [ajint*] Array length 
**
** @return [AjBool] True on succcess
** @@
****************************************************************************/

AjBool embPdbAtomIndexCCA(AjPPdb pdb, char chn, AjPInt *idx, ajint *nres)
{
    ajint chnn;
    
    if(!ajPdbChnidToNum(chn, pdb, &chnn))
    {
	ajWarn("Chain not found in embPdbAtomIndexCCA");
	return ajFalse;
    }
    
    if(!embPdbAtomIndexICA(pdb, chnn, idx, nres))
	return ajFalse;


    return ajTrue;
}





/* @func embPdbToIdx ********************************************************
**
** Reads a Pdb object and writes an integer which gives the index into the 
** protein sequence for a residue with a specified pdb residue number and a 
** specified chain number.
** 
** @param [w] idx [ajint*]  Residue number (index into sequence)
** @param [r] pdb [AjPPdb]  Pdb object
** @param [r] res [AjPStr]  Residue number (PDB numbering)
** @param [r] chn [ajint]   Chain number
**
** @return [AjBool] True on succcess (res was found in pdb object)
** @@
****************************************************************************/
AjBool embPdbToIdx(ajint *idx, AjPPdb pdb, AjPStr res, ajint chn)
{
    AjIList  iter = NULL;
    AjPAtom  atm  = NULL;
    
    
    if(!pdb || !(res) || !(idx))
    {
	ajWarn("Bad arg's passed to embPdbToIdx");
	return ajFalse;
    }
    
    if((chn > pdb->Nchn) || (!pdb->Chains) || (chn<1))
    {
	ajWarn("Bad arg's passed to embPdbToIdx");
	return ajFalse;
    }
    

    /* Initialise the iterator */
    iter=ajListIter(pdb->Chains[chn-1]->Atoms);


    /* Iterate through the list of atoms */
    while((atm = (AjPAtom)ajListIterNext(iter)))
    {
	if(atm->Chn!=chn)
	    continue;
	
	/*
	** JCI hard-coded to work on model 1
	** Continue / break if a non-protein atom is found or model no. !=1
	*/
	if(atm->Mod!=1)
	    break;

	if(atm->Type!='P') 
	    continue;

	/* If we have found the residue */
	if(ajStrMatch(res, atm->Pdb))
	{
	    ajListIterFree(iter);		
	    *idx = atm->Idx;
	    return ajTrue;
	}
    }
        
    ajWarn("Residue number not found in embPdbToIdx");
    ajListIterFree(iter);		

    return ajFalse;
}





/* @func embPdbListHeterogens ***********************************************
** 
** Function to create a list of arrays of Atom objects for ligands in the 
** current Pdb object (a single array for each ligand).  An array of int's
** giving the number of Atom objects in each array, is also written. The
** number of ligands is also written.
** 
** @param [r] pdb             [AjPPdb]   Pointer to pdb object 
** @param [w] list_heterogens [AjPList*] Pointer to list of heterogen Atom
**                                       arrays 
** @param [w] siz_heterogens  [AjPInt*]  Pointer to integer array of sizes
**                                       (number of Atom objects in each 
**                                       array).
** @param [w] nhet            [ajint*]   Number of arrays in the list that
**                                       was written. 
** @param [w] logf            [AjPFile]  Log file for error messages
**
** @return [AjBool] ajTrue on success
** @@ 
****************************************************************************/

AjBool embPdbListHeterogens(AjPPdb pdb, AjPList *list_heterogens, 
			    AjPInt *siz_heterogens, ajint *nhet, 
			    AjPFile logf)
{ 
  /*
  ** NOTE: EVERYTHING IN THE CLEAN PDB FILES IS CURRENTLY CHAIN
  ** ASSOCIATED!  THIS WILL BE CHANGED IN FUTURE
  */
  AjIList iter  = NULL;	/* Iterator for atoms in current pdb object */
  AjPAtom hetat = NULL;		/* Pointer to current Atom object */
  ajint i=0;			/* Counter for chains */
  ajint prev_gpn = -10000; 	/* Group number of atom object from
				   previous iteration */
  AjPList GrpAtmList = NULL; 	/* List to hold atoms from the current 
				   group */
  AjPAtom *AtmArray  = NULL;	/* Array of atom objects */
  ajint n=0;			/* number of elements in AtmArray */
  ajint grp_count = 0;		/* No. of groups */
  ajint arr_count = 0;          /* Index for siz_heterogens */
    
  /* Check args */
  if((pdb==NULL)||(list_heterogens==NULL)||(siz_heterogens==NULL))
  {
      ajWarn("Bad args passed to embPdbListHeterogens\n");
      return ajFalse;
  }
  
  if((!(*list_heterogens))||(!(*siz_heterogens)))
  {
      ajWarn("Bad args passed to embPdbListHeterogens\n");
      return ajFalse;
  }
  
  if(pdb->Ngp>0)
      ajFmtPrintF(logf, "\tNGP:%d\n", pdb->Ngp);
  
  if(pdb->Nchn>0)
  {      
      for(i=0;i<pdb->Nchn;++i) 
      {
	  prev_gpn=-100000;	   /* Reset prev_gpn for each chain */
	  /* initialise iterator for pdb->Chains[i]->Atoms */
	  iter=ajListIter(pdb->Chains[i]->Atoms);
	  /* Iterate through list of Atom objects */
	  while((hetat=(AjPAtom)ajListIterNext(iter)))
	  {		
	      /* check for type  */
	      if(hetat->Type != 'H')
		  continue;

	      /* TEST FOR A NEW GROUP */
	      if(prev_gpn != hetat->Gpn) 
	      {
		  grp_count++;
		  if(GrpAtmList)
		  {
		      n=(ajListToArray(GrpAtmList, (void ***) &AtmArray));
		      ajListPushApp(*list_heterogens, AtmArray);
		      ajIntPut(siz_heterogens, arr_count, n);
		      (*nhet)++;
		      ajListDel(&GrpAtmList);
		      GrpAtmList=NULL;
		      arr_count++;
		  }		    		    
		  GrpAtmList=ajListNew();
		  prev_gpn=hetat->Gpn;
	      } /* End of new group loop */
	      ajListPushApp(GrpAtmList, (AjPAtom) hetat);
	  } /* End of list iteration loop */

	  /* Free list iterator */
	  ajListIterFree(iter);
	    
      } /* End of chain for loop */

      if(GrpAtmList)
      {
	  n=(ajListToArray(GrpAtmList, (void ***) &AtmArray));
	  ajListPushApp(*list_heterogens, AtmArray);
	  ajIntPut(siz_heterogens, arr_count, n);
	  (*nhet)++;
	  ajListDel(&GrpAtmList);
	  GrpAtmList=NULL;
      }
	
      GrpAtmList = NULL;
      prev_gpn   = -10000;  

  } /* End of chain loop */
  
  
  return ajTrue;
}





/* @func embPdbidToSp *******************************************************
**
** Read a pdb identifier code and writes the equivalent swissprot identifier
** code.  Relies on list of Pdbtosp objects sorted by PDB code, which is 
** usually obtained by a call to embPdbtospReadAllNew.
** 
** @param [r] pdb  [AjPStr]   Pdb  identifier code
** @param [w] spr  [AjPStr*]  Swissprot identifier code
** @param [r] list [AjPList]  Sorted list of Pdbtosp objects
**
** @return [AjBool]  True if a swissprot identifier code was found
**                   for the Pdb code.
** @@
****************************************************************************/

AjBool embPdbidToSp(AjPStr pdb, AjPStr *spr, AjPList list)
{
    AjPPdbtosp *arr = NULL;  /* Array derived from list */
    ajint dim = 0;           /* Size of array */
    ajint idx = 0;           /* Index into array for the Pdb code */

    
    if(!pdb || !list)
    {
	ajWarn("Bad args passed to embPdbidToSp");
	return ajFalse;
    }
    

    dim = ajListToArray(list, (void ***) &(arr));
    if(!dim)
    {
	ajWarn("Empty list passed to embPdbidToSp");
	return ajFalse;
    }


    if( (idx = ajPdbtospArrFindPdbid(pdb, arr, dim))==-1)
	return ajFalse;
    else
    {
	ajStrAssS(spr, arr[idx]->Spr[0]);
	return ajTrue;
    }
}





/* @func embPdbidToAcc ******************************************************
**
** Read a pdb identifier code and writes the equivalent accession number.
** Relies on list of Pdbtosp objects sorted by PDB code, which is usually 
** obtained by a call to embPdbtospReadAllNew.
** 
** @param [r] pdb  [AjPStr]   Pdb  identifier code
** @param [w] acc  [AjPStr*]  Accession number
** @param [r] list [AjPList]  Sorted list of Pdbtosp objects
**
** @return [AjBool]  True if a swissprot identifier code was found for the
**                   Pdb code.
** @@
****************************************************************************/

AjBool embPdbidToAcc(AjPStr pdb, AjPStr *acc, AjPList list)
{
    AjPPdbtosp *arr = NULL;  /* Array derived from list */
    ajint dim = 0;           /* Size of array */
    ajint idx = 0;           /* Index into array for the Pdb code */

    
    if(!pdb || !list)
    {
	ajWarn("Bad args passed to embPdbidToAcc");
	return ajFalse;
    }
    

    dim = ajListToArray(list, (void ***) &(arr));
    if(!dim)
    {
	ajWarn("Empty list passed to embPdbidToAcc");
	return ajFalse;
    }


    if( (idx = ajPdbtospArrFindPdbid(pdb, arr, dim))==-1)
    {
	AJFREE(arr);
	return ajFalse;
    }
    
    else
    {
	ajStrAssS(acc, arr[idx]->Acc[0]);
	AJFREE(arr);
	return ajTrue;
    }
}





/* @func  embPdbidToScop ****************************************************
**
** Writes a list of scop identifier codes for the domains that a Pdb object
** contains.  The domain data is taken from a list of scop objects.
**
** @param [r] pdb             [AjPPdb]   Pointer to pdb object
** @param [r] list_allscop    [AjPList]  Pointer to SCOP list of SCOP 
**                                       classification objects  
** @param [w] list_pdbscopids [AjPList*] Pointer to list of scop domain ids
**                                       in the current pdb object
** 
** @return [AjBool] ajTrue on success
** @@
****************************************************************************/

AjBool embPdbidToScop(AjPPdb pdb, AjPList list_allscop,
		      AjPList *list_pdbscopids)
{
  
    AjIList iter    = NULL; /* List iterator for SCOP classification list */
    AjPScop ptr     = NULL;
    AjPStr tmpPdbId = NULL;
    AjPStr tmpDomId = NULL;
    ajint found     = 0;

    iter=ajListIter(list_allscop);


    while((ptr=(AjPScop)ajListIterNext(iter)))
    {
	ajStrAssS(&tmpPdbId, ptr->Pdb);
	ajStrToLower(&tmpPdbId);

	if(ajStrMatch(pdb->Pdb, tmpPdbId))
	{
	    ajStrAssS(&tmpDomId, ptr->Entry);
	    ajStrToLower(&tmpDomId);
	    ajListPushApp(*list_pdbscopids, tmpDomId);
	    tmpDomId = NULL;
	    found = 1;
	}
    }
    ajListIterFree(iter);
    ajStrDel(&tmpPdbId);
    ajStrDel(&tmpDomId);
  
    if(found==1) 
	return ajTrue;

    return ajFalse;
}




/* @func embVdwRad **********************************************************
**
** Returns the van der Waals radius of an atom. Returns 1.2 as default.
**
** @param [r] atm    [AjPAtom]     Atom object
** @param [r] vdw    [AjPVdwall]   Vdwall object
**
** @return [float] van der Waals radius of the atom
** @@
**
****************************************************************************/

float embVdwRad(AjPAtom atm, AjPVdwall vdw)
{
    ajint x = 0;
    ajint y = 0;
    
    for(x=0;x<vdw->N;x++)
	for(y=0;y<vdw->Res[x]->N;y++)
	    if(ajStrMatch(atm->Atm, vdw->Res[x]->Atm[y]))
		return(vdw->Res[x]->Rad[y]);	 
    
    return((float)1.2);
}





/* @func embPdbtospReadAllRawNew ********************************************
**
** Reads the swissprot:pdb equivalence table available at URL (1)
**  (1) http://www.expasy.ch/cgi-bin/lists?pdbtosp.txt
** and returns the data as a list of Pdbtosp objects. 
**
** @param [r] inf    [AjPFile]     
**
** @return [AjPList] List of Pdbtosp objects. 
** @@
**
****************************************************************************/

AjPList       embPdbtospReadAllRawNew(AjPFile inf)
{
    AjPList    ret     =NULL;   /* List of Pdbtosp objects to return */
    AjPPdbtosp tmp     =NULL;   /* Temp. pointer to Pdbtosp object */
    AjPStr     pdb     =NULL;   /* PDB identifier */
    AjPStr     spr     =NULL;   /* Swissprot identifier */
    AjPStr     acc     =NULL;   /* Accession number */
    AjPStr     pspr    =NULL;   /* Swissprot identifier pointer */
    AjPStr     pacc    =NULL;   /* Accession number pointer */
    AjPStr     line    =NULL;   /* Line from file */
    AjPStr     token   =NULL;   /* Token from line */
    AjPStr     subtoken=NULL;   /* Token from line */
    AjPList    acclist =NULL;   /* List of accession numbers */
    AjPList    sprlist =NULL;   /* List of swissprot identifiers */
    ajint      n       =0;      /* No. of accession numbers for current pdb 
				   code */
    AjBool     ok      =ajFalse;/* True if "____  _" has been found and we 
				   can start parsing */
    AjBool     done_1st=ajFalse;/* True if the first line of data has been 
				   parsed*/

    

    /* Memory allocation */
    line    = ajStrNew();
    token   = ajStrNew();
    subtoken= ajStrNew();
    pdb     = ajStrNew();
    acclist = ajListstrNew();
    sprlist = ajListstrNew();
    ret     = ajListNew();
    


    /* Read lines from file */
    while(ajFileReadLine(inf, &line))
    {
	if(ajStrPrefixC(line, "____  _"))
	{
	    ok=ajTrue;
	    continue;
	}
	
	
	if(!ok)
	    continue;

	if(ajStrMatchC(line, ""))
	    break; 
	
	

	/* Read in pdb code first.  Then tokenise by ':', discard the 
	   first token, then tokenise the second token by ',', parsing 
	   out the swisssprot codes and accession numbers from the 
	   subtokens */


	/* Make sure this is a line containing the pdb code */
	if((ajStrFindC(line, ":")!=-1))
	{
	    if(done_1st)
	    {
		tmp = ajPdbtospNew(0);
		ajStrAssS(&tmp->Pdb, pdb);
		tmp->n = n;
		tmp->Acc = (AjPStr *) ajListToArray(acclist, 
						    (void ***) &tmp->Acc);
		tmp->Spr = (AjPStr *) ajListToArray(sprlist, 
						    (void ***) &tmp->Spr);		
		ajListPushApp(ret, (void *)tmp);
		
		
		ajListstrDel(&acclist);
		ajListstrDel(&sprlist);		
		acclist = ajListstrNew();
		sprlist = ajListstrNew();

		n=0;
	    }	

	    ajFmtScanS(line, "%S", &pdb);

	    ajStrTokC(line, ":");
	    ajStrAssS(&token, ajStrTokC(NULL, ":"));

	    done_1st=ajTrue;
	}
	else 
	{
	    ajStrAssS(&token, line);
	}
	

	spr  = ajStrNew();
	acc  = ajStrNew();
	ajFmtScanS(token, "%S (%S", &spr, &acc);
	
	if(ajStrSuffixC(acc, "),"))
	{
	    ajStrChop(&acc);
	    ajStrChop(&acc);
	}
	else
       	    ajStrChop(&acc);
	
	
	ajListstrPushApp(acclist, acc);
	ajListstrPushApp(sprlist, spr);
	n++;

	ajStrTokC(token, ",");
	while((subtoken=ajStrTokC(NULL, ",")))
	{
	    spr  = ajStrNew();
	    acc  = ajStrNew();

	    ajFmtScanS(subtoken, "%S (%S", &spr, &acc); 

	    if(ajStrSuffixC(acc, "),"))
	    {
		ajStrChop(&acc);
		ajStrChop(&acc);
	    }
	    else
		ajStrChop(&acc);


	    ajListstrPushApp(acclist, acc);
	    ajListstrPushApp(sprlist, spr);
	    n++;
	}
    }	

    /* Data for last pdb code ! */
    tmp = ajPdbtospNew(0);
    ajStrAssS(&tmp->Pdb, pdb);
    tmp->n = n;
    tmp->Acc = (AjPStr *) ajListToArray(acclist, (void ***) &tmp->Acc);
    tmp->Spr = (AjPStr *) ajListToArray(sprlist, (void ***) &tmp->Spr);	  
    ajListPushApp(ret, (void *)tmp);
    ajListstrDel(&acclist);
    ajListstrDel(&sprlist);		



    /* Tidy up */
    ajStrDel(&line);
    ajStrDel(&token);
    ajStrDel(&subtoken);
    ajStrDel(&pdb);
    ajListstrDel(&acclist);	
    ajListstrDel(&sprlist);	


    return ret;
}



/* @func embPdbtospWrite ****************************************************
**
** Reads a list of Pdbtosp objects, e.g. taken from the swissprot:pdb 
** equivalence table available at URL:
**  (1) http://www.expasy.ch/cgi-bin/lists?pdbtosp.txt)
** and writes the list out to file in embl-like format (see 
** documentation for DOMAINATRIX "pdbtosp" application).
**
** @param [r] out   [AjPFile]     
** @param [r] list  [AjPList]     
**
** @return [AjBool] True of file was written ok.
** @@
**
****************************************************************************/

AjBool       embPdbtospWrite(AjPFile outf, AjPList list)
{
    AjIList    iter = NULL;
    AjPPdbtosp tmp  = NULL;
    ajint        x  = 0;
    

    if(!outf || !list)
    {
	ajWarn("Bad args passed to embPdbtospWrite");
	return ajFalse;
    }
    
    iter = ajListIter(list);
    
    while((tmp=(AjPPdbtosp)ajListIterNext(iter)))
    {
	ajFmtPrintF(outf, "%-5s%S\nXX\n%-5s%d\nXX\n", 
		    "EN", tmp->Pdb, "NE", tmp->n);	

	for(x=0; x<tmp->n; x++)
	    ajFmtPrintF(outf, "%-5s%S ID; %S ACC;\n", 
			"IN", tmp->Spr, tmp->Acc);
	ajFmtPrintF(outf, "XX\n//\n");
    }

    return ajTrue;
}





/* ======================================================================= */
/* ======================== Miscellaneous =================================*/
/* ======================================================================= */

/* @section Miscellaneous ***************************************************
**
** These functions may have diverse functions that do not fit into the other
** categories. 
**
****************************************************************************/





