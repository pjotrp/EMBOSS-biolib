/****************************************************************************
**
** @source embdomain.c
**
** Algorithms for reading SCOP and CATH parsable files, for reading and 
** writing dcf (domain classification file) format and for handling protein
** domain data.
** For use with the Scop and Cath objects. 
** 
** Copyright (c) 2004 Jon Ison (jison@hgmp.mrc.ac.uk) 
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





/* ======================================================================= */
/* ============================ Private data ============================= */
/* ======================================================================= */

/* @data EmbPScopcla ********************************************************
**
** Nucleus Scopcla object.
**
** Holds scop database data from raw file (dir.cla.scop.txt from SCOP authors)
**
** The variables have the following meaning:
**
**  AjPStr Entry           Domain identifer code. 
**  AjPStr Pdb             Corresponding pdb identifier code.
**  AjPStr Sccs            Scop compact classification string.
**  ajint  Class           SCOP sunid for class 
**  ajint  Fold            SCOP sunid for fold 
**  ajint  Superfamily     SCOP sunid for superfamily 
**  ajint  Family          SCOP sunid for family 
**  ajint  Domain          SCOP sunid for domain   
**  ajint  Source          SCOP sunid for species 
**  ajint  Domdat          SCOP sunid for domain data
**  ajint  N               No. chains from which this domain is comprised 
**  char   *Chain          Chain identifiers 
**  AjPStr *Start          PDB residue number of first residue in domain 
**  AjPStr *End            PDB residue number of last residue in domain 
**
** EmbPScopcla is implemented as a pointer to a C data structure.
**
** @alias EmbSScopcla
** @alias EmbOScopcla
**
** @@
****************************************************************************/

typedef struct EmbSScopcla
{
    AjPStr Entry;
    AjPStr Pdb;
    AjPStr Sccs;

    ajint  Class;
    ajint  Fold;
    ajint  Superfamily;
    ajint  Family;  
    ajint  Domain;
    ajint  Source;
    ajint  Domdat;
        
    ajint    N;
    char   *Chain;
    AjPStr *Start;
    AjPStr *End;
} EmbOScopcla,*EmbPScopcla;





/* @data EmbPScopdes ********************************************************
**
** Nucleus Scopdes object.
**
** Holds SCOP database data from raw file (dir.des.scop.txt from SCOP authors)
**
** The variables have the following meaning:
**
**  ajint  Sunid    SCOP sunid for node.
**  AjPStr Type     Type of node, either 'px' (domain data), 'cl' (class),
**                  'cf' (fold), 'sf' (superfamily), 'fa' (family), 'dm' 
**                  (domain) or 'sp' (species).
**  AjPStr Sccs     Scop compact classification string.
**  AjPStr Entry    Domain identifer code (or '-' if Type!='px').
**  AjPStr Desc     Description in english of the node.
**
** EmbPScopdes is implemented as a pointer to a C data structure.
**
** @alias EmbSScopdes
** @alias EmbOScopdes
**
** @@
****************************************************************************/

typedef struct EmbSScopdes
{
    ajint  Sunid;
    AjPStr Type;
    AjPStr Sccs;
    AjPStr Entry;
    AjPStr Desc;
} EmbOScopdes,*EmbPScopdes;





/*@data EmbPCathDom**********************************************************
**
** Nucleus CathDom object
**
** Holds CATH database data from domlist.v2.4. This file only contains
** domain information for proteins that have 2 or more domains. 
**
** The variables have the following meaning:
**
**  AjPStr DomainID 	   ID for protein containing 2 or more domains
**  AjPStr *Start 	   PDB residue number of first residue in segment
**  AjPStr *End 	   PDB residue number of last residue in segment
**  ajint  NSegment 	   No. of chain segments domain is comprised of
**  
**  @alias EmbSCathDom
**  @alias EmbOCathDom
**
**  @@
****************************************************************************/

typedef struct EmbSCathDom 
{
   AjPStr DomainID; 
   AjPStr *Start;
   AjPStr *End;          
   ajint  NSegment;
} EmbOCathDom, *EmbPCathDom;





/*@data EmbPCathName*********************************************************
**
** Nucleus CathName object
**
** Holds CATH database data from CAT.names.all.v2.4. This file contains
** a description of each level in the CATH hierarchy. 
**
** The variables have the following meaning:
**
**  AjPStr Id 	           Classification Id 
**  AjPStr Desc 	   Description of level in CATH hierarchy
**  
**  @alias EmbSCathName
**  @alias EmbOCathName
**
**  @@
****************************************************************************/

typedef struct EmbSCathName
{
    AjPStr Id;
    AjPStr Desc;          
} EmbOCathName, *EmbPCathName;





/* ======================================================================= */
/* ================= Prototypes for private functions ==================== */
/* ======================================================================= */





/* ======================================================================= */
/* ========================== Private functions ========================== */
/* ======================================================================= */
ajint         embScopdesCompSunid(const void *scop1, const void *scop2);
EmbPScopcla   embScopclaNew(ajint chains);
void          embScopclaDel(EmbPScopcla *thys);
AjBool        embScopclaRead(AjPFile inf, AjPStr entry, EmbPScopcla *thys);
AjBool        embScopclaReadC(AjPFile inf, char *entry, EmbPScopcla *thys);

EmbPScopdes   embScopdesNew(void);
void          embScopdesDel(EmbPScopdes *ptr);
AjBool        embScopdesRead(AjPFile inf, AjPStr entry, EmbPScopdes *thys);
AjBool        embScopdesReadC(AjPFile inf, char *entry, EmbPScopdes *thys);
ajint         embScopdesBinSearch(ajint id, EmbPScopdes *arr, ajint siz);

ajint         embCathNameBinSearch(AjPStr id, EmbPCathName *arr, ajint siz);
ajint         embCathDomBinSearch(AjPStr id, EmbPCathDom *arr, ajint siz);
static ajint  embSortDomainID(const void *DomID1, const void *DomID2);
static ajint  embSortNameId(const void *cath1, const void *cath2);
void          embCathNameDel(EmbPCathName *ptr);
EmbPCathName  embCathNameNew(void);
void          embCathDomDel(EmbPCathDom *ptr);;
EmbPCathDom   embCathDomNew(ajint nsegments);





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

/* @func embCathReadNew *****************************************************
**
** Read a Cath object from a file in embl-like format (see documentation for 
** DOMAINATRIX "cathparse" application).
**
** @param [r] inf   [AjPFile]  Input file stream
** @param [r] entry [AjPStr]   CATH id of entry to retrieve (or "*" for next
**                             domain in file).
**
** @return [AjPCath] Cath object.
** @@
****************************************************************************/

 AjPCath embCathReadNew(AjPFile inf, AjPStr entry)
{
    AjPCath ret = NULL;

    if((ret = embCathReadCNew(inf, entry->Ptr)))
	return ret;
    else 
	return NULL;
}





/* @func embCathReadCNew ****************************************************
**
** Read a Cath object from a file in embl-like format (see documentation for 
** DOMAINATRIX "cathparse" application).
**
** @param [r] inf   [AjPFile]  Input file stream
** @param [r] entry [char*]    CATH id of entry to retrieve (or "*" for next
**                             domain in file).
**
** @return [AjPCath] Cath object.
** @@
****************************************************************************/

 AjPCath embCathReadCNew(AjPFile inf, char *entry)
{
    AjPCath ret = NULL;
    
    static AjPRegexp exp1  = NULL;
    static AjPRegexp exp2  = NULL;
    static AjPStr domainID = NULL;
    static AjPStr pdb      = NULL;
    static AjPStr class    = NULL;
    
    static AjPStr architecture = NULL;
    static AjPStr topology     = NULL;
    static AjPStr superfamily  = NULL;

    static AjPStr line   = NULL; 	
    static AjPStr str    = NULL;
    static AjPStr stmp   = NULL;
    static AjPStr tentry = NULL;
    
    AjBool ok = ajFalse; 
    
    ajint idx = 0;     
    ajint n   = 0;                   
    
    
    
    ajint  Length;		/* No. of residues in domain */
    

    /* Only initialise strings if this is called for the first time */
    if(!line)
    {
	domainID        = ajStrNew();
	pdb             = ajStrNew();
	class           = ajStrNew();
	architecture    = ajStrNew();
	topology        = ajStrNew();
	superfamily     = ajStrNew();
	line		= ajStrNew();
	str		= ajStrNew();
	tentry          = ajStrNew();
	exp1    = ajRegCompC("^([^ \t\r\n]+)[ \t\n\r]+");
	exp2    = ajRegCompC(
		   "^([A-Za-z0-9.]+)[ ]*[^ \t\r\n]+[ ]*([0-9.-]+)[ ]*"
		     "[^ \t\r\n]+[ ]*([0-9.-]+)");
    }

    
    ajStrAssC(&tentry,entry);    
    ajStrToUpper(&tentry);	   
    
    while((ok=ajFileReadLine(inf,&line)))
    {
	if(!ajStrPrefixC(line,"ID   "))
	    continue;
	
	ajFmtScanS(line, "%*S %S", &stmp);
	
	if(ajStrMatchWild(stmp,tentry))
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
	
	ajRegExec(exp1,line);
	ajRegPost(exp1,&str);
	

	if(ajStrPrefixC(line,"ID"))
	    ajStrAssS(&domainID,str);
	else if(ajStrPrefixC(line,"EN"))
	    ajStrAssS(&pdb,str);
	else if(ajStrPrefixC(line,"CL"))
	    ajStrAssS(&class,str);
	else if(ajStrPrefixC(line,"AR"))
	{
	    ajStrAssS(&architecture,str);
	    while(ajFileReadLine(inf,&line))
	    {
		if(ajStrPrefixC(line,"XX"))
		    break;
		ajStrAppC(&architecture,ajStrStr(line)+3);
	    }
	    ajStrClean(&architecture);
	}
	else if(ajStrPrefixC(line,"TP"))
	{
	    ajStrAssS(&topology,str);
	    while(ajFileReadLine(inf,&line))
	    {
		if(ajStrPrefixC(line,"XX"))
		    break;
		ajStrAppC(&topology,ajStrStr(line)+3);
	    }
	    ajStrClean(&topology);
	}
	else if(ajStrPrefixC(line,"SP"))
	{
	    ajStrAssS(&superfamily,str);
	    while(ajFileReadLine(inf,&line))
	    {
		if(ajStrPrefixC(line,"XX"))
		    break;
		ajStrAppC(&superfamily,ajStrStr(line)+3);
	    }
	    ajStrClean(&superfamily);
	}
	else if(ajStrPrefixC(line,"LN"))  /* Not sure about this bit */
	{
	    ajFmtScanS(line, "%*S %d", &Length);
	}
	else if(ajStrPrefixC(line,"NS"))
	{
	    ajStrToInt(str,&n);
	    (ret) = ajCathNew(n);
	    ajStrAssS(&(ret)->DomainID,domainID);
	    ajStrAssS(&(ret)->Pdb,pdb);
	    ajStrAssS(&(ret)->Class,class);
	    ajStrAssS(&(ret)->Architecture,architecture);
	    ajStrAssS(&(ret)->Topology, topology);
	    ajStrAssS(&(ret)->Superfamily,superfamily);
	}
	else if(ajStrPrefixC(line,"SN"))
	    ajFmtScanS(line, "%*S %*c%d", &idx);
	else if(ajStrPrefixC(line,"CH"))
	{
	    if(!ajRegExec(exp2,str))
		return NULL;
	    ajRegSubI(exp2,1,&stmp);
	    (ret)->Chain = *ajStrStr(stmp);
	    ajRegSubI(exp2,2,&str);
	    ajStrAssC(&(ret)->Start[idx-1],ajStrStr(str)); 

	    ajRegSubI(exp2,3,&str);
	    ajStrAssC(&(ret)->End[idx-1],ajStrStr(str)); 

	}
	
	
	ok = ajFileReadLine(inf,&line);
    }
 
    
    return ret;
}





/* @func embCathWrite *******************************************************
**
** Write contents of a Cath object to an output file in embl-like format
** (see documentation for DOMAINATRIX "cathparse" application).
** 
** @param [w] outf [AjPFile] Output file stream
** @param [r] obj  [AjPCath] Cath object
**
** @return [AjBool] True if file was written ok.
** @@
****************************************************************************/
     
AjBool embCathWrite(AjPFile outf, AjPCath obj)  
{
    
    ajint i;
    AjPStr tmp;


    /* Check args */
    if(!outf || !obj)
    {
	ajWarn("Bad args passed to embCathWrite");
	return ajFalse;
    }
    


    tmp = ajStrNew();

    ajStrAss(&tmp, obj->DomainID);
    ajStrToUpper(&tmp);
    ajFmtPrintF(outf,"ID   %S\nXX\n",tmp);
    
    ajStrAss(&tmp, obj->Pdb);
    ajStrToUpper(&tmp);
    ajFmtPrintF(outf,"EN   %S\nXX\n",tmp);
    
    ajFmtPrintF(outf,"CL   %S",obj->Class);
    ajFmtPrintSplit(outf,obj->Architecture,"\nXX\nAR   ",75," \t\n\r");
    ajFmtPrintSplit(outf,obj->Topology,"XX\nTP   ",75," \t\n\r");
    ajFmtPrintSplit(outf,obj->Superfamily,"XX\nSP   ",75," \t\n\r");
    ajFmtPrintF(outf,"XX\nLN   %d\n",obj->Length);
    ajFmtPrintF(outf,"XX\nNS   %d\n",obj->NSegment);
    
    for(i=0;i<obj->NSegment;++i)
    {
	ajFmtPrintF(outf,"XX\nSN   [%d]\n",i+1);
	
	ajFmtPrintF(outf,"XX\nCH   %c CHAIN; %S START; %S END;\n",
		    obj->Chain,
		    obj->Start[i],
		    obj->End[i]);
	
    }

    ajFmtPrintF(outf,"//\n");

    return ajTrue;
}    





/* @func embScopWrite *******************************************************
**
** Write contents of a Scop object to an output file in embl-like format
** (see documentation for DOMAINATRIX "contacts" application).
**
** @param [w] outf [AjPFile] Output file stream
** @param [r] obj  [AjPScop] Scop object
**
** @return [AjBool] True if file was written ok.
** @@
****************************************************************************/

AjBool embScopWrite(AjPFile outf, AjPScop obj)
{
    ajint i;


    if(!outf || !obj)
    {
	ajWarn("Bad args passed to embScopWrite");
	return ajFalse;
    }
    

    ajFmtPrintF(outf,"ID   %S\nXX\n",obj->Entry);
    ajFmtPrintF(outf,"EN   %S\nXX\n",obj->Pdb);
    ajFmtPrintF(outf,"SI   %d CL; %d FO; %d SF; %d FA; %d DO; %d SO; "
		"%d DD;\nXX\n",
		obj->Sunid_Class,obj->Sunid_Fold, obj->Sunid_Superfamily,
		obj->Sunid_Family,obj->Sunid_Domain, obj->Sunid_Source,
		obj->Sunid_Domdat);

    ajFmtPrintF(outf,"CL   %S",obj->Class);
    ajFmtPrintSplit(outf,obj->Fold,"\nXX\nFO   ",75," \t\n\r");
    ajFmtPrintSplit(outf,obj->Superfamily,"XX\nSF   ",75," \t\n\r");
    ajFmtPrintSplit(outf,obj->Family,"XX\nFA   ",75," \t\n\r");
    ajFmtPrintSplit(outf,obj->Domain,"XX\nDO   ",75," \t\n\r");;
    ajFmtPrintF(outf,"XX\nOS   %S\n",obj->Source);

    if(ajStrLen(obj->SeqPdb))
    {
	ajFmtPrintF(outf,"XX\n");
	ajSeqWriteXyz(outf, obj->SeqPdb, "DS");		
    }	

    if(ajStrLen(obj->Acc))
	ajFmtPrintF(outf,"XX\nAC   %S\n",obj->Acc);    

    if(ajStrLen(obj->Spr))
	ajFmtPrintF(outf,"XX\nSP   %S\n",obj->Spr);

    if(ajStrLen(obj->SeqSpr))
    {
	ajFmtPrintF(outf, "XX\n%-5s%d START; %d END;\n", "RA", obj->Startd,
		    obj->Endd);
	ajFmtPrintF(outf, "XX\n");	
	ajSeqWriteXyz(outf, obj->SeqSpr, "SQ");
    }
    
    ajFmtPrintF(outf,"XX\nNC   %d\n",obj->N);

    for(i=0;i<obj->N;++i)
    {
	ajFmtPrintF(outf,"XX\nCN   [%d]\n",i+1);
	ajFmtPrintF(outf,"XX\nCH   %c CHAIN; %S START; %S END;\n",
		    obj->Chain[i],
		    obj->Start[i],
		    obj->End[i]);
    }
    ajFmtPrintF(outf,"//\n");
    
    return ajTrue;
}





/* @func embScopReadNew *****************************************************
**
** Read a Scop object from a file in embl-like format (see documentation for 
** DOMAINATRIX "scopparse" application).
**
** @param [r] inf   [AjPFile] Input file stream.
** @param [r] entry [AjPStr]  SCOP id of domain to read (or "*" for next 
**                            domain in file).
**
** @return [AjPScop] Scop object. 
** @@
****************************************************************************/

AjPScop embScopReadNew(AjPFile inf, AjPStr entry)
{
    AjPScop ret = NULL;
    
    ret = embScopReadCNew(inf,ajStrStr(entry));
    
    return ret;
}





/* @func embScopReadCNew ****************************************************
**
** Read a Scop object from a file in embl-like format (see documentation for 
** DOMAINATRIX "scopparse" application).
**
** @param [r] inf   [AjPFile]  Input file stream
** @param [r] entry [char*]    SCOP id of domain to parse
**
** @return [AjPScop] Scop object or NULL (file read problem).
** @@
****************************************************************************/

AjPScop embScopReadCNew(AjPFile inf, char *entry)
{
    AjPScop ret = NULL;
    
    static AjPRegexp exp1 = NULL;
    static AjPRegexp exp2 = NULL;
    static AjPStr line    = NULL;
    static AjPStr str     = NULL;
    static AjPStr xentry  = NULL;
    static AjPStr source  = NULL;
    static AjPStr class   = NULL;
    static AjPStr fold    = NULL;
    static AjPStr super   = NULL;
    static AjPStr family  = NULL;
    static AjPStr domain  = NULL;
    static AjPStr pdb     = NULL;
    static AjPStr tentry  = NULL;
    static AjPStr stmp    = NULL;
    static AjPStr Acc     = NULL;         
    static AjPStr Spr     = NULL;          
    static AjPStr SeqPdb  = NULL;	
    static AjPStr SeqSpr  = NULL;	

    AjBool ok             = ajFalse;
    
    char *p;
    ajint idx = 0;
    ajint n   = 0;
    ajint  Startd;      /* Start of sequence relative to full length 
			    swissprot sequence */
    ajint  Endd;        /* End of sequence relative to full length 
			    swissprot sequence */

    ajint  Sunid_Class;         /* SCOP sunid for class */
    ajint  Sunid_Fold;          /* SCOP sunid for fold */
    ajint  Sunid_Superfamily;   /* SCOP sunid for superfamily */
    ajint  Sunid_Family;        /* SCOP sunid for family */
    ajint  Sunid_Domain;        /* SCOP sunid for domain */  
    ajint  Sunid_Source;        /* SCOP sunid for species */
    ajint  Sunid_Domdat;        /* SCOP sunid for domain data */


    /* Only initialise strings if this is called for the first time */
    if(!line)
    {
	str     = ajStrNew();
	xentry  = ajStrNew();
	pdb     = ajStrNew();
	source  = ajStrNew();
	class   = ajStrNew();
	fold    = ajStrNew();
	super   = ajStrNew();
	family  = ajStrNew();
	domain  = ajStrNew();
	line    = ajStrNew();
	tentry  = ajStrNew();
	stmp    = ajStrNew();
	Acc     = ajStrNew();
	Spr     = ajStrNew();
	exp1    = ajRegCompC("^([^ \t\r\n]+)[ \t\n\r]+");
	exp2    = ajRegCompC("^([A-Za-z0-9.]+)[ ]*[^ \t\r\n]+[ ]*"
			     "([0-9.-]+)[ ]*"
			     "[^ \t\r\n]+[ ]*([0-9.-]+)");
    }
    
    SeqSpr  = ajStrNew();
    SeqPdb  = ajStrNew();


    
    ajStrAssC(&tentry,entry);
    ajStrToUpper(&tentry);
    
    while((ok=ajFileReadLine(inf,&line)))
    {
	if(!ajStrPrefixC(line,"ID   "))
	    continue;
	
	if(!ajRegExec(exp1,line))
	    return NULL;
	ajRegPost(exp1,&stmp);
	if(ajStrMatchWild(stmp,tentry))
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
	ajRegExec(exp1,line);
	ajRegPost(exp1,&str);

	if(ajStrPrefixC(line,"ID"))
	    ajStrAssS(&xentry,str);
	else if(ajStrPrefixC(line,"EN"))
	    ajStrAssS(&pdb,str);
	else if(ajStrPrefixC(line,"OS"))
	    ajStrAssS(&source,str);
	else if(ajStrPrefixC(line,"CL"))
	    ajStrAssS(&class,str);
	else if(ajStrPrefixC(line,"FO"))
	{
	    ajStrAssS(&fold,str);
	    while(ajFileReadLine(inf,&line))
	    {
		if(ajStrPrefixC(line,"XX"))
		    break;
		ajStrAppC(&fold,ajStrStr(line)+3);
	    }
	    ajStrClean(&fold);
	}
	else if(ajStrPrefixC(line,"SF"))
	{
	    ajStrAssS(&super,str);
	    while(ajFileReadLine(inf,&line))
	    {
		if(ajStrPrefixC(line,"XX"))
		    break;
		ajStrAppC(&super,ajStrStr(line)+3);
	    }
	    ajStrClean(&super);
	}
	else if(ajStrPrefixC(line,"FA"))
	{
	    ajStrAssS(&family,str);
	    while(ajFileReadLine(inf,&line))
	    {
		if(ajStrPrefixC(line,"XX"))
		    break;
		ajStrAppC(&family,ajStrStr(line)+3);
	    }
	    ajStrClean(&family);
	}
	else if(ajStrPrefixC(line,"DO"))
	{
	    ajStrAssS(&domain,str);
	    while(ajFileReadLine(inf,&line))
	    {
		if(ajStrPrefixC(line,"XX"))
		    break;
		ajStrAppC(&domain,ajStrStr(line)+3);
	    }
	    ajStrClean(&domain);
	}
	else if(ajStrPrefixC(line,"NC"))
	{
	    ajStrToInt(str,&n);
	    (ret) = ajScopNew(n);
	    ajStrAssS(&(ret)->Entry,xentry);
	    ajStrAssS(&(ret)->Pdb,pdb);
	    ajStrAssS(&(ret)->Source,source);
	    ajStrAssS(&(ret)->Class,class);
	    ajStrAssS(&(ret)->Fold,fold);
	    ajStrAssS(&(ret)->Domain,domain);
	    ajStrAssS(&(ret)->Superfamily,super);
	    ajStrAssS(&(ret)->Family,family);
	    ajStrAssS(&(ret)->Acc,Acc);
	    ajStrAssS(&(ret)->Spr,Spr);
	    ajStrAssS(&(ret)->SeqPdb,SeqPdb);
	    ajStrAssS(&(ret)->SeqSpr,SeqSpr);
	    (ret)->Sunid_Class = Sunid_Class;
	    (ret)->Sunid_Fold = Sunid_Fold;
	    (ret)->Sunid_Superfamily = Sunid_Superfamily;
	    (ret)->Sunid_Family = Sunid_Family;
	    (ret)->Sunid_Domain = Sunid_Domain;
	    (ret)->Sunid_Source = Sunid_Source;
	    (ret)->Sunid_Domdat = Sunid_Domdat;
	    (ret)->Startd       = Startd ;
	    (ret)->Endd         = Endd;
	}
	else if(ajStrPrefixC(line,"CN"))
	{
	    p = ajStrStr(str);
	    sscanf(p,"[%d]",&idx);
	}
	else if(ajStrPrefixC(line,"CH"))
	{
	    if(!ajRegExec(exp2,str))
		return NULL;
	    ajRegSubI(exp2,1,&stmp);
	    (ret)->Chain[idx-1] = *ajStrStr(stmp);
	    ajRegSubI(exp2,2,&str);
	    ajStrAssC(&(ret)->Start[idx-1],ajStrStr(str)); 

	    ajRegSubI(exp2,3,&str);
	    ajStrAssC(&(ret)->End[idx-1],ajStrStr(str)); 

	}
	/* Sequence from pdb file */
	else if(ajStrPrefixC(line,"DS"))
	{
	    while((ok=ajFileReadLine(inf,&line)) && !ajStrPrefixC(line,"XX"))
		ajStrAppC(&SeqPdb,ajStrStr(line));
	    ajStrCleanWhite(&SeqPdb);
	    continue;
	}
	/* Sequence from swissprot */
	else if(ajStrPrefixC(line,"SQ"))
	{
	    while((ok=ajFileReadLine(inf,&line)) && !ajStrPrefixC(line,"XX"))
		ajStrAppC(&SeqSpr,ajStrStr(line));
	    ajStrCleanWhite(&SeqSpr);
	    continue;
	}
	/* Accession number */
	else if(ajStrPrefixC(line,"AC"))
	    ajFmtScanS(line, "%*s %S", &Acc);
	/* Swissprot code */
	else if(ajStrPrefixC(line,"SP"))
	    ajFmtScanS(line, "%*s %S", &Spr);
	/* Start and end relative to swissprot sequence */
	else if(ajStrPrefixC(line,"RA"))
	    ajFmtScanS(line, "%*s %d %*s %d", &Startd, &Endd);
	/* Sunid of domain data */
	else if(ajStrPrefixC(line,"SI"))
	    ajFmtScanS(line, "%*s %d %*s %d %*s %d %*s "
		       "%d %*s %d %*s %d %*s %d", 
		       &Sunid_Class, &Sunid_Fold, &Sunid_Superfamily,
		       &Sunid_Family, 
		       &Sunid_Domain, &Sunid_Source, &Sunid_Domdat);
	
	ok = ajFileReadLine(inf,&line);
    }
 
    ajStrDel(&SeqSpr);
    ajStrDel(&SeqPdb);
    
    return ret;
}





/* @func embScopReadAllNew **************************************************
**
** Reads the SCOP classification file (embl-like format, see documentation 
** for DOMAINATRIX "scopparse" application) and creates a list of Scop objects 
** for the entire content.
** 
** @param [r] inf  [AjPFile]     Pointer to SCOP classification file
** 
** @return [AjPList] List of scop objects or NULL (file read problem).
** @@
****************************************************************************/

AjPList  embScopReadAllNew(AjPFile inf)
{
    AjPList ret = NULL;
    
    AjPScop scop_object = NULL;

    /* Check arg's */
    if((!inf))
    {
	ajWarn("Bad args passed to embScopReadAllNew\n");
	return NULL;
    }

    ret = ajListNew();
    

    while((scop_object=embScopReadCNew(inf, "*")))
	ajListPushApp(ret, scop_object);
  
    return ret;
}





/* @func embCathReadAllNew **************************************************
**
** Reads the CATH classification file (embl-format, see documentation for 
** DOMAINATRIX "cathparse" application) and creates a list of Cath objects 
** for the entire content.
** 
** @param [r] inf  [AjPFile]     Pointer to CATH classification file
** 
** @return [AjPList] List of Cath objects. 
** @@
****************************************************************************/

AjPList  embCathReadAllNew(AjPFile inf)
{
    AjPList ret = NULL;
    
    AjPCath cath_object = NULL;

    /* Check arg's */
    if((!inf))
    {
	ajWarn("Bad args passed to embCathReadAllNew\n");
	return NULL;
    }

    ret = ajListNew();
    

    while((cath_object=embCathReadCNew(inf, "*")))
	ajListPushApp(ret, cath_object);
  
    return ret;
}





/* @func embScopToPdbid *****************************************************
**
** Read a scop identifier code and writes the equivalent pdb identifier code
**
** @param [r] scop [AjPStr]   Scop identifier code
** @param [w] pdb  [AjPStr*]  Pdb identifier code
**
** @return [AjPStr] Pointer to pdb identifier code.
** @@
****************************************************************************/

AjPStr embScopToPdbid(AjPStr scop, AjPStr *pdb)
{
    ajStrAssSub(pdb, scop, 1, 4);

    return *pdb;
}





/* @func embScopToSp ********************************************************
**
** Read a scop identifier code and writes the equivalent swissprot 
** identifier code.  Relies on a list of Pdbtosp objects sorted by PDB
** code, which is usually obtained by a call to embDomainPdbtospReadAll.
** 
** @param [r] scop  [AjPStr]  Scop domain identifier code
** @param [w] spr   [AjPStr*]  Swissprot identifier code
** @param [r] list  [AjPList]  Sorted list of Pdbtosp objects
**
** @return [AjBool]  True if a swissprot identifier code was
**                   found for the Scop code.
** @@
****************************************************************************/

AjBool embScopToSp(AjPStr scop, AjPStr *spr, AjPList list)
{
    AjPStr pdb = NULL;
    
    pdb = ajStrNew();
    
    if(embPdbidToSp(embScopToPdbid(scop, &pdb), spr, list))
    {
	ajStrDel(&pdb);
	return ajTrue;
    }

    ajStrDel(&pdb);

    return ajFalse;
}





/* @func embScopToAcc *******************************************************
**
** Read a scop identifier code and writes the equivalent accession number.
** Relies on a list of Pdbtosp objects sorted by PDB code, which is usually
** obtained by a call to embDomainPdbtospReadAll.
** 
** @param [r] scop  [AjPStr]  Scop domain identifier code
** @param [w] acc   [AjPStr*]  Accession number
** @param [r] list  [AjPList]  Sorted list of Pdbtosp objects
**
** @return [AjBool]  True if a swissprot identifier code was found for the
**                   Scop code.
** @@
****************************************************************************/

AjBool embScopToAcc(AjPStr scop, AjPStr *acc, AjPList list)
{
    AjPStr pdb = NULL;
    
    pdb = ajStrNew();
    
    if(embPdbidToAcc(embScopToPdbid(scop, &pdb), acc, list))
    {
	ajStrDel(&pdb);
	return ajTrue;
    }

    ajStrDel(&pdb);
    
    return ajFalse;
}





/* @func embScopReadAllRawNew ***********************************************
**
** Reads the SCOP parsable files (dir.cla.scop.txt & dir.des.scop.txt) and 
** writes a list of Scop objects.
**
** @param [r] claf      [AjPFile] 
** @param [r] desf      [AjPStr]  
** @param [r] outputall [AjPStr]  
**
** @return [AjPList] List of Scop objects.
** @@
****************************************************************************/

AjPList   embScopReadAllRawNew(AjPFile claf, AjPFile desf, AjBool outputall)
{
    EmbPScopcla cla=NULL;   
    EmbPScopdes des=NULL;  
    EmbPScopdes *desarr=NULL;
    AjPScop tmp       = NULL;
    
    AjPList  clalist=NULL;
    AjPList  deslist=NULL;
    AjPList      ret=NULL;    

    AjBool   nooutput=ajFalse;
    char     chn;
    
    
    ajint  dim=0;  /* Dimension of array */
    ajint  idx=0;  /* Index into array */
    ajint  i=0;
    

    clalist = ajListNew();
    deslist = ajListNew();
        ret = ajListNew();


    


    /* Read the dir.cla.scop.txt file */ 
    while(embScopclaReadC(claf, "*", &cla))
    {
	ajListPushApp(clalist, cla);
/*	ajFmtPrint(" %d ", cla->Domdat); */
    }
    
    
    
    /* Read the dir.des.scop.txt file, sort the list by Sunid
       and convert to an array */
    while(embScopdesReadC(desf, "*", &des))
    {
	ajListPush(deslist, des);
/*	ajFmtPrint("%d\n", des->Sunid); */
    }
    

    ajListSort(deslist, embScopdesCompSunid);
    dim=ajListToArray(deslist, (void ***) &desarr);
    

    while(ajListPop(clalist, (void **)&cla))
    {
	if(!outputall)
	{
	    if(cla->N > 1)
	    {
		chn=cla->Chain[0];
		for(nooutput=ajFalse, i=1;i<cla->N;i++)
		    if(chn != cla->Chain[i])
		    {
			nooutput=ajTrue;
			break;
		    }
		if(nooutput)
		    continue;
	    }
	}
	
	tmp = ajScopNew(cla->N);
	ajStrAssS(&tmp->Entry, cla->Entry);
	ajStrAssS(&tmp->Pdb, cla->Pdb);

	tmp->Sunid_Class       = cla->Class;
	tmp->Sunid_Fold        = cla->Fold;
	tmp->Sunid_Superfamily = cla->Superfamily;
	tmp->Sunid_Family      = cla->Family;
	tmp->Sunid_Domain      = cla->Domain;
	tmp->Sunid_Source      = cla->Source;
	tmp->Sunid_Domdat      = cla->Domdat;


	idx = embScopdesBinSearch(cla->Class,  desarr, dim);
	ajStrAssS(&tmp->Class, desarr[idx]->Desc);

	idx = embScopdesBinSearch(cla->Fold,  desarr, dim);
	ajStrAssS(&tmp->Fold, desarr[idx]->Desc);

	idx = embScopdesBinSearch(cla->Superfamily,  desarr, dim);
	ajStrAssS(&tmp->Superfamily, desarr[idx]->Desc);

	idx = embScopdesBinSearch(cla->Family,  desarr, dim);
	ajStrAssS(&tmp->Family, desarr[idx]->Desc);

	idx = embScopdesBinSearch(cla->Domain,  desarr, dim);
	ajStrAssS(&tmp->Domain, desarr[idx]->Desc);

	idx = embScopdesBinSearch(cla->Source,  desarr, dim);
	ajStrAssS(&tmp->Source, desarr[idx]->Desc);

	for(i=0;i<cla->N;++i)
	{
	    tmp->Chain[i] = cla->Chain[i];
	    ajStrAssS(&tmp->Start[i], cla->Start[i]);
	    ajStrAssS(&tmp->End[i], cla->End[i]);	    
	}
	

	ajListPushApp(ret, tmp);
	

	embScopclaDel(&cla);
    
    }

    while(ajListPop(deslist, (void **)&des))
	embScopdesDel(&des);
    
    /* Tidy up */
    AJFREE(desarr);
    ajListDel(&clalist);
    ajListDel(&deslist);

    
    return ret;
}





/* @func embCathReadAllRawNew ***********************************************
**
** Reads the CATH parsable files (dir.cla.scop.txt & dir.des.scop.txt) and 
** writes a list of Cath objects.
**
** @param [r] cathf   [AjPFile] 
** @param [r] domf    [AjPFile]  
** @param [r] namesf  [AjPFile]  
** @param [r] logf    [AjPFile]  
**
** @return [AjPList] List of Scop objects.
** @@
****************************************************************************/

AjPList   embCathReadAllRawNew(AjPFile cathf, AjPFile domf, AjPFile namesf, 
			       AjPFile logf)
{ 
    AjPList ret = NULL;
    AjPStr CathNameLine    = NULL;  /* String used to hold line from namesf*/
    AjPStr CathDomLine     = NULL;  /* String used to hold line from domf  */
    AjPStr CathListLine    = NULL;  /* String used to hold line from cathf */
    AjPStr tmpDomainID     = NULL;  /* temp ptr to string to hold DomainId 
				       minus "0".                          */    
    AjPStr tmpStringDomPtr = NULL;  /* temp ptr to string to hold DXX as a 
				       string.                             */    
    AjPStr tmpNumDomPtr    = NULL;  /* temp ptr to string to hold number of 
				       domains as a string.                */ 
    ajint  tmpDomInt    = 0;        /* temp ptr to string to hold number of
				       domains as int.                     */  
    AjPStr tmpDomIDNumDom  = NULL;  /* temp ptr to string to hold DomainId
				       plus domain number before CathDom 
				       object created.                     */
    ajint  tmpNSegment     = 0;     /* temp ptr to string to hold NSegment
				       before CathDom object created.      */
    AjPStr NDomAsString    = NULL;  /* temp ptr to string to hold (d) 
				       number of domains (appended onto 
				       tmpDomainID).                       */
    AjPStr tmpNumString    = NULL;  /* temp string used to search CathName
				       objects. */
    AjPStr tmpNumString1   = NULL;  /* temp string used to search CathName 
				       objects - first part. */
    AjPStr tmpNumString2   = NULL;  /* temp string used to search CathName 
				       objects - second part. */

    AjPStr Search_DomainIDPtr = NULL;  /* temp search string used to search 
					  CathDomList. */
    AjPStr StrTokPtr       = NULL;
    
    
    EmbPCathDom *CathDomArray   = NULL; /* Array to hold sorted CathDomList  */
    EmbPCathName *CathNameArray = NULL; /* Array to hold sorted CathNameList */
    
    /* Initialise Integers */   
    ajint idxCathDom = 0;    /* Index to CathDomList array */
    ajint dimCathDom = 0;    /* Dimension of CathDomList array */  
    ajint idxCathName= 0;    /* Index to CathNameList array */
    ajint dimCathName= 0;    /* Dimension of CathNameList array */  
    
    
    ajint intC  = 0;  /* Class number as int */ 
    ajint intA  = 0;  /* Architecture number as int */
    ajint intT  = 0;  /* Topology number as int */
    ajint intH  = 0;  /* Homologous Superfamily number as int */
    ajint intF  = 0;  /* Family number as int */
    ajint intNI = 0;  /* Near Identical family number as int */
    ajint intI  = 0;  /* Identical family number as int */  
    ajint d;          /*Declare int for looping through domains*/   
    ajint s;          /*Declare int for looping through segments*/ 
    ajint single_seg = 1;   /* Number of segments when no match founf in 
			      CathDomList */
    
    
    EmbPCathName CathNamePtr = NULL;  /* Reusable CathName object pointer */
    EmbPCathDom  CathDomPtr  = NULL;  /* Reusable CathDom object pointer */
    AjPCath CathPtr         = NULL;   /* Reusable CathList object pointer */
    
    
    AjPList  CathNameList =NULL; /* List containing ptrs to CathName objects */
    AjPList  CathDomList  =NULL; /* List containing ptrs to CathDom objects */
    
    
    AjPStrTok handle = NULL;
    AjPStr    tmptok = NULL;
    
    
    /* Intitialise strings */
    tmptok          = ajStrNew();
    CathNameLine    = ajStrNew();
    CathDomLine     = ajStrNew();
    CathListLine    = ajStrNew();
    tmpDomainID     = ajStrNew();  
    tmpStringDomPtr = ajStrNew();
    tmpNumDomPtr    = ajStrNew();
    tmpDomIDNumDom  = ajStrNew();
    NDomAsString    = ajStrNew();
    tmpNumString    = ajStrNew();
    tmpNumString1    = ajStrNew();
    tmpNumString2    = ajStrNew();
    Search_DomainIDPtr = ajStrNew(); 
    
    ret = ajListNew();    

    
    /* Create list for EmbSCathName structures (CathNameList) to hold
       data from lines in CAT.names.all.v2.4  */
    
    
    CathNameList = ajListNew();
    CathDomList = ajListNew();
    
    
    /* Read all the lines in CAT.names.all.v2.4 and populate
       CathNameList */
    /* 1. Need a loop to read through every line in cathf 
       ... while(ajFileReadLine)
       {
	   2. Create a CathName structure ... CathNameNew
	       3. Extract data from line and write data structure   
		   4. Push the CathName pointer onto CathNameList ... 
       }
       */
    
    while(ajFileReadLine(namesf, &CathNameLine))
    {
    	CathNamePtr = embCathNameNew();
	
	/*1st token is classification index e.g 0002.0160 */
	handle = ajStrTokenInit (CathNameLine, " \t");
	ajStrToken(&(CathNamePtr)->Id, &handle, " \t");

	
	/*2nd token is domain code and should be discarded */
	ajStrToken(&tmptok, &handle, " \t");

	
	/*3rd token is classification text */
	ajStrTokenRest(&(CathNamePtr)->Desc, &handle);
	if(CathNamePtr->Desc->Ptr[0]==':')
	    ajStrTrim(&(CathNamePtr)->Desc, 1);

	ajStrTokenClear(&handle);
	

	/*
	StrTokPtr = ajStrTok(CathNameLine);		
	ajStrAssS(&(CathNamePtr)->Id, StrTokPtr);
	StrTokPtr = ajStrTok(NULL);		
	ajStrTokenRest(&StrTokPtr, NULL);
	ajStrAssS(&(CathNamePtr)->Desc, StrTokPtr);
	if(CathNamePtr->Desc->Ptr[0]==':')
	    ajStrTrim(&(CathNamePtr)->Desc, 1);
	    */

	/*
	ajFmtScanS(CathNameLine, "%S", &(CathNamePtr)->Id); 
	ajStrAssSub(&(CathNamePtr)->Desc, CathNameLine, 28, -1); 
	ajStrChomp(&(CathNamePtr)->Desc);
	if(CathNamePtr->Desc->Ptr[0]==':')
	    ajStrTrim(&(CathNamePtr)->Desc, 1);
	    */
	
	/* Push pointer to CathName object onto list*/
	ajListPush(CathNameList, CathNamePtr);
    } 
    
    
    
    /* Sort the list by cath classification number, AjPStr Id */

    /* Sort list using embSortNameId function */    
    ajListSort(CathNameList, embSortNameId); 
    /* make list into array and get array size - dimCathName */
    dimCathName = ajListToArray(CathNameList, (void ***) &CathNameArray); 
    
    
    /* We now have a list that we can do a binary search over */
    
    
    /* Create list of EmbSCathDom structures (CathDomList) to hold 
       data from lines in domlist.v2.4 */
    
    
    while(ajFileReadLine(domf, &CathDomLine))
    {	
	/*1st token is DomainID e.g 1cuk00*/
	/* ajStrTok goes through each element of line*/
	StrTokPtr = ajStrTok(CathDomLine);		
        
	/* Remove last num from string (0). Assign StrTokPtr to temp ptr 
	   tmpDomainID e.g. 1cuk0 */
	ajStrAssSub(&tmpDomainID, StrTokPtr, 0,4); 
	

	/*
	ajFmtPrint("tmpDomainID : %S\n",tmpDomainID );
	fflush(stdout);
	*/

	StrTokPtr = ajStrTok(NULL);	/*2nd token is no. of domains*/
	/* Assign value of StrTokPtr (no. of domains) to tmpStringDomPtr */ 
	ajStrAssS(&tmpStringDomPtr, StrTokPtr); 
	/* Remove first character (index 0 = the letter D) from 
	   tmpStringDomPtr and give to tmpDomInt */
	ajStrAssSub(&tmpNumDomPtr, tmpStringDomPtr, 1,2); 
	ajStrToInt(tmpNumDomPtr, &tmpDomInt);
	

	/*
	ajFmtPrint("tmpDomInt : %d\n",tmpDomInt );
	fflush(stdout);
	*/

	/*error-ajStrToInt(tmpDomIntPtr, &(tmpNumDomPtr)); */  
	/* Number of domains expressed as Int and assigned to tmpDomInt*/
	
	/*3rd token is no. of fragments, don't need this*/ 
	ajStrTok(NULL);			
	
	
	for (d=1; d<=tmpDomInt; d++)	/* For each domain */
	{
	    /* Get the number of segments */
	    StrTokPtr = ajStrTok(NULL); /* Token= no. of segments*/

/*	    ajFmtPrint("StrTokPtr : %S\n", StrTokPtr);
	    fflush(stdout); */
	    

	    /*Convert string containing no. of segs to int */
	    ajStrToInt(StrTokPtr, &(tmpNSegment)); 
	    
	    /* Create CathDom object giving tmpNSegment as argument */		
	    CathDomPtr = embCathDomNew(tmpNSegment); 
	    
	    
	    /* Converts value of d to a string */	
	    ajStrFromInt(&NDomAsString, d); 
	    if(d>1)
		ajStrChop(&tmpDomainID);
	    
		
	    /* Append no. of domains as a string (NDomAsString) onto end 
	       of tmpDomainID */
	    ajStrApp(&tmpDomainID, NDomAsString); 
	    /* Assign tmpDomainID to DomainID element in CathDom object */
	    ajStrAssS(&(CathDomPtr->DomainID), tmpDomainID); 
	    
	    
	    
	    for(s=0; s < CathDomPtr->NSegment; s++) /* For each segment */
	    {
		/* get Start and End residue numbers for each segment */ 
		/* nth (starting at token no. 5) token is Chain of starting residue*/
		ajStrTok(NULL);		
		/* (n+1)th token is start of segment res number */
		StrTokPtr = ajStrTok(NULL); 
		ajStrAssS(&(CathDomPtr->Start[s]), StrTokPtr);
		    
		/* Assign Start res no. to Start element in EmbPCathDom */
		/*error- ajStrToInt(StrTokPtr, &(CathDomPtr->Start[s]));*/ 
		/* (n+2)th token is "-" */
		ajStrTok(NULL);		
		/* (n+3)th token is Chain of ending residue*/
		ajStrTok(NULL);		
		/* (n+4)th token is end of segment res number */
		StrTokPtr = ajStrTok(NULL); 
		ajStrAssS(&(CathDomPtr->End[s]), StrTokPtr);
		    
		/* Assign End res no. to Start element in EmbPCathDom */
		/*error-   ajStrToInt(StrTokPtr, &(CathDomPtr->End[s]));*/ 
		ajStrTok(NULL);		/* (n+5)th token is "-" */

	    }

	    
	    
	    /* Read all the lines in domlist.v2.4 and populate CathDomList */
	    /* Push pointer to CathDom object onto list*/		 	
	    ajListPush(CathDomList, CathDomPtr); 
	}
    }
    
    
    
    /* Sort the list by domain code (ajListSort by DomainId) 
       We now have a list that we can do a binary search over */
    
    /* Sort list using embSortDomainID function */
    ajListSort(CathDomList, embSortDomainID); 
    /* make list into array and get array size - dimCathDom */
    dimCathDom = ajListToArray(CathDomList, (void ***) &CathDomArray); 
    
    
    /* Start of main application loop */
    /* while there is a line to read from caths.list.v2.4, 
       read a line into a string ... ajFileReadLine*/ 
    while(ajFileReadLine(cathf, &CathListLine))
    {
	/* Extract DomainId from string and write to temp. variable 
	   - Search_DomainIDPtr */
		    
	/* DomainID held in temp string */
	ajFmtScanS(CathListLine, "%S", &(Search_DomainIDPtr)); 
		    
		    
	/* Binary search of Search_StringPtr against DomainID element 
	   in CathDomList */
		    
	/* Binary search of Search_DomainIDPtr over array of 
	   CathDom objects */
	idxCathDom = embCathDomBinSearch(Search_DomainIDPtr, 
					 CathDomArray, dimCathDom); 
	/* sorted by AjPStr Id */
	if(idxCathDom != -1		/*match found*/)
	{
	    /* Extract number of segments  */
	    /* Extract number of segments from CathDom and assign 
	       to tmpNSegment */
	    (tmpNSegment) = (CathDomArray[idxCathDom]->NSegment); 
	    						   

	    /* Allocate the Cath object, AjXyzCathNew */
	    
	    /* Create Cath object giving tmpNSegment as argument */		
	    CathPtr = ajCathNew(tmpNSegment); 


	    
	    /* Assign DomainId from CathDom to Cath objects */ 	   
	    ajStrAssS(&(CathPtr->DomainID), 
		      CathDomArray[idxCathDom]->DomainID); 
	    /* Assign number of segments to NSegment element in Cath object */
	    (CathPtr->NSegment) = (CathDomArray[idxCathDom]->NSegment); 
	    
	   
	    /* Write the number of segments and start and end points */
	    /*
	    printf("Number of segments for CathPtr : %d\n", 
	    CathPtr->NSegment);
	    printf("Number of segments for CathDomPtr : %d\n\n", 
	    CathDomPtr->NSegment);
            */
	    for(s=0; s<(CathPtr->NSegment); s++) /* For each segment */
	    {
		/* get Start and End residue numbers for each segment */ 

		/* Assign value of start from CathDom to CathPtr */
		ajStrAssS(&(CathPtr->Start[s]), 
			  CathDomArray[idxCathDom]->Start[s]); 
		/* Assign value of end from CathDom to CathPtr */
		ajStrAssS(&(CathPtr->End[s]), 
			  CathDomArray[idxCathDom]->End[s]); 
			
	    }      
	}
	/* no match found => only one domain in protein */
	else				
	{
	    /* Presume that domain contains a single segment, 
	       single_seg = 1*/

	    /* Allocate the Cath object, AjXyzCathNew */
	    
	    /* Create Cath object giving tmpNSegment as argument */
	    CathPtr = ajCathNew(single_seg); 
	    
	    /* Assign DomainId from Search_DomainIDPtr */
	    ajStrAssS(&(CathPtr->DomainID), Search_DomainIDPtr); 
	    
	    /* Assign the number of segments as 1 */
	    /* Assign number of segments to NSegment element in Cath 
	       object from single_seg */
	    (CathPtr->NSegment) = (single_seg); 
	               
		  
	    /* get Start and End residue numbers for each segment */ 
	    ajStrAssC(&(CathPtr->Start[0]), ".");
	    ajStrAssC(&(CathPtr->End[0]), ".");

	    /* Assign value of start to "." */ 
	    /*urrggh! ((CathPtr->Start[0]) = ".");*/
	    /* Assign value of end to "." */ 		       
	    /*yeek!	  ((CathPtr->End[0]) = ".");*/
	    
	}


	/* Extract Pdb code from DomainId */
	ajStrAssSub(&(CathPtr->Pdb), CathPtr->DomainID, 0,3);
	
	/* Extract chain identifer from DomainId */
	CathPtr->Chain=ajStrChar(CathPtr->DomainID, 4);
		    
	/* ajStrChar char from string */
	/* error-ajStrAssSub(&(CathPtr->Chain), CathPtr->DomainID, 4,4);*/
	
        /* Extract length of domain from string */
	/* Take the 9th element of line and assign to Length in Cath object */
	ajFmtScanS(CathListLine, "%*S %*d %*d %*d %*d %*d %*d %*d %d", 
		   &(CathPtr->Length)); 

	
        /* Extract ajint Class_Id, Arch_Id, Topology_Id, Superfamily_Id, 
	   NIFamily_Id, Family_Id, IFamily_Id from string and write into
	   AjPCath */
	ajFmtScanS(CathListLine, "%*S %d %d %d %d %d %d %d", 
		   &intC, &intA, &intT, &intH, &intF, &intNI, &intI);
	
	(CathPtr->Class_Id)       = (intC); 
	(CathPtr->Arch_Id)        = (intA);
	(CathPtr->Topology_Id)   = (intT);
	(CathPtr->Superfamily_Id) = (intH);
	(CathPtr->Family_Id)      = (intF);
	(CathPtr->NIFamily_Id)    = (intNI);
	(CathPtr->IFamily_Id)        = (intI);
	          
	    
	/* Construct number string for SUPERFAMILY from Class_Id, Arch_Id, 
	   Topology_Id, 
	   Superfamily_Id and store in temp. variable (format X.XX.XX.XX) */ 
	    
	/* Make string containg CATH id numbers */ 
	ajFmtPrintS(&tmpNumString, "%d.%d.%d.%d", intC, intA, intT, intH); 

	/* Binary search using temp. variable in EmbSCathName */
	
	ajFmtPrintF(logf, "%S\n", tmpNumString);
	
	/* Binary search of tmpNumString over array of CathName objects */
        idxCathName = embCathNameBinSearch(tmpNumString, CathNameArray, 
					   dimCathName); 
	
	if ( idxCathName != -1)		/*match found*/
        {
	    /* Extract Superfamily string and write into AJPCath */
            ajStrAssS(&(CathPtr->Superfamily),
		      (CathNameArray[idxCathName]->Desc)); 
        }
        else				/*no match found*/
        {
            /* Write Superfamily string as a '.'  */
	    ajStrAssC(&(CathPtr->Superfamily), ".");
	    
	    /*error	    (CathPtr->Superfamily) = "."); */
        }               
        
	
	/* Construct number string for TOPOLOGY from Class_Id, Arch_Id, Topology_Id, 
	   and store in temp. variable (format X.XX.XX) */ 

	/* Make string containg CAT id numbers */
        ajFmtPrintS(&tmpNumString, "%d.%d.%d", intC, intA, intT); 
	
	/* Binary search using temp. variable in EmbSCathName */
	ajFmtPrintF(logf, "%S\n", tmpNumString);
	/* Binary search of tmpNumString over array of CathName objects */
        idxCathName = embCathNameBinSearch(tmpNumString, CathNameArray, 
					   dimCathName); 
        
	if ( idxCathName != -1)		/*match found*/
        {
	    /* Extract Topology string  and write into AJPCath*/
	    ajStrAssS(&(CathPtr->Topology), CathNameArray[idxCathName]->Desc);
        }
        else				/*no match found*/
        {
            /* Write topology as a '.'  */
	    ajStrAssC(&(CathPtr->Topology), ".");
	    /*error	    CathPtr->Topology) = "."); */
        }                         


	/* Construct number string for ARCHITECTURE from Class_Id, Arch_Id, 
	   and store in temp. variable */ 
	/* Class and Architecture numbers in domlist.v2.4 are in format 
	   XXXX.XXXX */
	/*
	if(intC < 10)
	    ajFmtPrintS(&tmpNumString1, "000%d", intC); 
	else
	    ajFatal("MIKE GIVE A ERROR MESSAGE");
	if(intA < 10)
	    ajFmtPrintS(&tmpNumString2, "000%d", intA); 
	else if(intA < 100)
	    ajFmtPrintS(&tmpNumString2, "00%d", intA); 
	else if(intA < 1000)
	    ajFmtPrintS(&tmpNumString2, "0%d", intA); 
	else if(intA < 10000)
	    ajFmtPrintS(&tmpNumString2, "%d", intA); 
	else
	    ajFatal("MIKE GIVE A ERROR MESSAGE");
	ajFmtPrintS(&tmpNumString, "%S.%S", tmpNumString1, tmpNumString2);
	*/
	ajFmtPrintS(&tmpNumString, "%04d.%04d", intC, intA);
	

	/* Make string containg CA id numbers */
	/*	ajFmtPrintS(&tmpNumString, "000%d.%d", intC, intA);*/ 
	
	/* Binary search using temp. variable in EmbSCathName */
	ajFmtPrintF(logf, "%S\n", tmpNumString);
	/* Binary search of tmpNumString over array of CathName objects */
        idxCathName = embCathNameBinSearch(tmpNumString, CathNameArray, 
					   dimCathName); 
	
        
	if ( idxCathName != -1 )	/*match found*/
        {
	    /* Extract Architecture string and write into AJPCath*/
	    ajStrAssS(&(CathPtr->Architecture), 
		      CathNameArray[idxCathName]->Desc);
        }
        else				/*no match found*/
        {
            /* Write architecture as a '.' */
	    ajStrAssC(&(CathPtr->Architecture), ".");
	    /*error	    (CathPtr->Architecture) = "."); */
        }    


	/* Construct number string for CLASS from Class_Id and store in 
	   temp. variable */ 
	if(intC < 10)
	    ajFmtPrintS(&tmpNumString, "%04d", intC); 
	else
	    ajFatal("MIKE GIVE A ERROR MESSAGE");




	/* Binary search using temp. variable in EmbSCathName */
	ajFmtPrintF(logf, "%S\n", tmpNumString);
	/* Binary search of tmpNumString over array of CathName objects */
        idxCathName = embCathNameBinSearch(tmpNumString, CathNameArray, 
					   dimCathName); 
        
	
	if ( idxCathName != -1)		/*match found*/
        {
	    /* Extract Class string and write into AJPCath*/
	    ajStrAssS(&(CathPtr->Class), CathNameArray[idxCathName]->Desc);
        }
        else				/*no match found*/
        {
            /* Write class as a '.' */
	    ajStrAssC(&(CathPtr->Class), ".");
	    /*error	    (CathPtr->Class) = "."); */
        }    

	/* Push the Cath object onto list */
	ajListPushApp(ret, CathPtr);

    } /* End of main application loop */
    
    
    /* Free the memory for the list and nodes in 
       list of EmbSCathName structures (ajListFree) */
    while(ajListPop(CathNameList, (void **) &CathNamePtr))
	embCathNameDel(&CathNamePtr);
    ajListDel(&CathNameList);
    
    /* Free the memory for the list and nodes in 
       list of EmbSCathDom structures (ajListFree)  */
    while(ajListPop(CathDomList, (void **) &CathDomPtr))
	embCathDomDel(&CathDomPtr);
    ajListDel(&CathDomList);
    

    
    /* Tidy up */
    ajStrDel(&tmptok);
    ajStrDel(&CathNameLine);
    ajStrDel(&CathDomLine);
    ajStrDel(&CathListLine);
    ajStrDel(&tmpStringDomPtr);
    ajStrDel(&tmpNumDomPtr);
    ajStrDel(&tmpDomainID);
    ajStrDel(&tmpDomIDNumDom);
    ajStrDel(&tmpNumString);
    ajStrDel(&tmpNumString1);
    ajStrDel(&tmpNumString2);
}





/* @func embCathDomNew ******************************************************
**
** CathDom object constructor. Fore-knowledge of the number of segments is 
** required.
**
** @param [r] nsegments [ajint] Number of segments
**
** @return [EmbPCathDom] Pointer to a CathDom object
** @@
****************************************************************************/
EmbPCathDom embCathDomNew(ajint nsegments)
{
    EmbPCathDom ret = NULL;
    ajint x;    

    AJNEW0(ret);
    
    ret->DomainID = ajStrNew();
    
    if(nsegments > 0)
    {
	AJCNEW0(ret->Start, nsegments);
	AJCNEW0(ret->End, nsegments);
	
	for(x=0; x<nsegments; x++)
	{
	    ret->Start[x] = ajStrNew();
	    ret->End[x]   = ajStrNew();
	}
    }
    else
    {
	ret->Start = NULL;
	ret->End   = NULL;
    }


    ret->NSegment = nsegments;

    return ret;
}





/* @func embCathDomDel ******************************************************
**
** Destructor for CathDom object. 
**
** @param [w] ptr [EmbPCathDom*] 
**
** @return [void] 
** @@
****************************************************************************/
void embCathDomDel(EmbPCathDom *ptr)
{
    EmbPCathDom pthis = *ptr;
    
    ajint x;
    
    if(!pthis || !ptr)      /*(pthis==NULL)||(ptr==NULL)*/
	return;
 
    ajStrDel(&pthis->DomainID);
    
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
    pthis=NULL;
    
    return;
}
  




/* @func embCathNameNew *****************************************************
**
** CathName object constructor.
**
** @return [EmbPCathName] Pointer to a CathName object
** @@
****************************************************************************/
EmbPCathName embCathNameNew(void)
{
    EmbPCathName ret = NULL;
    
    AJNEW0(ret);
    
    
    ret->Id	     = ajStrNew();
    ret->Desc	     = ajStrNew();

    return ret;
}





/* @func aembCathNameDel ****************************************************
**
** Destructor for CathName object. 
**
** @param [w] ptr [EmbPCathName *] CathName object pointer
** @return [void] 
** @@
****************************************************************************/
void embCathNameDel(EmbPCathName *ptr)
{
    EmbPCathName pthis = *ptr;
    
    if(!pthis || !ptr)     /*(pthis==NULL)||(ptr==NULL)*/
  return;
  
    ajStrDel(&pthis->Id);
    ajStrDel(&pthis->Desc);
    
    AJFREE(pthis);
    pthis=NULL;
    
    return;
}
 
 
 
 
    
/* @funcstatic  embSortNameId ***********************************************
**
** Function to sort CathName objects by Id element.
**
** @param [r] cath1  [const void*] Pointer to EmbOCathName object 1
** @param [r] cath2  [const void*] Pointer to EmbOCathName object 2
**
** @return [ajint] -1 if Id1 should sort before Id2, +1 if the Id2 
** should sort first. 0 if they are identical in value.
** @@
****************************************************************************/
static ajint embSortNameId(const void *cath1, const void *cath2)
{
    EmbPCathName p  = NULL;
    EmbPCathName q  = NULL;

    p = (*(EmbPCathName*)cath1);
    q = (*(EmbPCathName*)cath2);
    
    return ajStrCmpO(p->Id, q->Id);
}


 
  
   
/* @funcstatic  embSortDomainID *********************************************
**
** Function to sort CathDom objects by DomainID element.
**
** @param [r] DomID1  [const void*] Pointer to CathDom object 1
** @param [r] DomID2  [const void*] Pointer to CathDom object 2
**
** @return [ajint] -1 if DomID1 should sort before DomID2, +1 if the DomID2 
** should sort first. 0 if they are identical in value.
** @@
****************************************************************************/
static ajint embSortDomainID(const void *DomID1, const void *DomID2)
{
    EmbPCathDom p  = NULL;
    EmbPCathDom q  = NULL;

    p = (*(EmbPCathDom*)DomID1);
    q = (*(EmbPCathDom*)DomID2);
    
    return ajStrCmpO(p->DomainID, q->DomainID);

}





/* @func embCathDomBinSearch ************************************************
**
** Performs a binary search for a domain code over an array of CathDom
** structures (which of course must first have been sorted). This is a 
** case-insensitive search.
**
** @param [r] id  [AjPStr]       Search term
** @param [r] arr [EmbPCathDom*] Array of EmbPCathDom objects
** @param [r] siz [ajint]        Size of array
**
** @return [ajint] Index of first EmbPCathDom object found with an domain code
** matching id, or -1 if id is not found.
** @@
****************************************************************************/
ajint embCathDomBinSearch(AjPStr id, EmbPCathDom *arr, ajint siz)
{
    int l;
    int m;
    int h;
    int c;


    l=0;
    h=siz-1;
    while(l<=h)
    {
        m=(l+h)>>1;

        if((c=ajStrCmpCase(id, arr[m]->DomainID)) < 0) 
	    h=m-1;
        else if(c>0) 
	    l=m+1;
        else 
	    return m;
    }
    return -1;
}





/* @func embCathNameBinSearch ***********************************************
**
** Performs a binary search for a domain code over an array of CathName
** structures (which of course must first have been sorted). This is a 
** case-insensitive search.
**
** @param [r] id  [AjPStr]       Search term
** @param [r] arr [EmbPCathDom*] Array of CathDom objects
** @param [r] siz [ajint]        Size of array
**
** @return [ajint] Index of first CathName object found with an CATH Id code
** matching id, or -1 if id is not found.
** @@
****************************************************************************/
ajint embCathNameBinSearch(AjPStr id, EmbPCathName *arr, ajint siz)
{
    int l;
    int m;
    int h;
    int c;


    l=0;
    h=siz-1;
    while(l<=h)
    {
        m=(l+h)>>1;

        if((c=ajStrCmpCase(id, arr[m]->Id)) < 0) 
	    h=m-1;
        else if(c>0) 
	    l=m+1;
        else 
	    return m;
    }
    return -1;
}





/* @func embScopclaNew ******************************************************
**
** Scopcla object constructor. Fore-knowledge of the number of chains is 
** required. This is normally called by the embScopclaReadC / 
** embScopclaRead functions.
**
** @param [r] chains [ajint] Number of chains
**
** @return [EmbPScopcla] Pointer to a scopcla object
** @@
****************************************************************************/

EmbPScopcla embScopclaNew(ajint chains)
{
    EmbPScopcla ret = NULL;
    ajint i;

    AJNEW0(ret);

    ret->Entry = ajStrNew();
    ret->Pdb   = ajStrNew();
    ret->Sccs  = ajStrNew();

    if(chains)
    {
	ret->Chain = ajCharNewL(chains);
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





/* @func embScopdesNew ******************************************************
**
** Scopdes object constructor.
**
** This is normally called by the embScopdesReadC / embScopdesRead
** functions.
**
** @return [EmbPScopdes] Pointer to a scopdes object
** @@
****************************************************************************/

EmbPScopdes embScopdesNew(void)
{
    EmbPScopdes ret = NULL;

    AJNEW0(ret);

    ret->Type      = ajStrNew();
    ret->Sccs      = ajStrNew();
    ret->Entry     = ajStrNew();
    ret->Desc      = ajStrNew();
    
    return ret;
}





/* @func embScopclaRead *****************************************************
**
** Read a Scopcla object for a given SCOP domain from the SCOP parsable 
** file (dir.cla.scop.txt).
**
** @param [r] inf   [AjPFile]      Input file stream
** @param [r] entry [AjPStr]       id
** @param [w] thys  [EmbPScopcla*] Scopcla object
**
** @return [AjBool] True on success
** @@
****************************************************************************/

AjBool embScopclaRead(AjPFile inf, AjPStr entry, EmbPScopcla *thys)
{
    return embScopclaReadC(inf,ajStrStr(entry),thys);
}




/* @func embScopdesRead *****************************************************
**
** Read a Scopdes object for a given SCOP domain from the SCOP parsable 
** file (dir.des.scop.txt).
**
** @param [r] inf   [AjPFile]      Input file stream.
** @param [r] entry [AjPStr]       CATH id of domain.
** @param [w] thys  [EmbPScopdes*] Scopdes object.
**
** @return [AjBool] True on success
** @@
****************************************************************************/

AjBool embScopdesRead(AjPFile inf, AjPStr entry, EmbPScopdes *thys)
{
    return embScopdesReadC(inf,ajStrStr(entry),thys);
}





/* @func embScopdesReadC ****************************************************
**
** Read a Scopdes object for a given SCOP domain from the SCOP parsable 
** file (dir.des.scop.txt).
**
** @param [r] inf   [AjPFile]      Input file stream.
** @param [r] entry [char*]        SCOP id of domain.
** @param [w] thys  [EmbPScopdes*] Scopdes object.
**
** @return [AjBool] True on success
** @@
****************************************************************************/

AjBool embScopdesReadC(AjPFile inf, char *entry, EmbPScopdes *thys)
{
    static AjPStr line     = NULL;   /* Line from file */
    static AjPStr sunidstr = NULL;   /* sunid as string */
    static AjPStr tentry   = NULL;
    static AjPStr tmp      = NULL;
    static AjPRegexp rexp  = NULL;

    AjBool ok = ajFalse;
    
    
    /* Only initialise strings if this is called for the first time */
    if(!line)
    {    
	line     = ajStrNew();
	tentry   = ajStrNew();
	sunidstr = ajStrNew();
	tmp      = ajStrNew();

	rexp  = ajRegCompC(
	    "^([^ \t]+)[ \t]+([^ \t]+)[ \t]+([^ \t]+)[ \t]+([^ \t]+)[ \t]+");
    }
    

    /* Read up to the correcty entry (line) */
    ajStrAssC(&tentry,entry);
    ajStrToUpper(&tentry);
    
    while((ok=ajFileReadLine(inf,&line)))
    {
	if((ajFmtScanS(line, "%S", &sunidstr)==0))
	    return ajFalse;

	/* Ignore comment lines */
	if(*(line->Ptr) == '#')
	    continue;
	
	if(ajStrMatchWild(sunidstr,tentry))
	    break;
    }
    
    if(!ok)
	return ajFalse;

    *thys = embScopdesNew();
    
    if((ajFmtScanS(line, "%d %S %S %S", &(*thys)->Sunid,&(*thys)->Type,
		   &(*thys)->Sccs, &(*thys)->Entry)!=4))
	return ajFalse;


    /* Tokenise the line by ' ' and discard the first 4 strings */
    
    if(!ajRegExec(rexp,line))
    {
	ajFmtPrint("-->  %S\n", line);
	ajFatal("File read error in embScopdesReadC");
    }

    
    ajRegSubI(rexp,1,&tmp);
    ajRegSubI(rexp,2,&tmp);
    ajRegSubI(rexp,3,&tmp);
    ajRegSubI(rexp,4,&tmp);
    ajRegPost(rexp,&(*thys)->Desc);
    ajStrClean(&(*thys)->Desc);

    return ajTrue;
}





/* @func embScopclaReadC ****************************************************
**
** Read a Scopcla object for a given SCOP domain from the SCOP parsable 
** file (dir.des.scop.txt).
**
** @param [r] inf   [AjPFile]      Input file stream
** @param [r] entry [char*]        SCOP domain id
** @param [w] thys  [EmbPScopcla*] Scopcla object
**
** @return [AjBool] True on success
** @@
****************************************************************************/

AjBool embScopclaReadC(AjPFile inf, char *entry, EmbPScopcla *thys)
{
    static AjPStr line   = NULL;
    static AjPStr scopid = NULL;  /* SCOP code */
    static AjPStr pdbid  = NULL;  /* PDB code */
    static AjPStr chains = NULL;  /* Chain data */
    static AjPStr sccs   = NULL;  /* Scop compact classification string */
    static AjPStr class  = NULL;  /* Classification containing all
					 SCOP sunid's  */
    static AjPStr tentry = NULL;
    static AjPStr token  = NULL;
    static AjPStr str    = NULL;

    static AjPRegexp exp = NULL;

    AjPStrTok handle  = NULL;
    AjPStrTok bhandle = NULL;
    AjBool ok         = ajFalse;    

    char c   = ' ';
    char *p  = NULL;
    ajint n  = 0;
    ajint i  = 0;
    ajint from;
    ajint to;


    /* Only initialise strings if this is called for the first time */
    if(!line)
    {    
	line    = ajStrNew();
	scopid  = ajStrNew();
	pdbid   = ajStrNew();
	chains  = ajStrNew();
	sccs    = ajStrNew();
	tentry  = ajStrNew();
	token   = ajStrNew();
	str     = ajStrNew();
	
	exp   = ajRegCompC("^([0-9]+)([A-Za-z]+)[-]([0-9]+)");
    }
    

    /* Read up to the correcty entry (line) */
    ajStrAssC(&tentry,entry);
    ajStrToUpper(&tentry);
    
    while((ok=ajFileReadLine(inf,&line)))
    {
	if((ajFmtScanS(line, "%S", &scopid)==0))
	    return ajFalse;

	/* Ignore comment lines */
	if(*scopid->Ptr == '#')
	    continue;
		
	if(ajStrMatchWild(scopid,tentry))
	    break;
    }
    
    if(!ok)
	return ajFalse;


    if((ajFmtScanS(line, "%*S %S %S %S %*d %S", &pdbid,&chains, &sccs,
		   &class)!=4))
	return ajFalse;

    /* Count chains and allocate Scopcla object */
    n = ajStrTokenCount(&chains,",");
    *thys = embScopclaNew(n);

    ajStrToUpper(&scopid);
    ajStrAssS(&(*thys)->Entry,scopid);

    ajStrToUpper(&pdbid);
    ajStrAssS(&(*thys)->Pdb,pdbid);

    ajStrToUpper(&sccs);
    ajStrAssS(&(*thys)->Sccs,sccs);

    handle = ajStrTokenInit(chains,",");
    for(i=0;i<n;++i)
    {
	ajStrToken(&token,&handle,NULL);
	    	    
	p = ajStrStr(token);
	if(sscanf(p,"%d-%d",&from,&to)==2)
	{
	    (*thys)->Chain[i]='.';
	    ajFmtPrintS(&(*thys)->Start[i],"%d",from);
	    ajFmtPrintS(&(*thys)->End[i],"%d",to);
	}
	else if(sscanf(p,"%c:%d-%d",&c,&from,&to)==3)
	{
	    ajFmtPrintS(&(*thys)->Start[i],"%d",from);
	    ajFmtPrintS(&(*thys)->End[i],"%d",to);
	    (*thys)->Chain[i]=c;
	}
	else if(ajStrChar(token,1)==':')
	{
	    ajStrAssC(&(*thys)->Start[i],".");
	    ajStrAssC(&(*thys)->End[i],".");
	    (*thys)->Chain[i]=*ajStrStr(token);
	}
	else if(ajRegExec(exp,token))
	{
	    ajRegSubI(exp,1,&str);
	    ajStrAssS(&(*thys)->Start[i],str);
	    ajRegSubI(exp,2,&str);
	    (*thys)->Chain[i] = *ajStrStr(str);
	    ajRegSubI(exp,3,&str);
	    ajStrAssS(&(*thys)->End[i],str);
	}
	else if(ajStrChar(token,0)=='-')
	{
	    (*thys)->Chain[i]='.';
	    ajStrAssC(&(*thys)->Start[i],".");
	    ajStrAssC(&(*thys)->End[i],".");
	}
	else
	    ajFatal("Unparseable chain line [%S]\n",chains);
    }
    ajStrTokenClear(&handle);
	      
	      
    /* Read SCOP sunid's from classification string */
    bhandle = ajStrTokenInit(class,",\n");
    while(ajStrToken(&token,&bhandle,NULL))
    {
	if(ajStrPrefixC(token,"cl"))
	    ajFmtScanS(token, "cl=%d", &(*thys)->Class);
	else if(ajStrPrefixC(token,"cf"))
	    ajFmtScanS(token, "cf=%d", &(*thys)->Fold);
	else if(ajStrPrefixC(token,"sf"))
	    ajFmtScanS(token, "sf=%d", &(*thys)->Superfamily);
	else if(ajStrPrefixC(token,"fa"))
	    ajFmtScanS(token, "fa=%d", &(*thys)->Family);
	else if(ajStrPrefixC(token,"dm"))
	    ajFmtScanS(token, "dm=%d", &(*thys)->Domain);
	else if(ajStrPrefixC(token,"sp"))
	    ajFmtScanS(token, "sp=%d", &(*thys)->Source);
	else if(ajStrPrefixC(token,"px"))
	    ajFmtScanS(token, "px=%d", &(*thys)->Domdat);
    }
    ajStrTokenClear(&bhandle);

    return ajTrue;
}	





/* @func embScopclaDel ******************************************************
**
** Destructor for scopcla object.
**
** @param [w] thys [EmbPScopcla *] Scopcla object pointer
**
** @return [void]
** @@
****************************************************************************/

void embScopclaDel(EmbPScopcla *thys)
{
    EmbPScopcla pthis;
    ajint i;

    pthis = *thys;

    if(!pthis || !thys)
	return;

    ajStrDel(&pthis->Entry);
    ajStrDel(&pthis->Pdb);
    ajStrDel(&pthis->Sccs);

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
    (*thys)=NULL;

    return;
}





/* @func  embScopdesBinSearch ***********************************************
**
** Performs a binary search for a Sunid over an array of Scopdes objects 
** structures (which of course must first have been sorted, e.g. by using 
** embScopdesCompSunid).
**
** @param [r] id  [ajint]        Search value of Sunid
** @param [r] arr [EmbPScopdes*] Array of Scopdes objects
** @param [r] siz [ajint]        Size of array
**
** @return [ajint] Index of first Scopdes object found with a Sunid 
**                 element matching id, or -1 if id is not found.
** @@
****************************************************************************/

ajint embScopdesBinSearch(ajint id, EmbPScopdes *arr, ajint siz)
{
    int l;
    int m;
    int h;
    

    l = 0;
    h = siz-1;
    while(l<=h)
    {
        m = (l+h)>>1;

	if(id < arr[m]->Sunid)
	    h = m-1;
	else if(id > arr[m]->Sunid)
	    l = m+1;
	else
	    return m;
    }

    return -1;
}





/* @func embScopdesDel ******************************************************
**
** Scopdes object destructor.
**
** @param [w] ptr [EmbPScopdes *] Scopdes object pointer
**
** @return [void] 
** @@
****************************************************************************/

void embScopdesDel(EmbPScopdes *ptr)
{
    /* Check arg's */
    if(*ptr==NULL)
	return;
    
    if((*ptr)->Type)
	ajStrDel( &((*ptr)->Type)); 

    if((*ptr)->Sccs)
	ajStrDel( &((*ptr)->Sccs)); 

    if((*ptr)->Entry)
	ajStrDel( &((*ptr)->Entry)); 

    if((*ptr)->Desc)
	ajStrDel( &((*ptr)->Desc)); 

    AJFREE(*ptr);
    *ptr = NULL;
    
    return;
    
}





/* @func  embScopdesCompSunid ***********************************************
**
** Function to sort Scopdes objects by Sunid element.
**
** @param [r] scop1  [const void*] Pointer to Scopdes object 1
** @param [r] scop2  [const void*] Pointer to Scopdes object 2
**
** @return [ajint] -1 if Sunid1 should sort before Sunid2, +1 if the Sunid2 
** should sort first. 0 if they are identical in value.
** @@
****************************************************************************/

ajint embScopdesCompSunid(const void *scop1, const void *scop2)
{
    EmbPScopdes p = NULL;
    EmbPScopdes q = NULL;

    p = (*  (EmbPScopdes*)scop1);
    q = (*  (EmbPScopdes*)scop2);
    
    if(p->Sunid < q->Sunid)
	return -1;
    else if(p->Sunid == q->Sunid)
	return 0;

    return 1;
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
