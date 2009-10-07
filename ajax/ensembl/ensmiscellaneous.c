/******************************************************************************
** @source Ensembl Miscellaneous functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.3 $
** @@
**
** Bio::EnsEMBL::MiscSet CVS Revision: 1.5
** Bio::EnsEMBL::DBSQL::MiscSetadaptor CVS Revision: 1.8
** Bio::EnsEMBL::MiscFeature CVS Revision: 1.12
** Bio::EnsEMBL::DBSQL::MiscFeatureadaptor CVS Revision: 1.18
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
******************************************************************************/

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ensmiscellaneous.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPAssemblyMapperadaptor
ensRegistryGetAssemblyMapperadaptor(EnsPDatabaseadaptor dba);

extern EnsPCoordSystemadaptor
ensRegistryGetCoordSystemadaptor(EnsPDatabaseadaptor dba);

extern EnsPMiscellaneousFeatureadaptor
ensRegistryGetMiscellaneousFeatureadaptor(EnsPDatabaseadaptor dba);

extern EnsPMiscellaneousSetadaptor
ensRegistryGetMiscellaneousSetadaptor(EnsPDatabaseadaptor dba);

extern EnsPSliceadaptor
ensRegistryGetSliceadaptor(EnsPDatabaseadaptor dba);

static AjBool miscellaneousSetadaptorFetchAllBySQL(
    EnsPMiscellaneousSetadaptor msa,
    const AjPStr statement,
    AjPList mslist);

static AjBool miscellaneousSetadaptorCacheInsert(
    EnsPMiscellaneousSetadaptor msa,
    EnsPMiscellaneousSet *Pms);

#if AJFALSE
static AjBool miscellaneousSetadaptorCacheRemove(
    EnsPMiscellaneousSetadaptor msa,
    EnsPMiscellaneousSet ms);
#endif

static AjBool miscellaneousSetadaptorCacheInit(EnsPMiscellaneousSetadaptor msa);

static void miscellaneousSetadaptorCacheClearIdentifier(void **key,
                                                        void **value,
                                                        void *cl);

static void miscellaneousSetadaptorCacheClearCode(void **key,
                                                  void **value,
                                                  void *cl);

static AjBool miscellaneousSetadaptorCacheExit(
    EnsPMiscellaneousSetadaptor adaptor);

static void miscellaneousSetadaptorFetchAll(const void *key, void **value,
                                            void *cl);

static AjBool miscellaneousFeatureadaptorHasAttribute(AjPTable attributes,
                                                      const AjPStr code,
                                                      const AjPStr value);

static AjBool miscellaneousFeatureadaptorClearAttributes(AjPTable attributes);

static AjBool miscellaneousFeatureadaptorHasMiscellaneousSet(AjPTable sets,
                                                             ajuint msid);

static AjBool miscellaneousFeatureadaptorClearMiscellaneousSets(AjPTable sets);

static AjBool miscellaneousFeatureadaptorFetchAllBySQL(
    EnsPDatabaseadaptor dba,
    const AjPStr statement,
    EnsPAssemblyMapper am,
    EnsPSlice slice,
    AjPList mflist);

static void *miscellaneousFeatureadaptorCacheReference(void *value);

static void miscellaneousFeatureadaptorCacheDelete(void **value);

static ajuint miscellaneousFeatureadaptorCacheSize(const void *value);

static EnsPFeature miscellaneousFeatureadaptorGetFeature(const void *value);




/* @filesection ensmiscellaneous **********************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/



/* @datasection [EnsPMiscellaneousSet] Miscellaneous Set **********************
**
** Functions for manipulating Ensembl Miscellaneous Set objects
**
** @cc Bio::EnsEMBL::MiscSet CVS Revision: 1.6
**
** @nam2rule MiscellaneousSet
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Miscellaneous Set by pointer.
** It is the responsibility of the user to first destroy any previous
** Miscellaneous Set. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMiscellaneousSet]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMiscellaneousSet] Ensembl Miscellaneous Set
** @argrule Ref object [EnsPMiscellaneousSet] Ensembl Miscellaneous Set
**
** @valrule * [EnsPMiscellaneousSet] Ensembl Miscellaneous Set
**
** @fcategory new
******************************************************************************/




/* @func ensMiscellaneousSetNew ***********************************************
**
** Default constructor for an Ensembl Miscellaneous Set.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                                  Set Adaptor.
** @param [r] identifier [ajuint] SQL database-internal identifier.
** @cc Bio::EnsEMBL::MiscSet::new
** @param [u] code [AjPStr] Code.
** @param [u] name [AjPStr] Name.
** @param [u] description [AjPStr] Description.
** @param [r] maxlen [ajuint] Maximum length.
**
** @return [EnsPMiscellaneousSet] Ensembl Miscellaneous Set or NULL.
** @@
******************************************************************************/

EnsPMiscellaneousSet ensMiscellaneousSetNew(
    EnsPMiscellaneousSetadaptor adaptor,
    ajuint identifier,
    AjPStr code,
    AjPStr name,
    AjPStr description,
    ajuint maxlen)
{
    EnsPMiscellaneousSet ms = NULL;
    
    AJNEW0(ms);
    
    ms->Use = 1;
    
    ms->Identifier = identifier;
    
    ms->Adaptor = adaptor;
    
    if(code)
        ms->Code = ajStrNewRef(code);
    
    if(name)
        ms->Name = ajStrNewRef(name);
    
    if(description)
        ms->Description = ajStrNewRef(description);
    
    ms->MaximumLength = maxlen;
    
    return ms;
}




/* @func ensMiscellaneousSetNewObj ********************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
**
** @return [EnsPMiscellaneousSet] Ensembl Miscellaneous Set or NULL.
** @@
******************************************************************************/

EnsPMiscellaneousSet ensMiscellaneousSetNewObj(
    const EnsPMiscellaneousSet object)
{
    EnsPMiscellaneousSet ms = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(ms);
    
    ms->Use = 1;
    
    ms->Identifier = object->Identifier;
    
    ms->Adaptor = object->Adaptor;
    
    if(object->Code)
        ms->Code = ajStrNewRef(object->Code);
    
    if(object->Name)
        ms->Name = ajStrNewRef(object->Name);
    
    if(object->Description)
        ms->Description = ajStrNewRef(object->Description);
    
    ms->MaximumLength = object->MaximumLength;
    
    return ms;
}




/* @func ensMiscellaneousSetNewRef ********************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] ms [EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
**
** @return [EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
** @@
******************************************************************************/

EnsPMiscellaneousSet ensMiscellaneousSetNewRef(EnsPMiscellaneousSet ms)
{
    if(!ms)
	return NULL;
    
    ms->Use++;
    
    return ms;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Miscellaneous Set.
**
** @fdata [EnsPMiscellaneousSet]
** @fnote None
**
** @nam3rule Del Destroy (free) an Miscellaneous Set object
**
** @argrule * Pms [EnsPMiscellaneousSet*] Miscellaneous Set object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMiscellaneousSetDel ***********************************************
**
** Default destructor for an Ensembl Miscellaneous Set.
**
** @param [d] Pms [EnsPMiscellaneousSet*] Ensembl Miscellaneous Set address.
**
** @return [void]
** @@
******************************************************************************/

void ensMiscellaneousSetDel(EnsPMiscellaneousSet *Pms)
{
    EnsPMiscellaneousSet pthis = NULL;
    
    /*
     ajDebug("ensMiscellaneousSetDel\n"
	     "  *Pms %p\n",
	     *Pms);
     
     ensMiscellaneousSetTrace(*Pms, 1);
     */
    
    if(!Pms)
        return;
    
    if(!*Pms)
        return;
    
    (*Pms)->Use--;

    pthis = *Pms;
    
    if(pthis->Use)
    {
	*Pms = NULL;
	
	return;
    }
    
    ajStrDel(&pthis->Code);
    ajStrDel(&pthis->Name);
    ajStrDel(&pthis->Description);
    
    AJFREE(pthis);

    *Pms = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Miscellaneous Set object.
**
** @fdata [EnsPMiscellaneousSet]
** @fnote None
**
** @nam3rule Get Return Ensembl Miscellaneous Set attribute(s)
** @nam4rule Getadaptor Return the Ensembl Miscellaneous Set Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetCode Return the code
** @nam4rule GetName Return the name
** @nam4rule GetDescription Return the description
** @nam4rule GetMaximumLength Return the maximum length
**
** @argrule * ms [const EnsPMiscellaneousSet] Ensembl Miscellaneous Set
**
** @valrule Adaptor [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                                Set Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Code [AjPStr] Code
** @valrule Name [AjPStr] Name
** @valrule Description [AjPStr] Description
** @valrule MaximumLength [ajuint] Maximum length
**
** @fcategory use
******************************************************************************/




/* @func ensMiscellaneousSetGetadaptor ****************************************
**
** Get the Ensembl Miscellaneous Set Adaptor element of an
** Ensembl Miscellaneous Set.
**
** @param [r] ms [const EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
**
** @return [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous Set Adaptor.
** @@
******************************************************************************/

EnsPMiscellaneousSetadaptor ensMiscellaneousSetGetadaptor(
    const EnsPMiscellaneousSet ms)
{
    if(!ms)
        return NULL;
    
    return ms->Adaptor;
}




/* @func ensMiscellaneousSetGetIdentifier *************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Miscellaneous Set.
**
** @param [r] ms [const EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
**
** @return [ajuint] Internal database identifier.
** @@
******************************************************************************/

ajuint ensMiscellaneousSetGetIdentifier(const EnsPMiscellaneousSet ms)
{
    if(!ms)
        return 0;
    
    return ms->Identifier;
}




/* @func ensMiscellaneousSetGetCode *******************************************
**
** Get the code element of an Ensembl Miscellaneous Set.
**
** @param [r] ms [const EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
**
** @return [AjPStr] Code.
** @@
******************************************************************************/

AjPStr ensMiscellaneousSetGetCode(const EnsPMiscellaneousSet ms)
{
    if(!ms)
        return NULL;
    
    return ms->Code;
}




/* @func ensMiscellaneousSetGetName *******************************************
**
** Get the name element of an Ensembl Miscellaneous Set.
**
** @param [r] ms [const EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
**
** @return [AjPStr] Name.
** @@
******************************************************************************/

AjPStr ensMiscellaneousSetGetName(const EnsPMiscellaneousSet ms)
{
    if(!ms)
        return NULL;
    
    return ms->Name;
}




/* @func ensMiscellaneousSetGetDescription ************************************
**
** Get the description element of an Ensembl Miscellaneous Set.
**
** @param [r] ms [const EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
**
** @return [AjPStr] Description.
** @@
******************************************************************************/

AjPStr ensMiscellaneousSetGetDescription(const EnsPMiscellaneousSet ms)
{
    if(!ms)
        return NULL;
    
    return ms->Description;
}




/* @func ensMiscellaneousSetGetMaximumLength **********************************
**
** Get the maximum length element of an Ensembl Miscellaneous Set.
**
** @param [r] ms [const EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
**
** @return [ajuint] Maximum length.
** @@
******************************************************************************/

ajuint ensMiscellaneousSetGetMaximumLength(const EnsPMiscellaneousSet ms)
{
    if(!ms)
        return 0;
    
    return ms->MaximumLength;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl Miscellaneous Set object.
**
** @fdata [EnsPMiscellaneousSet]
** @fnote None
**
** @nam3rule Set Set one element of an Ensembl Miscellaneous Set
** @nam4rule Setadaptor Set the Ensembl Miscellaneous Set Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetCode Set the code
** @nam4rule SetName Set the name
** @nam4rule SetDescription Set the description
** @nam4rule SetMaximumLength Set the maximum length
**
** @argrule * ms [EnsPMiscellaneousSet] Ensembl Miscellaneous Set object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensMiscellaneousSetSetadaptor ****************************************
**
** Set the Ensembl Miscellaneous Set Adaptor element of an
** Ensembl Miscellaneous Set.
**
** @param [u] ms [EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
** @param [r] adaptor [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                              Set Adaptor.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousSetSetadaptor(EnsPMiscellaneousSet ms,
                                     EnsPMiscellaneousSetadaptor adaptor)
{
    if(!ms)
        return ajFalse;
    
    ms->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensMiscellaneousSetSetIdentifier *************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Miscellaneous Set.
**
** @param [u] ms [EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
** @param [r] identifier [ajuint] SQL database-internal identifier.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousSetSetIdentifier(EnsPMiscellaneousSet ms,
                                        ajuint identifier)
{
    if(!ms)
        return ajFalse;
    
    ms->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensMiscellaneousSetSetCode *******************************************
**
** Set the code element of an Ensembl Miscellaneous Set.
**
** @param [u] ms [EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
** @param [u] code [AjPStr] Code.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousSetSetCode(EnsPMiscellaneousSet ms, AjPStr code)
{
    if(!ms)
        return ajFalse;
    
    ajStrDel(&ms->Code);
    
    ms->Code = ajStrNewRef(code);
    
    return ajTrue;
}




/* @func ensMiscellaneousSetSetName *******************************************
**
** Set the name element of an Ensembl Miscellaneous Set.
**
** @param [u] ms [EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
** @param [u] name [AjPStr] Name.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousSetSetName(EnsPMiscellaneousSet ms, AjPStr name)
{
    if(!ms)
        return ajFalse;
    
    ajStrDel(&ms->Name);
    
    ms->Name = ajStrNewRef(name);
    
    return ajTrue;
}




/* @func ensMiscellaneousSetSetDescription ************************************
**
** Set the description element of an Ensembl Miscellaneous Set.
**
** @param [u] ms [EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
** @param [u] description [AjPStr] Description.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousSetSetDescription(EnsPMiscellaneousSet ms,
                                         AjPStr description)
{
    if(!ms)
        return ajFalse;
    
    ajStrDel(&ms->Description);
    
    ms->Description = ajStrNewRef(description);
    
    return ajTrue;
}




/* @func ensMiscellaneousSetSetMaximumLength **********************************
**
** Set the maximum length element of an Ensembl Miscellaneous Set.
**
** @param [u] ms [EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
** @param [r] maxlen [ajuint] Maximum length.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousSetSetMaximumLength(EnsPMiscellaneousSet ms,
                                           ajuint maxlen)
{
    if(!ms)
        return ajFalse;
    
    ms->MaximumLength = maxlen;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Miscellaneous Set object.
**
** @fdata [EnsPMiscellaneousSet]
** @nam3rule Trace Report Ensembl Miscellaneous Set elements to debug file
**
** @argrule Trace ms [const EnsPMiscellaneousSet] Ensembl Miscellaneous Set
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensMiscellaneousSetTrace *********************************************
**
** Trace an Ensembl Miscellaneous Set.
**
** @param [r] ms [const EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
** @param [r] level [ajuint] Indentation level.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousSetTrace(const EnsPMiscellaneousSet ms, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!ms)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensMiscellaneousSetTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Code '%S'\n"
	    "%S  Name '%S'\n"
	    "%S  Description '%S'\n"
	    "%S  MaximumLength %u\n",
	    indent, ms,
	    indent, ms->Use,
	    indent, ms->Identifier,
	    indent, ms->Adaptor,
	    indent, ms->Code,
	    indent, ms->Name,
	    indent, ms->Description,
	    indent, ms->MaximumLength);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensMiscellaneousSetGetMemSize ****************************************
**
** Get the memory size in bytes of an Ensembl Miscellaneous Set.
**
** @param [r] ms [const EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
**
** @return [ajuint] Memory size.
** @@
******************************************************************************/

ajuint ensMiscellaneousSetGetMemSize(const EnsPMiscellaneousSet ms)
{
    ajuint size = 0;
    
    if(!ms)
	return 0;
    
    size += (ajuint) sizeof (EnsOMiscellaneousSet);
    
    if(ms->Code)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(ms->Code);
    }
    
    if(ms->Name)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(ms->Name);
    }
    
    if(ms->Description)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(ms->Description);
    }
    
    return size;
}




/* @datasection [EnsPMiscellaneousSetadaptor] Miscellaneous Set Adaptor *******
**
** Functions for manipulating Ensembl Miscellaneous Set Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::MiscSetadaptor CVS Revision: 1.12
**
** @nam2rule MiscellaneousSetadaptor
**
******************************************************************************/




/* @funcstatic miscellaneousSetadaptorFetchAllBySQL ***************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl Miscellaneous Set objects.
**
** @param [r] msa [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                              Set Adaptor.
** @param [r] statement [const AjPStr] SQL statement.
** @param [u] mslist [AjPList] AJAX List of Ensembl Miscellaneous Sets.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

static AjBool miscellaneousSetadaptorFetchAllBySQL(
    EnsPMiscellaneousSetadaptor msa,
    const AjPStr statement,
    AjPList mslist)
{
    ajuint identifier = 0;
    ajuint maxlen     = 0;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr code        = NULL;
    AjPStr name        = NULL;
    AjPStr description = NULL;
    
    EnsPMiscellaneousSet ms = NULL;
    
    if(!msa)
        return ajFalse;
    
    if(!statement)
        return ajFalse;
    
    if(!mslist)
        return ajFalse;
    
    sqls = ensDatabaseadaptorSqlstatementNew(msa->Adaptor, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier = 0;
	code = ajStrNew();
	name = ajStrNew();
	description = ajStrNew();
	maxlen = 0;
	
        sqlr = ajSqlrowiterGet(sqli);
	
        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToStr(sqlr, &code);
        ajSqlcolumnToStr(sqlr, &name);
        ajSqlcolumnToStr(sqlr, &description);
        ajSqlcolumnToUint(sqlr, &maxlen);
	
        ms = ensMiscellaneousSetNew(msa,
				    identifier,
				    code,
				    name,
				    description,
				    maxlen);
	
        ajListPushAppend(mslist, (void *) ms);
	
	ajStrDel(&code);
	ajStrDel(&name);
	ajStrDel(&description);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    return ajTrue;
}




/* @funcstatic miscellaneousSetadaptorCacheInsert *****************************
**
** Insert an Ensembl Miscellaneous Set into the
** Miscellaneous Set Adaptor-internal cache.
** If a Miscellaneous Set with the same code element is already present in the
** adaptor cache, the Miscellaneous Set is deleted and a pointer to the cached
** Miscellaneous Set is returned.
**
** @param [u] msa [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                              Set Adaptor.
** @param [u] Pms [EnsPMiscellaneousSet*] Ensembl Miscellaneous Set address.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

static AjBool miscellaneousSetadaptorCacheInsert(
    EnsPMiscellaneousSetadaptor msa,
    EnsPMiscellaneousSet *Pms)
{
    ajuint *Pidentifier = NULL;
    
    EnsPMiscellaneousSet ms1 = NULL;
    EnsPMiscellaneousSet ms2 = NULL;
    
    if(!msa)
        return ajFalse;
    
    if(!msa->CacheByIdentifier)
        return ajFalse;
    
    if(!msa->CacheByCode)
        return ajFalse;
    
    if(!Pms)
        return ajFalse;
    
    if(!*Pms)
        return ajFalse;
    
    /* Search the identifer cache. */
    
    ms1 = (EnsPMiscellaneousSet)
	ajTableFetch(msa->CacheByIdentifier,
		     (const void *) &((*Pms)->Identifier));
    
    /* Search the code cache. */
    
    ms2 = (EnsPMiscellaneousSet)
	ajTableFetch(msa->CacheByCode,
		     (const void *) (*Pms)->Code);
    
    if((!ms1) && (! ms2))
    {
	/* Insert into the identifier cache. */
	
	AJNEW0(Pidentifier);
	
	*Pidentifier = (*Pms)->Identifier;
	
	ajTablePut(msa->CacheByIdentifier,
		   (void *) Pidentifier,
		   (void *) ensMiscellaneousSetNewRef(*Pms));
	
	/* Insert into the code cache. */
	
        ajTablePut(msa->CacheByCode,
		   (void *) ajStrNewS((*Pms)->Code),
		   (void *) ensMiscellaneousSetNewRef(*Pms));
    }
    
    if(ms1 && ms2 && (ms1 == ms2))
    {
        ajDebug("miscellaneousSetCacheInsert replaced Miscellaneous Set %p "
		"with one already cached %p.\n", *Pms, ms1);
	
	ensMiscellaneousSetDel(Pms);
	
	Pms = &ms1;
    }
    
    if(ms1 && ms2 && (ms1 != ms2))
	ajDebug("miscellaneousSetCacheInsert detected Miscellaneous Sets in "
		"the identifier and code cache with identical codes "
		"('%S' and '%S') but different addresses (%p and %p).\n",
		ms1->Code, ms2->Code, ms1, ms2);
    
    if(ms1 && (!ms2))
        ajDebug("miscellaneousSetCacheInsert detected an "
		"Ensembl Miscellaneous Set "
		"in the identifier, but not in the code cache.\n");
    
    if((!ms1) && ms2)
        ajDebug("miscellaneousSetCacheInsert detected an "
		"Ensembl Miscellaneous Set "
		"in the code, but not in the identifier cache.\n");
    
    return ajTrue;
}




/* @funcstatic miscellaneousSetadaptorCacheRemove *****************************
**
** Remove an Ensembl Miscellaneous Set from the
** Miscellaneous Set Adaptor-internal cache.
**
** @param [u] msa [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                              Set Adaptor.
** @param [r] ms [EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

#if AJFALSE

static AjBool miscellaneousSetadaptorCacheRemove(
    EnsPMiscellaneousSetadaptor msa,
    EnsPMiscellaneousSet ms)
{
    ajuint *Pidentifier = NULL;
    
    AjPStr key = NULL;
    
    EnsPMiscellaneousSet ms1 = NULL;
    EnsPMiscellaneousSet ms2 = NULL;
    
    if(!msa)
	return ajFalse;
    
    if(!ms)
	return ajFalse;
    
    /* Remove the table nodes. */
    
    ms1 = (EnsPMiscellaneousSet)
	ajTableRemoveKey(msa->CacheByIdentifier,
			 (const void *) &(ms->Identifier),
			 (void **) &Pidentifier);
    
    ms2 = (EnsPMiscellaneousSet)
	ajTableRemoveKey(msa->CacheByCode,
			 (const void *) ms->Code,
			 (void **) &key);
    
    if(ms1 && (!ms2))
	ajWarn("miscellaneousSetadaptorCacheRemove could remove "
	       "Miscellaneous Set '%S' (%u) "
	       "only from the identifier cache.\n",
	       ms->Code, ms->Identifier);
    
    if((!ms1) && ms2)
	ajWarn("miscellaneousSetadaptorCacheRemove could remove "
	       "Miscellaneous Set '%S' (%u) "
	       "only from the code cache.\n",
	       ms->Code, ms->Identifier);
    
    /* Delete the keys. */
    
    AJFREE(Pidentifier);
    
    ajStrDel(&key);
    
    /* Delete (or at least de-reference) the Ensembl Analyses. */
    
    ensMiscellaneousSetDel(&ms1);
    ensMiscellaneousSetDel(&ms2);
    
    return ajTrue;
}

#endif




/* @funcstatic miscellaneousSetadaptorCacheInit *******************************
**
** Initialise the internal Miscellaneous Set cache of an
** Ensembl Miscellaneous Set Adaptor.
**
** @param [u] msa [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                              Set Adaptor.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

static AjBool miscellaneousSetadaptorCacheInit(EnsPMiscellaneousSetadaptor msa)
{
    AjPList mslist   = NULL;
    AjPStr statement = NULL;
    
    EnsPMiscellaneousSet ms = NULL;
    
    if(!msa)
        return ajFalse;
    
    if(msa->CacheByIdentifier)
        return ajFalse;
    else
        msa->CacheByIdentifier =
	    ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
    
    if(msa->CacheByCode)
        return ajFalse;
    else
        msa->CacheByCode = ajTablestrNewLen(0);
    
    statement = ajStrNewC("SELECT "
			  "misc_set.misc_set_id, "
			  "misc_set.code, "
			  "misc_set.name, "
			  "misc_set.description, "
			  "misc_set.max_length "
			  "FROM "
			  "misc_set");
    
    mslist = ajListNew();
    
    miscellaneousSetadaptorFetchAllBySQL(msa, statement, mslist);
    
    while(ajListPop(mslist, (void **) &ms))
    {
        miscellaneousSetadaptorCacheInsert(msa, &ms);
	
	/*
	 ** Both caches hold internal references to the
	 ** Miscellaneous Set objects.
	 */
	
	ensMiscellaneousSetDel(&ms);
    }
    
    ajListFree(&mslist);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Miscellaneous Set Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Miscellaneous Set Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMiscellaneousSetadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                                   Set Adaptor
** @argrule Ref object [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                                   Set Adaptor
**
** @valrule * [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous Set Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensMiscellaneousSetadaptorNew ****************************************
**
** Default constructor for an Ensembl Miscellaneous Set Adaptor.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor.
**
** @return [EnsPMiscellaneousSetadaptor] Ensembl MiscellaneousSet Adaptor
**                                       or NULL.
** @@
******************************************************************************/

EnsPMiscellaneousSetadaptor ensMiscellaneousSetadaptorNew(
    EnsPDatabaseadaptor dba)
{
    EnsPMiscellaneousSetadaptor msa = NULL;
    
    if(!dba)
        return NULL;
    
    AJNEW0(msa);
    
    msa->Adaptor = dba;
    
    miscellaneousSetadaptorCacheInit(msa);
    
    return msa;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Miscellaneous Set Adaptor.
**
** @fdata [EnsPMiscellaneousSetadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Miscellaneous Set Adaptor object
**
** @argrule * Padaptor [EnsPMiscellaneousSetadaptor*] Ensembl Miscellaneous Set
**                                                    Adaptor address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @funcstatic miscellaneousSetadaptorCacheClearIdentifier ********************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Miscellaneous Set
** Adaptor-internal Miscellaneous Set cache. This function deletes the
** unsigned integer identifier key and the Ensembl Miscellaneous Set
** value data.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Miscellaneous Set value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void miscellaneousSetadaptorCacheClearIdentifier(void **key,
                                                        void **value,
                                                        void *cl)
{
    if(!key)
	return;
    
    if(!*key)
	return;
    
    if(!value)
	return;
    
    if(!*value)
	return;
    
    (void) cl;
    
    AJFREE(*key);
    
    ensMiscellaneousSetDel((EnsPMiscellaneousSet *) value);
    
    return;
}




/* @funcstatic miscellaneousSetadaptorCacheClearCode **************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Miscellaneous Set
** Adaptor-internal Miscellaneous Set cache. This function deletes the code
** AJAX String key data and the Ensembl Miscellaneous Set value data.
**
** @param [u] key [void**] AJAX String key data address
** @param [u] value [void**] Ensembl Miscellaneous Set value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void miscellaneousSetadaptorCacheClearCode(void **key,
                                                  void **value,
                                                  void *cl)
{
    if(!key)
	return;
    
    if(!*key)
	return;
    
    if(!value)
	return;
    
    if(!*value)
	return;
    
    (void) cl;
    
    ajStrDel((AjPStr *) key);
    
    ensMiscellaneousSetDel((EnsPMiscellaneousSet *) value);
    
    return;
}




/* @funcstatic miscellaneousSetadaptorCacheExit *******************************
**
** Clears the internal Miscellaneous Set cache of an
** Ensembl Miscellaneous Set Adaptor.
**
** @param [u] adaptor [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                                  Set Adaptor.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

static AjBool miscellaneousSetadaptorCacheExit(
    EnsPMiscellaneousSetadaptor adaptor)
{
    if(!adaptor)
        return ajFalse;
    
    /* Clear and delete the identifier cache. */
    
    ajTableMapDel(adaptor->CacheByIdentifier,
		  miscellaneousSetadaptorCacheClearIdentifier,
		  NULL);
    
    ajTableFree(&(adaptor->CacheByIdentifier));
    
    /* Clear and delete the code cache. */
    
    ajTableMapDel(adaptor->CacheByCode,
		  miscellaneousSetadaptorCacheClearCode,
		  NULL);
    
    ajTableFree(&(adaptor->CacheByCode));
    
    return ajTrue;
}




/* @func ensMiscellaneousSetadaptorDel ****************************************
**
** Default destructor for an Ensembl Miscellaneous Set Adaptor.
** This function also clears the internal caches.
**
** @param [d] Padaptor [EnsPMiscellaneousSetadaptor*] Ensembl Miscellaneous
**                                                    Set Adaptor address.
**
** @return [void]
** @@
******************************************************************************/

void ensMiscellaneousSetadaptorDel(EnsPMiscellaneousSetadaptor *Padaptor)
{
    EnsPMiscellaneousSetadaptor pthis = NULL;
    
    if(!Padaptor)
        return;
    
    if(!*Padaptor)
        return;

    pthis = *Padaptor;
    
    miscellaneousSetadaptorCacheExit(pthis);
    
    AJFREE(pthis);

    *Padaptor = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Miscellaneous Set Adaptor object.
**
** @fdata [EnsPMiscellaneousSetadaptor]
** @fnote None
**
** @nam3rule Get Return Ensembl Miscellaneous Set Adaptor attribute(s)
** @nam4rule Getadaptor Return the Ensembl Database Adaptor
**
** @argrule * adaptor [const EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                                        Set Adaptor
**
** @valrule Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @fcategory use
******************************************************************************/




/* @func ensMiscellaneousSetadaptorGetDatabaseadaptor *************************
**
** Get the Ensembl Database Adaptor element of an
** Ensembl Miscellaneous Set Adaptor.
**
** @param [r] adaptor [const EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                                        Set Adaptor.
**
** @return [const EnsPDatabaseadaptor] Ensembl Database Adaptor or NULL.
** @@
******************************************************************************/

const EnsPDatabaseadaptor ensMiscellaneousSetadaptorGetDatabaseadaptor(
    const EnsPMiscellaneousSetadaptor adaptor)
{
    if(!adaptor)
        return NULL;
    
    return adaptor->Adaptor;
}




/* @section object fetching ***************************************************
**
** Functions for retrieving Ensembl Miscellaneous Set objects from an
** Ensembl Core database.
**
** @fdata [EnsPMiscellaneousSetadaptor]
** @fnote None
**
** @nam3rule Fetch Retrieve Ensembl Miscellaneous Set object(s)
** @nam4rule FetchAll Retrieve all Ensembl Miscellaneous Set objects.
** @nam5rule FetchAllBy Retrieve all Ensembl Miscellaneous Set objects
**                      matching a criterion.
** @nam4rule FetchBy Retrieve one Ensembl Miscellaneous Set object
**                   matching a criterion.
**
** @argrule * adaptor [const EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                                        Set Adaptor
** @argrule FetchAll [AjPList] AJAX List of Ensembl Miscellaneous Set objects
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise.
**
** @fcategory use
******************************************************************************/




/* @funcstatic miscellaneousSetadaptorFetchAll ********************************
**
** An ajTableMap 'apply' function to return all Miscellaneous Set objects
** from the Ensembl Miscellaneous Set Adaptor-internal cache.
**
** @param [u] key [const void *] Pointer to unsigned integer identifier
**                               key data.
** @param [u] value [void**] Pointer to Ensembl Miscellaneous Set value data.
** @param [u] cl [void*] AJAX List of Ensembl Miscellaneous Set objects,
**                       passed in via ajTableMap
** @see ajTableMap
**
** @return [void]
** @@
******************************************************************************/

static void miscellaneousSetadaptorFetchAll(const void *key, void **value,
                                            void *cl)
{
    EnsPMiscellaneousSet *Pms = NULL;
    
    if(!key)
	return;
    
    if(!value)
	return;
    
    if(!*value)
	return;
    
    if(!cl)
	return;
    
    Pms = (EnsPMiscellaneousSet *) value;
    
    ajListPushAppend((AjPList) cl, (void *) ensMiscellaneousSetNewRef(*Pms));
    
    return;
}




/* @func ensMiscellaneousSetadaptorFetchAll ***********************************
**
** Fetch all Ensembl Miscellaneous Sets.
**
** @param [r] adaptor [const EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                                        Set Adaptor.
** @param [u] mslist [AjPList] AJAX List of Ensembl Miscellaneous Sets.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousSetadaptorFetchAll(
    const EnsPMiscellaneousSetadaptor adaptor,
    AjPList mslist)
{
    if(!adaptor)
        return ajFalse;
    
    if(!mslist)
        return ajFalse;
    
    ajTableMap(adaptor->CacheByIdentifier,
	       miscellaneousSetadaptorFetchAll,
	       (void *) mslist);
    
    return ajTrue;
}




/* @func ensMiscellaneousSetadaptorFetchByIdentifier **************************
**
** Fetch an Ensembl Miscellaneous Set by its SQL database-internal identifier.
**
** @param [r] adaptor [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                                  Set Adaptor.
** @param [r] identifier [ajuint] SQL database-internal identifier.
** @param [wP] Pms [EnsPMiscellaneousSet*] Ensembl Miscellaneous Set address.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousSetadaptorFetchByIdentifier(
    EnsPMiscellaneousSetadaptor adaptor,
    ajuint identifier,
    EnsPMiscellaneousSet *Pms)
{
    AjPList mslist = NULL;
    
    AjPStr statement = NULL;
    
    EnsPMiscellaneousSet ms = NULL;
    
    if(!adaptor)
        return ajFalse;
    
    if(!identifier)
        return ajFalse;
    
    if(!Pms)
	return ajFalse;
    
    /*
    ** Initally, search the identifier cache.
    ** For any object returned by the AJAX Table the reference counter needs
    ** to be incremented manually.
    */
    
    *Pms = (EnsPMiscellaneousSet)
	ajTableFetch(adaptor->CacheByIdentifier, (const void *) &identifier);
    
    if(*Pms)
    {
	ensMiscellaneousSetNewRef(*Pms);
	
	return ajTrue;
    }
    
    /* For a cache miss query the database. */
    
    statement = ajFmtStr("SELECT "
			 "misc_set.misc_set_id, "
			 "misc_set.code, "
			 "misc_set.name, "
			 "misc_set.description, "
			 "misc_set.max_length "
			 "FROM "
			 "misc_set"
			 "WHERE "
			 "misc_set.misc_set_id = %u",
			 identifier);
    
    mslist = ajListNew();
    
    miscellaneousSetadaptorFetchAllBySQL(adaptor, statement, mslist);
    
    if(ajListGetLength(mslist) > 1)
	ajWarn("ensMiscellaneousSetadaptorFetchByIdentifier got more "
	       "than one Ensembl Miscellaneous Set for(PRIMARY KEY) "
	       "identifier %u.\n", identifier);
    
    ajListPop(mslist, (void **) Pms);
    
    miscellaneousSetadaptorCacheInsert(adaptor, Pms);
    
    while(ajListPop(mslist, (void **) &ms))
    {
	miscellaneousSetadaptorCacheInsert(adaptor, &ms);
	
	ensMiscellaneousSetDel(&ms);
    }
    
    ajListFree(&mslist);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensMiscellaneousSetadaptorFetchByCode ********************************
**
** Fetch an Ensembl Miscellaneous Set by its code.
**
** @param [r] adaptor [EnsPMiscellaneousSetadaptor] Ensembl Miscellaneous
**                                                  Set Adaptor.
** @param [r] code [const AjPStr] Ensembl Miscellaneous Set code.
** @param [wP] Pms [EnsPMiscellaneousSet*] Ensembl Miscellaneous Set address.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousSetadaptorFetchByCode(
    EnsPMiscellaneousSetadaptor adaptor,
    const AjPStr code,
    EnsPMiscellaneousSet *Pms)
{
    char *txtcode = NULL;
    
    AjPList mslist = NULL;
    
    AjPStr statement = NULL;
    
    EnsPMiscellaneousSet ms = NULL;
    
    if(!adaptor)
        return ajFalse;
    
    if((!code) && (! ajStrGetLen(code)))
        return ajFalse;
    
    if(!Pms)
	return ajFalse;
    
    /*
    ** Initally, search the code cache.
    ** For any object returned by the AJAX Table the reference counter needs
    ** to be incremented manually.
    */
    
    *Pms = (EnsPMiscellaneousSet)
	ajTableFetch(adaptor->CacheByCode, (const void *) code);
    
    if(*Pms)
    {
	ensMiscellaneousSetNewRef(*Pms);
	
	return ajTrue;
    }
    
    /* In case of a cache miss, query the database. */
    
    ensDatabaseadaptorEscapeC(adaptor->Adaptor, &txtcode, code);
    
    statement = ajFmtStr("SELECT "
			 "misc_set.misc_set_id, "
			 "misc_set.code, "
			 "misc_set.name, "
			 "misc_set.description, "
			 "misc_set.max_length "
			 "FROM "
			 "misc_set"
			 "WHERE "
			 "misc_set.code = '%s'",
			 txtcode);
    
    ajCharDel(&txtcode);
    
    mslist = ajListNew();
    
    miscellaneousSetadaptorFetchAllBySQL(adaptor, statement, mslist);
    
    if(ajListGetLength(mslist) > 1)
	ajWarn("ensMiscellaneousSetadaptorFetchByCode got more than one "
	       "Ensembl Miscellaneous Sets for(UNIQUE) code '%S'.\n",
	       code);
    
    ajListPop(mslist, (void **) Pms);
    
    miscellaneousSetadaptorCacheInsert(adaptor, Pms);
    
    while(ajListPop(mslist, (void **) &ms))
    {
	miscellaneousSetadaptorCacheInsert(adaptor, &ms);
	
	ensMiscellaneousSetDel(&ms);
    }
    
    ajListFree(&mslist);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @datasection [EnsPMiscellaneousFeature] Miscellaneous Feature **************
**
** Functions for manipulating Ensembl Miscellaneous Feature objects
**
** @cc Bio::EnsEMBL::MiscFeature CVS Revision: 1.13
**
** @nam2rule MiscellaneousFeature
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Miscellaneous Feature by pointer.
** It is the responsibility of the user to first destroy any previous
** Miscellaneous Feature. The target pointer does not need to be initialised
** to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMiscellaneousFeature]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature
** @argrule Ref object [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature
**
** @valrule * [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature
**
** @fcategory new
******************************************************************************/




/* @func ensMiscellaneousFeatureNew *******************************************
**
** Default constructor for an Ensembl Miscellaneous Feature.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPMiscellaneousFeatureadaptor] Ensembl Miscellaneous
**                                                      Feature Adaptor.
** @param [r] identifier [ajuint] SQL database-internal identifier.
** @cc Bio::EnsEMBL::Feature::new
** @param [u] feature [EnsPFeature] Ensembl Feature.
** @cc Bio::EnsEMBL::MiscFeature::new
**
** @return [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature or NULL.
** @@
******************************************************************************/

EnsPMiscellaneousFeature ensMiscellaneousFeatureNew(
    EnsPMiscellaneousFeatureadaptor adaptor,
    ajuint identifier,
    EnsPFeature feature)
{
    EnsPMiscellaneousFeature mf = NULL;
    
    if(!feature)
	return NULL;
    
    AJNEW0(mf);
    
    mf->Use = 1;
    
    mf->Identifier = identifier;
    
    mf->Adaptor = adaptor;
    
    mf->Feature = ensFeatureNewRef(feature);
    
    mf->Attributes = ajListNew();
    
    mf->MiscellaneousSets = ajListNew();
    
    return mf;
}




/* @func ensMiscellaneousFeatureNewObj ****************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPMiscellaneousFeature] Ensembl Miscellaneous
**                                                    Feature.
**
** @return [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature or NULL.
** @@
******************************************************************************/

EnsPMiscellaneousFeature ensMiscellaneousFeatureNewObj(
    const EnsPMiscellaneousFeature object)
{
    AjIList iter = NULL;
    
    EnsPAttribute attribute = NULL;
    
    EnsPMiscellaneousFeature mf = NULL;
    
    EnsPMiscellaneousSet ms = NULL;
    
    AJNEW0(mf);
    
    mf->Use = 1;
    
    mf->Identifier = object->Identifier;
    
    mf->Adaptor = object->Adaptor;
    
    mf->Feature = ensFeatureNewRef(object->Feature);
    
    /* Copy the AJAX List of Ensembl Attributes. */
    
    mf->Attributes = ajListNew();
    
    iter = ajListIterNew(object->Attributes);
    
    while(!ajListIterDone(iter))
    {
	attribute = (EnsPAttribute) ajListIterGet(iter);
	
	ajListPushAppend(mf->Attributes,
			 (void *) ensAttributeNewRef(attribute));
    }
    
    ajListIterDel(&iter);
    
    /* Copy the AJAX List of Ensembl Miscellaneous Sets. */
    
    mf->MiscellaneousSets = ajListNew();
    
    iter = ajListIterNew(object->MiscellaneousSets);
    
    while(!ajListIterDone(iter))
    {
	ms = (EnsPMiscellaneousSet) ajListIterGet(iter);
	
	ajListPushAppend(mf->MiscellaneousSets,
			 (void *) ensMiscellaneousSetNewRef(ms));
    }
    
    ajListIterDel(&iter);
    
    return mf;
}




/* @func ensMiscellaneousFeatureNewRef ****************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] mf [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature.
**
** @return [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature or NULL.
** @@
******************************************************************************/

EnsPMiscellaneousFeature ensMiscellaneousFeatureNewRef(
    EnsPMiscellaneousFeature mf)
{
    if(!mf)
	return NULL;
    
    mf->Use++;
    
    return mf;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Miscellaneous Features.
**
** @fdata [EnsPMiscellaneousFeature]
** @fnote None
**
** @nam3rule Del Destroy (free) a Miscellaneous Feature object
**
** @argrule * Pmf [EnsPMiscellaneousFeature*] Miscellaneous Feature address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMiscellaneousFeatureDel *******************************************
**
** Default destructor for an Ensembl Miscellaneous Feature.
**
** @param [d] Pmf [EnsPMiscellaneousFeature*] Ensembl Miscellaneous
**                                            Feature address.
**
** @return [void]
** @@
******************************************************************************/

void ensMiscellaneousFeatureDel(EnsPMiscellaneousFeature *Pmf)
{
    EnsPAttribute attribute = NULL;
    
    EnsPMiscellaneousSet ms = NULL;

    EnsPMiscellaneousFeature pthis = NULL;
    
    
    if(!Pmf)
        return;
    
    if(!*Pmf)
        return;
    
    (*Pmf)->Use--;

    pthis = *Pmf;
    
    if(pthis->Use)
    {
	*Pmf = NULL;
	
	return;
    }
    
    ensFeatureDel(&pthis->Feature);
    
    /* Clear and free the AJAX List of Ensembl Attributes. */
    
    while(ajListPop(pthis->Attributes, (void **) &attribute))
	ensAttributeDel(&attribute);
    
    ajListFree(&pthis->Attributes);
    
    /* Clear and free the AJAX List of Ensembl Miscellaneous Sets. */
    
    while(ajListPop(pthis->MiscellaneousSets, (void **) &ms))
	ensMiscellaneousSetDel(&ms);
    
    ajListFree(&pthis->MiscellaneousSets);
    
    AJFREE(pthis);

    *Pmf = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Miscellaneous Feature object.
**
** @fdata [EnsPMiscellaneousFeature]
** @fnote None
**
** @nam3rule Get Return Miscellaneous Feature attribute(s)
** @nam4rule Getadaptor Return the Ensembl Miscellaneous Feature Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetFeature Return the Ensembl Feature
** @nam4rule GetAttributes Return all Ensembl Attributes
** @nam4rule GetMiscellaneousSets Return all Ensembl Miscellaneous Sets
**
** @argrule * mf [const EnsPMiscellaneousFeature] Miscellaneous Feature
**
** @valrule Adaptor [EnsPMiscellaneousFeatureadaptor] Ensembl Miscellaneous
**                                                    Feature Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Feature [EnsPFeature] Ensembl Feature
** @valrule Attributes [const AjPList] AJAX List of Ensembl Attributes
** @valrule MiscellaneousSets [const AjPList] AJAX List of Ensembl
**                                            Miscellaneous Sets
**
** @fcategory use
******************************************************************************/




/* @func ensMiscellaneousFeatureGetadaptor ************************************
**
** Get the Ensembl Miscellaneous Feature Adaptor element of an
** Ensembl Miscellaneous Feature.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] mf [const EnsPMiscellaneousFeature] Ensembl Miscellaneous
**                                                Feature.
**
** @return [EnsPMiscellaneousFeatureadaptor] Ensembl Miscellaneous
**                                           Feature Adaptor.
** @@
******************************************************************************/

EnsPMiscellaneousFeatureadaptor ensMiscellaneousFeatureGetadaptor(
    const EnsPMiscellaneousFeature mf)
{
    if(!mf)
        return NULL;
    
    return mf->Adaptor;
}




/* @func ensMiscellaneousFeatureGetIdentifier *********************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Miscellaneous Feature.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] mf [const EnsPMiscellaneousFeature] Ensembl Miscellaneous
**                                                Feature.
**
** @return [ajuint] Internal database identifier.
** @@
******************************************************************************/

ajuint ensMiscellaneousFeatureGetIdentifier(const EnsPMiscellaneousFeature mf)
{
    if(!mf)
        return 0;
    
    return mf->Identifier;
}




/* @func ensMiscellaneousFeatureGetFeature ************************************
**
** Get the Ensembl Feature element of an Ensembl Miscellaneous Feature.
**
** @param [r] mf [const EnsPMiscellaneousFeature] Ensembl Miscellaneous
**                                                Feature.
**
** @return [EnsPFeature] Ensembl Feature.
** @@
******************************************************************************/

EnsPFeature ensMiscellaneousFeatureGetFeature(const EnsPMiscellaneousFeature mf)
{
    if(!mf)
        return NULL;
    
    return mf->Feature;
}




/* @func ensMiscellaneousFeatureGetAttributes *********************************
**
** Get all Ensembl Attributes of an Ensembl Miscellaneous Feature.
**
** @param [r] mf [const EnsPMiscellaneousFeature] Ensembl Miscellaneous
**                                                Feature.
**
** @return [const AjPList] AJAX List of Ensembl Attributes.
** @@
******************************************************************************/

const AjPList ensMiscellaneousFeatureGetAttributes(
    const EnsPMiscellaneousFeature mf)
{
    if(!mf)
        return NULL;
    
    return mf->Attributes;
}




/* @func ensMiscellaneousFeatureGetMiscellaneousSets **************************
**
** Get all Ensembl Miscellaneous Sets of an Ensembl Miscellaneous Feature.
**
** @param [r] mf [const EnsPMiscellaneousFeature] Ensembl Miscellaneous
**                                                Feature.
**
** @return [const AjPList] AJAX List of Ensembl Miscellaneous Sets.
** @@
******************************************************************************/

const AjPList ensMiscellaneousFeatureGetMiscellaneousSets(
    const EnsPMiscellaneousFeature mf)
{
    if(!mf)
        return NULL;
    
    return mf->MiscellaneousSets;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl Miscellaneous Feature object.
**
** @fdata [EnsPMiscellaneousFeature]
** @fnote None
**
** @nam3rule Set Set one element of a Miscellaneous Feature
** @nam4rule Setadaptor Set the Ensembl Miscellaneous Feature Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetFeature Set the Ensembl Feature
** @nam4rule AddAttribute Add an Ensembl Attribute
** @nam4rule AddMiscellaneousSet Add an Ensembl Miscellaneous Set
**
** @argrule * mf [EnsPMiscellaneousFeature] Ensembl Miscellaneous
**                                          Feature object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensMiscellaneousFeatureSetadaptor ************************************
**
** Set the Ensembl Miscellaneous Feature Adaptor element of an
** Ensembl Miscellaneous Feature.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] mf [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature.
** @param [r] adaptor [EnsPMiscellaneousFeatureadaptor] Ensembl Miscellaneous
**                                                      Feature Adaptor.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousFeatureSetadaptor(
    EnsPMiscellaneousFeature mf,
    EnsPMiscellaneousFeatureadaptor adaptor)
{
    if(!mf)
        return ajFalse;
    
    mf->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensMiscellaneousFeatureSetIdentifier *********************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Miscellaneous Feature.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] mf [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature.
** @param [r] identifier [ajuint] SQL database-internal identifier.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousFeatureSetIdentifier(EnsPMiscellaneousFeature mf,
                                            ajuint identifier)
{
    if(!mf)
        return ajFalse;
    
    mf->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensMiscellaneousFeatureSetFeature ************************************
**
** Set the Ensembl Feature element of an Ensembl Miscellaneous Feature.
**
** @param [u] mf [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature.
** @param [u] feature [EnsPFeature] Ensembl Feature.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousFeatureSetFeature(EnsPMiscellaneousFeature mf,
                                         EnsPFeature feature)
{
    if(!mf)
        return ajFalse;
    
    ensFeatureDel(&(mf->Feature));
    
    mf->Feature = ensFeatureNewRef(feature);
    
    return ajTrue;
}




/* @func ensMiscellaneousFeatureAddAttribute **********************************
**
** Add an Ensembl Attribute to an Ensembl Miscellaneous Feature.
**
** @cc Bio::EnsEMBL::MiscellaneousFeature::add_Attribute
** @param [u] mf [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature.
** @param [u] attribute [EnsPAttribute] Ensembl Attribute.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousFeatureAddAttribute(EnsPMiscellaneousFeature mf,
                                           EnsPAttribute attribute)
{
    if(!mf)
        return ajFalse;
    
    if(!attribute)
	return ajFalse;
    
    ajListPushAppend(mf->Attributes, (void *) ensAttributeNewRef(attribute));
    
    return ajTrue;
}




/* @func ensMiscellaneousFeatureAddMiscellaneousSet ***************************
**
** Add an Ensembl Miscellaneous Set to an Ensembl Miscellaneous Feature.
**
** @cc Bio::EnsEMBL::MiscellaneousFeature::add_MiscSet
** @param [u] mf [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature.
** @param [u] ms [EnsPMiscellaneousSet] Ensembl Miscellaneous Set.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousFeatureAddMiscellaneousSet(EnsPMiscellaneousFeature mf,
                                                  EnsPMiscellaneousSet ms)
{
    if(!mf)
        return ajFalse;
    
    if(!ms)
	return ajFalse;
    
    ajListPushAppend(mf->MiscellaneousSets,
		     (void *) ensMiscellaneousSetNewRef(ms));
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Miscellaneous Feature object.
**
** @fdata [EnsPMiscellaneousFeature]
** @nam3rule Trace Report Ensembl Miscellaneous Feature elements to debug file
**
** @argrule Trace mf [const EnsPMiscellaneousFeature] Ensembl Miscellaneous
**                                                    Feature
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensMiscellaneousFeatureTrace *****************************************
**
** Trace an Ensembl Miscellaneous Feature.
**
** @param [r] mf [const EnsPMiscellaneousFeature] Ensembl Miscellaneous
**                                                Feature.
** @param [r] level [ajuint] Indentation level.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousFeatureTrace(const EnsPMiscellaneousFeature mf,
                                    ajuint level)
{
    AjPStr indent = NULL;
    
    if(!mf)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensMiscellaneousFeatureTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Feature %p\n"
	    "%S  Attributes %p\n"
	    "%S  MiscellaneousSets %p\n",
	    indent, mf,
	    indent, mf->Use,
	    indent, mf->Identifier,
	    indent, mf->Adaptor,
	    indent, mf->Feature,
	    indent, mf->Attributes,
	    indent, mf->MiscellaneousSets);
    
    ensFeatureTrace(mf->Feature, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensMiscellaneousFeatureGetMemSize ************************************
**
** Get the memory size in bytes of an Ensembl Miscellaneous Feature.
**
** @param [r] mf [const EnsPMiscellaneousFeature] Ensembl Miscellaneous
**                                                Feature.
**
** @return [ajuint] Memory size.
** @@
******************************************************************************/

ajuint ensMiscellaneousFeatureGetMemSize(const EnsPMiscellaneousFeature mf)
{
    ajuint size = 0;
    
    AjIList iter = NULL;
    
    EnsPAttribute attribute = NULL;
    
    EnsPMiscellaneousSet ms = NULL;
    
    if(!mf)
	return 0;
    
    size += (ajuint) sizeof (EnsOMiscellaneousFeature);
    
    size += ensFeatureGetMemSize(mf->Feature);
    
    if(mf->Attributes)
    {
	iter = ajListIterNewread(mf->Attributes);
	
	while(!ajListIterDone(iter))
	{
	    attribute = (EnsPAttribute) ajListIterGet(iter);
	    
	    size += ensAttributeGetMemSize(attribute);
	}
	
	ajListIterDel(&iter);
    }
    
    if(mf->MiscellaneousSets)
    {
	iter = ajListIterNewread(mf->MiscellaneousSets);
	
	while(!ajListIterDone(iter))
	{
	    ms = (EnsPMiscellaneousSet) ajListIterGet(iter);
	    
	    size += ensMiscellaneousSetGetMemSize(ms);
	}
	
	ajListIterDel(&iter);
    }
    
    return size;
}




/* @func ensMiscellaneousFeatureFetchAllAttributes ****************************
**
** Fetch all Ensembl Attributes of an Ensembl Miscellaneous Feature.
**
** @cc Bio::EnsEMBL::MiscFeature::get_all_Attributes
** @param [u] mf [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature.
** @param [r] code [const AjPStr] Ensembl Attribute code.
** @param [r] attributes [AjPList] AJAX List of Ensembl Attributes.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
** The caller is responsible for deleting the Ensembl Attributes from the
** AJAX List before deleting the List.
******************************************************************************/

AjBool ensMiscellaneousFeatureFetchAllAttributes(EnsPMiscellaneousFeature mf,
                                                 const AjPStr code,
                                                 AjPList attributes)
{
    AjIList iter = NULL;
    
    EnsPAttribute attribute = NULL;
    
    if(!mf)
	return ajFalse;
    
    if(!attributes)
	return ajFalse;
    
    iter = ajListIterNewread(mf->Attributes);
    
    while(!ajListIterDone(iter))
    {
	attribute = (EnsPAttribute) ajListIterGet(iter);
	
	if(code && ajStrGetLen(code))
	{
	    if(ajStrMatchCaseS(code, ensAttributeGetCode(attribute)))
		ajListPushAppend(attributes,
				 (void *) ensAttributeNewRef(attribute));
	}
	else
	    ajListPushAppend(attributes,
			     (void *) ensAttributeNewRef(attribute));
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}




/* @func ensMiscellaneousFeatureFetchAllMiscellaneousSets *********************
**
** Fetch all Ensembl Miscellaneous Sets of an Ensembl Miscellaneous Feature.
**
** @cc Bio::EnsEMBL::MiscFeature::get_all_MiscSets
** @param [u] mf [EnsPMiscellaneousFeature] Ensembl Miscellaneous Feature.
** @param [r] code [const AjPStr] Ensembl Miscellaneous Set code.
** @param [r] mslist [AjPList] AJAX List of Ensembl Miscellanaeous Sets.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
** The caller is responsible for deleting the Ensembl Miscellaneous Sets from
** the AJAX List before deleting the List.
******************************************************************************/

AjBool ensMiscellaneousFeatureFetchAllMiscellaneousSets(
    EnsPMiscellaneousFeature mf,
    const AjPStr code,
    AjPList mslist)
{
    AjIList iter = NULL;
    
    EnsPMiscellaneousSet ms = NULL;
    
    if(!mf)
	return ajFalse;
    
    if(!mslist)
	return ajFalse;
    
    iter = ajListIterNewread(mf->MiscellaneousSets);
    
    while(!ajListIterDone(iter))
    {
	ms = (EnsPMiscellaneousSet) ajListIterGet(iter);
	
	if(code && ajStrGetLen(code))
	{
	    if(ajStrMatchCaseS(code, ensMiscellaneousSetGetCode(ms)))
		ajListPushAppend(mslist,
				 (void *) ensMiscellaneousSetNewRef(ms));
	}
	else
	    ajListPushAppend(mslist, (void *) ensMiscellaneousSetNewRef(ms));
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}




/* @datasection [EnsPMiscellaneousFeatureadaptor] Miscellaneous Feature Adaptor
**
** Functions for manipulating Ensembl Miscellaneous Feature Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::MiscFeatureadaptor CVS Revision: 1.22
**
** @nam2rule MiscellaneousFeatureadaptor
**
******************************************************************************/

static const char *miscellaneousFeatureadaptorTables[] =
{
    "misc_feature",
    "misc_feature_misc_set",
    "misc_attrib",
    "attrib_type",
    NULL
};




static const char *miscellaneousFeatureadaptorColumns[] =
{
    "misc_feature.misc_feature_id",
    "misc_feature.seq_region_id",
    "misc_feature.seq_region_start",
    "misc_feature.seq_region_end",
    "misc_feature.seq_region_strand",
    "misc_attrib.value",
    "attrib_type.code",
    "attrib_type.name",
    "attrib_type.description",
    "misc_feature_misc_set.misc_set_id",
    NULL
};




static EnsOBaseadaptorLeftJoin miscellaneousFeatureadaptorLeftJoin[] =
{
    {
	"misc_feature_misc_set",
	"misc_feature.misc_feature_id = misc_feature_misc_set.misc_feature_id"
    },
    {
	"misc_attrib",
	"misc_feature.misc_feature_id = misc_attrib.misc_feature_id"
    },
    {
	"attrib_type",
	"misc_attrib.attrib_type_id = attrib_type.attrib_type_id"
    },
    {NULL, NULL}
};




static const char *miscellaneousFeatureadaptorDefaultCondition = NULL;

static const char *miscellaneousFeatureadaptorFinalCondition =
" ORDER BY misc_feature.misc_feature_id";




/* @funcstatic miscellaneousFeatureadaptorHasAttribute ************************
**
** Check whether an Ensembl Attribute has already been indexed for a particular
** Ensembl Miscellaneous Feature.
**
** @param [u] attributes [AjPTable] Attribute Table.
** @param [r] code [const AjPStr] Ensembl Attribute code.
** @param [r] value [const AjPStr] Ensembl Attribute value.
**
** @return [AjBool] ajTrue: A particular code:value String has already
**                          been indexed.
**                  ajFalse: A particular code:value String has not been
**                           indexed before, but has been added now.
** @@
** This function keeps an AJAX Table of Attribute code:value String key
** data and AjBool value data.
******************************************************************************/

static AjBool miscellaneousFeatureadaptorHasAttribute(AjPTable attributes,
                                                      const AjPStr code,
                                                      const AjPStr value)
{
    AjBool *Pbool = NULL;
    
    AjPStr key = NULL;
    
    key = ajFmtStr("%S:$S", code, value);
    
    if(ajTableFetch(attributes, (const void *) key))
    {
	ajStrDel(&key);
	
	return ajTrue;
    }
    else
    {
	AJNEW0(Pbool);
	
	*Pbool = ajTrue;
	
	ajTablePut(attributes, (void *) key, (void *) Pbool);
	
	return ajFalse;
    }
}




/* @funcstatic miscellaneousFeatureadaptorClearAttributes *********************
**
** Clear an Ensembl Miscellaneous Feature Adaptor-internal
** Ensembl Attribute cache.
**
** This function clears Attribute code:value AJAX String key and
** AJAX Boolean value data from the AJAX Table.
**
** @param [u] attributes [AjPPTable] Attribute cache Table.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

static AjBool miscellaneousFeatureadaptorClearAttributes(AjPTable attributes)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    if(!attributes)
	return ajFalse;
    
    ajTableToarray(attributes, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	/* Delete the AJAX String key data. */
	
	ajStrDel((AjPStr *) &keyarray[i]);
	
	/* Delete the AjBool value data. */
	
	AJFREE(valarray[i]);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    return ajTrue;
}




/* @funcstatic miscellaneousFeatureadaptorHasMiscellaneousSet *****************
**
** Check whether an Ensembl Miscellaneous Set has already been indexed for a
** particular Ensembl Miscellaneous Feature.
**
** @param [u] sets [AjPTable] Miscellaneous Sets Table.
** @param [r] msid [ajuint] Ensembl Miscellaneous Set identifier.
**
** @return [AjBool] ajTrue: A particular Miscellaneous Set identifier
**                          has already been indexed.
**                  ajFalse: A particular Miscellaneous Set identifier has not
**                           been indexed before, but has been added now.
** @@
** This function keeps an AJAX Table of Miscellaneous Set identifier unsigned
** integer key data and AjBool value data.
******************************************************************************/

static AjBool miscellaneousFeatureadaptorHasMiscellaneousSet(AjPTable sets,
                                                             ajuint msid)
{
    AjBool *Pbool = NULL;
    
    ajuint *Pidentifier = NULL;
    
    if(ajTableFetch(sets, (const void *) &msid))
	return ajTrue;
    else
    {
	AJNEW0(Pidentifier);
	
	*Pidentifier = msid;
	
	AJNEW0(Pbool);
	
	*Pbool = ajTrue;
	
	ajTablePut(sets, (void *) Pidentifier, (void *) Pbool);
	
	return ajFalse;
    }
}




/* @funcstatic miscellaneousFeatureadaptorClearMiscellaneousSets **************
**
** Clear an Ensembl Miscellaneous Feature Adaptor-internal
** Ensembl Miscellaneous Set cache.
**
** This function clears Miscellaneous Set identifier unsigned integer key and
** AJAX Boolean value data from the AJAX Table.
**
** @param [u] sets [AjPPTable] Miscellaneous Sets cache Table.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

static AjBool miscellaneousFeatureadaptorClearMiscellaneousSets(AjPTable sets)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    if(!sets)
	return ajFalse;
    
    ajTableToarray(sets, &keyarray, &valarray);
    
    for(i = 0; keyarray[i]; i++)
    {
	/* Delete the unsigned integer key data. */
	
	AJFREE(keyarray[i]);
	
	/* Delete the AjBool value data. */
	
	AJFREE(valarray[i]);
    }
    
    AJFREE(keyarray);
    AJFREE(valarray);
    
    return ajTrue;
}




/* @funcstatic miscellaneousFeatureadaptorFetchAllBySQL ***********************
**
** Fetch all Ensembl Miscellaneous Feature objects via an SQL statement.
**
** @cc Bio::EnsEMBL::DBSQL::MiscellaneousFeatureadaptor::_objs_from_sth
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor.
** @param [r] statement [const AjPStr] SQL statement.
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper.
** @param [r] slice [EnsPSlice] Ensembl Slice.
** @param [u] mflist [AjPList] AJAX List of Ensembl Miscellaneous Feature
**                             objects.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

static AjBool miscellaneousFeatureadaptorFetchAllBySQL(
    EnsPDatabaseadaptor dba,
    const AjPStr statement,
    EnsPAssemblyMapper am,
    EnsPSlice slice,
    AjPList mflist)
{
    ajuint identifier = 0;
    ajuint msid       = 0;
    
    ajuint srid    = 0;
    ajuint srstart = 0;
    ajuint srend   = 0;
    ajint srstrand = 0;
    
    ajint slstart  = 0;
    ajint slend    = 0;
    ajint slstrand = 0;
    ajint sllength = 0;
    
    ajuint current = 0;
    ajuint throw = 0;
    
    AjPList mrs = NULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr avalue = NULL;
    AjPStr atcode = NULL;
    AjPStr atname = NULL;
    AjPStr atdescription = NULL;
    
    AjPTable attributes = NULL;
    AjPTable sets       = NULL;
    
    EnsPAssemblyMapperadaptor ama = NULL;
    
    EnsPAttribute attribute = NULL;
    
    EnsPCoordSystemadaptor csa = NULL;
    
    EnsPFeature feature = NULL;
    
    EnsPMiscellaneousFeature mf         = NULL;
    EnsPMiscellaneousFeatureadaptor mfa = NULL;
    
    EnsPMiscellaneousSet ms         = NULL;
    EnsPMiscellaneousSetadaptor msa = NULL;
    
    EnsPMapperResult mr = NULL;
    
    EnsPSlice srslice   = NULL;
    EnsPSliceadaptor sa = NULL;
    
    ajDebug("miscellaneousFeatureadaptorFetchAllBySQL\n"
	    "  dba %p\n"
	    "  statement %p\n"
	    "  am %p\n"
	    "  slice %p\n"
	    "  mflist %p\n",
	    dba,
	    statement,
	    am,
	    slice,
	    mflist);
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    if(!mflist)
	return ajFalse;
    
    csa = ensRegistryGetCoordSystemadaptor(dba);
    
    mfa = ensRegistryGetMiscellaneousFeatureadaptor(dba);
    
    msa = ensRegistryGetMiscellaneousSetadaptor(dba);
    
    sa = ensRegistryGetSliceadaptor(dba);
    
    if(slice)
	ama = ensRegistryGetAssemblyMapperadaptor(dba);
    
    attributes = MENSTABLEUINTNEW(0);
    
    sets = ajTablestrNewLen(0);
    
    mrs = ajListNew();
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier    = 0;
	srid          = 0;
	srstart       = 0;
	srend         = 0;
	srstrand      = 0;
	avalue        = ajStrNew();
	atcode        = ajStrNew();
	atname        = ajStrNew();
	atdescription = ajStrNew();
	msid          = 0;
	
	sqlr = ajSqlrowiterGet(sqli);
	
	ajSqlcolumnToUint(sqlr, &identifier);
	ajSqlcolumnToUint(sqlr, &srid);
	ajSqlcolumnToUint(sqlr, &srstart);
	ajSqlcolumnToUint(sqlr, &srend);
	ajSqlcolumnToInt(sqlr, &srstrand);
	ajSqlcolumnToStr(sqlr, &avalue);
	ajSqlcolumnToStr(sqlr, &atcode);
        ajSqlcolumnToStr(sqlr, &atname);
	ajSqlcolumnToStr(sqlr, &atdescription);
	ajSqlcolumnToUint(sqlr, &msid);
	
	if(identifier == throw)
	{
	    /*
	    ** This Miscellanous Feature is already known to map into a gap,
	    ** therefore throw it away immediately.
	    */
	    
	    ajStrDel(&avalue);
	    ajStrDel(&atcode);
	    ajStrDel(&atname);
	    ajStrDel(&atdescription);
	    
	    continue;
	}
	
	if(identifier == current)
	{
	    /* Still working on the same Miscellaneous Feature. */
	    
	    if(msid)
		ensMiscellaneousSetadaptorFetchByIdentifier(msa, msid, &ms);
	    
	    if(ajStrGetLen(atcode) &&
               ajStrGetLen(avalue) &&
               !miscellaneousFeatureadaptorHasAttribute(attributes,
                                                        atcode,
                                                        avalue))
		attribute =
		    ensAttributeNew(atcode, atname, atdescription, avalue);
	}
	else
	{
	    /*
	    ** This is a new Miscellaneous Feature, therefore clear all
	    ** internal caches.
	    */
	    
	    miscellaneousFeatureadaptorClearAttributes(attributes);
	    
	    miscellaneousFeatureadaptorClearMiscellaneousSets(sets);
	    
	    current = identifier;
	    
	    throw = 0;
	    
	    /* Need to get the internal Ensembl Sequence Region identifier. */
	    
	    srid =
		ensCoordSystemadaptorGetInternalSeqRegionIdentifier(csa, srid);
	    
	    /*
	    ** Since the Ensembl SQL schema defines Sequence Region start and
	    ** end coordinates as unsigned integers for all Features, the
	    ** range needs checking.
	    */
	    
	    if(srstart <= INT_MAX)
		slstart = (ajint) srstart;
	    else
		ajFatal("miscellaneousFeatureadaptorFetchAllBySQL got a "
			"Sequence Region start coordinate (%u) outside the "
			"maximum integer limit (%d).",
			srstart, INT_MAX);
	    
	    if(srend <= INT_MAX)
		slend = (ajint) srend;
	    else
		ajFatal("miscellaneousFeatureadaptorFetchAllBySQL got a "
			"Sequence Region end coordinate (%u) outside the "
			"maximum integer limit (%d).",
			srend, INT_MAX);
	    
	    slstrand = srstrand;
	    
	    /* Fetch a Slice spanning the entire Sequence Region. */
	    
	    ensSliceadaptorFetchBySeqRegionIdentifier(sa,
						      srid,
						      0,
						      0,
						      0,
						      &srslice);
	    
	    /*
	    ** Obtain an Ensembl Assembly Mapper if none was defined, but a
	    ** destination Slice was.
	    */
	    
	    if(am)
		am = ensAssemblyMapperNewRef(am);
	    
	    if((!am) &&
		slice &&
		(!ensCoordSystemMatch(ensSliceGetCoordSystem(slice),
				       ensSliceGetCoordSystem(srslice))))
		am = ensAssemblyMapperadaptorFetchByCoordSystems(
                    ama,
                    ensSliceGetCoordSystem(slice),
                    ensSliceGetCoordSystem(srslice));
	    
	    /*
	    ** Remap the Feature coordinates to another Ensembl Coordinate
	    ** System if an Ensembl Assembly Mapper was provided.
	    */
	    
	    if(am)
	    {
		ensAssemblyMapperFastMap(am,
					 ensSliceGetSeqRegion(srslice),
					 slstart,
					 slend,
					 slstrand,
					 mrs);
		
		/*
		** The ensAssemblyMapperFastMap function returns at best one
		** Ensembl Mapper Result.
		*/
		
		ajListPop(mrs, (void **) &mr);
		
		/*
		** Skip Features that map to gaps or
		** Coordinate System boundaries.
		*/
		
		if(ensMapperResultGetType(mr) != ensEMapperResultCoordinate)
		{
		    /* Load the next Feature but destroy first! */
		    
		    ajStrDel(&avalue);
		    ajStrDel(&atcode);
		    ajStrDel(&atname);
		    ajStrDel(&atdescription);
		    
		    ensSliceDel(&srslice);
		    
		    ensAssemblyMapperDel(&am);
		    
		    ensMapperResultDel(&mr);
		    
		    /* Mark the Miscellaneous Feature as one to throw away. */
		    
		    throw = identifier;
		    
		    continue;
		}
		
		srid = ensMapperResultGetObjectIdentifier(mr);
		
		slstart = ensMapperResultGetStart(mr);
		
		slend = ensMapperResultGetEnd(mr);
		
		slstrand = ensMapperResultGetStrand(mr);
		
		/*
		** Delete the Sequence Region Slice and fetch a Slice
		** in the Coordinate System we just mapped to.
		*/
		
		ensSliceDel(&srslice);
		
		ensSliceadaptorFetchBySeqRegionIdentifier(sa,
							  srid,
							  0,
							  0,
							  0,
							  &srslice);
		
		ensMapperResultDel(&mr);
	    }
	    
	    /*
	    ** Convert Sequence Region Slice coordinates to destination Slice
	    ** coordinates, if a destination Slice has been provided.
	    */
	    
	    if(slice)
	    {
		/* Check that the length of the Slice is within range. */
		
		if(ensSliceGetLength(slice) <= INT_MAX)
		    sllength = (ajint) ensSliceGetLength(slice);
		else
		    ajFatal("miscellaneousFeatureadaptorFetchAllBySQL "
			    "got a Slice, "
			    "whose length (%u) exceeds the "
			    "maximum integer limit (%d).",
			    ensSliceGetLength(slice), INT_MAX);
		
		/*
		** Nothing needs to be done if the destination Slice starts
		** at 1 and is on the forward strand.
		*/
		
		if((ensSliceGetStart(slice) != 1) ||
		    (ensSliceGetStrand(slice) < 0))
		{
		    if(ensSliceGetStrand(slice) >= 0)
		    {
			slstart = slstart - ensSliceGetStart(slice) + 1;
			
			slend = slend - ensSliceGetStart(slice) + 1;
		    }
		    else
		    {
			slend = ensSliceGetEnd(slice) - slstart + 1;
			
			slstart = ensSliceGetEnd(slice) - slend + 1;
			
			slstrand *= -1;
		    }
		}
		
		/*
		** Throw away Features off the end of the requested Slice or
		** on any other than the requested Slice.
		*/
		
		if((slend < 1) ||
		    (slstart > sllength) ||
		    (srid != ensSliceGetSeqRegionIdentifier(slice)))
		{
		    /* Load the next Feature but destroy first! */
		    
		    ajStrDel(&avalue);
		    ajStrDel(&atcode);
		    ajStrDel(&atname);
		    ajStrDel(&atdescription);
		    
		    ensSliceDel(&srslice);
		    
		    ensAssemblyMapperDel(&am);
		    
		    /* Mark the Miscellaneous Feature as one to throw away. */
		    
		    throw = identifier;
		    
		    continue;
		}
		
		/*
		** Delete the Sequence Region Slice and set the
		** requested Slice.
		*/
		
		ensSliceDel(&srslice);
		
		srslice = ensSliceNewRef(slice);
	    }
	    
	    /* Finally, create a new Ensembl Miscellaneous Feature. */
	    
	    feature = ensFeatureNewS((EnsPAnalysis) NULL,
				     srslice,
				     slstart,
				     slend,
				     slstrand);
	    
	    mf = ensMiscellaneousFeatureNew(mfa, identifier, feature);
	    
	    ajListPushAppend(mflist, (void *) mf);
	    
	    /* Add an Ensembl Attribute if one was defined. */
	    
	    if(ajStrGetLen(atcode) && ajStrGetLen(avalue))
	    {
		attribute =
		ensAttributeNew(atcode, atname, atdescription, avalue);
		
		if(!miscellaneousFeatureadaptorHasAttribute(attributes,
							      atcode,
							      avalue))
		    ensMiscellaneousFeatureAddAttribute(mf, attribute);
		
		ensAttributeDel(&attribute);
	    }
	    
	    /* Add an Ensembl Miscellaneous Set if one was defined. */
	    
	    if(msid)
	    {
		if(!miscellaneousFeatureadaptorHasMiscellaneousSet(sets, msid))
		{
		    ensMiscellaneousSetadaptorFetchByIdentifier(msa, msid, &ms);
		    
		    ensMiscellaneousFeatureAddMiscellaneousSet(mf, ms);
		    
		    ensMiscellaneousSetDel(&ms);
		}
	    }
	    
	    ensFeatureDel(&feature);
	    
	    ensAssemblyMapperDel(&am);
	    
	    ensSliceDel(&srslice);
	    
	    ajStrDel(&avalue);
	    ajStrDel(&atcode);
	    ajStrDel(&atname);
	    ajStrDel(&atdescription);
	}
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    ajListFree(&mrs);
    
    miscellaneousFeatureadaptorClearAttributes(attributes);
    
    ajTableFree(&attributes);
    
    miscellaneousFeatureadaptorClearMiscellaneousSets(sets);
    
    ajTableFree(&sets);
    
    return ajTrue;
}




/* @funcstatic miscellaneousFeatureadaptorCacheReference **********************
**
** Wrapper function to reference an Ensembl Miscellaneous Feature
** from an Ensembl Cache.
**
** @param [r] value [void *] Ensembl Miscellaneous Feature.
**
** @return [void *] Ensembl Miscellaneous Feature or NULL;
** @@
******************************************************************************/

static void *miscellaneousFeatureadaptorCacheReference(void *value)
{
    if(!value)
	return NULL;
    
    return (void *)
        ensMiscellaneousFeatureNewRef((EnsPMiscellaneousFeature) value);
}




/* @funcstatic miscellaneousFeatureadaptorCacheDelete *************************
**
** Wrapper function to delete an Ensembl Miscellaneous Feature
** from an Ensembl Cache.
**
** @param [r] value [void**] Ensembl Miscellaneous Feature address.
**
** @return [void]
** @@
******************************************************************************/

static void miscellaneousFeatureadaptorCacheDelete(void **value)
{
    if(!value)
	return;
    
    ensMiscellaneousFeatureDel((EnsPMiscellaneousFeature *) value);
    
    return;
}




/* @funcstatic miscellaneousFeatureadaptorCacheSize ***************************
**
** Wrapper function to determine the memory size of an
** Ensembl Miscellaneous Feature from an Ensembl Cache.
**
** @param [r] value [const void*] Ensembl Miscellaneous Feature.
**
** @return [ajuint] Memory size.
** @@
******************************************************************************/

static ajuint miscellaneousFeatureadaptorCacheSize(const void *value)
{
    if(!value)
	return 0;
    
    return ensMiscellaneousFeatureGetMemSize(
        (const EnsPMiscellaneousFeature) value);
}




/* @funcstatic miscellaneousFeatureadaptorGetFeature **************************
**
** Wrapper function to get the Ensembl Feature of an
** Ensembl Miscellaneous Feature from an Ensembl Feature Adaptor.
**
** @param [r] value [const void*] Ensembl Miscellaneous Feature.
**
** @return [EnsPFeature] Ensembl Feature.
** @@
******************************************************************************/

static EnsPFeature miscellaneousFeatureadaptorGetFeature(const void *value)
{
    if(!value)
	return NULL;
    
    return ensMiscellaneousFeatureGetFeature(
        (const EnsPMiscellaneousFeature) value);
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Miscellaneous Feature Adaptor by
** pointer.
** It is the responsibility of the user to first destroy any previous
** Miscellaneous Feature Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMiscellaneousFeatureadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPMiscellaneousFeatureadaptor] Ensembl Miscellaneous
**                                                       Feature Adaptor
** @argrule Ref object [EnsPMiscellaneousFeatureadaptor] Ensembl Miscellaneous
**                                                       Feature Adaptor
**
** @valrule * [EnsPMiscellaneousFeatureadaptor] Ensembl Miscellaneous
**                                              Feature Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensMiscellaneousFeatureadaptorNew ************************************
**
** Default Ensembl Miscellaneous Feature Adaptor constructor.
**
** @cc Bio::EnsEMBL::DBSQL::MiscFeatureadaptor::new
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor.
**
** @return [EnsPMiscellaneousFeatureadaptor] Ensembl Miscellaneous Feature
**                                           Adaptor or NULL.
** @@
******************************************************************************/

EnsPMiscellaneousFeatureadaptor ensMiscellaneousFeatureadaptorNew(
    EnsPDatabaseadaptor dba)
{
    EnsPMiscellaneousFeatureadaptor mfa = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(mfa);
    
    mfa->Adaptor =
	ensFeatureadaptorNew(dba,
			     miscellaneousFeatureadaptorTables,
			     miscellaneousFeatureadaptorColumns,
			     miscellaneousFeatureadaptorLeftJoin,
			     miscellaneousFeatureadaptorDefaultCondition,
			     miscellaneousFeatureadaptorFinalCondition,
			     miscellaneousFeatureadaptorFetchAllBySQL,
			     (void* (*)(const void* key)) NULL, /* Fread */
			     miscellaneousFeatureadaptorCacheReference,
			     (AjBool (*)(const void* value)) NULL, /* Fwrite */
			     miscellaneousFeatureadaptorCacheDelete,
			     miscellaneousFeatureadaptorCacheSize,
			     miscellaneousFeatureadaptorGetFeature,
			     "Miscellaneous Feature");
    
    return mfa;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Miscellaneous Set Adaptor.
**
** @fdata [EnsPMiscellaneousFeatureadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Miscellaneous Feature Adaptor object
**
** @argrule * Padaptor [EnsPMiscellaneousFeatureadaptor*] Ensembl Miscellaneous
**                                                        Feature Adaptor
**                                                        address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMiscellaneousFeatureadaptorDel ************************************
**
** Default destructor for an Ensembl Miscellaneous Feature Adaptor.
**
** @param [d] Padaptor [EnsPMiscellaneousFeatureadaptor*] Ensembl Miscellaneous
**                                                        Feature Adaptor
**                                                        address.
**
** @return [void]
** @@
******************************************************************************/

void ensMiscellaneousFeatureadaptorDel(
    EnsPMiscellaneousFeatureadaptor *Padaptor)
{
    EnsPMiscellaneousFeatureadaptor pthis = NULL;
    
    if(!Padaptor)
	return;
    
    if(!*Padaptor)
	return;

    pthis = *Padaptor;
    
    ensFeatureadaptorDel(&pthis->Adaptor);
    
    AJFREE(pthis);

    *Padaptor = NULL;
    
    return;
}




/* @section object fetching ***************************************************
**
** Functions for retrieving Ensembl Miscellaneous Feature objects from an
** Ensembl Core database.
**
** @fdata [EnsPMiscellaneousFeatureadaptor]
** @fnote None
**
** @nam3rule Fetch Retrieve Ensembl Miscellaneous Feature object(s)
** @nam4rule FetchAll Retrieve all Ensembl Miscellaneous Feature objects.
** @nam5rule FetchAllBy Retrieve all Ensembl Miscellaneous Feature objects
**                      matching a criterion.
** @nam4rule FetchBy Retrieve one Ensembl Miscellaneous Feature object
**                   matching a criterion.
**
** @argrule * adaptor [const EnsPMiscellaneousFeatureadaptor] Ensembl
**                                                            Miscellaneous
**                                                            Feature Adaptor
** @argrule FetchAll [AjPList] AJAX List of Ensembl Miscellaneous Feature
**                             objects
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise.
**
** @fcategory use
******************************************************************************/




/* @func ensMiscellaneousFeatureadaptorFetchAllBySliceAndSetCode **************
**
** Fetch all Ensembl Miscellaneous Features by an Ensembl Slice and
** Ensembl Miscellaneous Set codes.
**
** @cc Bio::EnsEMBL::DBSQL::MiscFeatureadaptor::
**     fetch_all_by_Slice_and_set_code
** @param [r] adaptor [const EnsPMiscellaneousFeatureadaptor] Ensembl
**                                                            Miscellaneous
**                                                            Feature Adaptor.
** @param [r] slice [const EnsPSlice] Ensembl Slice.
** @param [r] codes [const AjPList] Ensembl Miscellaneous Set codes.
** @param [u] mflist [AjPList] AJAX List of Ensembl Miscellaneous Features.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousFeatureadaptorFetchAllBySliceAndSetCode(
    EnsPMiscellaneousFeatureadaptor adaptor,
    EnsPSlice slice,
    const AjPList codes,
    AjPList mflist)
{
    ajuint maxlen = 0;
    
    AjIList iter   = NULL;
    AjPList mslist = NULL;
    
    AjPStr code       = NULL;
    AjPStr constraint = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    EnsPMiscellaneousSet ms         = NULL;
    EnsPMiscellaneousSetadaptor msa = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!slice)
	return ajFalse;
    
    if(!codes)
	return ajFalse;
    
    if(!mflist)
	return ajFalse;
    
    dba = ensFeatureadaptorGetDatabaseadaptor(adaptor->Adaptor);
    
    msa = ensRegistryGetMiscellaneousSetadaptor(dba);
    
    mslist = ajListNew();
    
    iter = ajListIterNewread(codes);
    
    while(!ajListIterDone(iter))
    {
	code = (AjPStr) ajListIterGet(iter);
	
	ensMiscellaneousSetadaptorFetchByCode(msa, code, &ms);
	
	if(ms)
	{
	    maxlen = (ensMiscellaneousSetGetMaximumLength(ms) > maxlen) ?
	    ensMiscellaneousSetGetMaximumLength(ms) : maxlen;
	    
	    ajListPushAppend(mslist, (void *) ms);
	}
	else
	    ajWarn("ensMiscellaneousFeatureadaptorFetchAllBySliceAndSetCode "
		   "no Miscellaneous Set with code '%S'.\n", code);
    }
    
    ajListIterDel(&iter);
    
    if(ajListGetLength(mslist) > 0)
    {
	constraint = ajStrNewC("misc_feature_misc_set.misc_set_id IN (");
	
	while(ajListPop(mslist, (void **) &ms))
	{
	    ajFmtPrintAppS(&constraint, "%u, ",
			   ensMiscellaneousSetGetIdentifier(ms));
	    
	    ensMiscellaneousSetDel(&ms);
	}
	
	/* Remove last comma and space from the constraint. */
	
	ajStrCutEnd(&constraint, 2);
	
	ajStrAppendC(&constraint, ")");
	
	ensFeatureadaptorSetMaximumFeatureLength(adaptor->Adaptor, maxlen);
	
	ensFeatureadaptorFetchAllBySliceConstraint(adaptor->Adaptor,
						   slice,
						   constraint,
						   (const AjPStr) NULL,
						   mflist);
	
	ensFeatureadaptorSetMaximumFeatureLength(adaptor->Adaptor, 0);
	
	ajStrDel(&constraint);
    }
    
    ajListFree(&mslist);
    
    return ajTrue;
}




/* @func ensMiscellaneousFeatureadaptorFetchAllByAttributeCodeValue ***********
**
** Fetch all Ensembl Miscellaneous Features by an Ensembl Slice and
** Ensembl Miscellaneous Set codes.
**
** @cc Bio::EnsEMBL::DBSQL::MiscFeatureadaptor::
**     fetch_all_by_attribute_type_value
** @param [r] adaptor [const EnsPMiscellaneousFeatureadaptor] Ensembl
**                                                            Miscellaneous
**                                                            Feature Adaptor.
** @param [r] slice [const EnsPSlice] Ensembl Slice.
** @param [r] codes [const AjPStr] Attribute type codes.
** @param [r] value [const AjPStr] Miscellaneous attribute value.
** @param [u] mflist [AjPList] AJAX List of Ensembl Miscellaneous Features.
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise.
** @@
******************************************************************************/

AjBool ensMiscellaneousFeatureadaptorFetchAllByAttributeCodeValue(
    EnsPMiscellaneousFeatureadaptor adaptor,
    const AjPStr code,
    const AjPStr value,
    AjPList mflist)
{
    ajuint counter = 0;
    ajuint mfid    = 0;
    
    char *txtcode  = NULL;
    char *txtvalue = NULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr constraint  = NULL;
    AjPStr identifiers = NULL;
    AjPStr statement   = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!code)
	ajFatal("ensMiscellaneousFeatureadaptorFetchAllByAttributeCodeValue "
		"requires an Ensembl Attribute code.\n");
    
    ba = ensFeatureadaptorGetBaseadaptor(adaptor->Adaptor);
    
    dba = ensFeatureadaptorGetDatabaseadaptor(adaptor->Adaptor);
    
    /*
    ** Need to do two queries so that all of the ids come back with the
    ** features.  The problem with adding attrib constraints to filter the
    ** misc_features which come back is that not all of the attributes will
    ** come back
    */
    
    ensDatabaseadaptorEscapeC(dba, &txtcode, code);
    
    statement =
	ajFmtStr("SELECT DISTINCT "
		 "misc_attrib.misc_feature_id "
		 "FROM "
		 "misc_attrib, "
		 "attrib_type, "
		 "misc_feature, "
		 "seq_region, "
		 "coord_system "
		 "WHERE "
		 "misc_attrib.attrib_type_id = attrib_type.attrib_type_id "
		 "AND "
		 "attrib_type.code = '%s' "
		 "AND "
		 "misc_attrib.misc_feature_id = misc_feature.misc_feature_id "
		 "AND "
		 "misc_feature.seq_region_id = seq_region.seq_region_id "
		 "AND "
		 "seq_region.coord_system_id = coord_system.coord_system_id "
		 "AND "
		 "coord_system.species_id = %u",
		 txtcode,
		 ensDatabaseadaptorGetIdentifier(dba));
    
    ajCharDel(&txtcode);
    
    if(value)
    {
	ensDatabaseadaptorEscapeC(dba, &txtvalue, value);
	
	ajFmtPrintAppS(&statement, " AND misc_attrib.value = '%s'", txtvalue);
	
	ajCharDel(&txtvalue);
    }
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    identifiers = ajStrNew();
    
    while(!ajSqlrowiterDone(sqli))
    {
        mfid = 0;
	
	sqlr = ajSqlrowiterGet(sqli);
	
        ajSqlcolumnToUint(sqlr, &mfid);
	
	ajFmtPrintAppS(&identifiers, "%u, ", mfid);
	
	/*
	** Construct constraints from the list of identifiers.  Split
	** identifiers into groups of 1000 to ensure that the statement is not
	** too big.
	*/
    	
	counter++;
	
	if(counter > 1000)
	{
	    constraint = ajFmtStr("misc_feature.misc_feature_id in (%S)",
				  identifiers);
	    
	    ensBaseadaptorGenericFetch(ba,
				       constraint,
				       (EnsPAssemblyMapper) NULL,
				       (EnsPSlice) NULL,
				       mflist);
	    
	    ajStrDel(&constraint);
	    
	    ajStrSetClear(&identifiers);
	    
	    counter = 0;
	}
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    /* Run the last statement. */
    
    constraint = ajFmtStr("misc_feature.misc_feature_id in (%S)",
			  identifiers);
    
    ensBaseadaptorGenericFetch(ba,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       mflist);
    
    
    
    ajStrDel(&constraint);
    ajStrDel(&identifiers);
    
    return ajTrue;
}
