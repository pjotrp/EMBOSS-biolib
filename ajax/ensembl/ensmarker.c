/******************************************************************************
** @source Ensembl Marker functions
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

#include "ensmarker.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

static const char *markerType[] =
{
    NULL,
    "est",
    "microsatellite",
    NULL
};




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPAnalysisAdaptor
ensRegistryGetAnalysisAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPAssemblyMapperAdaptor
ensRegistryGetAssemblyMapperAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPMarkerAdaptor
ensRegistryGetMarkerAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPMarkerFeatureAdaptor
ensRegistryGetMarkerFeatureAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPSliceAdaptor
ensRegistryGetSliceAdaptor(EnsPDatabaseAdaptor dba);

static AjBool markerSynonymAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                                const AjPStr statement,
                                                AjPList mslist);

static AjBool markerMapLocationAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                                    const AjPStr statement,
                                                    AjPList mmlist);

static AjBool markerAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                         const AjPStr statement,
                                         EnsPAssemblyMapper am,
                                         EnsPSlice slice,
                                         AjPList markers);

static void markerAdaptorClearIdentifierTable(void **key, void **value,
                                              void *cl);

static void markerFeatureAdaptorClearMarkerTable(void **key, void **value,
                                                 void *cl);

static AjBool markerFeatureAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                                const AjPStr statement,
                                                EnsPAssemblyMapper am,
                                                EnsPSlice slice,
                                                AjPList mflist);

static void *markerFeatureAdaptorCacheReference(void *value);

static void markerFeatureAdaptorCacheDelete(void **value);

static ajuint markerFeatureAdaptorCacheSize(const void *value);

static EnsPFeature markerFeatureAdaptorGetFeature(const void *value);




/* @filesection ensmarker *****************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPMarkerSynonym] Marker Synonym ****************************
**
** Functions for manipulating Ensembl Marker Synonym objects
**
** @cc Bio::EnsEMBL::Map::MarkerSynonym CVS Revision: 1.2
**
** @nam2rule MarkerSynonym
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Marker Synonym by pointer.
** It is the responsibility of the user to first destroy any previous
** Marker Synonyms. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMarkerSynonym]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMarkerSynonym] Ensembl Marker Synonym
** @argrule Ref object [EnsPMarkerSynonym] Ensembl Marker Synonym
**
** @valrule * [EnsPMarkerSynonym] Ensembl Marker Synonym
**
** @fcategory new
******************************************************************************/




/* @func ensMarkerSynonymNew **************************************************
**
** Default constructor for an Ensembl Marker Synonym.
**
** @cc Bio::EnsEMBL::Map::MarkerSynonym::new
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [u] source [AjPStr] Source
** @param [u] name [AjPStr] Name
**
** @return [EnsPMarkerSynonym] Ensembl Marker Synonym or NULL
** @@
******************************************************************************/

EnsPMarkerSynonym ensMarkerSynonymNew(ajuint identifier,
                                      AjPStr source,
                                      AjPStr name)
{
    EnsPMarkerSynonym ms = NULL;
    
    AJNEW0(ms);
    
    ms->Use = 1;
    
    ms->Identifier = identifier;
    
    if(source)
        ms->Source = ajStrNewRef(source);
    
    if(name)
        ms->Name = ajStrNewRef(name);
    
    return ms;
}




/* @func ensMarkerSynonymNewObj ***********************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPMarkerSynonym] Ensembl Marker Synonym
**
** @return [EnsPMarkerSynonym] Ensembl Marker Synonym or NULL
** @@
******************************************************************************/

EnsPMarkerSynonym ensMarkerSynonymNewObj(const EnsPMarkerSynonym object)
{
    EnsPMarkerSynonym ms = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(ms);
    
    ms->Use = 1;
    
    ms->Identifier = object->Identifier;
    
    if(object->Source)
        ms->Source = ajStrNewRef(object->Source);
    
    if(object->Name)
        ms->Name = ajStrNewRef(object->Name);
    
    return ms;
}




/* @func ensMarkerSynonymNewRef ***********************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] ms [EnsPMarkerSynonym] Ensembl Marker Synonym
**
** @return [EnsPMarkerSynonym] Ensembl Marker Synonym or NULL
** @@
******************************************************************************/

EnsPMarkerSynonym ensMarkerSynonymNewRef(EnsPMarkerSynonym ms)
{
    if(!ms)
	return NULL;
    
    ms->Use++;
    
    return ms;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Marker Synonym.
**
** @fdata [EnsPMarkerSynonym]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Marker Synonym object
**
** @argrule * Pms [EnsPMarkerSynonym*] Ensembl Marker Synonym object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMarkerSynonymDel **************************************************
**
** Default destructor for an Ensembl Marker Synonym.
**
** @param [d] Pms [EnsPMarkerSynonym*] Ensembl Marker Synonym address
**
** @return [void]
** @@
******************************************************************************/

void ensMarkerSynonymDel(EnsPMarkerSynonym *Pms)
{
    EnsPMarkerSynonym pthis = NULL;
    
    /*
     ajDebug("ensMarkerSynonymDel\n"
	     "  *Pms %p\n",
	     *Pms);
     
     ensMarkerSynonymTrace(*Pms, 1);
     */
    
    if(!Pms)
        return;
    
    if(!*Pms)
        return;

    pthis = *Pms;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pms = NULL;
	
	return;
    }
    
    ajStrDel(&pthis->Source);
    ajStrDel(&pthis->Name);
    
    AJFREE(pthis);

    *Pms = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Marker Synonym object.
**
** @fdata [EnsPMarkerSynonym]
** @fnote None
**
** @nam3rule Get Return Marker Synonym attribute(s)
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetSource Return the source
** @nam4rule GetName Return the name
**
** @argrule * ms [const EnsPMarkerSynonym] Marker Synonym
**
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Source [AjPStr] Source
** @valrule Name [AjPStr] Name
**
** @fcategory use
******************************************************************************/




/* @func ensMarkerSynonymGetIdentifier ****************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Marker Synonym.
**
** @cc Bio::EnsEMBL::Map::MarkerSynonym::dbID
** @param [r] ms [const EnsPMarkerSynonym] Ensembl Marker Synonym
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensMarkerSynonymGetIdentifier(const EnsPMarkerSynonym ms)
{
    if(!ms)
        return 0;
    
    return ms->Identifier;
}




/* @func ensMarkerSynonymGetSource ********************************************
**
** Get the source element of an Ensembl Marker Synonym.
**
** @cc Bio::EnsEMBL::Map::MarkerSynonym::source
** @param [r] ms [const EnsPMarkerSynonym] Ensembl Marker Synonym
**
** @return [AjPStr] Source
** @@
******************************************************************************/

AjPStr ensMarkerSynonymGetSource(const EnsPMarkerSynonym ms)
{
    if(!ms)
        return NULL;
    
    return ms->Source;
}




/* @func ensMarkerSourceGetName ***********************************************
**
** Get the name element of an Ensembl Marker Synonym.
**
** @cc Bio::EnsEMBL::Map::MarkerSynonym::name
** @param  [r] ms [const EnsPMarkerSynonym] Ensembl Marker Synonym
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensMarkerSynonymGetName(const EnsPMarkerSynonym ms)
{
    if(!ms)
        return NULL;
    
    return ms->Name;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Marker Synonym object.
**
** @fdata [EnsPMarkerSynonym]
** @fnote None
**
** @nam3rule Set Set one element of an Ensembl Marker Synonym
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetSource Set the source
** @nam4rule SetName Set the name
**
** @argrule * ms [EnsPMarkerSynonym] Ensembl Marker Synonym object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensMarkerSynonymSetIdentifier ****************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Marker Synonym.
**
** @cc Bio::EnsEMBL::Map::MarkerSynonym::dbID
** @param [u] ms [EnsPMarkerSynonym] Ensembl Marker Synonym
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSynonymSetIdentifier(EnsPMarkerSynonym ms, ajuint identifier)
{
    if(!ms)
        return ajFalse;
    
    ms->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensMarkerSynonymSetSource ********************************************
**
** Set the source element of an Ensembl Marker Synonym.
**
** @cc Bio::EnsEMBL::Map::MarkerSynonym::source
** @param [u] ms [EnsPMarkerSynonym] Ensembl Marker Synonym
** @param [u] synonym [AjPStr] Synonym
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSynonymSetSource(EnsPMarkerSynonym ms, AjPStr source)
{
    if(!ms)
        return ajFalse;
    
    ajStrDel(&ms->Source);
    
    ms->Source = ajStrNewRef(source);
    
    return ajTrue;
}




/* @func ensMarkerSynonymSetName **********************************************
**
** Set the name element of an Ensembl Marker Synonym.
**
** @cc Bio::EnsEMBL::Map::MarkerSynonym::name
** @param [u] ms [EnsPMarkerSynonym] Ensembl Marker Synonym
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSynonymSetName(EnsPMarkerSynonym ms, AjPStr name)
{
    if(!ms)
        return ajFalse;
    
    ajStrDel(&ms->Name);
    
    ms->Name = ajStrNewRef(name);
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Marker Synonym object.
**
** @fdata [EnsPMarkerSynonym]
** @nam3rule Trace Report Ensembl Marker Synonym elements to debug file.
**
** @argrule Trace ms [const EnsPMarkerSynonym] Ensembl Marker Synonym
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensMarkerSynonymTrace ************************************************
**
** Trace an Ensembl Marker Synonym.
**
** @param [r] ms [const EnsPMarkerSynonym] Ensembl Marker Synonym
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSynonymTrace(const EnsPMarkerSynonym ms, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!ms)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensMarkerSynonymTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Source '%S'\n"
	    "%S  Name '%S'\n",
	    indent, ms,
	    indent, ms->Use,
	    indent, ms->Identifier,
	    indent, ms->Source,
	    indent, ms->Name);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensMarkerSynonymGetMemSize *******************************************
**
** Get the memory size in bytes of an Ensembl Marker Synonym.
**
** @param [r] ms [const EnsPMarkerSynonym] Ensembl Marker Synonym
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensMarkerSynonymGetMemSize(const EnsPMarkerSynonym ms)
{
    ajuint size = 0;
    
    if(!ms)
	return 0;
    
    size += (ajuint) sizeof (EnsOMarkerSynonym);
    
    if(ms->Source)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(ms->Source);
    }
    
    if(ms->Name)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(ms->Name);
    }
    
    return size;
}




/* @datasection [EnsPDatabaseAdaptor] Ensembl Database Adaptor ****************
**
** Functions for manipulating Ensembl Marker Synonym Adaptor objects
**
** @nam2rule MarkerSynonymAdaptor
**
******************************************************************************/




/* @funcstatic markerSynonymAdaptorFetchAllBySQL ******************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl Marker Synonym objects.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] mslist [AjPList] AJAX List of Ensembl Marker Synonyms
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool markerSynonymAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                                const AjPStr statement,
                                                AjPList mslist)
{
    ajuint identifier = 0;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr name   = NULL;
    AjPStr source = NULL;
    
    EnsPMarkerSynonym ms = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!(statement && ajStrGetLen(statement)))
	return ajFalse;
    
    if(!mslist)
	return ajFalse;
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	identifier = 0;
	name = ajStrNew();
	source = ajStrNew();
	
        sqlr = ajSqlRowIterGet(sqli);
	
        ajSqlColumnToUint(sqlr, &identifier);
	ajSqlColumnToStr(sqlr, &name);
	ajSqlColumnToStr(sqlr, &source);
	
	ms = ensMarkerSynonymNew(identifier, source, name);
	
	ajListPushAppend(mslist, (void *) ms);
	
	ajStrDel(&name);
	ajStrDel(&source);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    return ajTrue;
}




/* @func ensMarkerSynonymAdaptorFetchByIdentifier *****************************
**
** Fetch an Ensembl Marker Synonym via its SQL database-internal identifier.
**
** The caller is responsible for deleting the Ensembl Marker Synonym.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pms [EnsPMarkerSynonym*] Ensembl Marker Synonym address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSynonymAdaptorFetchByIdentifier(EnsPDatabaseAdaptor dba,
                                                ajuint identifier,
                                                EnsPMarkerSynonym *Pms)
{
    AjPList mslist = NULL;
    
    AjPStr statement = NULL;
    
    EnsPMarkerSynonym ms = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pms)
	return ajFalse;
    
    *Pms = NULL;
    
    statement = ajFmtStr("SELECT "
			 "marker_synonym.marker_synonym_id, "
			 "marker_synonym.source, "
			 "marker_synonym.name "
			 "FROM "
			 "marker_synonym "
			 "WHERE "
			 "marker_synonym.marker_synonym_id = %u",
			 identifier);
    
    mslist = ajListNew();
    
    markerSynonymAdaptorFetchAllBySQL(dba, statement, mslist);
    
    if(ajListGetLength(mslist) > 1)
	ajWarn("ensMarkerSynonymAdaptorFetchByIdentifier got more than one "
	       "Ensembl Marker Synonym for identifier %u.\n",
	       identifier);
    
    ajListPop(mslist, (void **) Pms);
    
    while(ajListPop(mslist, (void **) &ms))
	ensMarkerSynonymDel(&ms);
    
    ajListFree(&mslist);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensMarkerSynonymAdaptorFetchAllByMarkerIdentifier ********************
**
** Fetch all Ensembl Marker Synonyms by an Ensembl Marker identifier.
**
** The caller is responsible for deleting the Ensembl Marker Synonyms before
** deleting the AJAX List.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] markerid [ajuint] Ensembl Marker identifier
** @param [w] mslist [AjPList] AJAX List of Ensembl Marker Synonyms
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSynonymAdaptorFetchAllByMarkerIdentifier(
    EnsPDatabaseAdaptor dba,
    ajuint markerid,
    AjPList mslist)
{
    AjBool value = AJFALSE;
    
    AjPStr statement = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!mslist)
	return ajFalse;
    
    statement = ajFmtStr("SELECT "
			 "marker_synonym.marker_synonym_id, "
			 "marker_synonym.source, "
			 "marker_synonym.name "
			 "FROM "
			 "marker_synonym "
			 "WHERE "
			 "marker_synonym.marker_id = %u",
			 markerid);
    
    value = markerSynonymAdaptorFetchAllBySQL(dba, statement, mslist);
    
    ajStrDel(&statement);
    
    return value;
}




/* @datasection [EnsPMarkerMapLocation] Marker Map Location *******************
**
** Functions for manipulating Ensembl Marker Map Location objects
**
** @cc Bio::EnsEMBL::Map::MapLocation CVS Revision: 1.3
**
** @nam2rule MarkerMapLocation
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Marker Map Location by pointer.
** It is the responsibility of the user to first destroy any previous
** Marker Map Locations. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMarkerMapLocation]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMarkerMapLocation] Ensembl Marker Map Location
** @argrule Ref object [EnsPMarkerMapLocation] Ensembl Marker Map Location
**
** @valrule * [EnsPMarkerMapLocation] Ensembl Marker Map Location
**
** @fcategory new
******************************************************************************/




/* @func ensMarkerMapLocationNew **********************************************
**
** Default constructor for an Ensembl Marker Map Location.
**
** @cc Bio::EnsEMBL::Map::MapLocation::new
** @param [u] ms [EnsPMarkerSynonym] Ensembl Marker Synonym
** @param [u] mapname [AjPStr] Map name
** @param [u] chrname [AjPStr] Chromosome name
** @param [u] position [AjPStr] Position
** @param [r] lodscore [float] LOD score
**
** @return [EnsPMarkerMapLocation] Ensembl Marker Map Location or NULL
** @@
******************************************************************************/

EnsPMarkerMapLocation ensMarkerMapLocationNew(EnsPMarkerSynonym ms,
                                              AjPStr mapname,
                                              AjPStr chrname,
                                              AjPStr position,
                                              float lodscore)
{
    EnsPMarkerMapLocation mml = NULL;
    
    AJNEW0(mml);
    
    mml->MarkerSynonym = ensMarkerSynonymNewRef(ms);
    
    if(mapname)
        mml->MapName = ajStrNewRef(mapname);
    
    if(chrname)
	mml->ChromosomeName = ajStrNewRef(chrname);
    
    if(position)
	mml->Position = ajStrNewRef(position);
    
    mml->LODScore = lodscore;
    
    mml->Use = 1;
    
    return mml;
}




/* @func ensMarkerMapLocationNewObj *******************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPMarkerMapLocation] Ensembl Marker Map Location
**
** @return [EnsPMarkerMapLocation] Ensembl Marker Map Location or NULL
** @@
******************************************************************************/

EnsPMarkerMapLocation ensMarkerMapLocationNewObj(
    const EnsPMarkerMapLocation object)
{
    EnsPMarkerMapLocation mml = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(mml);
    
    mml->MarkerSynonym = ensMarkerSynonymNewRef(object->MarkerSynonym);
    
    if(object->MapName)
        mml->MapName = ajStrNewRef(object->MapName);
    
    if(object->ChromosomeName)
        mml->ChromosomeName = ajStrNewRef(object->ChromosomeName);
    
    if(object->Position)
	mml->Position = ajStrNewRef(object->Position);
    
    mml->LODScore = object->LODScore;
    
    mml->Use = 1;
    
    return mml;
}




/* @func ensMarkerMapLocationNewRef *******************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] mml [EnsPMarkerMapLocation] Ensembl Marker Map Location
**
** @return [EnsPMarkerMapLocation] Ensembl Marker Map Location or NULL
** @@
******************************************************************************/

EnsPMarkerMapLocation ensMarkerMapLocationNewRef(EnsPMarkerMapLocation mml)
{
    if(!mml)
	return NULL;
    
    mml->Use++;
    
    return mml;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Marker Map Location.
**
** @fdata [EnsPMarkerMapLocation]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Marker Map Location object
**
** @argrule * Pmml [EnsPMarkerMapLocation*] Ensembl Marker Map Location
**                                          object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMarkerMapLocationDel **********************************************
**
** Default destructor for an Ensembl Marker Map Location.
**
** @param [d] Pmml [EnsPMarkerMapLocation*] Ensembl Marker Map Location address
**
** @return [void]
** @@
******************************************************************************/

void ensMarkerMapLocationDel(EnsPMarkerMapLocation *Pmml)
{
    EnsPMarkerMapLocation pthis = NULL;
    
    /*
     ajDebug("ensMarkerMapLocationDel\n"
	     "  *Pmml %p\n",
	     *Pmml);
     
     ensMarkerMapLocationTrace(*Pmml, 1);
     */
    
    if(!Pmml)
        return;
    
    if(!*Pmml)
        return;

    pthis = *Pmml;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pmml = NULL;
	
	return;
    }
    
    ensMarkerSynonymDel(&pthis->MarkerSynonym);
    
    ajStrDel(&pthis->MapName);
    ajStrDel(&pthis->ChromosomeName);
    ajStrDel(&pthis->Position);
    
    AJFREE(pthis);

    *Pmml = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Marker Map Location object.
**
** @fdata [EnsPMarkerMapLocation]
** @fnote None
**
** @nam3rule Get Return Marker Map Location attribute(s)
** @nam4rule GetMarkerSynonym Return the Ensembl Marker Synonym
** @nam4rule GetMapName Return the map name
** @nam4rule GetChromosomeName Return the chromosome name
** @nam4rule GetPosition Return the position
** @nam4rule GetLODScore Return the LOD score
**
** @argrule * mml [const EnsPMarkerMapLocation] Marker Map Location
**
** @valrule MarkerSynonym [EnsPMarkerSynonym] Ensembl Marker Synonym
** @valrule MapName [AjPStr] Map name
** @valrule ChromosomeName [AjPStr] Chromosome name
** @valrule Position [AjPStr] Position
** @valrule LODScore [float] LOD score
**
** @fcategory use
******************************************************************************/




/* @func ensMarkerMapLocationGetMarkerSynonym *********************************
**
** Get the Ensembl Marker Synonym element of an Ensembl Marker Map Location.
**
** @cc Bio::EnsEMBL::Map::MapLocation::name
** @param  [r] mml [const EnsPMarkerMapLocation] Ensembl Marker Map Location
**
** @return [EnsPMarkerSynonym] Ensembl Marker Synonym
** @@
******************************************************************************/

EnsPMarkerSynonym ensMarkerMapLocationGetMarkerSynonym(
    const EnsPMarkerMapLocation mml)
{
    if(!mml)
        return NULL;
    
    return mml->MarkerSynonym;
}




/* @func ensMarkerMapLocationGetMapName ***************************************
**
** Get the map name element of an Ensembl Marker Map Location.
**
** @cc Bio::EnsEMBL::Map::MapLocation::map_name
** @param  [r] mml [const EnsPMarkerMapLocation] Ensembl Marker Map Location
**
** @return [AjPStr] Map name
** @@
******************************************************************************/

AjPStr ensMarkerMapLocationGetMapName(const EnsPMarkerMapLocation mml)
{
    if(!mml)
        return NULL;
    
    return mml->MapName;
}




/* @func ensMarkerMapLocationGetChromosomeName ********************************
**
** Get the chromosome name element of an Ensembl Marker Map Location.
**
** @cc Bio::EnsEMBL::Map::MapLocation::chromosome_name
** @param  [r] mml [const EnsPMarkerMapLocation] Ensembl Marker Map Location
**
** @return [AjPStr] Chromosome name
** @@
******************************************************************************/

AjPStr ensMarkerMapLocationGetChromosomeName(const EnsPMarkerMapLocation mml)
{
    if(!mml)
        return NULL;
    
    return mml->ChromosomeName;
}




/* @func ensMarkerMapLocationGetPosition **************************************
**
** Get the position element of an Ensembl Marker Map Location.
**
** @cc Bio::EnsEMBL::Map::MapLocation::position
** @param  [r] mml [const EnsPMarkerMapLocation] Ensembl Marker Map Location
**
** @return [AjPStr] Position
** @@
******************************************************************************/

AjPStr ensMarkerMapLocationGetPosition(const EnsPMarkerMapLocation mml)
{
    if(!mml)
        return NULL;
    
    return mml->Position;
}




/* @func ensMarkerMapLocationGetLODScore **************************************
**
** Get the LOD score element of an Ensembl Marker Map Location.
**
** @cc Bio::EnsEMBL::Map::MapLocation::lod_score
** @param [r] mml [const EnsPMarkerMapLocation] Ensembl Marker Map Location
**
** @return [float] LOD score
** @@
******************************************************************************/

float ensMarkerMapLocationGetLODScore(const EnsPMarkerMapLocation mml)
{
    if(!mml)
        return 0;
    
    return mml->LODScore;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Marker Map Location object.
**
** @fdata [EnsPMarkerMapLocation]
** @fnote None
**
** @nam3rule Set Set one element of an Ensembl Marker Map Location
** @nam4rule SetMarkerSynonym Set the Ensembl Marker Synonym
** @nam4rule SetMapName Set the map name
** @nam4rule SetChromosomeName Set the chromosome name
** @nam4rule SetPosition Set the position
** @nam4rule SetLODScore Set the LOD score
**
** @argrule * mml [EnsPMarkerMapLocation] Ensembl Marker Map Location object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensMarkerMapLocationSetMarkerSynonym *********************************
**
** Set the Ensembl Marker Synonym element of an Ensembl Marker Map Location.
**
** @cc Bio::EnsEMBL::Map::MapLocation::name
** @param [u] mml [EnsPMarkerMapLocation] Ensembl Marker Map Location
** @param [u] ms [EnsPMarkerSynonym] Ensembl Marker Synonym
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerMapLocationSetMarkerSynonym(EnsPMarkerMapLocation mml,
                                            EnsPMarkerSynonym ms)
{
    if(!mml)
        return ajFalse;
    
    ensMarkerSynonymDel(&(mml->MarkerSynonym));
    
    mml->MarkerSynonym = ensMarkerSynonymNewRef(ms);
    
    return ajTrue;
}




/* @func ensMarkerMapLocationSetMapName ***************************************
**
** Set the map name element of an Ensembl Marker Map Location.
**
** @cc Bio::EnsEMBL::Map::MapLocation::map_name
** @param [u] mml [EnsPMarkerMapLocation] Ensembl Marker Map Location
** @param [u] mapname [AjPStr] Map name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerMapLocationSetMapName(EnsPMarkerMapLocation mml, AjPStr mapname)
{
    if(!mml)
        return ajFalse;
    
    ajStrDel(&mml->MapName);
    
    mml->MapName = ajStrNewRef(mapname);
    
    return ajTrue;
}




/* @func ensMarkerMapLocationSetChromosomeName ********************************
**
** Set the chromosome name element of an Ensembl Marker Map Location.
**
** @cc Bio::EnsEMBL::Map::MapLocation::chromosome_name
** @param [u] mml [EnsPMarkerMapLocation] Ensembl Marker Map Location
** @param [u] chrname [AjPStr] Chromosome name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerMapLocationSetChromosomeName(EnsPMarkerMapLocation mml,
                                             AjPStr chrname)
{
    if(!mml)
        return ajFalse;
    
    ajStrDel(&mml->ChromosomeName);
    
    mml->ChromosomeName = ajStrNewRef(chrname);
    
    return ajTrue;
}




/* @func ensMarkerMapLocationSetPosition **************************************
**
** Set the position element of an Ensembl Marker Map Location.
**
** @cc Bio::EnsEMBL::Map::MapLocation::position
** @param [u] mml [EnsPMarkerMapLocation] Ensembl Marker Map Location
** @param [u] position [AjPStr] Position
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerMapLocationSetPosition(EnsPMarkerMapLocation mml,
                                       AjPStr position)
{
    if(!mml)
        return ajFalse;
    
    ajStrDel(&mml->Position);
    
    mml->Position = ajStrNewRef(position);
    
    return ajTrue;
}




/* @func ensMarkerMapLocationSetLODScore **************************************
**
** Set the LOD score element of an Ensembl Marker Map Location.
**
** @cc Bio::EnsEMBL::Map::MapLocation::lod_score
** @param [u] mml [EnsPMarkerMapLocation] Ensembl Marker Map Location
** @param [r] lodscore [float] LOD score
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerMapLocationSetLODScore(EnsPMarkerMapLocation mml,
                                       float lodscore)
{
    if(!mml)
        return ajFalse;
    
    mml->LODScore = lodscore;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Marker Map Location object.
**
** @fdata [EnsPMarkerMapLocation]
** @nam3rule Trace Report Ensembl Marker Map Location elements to debug file.
**
** @argrule Trace mml [const EnsPMarkerMapLocation] Ensembl Marker Map Location
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensMarkerMapLocationTrace ********************************************
**
** Trace an Ensembl Marker Map Location.
**
** @param [r] mml [const EnsPMarkerMapLocation] Ensembl Marker Map Location
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerMapLocationTrace(const EnsPMarkerMapLocation mml, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!mml)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensMarkerMapLocationTrace %p\n"
	    "%S  MarkerSynonym %p\n"
	    "%S  MapName '%S'\n"
	    "%S  ChromosomeName '%S'\n"
	    "%S  Position '%S'\n"
	    "%S  LODScore %f\n"
	    "%S  Use %u\n",
	    indent, mml,
	    indent, mml->Use,
	    indent, mml->MarkerSynonym,
	    indent, mml->MapName,
	    indent, mml->ChromosomeName,
	    indent, mml->LODScore);
    
    ensMarkerSynonymTrace(mml->MarkerSynonym, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensMarkerMapLocationGetMemSize ***************************************
**
** Get the memory size in bytes of an Ensembl Marker Map Location.
**
** @param [r] mml [const EnsPMarkerMapLocation] Ensembl Marker Map Location
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensMarkerMapLocationGetMemSize(const EnsPMarkerMapLocation mml)
{
    ajuint size = 0;
    
    if(!mml)
	return 0;
    
    size += (ajuint) sizeof (EnsOMarkerMapLocation);
    
    size += ensMarkerSynonymGetMemSize(mml->MarkerSynonym);
    
    if(mml->MapName)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(mml->MapName);
    }
    
    if(mml->ChromosomeName)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(mml->ChromosomeName);
    }
    
    if(mml->Position)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(mml->Position);
    }
    
    return size;
}




/* @datasection [EnsPDatabaseAdaptor] Database Adaptor ************************
**
** Functions for manipulating Ensembl Marker Map Location Adaptor objects
**
** @nam2rule MarkerMapLocationAdaptor
**
******************************************************************************/




/* @funcstatic markerMapLocationAdaptorFetchAllBySQL **************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl Marker Map Location objects.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] mmlist [AjPList] AJAX List of Ensembl Marker Map Locations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool markerMapLocationAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                                    const AjPStr statement,
                                                    AjPList mmlist)
{
    float lodscore = 0;
    
    ajuint msid = 0;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr msname   = NULL;
    AjPStr mssource = NULL;
    AjPStr chrname  = NULL;
    AjPStr position = NULL;
    AjPStr mapname  = NULL;
    
    EnsPMarkerSynonym ms = NULL;
    
    EnsPMarkerMapLocation mml = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    if(!mmlist)
	return ajFalse;
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
        chrname  = ajStrNew();
	position = ajStrNew();
	lodscore = 0;
	mapname  = ajStrNew();
	msid     = 0;
	mssource = ajStrNew();
	msname   = ajStrNew();
	
	sqlr = ajSqlRowIterGet(sqli);
	
	ajSqlColumnToStr(sqlr, &chrname);
	ajSqlColumnToStr(sqlr, &position);
	ajSqlColumnToFloat(sqlr, &lodscore);
	ajSqlColumnToStr(sqlr, &mapname);
	ajSqlColumnToUint(sqlr, &msid);
	ajSqlColumnToStr(sqlr, &mssource);
	ajSqlColumnToStr(sqlr, &msname);
	
	ms = ensMarkerSynonymNew(msid, mssource, msname);
	
	mml = ensMarkerMapLocationNew(ms,
				      mapname,
				      chrname,
				      position,
				      lodscore);
	
	ajListPushAppend(mmlist, mml);
	
	ensMarkerSynonymDel(&ms);
	
	ajStrDel(&chrname);
	ajStrDel(&position);
	ajStrDel(&mapname);
	ajStrDel(&msname);
	ajStrDel(&mssource);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    return ajTrue;
}




/* @func ensMarkerMapLocationAdaptorFetchAllByMarkerIdentifier ****************
**
** Fetch all Ensembl Marker Map Locations by an Ensembl Marker identifier.
**
** The caller is responsible for deleting the Ensembl Marker Map Locations
** before deleting the AJAX List.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] markerid [ajuint] Ensembl Marker identifier
** @param [w] mmlist [AjPList] AJAX List of Ensembl Marker Map Locations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerMapLocationAdaptorFetchAllByMarkerIdentifier(
    EnsPDatabaseAdaptor dba,
    ajuint markerid,
    AjPList mmlist)
{
    AjBool value = AJFALSE;
    
    AjPStr statement = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!markerid)
	return ajFalse;
    
    if(!mmlist)
	return ajFalse;
    
    statement = ajFmtStr("SELECT "
			 "marker_map_location.chromosome_name, "
			 "marker_map_location.position, "
			 "marker_map_location.lod_score, "
			 "map.map_name, "
			 "marker_synonym.marker_synonym_id, "
			 "marker_synonym.source, "
			 "marker_synonym.name, "
			 "FROM "
			 "marker_map_location, "
			 "map, "
			 "marker_synonym "
			 "WHERE "
			 "marker_map_location.marker_id = %u "
			 "AND "
			 "marker_map_location.map_id = map.map_id "
			 "AND "
			 "marker_map_location.marker_synonym_id = "
			 "marker_synonym.marker_synonym_id",
			 markerid);
			 
    value = markerMapLocationAdaptorFetchAllBySQL(dba, statement, mmlist);
    
    ajStrDel(&statement);
    
    return value;
}




/* @datasection [EnsPMarker] Marker *******************************************
**
** Functions for manipulating Ensembl Marker objects
**
** @cc Bio::EnsEMBL::Map::Marker CVS Revision: 1.6
**
** @nam2rule Marker
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Marker by pointer.
** It is the responsibility of the user to first destroy any previous
** Markers. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMarker]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMarker] Ensembl Marker
** @argrule Ref object [EnsPMarker] Ensembl Marker
**
** @valrule * [EnsPMarker] Ensembl Marker
**
** @fcategory new
******************************************************************************/




/* @func ensMarkerNew *********************************************************
**
** Default constructor for an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPMarkerAdaptor] Ensembl Marker Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Map::Marker::new
** @param [u] display [EnsPMarkerSynonym] Ensembl Marker Synonym
** @param [u] lprimer [AjPStr] Left primer
** @param [u] rprimer [AjPStr] Right primer
** @param [r] synonyms [AjPList] AJAX List of Ensembl Marker Synonyms
** @param [r] locations [AjPList] AJAX List of Ensembl Marker Map Locations
** @param [r] mindistance [ajuint] Minimum primer distance
** @param [r] maxdistance [ajuint] Maximum primer distance
** @param [r] type [AjEnum] Type
** @param [r] priority [ajint] Priority
**
** @return [EnsPMarker] Ensembl Marker or NULL
** @@
******************************************************************************/

EnsPMarker ensMarkerNew(EnsPMarkerAdaptor adaptor,
                        ajuint identifier,
                        EnsPMarkerSynonym display,
                        AjPStr lprimer,
                        AjPStr rprimer,
                        AjPList synonyms,
                        AjPList locations,
                        ajuint mindistance,
                        ajuint maxdistance,
                        AjEnum type,
                        ajint priority)
{
    AjIList iter = NULL;
    
    EnsPMarker marker = NULL;
    
    EnsPMarkerMapLocation mml = NULL;
    
    EnsPMarkerSynonym ms = NULL;
    
    AJNEW0(marker);
    
    marker->Use = 1;
    
    marker->Identifier = identifier;
    
    marker->Adaptor = adaptor;
    
    marker->DisplaySynonym = ensMarkerSynonymNewRef(display);
    
    if(lprimer)
        marker->LeftPrimer = ajStrNewRef(lprimer);
    
    if(rprimer)
        marker->RightPrimer = ajStrNewRef(rprimer);
    
    /* Copy the AJAX List of Ensembl Marker Synonyms. */
    
    if(synonyms && ajListGetLength(synonyms))
    {
	marker->MarkerSynonyms = ajListNew();
	
	iter = ajListIterNew(synonyms);
	
	while(!ajListIterDone(iter))
	{
	    ms = (EnsPMarkerSynonym) ajListIterGet(iter);
	    
	    if(ms)
		ajListPushAppend(marker->MarkerSynonyms,
				 (void *) ensMarkerSynonymNewRef(ms));
	}
	
	ajListIterDel(&iter);
    }
    else
	marker->MarkerSynonyms = NULL;
    
    /* Copy the AJAX List of Ensembl Marker Map Locations. */
    
    if(locations && ajListGetLength(locations))
    {
	marker->MarkerMapLocations = ajListNew();
	
	iter = ajListIterNew(locations);
	
	while(!ajListIterDone(iter))
	{
	    mml = (EnsPMarkerMapLocation) ajListIterGet(iter);
	    
	    if(mml)
		ajListPushAppend(marker->MarkerMapLocations,
				 (void *) ensMarkerMapLocationNewRef(mml));
	}
	
	ajListIterDel(&iter);
    }
    else
	marker->MarkerMapLocations = NULL;
    
    marker->MinimumDistance = mindistance;
    
    marker->MaximumDistance = maxdistance;
    
    marker->Type = type;
    
    marker->Priority = priority;
    
    return marker;
}




/* @func ensMarkerNewObj ******************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPMarker] Ensembl Marker
**
** @return [EnsPMarker] Ensembl Marker or NULL
** @@
******************************************************************************/

EnsPMarker ensMarkerNewObj(const EnsPMarker object)
{
    AjIList iter = NULL;
    
    EnsPMarker marker = NULL;
    
    EnsPMarkerMapLocation mml = NULL;
    
    EnsPMarkerSynonym ms = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(marker);
    
    marker->Use = 1;
    
    marker->Identifier = object->Identifier;
    
    marker->Adaptor = object->Adaptor;
    
    marker->DisplaySynonym = ensMarkerSynonymNewRef(object->DisplaySynonym);
    
    if(object->LeftPrimer)
        marker->LeftPrimer = ajStrNewRef(object->LeftPrimer);
    
    if(object->RightPrimer)
        marker->RightPrimer = ajStrNewRef(object->RightPrimer);
    
    /* Copy the AJAX List of Ensembl Marker Synonyms. */
    
    if(object->MarkerSynonyms && ajListGetLength(object->MarkerSynonyms))
    {
	marker->MarkerSynonyms = ajListNew();
	
	iter = ajListIterNew(object->MarkerSynonyms);
	
	while(!ajListIterDone(iter))
	{
	    ms = (EnsPMarkerSynonym) ajListIterGet(iter);
	    
	    if(ms)
		ajListPushAppend(marker->MarkerSynonyms,
				 (void *) ensMarkerSynonymNewRef(ms));
	}
	
	ajListIterDel(&iter);
    }
    else
	marker->MarkerSynonyms = NULL;
    
    /* Copy the AJAX List of Ensembl Marker Map Locations. */
    
    if(object->MarkerMapLocations &&
	ajListGetLength(object->MarkerMapLocations))
    {
	marker->MarkerMapLocations = ajListNew();
	
	iter = ajListIterNew(object->MarkerMapLocations);
	
	while(!ajListIterDone(iter))
	{
	    mml = (EnsPMarkerMapLocation) ajListIterGet(iter);
	    
	    if(mml)
		ajListPushAppend(marker->MarkerMapLocations,
				 (void *) ensMarkerMapLocationNewRef(mml));
	}
	
	ajListIterDel(&iter);
    }
    else
	marker->MarkerMapLocations = NULL;
    
    marker->MinimumDistance = object->MinimumDistance;
    
    marker->MaximumDistance = object->MaximumDistance;
    
    marker->Type = object->Type;
    
    marker->Priority = object->Priority;
    
    return marker;
}




/* @func ensMarkerNewRef ******************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] marker [EnsPMarker] Ensembl Marker
**
** @return [EnsPMarker] Ensembl Marker or NULL
** @@
******************************************************************************/

EnsPMarker ensMarkerNewRef(EnsPMarker marker)
{
    if(!marker)
	return NULL;
    
    marker->Use++;
    
    return marker;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Marker.
**
** @fdata [EnsPMarker]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Marker object
**
** @argrule * Pmarker [EnsPMarker*] Ensembl Marker object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMarkerDel *********************************************************
**
** Default destructor for an Ensembl Marker.
**
** @param [d] Pmarker [EnsPMarker*] Ensembl Marker address
**
** @return [void]
** @@
******************************************************************************/

void ensMarkerDel(EnsPMarker *Pmarker)
{
    EnsPMarkerMapLocation mml = NULL;
    
    EnsPMarkerSynonym ms = NULL;

    EnsPMarker pthis = NULL;
    
    /*
     ajDebug("ensMarkerDel\n"
	     "  *Pmarker %p\n",
	     *Pmarker);
     
     ensMarkerTrace(*Pmarker, 1);
     */
    
    if(!Pmarker)
        return;
    
    if(!*Pmarker)
        return;

    pthis = *Pmarker;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pmarker = NULL;
	
	return;
    }
    
    ajStrDel(&pthis->LeftPrimer);
    ajStrDel(&pthis->RightPrimer);
    
    ensMarkerSynonymDel(&pthis->DisplaySynonym);
    
    while(ajListPop(pthis->MarkerSynonyms, (void **) &ms))
	ensMarkerSynonymDel(&ms);
    
    ajListFree(&pthis->MarkerSynonyms);
    
    while(ajListPop(pthis->MarkerMapLocations, (void **) &mml))
	ensMarkerMapLocationDel(&mml);
    
    ajListFree(&pthis->MarkerMapLocations);
    
    AJFREE(pthis);

    *Pmarker = NULL;

    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Marker object.
**
** @fdata [EnsPMarker]
** @fnote None
**
** @nam3rule Get Return Marker attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Marker Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetDisplaySynonym Return the display synonym
** @nam4rule GetLeftPrimer Return the left primer
** @nam4rule GetRightPrimer Return the right primer
** @nam4rule GetMarkerSynonyms Return all Ensembl Marker Synonyms
** @nam4rule GetMarkerMapLocations Return all Ensembl Marker Map Locations
** @nam4rule GetMinimumDistance Return the minimum distance
** @nam4rule GetMaximumDistance Return the maximum distance
** @nam4rule GetType Return the type
** @nam4rule GetPriority return the priority
**
** @argrule * marker [const EnsPMarker] Marker
**
** @valrule Adaptor [EnsPMarkerAdaptor] Ensembl Marker Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule DisplaySynonym [EnsPMarkerSynonym] Ensembl Marker Synonym
** @valrule LeftPrimer [AjPStr] Left primer
** @valrule RightPrimer [AjPStr] Right primer
** @valrule MarkerSynonyms [const AjPList] AJAX List of Ensembl Marker Synonyms
** @valrule MarkerMapLocations [const AjPList] AJAX List of Ensembl Marker
**                                             Map Locations
** @valrule MinimumDistance [ajuint] Minimum distance
** @valrule MaximumDistance [ajuint] Maximum distance
** @valrule Type [AjEnum] Type
** @valrule Priority [ajuint] Priority
**
** @fcategory use
******************************************************************************/




/* @func ensMarkerGetAdaptor **************************************************
**
** Get the Ensembl Marker Adaptor element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] marker [const EnsPMarker] Ensembl Marker
**
** @return [EnsPMarkerAdaptor] Ensembl Marker Adaptor
** @@
******************************************************************************/

EnsPMarkerAdaptor ensMarkerGetAdaptor(const EnsPMarker marker)
{
    if(!marker)
        return NULL;
    
    return marker->Adaptor;
}




/* @func ensMarkerGetIdentifier ***********************************************
**
** Get the SQL database-internal identifier element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] marker [const EnsPMarker] Ensembl Marker
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensMarkerGetIdentifier(const EnsPMarker marker)
{
    if(!marker)
        return 0;
    
    return marker->Identifier;
}




/* @func ensMarkerGetDisplaySynonym *******************************************
**
** Get the display synonym element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::display_MarkerSynonym
** @param [r] marker [const EnsPMarker] Ensembl Marker
**
** @return [EnsPMarkerSynonym] Display Ensembl Marker Synonym
** @@
******************************************************************************/

EnsPMarkerSynonym ensMarkerGetDisplaySynonym(const EnsPMarker marker)
{
    if(!marker)
        return NULL;
    
    return marker->DisplaySynonym;
}




/* @func ensMarkerGetLeftPrimer ***********************************************
**
** Get the left primer element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::left_primer
** @param [r] marker [const EnsPMarker] Ensembl Marker
**
** @return [AjPStr] Left primer
** @@
******************************************************************************/

AjPStr ensMarkerGetLeftPrimer(const EnsPMarker marker)
{
    if(!marker)
        return NULL;
    
    return marker->LeftPrimer;
}




/* @func ensMarkerGetRightPrimer **********************************************
**
** Get the right primer element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::right_primer
** @param [r] marker [const EnsPMarker] Ensembl Marker
**
** @return [AjPStr] Right primer
** @@
******************************************************************************/

AjPStr ensMarkerGetRightPrimer(const EnsPMarker marker)
{
    if(!marker)
        return NULL;
    
    return marker->RightPrimer;
}




/* @func ensMarkerGetMarkerSynonyms *******************************************
**
** Get all Ensembl Marker Synonyms of an Ensembl Marker.
**
** This is not a simple accessor function, it will fetch
** Ensembl Marker Synonyms from the Ensembl Core database
** in case the AJAX List is not defined.
**
** @cc Bio::EnsEMBL::Map::Marker::get_all_MarkerSynonyms
** @param [u] marker [EnsPMarker] Ensembl Marker
**
** @return [const AjPList] AJAX List of Ensembl Marker Synonyms
** @@
** NOTE: The Perl API uses a
** Bio::EnsEMBL::Map::DBSQL::MarkerAdaptor::fetch_attributes method to load
** Bio::EnsEMBL::Map::MarkerSynonym and Bio::EnsEMBL::Map::MapLocation objects.
** This is not ideal as the method requires a Marker Adaptor and a Marker.
** There is no check that the MArker Adaptor and Marker refer to the same
** Ensembl Core database.
******************************************************************************/

const AjPList ensMarkerGetMarkerSynonyms(EnsPMarker marker)
{
    ajuint identifier = 0;
    
    EnsPBaseAdaptor ba = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!marker)
        return NULL;
    
    if(marker->MarkerSynonyms)
	return marker->MarkerSynonyms;
    
    ba = ensMarkerAdaptorGetBaseAdaptor(marker->Adaptor);
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(ba);
    
    if(!dba)
	return NULL;
    
    identifier = ensMarkerGetIdentifier(marker);
    
    marker->MarkerSynonyms = ajListNew();
    
    ensMarkerSynonymAdaptorFetchAllByMarkerIdentifier(dba,
						      identifier,
						      marker->MarkerSynonyms);
    
    return marker->MarkerSynonyms;
}




/* @func ensMarkerGetMarkerMapLocations ***************************************
**
** Get all Ensembl Marker Map Locations of an Ensembl Marker.
**
** This is not a simple accessor function, it will fetch
** Ensembl Marker Map Locations from the Ensembl Core database
** in case the AJAX List is not defined.
**
** @cc Bio::EnsEMBL::Map::Marker::get_all_MapLocations
** @param [u] marker [EnsPMarker] Ensembl Marker
**
** @return [const AjPList] AJAX List of Ensembl Marker Map Locations
** @@
** NOTE: The Perl API uses a
** Bio::EnsEMBL::Map::DBSQL::MarkerAdaptor::fetch_attributes method to load
** Bio::EnsEMBL::Map::MarkerSynonym and Bio::EnsEMBL::Map::MapLocation objects.
** This is not ideal as the method requires a Marker Adaptor and a Marker.
** There is no check that the MArker Adaptor and Marker refer to the same
** Ensembl Core database.
******************************************************************************/

const AjPList ensMarkerGetMarkerMapLocations(EnsPMarker marker)
{
    ajuint identifier = 0;
    
    EnsPBaseAdaptor ba = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!marker)
        return NULL;
    
    if(marker->MarkerMapLocations)
	return marker->MarkerMapLocations;
    
    ba = ensMarkerAdaptorGetBaseAdaptor(marker->Adaptor);
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(ba);
    
    if(!dba)
	return NULL;
    
    identifier = ensMarkerGetIdentifier(marker);
    
    marker->MarkerMapLocations = ajListNew();
    
    ensMarkerMapLocationAdaptorFetchAllByMarkerIdentifier(
        dba,
        identifier,
        marker->MarkerMapLocations);
    
    return marker->MarkerMapLocations;
}




/* @func ensMarkerGetMinimumDistance ******************************************
**
** Get the minimum distance element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::min_primer_dist
** @param [r] marker [const EnsPMarker] Ensembl Marker
**
** @return [ajuint] Minimum distance
** @@
******************************************************************************/

ajuint ensMarkerGetMinimumDistance(const EnsPMarker marker)
{
    if(!marker)
        return 0;
    
    return marker->MinimumDistance;
}




/* @func ensMarkerGetMaximumDistance ******************************************
**
** Get the maximum distance element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::max_primer_dist
** @param [r] marker [const EnsPMarker] Ensembl Marker
**
** @return [ajuint] Maximum distance
** @@
******************************************************************************/

ajuint ensMarkerGetMaximumDistance(const EnsPMarker marker)
{
    if(!marker)
        return 0;
    
    return marker->MaximumDistance;
}




/* @func ensMarkerGetType *****************************************************
**
** Get the type element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::type
** @param [r] marker [const EnsPMarker] Ensembl Marker
**
** @return [AjEnum] Type
** @@
******************************************************************************/

AjEnum ensMarkerGetType(const EnsPMarker marker)
{
    if(!marker)
        return ensEMarkerTypeNULL;
    
    return marker->Type;
}




/* @func ensMarkerGetPriority *************************************************
**
** Get the priority element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::priority
** @param [r] marker [const EnsPMarker] Ensembl Marker
**
** @return [ajint] Priority
** @@
******************************************************************************/

ajint ensMarkerGetPriority(const EnsPMarker marker)
{
    if(!marker)
        return 0;
    
    return marker->Priority;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Marker object.
**
** @fdata [EnsPMarker]
** @fnote None
**
** @nam3rule Set Set one element of an Ensembl Marker
** @nam4rule SetAdaptor Set the Ensembl Marker Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetDisplaySynonym Set the display synonym
** @nam4rule SetLeftPrimer Set the left primer
** @nam4rule SetRightPrimer Set the right primer
** @nam4rule SetMinimumDistance Set the minimum distance
** @nam4rule SetMaximumDistance Set the maximum distance
** @nam4rule SetType Set the type
** @nam4rule SetPriority Set the priority
**
** @argrule * marker [EnsPMarker] Ensembl Marker object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensMarkerSetAdaptor **************************************************
**
** Set the Ensembl Marker Adaptor element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] marker [EnsPMarker] Ensembl Marker
** @param [r] adaptor [EnsPMarkerAdaptor] Ensembl Marker Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSetAdaptor(EnsPMarker marker, EnsPMarkerAdaptor adaptor)
{
    if(!marker)
        return ajFalse;
    
    marker->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensMarkerSetIdentifier ***********************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Marker.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] marker [EnsPMarker] Ensembl Marker
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSetIdentifier(EnsPMarker marker, ajuint identifier)
{
    if(!marker)
        return ajFalse;
    
    marker->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensMarkerSetDisplaySynonym *******************************************
**
** Set the display synonym element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::display_MarkerSynonym
** @param [u] marker [EnsPMarker] Ensembl Marker
** @param [u] display [EnsPMarkerSynonym] Display synonym
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSetDisplaySynonym(EnsPMarker marker, EnsPMarkerSynonym display)
{
    if(!marker)
        return ajFalse;
    
    ensMarkerSynonymDel(&marker->DisplaySynonym);
    
    marker->DisplaySynonym = ensMarkerSynonymNewRef(display);
    
    return ajTrue;
}




/* @func ensMarkerSetLeftPrimer ***********************************************
**
** Set the left primer element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::left_primer
** @param [u] marker [EnsPMarker] Ensembl Marker
** @param [u] lprimer [AjPStr] Left primer
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSetLeftPrimer(EnsPMarker marker, AjPStr lprimer)
{
    if(!marker)
        return ajFalse;
    
    ajStrDel(&marker->LeftPrimer);
    
    if(lprimer)
	marker->LeftPrimer = ajStrNewRef(lprimer);
    
    return ajTrue;
}




/* @func ensMarkerSetRightPrimer **********************************************
**
** Set the right primer element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::right_primer
** @param [u] marker [EnsPMarker] Ensembl Marker
** @param [u] rprimer [AjPStr] Right primer
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSetRightPrimer(EnsPMarker marker, AjPStr rprimer)
{
    if(!marker)
        return ajFalse;
    
    ajStrDel(&marker->RightPrimer);
    
    if(rprimer)
	marker->RightPrimer = ajStrNewRef(rprimer);
    
    return ajTrue;
}




/* @func ensMarkerSetMinimumDistance ******************************************
**
** Set the minimum distance element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::min_primer_dist
** @param [u] marker [EnsPMarker] Ensembl Marker
** @param [r] mindistance [ajuint] Minimum distance
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSetMinimumDistance(EnsPMarker marker, ajuint mindistance)
{
    if(!marker)
        return ajFalse;
    
    marker->MinimumDistance = mindistance;
    
    return ajTrue;
}




/* @func ensMarkerSetMaximumDistance ******************************************
**
** Set the maximum distance element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::max_primer_dist
** @param [u] marker [EnsPMarker] Ensembl Marker
** @param [r] maxdistance [ajuint] Maximum distance
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSetMaximumDistance(EnsPMarker marker, ajuint maxdistance)
{
    if(!marker)
        return ajFalse;
    
    marker->MaximumDistance = maxdistance;
    
    return ajTrue;
}




/* @func ensMarkerSetType *****************************************************
**
** Set the type element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::type
** @param [u] marker [EnsPMarker] Ensembl Marker
** @param [u] type [AjEnum] Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSetType(EnsPMarker marker, AjEnum type)
{
    if(!marker)
        return ajFalse;
    
    marker->Type = type;
    
    return ajTrue;
}




/* @func ensMarkerSetPriority *************************************************
**
** Set the priority element of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::priority
** @param [u] marker [EnsPMarker] Ensembl Marker
** @param [r] priority [ajint] Priority
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerSetPriority(EnsPMarker marker, ajint priority)
{
    if(!marker)
        return ajFalse;
    
    marker->Priority = priority;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Marker object.
**
** @fdata [EnsPMarker]
** @nam3rule Trace Report Ensembl Marker elements to debug file.
**
** @argrule Trace marker [const EnsPMarker] Ensembl Marker
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensMarkerTrace *******************************************************
**
** Trace an Ensembl Marker.
**
** @param [r] marker [const EnsPMarker] Ensembl Marker
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerTrace(const EnsPMarker marker, ajuint level)
{
    AjIList iter = NULL;
    
    AjPStr indent = NULL;
    
    EnsPMarkerMapLocation mml = NULL;
    
    EnsPMarkerSynonym ms = NULL;
    
    if(!marker)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensMarkerTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  LeftPrimer '%S'\n"
	    "%S  RightPrimer '%S'\n"
	    "%S  Type '%S'\n"
	    "%S  DisplaySynonym %p\n"
	    "%S  MarkerSynonyms %p\n"
	    "%S  MarkerMapLocations %p\n"
	    "%S  MinimumDistance %u\n"
	    "%S  MaximumDistance %u\n"
	    "%S  Priority %d\n",
	    indent, marker,
	    indent, marker->Use,
	    indent, marker->Identifier,
	    indent, marker->Adaptor,
	    indent, marker->LeftPrimer,
	    indent, marker->RightPrimer,
	    indent, marker->Type,
	    indent, marker->DisplaySynonym,
	    indent, marker->MarkerSynonyms,
	    indent, marker->MarkerMapLocations,
	    indent, marker->MinimumDistance,
	    indent, marker->MaximumDistance,
	    indent, marker->Priority);
    
    ensMarkerSynonymTrace(marker->DisplaySynonym, level + 1);
    
    /* Trace the AJAX List of Ensembl Marker Synonyms. */
    
    iter = ajListIterNew(marker->MarkerSynonyms);
    
    while(!ajListIterDone(iter))
    {
	ms = (EnsPMarkerSynonym) ajListIterGet(iter);
	
	ensMarkerSynonymTrace(ms, level + 1);
    }
    
    ajListIterDel(&iter);
    
    /* Trace the AJAX List of Ensembl Marker Map Locations. */
    
    iter = ajListIterNew(marker->MarkerMapLocations);
    
    while(!ajListIterDone(iter))
    {
	mml = (EnsPMarkerMapLocation) ajListIterGet(iter);
	
	ensMarkerMapLocationTrace(mml, level + 1);
    }
    
    ajListIterDel(&iter);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensMarkerGetMemSize **************************************************
**
** Get the memory size in bytes of an Ensembl Marker.
**
** @param [r] marker [const EnsPMarker] Ensembl Marker
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensMarkerGetMemSize(const EnsPMarker marker)
{
    ajuint size = 0;
    
    AjIList iter = NULL;
    
    EnsPMarkerMapLocation mml = NULL;
    
    EnsPMarkerSynonym ms = NULL;
    
    if(!marker)
	return 0;
    
    size += (ajuint) sizeof (EnsOMarker);
    
    size += ensMarkerSynonymGetMemSize(marker->DisplaySynonym);
    
    if(marker->LeftPrimer)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(marker->LeftPrimer);
    }
    
    if(marker->RightPrimer)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(marker->RightPrimer);
    }
    
    /* Summarise the AJAX List of Ensembl Marker Synonyms. */
    
    iter = ajListIterNew(marker->MarkerSynonyms);
    
    while(!ajListIterDone(iter))
    {
	ms = (EnsPMarkerSynonym) ajListIterGet(iter);
	
	size += ensMarkerSynonymGetMemSize(ms);
    }
    
    ajListIterDel(&iter);
    
    /* Summarise the AJAX List of Ensembl Marker Map Locations. */
    
    iter = ajListIterNew(marker->MarkerMapLocations);
    
    while(!ajListIterDone(iter))
    {
	mml = (EnsPMarkerMapLocation) ajListIterGet(iter);
	
	size += ensMarkerMapLocationGetMemSize(mml);
    }
    
    ajListIterDel(&iter);
    
    return size;
}




/* @func ensMarkerAddMarkerSynonym ********************************************
**
** Add an Ensembl Marker Synonym to an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::add_MarkerSynonym
** @param [u] marker [EnsPMarker] Ensembl Marker
** @param [u] ms [EnsPMarkerSynonym] Ensembl Marker Synonym
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerAddMarkerSynonym(EnsPMarker marker, EnsPMarkerSynonym ms)
{
    if(!marker)
	return ajFalse;
    
    if(!ms)
	return ajFalse;
    
    if(!marker->MarkerSynonyms)
	marker->MarkerSynonyms = ajListNew();
    
    ajListPushAppend(marker->MarkerSynonyms,
		     (void *) ensMarkerSynonymNewRef(ms));
    
    return ajTrue;
}




/* @func ensMarkerAddMarkerMapLocation ****************************************
**
** Add an Ensembl Marker Map Location to an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::add_MapLocations
** @param [u] marker [EnsPMarker] Ensembl Marker
** @param [u] mml [EnsPMarkerMapLocation] Ensembl Marker Map Location
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerAddMarkerMapLocation(EnsPMarker marker,
                                     EnsPMarkerMapLocation mml)
{
    if(!marker)
	return ajFalse;
    
    if(!mml)
	return ajFalse;
    
    if(!marker->MarkerMapLocations)
	marker->MarkerMapLocations = ajListNew();
    
    ajListPushAppend(marker->MarkerMapLocations,
		     (void *) ensMarkerMapLocationNewRef(mml));
    
    return ajTrue;
}




/* @func ensMarkerTypeFromStr *************************************************
**
** Convert an AJAX String into an Ensembl Marker type element.
**
** @param [r] type [const AjPStr] Marker type string
**
** @return [AjEnum] Ensembl Marker type element or ensEMarkerTypeNULL
** @@
******************************************************************************/

AjEnum ensMarkerTypeFromStr(const AjPStr type)
{
    register ajint i = 0;
    
    AjEnum etype = ensEMarkerTypeNULL;
    
    for(i = 1; markerType[i]; i++)
	if(ajStrMatchC(type, markerType[i]))
	    etype = i;
    
    if(!etype)
	ajDebug("ensMarkerTypeFromStr encountered "
		"unexpected string '%S'.\n", type);
    
    return etype;
}




/* @func ensMarkerTypeToChar **************************************************
**
** Convert an Ensembl Marker type element into a C-type (char*) string.
**
** @param [r] type [const AjEnum] Ensembl Marker type enumerator
**
** @return [const char*] Ensembl Marker type C-type (char*) string
** @@
******************************************************************************/

const char* ensMarkerTypeToChar(const AjEnum type)
{
    register ajint i = 0;
    
    if(!type)
	return NULL;
    
    for(i = 1; markerType[i] && (i < type); i++);
    
    if(!markerType[i])
	ajDebug("ensMarkerTypeToChar encountered an "
		"out of boundary error on type %d.\n", type);
    
    return markerType[i];
}




/* @func ensMarkerClearMarkerSynonyms *****************************************
**
** Clear all Ensembl Marker Synonyms of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::flush_MarkerSynonyms
** @param [u] marker [EnsPMarker] Ensembl Marker
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerClearMarkerSynonyms(EnsPMarker marker)
{
    EnsPMarkerSynonym ms = NULL;
    
    if(!marker)
	return ajFalse;
    
    while(ajListPop(marker->MarkerSynonyms, (void **) &ms))
	ensMarkerSynonymDel(&ms);
    
    return ajTrue;
}




/* @func ensMarkerClearMarkerMapLocations *************************************
**
** Clear all Ensembl Marker Map Locations of an Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::Marker::flush_MapLocations
** @param [u] marker [EnsPMarker] Ensembl Marker
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerClearMarkerMapLocations(EnsPMarker marker)
{
    EnsPMarkerMapLocation mml = NULL;
    
    if(!marker)
	return ajFalse;
    
    while(ajListPop(marker->MarkerMapLocations, (void **) &mml))
	ensMarkerMapLocationDel(&mml);
    
    return ajTrue;
}




/* @datasection [EnsPMarkerAdaptor] Marker Adaptor ****************************
**
** Functions for manipulating Ensembl Marker Adaptor objects
**
** @cc Bio::EnsEMBL::Map::DBSQL::MarkerAdaptor CVS Revision: 1.10
**
** @nam2rule MarkerAdaptor
**
******************************************************************************/

static const char *markerAdaptorTables[] =
{
    "marker",
    "marker_synonym",
    (const char *) NULL
};




static const char *markerAdaptorColumns[] =
{
    "marker.marker_id",
    "marker.display_marker_synonym_id",
    "marker.left_primer",
    "marker.right_primer",
    "marker.min_primer_dist",
    "marker.max_primer_dist",
    "marker.priority",
    "marker.type",
    "marker_synonym.marker_synonym_id",
    "marker_synonym.name",
    "marker_synonym.source",
    (const char *) NULL
};




/*
** NOTE: The following LEFT JOIN condition selects all Marker Synonyms.
** The Bio::EnsEMBL::Map::DBSQL::MarkerAdaptor::fetch_all function selects
** only the Marker Synonym via
** marker.display_marker_synonym = marker_synonym.marker_synonym_id
*/

static EnsOBaseAdaptorLeftJoin markerAdaptorLeftJoin[] =
{
    {
	"marker_synonym",
	"marker.marker_id = marker_synonym.marker_id"
	/*
	 "marker.display_marker_synonym_id = marker_synonym.marker_synonym_id"
	 */
    },
    {(const char *) NULL, (const char *) NULL}
};

static const char *markerAdaptorDefaultCondition = NULL;

static const char *markerAdaptorFinalCondition = NULL;




/* @funcstatic markerAdaptorFetchAllBySQL *************************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl Marker objects.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] markers [AjPList] AJAX List of Ensembl Markers
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool markerAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                         const AjPStr statement,
                                         EnsPAssemblyMapper am,
                                         EnsPSlice slice,
                                         AjPList markers)
{
    ajint priority = 0;
    
    ajuint current     = 0;
    ajuint identifier  = 0;
    ajuint displaymsid = 0;
    ajuint mindistance = 0;
    ajuint maxdistance = 0;
    ajuint msid = 0;
    
    AjEnum etype = ensEMarkerTypeNULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr lprimer  = NULL;
    AjPStr rprimer  = NULL;
    AjPStr type     = NULL;
    AjPStr msname   = NULL;
    AjPStr mssource = NULL;
    
    EnsPMarker marker         = NULL;
    EnsPMarkerAdaptor adaptor = NULL;
    
    EnsPMarkerSynonym ms = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    (void) am;
    
    (void) slice;
    
    if(!markers)
	return ajFalse;
    
    adaptor = ensRegistryGetMarkerAdaptor(dba);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	identifier  = 0;
	displaymsid = 0;
	lprimer     = ajStrNew();
	rprimer     = ajStrNew();
	mindistance = 0;
	maxdistance = 0;
	priority    = 0;
	type        = ajStrNew();
	msid        = 0;
	msname      = ajStrNew();
	mssource    = ajStrNew();

	etype = ensEMarkerTypeNULL;
	
        sqlr = ajSqlRowIterGet(sqli);
	
        ajSqlColumnToUint(sqlr, &identifier);
	ajSqlColumnToUint(sqlr, &displaymsid);
	ajSqlColumnToStr(sqlr, &lprimer);
	ajSqlColumnToStr(sqlr, &rprimer);
	ajSqlColumnToUint(sqlr, &mindistance);
	ajSqlColumnToUint(sqlr, &maxdistance);
	ajSqlColumnToInt(sqlr, &priority);
	ajSqlColumnToStr(sqlr, &type);
	ajSqlColumnToUint(sqlr, &msid);
	ajSqlColumnToStr(sqlr, &msname);
	ajSqlColumnToStr(sqlr, &mssource);
	
	if(current != identifier)
	{
	    etype = ensMarkerTypeFromStr(type);
	    
	    marker = ensMarkerNew(adaptor,
				  identifier,
				  (EnsPMarkerSynonym) NULL,
				  lprimer,
				  rprimer,
				  (AjPList) NULL,
				  (AjPList) NULL,
				  mindistance,
				  maxdistance,
				  etype,
				  priority);
	    
	    ajListPushAppend(markers, (void *) marker);
	    
	    current = identifier;
	}
	
	if(msid)
	{
	    ms = ensMarkerSynonymNew(msid, mssource, msname);
	    
	    if(msid == displaymsid)
		ensMarkerSetDisplaySynonym(marker, ms);
	    else
		ensMarkerAddMarkerSynonym(marker, ms);
	    
	    ensMarkerSynonymDel(&ms);
	}
	
	ajStrDel(&lprimer);
	ajStrDel(&rprimer);
	ajStrDel(&type);
	ajStrDel(&msname);
	ajStrDel(&mssource);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Marker Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Marker Adaptor. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMarkerAdaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPMarkerAdaptor] Ensembl Marker Adaptor
** @argrule Ref object [EnsPMarkerAdaptor] Ensembl Marker Adaptor
**
** @valrule * [EnsPMarkerAdaptor] Ensembl Marker Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensMarkerAdaptorNew **************************************************
**
** Default Ensembl Marker Adaptor constructor.
**
** @cc Bio::EnsEMBL::Map::DBSQL::MarkerAdaptor::new
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPMarkerAdaptor] Ensembl Marker Adaptor or NULL
** @@
******************************************************************************/

EnsPMarkerAdaptor ensMarkerAdaptorNew(EnsPDatabaseAdaptor dba)
{
    EnsPMarkerAdaptor ma = NULL;
    
    AJNEW0(ma);
    
    ma->Adaptor = ensBaseAdaptorNew(dba,
				    markerAdaptorTables,
				    markerAdaptorColumns,
				    markerAdaptorLeftJoin,
				    markerAdaptorDefaultCondition,
				    markerAdaptorFinalCondition,
				    markerAdaptorFetchAllBySQL);
    
    return ma;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Marker Adaptor.
**
** @fdata [EnsPMarkerAdaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Marker Adaptor object.
**
** @argrule * Padaptor [EnsPMarkerAdaptor*] Ensembl Marker Adaptor
**                                          object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMarkerAdaptorDel **************************************************
**
** Default destructor for an Ensembl Marker Adaptor.
** This function also clears the internal caches.
**
** @param [d] Padaptor [EnsPMarkerAdaptor*] Ensembl Marker Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensMarkerAdaptorDel(EnsPMarkerAdaptor *Padaptor)
{
    EnsPMarkerAdaptor pthis = NULL;
    
    if(!Padaptor)
        return;
    
    if(!*Padaptor)
        return;

    pthis = *Padaptor;
    
    ensBaseAdaptorDel(&pthis->Adaptor);
    
    AJFREE(pthis);

    *Padaptor = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Marker Adaptor object.
**
** @fdata [EnsPMarkerAdaptor]
** @fnote None
**
** @nam3rule Get Return Marker Adaptor attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Base Adaptor
**
** @argrule * adaptor [const EnsPMarkerAdaptor] Marker Adaptor
**
** @valrule Adaptor [EnsPBaseAdaptor] Ensembl Base Adaptor
**
** @fcategory use
******************************************************************************/




/* @func ensMarkerAdaptorGetBaseAdaptor ***************************************
**
** Get the Ensembl Base Adaptor element of an Ensembl Marker Adaptor.
**
** @param [r] adaptor [const EnsPMarkerAdaptor] Ensembl Marker Adaptor
**
** @return [EnsPBaseAdaptor] Ensembl Base Adaptor
** @@
******************************************************************************/

EnsPBaseAdaptor ensMarkerAdaptorGetBaseAdaptor(const EnsPMarkerAdaptor adaptor)
{
    if(!adaptor)
	return NULL;
    
    return adaptor->Adaptor;
}




/* @section object retrieval **************************************************
**
** Functions for retrieving Ensembl Marker objects from an
** Ensembl Core database.
**
** @fdata [EnsPMarkerAdaptor]
** @fnote None
**
** @nam3rule Fetch Retrieve Ensembl Marker object(s)
** @nam4rule FetchAll Retrieve all Ensembl Marker objects
** @nam5rule FetchAllBy Retrieve all Ensembl Marker objects
**                      matching a criterion
** @nam4rule FetchBy Retrieve one Ensembl Marker object
**                   matching a criterion
**
** @argrule * adaptor [const EnsPMarkerAdaptor] Ensembl Marker Adaptor
** @argrule FetchAll [AjPList] AJAX List of Ensembl Marker objects
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory use
******************************************************************************/




/* @func ensMarkerAdaptorFetchAll *********************************************
**
** Fetch all Ensembl Markers.
**
** The caller is responsible for deleting the Ensembl Markers before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Map::DBSQL::MarkerAdaptor::fetch_all
** @param [r] ma [const EnsPMarkerAdaptor] Ensembl Marker Adaptor
** @param [u] markers [AjPList] AJAX List of Ensembl Markers
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerAdaptorFetchAll(EnsPMarkerAdaptor adaptor, AjPList markers)
{
    if(!adaptor)
	return ajFalse;
    
    if(!markers)
	return ajFalse;
    
    ensBaseAdaptorGenericFetch(adaptor->Adaptor,
			       (AjPStr) NULL,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       markers);
    
    return ajTrue;
}




/* @func ensMarkerAdaptorFetchByIdentifier ************************************
**
** Fetch an Ensembl Marker via its SQL database-internal identifier.
**
** The caller is responsible for deleting the Ensembl Marker.
**
** @cc Bio::EnsEMBL::Map::DBSQL::MarkerAdaptor::fetch_by_dbID
** @param [r] adaptor [EnsPMarkerAdaptor] Ensembl Marker Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pmarker [EnsPMarker*] Ensembl Marker address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerAdaptorFetchByIdentifier(EnsPMarkerAdaptor adaptor,
                                         ajuint identifier,
                                         EnsPMarker *Pmarker)
{
    AjPList markers = NULL;
    
    AjPStr constraint = NULL;
    
    EnsPMarker marker = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!Pmarker)
	return ajFalse;
    
    *Pmarker = NULL;
    
    constraint = ajFmtStr("marker.marker_id = %u", identifier);
    
    markers = ajListNew();
    
    ensBaseAdaptorGenericFetch(adaptor->Adaptor,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       markers);
    
    if(ajListGetLength(markers) == 0)
	ajDebug("ensMarkerAdaptorFetchByIdentifier got no Ensembl Marker "
		"for identifier %u.\n", identifier);
    
    if(ajListGetLength(markers) > 1)
	ajWarn("ensMarkerAdaptorFetchByIdentifier got more than one "
	       "Ensembl Marker for identifier %u.\n", identifier);
    
    ajListPop(markers, (void **) Pmarker);
    
    while(ajListPop(markers, (void **) &marker))
	ensMarkerDel(&marker);
    
    ajListFree(&markers);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @funcstatic markerAdaptorClearIdentifierTable ******************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Marker Adaptor-
** internal table. This function deletes the AJAX unsigned integer key and
** the AJAX boolean value data.
**
** @param [u] key [void **] AJAX unsigned integer key data address
** @param [u] value [void **] AJAX boolean value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void markerAdaptorClearIdentifierTable(void **key, void **value,
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
    
    AJFREE(*value);
    
    return;
}




/* @func ensMarkerAdaptorFetchAllBySynonym ************************************
**
** Fetch all Ensembl Markers via a Marker Synonym name and source.
**
** The caller is responsible for deleting the Ensembl Markers before deleting
** the AJAX List.
**
** @cc Bio::EnsEMBL::Map::DBSQL::MarkerAdaptor::fetch_all_by_synonym
** @param [r] adaptor [EnsPMarkerAdaptor] Ensembl Marker Adaptor
** @param [r] synonym [const AjPStr] Marker Synonym name
** @param [rN] source [const AjPStr] Marker Synonym source
** @param [w] markers [AjPList] AJAX List of Ensembl Markers
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerAdaptorFetchAllBySynonym(EnsPMarkerAdaptor ma,
                                         const AjPStr name,
                                         const AjPStr source,
                                         AjPList markers)
{
    char *txtname   = NULL;
    char *txtsource = NULL;
    
    ajuint identifier = 0;
    
    ajuint *Pidentifier = NULL;
    
    AjBool *Pbool = NULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr statement = NULL;
    
    AjPTable table = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPMarker marker = NULL;
    
    if(!ma)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    if(!markers)
	return ajFalse;
    
    table = ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(ma->Adaptor);
    
    ensDatabaseAdaptorEscapeCS(dba, &txtname, name);
    
    statement = ajFmtStr("SELECT "
			 "marker_synonym.marker_id "
			 "FROM "
			 "marker_synonym "
			 "WHERE "
			 "marker_synonym.name = '%s'",
			 txtname);
    
    ajCharDel(&txtname);
    
    if(source && ajStrGetLen(source))
    {
	ensDatabaseAdaptorEscapeCS(dba, &txtsource, source);
	
	ajFmtPrintAppS(&statement,
		       " AND marker_synonym.source = '%s'",
		       txtsource);
	
	ajCharDel(&txtsource);
    }
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	identifier = 0;
	marker = NULL;
	
        sqlr = ajSqlRowIterGet(sqli);
	
        ajSqlColumnToUint(sqlr, &identifier);
	
	if(ajTableFetch(table, (const void *) &identifier))
	    continue;
	
	AJNEW0(Pidentifier);
	
	*Pidentifier = identifier;
	
	AJNEW0(Pbool);
	
	*Pbool = ajTrue;
	
	ajTablePut(table, (void *) Pidentifier, (void *) Pbool);
	
	ensMarkerAdaptorFetchByIdentifier(ma, identifier, &marker);
	
	if(marker)
	    ajListPushAppend(markers, (void *) marker);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    ajTableMapDel(table, markerAdaptorClearIdentifierTable, NULL);
    
    ajTableFree(&table);
    
    return ajTrue;
}




/* @func ensMarkerAdaptorFetchAttributes **************************************
**
** Fetch Ensembl Marker Synonyms and Ensembl Maker Map Locations of an
** Ensembl Marker.
**
** Deperecated. Use ensMarkerGetMarkerSynonyms and
** ensMarkerGetMarkerMapLocations instead.
**
** @cc Bio::EnsEMBL::Map::DBSQL::MarkerAdaptor::fetch_attributes
** @param [r] adaptor [EnsPMarkerAdaptor] Ensembl Marker Adaptor
** @param [u] marker [EnsPMarker] Ensembl Marker
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
** NOTE: This function is not ideal as it requires a Marker Adaptor although
** a Marker Adaptor is most likely already set in the Marker object.
** The Perl API version of this function does not test whether the Marker
** object has actually been generated from the same source database.
** By loading Marker Synonyms and Marker Map Locations via ensMarkerGetSynonyms
** and ensMarkerGetLocations, respectively, it is ensured that Marker and
** Attribute objects come form the same database.
******************************************************************************/

__deprecated AjBool ensMarkerAdaptorFetchAttributes(EnsPMarkerAdaptor adaptor,
                                                    EnsPMarker marker)
{
    float lodscore = 0;
    
    ajuint msid = 0;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr statement = NULL;
    AjPStr msname    = NULL;
    AjPStr mssource  = NULL;
    AjPStr chrname   = NULL;
    AjPStr position  = NULL;
    AjPStr mapname   = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPMarkerSynonym ms = NULL;
    
    EnsPMarkerMapLocation mml = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!marker)
	return ajFalse;
    
    if(!marker->Identifier)
    {
	ajWarn("ensMarkerAdaptorFetchAttributes got an Ensembl Marker "
	       "without an identifier.");
	
	return ajFalse;
    }
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(adaptor->Adaptor);
    
    /* First, retrieve all Ensembl Marker Synonyms for this Ensembl Marker. */
    
    ensMarkerClearMarkerSynonyms(marker);
    
    statement = ajFmtStr("SELECT "
			 "marker_synonym.marker_synonym_id, "
			 "marker_synonym.source, "
			 "marker_synonym.name "
			 "FROM "
			 "marker_synonym "
			 "WHERE "
			 "marker_synonym.marker_id = %u",
			 marker->Identifier);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	msid     = 0;
	msname   = ajStrNew();
	mssource = ajStrNew();
	
        sqlr = ajSqlRowIterGet(sqli);
	
        ajSqlColumnToUint(sqlr, &msid);
	ajSqlColumnToStr(sqlr, &msname);
	ajSqlColumnToStr(sqlr, &mssource);
	
	ms = ensMarkerSynonymNew(msid, mssource, msname);
	
	ensMarkerAddMarkerSynonym(marker, ms);
	
	ensMarkerSynonymDel(&ms);
	
	ajStrDel(&msname);
	ajStrDel(&mssource);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    /*
    ** Now, retrieve all Ensembl Marker Map Locations for this
    ** Ensembl Marker.
    */
    
    ensMarkerClearMarkerMapLocations(marker);
    
    statement = ajFmtStr("SELECT "
			 "marker_map_location.chromosome_name, "
			 "marker_map_location.position, "
			 "marker_map_location.lod_score, "
			 "map.map_name, "
			 "marker_synonym.marker_synonym_id, "
			 "marker_synonym.source, "
			 "marker_synonym.name, "
			 "FROM "
			 "marker_map_location, "
			 "map, "
			 "marker_synonym "
			 "WHERE "
			 "marker_map_location.marker_id = %u "
			 "AND "
			 "map.map_id = marker_map_location.map_id "
			 "AND "
			 "marker_synonym.marker_synonym_id = "
			 "marker_map_location.marker_synonym_id",
			 marker->Identifier);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
        chrname  = ajStrNew();
	position = ajStrNew();
	lodscore = 0;
	mapname  = ajStrNew();
	msid     = 0;
	mssource = ajStrNew();
	msname   = ajStrNew();
	
	sqlr = ajSqlRowIterGet(sqli);
	
	ajSqlColumnToStr(sqlr, &chrname);
	ajSqlColumnToStr(sqlr, &position);
	ajSqlColumnToFloat(sqlr, &lodscore);
	ajSqlColumnToStr(sqlr, &mapname);
	ajSqlColumnToUint(sqlr, &msid);
	ajSqlColumnToStr(sqlr, &mssource);
	ajSqlColumnToStr(sqlr, &msname);
	
	ms = ensMarkerSynonymNew(msid, mssource, msname);
	
	mml = ensMarkerMapLocationNew(ms,
				      mapname,
				      chrname,
				      position,
				      lodscore);
	
	ensMarkerAddMarkerMapLocation(marker, mml);
	
	ensMarkerMapLocationDel(&mml);
	
	ensMarkerSynonymDel(&ms);
	
	ajStrDel(&chrname);
	ajStrDel(&position);
	ajStrDel(&mapname);
	ajStrDel(&msname);
	ajStrDel(&mssource);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/*
** FIXME: The above function is problematic as the object could be based on a
** different adaptor than the one passed in here. There should really just be
** ensMarkerGetMarkerSynonyms and ensMarkerGetMarkerMapLocations functions
** that lazy-load from the database with the object internal adaptor.
**
** FIXME: Alternatively, the Marker Adaptor could just fetch the
** Marker Synonym that is linked to the display_marker_synoym_id. All other
** Marker Synonyms could be loaded from the database via ensMarkerGetSynonyms
** when neeeded. This could work like ensTranscriptGetExons.
** Done!
**
** TODO: Report this also to the Ensembl Core team?
*/




/* @datasection [EnsPMarkerFeature] Marker Feature ****************************
**
** Functions for manipulating Ensembl Marker Feature objects
**
** @cc Bio::EnsEMBL::Map::MarkerFeature CVS Revision: 1.4
**
** @nam2rule MarkerFeature
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Marker Feature by pointer.
** It is the responsibility of the user to first destroy any previous
** Marker Features. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMarkerFeature]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPMarkerFeature] Ensembl Marker Feature
** @argrule Ref object [EnsPMarkerFeature] Ensembl Marker Feature
**
** @valrule * [EnsPMarkerFeature] Ensembl Marker Feature
**
** @fcategory new
******************************************************************************/




/* @func ensMarkerFeatureNew **************************************************
**
** Default constructor for an Ensembl Marker Feature.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] adaptor [EnsPMarkerFeatureAdaptor] Ensembl Marker Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Feature::new
** @param [u] feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Map::MarkerFeature::new
** @param [u] marker [EnsPMarker] Ensembl Marker
** @param [r] mapweight [ajint] Map weight
**
** @return [EnsPMarkerFeature] Ensembl Marker Feature or NULL
** @@
******************************************************************************/

EnsPMarkerFeature ensMarkerFeatureNew(EnsPMarkerFeatureAdaptor adaptor,
                                      ajuint identifier,
                                      EnsPFeature feature,
                                      EnsPMarker marker,
                                      ajint mapweight)
{
    EnsPMarkerFeature mf = NULL;
    
    AJNEW0(mf);
    
    mf->Use = 1;
    
    mf->Identifier = identifier;
    
    mf->Adaptor = adaptor;
    
    mf->Feature = ensFeatureNewRef(feature);
    
    mf->Marker = ensMarkerNewRef(marker);
    
    mf->MapWeight = mapweight;
    
    return mf;
}




/* @func ensMarkerFeatureNewObj ***********************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPMarkerFeature] Ensembl Marker Feature
**
** @return [EnsPMarkerFeature] Ensembl Marker Feature or NULL
** @@
******************************************************************************/

EnsPMarkerFeature ensMarkerFeatureNewObj(const EnsPMarkerFeature object)
{
    EnsPMarkerFeature mf = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(mf);
    
    mf->Use = 1;
    
    mf->Identifier = object->Identifier;
    
    mf->Adaptor = object->Adaptor;
    
    mf->Feature = ensFeatureNewRef(object->Feature);
    
    mf->Marker = ensMarkerNewRef(object->Marker);
    
    mf->MapWeight = object->MapWeight;
    
    return mf;
}




/* @func ensMarkerFeatureNewRef ***********************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] mf [EnsPMarkerFeature] Ensembl Marker Feature
**
** @return [EnsPMarkerFeature] Ensembl Marker Feature or NULL
** @@
******************************************************************************/

EnsPMarkerFeature ensMarkerFeatureNewRef(EnsPMarkerFeature mf)
{
    if(!mf)
	return NULL;
    
    mf->Use++;
    
    return mf;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Marker Feature.
**
** @fdata [EnsPMarkerFeature]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Marker Feature object
**
** @argrule * Pmf [EnsPMarkerFeature*] Ensembl Marker Feature object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMarkerFeatureDel **************************************************
**
** Default destructor for an Ensembl Marker Feature.
**
** @param [d] Pmf [EnsPMarkerFeature*] Ensembl Marker Feature address
**
** @return [void]
** @@
******************************************************************************/

void ensMarkerFeatureDel(EnsPMarkerFeature *Pmf)
{
    EnsPMarkerFeature pthis = NULL;
    
    /*
     ajDebug("ensMarkerFeatureDel\n"
	     "  *Pmf %p\n",
	     *Pmf);
     
     ensMarkerFeatureTrace(*Pmf, 1);
     */
    
    if(!Pmf)
        return;
    
    if(!*Pmf)
        return;

    pthis = *Pmf;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pmf = NULL;
	
	return;
    }
    
    ensFeatureDel(&pthis->Feature);
    
    ensMarkerDel(&pthis->Marker);
    
    AJFREE(pthis);

    *Pmf = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Marker Feature object.
**
** @fdata [EnsPMarkerFeature]
** @fnote None
**
** @nam3rule Get Return Marker Feature attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Marker Feature Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetFeature Return the Ensembl Feature
** @nam4rule GetMarker Return the Ensembl Marker
** @nam4rule GetMapWeight return the map weight
**
** @argrule * mf [const EnsPMarkerFeature] Marker Feature
**
** @valrule Adaptor [EnsPMarkerFeatureAdaptor] Ensembl Marker Feature Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Feature [EnsPFeature] Ensembl Feature
** @valrule Marker [EnsPMarker] Ensembl Marker
** @valrule MapWeight [ajint] Map weight
**
** @fcategory use
******************************************************************************/




/* @func ensMarkerFeatureGetAdaptor *******************************************
**
** Get the Ensembl Marker Feature Adaptor element of an Ensembl Marker Feature.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] mf [const EnsPMarkerFeature] Ensembl Marker Feature
**
** @return [EnsPMarkerFeatureAdaptor] Ensembl Marker Feature Adaptor
** @@
******************************************************************************/

EnsPMarkerFeatureAdaptor ensMarkerFeatureGetAdaptor(const EnsPMarkerFeature mf)
{
    if(!mf)
        return NULL;
    
    return mf->Adaptor;
}




/* @func ensMarkerFeatureGetIdentifier ****************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Marker Feature.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] mf [const EnsPMarkerFeature] Ensembl Marker Feature
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensMarkerFeatureGetIdentifier(const EnsPMarkerFeature mf)
{
    if(!mf)
        return 0;
    
    return mf->Identifier;
}




/* @func ensMarkerFeatureGetFeature *******************************************
**
** Get the Ensembl Feature element of an Ensembl Marker Feature.
**
** @param [r] mf [const EnsPMarkerFeature] Ensembl Marker Feature
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

EnsPFeature ensMarkerFeatureGetFeature(const EnsPMarkerFeature mf)
{
    if(!mf)
        return NULL;
    
    return mf->Feature;
}




/* @func ensMarkerFeatureGetMarker ********************************************
**
** Get the Ensembl Marker element of an Ensembl Marker Feature.
**
** @cc Bio::EnsEMBL::Map::MarkerFeature::marker
** @param [r] mf [const EnsPMarkerFeature] Ensembl Marker Feature
**
** @return [EnsPMarker] Ensembl Marker
** @@
******************************************************************************/

EnsPMarker ensMarkerFeatureGetMarker(const EnsPMarkerFeature mf)
{
    if(!mf)
        return NULL;
    
    return mf->Marker;
}




/* @func ensMarkerFeatureGetMapWeight *****************************************
**
** Get the map wight element of an Ensembl Marker Feature.
**
** @cc Bio::EnsEMBL::Map::MarkerFeature::map_weight
** @param [r] mf [const EnsPMarkerFeature] Ensembl Marker Feature
**
** @return [ajint] Map weight
** @@
******************************************************************************/

ajint ensMarkerFeatureGetMapWeight(const EnsPMarkerFeature mf)
{
    if(!mf)
        return 0;
    
    return mf->MapWeight;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Marker Feature object.
**
** @fdata [EnsPMarkerFeature]
** @fnote None
**
** @nam3rule Set Set one element of an Ensembl Marker Feature
** @nam4rule SetAdaptor Set the Ensembl Marker Feature Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetFeature Set the Ensembl Feature
** @nam4rule SetMarker Set the Ensembl Marker
** @nam4rule SetMapWeight Set the map weight
**
** @argrule * mf [EnsPMarkerFeature] Ensembl Marker Feature object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensMarkerFeatureSetAdaptor *******************************************
**
** Set the Ensembl Marker Feature Adaptor element of an Ensembl Marker Feature.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] mf [EnsPMarkerFeature] Ensembl Marker Feature
** @param [r] adaptor [EnsPMarkerFeatureAdaptor] Ensembl Marker Feature Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerFeatureSetAdaptor(EnsPMarkerFeature mf,
                                  EnsPMarkerFeatureAdaptor adaptor)
{
    if(!mf)
        return ajFalse;
    
    mf->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensMarkerFeatureSetIdentifier ****************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Marker Feature.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] mf [EnsPMarkerFeature] Ensembl Marker Feature
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerFeatureSetIdentifier(EnsPMarkerFeature mf, ajuint identifier)
{
    if(!mf)
        return ajFalse;
    
    mf->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensMarkerFeatureSetFeature *******************************************
**
** Set the Ensembl Feature element of an Ensembl Marker Feature.
**
** @param [u] mf [EnsPMarkerFeature] Ensembl Marker Feature
** @param [u] feature [EnsPFeaturer] Ensembl Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerFeatureSetFeature(EnsPMarkerFeature mf, EnsPFeature feature)
{
    if(!mf)
        return ajFalse;
    
    ensFeatureDel(&mf->Feature);
    
    mf->Feature = ensFeatureNewRef(feature);
    
    return ajTrue;
}




/* @func ensMarkerFeatureSetMarker ********************************************
**
** Set the Ensembl Marker element of an Ensembl Marker Feature.
**
** @cc Bio::EnsEMBL::Map::MarkerFeature::marker
** @param [u] mf [EnsPMarkerFeature] Ensembl Marker Feature
** @param [u] marker [EnsPMarker] Ensembl Marker
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerFeatureSetMarker(EnsPMarkerFeature mf, EnsPMarker marker)
{
    if(!mf)
        return ajFalse;
    
    ensMarkerDel(&(mf->Marker));
    
    mf->Marker = ensMarkerNewRef(marker);
    
    return ajTrue;
}




/* @func ensMarkerFeatureSetMapWeight *****************************************
**
** Set the map weight element of an Ensembl Marker Feature.
**
** @cc Bio::EnsEMBL::Map::MarkerFeature::map_weight
** @param [u] mf [EnsPMarkerFeature] Ensembl Marker Feature
** @param [r] mapweight [ajint] Map weight
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerFeatureSetMapWeight(EnsPMarkerFeature mf, ajint mapweight)
{
    if(!mf)
        return ajFalse;
    
    mf->MapWeight = mapweight;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Marker Feature object.
**
** @fdata [EnsPMarkerFeature]
** @nam3rule Trace Report Ensembl Marker Feature elements to debug file.
**
** @argrule Trace mf [const EnsPMarkerFeature] Ensembl Marker Feature
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensMarkerFeatureTrace ************************************************
**
** Trace an Ensembl Marker Feature.
**
** @param [r] mf [const EnsPMarkerFeature] Ensembl Marker Feature
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerFeatureTrace(const EnsPMarkerFeature mf, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!mf)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensMarkerFeatureTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Feature %p\n"
	    "%S  Marker %p\n"
	    "%S  MapWeight %d\n",
	    indent, mf,
	    indent, mf->Use,
	    indent, mf->Identifier,
	    indent, mf->Adaptor,
	    indent, mf->Feature,
	    indent, mf->Marker,
	    indent, mf->MapWeight);
    
    ensFeatureTrace(mf->Feature, level + 1);
    
    ensMarkerTrace(mf->Marker, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensMarkerFeatureGetMemSize *******************************************
**
** Get the memory size in bytes of an Ensembl Marker Feature.
**
** @param [r] mf [const EnsPMarkerFeature] Ensembl Marker Feature
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensMarkerFeatureGetMemSize(const EnsPMarkerFeature mf)
{
    ajuint size = 0;
    
    if(!mf)
	return 0;
    
    size += (ajuint) sizeof (EnsOMarkerFeature);
    
    size += ensFeatureGetMemSize(mf->Feature);
    size += ensMarkerGetMemSize(mf->Marker);
    
    return size;
}




/* @datasection [EnsPMarkerFeatureAdaptor] Marker Feature Adaptor *************
**
** Functions for manipulating Ensembl Marker Feature Adaptor objects
**
** @cc Bio::EnsEMBL::Map::DBSQL::MarkerFeatureAdaptor CVS Revision: 1.11
**
** @nam2rule MarkerFeatureAdaptor
**
******************************************************************************/

static const char *markerFeatureAdaptorTables[] =
{
    "marker_feature",
    "marker",
    "marker_synonym",
    (const char *) NULL
};




static const char *markerFeatureAdaptorColumns[] =
{
    "marker_feature.marker_feature_id",
    "marker_feature.seq_region_id",
    "marker_feature.seq_region_start",
    "marker_feature.seq_region_end",
    "marker_feature.analysis_id",
    "marker_feature.map_weight",
    "marker.marker_id",
    "marker.left_primer",
    "marker.right_primer",
    "marker.min_primer_dist",
    "marker.max_primer_dist",
    "marker.priority",
    "marker.type",
    "marker_synonym.marker_synonym_id",
    "marker_synonym.name",
    "marker_synonym.source",
    (const char *) NULL
};




static EnsOBaseAdaptorLeftJoin markerFeatureAdaptorLeftJoin[] =
{
    {
	"marker_synonym",
	"marker.display_marker_synonym_id = marker_synonym.marker_synonym_id"
    },
    {(const char *) NULL, (const char *) NULL}
};




static const char *markerFeatureAdaptorDefaultCondition =
"marker_feature.marker_id = marker.marker_id";

static const char *markerFeatureAdaptorFinalCondition = NULL;




/* @funcstatic markerFeatureAdaptorClearMarkerTable ***************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Marker table.
** This function deletes the AJAX unsigned integer key and the
** Ensembl Marker value data.
**
** @param [u] key [void **] AJAX unsigned integer key data address
** @param [u] value [void **] Ensembl Marker value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void markerFeatureAdaptorClearMarkerTable(void **key, void **value,
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
    
    ensMarkerDel((EnsPMarker *) value);
    
    return;
}




/* @funcstatic markerFeatureAdaptorFetchAllBySQL ******************************
**
** Fetch all Ensembl Marker Feature objects via an SQL statement.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] mflist [AjPList] AJAX List of Ensembl Marker Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool markerFeatureAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                                const AjPStr statement,
                                                EnsPAssemblyMapper am,
                                                EnsPSlice slice,
                                                AjPList mflist)
{
    ajint mapweight = 0;
    ajint priority  = 0;
    ajint slstart   = 0;
    ajint slend     = 0;
    ajint slstrand  = 0;
    ajint sllength  = 0;
    
    ajuint identifier  = 0;
    ajuint srid        = 0;
    ajuint srstart     = 0;
    ajuint srend       = 0;
    ajuint analysisid  = 0;
    ajuint markerid    = 0;
    ajuint mindistance = 0;
    ajuint maxdistance = 0;
    ajuint msid        = 0;
    
    ajuint *Pidentifier = NULL;
    
    AjEnum etype = ensEMarkerTypeNULL;
    
    AjPList mrs = NULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr lprimer  = NULL;
    AjPStr rprimer  = NULL;
    AjPStr type     = NULL;
    AjPStr msname   = NULL;
    AjPStr mssource = NULL;
    
    AjPTable table = NULL;
    
    EnsPAnalysis analysis  = NULL;
    EnsPAnalysisAdaptor aa = NULL;
    
    EnsPAssemblyMapperAdaptor ama = NULL;
    
    EnsPFeature feature = NULL;
    
    EnsPMapperResult mr = NULL;
    
    EnsPMarker marker    = NULL;
    EnsPMarkerAdaptor ma = NULL;
    
    EnsPMarkerFeature mf         = NULL;
    EnsPMarkerFeatureAdaptor mfa = NULL;
    
    EnsPMarkerSynonym ms = NULL;
    
    EnsPSlice srslice   = NULL;
    EnsPSliceAdaptor sa = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    (void) am;
    
    (void) slice;
    
    if(!mflist)
	return ajFalse;
    
    aa = ensRegistryGetAnalysisAdaptor(dba);
    
    ma = ensRegistryGetMarkerAdaptor(dba);
    
    mfa = ensRegistryGetMarkerFeatureAdaptor(dba);
    
    sa = ensRegistryGetSliceAdaptor(dba);
    
    if(slice)
	ama = ensRegistryGetAssemblyMapperAdaptor(dba);
    
    mrs = ajListNew();
    
    table = ajTableNewFunctionLen(0, ensTableCmpUInt, ensTableHashUInt);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	identifier  = 0;
	srid        = 0;
	srstart     = 0;
	srend       = 0;
	analysisid  = 0;
	mapweight   = 0;
	markerid    = 0;
	lprimer     = ajStrNew();
	rprimer     = ajStrNew();
	mindistance = 0;
	maxdistance = 0;
	priority    = 0;
	type        = ajStrNew();
	msid        = 0;
	msname      = ajStrNew();
	mssource    = ajStrNew();
	etype       = ensEMarkerTypeNULL;
	
	sqlr = ajSqlRowIterGet(sqli);
	
	ajSqlColumnToUint(sqlr, &identifier);
	ajSqlColumnToUint(sqlr, &srid);
	ajSqlColumnToUint(sqlr, &srstart);
	ajSqlColumnToUint(sqlr, &srend);
	ajSqlColumnToUint(sqlr, &analysisid);
	ajSqlColumnToInt(sqlr, &mapweight);
	ajSqlColumnToUint(sqlr, &markerid);
	ajSqlColumnToStr(sqlr, &lprimer);
	ajSqlColumnToStr(sqlr, &rprimer);
	ajSqlColumnToUint(sqlr, &mindistance);
	ajSqlColumnToUint(sqlr, &maxdistance);
	ajSqlColumnToInt(sqlr, &priority);
	ajSqlColumnToStr(sqlr, &type);
	ajSqlColumnToUint(sqlr, &msid);
	ajSqlColumnToStr(sqlr, &msname);
	ajSqlColumnToStr(sqlr, &mssource);
	
	/*
	** Since the Ensembl SQL schema defines Sequence Region start and end
	** coordinates as unsigned integers for all Features, the range needs
	** checking.
	*/
	
	if(srstart <= INT_MAX)
	    slstart = (ajint) srstart;
	else
	    ajFatal("markerFeatureAdaptorFetchAllBySQL got a "
		    "Sequence Region start coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srstart, INT_MAX);
	
	if(srend <= INT_MAX)
	    slend = (ajint) srend;
	else
	    ajFatal("markerFeatureAdaptorFetchAllBySQL got a "
		    "Sequence Region end coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srend, INT_MAX);
	
	slstrand = 1;
	
	/* Fetch a Slice spanning the entire Sequence Region. */
	
	ensSliceAdaptorFetchBySeqRegionIdentifier(sa, srid, 0, 0, 0, &srslice);
	
	/*
	** Increase the reference counter of the Ensembl Assembly Mapper if
	** one has been specified, otherwise fetch it from the database if a
	** destination Slice has been specified.
	*/
	
	/*
	** FIXME: The Perl API does not retrieve an Assembly Mapper if it has
	** not been passed in!
	*/
	
	if(am)
	    am = ensAssemblyMapperNewRef(am);
	
	if((!am) &&
           slice &&
           (!ensCoordSystemMatch(ensSliceGetCoordSystem(slice),
				   ensSliceGetCoordSystem(srslice))))
	    am = ensAssemblyMapperAdaptorFetchByCoordSystems(
                ama,
                ensSliceGetCoordSystem(slice),
                ensSliceGetCoordSystem(srslice));
	
	/*
	** Remap the Feature coordinates to another Ensembl Coordinate System
	** if an Ensembl Assembly Mapper is defined at this point.
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
		
		ajStrDel(&lprimer);
		ajStrDel(&rprimer);
		ajStrDel(&type);
		ajStrDel(&msname);
		ajStrDel(&mssource);
		
		ensSliceDel(&srslice);
		
		ensAssemblyMapperDel(&am);
		
		ensMapperResultDel(&mr);
		
		continue;
	    }
	    
	    srid = ensMapperResultGetObjectIdentifier(mr);
	    
	    slstart = ensMapperResultGetStart(mr);
	    
	    slend = ensMapperResultGetEnd(mr);
	    
	    slstrand = ensMapperResultGetStrand(mr);
	    
	    /*
	    ** Delete the Sequence Region Slice and fetch a Slice in the
	    ** Coordinate System we just mapped to.
	    */
	    
	    ensSliceDel(&srslice);
	    
	    ensSliceAdaptorFetchBySeqRegionIdentifier(sa,
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
		ajFatal("markerFeatureAdaptorFetchAllBySQL got a Slice, "
			"whose length (%u) exceeds the "
			"maximum integer limit (%d).",
			ensSliceGetLength(slice), INT_MAX);
	    
	    /*
	    ** Nothing needs to be done if the destination Slice starts at 1
	    ** and is on the forward strand.
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
	    ** Throw away Features off the end of the requested Slice or on
	    ** any other than the requested Slice.
	    */
	    
	    if((slend < 1) ||
               (slstart > sllength) ||
               (srid != ensSliceGetSeqRegionIdentifier(slice)))
	    {
		/* Load the next Feature but destroy first! */
		
		ajStrDel(&lprimer);
		ajStrDel(&rprimer);
		ajStrDel(&type);
		ajStrDel(&msname);
		ajStrDel(&mssource);
		
		ensSliceDel(&srslice);
		
		ensAssemblyMapperDel(&am);
		
		continue;
	    }
	    
	    /* Delete the Sequence Region Slice and set the requested Slice. */
	    
	    ensSliceDel(&srslice);
	    
	    srslice = ensSliceNewRef(slice);
	}
	
	ensAnalysisAdaptorFetchByIdentifier(aa, analysisid, &analysis);
	
	feature = ensFeatureNewS(analysis, srslice, slstart, slend, slstrand);
	
	marker = (EnsPMarker) ajTableFetch(table, (void *) &markerid);
	
	if(!marker)
	{
	    if(msid)
		ms = ensMarkerSynonymNew(msid, mssource, msname);
	    
	    AJNEW0(Pidentifier);
	    
	    *Pidentifier = markerid;
	    
	    etype = ensMarkerTypeFromStr(type);
	    
	    marker = ensMarkerNew(ma,
				  markerid,
				  ms,
				  lprimer,
				  rprimer,
				  (AjPList) NULL,
				  (AjPList) NULL,
				  mindistance,
				  maxdistance,
				  etype,
				  priority);
	    
	    ajTablePut(table, (void *) Pidentifier, (void *) marker);
	    
	    ensMarkerSynonymDel(&ms);
	}
	
	mf = ensMarkerFeatureNew(mfa, identifier, feature, marker, mapweight);
	
	ajListPushAppend(mflist, (void *) mf);
	
	ensFeatureDel(&feature);
	
	ensAnalysisDel(&analysis);
	
	ajStrDel(&lprimer);
	ajStrDel(&rprimer);
	ajStrDel(&type);
	ajStrDel(&msname);
	ajStrDel(&mssource);
	
	ensSliceDel(&srslice);
	
	ensAssemblyMapperDel(&am);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajListFree(&mrs);
    
    ajTableMapDel(table, markerFeatureAdaptorClearMarkerTable, NULL);
    
    ajTableFree(&table);
    
    return ajTrue;
}




/* @funcstatic markerFeatureAdaptorCacheReference *****************************
**
** Wrapper function to reference an Ensembl Marker Feature
** from an Ensembl Cache.
**
** @param [r] value [void *] Ensembl Marker Feature
**
** @return [void *] Ensembl Marker Feature or NULL
** @@
******************************************************************************/

static void *markerFeatureAdaptorCacheReference(void *value)
{
    if(!value)
	return NULL;
    
    return (void *) ensMarkerFeatureNewRef((EnsPMarkerFeature) value);
}




/* @funcstatic markerFeatureAdaptorCacheDelete ********************************
**
** Wrapper function to delete an Ensembl Marker Feature
** from an Ensembl Cache.
**
** @param [r] value [void **] Ensembl Marker Feature address
**
** @return [void]
** @@
******************************************************************************/

static void markerFeatureAdaptorCacheDelete(void **value)
{
    if(!value)
	return;
    
    ensMarkerFeatureDel((EnsPMarkerFeature *) value);
    
    return;
}




/* @funcstatic markerFeatureAdaptorCacheSize **********************************
**
** Wrapper function to determine the memory size of an Ensembl Marker Feature
** via an Ensembl Cache.
**
** @param [r] value [const void*] Ensembl Marker Feature
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

static ajuint markerFeatureAdaptorCacheSize(const void *value)
{
    if(!value)
	return 0;
    
    return ensMarkerFeatureGetMemSize((const EnsPMarkerFeature) value);
}




/* @funcstatic markerFeatureAdaptorGetFeature *********************************
**
** Wrapper function to get the Ensembl Feature of an
** Ensembl Marker Feature from an Ensembl Feature Adaptor.
**
** @param [r] value [const void*] Ensembl Marker Feature
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

static EnsPFeature markerFeatureAdaptorGetFeature(const void *value)
{
    if(!value)
	return NULL;
    
    return ensMarkerFeatureGetFeature((const EnsPMarkerFeature) value);
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Marker Feature Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Marker Feature Adaptor. The target pointer does not need to be initialised
** to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPMarkerFeatureAdaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPMarkerFeatureAdaptor] Ensembl Marker
**                                                Feature Adaptor
** @argrule Ref object [EnsPMarkerFeatureAdaptor] Ensembl Marker
**                                                Feature Adaptor
**
** @valrule * [EnsPMarkerFeatureAdaptor] Ensembl Marker Feature Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensMarkerFeatureAdaptorNew *******************************************
**
** Default Ensembl Marker Feature Adaptor constructor.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPMarkerFeatureAdaptor] Ensembl Marker Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPMarkerFeatureAdaptor ensMarkerFeatureAdaptorNew(EnsPDatabaseAdaptor dba)
{
    EnsPMarkerFeatureAdaptor adaptor = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(adaptor);
    
    adaptor->Adaptor =
	ensFeatureAdaptorNew(dba,
			     markerFeatureAdaptorTables,
			     markerFeatureAdaptorColumns,
			     markerFeatureAdaptorLeftJoin,
			     markerFeatureAdaptorDefaultCondition,
			     markerFeatureAdaptorFinalCondition,
			     markerFeatureAdaptorFetchAllBySQL,
			     (void* (*)(const void* key)) NULL, /* Fread */
			     markerFeatureAdaptorCacheReference,
			     (AjBool (*)(const void* value)) NULL, /* Fwrite */
			     markerFeatureAdaptorCacheDelete,
			     markerFeatureAdaptorCacheSize,
			     markerFeatureAdaptorGetFeature,
			     "Marker Feature");
    
    return adaptor;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Marker Feature Adaptor.
**
** @fdata [EnsPMarkerFeatureAdaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Marker Feature Adaptor object.
**
** @argrule * Padaptor [EnsPMarkerFeatureAdaptor*] Ensembl Marker Feature
**                                                 Adaptor object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensMarkerFeatureAdaptorDel *******************************************
**
** Default destructor for an Ensembl Marker Feature Adaptor.
**
** @param [d] Padaptor [EnsPMarkerFeatureAdaptor*] Ensembl Marker Feature
**                                                 Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensMarkerFeatureAdaptorDel(EnsPMarkerFeatureAdaptor *Padaptor)
{
    EnsPMarkerFeatureAdaptor pthis = NULL;
    
    if(!Padaptor)
	return;
    
    if(!*Padaptor)
	return;

    pthis = *Padaptor;
    
    ensFeatureAdaptorDel(&pthis->Adaptor);
    
    AJFREE(pthis);

    *Padaptor = NULL;
    
    return;
}




/* @section object retrieval **************************************************
**
** Functions for retrieving Ensembl Marker Feature objects from an
** Ensembl Core database.
**
** @fdata [EnsPMarkerFeatureAdaptor]
** @fnote None
**
** @nam3rule Fetch Retrieve Ensembl Marker Feature object(s)
** @nam4rule FetchAll Retrieve all Ensembl Marker Feature objects
** @nam5rule FetchAllBy Retrieve all Ensembl Marker Feature objects
**                      matching a criterion
** @nam4rule FetchBy Retrieve one Ensembl Marker Feature object
**                   matching a criterion
**
** @argrule * adaptor [const EnsPMarkerFeature Adaptor] Ensembl Marker
**                                                      Feature Adaptor
** @argrule FetchAll [AjPList] AJAX List of Ensembl Marker Feature objects
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory use
******************************************************************************/




/* @func ensMarkerFeatureAdaptorFetchAllByMarker ******************************
**
** Fetch all Ensembl Marker Features by an Ensembl Marker.
**
** The caller is responsible for deleting the Ensembl Markers Features before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Map::DBSQL::MarkerFeatureAdaptor::fetch_all_by_Marker
** @param [r] adaptor [const EnsPMarkerFeatureAdaptor] Ensembl Marker
**                                                     Feature Adaptor
** @param [r] marker [const EnsPMarker] Ensembl Marker
** @param [u] mflist [AjPList] AJAX List of Ensembl Marker Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerFeatureAdaptorFetchAllByMarker(EnsPMarkerFeatureAdaptor adaptor,
                                               const EnsPMarker marker,
                                               AjPList mflist)
{
    AjBool value = AJFALSE;
    
    AjPStr constraint = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!marker)
	return ajFalse;
    
    if(!mflist)
	return ajFalse;
    
    ba = ensFeatureAdaptorGetBaseAdaptor(adaptor->Adaptor);
    
    constraint = ajFmtStr("marker.marker_id = %u", marker->Identifier);
    
    value = ensBaseAdaptorGenericFetch(ba,
				       constraint,
				       (EnsPAssemblyMapper) NULL,
				       (EnsPSlice) NULL,
				       mflist);
    
    ajStrDel(&constraint);
    
    return value;
}




/* @func ensMarkerFeatureAdaptorFetchAllBySlice *******************************
**
** Fetch all Ensembl Marker Features by an Ensembl Slice.
**
** The caller is responsible for deleting the Ensembl Markers Features before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Map::DBSQL::MarkerFeatureAdaptor::
**     fetch_all_by_Slice_and_priority
** @param [r] adaptor [const EnsPMarkerFeatureAdaptor] Ensembl Marker
**                                                     Feature Adaptor
** @param [r] slice [const EnsPSlice] Ensembl Slice
** @param [r] priority [ajint] Priority
** @param [r] mapweight [ajuint] Map weight
** @param [r] anname [const AjPStr] Ensembl Analysis name
** @param [u] mflist [AjPList] AJAX List of Ensembl Marker Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensMarkerFeatureAdaptorFetchAllBySlice(EnsPMarkerFeatureAdaptor adaptor,
                                              EnsPSlice slice,
                                              ajint priority,
                                              ajuint mapweight,
                                              const AjPStr anname,
                                              AjPList mflist)
{
    AjBool value = AJFALSE;
    
    AjPStr constraint = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!slice)
	return ajFalse;
    
    if(!mflist)
	return ajFalse;
    
    if(priority)
	constraint = ajFmtStr("marker.priority > %d", priority);
    
    if(mapweight)
    {
	if(constraint)
	    ajFmtPrintAppS(&constraint,
			   " AND marker_feature.map_weight < %u",
			   mapweight);
	else
	    constraint = ajFmtStr("marker_feature.map_weight < %u", mapweight);
    }
    
    value = ensFeatureAdaptorFetchAllBySliceConstraint(adaptor->Adaptor,
						       slice,
						       constraint,
						       anname,
						       mflist);
    
    ajStrDel(&constraint);
    
    return value;
}
