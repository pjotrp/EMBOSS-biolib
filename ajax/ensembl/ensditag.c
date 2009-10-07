/******************************************************************************
** @source Ensembl diTag functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.2 $
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

#include "ensditag.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

static const char *diTagFeatureSide[] =
{
    NULL,
    "L",
    "R",
    "F",
    NULL
};




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPAnalysisadaptor
ensRegistryGetAnalysisadaptor(EnsPDatabaseadaptor dba);

extern EnsPAssemblyMapperadaptor
ensRegistryGetAssemblyMapperadaptor(EnsPDatabaseadaptor dba);

extern EnsPDiTagFeatureadaptor
ensRegistryGetDiTagFeatureadaptor(EnsPDatabaseadaptor dba);

extern EnsPSliceadaptor
ensRegistryGetSliceadaptor(EnsPDatabaseadaptor dba);

static AjBool diTagadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                        const AjPStr statement,
                                        AjPList list);

static AjBool diTagFeatureadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblyMapper am,
                                               EnsPSlice slice,
                                               AjPList dtfs);

static void *diTagFeatureadaptorCacheReference(void *value);

static void diTagFeatureadaptorCacheDelete(void **value);

static ajuint diTagFeatureadaptorCacheSize(const void *value);

static EnsPFeature diTagFeatureadaptorGetFeature(const void *value);




/* @filesection ensditag ******************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPDiTag] DiTag *********************************************
**
** Functions for manipulating Ensembl DiTag objects
**
** @cc Bio::EnsEMBL::Map::Ditag CVS Revision: 1.4
**
** @nam2rule DiTag
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl DiTag by pointer.
** It is the responsibility of the user to first destroy any previous
** DiTags. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPDiTag]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPDiTag] Ensembl DiTag
** @argrule Ref object [EnsPDiTag] Ensembl DiTag
**
** @valrule * [EnsPDiTag] Ensembl DiTag
**
** @fcategory new
******************************************************************************/




/* @func ensDiTagNew **********************************************************
**
** Default constructor for an Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Storable
** @param [r] adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Map::Ditag::new
** @param [u] name [AjPStr] Name
** @param [u] type [AjPStr] Type
** @param [u] sequence [AjPStr] Sequence
** @param [r] count [ajuint] Count
**
** @return [EnsPDiTag] Ensembl DiTag or NULL
** @@
******************************************************************************/

EnsPDiTag ensDiTagNew(EnsPDatabaseadaptor adaptor,
                      ajuint identifier,
                      AjPStr name,
                      AjPStr type,
                      AjPStr sequence,
                      ajuint count)
{
    EnsPDiTag dt = NULL;
    
    AJNEW0(dt);
    
    dt->Use        = 1;
    dt->Identifier = identifier;
    dt->Adaptor    = adaptor;
    
    if(name)
        dt->Name = ajStrNewRef(name);
    
    if(type)
        dt->Type = ajStrNewRef(type);
    
    if(sequence)
        dt->Sequence = ajStrNewRef(sequence);
    
    dt->Count = count;
    
    return dt;
}




/* @func ensDiTagNewObj *******************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPDiTag] Ensembl DiTag
**
** @return [EnsPDiTag] Ensembl DiTag or NULL
** @@
******************************************************************************/

EnsPDiTag ensDiTagNewObj(const EnsPDiTag object)
{
    EnsPDiTag dt = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(dt);
    
    dt->Use        = 1;
    dt->Identifier = object->Identifier;
    dt->Adaptor    = object->Adaptor;
    
    if(object->Name)
        dt->Name = ajStrNewRef(object->Name);
    
    if(object->Type)
        dt->Type = ajStrNewRef(object->Type);
    
    if(object->Sequence)
        dt->Sequence = ajStrNewRef(object->Sequence);
    
    dt->Count = object->Count;
    
    return dt;
}




/* @func ensDiTagNewRef *******************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] dt [EnsPDiTag] Ensembl DiTag
**
** @return [EnsPDiTag] Ensembl DiTag or NULL
** @@
******************************************************************************/

EnsPDiTag ensDiTagNewRef(EnsPDiTag dt)
{
    if(!dt)
	return NULL;
    
    dt->Use++;
    
    return dt;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl DiTag.
**
** @fdata [EnsPDiTag]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl DiTag object
**
** @argrule * Pdt [EnsPDiTag*] Ensembl DiTag object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensDiTagDel **********************************************************
**
** Default destructor for an Ensembl DiTag.
**
** @param [d] Pdt [EnsPDiTag*] Ensembl DiTag address
**
** @return [void]
** @@
******************************************************************************/

void ensDiTagDel(EnsPDiTag *Pdt)
{
    EnsPDiTag pthis = NULL;
    /*
     ajDebug("ensDiTagDel\n"
	     "  *Pdt %p\n",
	     *Pdt);
     
     ensDiTagTrace(*Pdt, 1);
     */
    
    if(!Pdt)
        return;
    
    if(!*Pdt)
        return;

    pthis = *Pdt;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pdt = NULL;
	
	return;
    }
    
    ajStrDel(&pthis->Name);
    ajStrDel(&pthis->Type);
    ajStrDel(&pthis->Sequence);
    
    AJFREE(*Pdt);
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl DiTag object.
**
** @fdata [EnsPDiTag]
** @fnote None
**
** @nam3rule Get Return DiTag attribute(s)
** @nam4rule Getadaptor Return the Ensembl Database Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetName Return the name
** @nam4rule GetType Return the type
** @nam4rule GetSequence Return the sequence
** @nam4rule GetCount Return the count
**
** @argrule * dt [const EnsPDiTag] DiTag
**
** @valrule Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Name [AjPStr] Name
** @valrule Type [AjPStr] Type
** @valrule Sequence [AjPStr] Sequence
** @valrule Count [ajuint] Count
**
** @fcategory use
******************************************************************************/




/* @func ensDiTagGetadaptor ***************************************************
**
** Get the Ensembl Database Adaptor element of an Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] dt [const EnsPDiTag] Ensembl DiTag
**
** @return [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

EnsPDatabaseadaptor ensDiTagGetadaptor(const EnsPDiTag dt)
{
    if(!dt)
        return NULL;
    
    return dt->Adaptor;
}




/* @func ensDiTagGetIdentifier ************************************************
**
** Get the SQL database-internal identifier element of an Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] dt [const EnsPDiTag] Ensembl DiTag
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensDiTagGetIdentifier(const EnsPDiTag dt)
{
    if(!dt)
        return 0;
    
    return dt->Identifier;
}




/* @func ensDiTagGetName ******************************************************
**
** Get the name element of an Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Map::Ditag::name
** @param [r] dt [const EnsPDiTag] Ensembl DiTag
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensDiTagGetName(const EnsPDiTag dt)
{
    if(!dt)
        return NULL;
    
    return dt->Name;
}




/* @func ensDiTagGetType ******************************************************
**
** Get the type element of an Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Map::Ditag::type
** @param [r] dt [const EnsPDiTag] Ensembl DiTag
**
** @return [AjPStr] Type
** @@
******************************************************************************/

AjPStr ensDiTagGetType(const EnsPDiTag dt)
{
    if(!dt)
        return NULL;
    
    return dt->Type;
}




/* @func ensDiTagGetSequence **************************************************
**
** Get the sequence element of an Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Map::Ditag::sequence
** @param [r] dt [const EnsPDiTag] Ensembl DiTag
**
** @return [AjPStr] Sequence
** @@
******************************************************************************/

AjPStr ensDiTagGetSequence(const EnsPDiTag dt)
{
    if(!dt)
        return NULL;
    
    return dt->Sequence;
}




/* @func ensDiTagGetCount *****************************************************
**
** Get the count element of an Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Map::Ditag::tag_count
** @param [r] dt [const EnsPDiTag] Ensembl DiTag
**
** @return [ajuint] Count
** @@
******************************************************************************/

ajuint ensDiTagGetCount(const EnsPDiTag dt)
{
    if(!dt)
        return 0;
    
    return dt->Count;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl DiTag object.
**
** @fdata [EnsPDiTag]
** @fnote None
**
** @nam3rule Set Set one element of an Ensembl DiTag
** @nam4rule Setadaptor Set the Ensembl Database Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetName Set the name
** @nam4rule SetType Set the type
** @nam4rule SetSequence Set the sequence
** @nam4rule SetCount Set the count
**
** @argrule * dt [EnsPDiTag] Ensembl DiTag object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensDiTagSetadaptor ***************************************************
**
** Set the Ensembl Database Adaptor element of an Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] dt [EnsPDiTag] Ensembl DiTag
** @param [r] adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagSetadaptor(EnsPDiTag dt, EnsPDatabaseadaptor adaptor)
{
    if(!dt)
        return ajFalse;
    
    dt->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensDiTagSetIdentifier ************************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] dt [EnsPDiTag] Ensembl DiTag
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagSetIdentifier(EnsPDiTag dt, ajuint identifier)
{
    if(!dt)
        return ajFalse;
    
    dt->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensDiTagSetName ******************************************************
**
** Set the name element of an Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Map::Ditag::name
** @param [u] dt [EnsPDiTag] Ensembl DiTag
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagSetName(EnsPDiTag dt, AjPStr name)
{
    if(!dt)
        return ajFalse;
    
    ajStrDel(&dt->Name);
    
    dt->Name = ajStrNewRef(name);
    
    return ajTrue;
}




/* @func ensDiTagSetType ******************************************************
**
** Set the type element of an Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Map::Ditag::type
** @param [u] dt [EnsPDiTag] Ensembl DiTag
** @param [u] type [AjPStr] Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagSetType(EnsPDiTag dt, AjPStr type)
{
    if(!dt)
        return ajFalse;
    
    ajStrDel(&dt->Type);
    
    dt->Type = ajStrNewRef(type);
    
    return ajTrue;
}




/* @func ensDiTagSetSequence **************************************************
**
** Set the sequence element of an Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Map::Ditag::sequence
** @param [u] dt [EnsPDiTag] Ensembl DiTag
** @param [u] sequence [AjPStr] Sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagSetSequence(EnsPDiTag dt, AjPStr sequence)
{
    if(!dt)
        return ajFalse;
    
    ajStrDel(&dt->Sequence);
    
    dt->Sequence = ajStrNewRef(sequence);
    
    return ajTrue;
}




/* @func ensDiTagSetCount *****************************************************
**
** Set the count element of an Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Map::Ditag::tag_count
** @param [u] dt [EnsPDiTag] Ensembl DiTag
** @param [r] count [ajuint] Count
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagSetCount(EnsPDiTag dt, ajuint count)
{
    if(!dt)
        return ajFalse;
    
    dt->Count = count;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl DiTag object.
**
** @fdata [EnsPDiTag]
** @nam3rule Trace Report Ensembl DiTag elements to debug file.
**
** @argrule Trace dt [const EnsPDiTag] Ensembl DiTag
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensDiTagTrace ********************************************************
**
** Trace an Ensembl DiTag.
**
** @param [r] dt [const EnsPDiTag] Ensembl DiTag
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagTrace(const EnsPDiTag dt, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!dt)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensDiTagTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Name '%S'\n"
	    "%S  Type '%S'\n"
	    "%S  Sequence '%S'\n"
	    "%S  Count %u\n",
	    indent, dt,
	    indent, dt->Use,
	    indent, dt->Identifier,
	    indent, dt->Adaptor,
	    indent, dt->Name,
	    indent, dt->Type,
	    indent, dt->Sequence,
	    indent, dt->Count);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensDiTagGetMemSize ***************************************************
**
** Get the memory size in bytes of an Ensembl DiTag.
**
** @param [r] dt [const EnsPDiTag] Ensembl DiTag
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensDiTagGetMemSize(const EnsPDiTag dt)
{
    ajuint size = 0;
    
    if(!dt)
	return 0;
    
    size += (ajuint) sizeof (EnsODiTag);
    
    if(dt->Name)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(dt->Name);
    }
    
    if(dt->Type)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(dt->Type);
    }
    
    if(dt->Sequence)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(dt->Sequence);
    }
    
    return size;
}




/* @datasection [EnsPDatabaseadaptor] Ensembl Database Adaptor ****************
**
** Functions for manipulating Ensembl DiTag Adaptor objects
**
** @cc Bio::EnsEMBL::Map::DBSQL::Ditagadaptor CVS Revision: 1.6
**
** @nam2rule DiTagadaptor
**
******************************************************************************/



/* @funcstatic diTagadaptorFetchAllBySQL **************************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl DiTag objects.
**
** @cc Bio::EnsEMBL::Map::DBSQL::Ditagadaptor::_fetch
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] list [AjPList] AJAX List of Ensembl DiTags
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool diTagadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                        const AjPStr statement,
                                        AjPList list)
{
    ajuint identifier = 0;
    ajuint count      = 0;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr name     = NULL;
    AjPStr type     = NULL;
    AjPStr sequence = NULL;
    
    EnsPDiTag dt = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!(statement && ajStrGetLen(statement)))
	return ajFalse;
    
    if(!list)
	return ajFalse;
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier = 0;
	name       = ajStrNew();
	type       = ajStrNew();
	sequence   = ajStrNew();
	count      = 0;
	
        sqlr = ajSqlrowiterGet(sqli);
	
        ajSqlcolumnToUint(sqlr, &identifier);
	ajSqlcolumnToStr(sqlr, &name);
	ajSqlcolumnToStr(sqlr, &type);
	ajSqlcolumnToStr(sqlr, &sequence);
        ajSqlcolumnToUint(sqlr, &count);
	
	dt = ensDiTagNew(dba, identifier, name, type, sequence, count);
	
	ajListPushAppend(list, (void *) dt);
	
	ajStrDel(&name);
	ajStrDel(&type);
	ajStrDel(&sequence);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    return ajTrue;
}




/* @section object retrieval **************************************************
**
** Functions for retrieving Ensembl DiTag objects from an
** Ensembl Core database.
**
** @fdata [EnsPDatabaseadaptor]
** @fnote None
**
** @nam3rule Fetch Retrieve Ensembl DiTag object(s)
** @nam4rule FetchAll Retrieve all Ensembl DiTag objects
** @nam5rule FetchAllBy Retrieve all Ensembl DiTag objects
**                      matching a criterion
** @nam4rule FetchBy Retrieve one Ensembl DiTag object
**                   matching a criterion
**
** @argrule * adaptor [const EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule FetchAll [AjPList] AJAX List of Ensembl DiTag objects
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory use
******************************************************************************/




/* @func ensDiTagadaptorFetchByIdentifier *************************************
**
** Fetch an Ensembl DiTag via its SQL database-internal identifier.
**
** The caller is responsible for deleting the Ensembl DiTag.
**
** @cc Bio::EnsEMBL::Map::DBSQL::Ditagadaptor::fetch_by_dbID
** @param [r] dba [const EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pdt [EnsPDiTag*] Ensembl DiTag address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagadaptorFetchByIdentifier(EnsPDatabaseadaptor dba,
                                        ajuint identifier,
                                        EnsPDiTag *Pdt)
{
    AjBool value = AJFALSE;
    
    AjPList dts = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDiTag dt = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pdt)
	return ajFalse;
    
    *Pdt = NULL;
    
    statement = ajFmtStr("SELECT "
			 "ditag.ditag_id, "
			 "ditag.name, "
			 "ditag.type, "
			 "ditag.tag_count, "
			 "ditag.sequence "
			 "FROM "
			 "ditag "
			 "WHERE "
			 "ditag.ditag_id = %u",
			 identifier);
    
    dts = ajListNew();
    
    value = diTagadaptorFetchAllBySQL(dba, statement, dts);
    
    if(ajListGetLength(dts) == 0)
	ajDebug("ensDiTagadaptorFetchByIdentifier got no Ensembl DiTag "
		"for identifier %u.\n", identifier);
    
    if(ajListGetLength(dts) > 1)
	ajWarn("ensDiTagadaptorFetchByIdentifier got more than one "
	       "Ensembl DiTag for identifier %u.\n", identifier);
    
    ajListPop(dts, (void **) Pdt);
    
    while(ajListPop(dts, (void **) &dt))
	ensDiTagDel(&dt);
    
    ajListFree(&dts);
    
    ajStrDel(&statement);
    
    return value;
}




/* @func ensDiTagadaptorFetchAll **********************************************
**
** Fetch all Ensembl DiTag objects by name or type.
**
** The caller is responsible for deleting the Ensembl DiTags before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Map::DBSQL::Ditagadaptor::fetch_all
** @cc Bio::EnsEMBL::Map::DBSQL::Ditagadaptor::fetch_all_by_name
** @cc Bio::EnsEMBL::Map::DBSQL::Ditagadaptor::fetch_all_by_type
** @param [r] dba [const EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [rN] name [const AjPStr] Name
** @param [rN] type [const AjPStr] Type
** @param [u] dts [AjPList] AJAX List of Ensembl DiTags
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagadaptorFetchAll(EnsPDatabaseadaptor dba,
                               const AjPStr name,
                               const AjPStr type,
                               AjPList dts)
{
    char *txtname = NULL;
    char *txttype = NULL;
    
    AjBool value = AJFALSE;
    
    AjPStr statement = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!dts)
	return ajFalse;
    
    if(name && ajStrGetLen(name))
	ensDatabaseadaptorEscapeC(dba, &txtname, name);
    
    if(type && ajStrGetLen(type))
	ensDatabaseadaptorEscapeC(dba, &txttype, type);
    
    statement = ajStrNewC("SELECT "
			  "ditag.ditag_id, "
			  "ditag.name, "
			  "ditag.type, "
			  "ditag.tag_count, "
			  "ditag.sequence "
			  "FROM "
			  "ditag");
    
    if(txtname || txttype)
	ajStrAppendC(&statement, " WHERE");
    
    if(txtname)
	ajFmtPrintAppS(&statement, " ditag.name = '%s'", txtname);
    
    if(txttype)
    {
	if(txtname)
	    ajStrAppendC(&statement, " AND");
	
	ajFmtPrintAppS(&statement, " ditag.type = '%s'", txttype);
    }
    
    ajCharDel(&txtname);
    
    ajCharDel(&txttype);
    
    value = diTagadaptorFetchAllBySQL(dba, statement, dts);
    
    ajStrDel(&statement);
    
    return value;
}




/* @func ensDiTagadaptorFetchAllByName ****************************************
**
** Fetch all Ensembl DiTag objects by name.
**
** The caller is responsible for deleting the Ensembl DiTags before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Map::DBSQL::Ditagadaptor::fetch_all_by_name
** @param [r] dba [const EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] name [const AjPStr] Name
** @param [u] dts [AjPList] AJAX List of Ensembl DiTags
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagadaptorFetchAllByName(EnsPDatabaseadaptor dba,
                                     const AjPStr name,
                                     AjPList dts)
{
    char *txtname = NULL;
    
    AjBool value = AJFALSE;
    
    AjPStr statement = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    if(!dts)
	return ajFalse;
    
    ensDatabaseadaptorEscapeC(dba, &txtname, name);
    
    statement = ajFmtStr("SELECT "
			 "ditag.ditag_id, "
			 "ditag.name, "
			 "ditag.type, "
			 "ditag.tag_count, "
			 "ditag.sequence "
			 "FROM "
			 "ditag "
			 "WHERE "
			 "ditag.name = '%s'",
			 txtname);
    
    ajCharDel(&txtname);
    
    value = diTagadaptorFetchAllBySQL(dba, statement, dts);
    
    ajStrDel(&statement);
    
    return value;
}




/* @func ensDiTagadaptorFetchAllByType ****************************************
**
** Fetch all Ensembl DiTag objects by type.
**
** The caller is responsible for deleting the Ensembl DiTags before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Map::DBSQL::Ditagadaptor::fetch_all_by_type
** @param [r] dba [const EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] type [const AjPStr] Type
** @param [u] dts [AjPList] AJAX List of Ensembl DiTags
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagadaptorFetchAllByType(EnsPDatabaseadaptor dba,
                                     const AjPStr type,
                                     AjPList dts)
{
    char *txttype = NULL;
    
    AjBool value = AJFALSE;
    
    AjPStr statement = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!type)
	return ajFalse;
    
    if(!dts)
	return ajFalse;
    
    ensDatabaseadaptorEscapeC(dba, &txttype, type);
    
    statement = ajFmtStr("SELECT "
			 "ditag.ditag_id, "
			 "ditag.name, "
			 "ditag.type, "
			 "ditag.tag_count, "
			 "ditag.sequence "
			 "FROM "
			 "ditag "
			 "WHERE "
			 "ditag.type = '%s'",
			 txttype);
    
    ajCharDel(&txttype);
    
    value = diTagadaptorFetchAllBySQL(dba, statement, dts);
    
    ajStrDel(&statement);
    
    return value;
}




/* @datasection [EnsPDiTagFeature] DiTag Feature ******************************
**
** Functions for manipulating Ensembl DiTag Feature objects
**
** @cc Bio::EnsEMBL::Map::DitagFeature CVS Revision: 1.11
**
** @nam2rule DiTagFeature
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl DiTag Feature by pointer.
** It is the responsibility of the user to first destroy any previous
** DiTag Features. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPDiTagFeature]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPDiTagFeature] Ensembl DiTag Feature
** @argrule Ref object [EnsPDiTagFeature] Ensembl DiTag Feature
**
** @valrule * [EnsPDiTagFeature] Ensembl DiTag Feature or NULL
**
** @fcategory new
******************************************************************************/




/* @func ensDiTagFeatureNew ***************************************************
**
** Default constructor for an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Storable
** @param [r] adaptor [EnsPDiTagFeatureadaptor] Ensembl DiTag Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Feature::new
** @param [u] feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Map::DitagFeature::new
** @param [u] dt [EnsPDiTag] Ensembl DiTag
** @param [u] cigar [AjPStr] CIGAR line
** @param [r] side [AjEnum] Side
** @param [r] tstart [ajint] Target start
** @param [r] tend [ajint] Target end
** @param [r] tstrand [ajint] Target strand
** @param [r] pairid [ajuint] Pair identifier
**
** @return [EnsPDiTag] Ensembl DiTag or NULL
** @@
******************************************************************************/

EnsPDiTagFeature ensDiTagFeatureNew(EnsPDiTagFeatureadaptor adaptor,
                                    ajuint identifier,
                                    EnsPFeature feature,
                                    EnsPDiTag dt,
                                    AjPStr cigar,
                                    AjEnum side,
                                    ajint tstart,
                                    ajint tend,
                                    ajint tstrand,
                                    ajuint pairid)
{
    EnsPDiTagFeature dtf = NULL;
    
    if(!feature)
	return NULL;
    
    if((tstrand < -1) || (tstrand > 1))
    {
	ajDebug("ensDiTagFeatureNew got target strand not -1, 0 or +1.\n");
	
	return NULL;
    }
    
    AJNEW0(dtf);
    
    dtf->Use        = 1;
    dtf->Identifier = identifier;
    dtf->Adaptor    = adaptor;
    dtf->Feature    = ensFeatureNewRef(feature);
    dtf->DiTag      = ensDiTagNewRef(dt);
    
    if(cigar)
        dtf->Cigar = ajStrNewRef(cigar);
    
    dtf->Side           = side;
    dtf->TargetStart    = tstart;
    dtf->TargetEnd      = tend;
    dtf->TargetStrand   = tstrand;
    dtf->PairIdentifier = pairid;
    
    return dtf;
}




/* @func ensDiTagFeatureNewObj ************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [EnsPDiTagFeature] Ensembl DiTag Feature or NULL
** @@
******************************************************************************/

EnsPDiTagFeature ensDiTagFeatureNewObj(const EnsPDiTagFeature object)
{
    EnsPDiTagFeature dtf = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(dtf);
    
    dtf->Use        = 1;
    dtf->Identifier = object->Identifier;
    dtf->Adaptor    = object->Adaptor;
    dtf->Feature    = ensFeatureNewRef(object->Feature);
    dtf->DiTag      = ensDiTagNewRef(object->DiTag);
    
    if (object->Cigar)
        dtf->Cigar = ajStrNewRef(object->Cigar);
    
    dtf->Side           = object->Side;
    dtf->TargetStart    = object->TargetStart;
    dtf->TargetEnd      = object->TargetEnd;
    dtf->TargetStrand   = object->TargetStrand;
    dtf->PairIdentifier = object->PairIdentifier;
    
    return dtf;
}




/* @func ensDiTagFeatureNewRef ************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] dtf [EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [EnsPDiTagFeature] Ensembl DiTag Feature or NULL
** @@
******************************************************************************/

EnsPDiTagFeature ensDiTagFeatureNewRef(EnsPDiTagFeature dtf)
{
    if(!dtf)
	return NULL;
    
    dtf->Use++;
    
    return dtf;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl DiTag Feature.
**
** @fdata [EnsPDiTagFeature]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl DiTag Feature object
**
** @argrule * Pdtf [EnsPDiTagFeature*] Ensembl DiTag Feature object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensDiTagFeatureDel ***************************************************
**
** Default destructor for an Ensembl DiTag Feature.
**
** @param [d] Pdtf [EnsPDiTagFeature*] Ensembl DiTag Feature address
**
** @return [void]
** @@
******************************************************************************/

void ensDiTagFeatureDel(EnsPDiTagFeature *Pdtf)
{
    EnsPDiTagFeature pthis = NULL;
    
    /*
     ajDebug("ensDiTagFeatureDel\n"
	     "  *Pdtf %p\n",
	     *Pdtf);
     
     ensDiTagFeatureTrace(*Pdtf, 1);
     */
    
    if(!Pdtf)
        return;
    
    if(!*Pdtf)
        return;

    pthis = *Pdtf;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pdtf = NULL;
	
	return;
    }
    
    ensFeatureDel(&pthis->Feature);
    
    ensDiTagDel(&pthis->DiTag);
    
    ajStrDel(&pthis->Cigar);
    
    AJFREE(pthis);

    *Pdtf = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl DiTag Feature object.
**
** @fdata [EnsPDiTagFeature]
** @fnote None
**
** @nam3rule Get Return DiTag Feature attribute(s)
** @nam4rule Getadaptor Return the Ensembl DiTag Feature Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetFeature Return the Ensembl Feature
** @nam4rule GetDiTag Return the Ensembl DiTag
** @nam4rule GetCigar Return the CIGAR line
** @nam4rule GetSide Return the side
** @nam4rule GetTartgetStart Return the target start
** @nam4rule GetTargetEnd Return the target end
** @nam4rule GetTargetStrand Return the target strand
** @nam4rule GetPairIdentifier Return the pair identifier
**
** @argrule * dtf [const EnsPDiTagFeature] DiTag Feature
**
** @valrule Adaptor [EnsPDiTagFeatureadaptor] Ensembl DiTag Feature Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Feature [EnsPFeature] Ensembl Feature
** @valrule DiTag [EnsPDiTag] Ensembl DiTag
** @valrule Cigar [AjPStr] CIGAR line
** @valrule Side [AjEnum] Side
** @valrule TargetStart [ajint] Target start
** @valrule TargetEnd [ajint] Target end
** @valrule TargetStrand [ajint] Target strand
** @valrule PairIdentifier [ajuint] Pair identifier
**
** @fcategory use
******************************************************************************/




/* @func ensDiTagFeatureGetadaptor ********************************************
**
** Get the Ensembl DiTag Feature Adaptor element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [EnsPDiTagFeatureadaptor] Ensembl DiTag Feature Adaptor
** @@
******************************************************************************/

EnsPDiTagFeatureadaptor ensDiTagFeatureGetadaptor(const EnsPDiTagFeature dtf)
{
    if(!dtf)
        return NULL;
    
    return dtf->Adaptor;
}




/* @func ensDiTagFeatureGetIdentifier *****************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensDiTagFeatureGetIdentifier(const EnsPDiTagFeature dtf)
{
    if(!dtf)
        return 0;
    
    return dtf->Identifier;
}




/* @func ensDiTagFeatureGetFeature ********************************************
**
** Get the Ensembl Feature element of an Ensembl DiTag Feature.
**
** @param [r] dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

EnsPFeature ensDiTagFeatureGetFeature(const EnsPDiTagFeature dtf)
{
    if(!dtf)
        return NULL;
    
    return dtf->Feature;
}




/* @func ensDiTagFeatureGetDiTag **********************************************
**
** Get the Ensembl DiTag element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::ditag
** @param [r] dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [EnsPDiTag] Ensembl DiTag
** @@
******************************************************************************/

EnsPDiTag ensDiTagFeatureGetDiTag(const EnsPDiTagFeature dtf)
{
    if(!dtf)
        return NULL;
    
    return dtf->DiTag;
}




/* @func ensDiTagFeatureGetCigar **********************************************
**
** Get the CIGAR line element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::cigar_line
** @param [r] dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [AjPStr] CIGAR line
** @@
******************************************************************************/

AjPStr ensDiTagFeatureGetCigar(const EnsPDiTagFeature dtf)
{
    if(!dtf)
        return NULL;
    
    return dtf->Cigar;
}




/* @func ensDiTagFeatureGetSide ***********************************************
**
** Get the side element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::ditag_side
** @param [r] dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [AjEnum] Side
** @@
******************************************************************************/

AjEnum ensDiTagFeatureGetSide(const EnsPDiTagFeature dtf)
{
    if(!dtf)
        return ensEDiTagFeatureSideNULL;
    
    return dtf->Side;
}




/* @func ensDiTagFeatureGetTargetStart ****************************************
**
** Get the target start element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::hit_start
** @param [r] dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [ajint] Target start
** @@
******************************************************************************/

ajint ensDiTagFeatureGetTargetStart(const EnsPDiTagFeature dtf)
{
    if(!dtf)
        return 0;
    
    return dtf->TargetStart;
}




/* @func ensDiTagFeatureGetTargetEnd ******************************************
**
** Get the target end element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::hit_end
** @param [r] dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [ajint] Target end
** @@
******************************************************************************/

ajint ensDiTagFeatureGetTargetEnd(const EnsPDiTagFeature dtf)
{
    if(!dtf)
        return 0;
    
    return dtf->TargetEnd;
}




/* @func ensDiTagFeatureGetTargetStrand ***************************************
**
** Get the target strand element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::hit_strand
** @param [r] dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [ajint] Target strand
** @@
******************************************************************************/

ajint ensDiTagFeatureGetTargetStrand(const EnsPDiTagFeature dtf)
{
    if(!dtf)
        return 0;
    
    return dtf->TargetStrand;
}




/* @func ensDiTagFeatureGetPairIdentifier *************************************
**
** Get the pair identifier element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::ditag_pair_id
** @param [r] dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [ajuint] Pair identifier
** @@
******************************************************************************/

ajuint ensDiTagFeatureGetPairIdentifier(const EnsPDiTagFeature dtf)
{
    if(!dtf)
        return 0;
    
    return dtf->PairIdentifier;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl DiTag Feature object.
**
** @fdata [EnsPDiTagFeature]
** @fnote None
**
** @nam3rule Set Set one element of an Ensembl DiTag Feature
** @nam4rule Setadaptor Set the Ensembl DiTag Feature Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetFeature Set the Ensembl Feature
** @nam4rule SetDiTag Set the Ensembl DiTag
** @nam4rule SetCigar Set the CIGAR line
** @nam4rule SetSide Set the side
** @nam4rule SetTargetStart Set the target start
** @nam4rule SetTargetEnd Set the target end
** @nam4rule SetTargetStrand Set the target strand
**
** @argrule * dtf [EnsPDiTagFeature] Ensembl DiTag Feature object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensDiTagFeatureSetadaptor ********************************************
**
** Set the Ensembl DiTag Feature Adaptor element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] dtf [EnsPDiTagFeature] Ensembl DiTag Feature
** @param [r] adaptor [EnsPDiTagFeatureadaptor] Ensembl DiTag Feature Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureSetadaptor(EnsPDiTagFeature dtf,
                                 EnsPDiTagFeatureadaptor adaptor)
{
    if(!dtf)
        return ajFalse;
    
    dtf->Adaptor = adaptor;
    
    return ajTrue;
}




/* @func ensDiTagFeatureSetIdentifier *****************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] dtf [EnsPDiTagFeature] Ensembl DiTag Feature
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureSetIdentifier(EnsPDiTagFeature dtf, ajuint identifier)
{
    if(!dtf)
        return ajFalse;
    
    dtf->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensDiTagFeatureSetFeature ********************************************
**
** Set the Ensembl Feature element of an Ensembl DiTag Feature.
**
** @param [u] dtf [EnsPDiTagFeature] Ensembl DiTag Feature
** @param [u] feature [EnsPFeaturer] Ensembl Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureSetFeature(EnsPDiTagFeature dtf, EnsPFeature feature)
{
    if(!dtf)
        return ajFalse;
    
    ensFeatureDel(&(dtf->Feature));
    
    dtf->Feature = ensFeatureNewRef(feature);
    
    return ajTrue;
}




/* @func ensDiTagFeatureSetDiTag **********************************************
**
** Set the Ensembl DiTag element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::ditag
** @param [u] dtf [EnsPDiTagFeature] Ensembl DiTag Feature
** @param [u] dt [EnsPDiTag] Ensembl DiTag
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureSetDiTag(EnsPDiTagFeature dtf, EnsPDiTag dt)
{
    if(!dtf)
        return ajFalse;
    
    ensDiTagDel(&(dtf->DiTag));
    
    dtf->DiTag = ensDiTagNewRef(dt);
    
    return ajTrue;
}




/* @func ensDiTagFeatureSetCigar **********************************************
**
** Set the CIGAR line element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::cigar_line
** @param [u] dtf [EnsPDiTagFeature] Ensembl DiTag Feature
** @param [u] cigar [AjPStr] CIGAR line
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureSetCigar(EnsPDiTagFeature dtf, AjPStr cigar)
{
    if(!dtf)
        return ajFalse;
    
    ajStrDel(&dtf->Cigar);
    
    dtf->Cigar = ajStrNewRef(cigar);
    
    return ajTrue;
}




/* @func ensDiTagFeatureSetSide ***********************************************
**
** Set the side element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::ditag_side
** @param [u] dtf [EnsPDiTagFeature] Ensembl DiTag Feature
** @param [r] side [AjEnum] Side
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureSetSide(EnsPDiTagFeature dtf, AjEnum side)
{
    if(!dtf)
        return ajFalse;
    
    dtf->Side = side;
    
    return ajTrue;
}




/* @func ensDiTagFeatureSetTargetStart ****************************************
**
** Set the target start element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::hit_start
** @param [u] dtf [EnsPDiTagFeature] Ensembl DiTag Feature
** @param [r] tstart [ajint] Target start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureSetTargetStart(EnsPDiTagFeature dtf, ajint tstart)
{
    if(!dtf)
        return ajFalse;
    
    dtf->TargetStart = tstart;
    
    return ajTrue;
}




/* @func ensDiTagFeatureSetTargetEnd ******************************************
**
** Set the target end element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::hit_end
** @param [u] dtf [EnsPDiTagFeature] Ensembl DiTag Feature
** @param [r] tend [ajint] Target end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureSetTargetEnd(EnsPDiTagFeature dtf, ajint tend)
{
    if(!dtf)
        return ajFalse;
    
    dtf->TargetEnd = tend;
    
    return ajTrue;
}




/* @func ensDiTagFeatureSetTargetStrand ***************************************
**
** Set the target strand element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::hit_strand
** @param [u] dtf [EnsPDiTagFeature] Ensembl DiTag Feature
** @param [r] tstrand [ajint] Target strand
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureSetTargetStrand(EnsPDiTagFeature dtf, ajint tstrand)
{
    if(!dtf)
        return ajFalse;
    
    dtf->TargetStrand = tstrand;
    
    return ajTrue;
}




/* @func ensDiTagFeatureSetPairIdentifier *************************************
**
** Set the pair identifier element of an Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DitagFeature::ditag_pair_id
** @param [u] dtf [EnsPDiTagFeature] Ensembl DiTag Feature
** @param [r] pairid [ajuint] Pair identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureSetPairIdentifier(EnsPDiTagFeature dtf, ajuint pairid)
{
    if(!dtf)
        return ajFalse;
    
    dtf->PairIdentifier = pairid;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl DiTag Feature object.
**
** @fdata [EnsPDiTagFeature]
** @nam3rule Trace Report Ensembl DiTag Feature elements to debug file.
**
** @argrule Trace dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensDiTagFeatureTrace *************************************************
**
** Trace an Ensembl DiTag Feature.
**
** @param [r] dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureTrace(const EnsPDiTagFeature dtf, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!dtf)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensDiTagFeatureTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Feature %p\n"
	    "%S  DiTag %p\n"
	    "%S  Cigar '%S'\n"
	    "%S  Side '%s'\n"
	    "%S  TargetStart %d\n"
	    "%S  TargetEnd %d\n"
	    "%S  TargetStrand %d\n"
	    "%S  PairIdentifier %u\n",
	    indent, dtf,
	    indent, dtf->Use,
	    indent, dtf->Identifier,
	    indent, dtf->Adaptor,
	    indent, dtf->Feature,
	    indent, dtf->DiTag,
	    indent, dtf->Cigar,
	    indent, ensDiTagFeatureSideToChar(dtf->Side),
	    indent, dtf->TargetStart,
	    indent, dtf->TargetEnd,
	    indent, dtf->TargetStrand,
	    indent, dtf->PairIdentifier);
    
    ensFeatureTrace(dtf->Feature, level + 1);
    
    ensDiTagTrace(dtf->DiTag, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensDiTagFeatureGetMemSize ********************************************
**
** Get the memory size in bytes of an Ensembl DiTag Feature.
**
** @param [r] dtf [const EnsPDiTagFeature] Ensembl DiTag Feature
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensDiTagFeatureGetMemSize(const EnsPDiTagFeature dtf)
{
    ajuint size = 0;
    
    if(!dtf)
	return 0;
    
    size += (ajuint) sizeof (EnsODiTagFeature);
    
    size += ensFeatureGetMemSize(dtf->Feature);
    
    size += ensDiTagGetMemSize(dtf->DiTag);
    
    if(dtf->Cigar)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(dtf->Cigar);
    }
    
    return size;
}




/* @func ensDiTagFeatureSideFromStr *******************************************
**
** Convert an AJAX String into an Ensembl DiTag Feature side element.
**
** @param [r] side [const AjPStr] Ensembl DiTag Feature side string
**
** @return [AjEnum] Ensembl DiTag Feature side element or
**                  ensEDiTagFeatureSideNULL
** @@
******************************************************************************/

AjEnum ensDiTagFeatureSideFromStr(const AjPStr side)
{
    register ajint i = 0;
    
    AjEnum eside = ensEDiTagFeatureSideNULL;
    
    for(i = 1; diTagFeatureSide[i]; i++)
	if (ajStrMatchC(side, diTagFeatureSide[i]))
	    eside = i;
    
    if(!eside)
	ajDebug("ensDiTagFeatureSideFromStr encountered "
		"unexpected string '%S'.\n", side);
    
    return eside;
}




/* @func ensDiTagFeatureSideToChar ********************************************
**
** Convert an Ensembl DiTag Feature side element into a C-type (char*) string.
**
** @param [r] side [const AjEnum] Ensembl DiTag Feature side enumerator
**
** @return [const char*] Ensembl DiTag Feature side C-type (char*) string
** @@
******************************************************************************/

const char* ensDiTagFeatureSideToChar(const AjEnum side)
{
    register ajint i = 0;
    
    if(!side)
	return NULL;
    
    for(i = 1; diTagFeatureSide[i] && (i < side); i++);
    
    if(!diTagFeatureSide[i])
	ajDebug("ensDiTagFeatureSideToChar encountered an "
		"out of boundary error on gender %d.\n", side);
    
    return diTagFeatureSide[i];
}




/* @datasection [EnsPDiTagFeatureadaptor] DiTag Feature Adaptor ***************
**
** Functions for manipulating Ensembl DiTag Feature Adaptor objects
**
** @cc Bio::EnsEMBL::Map::DBSQL::DiTagFeatureadaptor CVS Revision: 1.16
**
** @nam2rule DiTagFeatureadaptor
**
******************************************************************************/

/*
** NOTE: For now, the ditag_feature and ditag tables are permanently joined to
** allow for selection of Ditag Features on the basis of a Ditag type.
*/

static const char *diTagFeatureadaptorTables[] =
{
    "ditag_feature",
    "ditag",
    (const char *) NULL
};

static const char *diTagFeatureadaptorColumns[] =
{
    "ditag_feature.ditag_feature_id",
    "ditag_feature.seq_region_id",
    "ditag_feature.seq_region_start",
    "ditag_feature.seq_region_end",
    "ditag_feature.seq_region_strand",
    "ditag_feature.analysis_id",
    "ditag_feature.ditag_id",
    "ditag_feature.hit_start",
    "ditag_feature.hit_end",
    "ditag_feature.hit_strand",
    "ditag_feature.cigar_line",
    "ditag_feature.ditag_side",
    "ditag_feature.ditag_pair_id",
    (const char *) NULL
};

static EnsOBaseadaptorLeftJoin diTagFeatureadaptorLeftJoin[] =
{
    {(const char *) NULL, (const char *) NULL}
};

static const char *diTagFeatureadaptorDefaultCondition =
"ditag_feature.ditag_id = ditag.ditag_id";

static const char *diTagFeatureadaptorFinalCondition = NULL;

/*
** TODO: Remove this?
 static const char *diTagFeatureDiTagadaptorTables[] =
 {
     "ditag_feature",
     "ditag",
     (const char *) NULL
 };
 
 static const char *diTagFeatureDiTagadaptorColumns[] =
 {
     "ditag_feature.ditag_feature_id",
     "ditag_feature.seq_region_id",
     "ditag_feature.seq_region_start",
     "ditag_feature.seq_region_end",
     "ditag_feature.seq_region_strand",
     "ditag_feature.analysis_id",
     "ditag_feature.ditag_id",
     "ditag_feature.hit_start",
     "ditag_feature.hit_end",
     "ditag_feature.hit_strand",
     "ditag_feature.cigar_line",
     "ditag_feature.ditag_side",
     "ditag_feature.ditag_pair_id",
     "ditag.ditag_id",
     "ditag.name",
     "ditag.type",
     "ditag.tag_count",
     "ditag.sequence",
     (const char *) NULL
 };
 
 static const char *diTagFeatureDiTagadaptorDefaultCondition =
 "ditag_feature.ditag_id = ditag.ditag_id";
 
 static const char *diTagFeatureDiTagadaptorFinalCondition =
 "ORDER BY ditag_feature.ditag_id, ditag_feature.ditag_pair_id";
 */




/* @funcstatic diTagFeatureadaptorFetchAllBySQL *******************************
**
** Fetch all Ensembl DiTag Feature objects via an SQL statement.
**
** @cc Bio::EnsEMBL::Map::DBSQL::DiTagFeatureadaptor::_fetch
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] dtfs [AjPList] AJAX List of Ensembl DiTag Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool diTagFeatureadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblyMapper am,
                                               EnsPSlice slice,
                                               AjPList dtfs)
{
    ajint slstart  = 0;
    ajint slend    = 0;
    ajint slstrand = 0;
    ajint sllength = 0;
    ajint srstrand = 0;
    ajint dtstrand = 0;
    
    ajuint identifier = 0;
    ajuint srid       = 0;
    ajuint srstart    = 0;
    ajuint srend      = 0;
    ajuint analysisid = 0;
    ajuint dtid       = 0;
    ajuint dtstart    = 0;
    ajuint dtend      = 0;
    ajuint dtpairid   = 0;
    
    AjEnum eside = ensEDiTagFeatureSideNULL;
    
    AjPList mrs = NULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr cigar = NULL;
    AjPStr side  = NULL;
    
    EnsPAnalysis analysis  = NULL;
    EnsPAnalysisadaptor aa = NULL;
    
    EnsPAssemblyMapperadaptor ama = NULL;
    
    EnsPDiTag dt = NULL;
    
    EnsPDiTagFeature dtf         = NULL;
    EnsPDiTagFeatureadaptor dtfa = NULL;
    
    EnsPFeature feature = NULL;
    
    EnsPMapperResult mr = NULL;
    
    EnsPSlice srslice   = NULL;
    EnsPSliceadaptor sa = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    (void) am;
    
    (void) slice;
    
    if(!dtfs)
	return ajFalse;
    
    aa = ensRegistryGetAnalysisadaptor(dba);
    
    dtfa = ensRegistryGetDiTagFeatureadaptor(dba);
    
    sa = ensRegistryGetSliceadaptor(dba);
    
    if(slice)
	ama = ensRegistryGetAssemblyMapperadaptor(dba);
    
    mrs = ajListNew();
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier = 0;
	srid = 0;
	srstart = 0;
	srend = 0;
	srstrand = 0;
	analysisid = 0;
	dtid = 0;
	dtstart = 0;
	dtend = 0;
	dtstrand = 0;
	cigar = ajStrNew();
	side = ajStrNew();
	dtpairid = 0;
	
	sqlr = ajSqlrowiterGet(sqli);
	
	ajSqlcolumnToUint(sqlr, &identifier);
	ajSqlcolumnToUint(sqlr, &srid);
	ajSqlcolumnToUint(sqlr, &srstart);
	ajSqlcolumnToUint(sqlr, &srend);
	ajSqlcolumnToInt(sqlr, &srstrand);
	ajSqlcolumnToUint(sqlr, &analysisid);
	ajSqlcolumnToUint(sqlr, &dtid);
	ajSqlcolumnToUint(sqlr, &dtstart);
	ajSqlcolumnToUint(sqlr, &dtend);
	ajSqlcolumnToInt(sqlr, &dtstrand);
	ajSqlcolumnToStr(sqlr, &cigar);
	ajSqlcolumnToStr(sqlr, &side);
	ajSqlcolumnToUint(sqlr, &dtpairid);
	
	/*
	** Since the Ensembl SQL schema defines Sequence Region start and end
	** coordinates as unsigned integers for all Features, the range needs
	** checking.
	*/
	
	if(srstart <= INT_MAX)
	    slstart = (ajint) srstart;
	else
	    ajFatal("diTagFeatureadaptorFetchAllBySQL got a "
		    "Sequence Region start coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srstart, INT_MAX);
	
	if(srend <= INT_MAX)
	    slend = (ajint) srend;
	else
	    ajFatal("diTagFeatureadaptorFetchAllBySQL got a "
		    "Sequence Region end coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srend, INT_MAX);
	
	slstrand = srstrand;
	
	/* Fetch a Slice spanning the entire Sequence Region. */
	
	ensSliceadaptorFetchBySeqRegionIdentifier(sa, srid, 0, 0, 0, &srslice);
	
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
	
	if((! am) &&
           slice &&
           (! ensCoordSystemMatch(ensSliceGetCoordSystem(slice),
                                  ensSliceGetCoordSystem(srslice))))
	    am = ensAssemblyMapperadaptorFetchByCoordSystems(
                ama,
                ensSliceGetCoordSystem(slice),
                ensSliceGetCoordSystem(srslice));
	
	/*
	** Remap the Feature coordinates to another Ensembl Coordinate System
	** if an Ensembl Assembly Mapper is defined at this point.
	*/
	
	if(am)
	{
	    ensAssemblyMapperFastMap(
                am,
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
		
		ajStrDel(&cigar);
		ajStrDel(&side);
		
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
	    
	    ensSliceadaptorFetchBySeqRegionIdentifier(
                sa,
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
		ajFatal("diTagFeatureadaptorFetchAllBySQL got a Slice, "
			"which length (%u) exceeds the "
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
		
		ajStrDel(&cigar);
		ajStrDel(&side);
		
		ensSliceDel(&srslice);
		
		ensAssemblyMapperDel(&am);
		
		continue;
	    }
	    
	    /* Delete the Sequence Region Slice and set the requested Slice. */
	    
	    ensSliceDel(&srslice);
	    
	    srslice = ensSliceNewRef(slice);
	}
	
	ensAnalysisadaptorFetchByIdentifier(aa, analysisid, &analysis);
	
	feature = ensFeatureNewS(analysis, srslice, slstart, slend, slstrand);
	
	ensDiTagadaptorFetchByIdentifier(dba, dtid, &dt);
	
	eside = ensDiTagFeatureSideFromStr(side);
	
	dtf = ensDiTagFeatureNew(dtfa,
				 identifier,
				 feature,
				 dt,
				 cigar,
				 eside,
				 dtstart,
				 dtend,
				 dtstrand,
				 dtpairid);
	
	ajListPushAppend(dtfs, (void *) dtf);
	
	ensDiTagDel(&dt);
	
	ensFeatureDel(&feature);
	
	ensAnalysisDel(&analysis);
	
	ajStrDel(&cigar);
	ajStrDel(&side);
	
	ensSliceDel(&srslice);
	
	ensAssemblyMapperDel(&am);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    ajListFree(&mrs);
    
    return ajTrue;
}




/* @funcstatic diTagFeatureadaptorCacheReference ******************************
**
** Wrapper function to reference an Ensembl DiTag Feature
** from an Ensembl Cache.
**
** @param [r] value [void *] Ensembl DiTag Feature
**
** @return [void *] Ensembl DiTag Feature or NULL
** @@
******************************************************************************/

static void *diTagFeatureadaptorCacheReference(void *value)
{
    if(!value)
	return NULL;
    
    return (void *) ensDiTagFeatureNewRef((EnsPDiTagFeature) value);
}




/* @funcstatic diTagFeatureadaptorCacheDelete *********************************
**
** Wrapper function to delete an Ensembl DiTag Feature
** from an Ensembl Cache.
**
** @param [r] value [void**] Ensembl DiTag Feature address
**
** @return [void]
** @@
******************************************************************************/

static void diTagFeatureadaptorCacheDelete(void **value)
{
    if(!value)
	return;
    
    ensDiTagFeatureDel((EnsPDiTagFeature *) value);
    
    return;
}




/* @funcstatic DiTagFeatureadaptorCacheSize ***********************************
**
** Wrapper function to determine the memory size of an Ensembl DiTag Feature
** via an Ensembl Cache.
**
** @param [r] value [const void*] Ensembl DiTag Feature
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

static ajuint diTagFeatureadaptorCacheSize(const void *value)
{
    if(!value)
	return 0;
    
    return ensDiTagFeatureGetMemSize((const EnsPDiTagFeature) value);
}




/* @funcstatic diTagFeatureadaptorGetFeature **********************************
**
** Wrapper function to get the Ensembl Feature of an
** Ensembl DiTag Feature from an Ensembl Feature Adaptor.
**
** @param [r] value [const void*] Ensembl DiTag Feature
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

static EnsPFeature diTagFeatureadaptorGetFeature(const void *value)
{
    if(!value)
	return NULL;
    
    return ensDiTagFeatureGetFeature((const EnsPDiTagFeature) value);
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl DiTag Feature Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** DiTag Feature Adaptor. The target pointer does not need to be initialised
** to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPDiTagFeatureadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPDiTagFeatureadaptor] Ensembl DiTag Feature Adaptor
** @argrule Ref object [EnsPDiTagFeatureadaptor] Ensembl DiTag Feature Adaptor
**
** @valrule * [EnsPDiTagFeatureadaptor] Ensembl DiTag Feature Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensDiTagFeatureadaptorNew ********************************************
**
** Default Ensembl DiTag Feature Adaptor constructor.
**
** @cc Bio::EnsEMBL::Map::DBSQL::DiTagFeatureadaptor::new
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPDiTagFeatureadaptor] Ensembl DiTag Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPDiTagFeatureadaptor ensDiTagFeatureadaptorNew(EnsPDatabaseadaptor dba)
{
    EnsPDiTagFeatureadaptor adaptor = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(adaptor);
    
    adaptor->Adaptor =
	ensFeatureadaptorNew(dba,
			     diTagFeatureadaptorTables,
			     diTagFeatureadaptorColumns,
			     diTagFeatureadaptorLeftJoin,
			     diTagFeatureadaptorDefaultCondition,
			     diTagFeatureadaptorFinalCondition,
			     diTagFeatureadaptorFetchAllBySQL,
			     (void* (*)(const void* key)) NULL, /* Fread */
			     diTagFeatureadaptorCacheReference,
			     (AjBool (*)(const void* value)) NULL, /* Fwrite */
			     diTagFeatureadaptorCacheDelete,
			     diTagFeatureadaptorCacheSize,
			     diTagFeatureadaptorGetFeature,
			     "DiTag Feature");
    
    return adaptor;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl DiTag Feature Adaptor.
**
** @fdata [EnsPDiTagFeatureadaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl DiTag Feature Adaptor object.
**
** @argrule * Padaptor [EnsPDiTagFeatureadaptor*] Ensembl DiTag Feature
**                                                Adaptor object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensDiTagFeatureadaptorDel ********************************************
**
** Default destructor for an Ensembl DiTag Feature Adaptor.
**
** @param [d] Padaptor [EnsPDiTagFeatureadaptor*] Ensembl DiTag Feature
**                                                Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensDiTagFeatureadaptorDel(EnsPDiTagFeatureadaptor *Padaptor)
{
    EnsPDiTagFeatureadaptor pthis = NULL;
    
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




/* @func ensDiTagFeatureadaptorFetchByIdentifier ******************************
**
** Fetch an Ensembl DiTag Feature via its SQL database-internal identifier.
**
** The caller is responsible for deleting the Ensembl DiTag Feature.
**
** @cc Bio::EnsEMBL::Map::DBSQL::DiTagFeatureadaptor::fetch_by_dbID
** @param [r] adaptor [EnsPDiTagFeatureadaptor] Ensembl DiTag Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pdtf [EnsPDiTagFeature*] Ensembl DiTag Feature address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagadaptorFetchAllByIdentifier(EnsPDiTagFeatureadaptor adaptor,
                                           ajuint identifier,
                                           EnsPDiTagFeature *Pdtf)
{
    EnsPBaseadaptor ba = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pdtf)
	return ajFalse;
    
    ba = ensFeatureadaptorGetBaseadaptor(adaptor->Adaptor);
    
    *Pdtf = (EnsPDiTagFeature) ensBaseadaptorFetchByIdentifier(ba, identifier);
    
    return ajTrue;
}




/* @func ensDiTagFeatureadaptorFetchAllByDiTag ********************************
**
** Fetch all Ensembl DiTag Features by an Ensembl DiTag.
**
** The caller is responsible for deleting the Ensembl DiTag Features before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Map::DBSQL::DiTagFeatureadaptor::fetch_all_by_ditagID
** @param [r] adaptor [const EnsPDiTagFeatureadaptor] Ensembl DiTag
**                                                    Feature Adaptor
** @param [r] ditag [const EnsPDiTag] Ensembl DiTag
** @param [u] dtfs [AjPList] AJAX List of Ensembl DiTag Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureadaptorFetchAllByDiTag(EnsPDiTagFeatureadaptor adaptor,
                                             const EnsPDiTag dt,
                                             AjPList dtfs)
{
    AjBool value = AJFALSE;
    
    AjPStr constraint = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!dt)
	return ajFalse;
    
    if(!dtfs)
	return ajFalse;
    
    ba = ensFeatureadaptorGetBaseadaptor(adaptor->Adaptor);
    
    constraint = ajFmtStr("ditag_feature.ditag_id = %u", dt->Identifier);
    
    value = ensBaseadaptorGenericFetch(ba,
				       constraint,
				       (EnsPAssemblyMapper) NULL,
				       (EnsPSlice) NULL,
				       dtfs);
    
    ajStrDel(&constraint);
    
    return value;
}




/* @func ensDiTagFeatureadaptorFetchAllByType *********************************
**
** Fetch all Ensembl DiTag Features by an Ensembl DiTag type.
**
** The caller is responsible for deleting the Ensembl DiTag Features before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Map::DBSQL::DiTagFeatureadaptor::fetch_all
** @cc Bio::EnsEMBL::Map::DBSQL::DiTagFeatureadaptor::fetch_all_by_type
** @param [r] adaptor [const EnsPDiTagFeatureadaptor] Ensembl DiTag
**                                                    Feature Adaptor
** @param [rN] type [const AjPStr] Ensembl DiTag type
** @param [u] dtfs [AjPList] AJAX List of Ensembl DiTag Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureadaptorFetchAllByType(EnsPDiTagFeatureadaptor adaptor,
                                            const AjPStr type,
                                            AjPList dtfs)
{
    char *txttype = NULL;
    
    AjBool value = AJFALSE;
    
    AjPStr constraint = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!(type && ajStrGetLen(type)))
	return ajFalse;
    
    if(!dtfs)
	return ajFalse;
    
    ba = ensFeatureadaptorGetBaseadaptor(adaptor->Adaptor);
    
    ensBaseadaptorEscapeC(ba, &txttype, type);
    
    /*
    ** NOTE: For this query the ditag_feature and the ditag tables need to be
    ** permanently joined.
    */
    
    constraint = ajFmtStr("ditag.type = '%s'", txttype);
    
    ajCharDel(&txttype);
    
    value = ensBaseadaptorGenericFetch(ba,
				       constraint,
				       (EnsPAssemblyMapper) NULL,
				       (EnsPSlice) NULL,
				       dtfs);
    
    ajStrDel(&constraint);
    
    return value;
}




/* @func ensDiTagFeatureadaptorFetchAllBySlice ********************************
**
** Fetch all Ensembl DiTag Features by an Ensembl Slice.
**
** The caller is responsible for deleting the Ensembl DiTag Features before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Map::DBSQL::DiTagFeatureadaptor::fetch_all_by_Slice
** @param [r] adaptor [const EnsPDiTagFeatureadaptor] Ensembl DiTag
**                                                    Feature Adaptor
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [rN] type [const AjPStr] Ensembl DiTag type
** @param [rN] anname [const AjPStr] Ensembl Analysis name
** @param [u] dtfs [AjPList] AJAX List of Ensembl DiTag Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDiTagFeatureadaptorFetchAllBySlice(EnsPDiTagFeatureadaptor adaptor,
                                             EnsPSlice slice,
                                             const AjPStr type,
                                             const AjPStr anname,
                                             AjPList dtfs)
{
    char *txttype = NULL;
    
    AjBool value = AJFALSE;
    
    AjPStr constraint = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!slice)
	return ajFalse;
    
    if(!dtfs)
	return ajFalse;
    
    if(type && ajStrGetLen(type))
    {
	ensFeatureadaptorEscapeC(adaptor->Adaptor, &txttype, type);
	
	constraint = ajFmtStr("ditag.type = '%s'", txttype);
	
	ajCharDel(&txttype);
    }
    
    value = ensFeatureadaptorFetchAllBySliceConstraint(adaptor->Adaptor,
						       slice,
						       constraint,
						       anname,
						       dtfs);
    
    ajStrDel(&constraint);
    
    return value;
}




#if AJFALSE

AjBool ensDitagFeatureadaptorFetchPairsBySlice(EnsPDiTagFeatureadaptor adaptor,
                                               EnsPSlice slice,
                                               const AjPStr type,
                                               const AjPStr anname,
                                               AjPList dtfs)
{
    char *txttype = NULL;
    
    AjBool value = AJFALSE;
    
    AjPStr constraint = NULL;
    
    if(!adaptor)
	return ajFalse;
    
    if(!slice)
	return ajFalse;
    
    if(!dtfs)
	return ajFalse;
    
    if(type && ajStrGetLen(type))
    {
	ensFeatureadaptorEscapeC(adaptor->Adaptor, &txttype, type);
	
	constraint = ajFmtStr("ditag.type = '%s'", txttype);
	
	ajCharDel(&txttype);
    }
    
    /*
    ** FIXME: This would be problematic in multi-threaded environments.
    **  ensFeatureadaptorSetFinalCondition(adaptor->Adaptor,
    **  <#const char * final#>)
    */
    
    value = ensFeatureadaptorFetchAllBySliceConstraint(adaptor->Adaptor,
						       slice,
						       constraint,
						       anname,
						       dtfs);
    
    ajStrDel(&constraint);
    
    /*
    ** FIXME: Finish this. Since it is not simply possible to append a
    ** GROUP BY df.ditag_id, df.ditag_pair_id condition, the results would
    ** need to be calculated by means of Lists and Tables here.
    ** It would be possible to append the condition via
    ** ensFeatureadaptorSetFinalCondition(adaptor->Adaptor,
    ** <#const char * final#>),
    ** but this would be problematic in a multi-threaded environment.
    ** Could a separate EnsPDitagFeaturePairadaptor be the solution?
    ** A similr prolem exists for the EnsPExonadaptor and the
    ** ExonTranscript Adaptor ...
    */
    
    return value;
}

#endif
