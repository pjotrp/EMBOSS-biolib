/******************************************************************************
** @source Ensembl Meta-Information functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.1 $
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
******************************************************************************/

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ensmetainformation.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

static AjBool metaInformationAdaptorFetchAllBySQL(
    EnsPMetaInformationAdaptor adaptor,
    const AjPStr statement,
    AjPList mis);

static AjBool metaInformationAdaptorCacheInit(
    EnsPMetaInformationAdaptor adaptor);

static void metaInformationAdaptorClearIdentifierCache(void **key,
                                                       void **value,
                                                       void *cl);

static void metaInformationAdaptorClearKeyCache(void **key, void **value,
                                                void *cl);




/* @filesection ensmetainformation ********************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPMetaInformation] Meta-Information ************************
**
** Functions for manipulating Ensembl Meta-Information objects
**
** @nam2rule MetaInformation
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Meta-Information by pointer.
** It is the responsibility of the user to first destroy any previous
** Meta-Information. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMetaInformation]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMetaInformation] Ensembl Meta-Information
** @argrule Ref object [EnsPMetaInformation] Ensembl Meta-Information
**
** @valrule * [EnsPMetaInformation] Ensembl Meta-Information
**
** @fcategory new
******************************************************************************/




/* @func ensMetaInformationNew ************************************************
**
** Default constructor for an Ensembl Meta-Information object.
**
** @cc Bio::EnsEMBL::Storable
** @param [r] adaptor [EnsPMetaInformationAdaptor] Ensembl Meta-Information
**                                                 Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::MetaContainer
** @param [r] species [ajuint] Species identififer
** @param [u] key [AjPStr] Key
** @param [u] value [AjPStr] Value
**
** @return [EnsPMetaInformation] Ensembl Meta-Information or NULL
** @@
******************************************************************************/

EnsPMetaInformation ensMetaInformationNew(EnsPMetaInformationAdaptor adaptor,
                                          ajuint identifier,
                                          ajuint species,
                                          AjPStr key,
                                          AjPStr value)
{
    EnsPMetaInformation mi = NULL;
    
    AJNEW0(mi);
    
    mi->Use = 1;
    
    mi->Identifier = identifier;
    
    mi->Adaptor = adaptor;
    
    if(key)
	mi->Key = ajStrNewRef(key);
    
    if(value)
	mi->Value = ajStrNewRef(value);
    
    mi->Species = (species) ? species : 1;
    
    return mi;
}




/* @func ensMetaInformationNewObj *********************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [EnsPMetaInformation] Ensembl Meta-Information
**
** @return [EnsPMetaInformation] Ensembl Meta-Information or NULL
** @@
******************************************************************************/

EnsPMetaInformation ensMetaInformationNewObj(EnsPMetaInformation object)
{
    EnsPMetaInformation mi = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(mi);
    
    mi->Use = 1;
    
    mi->Identifier = object->Identifier;
    
    mi->Adaptor = object->Adaptor;
    
    if(object->Key)
        mi->Key = ajStrNewRef(object->Key);
    
    if(object->Value)
        mi->Value = ajStrNewRef(object->Value);
    
    mi->Species = (object->Species) ? object->Species : 1;
    
    return mi;
}




/* @func ensMetaInformationNewRef *********************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] mi [EnsPMetaInformation] Ensembl Meta-Information
**
** @return [EnsPMetaInformation] Ensembl Meta-Information or NULL
** @@
******************************************************************************/

EnsPMetaInformation ensMetaInformationNewRef(EnsPMetaInformation mi)
{
    if(!mi)
	return NULL;
    
    mi->Use++;
    
    return mi;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Meta-Information.
**
** @fdata [EnsPMetaInformation]
** @fnote None
**
** @nam3rule Del Destroy (free) a Meta-Information object
**
** @argrule * Pmi [EnsPMetaInformation*] Meta-Information object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMetaInformationDel ************************************************
**
** Default destructor for an Ensembl Meta-Information object.
**
** @param [d] Pmi [EnsPMetaInformation*] Ensembl Meta-Information address
**
** @return [void]
** @@
******************************************************************************/

void ensMetaInformationDel(EnsPMetaInformation *Pmi)
{
    EnsPMetaInformation pthis = NULL;
    
    if(!Pmi)
        return;
    
    if(!*Pmi)
        return;

    pthis = *Pmi;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pmi = NULL;
	
	return;
    }
    
    ajStrDel(&pthis->Key);
    
    ajStrDel(&pthis->Value);
    
    AJFREE(pthis);

    *Pmi = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Meta-Information object.
**
** @fdata [EnsPMetaInformation]
** @fnote None
**
** @nam3rule Get Return Meta-Information attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Meta-Information Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetKey Return the key
** @nam4rule GetValue Return the value
** @nam4rule GetSpecies Return the species identifier
**
** @argrule * mi [const EnsPMetaInformation] Ensembl Meta-Information
**
** @valrule Adaptor [EnsPMetaInformationAdaptor] Ensembl Meta-Information
**                                               Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Key [AjPStr] Key
** @valrule Value [AjPStr] Value
** @valrule Species [ajuint] Species identifier
**
** @fcategory use
******************************************************************************/




/* @func ensMetaInformationGetAdaptor *****************************************
**
** Get the Ensembl Meta-Information Adaptor element of an
** Ensembl Meta-Information.
**
** @param [r] mi [const EnsPMetaInformation] Ensembl Meta-Information
**
** @return [EnsPMetaInformationAdaptor] Ensembl Meta-Information Adaptor
**                                      or NULL
** @@
******************************************************************************/

EnsPMetaInformationAdaptor ensMetaInformationGetAdaptor(
    const EnsPMetaInformation mi)
{
    if(!mi)
        return NULL;
    
    return mi->Adaptor;
}




/* @func ensMetaInformationGetIdentifier **************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Meta-Information.
**
** @param [r] mi [const EnsPMetaInformation] Ensembl Meta-Information
**
** @return [ajuint] SQL database-internal identifier
** @@
******************************************************************************/

ajuint ensMetaInformationGetIdentifier(const EnsPMetaInformation mi)
{
    if(!mi)
	return 0;
    
    return mi->Identifier;
}




/* @func ensMetaInformationGetKey *********************************************
**
** Get the key element of an Ensembl Meta-Information.
**
** @param [r] mi [const EnsPMetaInformation] Ensembl Meta-Information
**
** @return [AjPStr] Key
** @@
******************************************************************************/

AjPStr ensMetaInformationGetKey(const EnsPMetaInformation mi)
{
    if(!mi)
        return NULL;
    
    return mi->Key;
}




/* @func ensMetaInformationGetValue *******************************************
**
** Get the value element of an Ensembl Meta-Information.
**
** @param [r] mi [const EnsPMetaInformation] Ensembl Meta-Information
**
** @return [AjPStr] Value
** @@
******************************************************************************/

AjPStr ensMetaInformationGetValue(const EnsPMetaInformation mi)
{
    if(!mi)
	return NULL;
    
    return mi->Value;
}




/* @func ensMetaInformationGetSpecies *****************************************
**
** Get the species identifier element of an Ensembl Meta-Information object.
**
** @param [r] mi [const EnsPMetaInformation] Ensembl Meta-Information
**
** @return [ajuint] Species identifier
** @@
******************************************************************************/

ajuint ensMetaInformationGetSpecies(const EnsPMetaInformation mi)
{
    if(!mi)
	return 0;
    
    return mi->Species;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl Meta-Information object.
**
** @fdata [EnsPMetaInformation]
** @fnote None
**
** @nam3rule Set Set one element of a Meta-Information
** @nam4rule SetAdaptor Set the Ensembl Meta-Information Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetKey Set the key
** @nam4rule SetValue Set the value
** @nam4rule SetSpecies Set the species identifier
**
** @argrule * mi [EnsPMetaInformation] Ensembl Meta-Information object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensMetaInformationSetAdaptor *****************************************
**
** Set the Ensembl Meta-Information Adaptor element of an
** Ensembl Meta-Information.
**
** @param [u] mi [EnsPMetaInformation] Ensembl Meta-Information
** @param [r] adaptor [EnsPMetaInformationAdaptor] Ensembl Meta-Information
**                                                 Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaInformationSetAdaptor(EnsPMetaInformation mi,
                                    EnsPMetaInformationAdaptor adaptor)
{
    if(!mi)
        return ajFalse;
    
    mi->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensMetaInformationSetIdentifier **************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Meta-Information.
**
** @param [u] mi [EnsPMetaInformation] Ensembl Meta-Information
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaInformationSetIdentifier(EnsPMetaInformation mi,
                                       ajuint identifier)
{
    if(!mi)
        return ajFalse;
    
    mi->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensMetaInformationSetKey *********************************************
**
** Set the key element of an Ensembl Meta-Information.
**
** @param [u] mi [EnsPMetaInformation] Ensembl Meta-Information
** @param [u] key [AjPStr] Key
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaInformationSetKey(EnsPMetaInformation mi, AjPStr key)
{
    if(!mi)
        return ajFalse;
    
    ajStrDel(&mi->Key);
    
    if(key)
	mi->Key = ajStrNewRef(key);
    
    return ajTrue;
}




/* @func ensMetaInformationSetValue *******************************************
**
** Set the value element of an Ensembl Meta-Information.
**
** @param [u] mi [EnsPMetaInformation] Ensembl Meta-Information
** @param [u] value [AjPStr] Value
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaInformationSetValue(EnsPMetaInformation mi, AjPStr value)
{
    if(!mi)
        return ajFalse;
    
    ajStrDel(&mi->Value);
    
    if(value)
	mi->Value = ajStrNewRef(value);
    
    return ajTrue;
}




/* @func ensMetaInformationSetSpecies *****************************************
**
** Set the species identifier element of an Ensembl Meta-Information.
**
** @param [u] mi [EnsPMetaInformation] Ensembl Meta-Information
** @param [r] species [ajuint] Species identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaInformationSetSpecies(EnsPMetaInformation mi, ajuint species)
{
    if(!mi)
        return ajFalse;
    
    mi->Species = species;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Meta-Information.
**
** @fdata [EnsPMetaInformation]
** @nam3rule Trace Report Ensembl Meta-Information elements to debug file
**
** @argrule Trace mi [const EnsPMetaInformation] Ensembl Meta-Information
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensMetaInformationTrace **********************************************
**
** Trace an Ensembl Meta-Information.
**
** @param [r] mi [const EnsPMetaInformation] Ensembl Meta-Information
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaInformationTrace(const EnsPMetaInformation mi, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!mi)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensMetaInformationTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Key '%S'\n"
	    "%S  Value '%S'\n"
	    "%S  Species %u\n",
	    indent, mi,
	    indent, mi->Use,
	    indent, mi->Identifier,
	    indent, mi->Adaptor,
	    indent, mi->Key,
	    indent, mi->Value,
	    indent, mi->Species);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensMetaInformationGetMemSize *****************************************
**
** Get the memory size in bytes of an Ensembl Meta-Information.
**
** @param [r] mi [const EnsPMetaInformation] Ensembl Meta-Information
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensMetaInformationGetMemSize(const EnsPMetaInformation mi)
{
    ajuint size = 0;
    
    if(!mi)
	return 0;
    
    size += (ajuint) sizeof (EnsOMetaInformation);
    
    if(mi->Key)
    {
	size += sizeof (AjOStr);
	
	size += ajStrGetRes(mi->Key);
    }
    
    if(mi->Value)
    {
	size += sizeof (AjOStr);
	
	size += ajStrGetRes(mi->Value);
    }
    
    return size;
}




/* @datasection [EnsPMetaInformationAdaptor] Meta-Information Adaptor *********
**
** Functions for manipulating Ensembl Meta-Information Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::BaseMetaContainer CVS Revision: 1.4
** @cc Bio::EnsEMBL::DBSQL::MetaContainer CVS Revision: 1.28
**
** @nam2rule MetaInformationAdaptor
**
******************************************************************************/




/* @funcstatic metaInformationAdaptorFetchAllBySQL ****************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl Meta-Information objects.
**
** @param [r] adaptor [EnsPMetaInformationAdaptor] Ensembl Meta-Information
**                                                 Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] mis [AjPList] AJAX List of Ensembl Meta-Informations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool metaInformationAdaptorFetchAllBySQL(
    EnsPMetaInformationAdaptor adaptor,
    const AjPStr statement,
    AjPList mis)
{
    ajuint identifier = 0;
    ajuint species    = 0;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr key   = NULL;
    AjPStr value = NULL;
    
    EnsPMetaInformation mi = NULL;
    
    /*
     ajDebug("metaInformationAdaptorFetchAllByQuery\n"
	     "  adaptor %p\n"
	     "  statement %p\n"
	     "  mis %p\n",
	     adaptor,
	     statement,
	     mis);
     */
    
    if(!adaptor)
        return ajFalse;
    
    if(!statement)
        return ajFalse;
    
    if(!mis)
        return ajFalse;
    
    sqls = ensDatabaseAdaptorSqlStatementNew(adaptor->Adaptor, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	identifier = 0;
	species    = 0;
	
	key   = ajStrNew();
	value = ajStrNew();
	
        sqlr = ajSqlRowIterGet(sqli);
	
        ajSqlColumnToUint(sqlr, &identifier);
        ajSqlColumnToUint(sqlr, &species);
        ajSqlColumnToStr(sqlr, &key);
        ajSqlColumnToStr(sqlr, &value);
	
        mi = ensMetaInformationNew(adaptor, identifier, species, key, value);
	
        ajListPushAppend(mis, (void *) mi);
	
	ajStrDel(&key);
	ajStrDel(&value);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    return ajTrue;
}




/* @funcstatic metaInformationAdaptorCacheInit ********************************
**
** Initialise the internal Meta-Information cache of an
** Ensembl Meta-Information Adaptor.
**
** @param [u] adaptor [EnsPMetaInformationAdaptor] Ensembl Meta-Information
**                                                 Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool metaInformationAdaptorCacheInit(
    EnsPMetaInformationAdaptor adaptor)
{
    ajuint *Pidentifier = NULL;
    
    AjPList mis      = NULL;
    AjPList list     = NULL;
    AjPStr statement = NULL;
    
    EnsPMetaInformation mi   = NULL;
    EnsPMetaInformation temp = NULL;
    
    /*
     ajDebug("metaInformationAdaptorCacheInit\n"
	     "  adaptor %p\n",
	     adaptor);
     */
    
    if(!adaptor)
	return ajFalse;
    
    statement = ajStrNewC("SELECT "
			  "meta.meta_id, "
			  "meta.species_id, "
			  "meta.meta_key, "
			  "meta.meta_value "
			  "FROM "
			  "meta ");
    
    mis = ajListNew();
    
    metaInformationAdaptorFetchAllBySQL(adaptor, statement, mis);
    
    while(ajListPop(mis, (void **) &mi))
    {
	/* Insert into the identifier cache. */
	
	AJNEW0(Pidentifier);
	
	*Pidentifier = mi->Identifier;
	
	temp = (EnsPMetaInformation) ajTablePut(adaptor->CacheByIdentifier,
						(void *) Pidentifier,
						(void *) mi);
	
	if(temp)
	{
	    ajWarn("metaInformationCacheInit got more than one "
		   "Ensembl Meta-Information with identifier %u.\n",
		   temp->Identifier);
	    
	    ensMetaInformationDel(&temp);
	}
	
	/* Insert into the key cache. */
	
	list = (AjPList)
	    ajTableFetch(adaptor->CacheByKey, (const void *) mi->Key);
	
	if(!list)
	{
	    list = ajListNew();
	    
	    ajTablePut(adaptor->CacheByKey,
		       (void *) ajStrNewS(mi->Key),
		       (void *) list);
	}
	
	ajListPushAppend(list, (void *) ensMetaInformationNewRef(mi));
    }
    
    ajListFree(&mis);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Meta-Information Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Meta-Information Adaptor. The target pointer does not need to be initialised
** to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMetaInformationAdaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPMetaInformationAdaptor] Ensembl Meta-Information
**                                                  Adaptor
** @argrule Ref object [EnsPMetaInformationAdaptor] Ensembl Meta-Information
**                                                  Adaptor
**
** @valrule * [EnsPMetaInformationAdaptor] Ensembl Meta-Information Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensMetaInformationAdaptorNew *****************************************
**
** Default constructor for an Ensembl Meta-Information Adaptor.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPMetaInformationAdaptor] Ensembl Meta-Information Adaptor
**                                      or NULL
** @@
******************************************************************************/

EnsPMetaInformationAdaptor ensMetaInformationAdaptorNew(EnsPDatabaseAdaptor dba)
{
    EnsPMetaInformationAdaptor mia = NULL;
    
    /*
     ajDebug("ensMetaInformationAdaptorNew\n"
	     "  dba %p\n",
	     dba);
     */
    
    if(!dba)
	return NULL;
    
    AJNEW0(mia);
    
    mia->Adaptor = dba;
    
    mia->CacheByIdentifier = MENSTABLEUINTNEW(0);
    
    mia->CacheByKey = ajTablestrNewLen(0);
    
    metaInformationAdaptorCacheInit(mia);
    
    return mia;
}




/* @funcstatic metaInformationAdaptorClearIdentifierCache *********************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Meta-Information
** Adaptor-internal Ensembl Meta-Information cache. This function deletes the
** unsigned integer identifier key and the Ensembl Meta-Information value data.
**
** @param [u] key [void **] AJAX unsigned integer key data address
** @param [u] value [void **] Ensembl Meta-Information value data address
** @param [r] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void metaInformationAdaptorClearIdentifierCache(void **key,
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
    
    ensMetaInformationDel((EnsPMetaInformation *) value);
    
    return;
}




/* @funcstatic metaInformationAdaptorClearKeyCache ****************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Meta-Information
** Adaptor-internal Ensembl Meta-Information cache. This function deletes the
** AJAX String Meta-Information key data and the AJAX List value data, as well
** as the Ensembl Meta-Information data from the AJAX List.
**
** @param [u] key [void **] AJAX String key data address
** @param [u] value [void **] AJAX List value data address
** @param [r] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void metaInformationAdaptorClearKeyCache(void **key, void **value,
                                                void *cl)
{
    EnsPMetaInformation mi = NULL;
    
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
    
    while(ajListPop(*((AjPList *) value), (void **) &mi))
	ensMetaInformationDel(&mi);
    
    ajListFree((AjPList *) value);
    
    return;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Meta-Information Adaptor.
**
** @fdata [EnsPMetaInformationAdaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) a Meta-Information Adaptor object
**
** @argrule * Padaptor [EnsPMetaInformationAdaptor*] Meta-Information Adaptor
**                                                   object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMetaInformationAdaptorDel *****************************************
**
** Default destructor for an Ensembl Meta-Information Adaptor.
** This function also clears the internal caches.
**
** @param [d] Padaptor [EnsPMetaInformationAdaptor*] Ensembl Meta-Information
**                                                   Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensMetaInformationAdaptorDel(EnsPMetaInformationAdaptor *Padaptor)
{
    EnsPMetaInformationAdaptor pthis = NULL;
    
    if(!Padaptor)
	return;
    
    if(!*Padaptor)
	return;

    pthis = *Padaptor;
    
    /* Clear and delete the identifier cache. */
    
    ajTableMapDel(pthis->CacheByIdentifier,
		  metaInformationAdaptorClearIdentifierCache,
		  NULL);
    
    ajTableFree(&((*Padaptor)->CacheByIdentifier));
    
    /* Clear and delete the key cache. */
    
    ajTableMapDel(pthis->CacheByKey,
		  metaInformationAdaptorClearKeyCache,
		  NULL);
    
    ajTableFree(&pthis->CacheByKey);
    
    AJFREE(pthis);

    *Padaptor = NULL;
    
    return;
}




/* @func ensMetaInformationAdaptorFetchAllByKey *******************************
**
** Fetch Ensembl Meta-Information objects by a key.
**
** @param [r] adaptor [const EnsPMetaInformationAdaptor] Ensembl
**                                                       Meta-Information
**                                                       Adaptor
** @param [r] key [const AjPStr] Key
** @param [u] mis [AjPList] AJAX List of Ensembl Meta-Informations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaInformationAdaptorFetchAllByKey(
    const EnsPMetaInformationAdaptor adaptor,
    const AjPStr key,
    AjPList mis)
{
    AjIList iter = NULL;
    AjPList list = NULL;
    
    EnsPMetaInformation mi = NULL;
    
    if(!adaptor)
        return ajFalse;
    
    if(!key)
        return ajFalse;
    
    if(!mis)
        return ajFalse;
    
    list = (AjPList) ajTableFetch(adaptor->CacheByKey, (const void *) key);
    
    iter = ajListIterNew(list);
    
    while(!ajListIterDone(iter))
    {
	mi = (EnsPMetaInformation) ajListIterGet(iter);
	
	ajListPushAppend(mis, (void **) ensMetaInformationNewRef(mi));
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}




/* @func ensMetaInformationAdaptorGetValueByKey *******************************
**
** Get a single Meta-Information value by a key.
** The function warns in case there is more than one Meta-Information value to
** a particular key and will return the first value returned by the database
** engine.
**
** @param [r] adaptor [const EnsPMetaInformationAdaptor] Ensembl
**                                                       Meta-Information
**                                                       Adaptor
** @param [r] key [const AjPStr] Key
** @param [wP] Pvalue [AjPStr*] Value String address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaInformationAdaptorGetValueByKey(
    const EnsPMetaInformationAdaptor adaptor,
    const AjPStr key,
    AjPStr* Pvalue)
{
    ajint errors = 0;
    ajint length = 0;
    
    AjPList list = NULL;
    
    EnsPMetaInformation mi = NULL;
    
    if(!adaptor)
        return ajFalse;
    
    if(!key)
        return ajFalse;
    
    if(!Pvalue)
        return ajFalse;
    
    if(!*Pvalue)
        return ajFalse;
    
    list = (AjPList) ajTableFetch(adaptor->CacheByKey, (const void *) key);
    
    length = ajListGetLength(list);
    
    if(!length)
    {
        errors++;
	
        ajDebug("ensMetaInformationAdaptorGetValueByKey No 'meta.meta_value' "
	        "matched for 'meta.meta_key' '%S'.\n", key);
    }
    else
    {
	if(length > 1)
	{
	    errors++;
	    
	    ajDebug("ensMetaInformationAdaptorGetValueByKey matched %d "
		    "'meta.meta_value' by 'meta.meta_key' '%S', "
		    "but only the first value was returned.\n",
		    length, key);
	}
	
	ajListPeekFirst(list, (void **) &mi);
	
	ajStrAssignS(Pvalue, ensMetaInformationGetValue(mi));
    }
    
    if(errors)
        return ajFalse;
    
    return ajTrue;
}




/* @func ensMetaInformationAdaptorGetGenebuildVersion *************************
**
** Get the Ensembl Genebuild Version.
**
** @param [r] adaptor [const EnsPMetaInformationAdaptor] Ensembl
**                                                       Meta-Information
*                                                        Adaptor
** @param [wP] Pvalue [AjPStr*] Value String address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaInformationAdaptorGetGenebuildVersion(
    const EnsPMetaInformationAdaptor adaptor,
    AjPStr *Pvalue)
{
    ajint errors = 0;
    
    AjPStr key = NULL;
    
    if(!adaptor)
        return ajFalse;
    
    if(!Pvalue)
        return ajFalse;
    
    if(!*Pvalue)
        return ajFalse;
    
    key = ajStrNewC("genebuild.version");
    
    if(!ensMetaInformationAdaptorGetValueByKey(adaptor, key, Pvalue))
        errors++;
    
    ajStrDel(&key);
    
    if(errors)
        return ajFalse;
    
    return ajTrue;
}




/* @func ensMetaInformationAdaptorGetSchemaVersion ****************************
**
** Get the Ensembl Database Schema Version.
**
** @param [r] adaptor [const EnsPMetaInformationAdaptor] Ensembl
**                                                       Meta-Information
**                                                       Adaptor
** @param [wP] Pvalue [AjPStr*] Value String address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaInformationAdaptorGetSchemaVersion(
    const EnsPMetaInformationAdaptor adaptor,
    AjPStr *Pvalue)
{
    ajint errors = 0;
    
    AjPStr key = NULL;
    
    if(!adaptor)
        return ajFalse;
    
    if(!Pvalue)
        return ajFalse;
    
    if(!*Pvalue)
        return ajFalse;
    
    key = ajStrNewC("schema_version");
    
    if(!ensMetaInformationAdaptorGetValueByKey(adaptor, key, Pvalue))
        errors++;
    
    ajStrDel(&key);
    
    if(errors)
        return ajFalse;
    
    return ajTrue;
}




/* @func ensMetaInformationAdaptorGetTaxonomyIdentifier ***********************
**
** Fetch the NCBI Taxonomy database identifier.
**
** @param [r] adaptor [const EnsPMetaInformationAdaptor] Ensembl
**                                                       Meta-Information
**                                                       Adaptor
** @param [wP] Pvalue [AjPStr*] Value String address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaInformationAdaptorGetTaxonomyIdentifier(
    const EnsPMetaInformationAdaptor adaptor,
    AjPStr *Pvalue)
{
    ajint errors = 0;
    
    AjPStr key = NULL;
    
    if(!adaptor)
        return ajFalse;
    
    if(!Pvalue)
        return ajFalse;
    
    if(!*Pvalue)
	return ajFalse;
    
    key = ajStrNewC("species.taxonomy_id");
    
    if(!ensMetaInformationAdaptorGetValueByKey(adaptor, key, Pvalue))
        errors++;
    
    ajStrDel(&key);
    
    if(errors)
	return ajFalse;
    
    return ajTrue;
}




/* @func ensMetaInformationAdaptorKeyValueExists ******************************
**
** Test whether an Ensembl Meta-Information objects with a particular
** key and value exists.
**
** @param [r] adaptor [const EnsPMetaInformationAdaptor] Ensembl
**                                                       Meta-Information
**                                                       Adaptor
** @param [r] key [const AjPStr] Key
** @param [r] value [const AjPStr] Value
**
** @return [AjBool] ajTrue if the Meta Information exists, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaInformationAdaptorKeyValueExists(
    const EnsPMetaInformationAdaptor adaptor,
    const AjPStr key,
    const AjPStr value)
{
    AjBool bool = AJFALSE;
    
    AjIList iter = NULL;
    AjPList list = NULL;
    
    EnsPMetaInformation mi = NULL;
    
    if(!adaptor)
        return ajFalse;
    
    if(!key)
        return ajFalse;
    
    if(!value)
        return ajFalse;
    
    list = (AjPList) ajTableFetch(adaptor->CacheByKey, (const void *) key);
    
    iter = ajListIterNew(list);
    
    while(!ajListIterDone(iter))
    {
	mi = (EnsPMetaInformation) ajListIterGet(iter);
	
	if(ajStrMatchS(mi->Value, value))
	{
	    bool = ajTrue;
	    
	    break;
	}
    }
    
    ajListIterDel(&iter);
    
    return bool;
}
