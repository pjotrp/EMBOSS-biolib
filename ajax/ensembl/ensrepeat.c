/******************************************************************************
** @source Ensembl Repeat functions
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

#include "ensrepeat.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPAnalysisadaptor
ensRegistryGetAnalysisadaptor(EnsPDatabaseadaptor dba);

extern EnsPAssemblyMapperadaptor
ensRegistryGetAssemblyMapperadaptor(EnsPDatabaseadaptor dba);

extern EnsPCoordSystemadaptor
ensRegistryGetCoordSystemadaptor(EnsPDatabaseadaptor dba);

extern EnsPRepeatFeatureadaptor
ensRegistryGetRepeatFeatureadaptor(EnsPDatabaseadaptor dba);

extern EnsPSliceadaptor
ensRegistryGetSliceadaptor(EnsPDatabaseadaptor dba);

static AjBool repeatConsensusadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                  const AjPStr statement,
                                                  AjPList rci);

static void *repeatFeatureadaptorCacheReference(void *value);

static void repeatFeatureadaptorCacheDelete(void **value);

static ajuint repeatFeatureadaptorCacheSize(const void *value);

static EnsPFeature repeatFeatureadaptorGetFeature(const void *rf);

static AjBool repeatFeatureadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                const AjPStr statement,
                                                EnsPAssemblyMapper am,
                                                EnsPSlice slice,
                                                AjPList rfs);




/* @filesection ensrepeat *****************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPRepeatConsensus] Repeat Consensus ************************
**
** Functions for manipulating Ensembl Repeat Consensus objects
**
** @cc Bio::EnsEMBL::RepeatConsensus CVS Revision: 1.9
**
** @nam2rule RepeatConsensus
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Repeat Consensus by pointer.
** It is the responsibility of the user to first destroy any previous
** Repeat Consensus. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPRepeatConsensus]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @argrule Ref object [EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @valrule * [EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @fcategory new
******************************************************************************/




/* @func ensRepeatConsensusNew ************************************************
**
** Default constructor for an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::RepeatConsensus::new
** @param [r] adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] identifier [ajuint] SQL databaseinternal identifier
** @param [r] name [AjPStr] Repeat Consensus name
** @param [r] class [AjPStr] Repeat Consensus class
** @param [r] type [AjPStr] Repeat Consensus type
** @param [r] consensus [AjPStr] Repeat Consensus sequence
** @param [r] length [ajuint] Repeat Consensus sequence length
**
** @return [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @@
******************************************************************************/

EnsPRepeatConsensus ensRepeatConsensusNew(EnsPDatabaseadaptor adaptor,
                                          ajuint identifier,
                                          AjPStr name,
                                          AjPStr class,
                                          AjPStr type,
                                          AjPStr consensus,
                                          ajuint length)
{
    EnsPRepeatConsensus rc = NULL;
    
    AJNEW0(rc);
    
    rc->Adaptor    = adaptor;
    rc->Identifier = identifier;
    
    if(name)
	rc->Name = ajStrNewRef(name);
    
    if(class)
	rc->Class = ajStrNewRef(class);
    
    if(type)
	rc->Type = ajStrNewRef(type);
    
    if(consensus)
	rc->Consensus = ajStrNewRef(consensus);
    
    rc->Length = length;
    
    rc->Use = 1;
    
    return rc;
}




/* @func ensRepeatConsensusNewObj *********************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [u] object [EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @return [EnsPRepeatConsensus] Ensembl Repeat Consensus or NULL
** @@
******************************************************************************/

EnsPRepeatConsensus ensRepeatConsensusNewObj(EnsPRepeatConsensus object)
{
    EnsPRepeatConsensus rc = NULL;
    
    AJNEW0(rc);
    
    rc->Adaptor = object->Adaptor;
    
    rc->Identifier = object->Identifier;
    
    if(object->Name)
	rc->Name = ajStrNewRef(object->Name);
    
    if(object->Class)
	rc->Class = ajStrNewRef(object->Class);
    
    if(object->Type)
	rc->Type = ajStrNewRef(object->Type);
    
    if(object->Consensus)
	rc->Consensus = ajStrNewRef(object->Consensus);
    
    rc->Length = object->Length;
    
    rc->Use = 1;
    
    return rc;
}




/* @func ensRepeatConsensusNewRef *********************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] rc [EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @return [EnsPRepeatConsensus] Ensembl Repeat Consensus or NULL
** @@
******************************************************************************/

EnsPRepeatConsensus ensRepeatConsensusNewRef(EnsPRepeatConsensus rc)
{
    if(!rc)
	return NULL;
    
    rc->Use++;
    
    return rc;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Repeat Consensus.
**
** @fdata [EnsPRepeatConsensus]
** @fnote None
**
** @nam3rule Del Destroy (free) a Repeat Consensus object
**
** @argrule * Prc [EnsPRepeatConsensus*] Repeat Consensus object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensRepeatConsensusDel ************************************************
**
** Default destructor for an Ensembl Repeat Consensus.
**
** @param [d] Prc [EnsPRepeatConsensus*] Ensembl Repeat Consensus address
**
** @return [void]
** @@
******************************************************************************/

void ensRepeatConsensusDel(EnsPRepeatConsensus *Prc)
{
    EnsPRepeatConsensus pthis = NULL;
    
    if(!Prc)
	return;
    
    if(!*Prc)
	return;
    
    /*
     ajDebug("ensRepeatConsensusDel\n"
	     "  *Prc %p\n",
	     *Prc);
     */

    pthis = *Prc;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Prc = NULL;
	
	return;
    }
    
    ajStrDel(&pthis->Name);
    ajStrDel(&pthis->Class);
    ajStrDel(&pthis->Type);
    ajStrDel(&pthis->Consensus);
    
    AJFREE(pthis);

    *Prc = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Repeat Consensus object.
**
** @fdata [EnsPRepeatConsensus]
** @fnote None
**
** @nam3rule Get Return Repeat Consensus attribute(s)
** @nam4rule Getadaptor Return the Ensembl Repeat Consensus Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetName Return the name
** @nam4rule GetClass Return the class
** @nam4rule GetType Return the type
** @nam4rule GetConsensus Return the consensus sequence
** @nam4rule GetLength Return the length
**
** @argrule * rc [const EnsPRepeatConsensus] Repeat Consensus
**
** @valrule Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Name [AjPStr] Name
** @valrule Class [AjPStr] Class
** @valrule Type [AjPStr] Type
** @valrule Consensus [AjPStr] Consensus sequence
** @valrule Length [ajuint] Length
**
** @fcategory use
******************************************************************************/




/* @func ensRepeatConsensusGetadaptor *****************************************
**
** Get the Ensembl Database Adaptor element of an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] rc [const EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @return [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

EnsPDatabaseadaptor ensRepeatConsensusGetadaptor(const EnsPRepeatConsensus rc)
{
    if(!rc)
	return NULL;
    
    return rc->Adaptor;
}




/* @func ensRepeatConsensusGetIdentifier **************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] rc [const EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @return [ajuint] SQL database-internal identifier
** @@
******************************************************************************/

ajuint ensRepeatConsensusGetIdentifier(const EnsPRepeatConsensus rc)
{
    if(!rc)
	return 0;
    
    return rc->Identifier;
}




/* @func ensRepeatConsensusGetName ********************************************
**
** Get the name element of an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::RepeatConsensus::name
** @param [r] rc [const EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensRepeatConsensusGetName(const EnsPRepeatConsensus rc)
{
    if(!rc)
	return NULL;
    
    return rc->Name;
}




/* @func ensRepeatConsensusGetClass ********************************************
**
** Get the class element of an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::RepeatConsensus::repeat_class
** @param [r] rc [const EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @return [AjPStr] Class
** @@
******************************************************************************/

AjPStr ensRepeatConsensusGetClass(const EnsPRepeatConsensus rc)
{
    if(!rc)
	return NULL;
    
    return rc->Class;
}




/* @func ensRepeatConsensusGetType ********************************************
**
** Get the type element of an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::RepeatConsensus::repeat_type
** @param [r] rc [const EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @return [AjPStr] Type
** @@
******************************************************************************/

AjPStr ensRepeatConsensusGetType(const EnsPRepeatConsensus rc)
{
    if(!rc)
	return NULL;
    
    return rc->Type;
}




/* @func ensRepeatConsensusGetConsensus ***************************************
**
** Get the consensus sequence element of an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::RepeatConsensus::repeat_consensus
** @param [r] rc [const EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @return [AjPStr] Consensus sequence
** @@
******************************************************************************/

AjPStr ensRepeatConsensusGetConsensus(const EnsPRepeatConsensus rc)
{
    if(!rc)
	return NULL;
    
    return rc->Consensus;
}




/* @func ensRepeatConsensusGetLength ******************************************
**
** Get the length element of an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::RepeatConsensus::length
** @param [r] rc [const EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @return [ajuint] Length
** @@
******************************************************************************/

ajuint ensRepeatConsensusGetLength(const EnsPRepeatConsensus rc)
{
    if(!rc)
	return 0;
    
    return rc->Length;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl Repeat Consensus object.
**
** @fdata [EnsPRepeatConsensus]
** @fnote None
**
** @nam3rule Set Set one element of a Repeat Consensus
** @nam4rule Setadaptor Set the Ensembl Repeat Consensus Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetName Set the name
** @nam4rule SetClass Set the class
** @nam4rule SetType Set the type
** @nam4rule SetConsensus Set the consensus sequence
** @nam4rule SetType Set the type
**
** @argrule * rc [EnsPRepeatConsensus] Ensembl Repeat Consensus object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensRepeatConsensusSetadaptor *****************************************
**
** Set the Ensembl Database Adaptor element of an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] rc [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatConsensusSetadaptor(EnsPRepeatConsensus rc,
                                    EnsPDatabaseadaptor dba)
{
    if(!rc)
        return ajFalse;
    
    rc->Adaptor = dba;
    
    return ajTrue;
}




/* @func ensRepeatConsensusSetIdentifier **************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] rc [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatConsensusSetIdentifier(EnsPRepeatConsensus rc,
                                       ajuint identifier)
{
    if(!rc)
        return ajFalse;
    
    rc->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensRepeatConsensusSetName ********************************************
**
** Set the name element of an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::RepeatConsensus::name
** @param [u] rc [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatConsensusSetName(EnsPRepeatConsensus rc, AjPStr name)
{
    if(!rc)
        return ajFalse;
    
    ajStrDel(&rc->Name);
    
    if(name)
	rc->Name = ajStrNewRef(name);
    
    return ajTrue;
}




/* @func ensRepeatConsensusSetClass *******************************************
**
** Set the class element of an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::RepeatConsensus::repeat_class
** @param [u] rc [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @param [u] class [AjPStr] Class
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatConsensusSetClass(EnsPRepeatConsensus rc, AjPStr class)
{
    if(!rc)
        return ajFalse;
    
    ajStrDel(&rc->Class);
    
    if(class)
	rc->Class = ajStrNewRef(class);
    
    return ajTrue;
}




/* @func ensRepeatConsensusSetType ********************************************
**
** Set the type element of an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::RepeatConsensus::repeat_type
** @param [u] rc [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @param [u] type [AjPStr] Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatConsensusSetType(EnsPRepeatConsensus rc, AjPStr type)
{
    if(!rc)
        return ajFalse;
    
    ajStrDel(&rc->Type);
    
    if(type)
	rc->Type = ajStrNewRef(type);
    
    return ajTrue;
}




/* @func ensRepeatConsensusSetConsensus ***************************************
**
** Set the consensus sequence element of an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::RepeatConsensus::repeat_consensus
** @param [u] rc [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @param [u] consensus [AjPStr] Consensus sequence
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatConsensusSetConsensus(EnsPRepeatConsensus rc, AjPStr consensus)
{
    if(!rc)
        return ajFalse;
    
    ajStrDel(&rc->Consensus);
    
    if(consensus)
	rc->Consensus = ajStrNewRef(consensus);
    
    return ajTrue;
}




/* @func ensRepeatConsensusSetLength ******************************************
**
** Set the length element of an Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::RepeatConsensus::length
** @param [u] rc [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @param [r] length [ajuint] Length
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatConsensusSetLength(EnsPRepeatConsensus rc, ajuint length)
{
    if(!rc)
        return ajFalse;
    
    rc->Length = length;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Repeat Consensus object.
**
** @fdata [EnsPRepeatConsensus]
** @nam3rule Trace Report Ensembl Repeat Consensus elements to debug file
**
** @argrule Trace rc [const EnsPRepeatConsensus] Ensembl Repeat Consensus
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensRepeatConsensusTrace **********************************************
**
** Trace an Ensembl Repeat Consensus.
**
** @param [r] rc [const EnsPRepeatConsensus] Ensembl Repeat Consensus
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatConsensusTrace(const EnsPRepeatConsensus rc, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!rc)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensRepeatConsensusTrace %p\n"
	    "%S  Adaptor %p\n"
	    "%S  Identifier %u\n"
	    "%S  Name '%S'\n"
	    "%S  Class '%S'\n"
	    "%S  Type '%S'\n"
	    "%S  Consensus '%S'\n"
	    "%S  Length %u\n"
	    "%S  Use %u\n",
	    indent, rc,
	    indent, rc->Adaptor,
	    indent, rc->Identifier,
	    indent, rc->Name,
	    indent, rc->Class,
	    indent, rc->Type,
	    indent, rc->Consensus,
	    indent, rc->Length,
	    indent, rc->Use);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensRepeatConsensusGetMemSize *****************************************
**
** Get the memory size in bytes of an Ensembl Repeat Consensus.
**
** @param [r] rc [const EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensRepeatConsensusGetMemSize(const EnsPRepeatConsensus rc)
{
    ajuint size = 0;
    
    if(!rc)
	return 0;
    
    size += (ajuint) sizeof (EnsORepeatConsensus);
    
    if(rc->Name)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(rc->Name);
    }
    
    if(rc->Class)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(rc->Class);
    }
    
    if(rc->Type)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(rc->Type);
    }
    
    if(rc->Consensus)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(rc->Consensus);
    }
    
    return size;
}




/* @datasection [EnsPRepeatConsensusadaptor] Repeat Consensus Adaptor *********
**
** Functions for manipulating Ensembl Repeat Consensus Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::RepeatConsensusadaptor CVS Revision: 1.20
**
** @nam2rule RepeatConsensusadaptor
**
******************************************************************************/




/* @funcstatic repeatConsensusadaptorFetchAllBySQL ****************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl Repeat Consensus objects.
** The caller is responsible for deleting the Ensembl Repeat Consensi before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::DBSQL::RepeatConsensusadaptor::_generic_fetch
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] rci [AjPList] AJAX List of Ensembl Repeat Consensi
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool repeatConsensusadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                  const AjPStr statement,
                                                  AjPList rci)
{
    ajuint identifier = 0;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr name      = NULL;
    AjPStr class     = NULL;
    AjPStr type      = NULL;
    AjPStr consensus = NULL;
    
    EnsPRepeatConsensus rc = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    if(!rci)
	return ajFalse;
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier = 0;
	name       = ajStrNew();
	class      = ajStrNew();
	type       = ajStrNew();
	consensus  = ajStrNew();
	
        sqlr = ajSqlrowiterGet(sqli);
	
        ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToStr(sqlr, &name);
        ajSqlcolumnToStr(sqlr, &class);
        ajSqlcolumnToStr(sqlr, &type);
        ajSqlcolumnToStr(sqlr, &consensus);
	
	rc = ensRepeatConsensusNew(dba,
				   identifier,
				   name,
				   class,
				   type,
				   consensus,
				   0);
	
	ajListPushAppend(rci, (void *) rc);
	
	ajStrDel(&name);
	ajStrDel(&class);
	ajStrDel(&type);
	ajStrDel(&consensus);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    return ajTrue;
}




/* @section object retrieval **************************************************
**
** Functions for retrieving Ensembl Repeat Consensus objects from an
** Ensembl Core database.
**
** @fdata [EnsPRepeatConsensusadaptor]
** @fnote None
**
** @nam3rule Fetch Retrieve Ensembl Repeat Consensus object(s)
** @nam4rule FetchAll Retrieve all Ensembl Repeat Consensus objects
** @nam5rule FetchAllBy Retrieve all Ensembl Repeat Consensus objects
**                      matching a criterion
** @nam4rule FetchBy Retrieve one Ensembl Repeat Consensus object
**                   matching a criterion
**
** @argrule * adaptor [const EnsPRepeatConsensusadaptor] Ensembl Repeat
**                                                       Consensus Adaptor
** @argrule FetchAll rci [AjPList] AJAX List of
**                                    Ensembl Repeat Consensus objects
** @argrule FetchBy Prc [EnsPRepeatConsensus*] Ensembl Repeat Consensus address
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory use
******************************************************************************/




/* @func ensRepeatConsensusadaptorFetchByIdentifier ***************************
**
** Fetch an Ensembl Repeat Consensus by its internal SQL database identifier.
** The caller is responsible for deleting the Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::DBSQL::RepeatConsensusadaptor::fetch_by_dbID
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [w] Prc [EnsPRepeatConsensus] Ensembl Repeat Consensus address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatConsensusadaptorFetchByIdentifier(EnsPDatabaseadaptor dba,
                                                  ajuint identifier,
                                                  EnsPRepeatConsensus *Prc)
{
    AjPList rci      = NULL;
    AjPStr statement = NULL;
    
    EnsPRepeatConsensus rc = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Prc)
	return ajFalse;
    
    statement = ajFmtStr("SELECT "
			 "repeat_consensus.repeat_consensus_id, "
			 "repeat_consensus.repeat_name, "
			 "repeat_consensus.repeat_class, "
			 "repeat_consensus.repeat_type, "
			 "repeat_consensus.repeat_consensus "
			 "FROM "
			 "repeat_consensus "
			 "WHERE "
			 "repeat_consensus.repeat_consensus_id = %u",
			 identifier);
    
    rci = ajListNew();
    
    repeatConsensusadaptorFetchAllBySQL(dba, statement, rci);
    
    if(ajListGetLength(rci) > 1)
	ajFatal("ensRepeatConsensusadaptorFetchByIdentifier got more than one "
		"Repeat Consensus for(PRIMARY KEY) identifier %u.\n",
		identifier);
    
    ajListPop(rci, (void **) Prc);
    
    while(ajListPop(rci, (void **) &rc))
	ensRepeatConsensusDel(&rc);
    
    ajListFree(&rci);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensRepeatConsensusadaptorFetchByName *********************************
**
** Fetch an Ensembl Repeat Consensus by its name.
** The caller is responsible for deleting the Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::DBSQL::RepeatConsensusadaptor::fetch_by_name
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] name [const AjPStr] Name
** @param [w] Prc [EnsPRepeatConsensus] Ensembl Repeat Consensus address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatConsensusadaptorFetchByName(EnsPDatabaseadaptor dba,
                                            const AjPStr name,
                                            EnsPRepeatConsensus *Prc)
{
    char *txtname;
    
    AjPList rci = NULL;
    
    AjPStr statement = NULL;
    
    EnsPRepeatConsensus rc = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    if(!Prc)
	return ajFalse;
    
    ensDatabaseadaptorEscapeC(dba, &txtname, name);
    
    statement = ajFmtStr("SELECT "
			 "repeat_consensus.repeat_consensus_id, "
			 "repeat_consensus.repeat_name, "
			 "repeat_consensus.repeat_class, "
			 "repeat_consensus.repeat_type, "
			 "repeat_consensus.repeat_consensus "
			 "FROM "
			 "repeat_consensus "
			 "WHERE "
			 "repeat_consensus.repeat_name = '%s'",
			 txtname);
    
    ajCharDel(&txtname);
    
    rci = ajListNew();
    
    repeatConsensusadaptorFetchAllBySQL(dba, statement, rci);
    
    if(ajListGetLength(rci) > 1)
	ajFatal("ensRepeatConsensusadaptorFetchByName got more than one "
		"Repeat Consensus from the database for name '%S'.\n", name);
    
    ajListPop(rci, (void **) Prc);
    
    while(ajListPop(rci, (void **) &rc))
	ensRepeatConsensusDel(&rc);
    
    ajListFree(&rci);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensRepeatConsensusadaptorFetchByNameClass ****************************
**
** Fetch an Ensembl Repeat Consensus by its name and class.
** The caller is responsible for deleting the Ensembl Repeat Consensus.
**
** @cc Bio::EnsEMBL::DBSQL::RepeatConsensusadaptor::fetch_by_name_class
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] name [const AjPStr] Name
** @param [r] class [const AjPStr] Class
** @param [w] Prc [EnsPRepeatConsensus] Ensembl Repeat Consensus address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatConsensusadaptorFetchByNameClass(EnsPDatabaseadaptor dba,
                                                 const AjPStr name,
                                                 const AjPStr class,
                                                 EnsPRepeatConsensus *Prc)
{
    char *txtname  = NULL;
    char *txtclass = NULL;
    
    AjPList rci = NULL;
    
    AjPStr statement = NULL;
    
    EnsPRepeatConsensus rc = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    if(!class)
	return ajFalse;
    
    if(!Prc)
	return ajFalse;
    
    ensDatabaseadaptorEscapeC(dba, &txtname, name);
    
    ensDatabaseadaptorEscapeC(dba, &txtclass, class);
    
    statement = ajFmtStr("SELECT "
			 "repeat_consensus.repeat_consensus_id, "
			 "repeat_consensus.repeat_name, "
			 "repeat_consensus.repeat_class, "
			 "repeat_consensus.repeat_type, "
			 "repeat_consensus.repeat_consensus "
			 "FROM "
			 "repeat_consensus "
			 "WHERE "
			 "repeat_consensus.repeat_name = '%s' "
			 "AND "
			 "repeat_consensus.repeat_class = '%s'",
			 txtname,
			 txtclass);
    
    ajCharDel(&txtname);
    
    ajCharDel(&txtclass);
    
    rci = ajListNew();
    
    repeatConsensusadaptorFetchAllBySQL(dba, statement, rci);
    
    if(ajListGetLength(rci) > 1)
	ajFatal("ensRepeatConsensusadaptorFetchByNameClass got more than one "
		"Repeat Consensus from the database for name '%S' and "
		"class '%S'.\n", name, class);
    
    ajListPop(rci, (void **) Prc);
    
    while(ajListPop(rci, (void **) &rc))
	ensRepeatConsensusDel(&rc);
    
    ajListFree(&rci);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensRepeatConsensusadaptorFetchAllByClassConsensus ********************
**
** Fetch all Ensembl Repeat Consensi by their class and
** consensus sequence.
** The caller is responsible for deleting the Ensembl Repeat Consensi before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::DBSQL::RepeatConsensusadaptor::fetch_all_by_class_seq
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] class [const AjPStr] Class
** @param [r] consensus [const AjPStr] Consensus sequence
** @param [u] rci [AjPList] AJAX List of Ensembl Repeat Consensi
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatConsensusadaptorFetchAllByClassConsensus(
    EnsPDatabaseadaptor dba,
    const AjPStr class,
    const AjPStr consensus,
    AjPList rci)
{
    char *txtclass     = NULL;
    char *txtconsensus = NULL;
    
    AjPStr statement = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!class)
	return ajFalse;
    
    if(!consensus)
	return ajFalse;
    
    if(!rci)
	return ajFalse;
    
    ensDatabaseadaptorEscapeC(dba, &txtclass, class);
    ensDatabaseadaptorEscapeC(dba, &txtconsensus, consensus);
    
    statement = ajFmtStr("SELECT "
			 "repeat_consensus.repeat_consensus_id, "
			 "repeat_consensus.repeat_name, "
			 "repeat_consensus.repeat_class, "
			 "repeat_consensus.repeat_type, "
			 "repeat_consensus.repeat_consensus "
			 "FROM "
			 "repeat_consensus "
			 "WHERE "
			 "repeat_consensus.repeat_class = '%s' "
			 "AND "
			 "repeat_consensus.repeat_consensus = '%s'",
			 txtclass,
			 txtconsensus);
    
    ajCharDel(&txtclass);
    ajCharDel(&txtconsensus);
    
    repeatConsensusadaptorFetchAllBySQL(dba, statement, rci);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @datasection [EnsPRepeatFeature] Repeat Feature ****************************
**
** Functions for manipulating Ensembl Repeat Feature objects
**
** @cc Bio::EnsEMBL::RepeatFeature CVS Revision: 1.24
**
** @nam2rule RepeatFeature
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Repeat Feature by pointer.
** It is the responsibility of the user to first destroy any previous
** Repeat Feature. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPRepeatFeature]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPRepeatFeature] Ensembl Repeat Feature
** @argrule Ref object [EnsPRepeatFeature] Ensembl Repeat Feature
**
** @valrule * [EnsPRepeatFeature] Ensembl Repeat Feature
**
** @fcategory new
******************************************************************************/




/* @func ensRepeatFeatureNew **************************************************
**
** Default constructor for an Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::RepeatFeature::new
** @param [r] adaptor [EnsPRepeatFeatureadaptor] Ensembl Repeat
**                                               Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [u] feature [EnsPFeature] Ensembl Feature
** @param [u] rc [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @param [r] hstart [ajint] Repeat Consensus hit start
** @param [r] hend [ajint] Repeat Consensus hit end
** @param [r] score [double] Repeat Consensus score
**
** @return [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @@
******************************************************************************/

EnsPRepeatFeature ensRepeatFeatureNew(EnsPRepeatFeatureadaptor adaptor,
                                      ajuint identifier,
                                      EnsPFeature feature,
                                      EnsPRepeatConsensus rc,
                                      ajint hstart,
                                      ajint hend,
                                      double score)
{
    EnsPRepeatFeature rf = NULL;
    
    if(!feature)
	return NULL;
    
    AJNEW0(rf);
    
    rf->Adaptor = adaptor;
    
    rf->Identifier = identifier;
    
    rf->Feature = ensFeatureNewRef(feature);
    
    rf->RepeatConsensus = ensRepeatConsensusNewRef(rc);
    
    rf->HitStart = hstart;
    
    rf->HitEnd = hend;
    
    rf->Score = score;
    
    rf->Use = 1;
    
    return rf;
}




/* @func ensRepeatFeatureNewObj ***********************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [u] object [EnsPRepeatFeature] Ensembl Repeat Feature
**
** @return [EnsPRepeatFeature] Ensembl Repeat Feature or NULL
** @@
******************************************************************************/

EnsPRepeatFeature ensRepeatFeatureNewObj(EnsPRepeatFeature object)
{
    EnsPRepeatFeature rf = NULL;
    
    AJNEW0(rf);
    
    rf->Adaptor = object->Adaptor;
    
    rf->Identifier = object->Identifier;
    
    rf->Feature = ensFeatureNewRef(object->Feature);
    
    rf->RepeatConsensus = ensRepeatConsensusNewRef(object->RepeatConsensus);
    
    rf->HitStart = object->HitStart;
    
    rf->HitEnd = object->HitEnd;
    
    rf->Score = object->Score;
    
    rf->Use = 1;
    
    return rf;
}




/* @func ensRepeatFeatureNewRef ***********************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] rf [EnsPRepeatFeature] Ensembl Repeat Feature
**
** @return [EnsPRepeatFeature] Ensembl Repeat Feature
** @@
******************************************************************************/

EnsPRepeatFeature ensRepeatFeatureNewRef(EnsPRepeatFeature rf)
{
    if(!rf)
	return NULL;
    
    rf->Use++;
    
    return rf;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Repeat Features.
**
** @fdata [EnsPRepeatFeature]
** @fnote None
**
** @nam3rule Del Destroy (free) a Repeat Feature object
**
** @argrule * Prf [EnsPRepeatFeature*] Repeat Feature object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensRepeatFeatureDel **************************************************
**
** Default destructor for an Ensembl Repeat Feature.
**
** @param [d] Prf [EnsPRepeatFeature*] Ensembl Repeat Feature address
**
** @return [void]
** @@
******************************************************************************/

void ensRepeatFeatureDel(EnsPRepeatFeature *Prf)
{
    EnsPRepeatFeature pthis = NULL;
    
    if(!Prf)
	return;
    
    if(!*Prf)
	return;

    pthis = *Prf;
    
    /*
     ajDebug("ensRepeatFeatureDel\n"
	     "  *Prf %p\n",
	     *Prf);
     */
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Prf = NULL;
	
	return;
    }
    
    ensFeatureDel(&pthis->Feature);
    
    ensRepeatConsensusDel(&pthis->RepeatConsensus);
    
    AJFREE(pthis);

    *Prf = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Repeat Feature object.
**
** @fdata [EnsPRepeatFeature]
** @fnote None
**
** @nam3rule Get Return Repeat Feature attribute(s)
** @nam4rule Getadaptor Return the Ensembl Repeat Feature Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetFeature Return the Ensembl Feature
** @nam4rule GetRepeatConsensus Return the Ensembl Repeat Consensus
** @nam4rule GetHitStart Return the hit start
** @nam4rule GetHitEnd Return the hit end
** @nam4rule GetHitStrand Return the hit strand
** @nam4rule GetScore Return the score
**
** @argrule * rf [const EnsPRepeatFeature] Repeat Feature
**
** @valrule Adaptor [EnsPRepeatFeatureadaptor] Ensembl Repeat Feature Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Feature [EnsPFeature] Ensembl Feature
** @valrule RepeatConsensus [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @valrule HitStart [ajint] Hit start
** @valrule HitEnd [ajint] Hit end
** @valrule HitStrand [ajint] Hit strand
** @valrule Score [double] Score
**
** @fcategory use
******************************************************************************/




/* @func ensRepeatFeatureGetadaptor *******************************************
**
** Get the Object Adaptor element of an Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [r] rf [const EnsPRepeatFeature] Ensembl Repeat Feature
**
** @return [const EnsPRepeatFeatureadaptor] Ensembl Repeat Feature Adaptor
** @@
******************************************************************************/

const EnsPRepeatFeatureadaptor ensRepeatFeatureGetadaptor(
    const EnsPRepeatFeature rf)
{
    if(!rf)
	return NULL;
    
    return rf->Adaptor;
}




/* @func ensRepeatFeatureGetIdentifier ****************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [r] rf [const EnsPRepeatFeature] Ensembl Repeat Feature
**
** @return [ajuint] SQL database-internal identifier
** @@
******************************************************************************/

ajuint ensRepeatFeatureGetIdentifier(const EnsPRepeatFeature rf)
{
    if(!rf)
	return 0;
    
    return rf->Identifier;
}




/* @func ensRepeatFeatureGetFeature *******************************************
**
** Get the Ensembl Feature element of an Ensembl Repeat Feature.
**
** @param [r] rf [const EnsPRepeatFeature] Ensembl Repeat Feature
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

EnsPFeature ensRepeatFeatureGetFeature(const EnsPRepeatFeature rf)
{
    if(!rf)
	return NULL;
    
    return rf->Feature;
}




/* @func ensRepeatFeatureGetRepeatConsensus ***********************************
**
** Get the Ensembl Repeat Consensus element of an Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::RepeatFeature::repeat_consensus
** @param [r] rf [const EnsPRepeatFeature] Ensembl Repeat Feature
**
** @return [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @@
******************************************************************************/

EnsPRepeatConsensus ensRepeatFeatureGetRepeatConsensus(
    const EnsPRepeatFeature rf)
{
    if(!rf)
	return NULL;
    
    return rf->RepeatConsensus;
}




/* @func ensRepeatFeatureGetHitStart ******************************************
**
** Get the hit start element of an Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::RepeatFeature::hstart
** @param [r] rf [const EnsPRepeatFeature] Ensembl Repeat Feature
**
** @return [ajint] Hit start coordinate
** @@
******************************************************************************/

ajint ensRepeatFeatureGetHitStart(const EnsPRepeatFeature rf)
{
    if(!rf)
	return 0;
    
    return rf->HitStart;
}




/* @func ensRepeatFeatureGetHitEnd ********************************************
**
** Get the hit end element of an Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::RepeatFeature::hend
** @param [r] rf [const EnsPRepeatFeature] Ensembl Repeat Feature
**
** @return [ajint] Hit end coordinate
** @@
******************************************************************************/

ajint ensRepeatFeatureGetHitEnd(const EnsPRepeatFeature rf)
{
    if(!rf)
	return 0;
    
    return rf->HitEnd;
}




/* @func ensRepeatFeatureGetHitStrand *****************************************
**
** Get the hit strand element of an Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::RepeatFeature::hstrand
** @param [r] rf [const EnsPRepeatFeature] Ensembl Repeat Feature
**
** @return [ajint] Hit strand
** @@
******************************************************************************/

ajint ensRepeatFeatureGetHitStrand(const EnsPRepeatFeature rf)
{
    if(!rf)
	return 0;
    
    return 1;
}




/* @func ensRepeatFeatureGetScore *********************************************
**
** Get the score element of an Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::RepeatFeature::score
** @param [r] rf [const EnsPRepeatFeature] Ensembl Repeat Feature
**
** @return [double] Score
** @@
******************************************************************************/

double ensRepeatFeatureGetScore(const EnsPRepeatFeature rf)
{
    if(!rf)
	return 0;
    
    return rf->Score;
}




/* @section modifiers *********************************************************
**
** Functions for assigning elements of an Ensembl Repeat Feature object.
**
** @fdata [EnsPRepeatFeature]
** @fnote None
**
** @nam3rule Set Set one element of a Repeat Feature
** @nam4rule Setadaptor Set the Ensembl Repeat Feature Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetFeature Set the Ensembl Feature
** @nam4rule SetRepeatConsensus Set the Ensembl Repeat Consensus
** @nam4rule SetHitStart Set the hit start
** @nam4rule SetHitEnd Set the hit end
** @nam4rule SetScore Set the score
**
** @argrule * rf [EnsPRepeatFeature] Ensembl Repeat Feature object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensRepeatFeatureSetadaptor *******************************************
**
** Set the Ensembl Database Adaptor element of an Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::Storable::adaptor
** @param [u] rf [EnsPRepeatFeature] Ensembl Repeat Feature
** @param [r] rfa [EnsPRepeatFeatureadaptor] Ensembl Repeat Feature Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatFeatureSetadaptor(EnsPRepeatFeature rf,
                                  EnsPRepeatFeatureadaptor rfa)
{
    if(!rf)
        return ajFalse;
    
    rf->Adaptor = rfa;
    
    return ajTrue;
}




/* @func ensRepeatFeatureSetIdentifier ****************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::Storable::dbID
** @param [u] rf [EnsPRepeatFeature] Ensembl Repeat Feature
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatFeatureSetIdentifier(EnsPRepeatFeature rf, ajuint identifier)
{
    if(!rf)
        return ajFalse;
    
    rf->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensRepeatFeatureSetFeature *******************************************
**
** Set the Ensembl Feature element of an Ensembl Repeat Feature.
**
** @param [u] rf [EnsPRepeatFeature] Ensembl Repeat Feature
** @param [u] feature [EnsPFeature] Ensembl Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatFeatureSetFeature(EnsPRepeatFeature rf, EnsPFeature feature)
{
    if(!rf)
        return ajFalse;
    
    ensFeatureDel(&(rf->Feature));
    
    rf->Feature = ensFeatureNewRef(feature);
    
    return ajTrue;
}




/* @func ensRepeatFeatureSetRepeatConsensus ***********************************
**
** Set the Ensembl Repeat Consensus element of an Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::RepeatFeature::repeat_consensus
** @param [u] rf [EnsPRepeatFeature] Ensembl Repeat Feature
** @param [u] rc [EnsPRepeatConsensus] Ensembl Repeat Consensus
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatFeatureSetRepeatConsensus(EnsPRepeatFeature rf,
                                          EnsPRepeatConsensus rc)
{
    if(!rf)
        return ajFalse;
    
    ensRepeatConsensusDel(&(rf->RepeatConsensus));
    
    rf->RepeatConsensus = ensRepeatConsensusNewRef(rc);
    
    return ajTrue;
}




/* @func ensRepeatFeatureSetHitStart ******************************************
**
** Set the hit start element of an Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::RepeatFeature::hstart
** @param [u] rf [EnsPRepeatFeature] Ensembl Repeat Feature
** @param [r] hstart [ajint] Hit start
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatFeatureSetHitStart(EnsPRepeatFeature rf, ajuint hstart)
{
    if(!rf)
        return ajFalse;
    
    rf->HitStart = hstart;
    
    return ajTrue;
}




/* @func ensRepeatFeatureSetHitEnd ********************************************
**
** Set the hit end element of an Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::RepeatFeature::hend
** @param [u] rf [EnsPRepeatFeature] Ensembl Repeat Feature
** @param [r] hend [ajint] Hit end
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatFeatureSetHitEnd(EnsPRepeatFeature rf, ajuint hend)
{
    if(!rf)
        return ajFalse;
    
    rf->HitEnd = hend;
    
    return ajTrue;
}




/* @func ensRepeatFeatureSetScore *********************************************
**
** Set the score element of an Ensembl Repeat Feature.
**
** @cc Bio::EnsEMBL::RepeatFeature::score
** @param [u] rf [EnsPRepeatFeature] Ensembl Repeat Feature
** @param [r] score [double] Score
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatFeatureSetScore(EnsPRepeatFeature rf, double score)
{
    if(!rf)
        return ajFalse;
    
    rf->Score = score;
    
    return ajTrue;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Repeat Feature object.
**
** @fdata [EnsPRepeatFeature]
** @nam3rule Trace Report Ensembl Repeat Feature elements to debug file
**
** @argrule Trace rf [const EnsPRepeatFeature] Ensembl Repeat Feature
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensRepeatFeatureTrace ************************************************
**
** Trace an Ensembl Repeat Feature.
**
** @param [r] rf [const EnsPRepeatFeature] Ensembl Repeat Feature
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatFeatureTrace(const EnsPRepeatFeature rf, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!rf)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensRepeatFeatureTrace %p\n"
	    "%S  Adaptor %p\n"
	    "%S  Identifier %u\n"
	    "%S  Feature %p\n"
	    "%S  RepeatConsensus %p\n"
	    "%S  HitStart %d\n"
	    "%S  HitEnd %d\n"
	    "%S  Score %f\n"
	    "%S  Use %u\n",
	    indent, rf,
	    indent, rf->Adaptor,
	    indent, rf->Identifier,
	    indent, rf->Feature,
	    indent, rf->RepeatConsensus,
	    indent, rf->HitStart,
	    indent, rf->HitEnd,
	    indent, rf->Score,
	    indent, rf->Use);
    
    ensFeatureTrace(rf->Feature, level + 1);
    
    ensRepeatConsensusTrace(rf->RepeatConsensus, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensRepeatFeatureGetMemSize *******************************************
**
** Get the memory size in bytes of an Ensembl Repeat Feature.
**
** @param [r] rf [const EnsPRepeatFeature] Ensembl Repeat Feature
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensRepeatFeatureGetMemSize(const EnsPRepeatFeature rf)
{
    ajuint size = 0;
    
    if(!rf)
	return 0;
    
    size += (ajuint) sizeof (EnsORepeatFeature);
    
    size += ensFeatureGetMemSize(rf->Feature);
    
    size += ensRepeatConsensusGetMemSize(rf->RepeatConsensus);
    
    return size;
}




/* @datasection [EnsPRepeatFeatureadaptor] Repeat Feature Adaptor *************
**
** Functions for manipulating Ensembl Repeat Feature Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::RepeatFeatureadaptor CVS Revision: 1.55
**
** @nam2rule RepeatFeatureadaptor
**
******************************************************************************/




/* @funcstatic repeatFeatureadaptorCacheReference *****************************
**
** Wrapper function to reference an Ensembl Repeat Feature
** from an Ensembl Cache.
**
** @param [u] value [void*] Ensembl Repeat Feature
**
** @return [void*] Ensembl Repeat Feature or NULL
** @@
******************************************************************************/

static void *repeatFeatureadaptorCacheReference(void *value)
{
    /*
     ajDebug("repeatFeatureadaptorCacheReference\n"
	     "  value %p\n",
	     value);
     */
    
    if(!value)
	return NULL;
    
    return (void *) ensRepeatFeatureNewRef((EnsPRepeatFeature) value);
}




/* @funcstatic repeatFeatureadaptorCacheDelete ********************************
**
** Wrapper function to delete or de-reference an Ensembl Repeat Feature
** from an Ensembl Cache.
**
** @param [u] value [void**] Ensembl Repeat Feature address
**
** @return [void]
** @@
******************************************************************************/

static void repeatFeatureadaptorCacheDelete(void **value)
{
    /*
     ajDebug("repeatFeatureadaptorCacheDelete\n"
	     "  value %p\n",
	     value);
     */
    
    if(!value)
	return;
    
    ensRepeatFeatureDel((EnsPRepeatFeature *) value);
    
    return;
}




/* @funcstatic repeatFeatureadaptorCacheSize **********************************
**
** Wrapper function to determine the memory size of an Ensembl Repeat Feature
** from an Ensembl Cache.
**
** @param [r] value [const void*] Ensembl Repeat Feature
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

static ajuint repeatFeatureadaptorCacheSize(const void *value)
{
    if(!value)
	return 0;
    
    return ensRepeatFeatureGetMemSize((const EnsPRepeatFeature) value);
}




/* @funcstatic repeatFeatureadaptorGetFeature *********************************
**
** Wrapper function to get the Ensembl Feature of an
** Ensembl Repeat Feature from an Ensembl Feature Adaptor.
**
** @param [r] rf [const void*] Ensembl Repeat Feature
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

static EnsPFeature repeatFeatureadaptorGetFeature(const void *rf)
{
    if(!rf)
	return 0;
    
    return ensRepeatFeatureGetFeature((const EnsPRepeatFeature) rf);
}




static const char *repeatFeatureadaptorTables[] =
{
    "repeat_feature",
    "repeat_consensus",
    NULL
};




static const char *repeatFeatureadaptorColumns[] =
{
    "repeat_feature.repeat_feature_id",
    "repeat_feature.seq_region_id",
    "repeat_feature.seq_region_start",
    "repeat_feature.seq_region_end",
    "repeat_feature.seq_region_strand",
    "repeat_feature.repeat_consensus_id",
    "repeat_feature.repeat_start",
    "repeat_feature.repeat_end",
    "repeat_feature.analysis_id",
    "repeat_feature.score",
    "repeat_consensus.repeat_name",
    "repeat_consensus.repeat_class",
    "repeat_consensus.repeat_type",
    "repeat_consensus.repeat_consensus",
    NULL
};




static EnsOBaseadaptorLeftJoin repeatFeatureadaptorLeftJoin[] =
{
    {NULL, NULL}
};




static const char *repeatFeatureadaptorDefaultCondition =
"repeat_feature.repeat_consensus_id = "
"repeat_consensus.repeat_consensus_id";




static const char *repeatFeatureadaptorFinalCondition = NULL;




/* @funcstatic repeatFeatureadaptorFetchAllBySQL ******************************
**
** Run a SQL statement against an Ensembl Database Adaptor and consolidate the
** results into an AJAX List of Ensembl Repeat Features.
** The caller is responsible for deleting the Ensembl Repeat Features before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::DBSQL::RepeatFeatureadaptor::_objs_from_sth
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] rfs [AjPList] AJAX List of Ensembl Repeat Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool repeatFeatureadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                                const AjPStr statement,
                                                EnsPAssemblyMapper am,
                                                EnsPSlice slice,
                                                AjPList rfs)
{
    ajuint anid = 0;
    ajuint rcid = 0;
    ajuint rfid = 0;
    ajuint srid = 0;
    
    ajuint rpstart = 0;
    ajuint rpend   = 0;
    ajuint srstart = 0;
    ajuint srend   = 0;
    ajint srstrand = 0;
    
    ajint slstart  = 0;
    ajint slend    = 0;
    ajint slstrand = 0;
    ajint sllength = 0;
    
    double score = 0;
    
    AjPList mrs = NULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr rcname  = NULL;
    AjPStr rcclass = NULL;
    AjPStr rctype  = NULL;
    AjPStr rccons  = NULL;
    
    EnsPAnalysis analysis = NULL;
    EnsPAnalysisadaptor aa = NULL;
    
    EnsPAssemblyMapperadaptor ama = NULL;
    
    EnsPCoordSystemadaptor csa = NULL;
    
    EnsPFeature feature = NULL;
    
    EnsPMapperResult mr = NULL;
    
    EnsPRepeatConsensus rc = NULL;
    
    EnsPRepeatFeature rf         = NULL;
    EnsPRepeatFeatureadaptor rfa = NULL;
    
    EnsPSlice srslice   = NULL;
    EnsPSliceadaptor sa = NULL;
    
    /*
     ajDebug("repeatFeatureadaptorFetchAllBySQL\n"
	     "  dba %p\n"
	     "  statement %p\n"
	     "  am %p\n"
	     "  slice %p\n"
	     "  rfs %p\n",
	     dba,
	     statement,
	     am,
	     slice,
	     rfs);
     */
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    if(!rfs)
	return ajFalse;
    
    aa = ensRegistryGetAnalysisadaptor(dba);
    
    csa = ensRegistryGetCoordSystemadaptor(dba);
    
    rfa = ensRegistryGetRepeatFeatureadaptor(dba);
    
    sa = ensRegistryGetSliceadaptor(dba);
    
    if(slice)
	ama = ensRegistryGetAssemblyMapperadaptor(dba);
    
    mrs = ajListNew();
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	rfid     = 0;
	srid     = 0;
	srstart  = 0;
	srend    = 0;
	srstrand = 0;
	rcid     = 0;
	rpstart  = 0;
	rpend    = 0;
	anid     = 0;
	score    = 0;
	rcname   = ajStrNew();
	rcclass  = ajStrNew();
	rctype   = ajStrNew();
	rccons   = ajStrNew();
	
	sqlr = ajSqlrowiterGet(sqli);
	
	ajSqlcolumnToUint(sqlr, &rfid);
	ajSqlcolumnToUint(sqlr, &srid);
	ajSqlcolumnToUint(sqlr, &srstart);
	ajSqlcolumnToUint(sqlr, &srend);
	ajSqlcolumnToInt(sqlr, &srstrand);
	ajSqlcolumnToUint(sqlr, &rcid);
	ajSqlcolumnToUint(sqlr, &rpstart);
	ajSqlcolumnToUint(sqlr, &rpend);
	ajSqlcolumnToUint(sqlr, &anid);
	ajSqlcolumnToDouble(sqlr, &score);
	ajSqlcolumnToStr(sqlr, &rcname);
	ajSqlcolumnToStr(sqlr, &rcclass);
	ajSqlcolumnToStr(sqlr, &rctype);
	ajSqlcolumnToStr(sqlr, &rccons);
	
	/* Need to get the internal Ensembl Sequence Region identifier. */
	
	srid = ensCoordSystemadaptorGetInternalSeqRegionIdentifier(csa, srid);
	
	/*
	** Since the Ensembl SQL schema defines Sequence Region start and end
	** coordinates as unsigned integers for all Features, the range needs
	** checking.
	*/
	
	if(srstart <= INT_MAX)
	    slstart = (ajint) srstart;
	else
	    ajFatal("repeatFeatureadaptorFetchAllBySQL got a "
		    "Sequence Region start coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srstart, INT_MAX);
	
	if(srend <= INT_MAX)
	    slend = (ajint) srend;
	else
	    ajFatal("repeatFeatureadaptorFetchAllBySQL got a "
		    "Sequence Region end coordinate (%u) outside the "
		    "maximum integer limit (%d).",
		    srend, INT_MAX);
	
	slstrand = srstrand;
	
	/* Create an Ensembl Repeat Consensus. */
	
	rc = ensRepeatConsensusNew(dba,
				   rcid,
				   rcname,
				   rcclass,
				   rctype,
				   rccons,
				   ajStrGetLen(rccons));
	
	ajStrDel(&rcname);
	ajStrDel(&rcclass);
	ajStrDel(&rctype);
	ajStrDel(&rccons);
	
	/* Fetch the corresponding Ensembl Analysis. */
	
	ensAnalysisadaptorFetchByIdentifier(aa, anid, &analysis);
	
	/* Fetch a Slice spanning the entire Sequence Region. */
	
	ensSliceadaptorFetchBySeqRegionIdentifier(sa, srid, 0, 0, 0, &srslice);
	
	/*
	** FIXME: There is a difference to the Exon Adaptor.
	** The Exonadaptor fetches an Assembly Mapper in case a Slice but no
	** Assembly Mapper has been provided.
	**
	** FIXME: There are big problems with signedess to resolve!
	** srstart from database is ajuint, ensSliceGetStart is ajint!
	** we need a second set of variables that is signed and
	** we should probably test before the assignment whether the value of
	** the unsigned is out of the range of the signed???
	*/
	
	/*
	** Increase the reference counter of the Ensembl Assembly Mapper if
	** one has been specified, otherwise fetch it from the database if a
	** destination Slice has been specified.
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
		/*
		** FIXME: For debugging only.
		** 
		** ajDebug("repeatFeatureadaptorFetchAllBySQL mapped "
		**	 "Repeat Feature %u on Sequence Region %u "
		**	 "start %u end %u strand %d to gap.\n",
		**	 rfid,
		**	 srid,
		**	 srstart,
		**	 srend,
		**	 srstrand);
                **
		** ensMapperResultTrace(mr, 1);
		*/
		
		ensSliceDel(&srslice);
		
		ensAnalysisDel(&analysis);
		
		ensRepeatConsensusDel(&rc);
		
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
		ajFatal("repeatFeatureadaptorFetchAllBySQL got a Slice, "
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
	    
	    /* Throw away Repeat Features off the end of the requested Slice. */
	    
	    if((slstart > sllength) ||
		(slend < 1) ||
		(srid != ensSliceGetSeqRegionIdentifier(slice)))
	    {
		ensSliceDel(&srslice);
		
		ensAnalysisDel(&analysis);
		
		ensRepeatConsensusDel(&rc);
		
		ensAssemblyMapperDel(&am);
		
		continue;
	    }
	    
	    /* Delete the Sequence Region Slice and set the requested Slice. */
	    
	    ensSliceDel(&srslice);
	    
	    srslice = ensSliceNewRef(slice);
	}
	
	/* Finally, create a new Ensembl Repeat Feature. */
	
	feature = ensFeatureNewS(analysis,
				 srslice,
				 slstart,
				 slend,
				 slstrand);
	
	rf = ensRepeatFeatureNew(rfa,
				 rfid,
				 feature,
				 rc,
				 rpstart,
				 rpend,
				 score);
	
	ajListPushAppend(rfs, (void *) rf);
	
	ensFeatureDel(&feature);
	
	ensAssemblyMapperDel(&am);
	
	ensSliceDel(&srslice);
	
	ensAnalysisDel(&analysis);
	
	ensRepeatConsensusDel(&rc);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    ajListFree(&mrs);
    
    return ajTrue;
}




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Repeat Feature Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Repeat Feature Adaptor. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPRepeatFeatureadaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule New dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @argrule Obj object [EnsPRepeatFeatureadaptor] Ensembl Repeat
**                                                Feature Adaptor
** @argrule Ref object [EnsPRepeatFeatureadaptor] Ensembl Repeat
**                                                Feature Adaptor
**
** @valrule * [EnsPRepeatFeatureadaptor] Ensembl Repeat Feature Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensRepeatFeatureadaptorNew *******************************************
**
** Default Ensembl Repeat Feature Adaptor constructor.
**
** @cc Bio::EnsEMBL::DBSQL::RepeatFeatureadaptor::new
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPRepeatFeatureadaptor] Ensembl Repeat Feature Adaptor or NULL
** @@
******************************************************************************/

EnsPRepeatFeatureadaptor ensRepeatFeatureadaptorNew(EnsPDatabaseadaptor dba)
{
    EnsPRepeatFeatureadaptor rfa = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(rfa);
    
    rfa->Adaptor =
	ensFeatureadaptorNew(dba,
			     repeatFeatureadaptorTables,
			     repeatFeatureadaptorColumns,
			     repeatFeatureadaptorLeftJoin,
			     repeatFeatureadaptorDefaultCondition,
			     repeatFeatureadaptorFinalCondition,
			     repeatFeatureadaptorFetchAllBySQL,
			     (void* (*)(const void* key)) NULL, /* Fread */
			     repeatFeatureadaptorCacheReference,
			     (AjBool (*)(const void* value)) NULL, /* Fwrite */
			     repeatFeatureadaptorCacheDelete,
			     repeatFeatureadaptorCacheSize,
			     repeatFeatureadaptorGetFeature,
			     "RepeatFeature");
    
    return rfa;
}




/* @func ensRepeatFeatureadaptorDel *******************************************
**
** Default destructor for an Ensembl Repeat Feature Adaptor.
**
** @param [d] Prfa [EnsPRepeatFeatureadaptor*] Ensembl Repeat Feature
**                                             Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensRepeatFeatureadaptorDel(EnsPRepeatFeatureadaptor *Prfa)
{
    EnsPRepeatFeatureadaptor pthis = NULL;
    
    if(!Prfa)
	return;
    
    if(!*Prfa)
	return;

    pthis = *Prfa;
    
    ensFeatureadaptorDel(&pthis->Adaptor);
    
    AJFREE(pthis);

    *Prfa = NULL;
    
    return;
}




/* @func ensRepeatFeatureadaptorFetchAllBySlice *******************************
**
** Fetch all Ensembl Repeat Features on an Ensembl Slice.
** The caller is responsible for deleting the Ensembl Repeat Features before
** deleting the AJAX List.
**
** @cc Bio::EnsEMBL::DBSQL::RepeatFeatureadaptor::fetch_all_by_Slice
** @param [r] rfa [const EnsPRepeatFeatureadaptor] Ensembl Repeat
**                                                 Feature Adaptor
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [r] anname [const AjPStr] Ensembl Analysis name
** @param [r] rctype [const AjPStr] Ensembl Repeat Consensus type
** @param [r] rcclass [const AjPStr] Ensembl Repeat Consensus class
** @param [r] rcname [const AjPStr] Ensembl Repeat Consensus name
** @param [u] rfs [AjPList] AJAX List of Ensembl Repeat Features
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensRepeatFeatureadaptorFetchAllBySlice(
    const EnsPRepeatFeatureadaptor rfa,
    EnsPSlice slice,
    const AjPStr anname,
    const AjPStr rctype,
    const AjPStr rcclass,
    const AjPStr rcname,
    AjPList rfs)
{
    char *txtrcclass = NULL;
    char *txtrcname = NULL;
    char *txtrctype = NULL;
    
    AjPStr constraint = NULL;
    
    /*
     ajDebug("ensRepeatFeatureadaptorFetchAllBySlice\n"
	     "  rfa %p\n"
	     "  slice %p\n"
	     "  anname '%S'\n"
	     "  rctype '%S'\n"
	     "  rcclass '%S'\n"
	     "  rcname '%S'\n"
	     "  rfs %p\n",
	     rfa,
	     slice,
	     anname,
	     rctype,
	     rcclass,
	     rcname,
	     rfs);
     */
    
    if(!rfa)
	return ajFalse;
    
    if(!slice)
	return ajFalse;
    
    if(!anname)
	return ajFalse;
    
    if(!rfs)
	return ajFalse;
    
    if(rctype && ajStrGetLen(rctype))
    {
	ensFeatureadaptorEscapeC(rfa->Adaptor, &txtrctype, rctype);
	
	constraint =
	    ajFmtStr("repeat_consensus.repeat_type = '%s'", txtrctype);
	
	ajCharDel(&txtrctype);
    }
    
    if(rcclass && ajStrGetLen(rcclass))
    {
	ensFeatureadaptorEscapeC(rfa->Adaptor, &txtrcclass, rcclass);
	
	if(constraint)
	    ajFmtPrintAppS(&constraint,
			   " AND repeat_consensus.repeat_class = '%s'",
			   txtrcclass);
	else
	    constraint =
		ajFmtStr("repeat_consensus.repeat_class = '%s'", txtrcclass);
	
	ajCharDel(&txtrcclass);
    }
    
    if(rcname && ajStrGetLen(rcname))
    {
	ensFeatureadaptorEscapeC(rfa->Adaptor, &txtrcname, rcname);
	
	if(constraint)
	    ajFmtPrintAppS(&constraint,
			   " AND repeat_consensus.repeat_name = '%s'",
			   txtrcname);
	else
	    constraint =
		ajFmtStr("repeat_consensus.repeat_name = '%s'", txtrcname);
	
	ajCharDel(&txtrcname);
    }
    
    if(!constraint)
	constraint = ajStrNew();
    
    ensFeatureadaptorFetchAllBySliceConstraint(rfa->Adaptor,
					       slice,
					       constraint,
					       anname,
					       rfs);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}
