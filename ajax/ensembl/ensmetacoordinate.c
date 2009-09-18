/******************************************************************************
** @source Ensembl Meta-Coordinate functions
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

#include "ensmetacoordinate.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPCoordSystemAdaptor
ensRegistryGetCoordSystemAdaptor(EnsPDatabaseAdaptor dba);

static AjBool metaCoordinateAdaptorCacheInit(EnsPMetaCoordinateAdaptor adaptor);

static void metaCoordinateAdaptorClearNameCache(void **key,
                                                void **value, void *cl);

static void metaCoordinateAdaptorClearLengthCacheL2(void **key, void **value,
                                                    void *cl);

static void metaCoordinateAdaptorClearLengthCacheL1(void **key, void **value,
                                                    void *cl);




/* @filesection ensmetacoordinate *********************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPMetaCoordinateAdaptor] Meta-Coordinate Adaptor ***********
**
** Functions for manipulating Ensembl Meta-Coordinate Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::MetaCoordContainer CVS Revision: 1.7
**
** @nam2rule MetaCoordinateAdaptor
**
******************************************************************************/




/* @funcstatic metaCoordinateAdaptorCacheInit *********************************
**
** Initialise the internal Meta-Coordinate cache of an
** Ensembl Meta-Coordinate Adaptor.
**
** @param [u] adaptor [EnsPMetaCoordinateAdaptor] Ensembl Meta-Coordinate
**                                                Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool metaCoordinateAdaptorCacheInit(EnsPMetaCoordinateAdaptor adaptor)
{
    ajuint csid   = 0;
    ajuint length = 0;
    
    ajuint *Pcsid   = NULL;
    ajuint *Plength = NULL;
    
    AjPList list = NULL;
    
    AjPTable table = NULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr name      = NULL;
    AjPStr statement = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    statement = ajStrNewC("SELECT "
			  "meta_coord.table_name, "
			  "meta_coord.coord_system_id, "
			  "meta_coord.max_length "
			  "FROM "
			  "meta_coord");
    
    sqls = ensDatabaseAdaptorSqlStatementNew(adaptor->Adaptor, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	name = ajStrNew();
	
	csid = 0;
	
	length = 0;
	
        sqlr = ajSqlRowIterGet(sqli);
	
        ajSqlColumnToStr(sqlr, &name);
        ajSqlColumnToUint(sqlr, &csid);
        ajSqlColumnToUint(sqlr, &length);
	
	/* Add to the Feature Type Cache. */
	
	list = (AjPList)
	    ajTableFetch(adaptor->CacheByName, (const void *) name);
	
	if(!list)
	{
	    list = ajListNew();
	    
	    ajTablePut(adaptor->CacheByName,
		       (void *) ajStrNewS(name),
		       (void *) list);
	}
	
	AJNEW0(Pcsid);
	
	*Pcsid = csid;
	
	ajListPushAppend(list, (void *) Pcsid);
	
	/* Add to the Length Cache. */
	
	table = (AjPTable)
	    ajTableFetch(adaptor->CacheByLength, (const void *) &csid);
	
	if(!table)
	{
	    AJNEW0(Pcsid);
	    
	    *Pcsid = csid;
	    
	    table = ajTablestrNewCaseLen(0);
	    
	    ajTablePut(adaptor->CacheByLength, (void *) Pcsid, (void *) table);
	}
	
	AJNEW0(Plength);
	
	*Plength = length;
	
	ajTablePut(table, (void *) ajStrNewS(name), (void *) Plength);
	
	ajStrDel(&name);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Meta-Coordinate Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Meta-Coordinate Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMetaCoordinateAdaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPMetaCoordinateAdaptor] Ensembl Meta-Coordinate
**                                                 Adaptor
** @argrule Ref object [EnsPMetaCoordinateAdaptor] Ensembl Meta-Coordinate
**                                                 Adaptor
**
** @valrule * [EnsPMetaCoordinateAdaptor] Ensembl Meta-Coordinate Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensMetaCoordinateAdaptorNew ******************************************
**
** Default constructor for an Ensembl Meta-Coordinate Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::MetaCoordContainer::new
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPMetaCoordinateAdaptor] Ensembl Meta-Coordinate Adaptor or NULL
** @@
******************************************************************************/

EnsPMetaCoordinateAdaptor ensMetaCoordinateAdaptorNew(EnsPDatabaseAdaptor dba)
{
    EnsPMetaCoordinateAdaptor mca = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(mca);
    
    mca->Adaptor = dba;
    
    mca->CacheByName = ajTablestrNewCaseLen(0);
    
    mca->CacheByLength = MENSTABLEUINTNEW(0);
    
    metaCoordinateAdaptorCacheInit(mca);
    
    return mca;
}




/* @funcstatic metaCoordinateAdaptorClearNameCache ****************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Meta-Coordinate
** Adaptor-internal cache. This function deletes the AJAX String Feature name
** key data and the AJAX List value data, as well as the pointers to ajuint
** Coordinate System identifiers list elements.
**
** @param [u] key [void **] AJAX String key data address
** @param [u] value [void **] AJAX List value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void metaCoordinateAdaptorClearNameCache(void **key,
                                                void **value, void *cl)
{
    void *Pcsid = NULL;
    
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
    
    while(ajListPop(*((AjPList *) value), &Pcsid))
	AJFREE(Pcsid);
    
    ajListFree((AjPList *) value);
    
    return;
}




/* @funcstatic metaCoordinateAdaptorClearLengthCacheL2 ************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Meta-Coordinate
** Adaptor-internal maximum length cache. This function clears the
** second-level cache of Ensembl Feture name AJAX String key data and
** pointer to ajuint maximum length value data.
**
** @param [u] key [void **] AJAX String key data address
** @param [u] value [void **] AJAX unsigned integer value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void metaCoordinateAdaptorClearLengthCacheL2(void **key, void **value,
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
    
    AJFREE(*value);
    
    return;
}




/* @funcstatic metaCoordinateAdaptorClearLengthCacheL1 ************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Meta-Coordinate
** Adaptor-internal maximum length cache. This function clears the
** first-level cache of ajuint Coordinate System identifier key data and the
** second-level AJAX Table value data.
**
** @param [u] key [void **] AJAX unsigned integer key data address
** @param [u] value [void **] AJAX Table value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void metaCoordinateAdaptorClearLengthCacheL1(void **key, void **value,
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
    
    ajTableMapDel(*((AjPTable *) value),
		  metaCoordinateAdaptorClearLengthCacheL2,
		  NULL);
    
    ajTableFree((AjPTable *) value);
    
    return;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Meta-Coordinate Adaptor.
**
** @fdata [EnsPMetaCoordinateAdaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Meta-Coordinate Adaptor object
**
** @argrule * Padaptor [EnsPMetaCoordinateAdaptor*] Ensembl Meta-Coordinate
**                                                  Adaptor object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMetaCoordinateAdaptorDel ******************************************
**
** Default destructor for an Ensembl Meta-Coordinate Adaptor.
** This function also clears the internal cordinate system identifer and
** maximum length caches.
**
** @param [d] Padaptor [EnsPMetaCoordinateAdaptor*] Ensembl Meta-Coordinate
**                                                  Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensMetaCoordinateAdaptorDel(EnsPMetaCoordinateAdaptor *Padaptor)
{
    EnsPMetaCoordinateAdaptor pthis = NULL;
    
    if(!Padaptor)
	return;
    
    if(!*Padaptor)
	return;

    pthis = *Padaptor;
    
    ajTableMapDel(pthis->CacheByName,
		  metaCoordinateAdaptorClearNameCache,
		  NULL);
    
    ajTableFree(&pthis->CacheByName);
    
    ajTableMapDel(pthis->CacheByLength,
		  metaCoordinateAdaptorClearLengthCacheL1,
		  NULL);
    
    ajTableFree(&pthis->CacheByLength);
    
    AJFREE(pthis);

    *Padaptor = NULL;
    
    return;
}




/* @func ensMetaCoordinateAdaptorFetchAllCoordSystems *************************
**
** Fetch all Ensembl Coordinate Systems associated with a particular
** Ensembl Feature name.
**
** @cc Bio::EnsEMBL::DBSQL::MetaCoordContainer::
**     fetch_all_CoordSystems_by_feature_type
** @param [r] adaptor [EnsPMetaCoordinateAdaptor] Ensembl Meta-Coordinate
**                                                Adaptor
** @param [r] name [const AjPStr] Ensembl Feature (i.e. SQL table) name
** @param [u] cslist [AjPList] AJAX List of Ensembl Coordinate Systems
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMetaCoordinateAdaptorFetchAllCoordSystems(
    EnsPMetaCoordinateAdaptor adaptor,
    const AjPStr name,
    AjPList cslist)
{
    ajuint *Pcsid = NULL;
    
    AjIList iter = NULL;
    AjPList list = NULL;
    
    EnsPCoordSystem cs = NULL;
    EnsPCoordSystemAdaptor csa = NULL;
    
    /*
     ajDebug("ensMetaCoordinateAdaptorFetchAllCoordSystems\n"
	     "  adaptor %p\n"
	     "  name '%S'\n"
	     "  cslist %p\n",
	     adaptor,
	     name,
	     cslist);
     */
    
    if(!adaptor)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    list = (AjPList) ajTableFetch(adaptor->CacheByName, (const void *) name);
    
    if(!list)
    {
	ajDebug("ensMetaCoordinateAdaptorFetchAllCoordSystems "
		"got no Coordinate System List for name '%S'\n", name);
	
	return ajTrue;
    }
    
    csa = ensRegistryGetCoordSystemAdaptor(adaptor->Adaptor);
    
    iter = ajListIterNew(list);
    
    while(!ajListIterDone(iter))
    {
	Pcsid = (ajuint *) ajListIterGet(iter);
	
	ensCoordSystemAdaptorFetchByIdentifier(csa, *Pcsid, &cs);
	
	if(cs)
	    ajListPush(cslist, (void *) cs);
	else
	    ajFatal("ensMetaCoordinateAdaptorFetchAllCoordSystems "
		    "Ensembl Core 'meta_coord' table refers to non-existing "
		    "Ensembl Coordinate System with identifier %u\n", *Pcsid);
    }
    
    ajListIterDel(&iter);
    
    return ajTrue;
}




/* @func ensMetaCoordinateAdaptorGetMaximumLength *****************************
**
** Get the maximum length of a particular Ensembl Feature type in an
** Ensembl Coordinate System.
**
** @cc Bio::EnsEMBL::DBSQL::MetaCoordContainer::
**     fetch_max_length_by_CoordSystem_feature_type
** @param [r] adaptor [EnsPMetaCoordinateAdaptor] Ensembl Meta-Coordinate
**                                                Adaptor
** @param [r] cs [const EnsPCoordSystem] Ensembl Coordinate System
** @param [r] name [const AjPStr] Ensembl Feature (i.e. SQL table) name
**
** @return [ajuint] Maximum length
** @@
******************************************************************************/

ajuint ensMetaCoordinateAdaptorGetMaximumLength(
    EnsPMetaCoordinateAdaptor adaptor,
    const EnsPCoordSystem cs,
    const AjPStr name)
{
    ajuint *Pcsid = NULL;
    ajuint *Plength = NULL;
    
    AjPTable table = NULL;
    
    AJNEW0(Pcsid);
    
    *Pcsid = ensCoordSystemGetIdentifier(cs);
    
    table = (AjPTable)
	ajTableFetch(adaptor->CacheByLength, (const void *) Pcsid);
    
    AJFREE(Pcsid);
    
    if(table)
	Plength = (ajuint *) ajTableFetch(table, (const void *) name);
    else
	return 0;
    
    return *Plength;
}
