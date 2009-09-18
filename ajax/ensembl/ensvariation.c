/******************************************************************************
** @source Ensembl Genetic Variation functions
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

#include "ensvariation.h"
#include "ensmetainformation.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

static const char *gvIndividualGender[] =
{
    NULL,
    "Male",
    "Female",
    "Unknown",
    NULL
};




static const char *gvIndividualType[] =
{
    NULL,
    "Fully_inbred",
    "Partly_inbred",
    "Outbred",
    "Mutant",
    "Unknown",
    NULL
};




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */

extern EnsPMetaInformationAdaptor
ensRegistryGetMetaInformationAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPGVIndividualAdaptor
ensRegistryGetGVIndividualAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPGVPopulationAdaptor
ensRegistryGetGVPopulationAdaptor(EnsPDatabaseAdaptor dba);

extern EnsPGVVariationAdaptor
ensRegistryGetGVVariationAdaptor(EnsPDatabaseAdaptor dba);

static AjBool gvIndividualAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblyMapper am,
                                               EnsPSlice slice,
                                               AjPList gvis);

static AjBool gvPopulationAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblyMapper am,
                                               EnsPSlice slice,
                                               AjPList gvps);

static void gvVariationAdaptorClearAlleles(void **key, void **value, void *cl);

static void gvVariationAdaptorClearPopulations(void **key, void **value,
                                               void *cl);

static void gvVariationAdaptorClearSynonyms(void **key, void **value, void *cl);

static AjBool gvVariationAdaptorFetchAllBySQL(EnsPGVVariationAdaptor gvva,
                                              const AjPStr statement,
                                              AjPList gvvs);




/* @filesection ensvariation **************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPGVSample] Genetic Variation Sample ***********************
**
** Functions for manipulating Ensembl Genetic Variation Sample objects
**
** @cc Bio::EnsEMBL::Variation::Sample CVS Revision: 1.2
**
** @nam2rule GVSample
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Sample by pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Sample. The target pointer does not need to be initialised
** to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGVSample]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGVSample] Ensembl Genetic Variation Sample
** @argrule Ref object [EnsPGVSample] Ensembl Genetic Variation Sample
**
** @valrule * [EnsPGVSample] Ensembl Genetic Variation Sample
**
** @fcategory new
******************************************************************************/




/* @func ensGVSampleNew *******************************************************
**
** Default constructor for an Ensembl Genetic Variation Sample.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvsa [EnsPGVSampleAdaptor] Ensembl Genetic Variation
**                                       Sample Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Sample::new
** @param [u] name [AjPStr] Name
** @param [u] description [AjPStr] Description
** @param [r] size [ajuint] Size
**
** @return [EnsPGVSample] Ensembl Genetic Variation Sample or NULL
** @@
******************************************************************************/

EnsPGVSample ensGVSampleNew(EnsPGVSampleAdaptor gvsa,
                            ajuint identifier,
                            AjPStr name,
                            AjPStr description,
                            ajuint size)
{
    EnsPGVSample gvs = NULL;
    
    AJNEW0(gvs);
    
    gvs->Use = 1;
    
    gvs->Identifier = identifier;
    
    gvs->Adaptor = gvsa;
    
    if(name)
        gvs->Name = ajStrNewRef(name);
    
    if(description)
        gvs->Description = ajStrNewRef(description);
    
    gvs->Size = size;
    
    return gvs;
}




/* @func ensGVSampleNewObj ****************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGVSample] Ensembl Genetic Variation Sample
**
** @return [EnsPGVSample] Ensembl Genetic Variation Sample or NULL
** @@
******************************************************************************/

EnsPGVSample ensGVSampleNewObj(const EnsPGVSample object)
{
    EnsPGVSample gvs = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(gvs);
    
    gvs->Use = 1;
    
    gvs->Identifier = object->Identifier;
    
    gvs->Adaptor = object->Adaptor;
    
    if(object->Name)
        gvs->Name = ajStrNewRef(object->Name);
    
    if(object->Description)
        gvs->Description = ajStrNewRef(object->Description);
    
    gvs->Size = object->Size;
    
    return gvs;
}




/* @func ensGVSampleNewRef ****************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvs [EnsPGVSample] Ensembl Genetic Variation Sample
**
** @return [EnsPGVSample] Ensembl Genetic Variation Sample
** @@
******************************************************************************/

EnsPGVSample ensGVSampleNewRef(EnsPGVSample gvs)
{
    if(!gvs)
	return NULL;
    
    gvs->Use++;
    
    return gvs;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Sample.
**
** @fdata [EnsPGVSample]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Sample object
**
** @argrule * Pgvs [EnsPGVSample*] Genetic Variation Sample object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGVSampleDel *******************************************************
**
** Default destructor for an Ensembl Genetic Variation Sample.
**
** @param [d] Pgvs [EnsPGVSample*] Ensembl Genetic Variation Sample address
**
** @return [void]
** @@
******************************************************************************/

void ensGVSampleDel(EnsPGVSample *Pgvs)
{
    EnsPGVSample pthis = NULL;
    
    /*
     ajDebug("ensGVSampleDel\n"
	     "  *Psample %p\n",
	     *Pgvs);
     
     ensGVSampleTrace(*Pgvs, 1);
     */
    
    if(!Pgvs)
        return;
    
    if(!*Pgvs)
        return;

    pthis = *Pgvs;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pgvs = NULL;
	
	return;
    }
    
    ajStrDel(&pthis->Name);
    ajStrDel(&pthis->Description);
    
    AJFREE(pthis);

    *Pgvs = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Sample object.
**
** @fdata [EnsPGVSample]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Sample attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Sample Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetName Return the name
** @nam4rule GetDescription Return the description
** @nam4rule GetSize Return the size
**
** @argrule * gvs [const EnsPGVSample] Genetic Variation Sample
**
** @valrule Adaptor [EnsPGVSampleAdaptor] Ensembl Genetic Variation
**                                        Sample Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Name [AjPStr] Name
** @valrule Description [AjPStr] Description
**
** @fcategory use
******************************************************************************/




/* @func ensGVSampleGetAdaptor ************************************************
**
** Get the Ensembl Genetic Variation Sample Adaptor element of an
** Ensembl Genetic Variation Sample.
**
** @param [r] gvs [const EnsPGVSample] Ensembl Genetic Variation Sample
**
** @return [EnsPGVSampleAdaptor] Ensembl Genetic Variation Sample Adaptor
** @@
******************************************************************************/

EnsPGVSampleAdaptor ensGVSampleGetAdaptor(const EnsPGVSample gvs)
{
    if(!gvs)
        return NULL;
    
    return gvs->Adaptor;
}




/* @func ensGVSampleGetIdentifier *********************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Sample.
**
** @param [r] gvs [const EnsPGVSample] Ensembl Genetic Variation Sample
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGVSampleGetIdentifier(const EnsPGVSample gvs)
{
    if(!gvs)
        return 0;
    
    return gvs->Identifier;
}




/* @func ensGVSampleGetName ***************************************************
**
** Get the name element of an Ensembl Genetic Variation Sample.
**
** @param  [r] gvs [const EnsPGVSample] Ensembl Genetic Variation Sample
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensGVSampleGetName(const EnsPGVSample gvs)
{
    if(!gvs)
        return NULL;
    
    return gvs->Name;
}




/* @func ensGVSampleGetDescription ********************************************
**
** Get the description element of an Ensembl Genetic Variation Sample.
**
** @param  [r] gvs [const EnsPGVSample] Ensembl Genetic Variation Sample
**
** @return [AjPStr] Description
** @@
******************************************************************************/

AjPStr ensGVSampleGetDescription(const EnsPGVSample gvs)
{
    if(!gvs)
        return NULL;
    
    return gvs->Description;
}




/* @func ensGVSampleGetSize ***************************************************
**
** Get the size element of an Ensembl Genetic Variation Sample.
**
** @param  [r] gvs [const EnsPGVSample] Ensembl Genetic Variation Sample
**
** @return [ajuint] Size
** @@
******************************************************************************/

ajuint ensGVSampleGetSize(const EnsPGVSample gvs)
{
    if(!gvs)
        return 0;
    
    return gvs->Size;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Sample object.
**
** @fdata [EnsPGVSample]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Sample
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Sample Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetName Set the name
** @nam4rule SetDescription Set the description
** @nam4rule SetSize Set the size
**
** @argrule * gvs [EnsPGVSample] Ensembl Genetic Variation Sample object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGVSampleSetAdaptor ************************************************
**
** Set the Ensembl Genetic Variation Sample Adaptor element of an
** Ensembl Genetic Variation Sample.
**
** @param [u] gvs [EnsPGVSample] Ensembl Genetic Variation Sample
** @param [r] gvsa [EnsPGVSampleAdaptor] Ensembl Genetic Variation
**                                       Sample Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVSampleSetAdaptor(EnsPGVSample gvs, EnsPGVSampleAdaptor gvsa)
{
    if(!gvs)
        return ajFalse;
    
    gvs->Adaptor = gvsa;
    
    return ajTrue;
}




/* @func ensGVSampleSetIdentifier *********************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Sample.
**
** @param [u] gvs [EnsPGVSample] Ensembl Genetic Variation Sample
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVSampleSetIdentifier(EnsPGVSample gvs, ajuint identifier)
{
    if(!gvs)
        return ajFalse;
    
    gvs->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGVSampleSetName ***************************************************
**
** Set the name element of an Ensembl Genetic Variation Sample.
**
** @param [u] gvs [EnsPGVSample] Ensembl Genetic Variation Sample
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVSampleSetName(EnsPGVSample gvs, AjPStr name)
{
    if(!gvs)
        return ajFalse;
    
    ajStrDel(&gvs->Name);
    
    if(name)
	gvs->Name = ajStrNewRef(name);
    
    return ajTrue;
}




/* @func ensGVSampleSetDescription ********************************************
**
** Set the description element of an Ensembl Genetic Variation Sample.
**
** @param [u] gvs [EnsPGVSample] Ensembl Genetic Variation Sample
** @param [u] description [AjPStr] Description
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVSampleSetDescription(EnsPGVSample gvs, AjPStr description)
{
    if(!gvs)
        return ajFalse;
    
    ajStrDel(&gvs->Description);
    
    if(description)
	gvs->Description = ajStrNewRef(description);
    
    return ajTrue;
}




/* @func ensGVSampleSetSize ***************************************************
**
** Set the size element of an Ensembl Genetic Variation Sample.
**
** @param [u] gvs [EnsPGVSample] Ensembl Genetic Variation Sample
** @param [r] size [ajuint] Size
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVSampleSetSize(EnsPGVSample gvs, ajuint size)
{
    if(!gvs)
        return ajFalse;
    
    gvs->Size = size;
    
    return ajTrue;
}




/* @func ensGVSampleGetMemSize ************************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Sample.
**
** @param [r] gvs [const EnsPGVSample] Ensembl Genetic Variation Sample
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGVSampleGetMemSize(const EnsPGVSample gvs)
{
    ajuint size = 0;
    
    if(!gvs)
	return 0;
    
    size += (ajuint) sizeof (EnsOGVSample);
    
    if(gvs->Name)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvs->Name);
    }
    
    if(gvs->Description)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvs->Description);
    }
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Genetic Variation Sample object.
**
** @fdata [EnsPGVSample]
** @nam3rule Trace Report Ensembl Genetic Variation Sample elements to
**                 debug file
**
** @argrule Trace gvs [const EnsPGVSample] Ensembl Genetic Variation Sample
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGVSampleTrace *****************************************************
**
** Trace an Ensembl Genetic Variation Sample.
**
** @param [r] gvs [const EnsPGVSample] Ensembl Genetic Variation Sample
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVSampleTrace(const EnsPGVSample gvs, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!gvs)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensGVSampleTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Name '%S'\n"
	    "%S  Description '%S'\n"
	    "%S  Size %u\n",
	    indent, gvs,
	    indent, gvs->Use,
	    indent, gvs->Identifier,
	    indent, gvs->Adaptor,
	    indent, gvs->Name,
	    indent, gvs->Description,
	    indent, gvs->Size);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @datasection [EnsPGVSampleAdaptor] Genetic Variation Sample Adaptor ********
**
** Functions for manipulating Ensembl Genetic Variation Sample Adaptor
** objects
**
** @nam2rule GVSampleAdaptor
**
******************************************************************************/




/* @func ensGVSampleAdaptorNew ************************************************
**
** Default constructor for an Ensembl Genetic Variation Sample Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::BaseAdaptor::new
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] Ptables [const char **] Array of char strings of table names
** @param [r] Pcolumns [const char **] Array of char strings of column names
** @param [r] left_join [EnsOBaseAdaptorLeftJoin*] Address of an array of
**                                   Ensembl Base Adaptor Left Join Statements
** @param [r] condition [const char *] Default condition in a
**                                     SQL SELECT statement
** @param [r] final [const char *] Final condition in a
**                                 SQL SELECT statement
** @param [f] Fquery [(AjBool)] Pointer to a function to handle the
**                              Ensembl Object-specific SQL statement
**
** @return [EnsPGVSampleAdaptor] Ensembl Genetic Variation Sample Adaptor
**                               or NULL
** @@
******************************************************************************/

EnsPGVSampleAdaptor ensGVSampleAdaptorNew(
    EnsPDatabaseAdaptor dba,
    const char **Ptables,
    const char **Pcolumns,
    EnsOBaseAdaptorLeftJoin *left_join,
    const char *condition,
    const char *final,
    AjBool Fquery(EnsPDatabaseAdaptor dba,
                  const AjPStr statement,
                  EnsPAssemblyMapper am,
                  EnsPSlice slice,
                  AjPList objects))

{
    EnsPGVSampleAdaptor gvsa = NULL;
    
    AJNEW0(gvsa);
    
    gvsa->Adaptor = ensBaseAdaptorNew(dba,
				      Ptables,
				      Pcolumns,
				      left_join,
				      condition,
				      final,
				      Fquery);
    
    if(!gvsa->Adaptor)
	AJFREE(gvsa);
    
    return gvsa;
}




/* @func ensGVSampleAdaptorDel ************************************************
**
** Default destructor for an Ensembl Gentic Variation Sample Adaptor.
**
** @param [d] Pgvsa [EnsPGVSampleAdaptor*] Ensembl Genetic Variation
**                                         Sample Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGVSampleAdaptorDel(EnsPGVSampleAdaptor *Pgvsa)
{
    EnsPGVSampleAdaptor pthis = NULL;
    
    if(!Pgvsa)
	return;
    
    if(!*Pgvsa)
	return;

    pthis = *Pgvsa;
    
    ensBaseAdaptorDel(&pthis->Adaptor);
    
    AJFREE(pthis);

    *Pgvsa = NULL;
    
    return;
}




/* @func ensGVSampleAdaptorGetAdaptor *****************************************
**
** Get the Ensembl Base Adaptor element of an
** Ensembl Genetic Variation Sample Adaptor.
**
** @param [r] gvsa [const EnsPGVSampleAdaptor] Ensembl Genetic Variation
**                                             Sample Adaptor
**
** @return [EnsPBaseAdaptor] Ensembl Base Adaptor
** @@
******************************************************************************/

EnsPBaseAdaptor ensGVSampleAdaptorGetAdaptor(const EnsPGVSampleAdaptor gvsa)
{
    if(!gvsa)
        return NULL;
    
    return gvsa->Adaptor;
}




/* @func ensGVSampleAdaptorFetchAllSynonymsByIdentifier ***********************
**
** Fetch all Ensembl Genetic Variation Sample synonyms for an
** Ensembl Genetic Variation Sample identifier.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::SampleAdaptor::fetch_synonyms
** @param [r] gvsa [const EnsPGVSampleAdaptor] Ensembl Genetic Variation
**                                             Sample Adaptor
** @param [r] identifier [ajuint] Ensembl Genetic Variation Sample identifier
** @param [r] source [const AjPStr] Source
** @param [u] synonyms [AjPList] AJAX List of synonym AJAX Strings
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVSampleAdaptorFetchAllSynonymsByIdentifier(EnsPGVSampleAdaptor gvsa,
                                                      ajuint identifier,
                                                      const AjPStr source,
                                                      AjPList synonyms)
{
    char *txtsource = NULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr name      = NULL;
    AjPStr statement = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!gvsa)
	return ajFalse;
    
    if(!synonyms)
	return ajFalse;
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(gvsa->Adaptor);
    
    if(!dba)
	return ajFalse;
    
    if(source)
    {
	ensDatabaseAdaptorEscapeCS(dba, &txtsource, source);
	
	statement = ajFmtStr("SELECT "
			     "sample_synonym.name "
			     "FROM "
			     "sample_synonym, "
			     "source "
			     "WHERE "
			     "sample_synonym.sample_id = %u "
			     "AND "
			     "sample_synonym.source_id = source.source_id "
			     "AND "
			     "source.name = '%s'",
			     identifier,
			     txtsource);
	
	ajCharDel(&txtsource);
    }
    else
	statement = ajFmtStr("SELECT "
			     "sample_synonym.name "
			     "FROM "
			     "sample_synonym "
			     "WHERE "
			     "sample_synonym.sample_id = %u",
			     identifier);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	name = ajStrNew();
	
	sqlr = ajSqlRowIterGet(sqli);
	
        ajSqlColumnToStr(sqlr, &name);
	
	ajListPushAppend(synonyms, (void *) ajStrNewRef(name));
	
	ajStrDel(&name);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensGVSampleAdaptorFetchAllIdentifiersBySynonym ***********************
**
** Fetch all Ensembl Genetic Variation Sample identifiers for an
** Ensembl Genetic Variation Sample synonym.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::SampleAdaptor::fetch_sample_by_synonym
** @param [r] gvsa [const EnsPGVSampleAdaptor] Ensembl Genetic Variation
**                                             Sample Adaptor
** @param [r] synonym [const AjPStr] Ensembl Genetic Variation Sample synonym
** @param [r] source [const AjPStr] Source
** @param [u] idlist [AjPList] AJAX List of Ensembl Genetic Variation
**                             Sample identifiers
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVSampleAdaptorFetchAllIdentifiersBySynonym(
    EnsPGVSampleAdaptor gvsa,
    const AjPStr synonym,
    const AjPStr source,
    AjPList idlist)
{
    char *txtsynonym = NULL;
    char *txtsource  = NULL;
    
    ajuint *Pidentifier = NULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!gvsa)
	return ajFalse;
    
    if(!idlist)
	return ajFalse;
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(gvsa->Adaptor);
    
    if(!dba)
	return ajFalse;
    
    ensDatabaseAdaptorEscapeCS(dba, &txtsynonym, synonym);
    
    if(source)
    {
	ensDatabaseAdaptorEscapeCS(dba, &txtsource, source);
	
	statement = ajFmtStr("SELECT "
			     "sample_synonym.sample_id "
			     "FROM "
			     "sample_synonym, "
			     "source "
			     "WHERE "
			     "sample_synonym.name = '%s' AND "
			     "sample_synonym.source_id = source.source_id AND "
			     "source.name = '%s'",
			     txtsynonym,
			     txtsource);
	
	ajCharDel(&txtsource);
    }
    else
	statement = ajFmtStr("SELECT "
			     "sample_synonym.sample_id "
			     "FROM "
			     "sample_synonym "
			     "WHERE "
			     "sample_synonym.name = '%s'",
			     txtsynonym);
    
    
    ajCharDel(&txtsynonym);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	AJNEW0(Pidentifier);
	
	sqlr = ajSqlRowIterGet(sqli);
	
        ajSqlColumnToUint(sqlr, Pidentifier);
	
	ajListPushAppend(idlist, (void *) Pidentifier);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensGVSampleAdaptorFetchByIdentifier **********************************
**
** Fetch an Ensembl Genetic Variation Sample by its SQL database-internal
** identifier.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::SampleAdaptor::fetch_by_dbID
** @param [r] gvsa [const EnsPGVSampleAdaptor] Ensembl Genetic Variation
**                                             Sample Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pgvs [EnsPGVSample*] Ensembl Genetic Variation Sample address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVSampleAdaptorFetchByIdentifier(EnsPGVSampleAdaptor gvsa,
                                           ajuint identifier,
                                           EnsPGVSample *Pgvs)
{
    AjPList gvss = NULL;
    
    AjPStr constraint = NULL;
    
    EnsPGVSample gvs = NULL;
    
    if(!gvsa)
	return ajFalse;
    
    if(!Pgvs)
	return ajFalse;
    
    /*
    ** The constraint must contain the sample identifier field that is used
    ** either for individuals or populations.
    */
    
    constraint = ajFmtStr("%s.sample_id = %d",
			  ensBaseAdaptorGetPrimaryTable(gvsa->Adaptor),
			  identifier);
    
    gvss = ajListNew();
    
    ensBaseAdaptorGenericFetch(gvsa->Adaptor,
			       constraint,
			       NULL,
			       NULL,
			       gvss);
    
    if(ajListGetLength(gvss) > 1)
	ajDebug("ensGVSampleAdaptorFetchByIdentifier got more than one "
		"Sample for constraint '%S'\n", constraint);
    
    ajListPop(gvss, (void **) Pgvs);
    
    while(ajListPop(gvss, (void **) &gvs))
	ensGVSampleDel(&gvs);
    
    ajListFree(&gvss);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/*
** TODO: The following method is missing.
** Bio::EnsEMBL::Variation::DBSQL::SampleAdaptor::fetch_all_by_dbID_list
*/




/* @datasection [EnsPGVIndividual] Genetic Variation Individual ***************
**
** Functions for manipulating Ensembl Genetic Variation Individual objects
**
** @nam2rule GVIndividual
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Individual by
** pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Individual. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGVIndividual]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGVIndividual] Ensembl Genetic Variation Individual
** @argrule Ref object [EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @valrule * [EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @fcategory new
******************************************************************************/




/* @func ensGVIndividualNew ***************************************************
**
** Default constructor for an Ensembl Genetic Variation Individual.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvia [EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                           Individual Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Sample::new
** @param [u] sample [EnsPGVSample] Ensembl Genetic Variation Sample
** @cc Bio::EnsEMBL::Variation::Individual::new
** @param [u] father [EnsPGVIndividual] Father
** @param [u] mother [EnsPGVIndividual] Mother
** @param [r] gender [AjEnum] Gender
** @param [r] type [AjEnum] Type
** @param [u] description [AjPStr] Description
**
** @return [EnsPGVIndividual] Ensembl Genetic Variation Individual or NULL
** @@
******************************************************************************/

EnsPGVIndividual ensGVIndividualNew(EnsPGVIndividualAdaptor gvia,
                                    ajuint identifier,
                                    EnsPGVSample gvs,
                                    EnsPGVIndividual father,
                                    EnsPGVIndividual mother,
                                    AjEnum gender,
                                    AjEnum type,
                                    AjPStr description)
{
    EnsPGVIndividual gvi = NULL;
    
    if(!gvs)
	return NULL;
    
    AJNEW0(gvi);
    
    gvi->Use = 1;
    
    gvi->Identifier = identifier;
    
    gvi->Adaptor = gvia;
    
    gvi->GVSample = ensGVSampleNewRef(gvs);
    
    gvi->Father = ensGVIndividualNewRef(father);
    
    gvi->Mother = ensGVIndividualNewRef(mother);
    
    gvi->Gender = gender;
    
    gvi->Type = type;
    
    if(description)
	gvi->Description = ajStrNewRef(description);
    
    return gvi;
}




/* @func ensGVIndividualNewObj ************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGVIndividual] Ensembl Genetic Variation
**                                            Individual
**
** @return [EnsPGVIndividual] Ensembl Genetic Variation Individual or NULL
** @@
******************************************************************************/

EnsPGVIndividual ensGVIndividualNewObj(const EnsPGVIndividual object)
{
    EnsPGVIndividual gvi = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(gvi);
    
    gvi->Use = 1;
    
    gvi->Identifier = object->Identifier;
    
    gvi->Adaptor = object->Adaptor;
    
    gvi->GVSample = ensGVSampleNewRef(object->GVSample);
    
    gvi->Father = ensGVIndividualNewRef(object->Father);
    
    gvi->Mother = ensGVIndividualNewRef(object->Mother);
    
    gvi->Gender = object->Gender;
    
    gvi->Type = object->Type;
    
    if(object->Description)
	gvi->Description = ajStrNewRef(object->Description);
    
    return gvi;
}




/* @func ensGVIndividualNewRef ************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvi [EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @return [EnsPGVIndividual] Ensembl Genetic Variation Individual
** @@
******************************************************************************/

EnsPGVIndividual ensGVIndividualNewRef(EnsPGVIndividual gvi)
{
    if(!gvi)
	return NULL;
    
    gvi->Use++;
    
    return gvi;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Individual.
**
** @fdata [EnsPGVIndividual]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Individual object
**
** @argrule * Pgvi [EnsPGVIndividual*] Genetic Variation Individual
**                                     object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGVIndividualDel ***************************************************
**
** Default destructor for an Ensembl Genetic Variation Individual.
**
** @param [d] Pgvi [EnsPGVIndividual*] Ensembl Genetic Variation Individual
**                                     address
**
** @return [void]
** @@
******************************************************************************/

void ensGVIndividualDel(EnsPGVIndividual *Pgvi)
{
    EnsPGVIndividual pthis = NULL;
    
    /*
     ajDebug("ensGVIndividualDel\n"
	     "  *Pgvi %p\n",
	     *Pgvi);
     
     ensGVIndividualTrace(*Pgvi, 1);
     */
    
    if(!Pgvi)
        return;
    
    if(!*Pgvi)
        return;

    pthis = *Pgvi;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pgvi = NULL;
	
	return;
    }
    
    ensGVSampleDel(&pthis->GVSample);
    
    ensGVIndividualDel(&pthis->Father);
    
    ensGVIndividualDel(&pthis->Mother);
    
    ajStrDel(&pthis->Description);
    
    AJFREE(pthis);

    *Pgvi = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Individual object.
**
** @fdata [EnsPGVIndividual]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Individual attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Individual Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetGVSample Return the Ensembl Genetic Variation Sample
** @nam4rule GetFather Return the father Ensembl Genetic Variation Individual
** @nam4rule GetMother Return the mother Ensembl Genetic Variation Individual
** @nam4rule GetGender Return the gender
** @nam4rule GetType Return the type
**
** @argrule * gvi [const EnsPGVIndividual] Genetic Variation Individual
**
** @valrule Adaptor [EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                            Individual Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule GVSample [EnsPGVSample] Ensembl Genetic Variation Sample
** @valrule Father [EnsPGVIndividual] Ensembl Genetic Variation Individual
** @valrule Mother [EnsPGVIndividual] Ensembl Genetic Variation Individual
** @valrule Gender [AjEnum] Gender
** @valrule Type [AjEnum] Type
** @valrule Description [AjPStr] Description
**
** @fcategory use
******************************************************************************/




/* @func ensGVIndividualGetAdaptor ********************************************
**
** Get the Ensembl Genetic Variation Individual Adaptor element of an
** Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @return [EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                   Individual Adaptor
** @@
******************************************************************************/

EnsPGVIndividualAdaptor ensGVIndividualGetAdaptor(const EnsPGVIndividual gvi)
{
    if(!gvi)
        return NULL;
    
    return gvi->Adaptor;
}




/* @func ensGVIndividualGetIdentifier *****************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGVIndividualGetIdentifier(const EnsPGVIndividual gvi)
{
    if(!gvi)
        return 0;
    
    return gvi->Identifier;
}




/* @func ensGVIndividualGetGVSample *******************************************
**
** Get the Ensembl Genetic Variation Sample element of an
** Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @return [EnsPGVSample] Ensembl Genetic Variation Sample
** @@
******************************************************************************/

EnsPGVSample ensGVIndividualGetGVSample(const EnsPGVIndividual gvi)
{
    if(!gvi)
        return NULL;
    
    return gvi->GVSample;
}




/* @func ensGVIndividualGetFather *********************************************
**
** Get the father Ensembl Genetic Variation Individual element of an
** Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @return [EnsPGVIndividual] Ensembl Genetic Variation Individual
** @@
******************************************************************************/

EnsPGVIndividual ensGVIndividualGetFather(const EnsPGVIndividual gvi)
{
    if(!gvi)
        return NULL;
    
    return gvi->Father;
}




/* @func ensGVIndividualGetMother *********************************************
**
** Get the mother Ensembl Genetic Variation Individual element of an
** Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @return [EnsPGVIndividual] Ensembl Genetic Variation Individual
** @@
******************************************************************************/

EnsPGVIndividual ensGVIndividualGetMother(const EnsPGVIndividual gvi)
{
    if(!gvi)
        return NULL;
    
    return gvi->Mother;
}




/* @func ensGVIndividualGetGender *********************************************
**
** Get the gender element of an Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @return [AjEnum] Gender or ensEGVIndividualGenderNULL
** @@
******************************************************************************/

AjEnum ensGVIndividualGetGender(const EnsPGVIndividual gvi)
{
    if(!gvi)
        return ensEGVIndividualGenderNULL;
    
    return gvi->Gender;
}




/* @func ensGVIndividualGetType ***********************************************
**
** Get the type element of an Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @return [AjEnum] Type or ensEGVIndividualTypeNULL
** @@
******************************************************************************/

AjEnum ensGVIndividualGetType(const EnsPGVIndividual gvi)
{
    if(!gvi)
        return ensEGVIndividualTypeNULL;
    
    return gvi->Type;
}




/* @func ensGVIndividualGetDescription ****************************************
**
** Get the description element of an Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @return [AjPStr] Description
** @@
******************************************************************************/

AjPStr ensGVIndividualGetDescription(const EnsPGVIndividual gvi)
{
    if(!gvi)
        return NULL;
    
    return gvi->Description;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Individual object.
**
** @fdata [EnsPGVIndividual]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Population
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Population Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetName Set the name
** @nam4rule SetSample Set the Ensembl Genetic Variation Sample
** @nam4rule SetFather Set the Ensembl Genetic Variation Individual
** @nam4rule SetMother Set the Ensembl Genetic Variation Individual
** @nam4rule SetGender Set the gender
** @nam4rule SetType Set the type
** @nam4rule SetDescription Set the description
**
** @argrule * gvi [EnsPGVIndividual] Ensembl Genetic Variation Individual
**                                   object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGVIndividualSetAdaptor ********************************************
**
** Set the Ensembl Genetic Variation Individual Adaptor element of an
** Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGVIndividual] Ensembl Genetic Variation Individual
** @param [r] gvia [EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                           Individual Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualSetAdaptor(EnsPGVIndividual gvi,
                                 EnsPGVIndividualAdaptor gvia)
{
    if(!gvi)
        return ajFalse;
    
    gvi->Adaptor = gvia;
    
    return ajTrue;
}




/* @func ensGVIndividualSetIdentifier *****************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGVIndividual] Ensembl Genetic Variation Individual
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualSetIdentifier(EnsPGVIndividual gvi, ajuint identifier)
{
    if(!gvi)
        return ajFalse;
    
    gvi->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGVIndividualSetGVSample *******************************************
**
** Set the Ensembl Genetic Variation Sample element of an
** Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGVPopulation] Ensembl Genetic Variation Individual
** @param [u] gvs [EnsPGVSample] Ensembl Genetic Variation Sample
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualSetGVSample(EnsPGVIndividual gvi, EnsPGVSample gvs)
{
    if(!gvi)
        return ajFalse;
    
    ensGVSampleDel(&(gvi->GVSample));
    
    gvi->GVSample = ensGVSampleNewRef(gvs);
    
    return ajTrue;
}




/* @func ensGVIndividualSetFather *********************************************
**
** Set the father Ensembl Genetic Variation Individual element of an
** Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGVPopulation] Ensembl Genetic Variation Individual
** @param [u] father [EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualSetFather(EnsPGVIndividual gvi, EnsPGVIndividual father)
{
    if(!gvi)
        return ajFalse;
    
    ensGVIndividualDel(&(gvi->Father));
    
    gvi->Father = ensGVIndividualNewRef(father);
    
    return ajTrue;
}




/* @func ensGVIndividualSetMother *********************************************
**
** Set the mother Ensembl Genetic Variation Individual element of an
** Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGVPopulation] Ensembl Genetic Variation Individual
** @param [u] mother [EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualSetMother(EnsPGVIndividual gvi, EnsPGVIndividual mother)
{
    if(!gvi)
        return ajFalse;
    
    ensGVIndividualDel(&(gvi->Mother));
    
    gvi->Mother = ensGVIndividualNewRef(mother);
    
    return ajTrue;
}




/* @func ensGVIndividualSetGender *********************************************
**
** Set the gender element of an Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGVPopulation] Ensembl Genetic Variation Individual
** @param [r] gender [AjEnum] Gender
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualSetGender(EnsPGVIndividual gvi, AjEnum gender)
{
    if(!gvi)
        return ajFalse;
    
    gvi->Gender = gender;
    
    return ajTrue;
}




/* @func ensGVIndividualSetType ***********************************************
**
** Set the type element of an Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGVPopulation] Ensembl Genetic Variation Individual
** @param [r] type [AjEnum] Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualSetType(EnsPGVIndividual gvi, AjEnum type)
{
    if(!gvi)
        return ajFalse;
    
    gvi->Type = type;
    
    return ajTrue;
}




/* @func ensGVIndividualSetDescription ****************************************
**
** Set the description element of an Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGVPopulation] Ensembl Genetic Variation Individual
** @param [u] description [AjPStr] Description
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualSetDescription(EnsPGVIndividual gvi, AjPStr description)
{
    if(!gvi)
        return ajFalse;
    
    ajStrDel(&gvi->Description);
    
    if(description)
	gvi->Description = ajStrNewRef(description);
    
    return ajTrue;
}




/* @func ensGVIndividualGenderFromStr *****************************************
**
** Convert an AJAX String into an Ensembl Genetic Variaton Individual
** gender element.
**
** @param [r] gender [const AjPStr] Gender string
**
** @return [AjEnum] Ensembl Genetic Variation Individual gender element or
**                  ensEGVIndividualGenderNULL
** @@
******************************************************************************/

AjEnum ensGVIndividualGenderFromStr(const AjPStr gender)
{
    register ajint i = 0;
    
    AjEnum egender = ensEGVIndividualGenderNULL;
    
    for(i = 1; gvIndividualGender[i]; i++)
	if(ajStrMatchC(gender, gvIndividualGender[i]))
	    egender = i;
    
    if(!egender)
	ajDebug("ensGVIndividualGenderFromStr encountered "
		"unexpected string '%S'.\n", gender);
    
    return egender;
}




/* @func ensGVIndividualTypeFromStr *******************************************
**
** Convert an AJAX String into an Ensembl Genetic Variaton Individual
** type element.
**
** @param [r] type [const AjPStr] Type string
**
** @return [AjEnum] Ensembl Genetic Variation Individual type element or
**                  ensEGVIndividualTypeNULL
** @@
******************************************************************************/

AjEnum ensGVIndividualTypeFromStr(const AjPStr type)
{
    register ajint i = 0;
    
    AjEnum etype = ensEGVIndividualTypeNULL;
    
    for(i = 1; gvIndividualType[i]; i++)
	if(ajStrMatchC(type, gvIndividualType[i]))
	    etype = i;
    
    if(!etype)
	ajDebug("ensGVIndividualTypeFromStr encountered "
		"unexpected string '%S'.\n", type);
    
    return etype;
}




/* @func ensGVIndividualGenderToChar ******************************************
**
** Convert an Ensembl Genetic Variation Individual gender element into a
** C-type (char*) string.
**
** @param [r] gender [const AjEnum] Ensembl Genetic Variation Individual
**                                  gender enumerator
**
** @return [const char*] Ensembl Genetic Variation Individual gender
**                       C-type (char*) string
** @@
******************************************************************************/

const char* ensGVIndividualGenderToChar(const AjEnum gender)
{
    register ajint i = 0;
    
    if(!gender)
	return NULL;
    
    for(i = 1; gvIndividualGender[i] && (i < gender); i++);
    
    if(!gvIndividualGender[i])
	ajDebug("ensGVIndividualGenderToChar encountered an "
		"out of boundary error on gender %d.\n", gender);
    
    return gvIndividualGender[i];
}




/* @func ensGVIndividualTypeToChar ********************************************
**
** Convert an Ensembl Genetic Variation Individual type element into a
** C-type (char*) string.
**
** @param [r] type [const AjEnum] Ensembl Genetic Variation Individual
**                                type enumerator
**
** @return [const char*] Ensembl Genetic Variation Individual type
**                       C-type (char*) string
** @@
******************************************************************************/

const char* ensGVIndividualTypeToChar(const AjEnum type)
{
    register ajint i = 0;
    
    if(!type)
	return NULL;
    
    for(i = 1; gvIndividualType[i] && (i < type); i++);
    
    if(!gvIndividualType[i])
	ajDebug("ensGVIndividualTypeToChar encountered an "
		"out of boundary error on type %d.\n", type);
    
    return gvIndividualType[i];
}




/* @func ensGVIndividualGetMemSize ********************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGVIndividual] Ensembl Genetic Variation Individual
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGVIndividualGetMemSize(const EnsPGVIndividual gvi)
{
    ajuint size = 0;
    
    if(!gvi)
	return 0;
    
    size += (ajuint) sizeof (EnsOGVIndividual);
    
    size += ensGVSampleGetMemSize(gvi->GVSample);
    
    size += ensGVIndividualGetMemSize(gvi->Father);
    
    size += ensGVIndividualGetMemSize(gvi->Mother);
    
    if(gvi->Description)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvi->Description);
    }
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Genetic Variation Individual object.
**
** @fdata [EnsPGVIndividual]
** @nam3rule Trace Report Ensembl Genetic Variation Individual elements to
**                 debug file
**
** @argrule Trace gvi [const EnsPGVIndividual] Ensembl Genetic Variation
**                                             Individual
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @valrule * [void]
**
** @fcategory misc
******************************************************************************/




/* @func ensGVIndividualTrace *************************************************
**
** Trace an Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGVIndividual] Ensembl Genetic Variation Individual
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualTrace(const EnsPGVIndividual gvi, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!gvi)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensGVIndividualTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Sample %p\n"
	    "%S  Father %p\n"
	    "%S  Mother %p\n"
	    "%S  Gender '%s'\n"
	    "%S  Type '%s'\n"
	    "%S  Description '%S'\n",
	    indent, gvi,
	    indent, gvi->Use,
	    indent, gvi->Identifier,
	    indent, gvi->Adaptor,
	    indent, gvi->GVSample,
	    indent, gvi->Father,
	    indent, gvi->Mother,
	    indent, ensGVIndividualGenderToChar(gvi->Gender),
	    indent, ensGVIndividualTypeToChar(gvi->Type),
	    indent, gvi->Description);
    
    ensGVSampleTrace(gvi->GVSample, level + 1);
    
    ensGVIndividualTrace(gvi->Father, level + 1);
    ensGVIndividualTrace(gvi->Mother, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @datasection [EnsPGVIndividualAdaptor] Genetic Variation Individual Adaptor
**
** Functions for manipulating Ensembl Genetic Variation Individual Adaptor
** objects
**
** @nam2rule GVIndividualAdaptor
**
******************************************************************************/

static const char *gvIndividualAdaptorTables[] =
{
    "individual",
    "sample",
    "individual_type",
    NULL
};




static const char *gvIndividualAdaptorColumns[] =
{
    "sample.sample_id",
    "sample.name",
    "sample.size",
    "sample.description",
    "individual.gender",
    "individual.father_individual_sample_id",
    "individual.mother_individual_sample_id",
    "individual_type.name",
    "individual_type.description",
    NULL
};




static EnsOBaseAdaptorLeftJoin gvIndividualAdaptorLeftJoin[] =
{
    {NULL, NULL}
};




static const char *gvIndividualAdaptorDefaultCondition =
"sample.sample_id = individual.sample_id AND "
"individual.individual_type_id = individual_type.individual_type_id";




static const char *gvIndividualAdaptorFinalCondition = NULL;




/* @funcstatic gvIndividualAdaptorFetchAllBySQL *******************************
**
** Fetch all Ensembl Genetic Variation Individual objects via an SQL statement.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] gvis [AjPList] AJAX List of Ensembl Genetic Variation Individual
**                           objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvIndividualAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblyMapper am,
                                               EnsPSlice slice,
                                               AjPList gvis)
{
    ajuint sidentifier = 0;
    ajuint fatherid    = 0;
    ajuint motherid    = 0;
    ajuint ssize       = 0;
    
    AjEnum egender = ensEGVIndividualGenderNULL;
    AjEnum etype   = ensEGVIndividualTypeNULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr sname        = NULL;
    AjPStr sdescription = NULL;
    AjPStr gender       = NULL;
    AjPStr tname        = NULL;
    AjPStr tdescription = NULL;
    
    EnsPGVIndividual gvi         = NULL;
    EnsPGVIndividual father      = NULL;
    EnsPGVIndividual mother      = NULL;
    EnsPGVIndividualAdaptor gvia = NULL;
    
    EnsPGVSample gvs         = NULL;
    EnsPGVSampleAdaptor gvsa = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    (void) am;
    
    (void) slice;
    
    if(!gvis)
	return ajFalse;
    
    gvia = ensRegistryGetGVIndividualAdaptor(dba);
    
    gvsa = ensGVIndividualAdaptorGetAdaptor(gvia);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	sidentifier  = 0;
	sname        = ajStrNew();
	ssize        = 0;
	sdescription = ajStrNew();
	gender       = ajStrNew();
	fatherid     = 0;
	motherid     = 0;
	tname        = ajStrNew();
	tdescription = ajStrNew();
	
	sqlr = ajSqlRowIterGet(sqli);
	
	ajSqlColumnToUint(sqlr, &sidentifier);
        ajSqlColumnToStr(sqlr, &sname);
	ajSqlColumnToUint(sqlr, &ssize);
        ajSqlColumnToStr(sqlr, &sdescription);
	ajSqlColumnToStr(sqlr, &gender);
	ajSqlColumnToUint(sqlr, &fatherid);
	ajSqlColumnToUint(sqlr, &motherid);
        ajSqlColumnToStr(sqlr, &tname);
        ajSqlColumnToStr(sqlr, &tdescription);
	
	egender = ensGVIndividualGenderFromStr(gender);
	etype = ensGVIndividualTypeFromStr(tname);
	
	gvs = ensGVSampleNew(gvsa,
			     sidentifier,
			     sname,
			     sdescription,
			     ssize);
	
	ensGVIndividualAdaptorFetchByIdentifier(gvia, fatherid, &father);
	
	ensGVIndividualAdaptorFetchByIdentifier(gvia, motherid, &mother);
	
	gvi = ensGVIndividualNew(gvia,
				 sidentifier,
				 gvs,
				 father,
				 mother,
				 egender,
				 etype,
				 tdescription);
	
	ajListPushAppend(gvis, (void *) gvi);
	
	ensGVSampleDel(&gvs);
	
	ensGVIndividualDel(&father);
	ensGVIndividualDel(&mother);
	
	ajStrDel(&sname);
	ajStrDel(&sdescription);
	ajStrDel(&gender);
	ajStrDel(&tname);
	ajStrDel(&tdescription);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    return ajTrue;
}




/* @func ensGVIndividualAdaptorNew ********************************************
**
** Default constructor for an Ensembl Genetic Variation Individual Adaptor.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::IndividualAdaptor::new
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                   Individual Adaptor or NULL
** @@
******************************************************************************/

EnsPGVIndividualAdaptor ensGVIndividualAdaptorNew(EnsPDatabaseAdaptor dba)
{
    EnsPGVIndividualAdaptor gvia = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(gvia);
    
    gvia->Adaptor = ensGVSampleAdaptorNew(dba,
					  gvIndividualAdaptorTables,
					  gvIndividualAdaptorColumns,
					  gvIndividualAdaptorLeftJoin,
					  gvIndividualAdaptorDefaultCondition,
					  gvIndividualAdaptorFinalCondition,
					  gvIndividualAdaptorFetchAllBySQL);
    
    if(!gvia->Adaptor)
	AJFREE(gvia);
    
    return gvia;
}




/* @func ensGVIndividualAdaptorDel ********************************************
**
** Default destructor for an Ensembl Gentic Variation Individual Adaptor.
**
** @param [d] Pgvia [EnsPGVIndividualAdaptor*] Ensembl Genetic Variation
**                                             Individual Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGVIndividualAdaptorDel(EnsPGVIndividualAdaptor *Pgvi)
{
    EnsPGVIndividualAdaptor pthis = NULL;
    
    if(!Pgvi)
	return;
    
    if(!*Pgvi)
	return;

    pthis = *Pgvi;
    
    ensGVSampleAdaptorDel(&pthis->Adaptor);
    
    AJFREE(pthis);

    *Pgvi = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Individual Adaptor object.
**
** @fdata [EnsPGVIndividualAdaptor]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Individual Adaptor attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Sample Adaptor
**
** @argrule * gvia [const EnsPGVIndividualAdaptor] Genetic Variation
**                                                 Individual Adaptor
**
** @valrule Adaptor [EnsPGVSampleAdaptor] Ensembl Genetic Variation
**                                        Sample Adaptor
**
** @fcategory use
******************************************************************************/




/* @func ensGVIndividualAdaptorGetAdaptor *************************************
**
** Get the Ensembl Genetic Variation Sample Adaptor element of an
** Ensembl Genetic Variation Individual Adaptor.
**
** @param [r] gvia [const EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                                    Individual Adaptor
**
** @return [EnsPGVSampleAdaptor] Ensembl Genetic Variation Sample Adaptor
** @@
******************************************************************************/

EnsPGVSampleAdaptor ensGVIndividualAdaptorGetAdaptor(
    const EnsPGVIndividualAdaptor gvia)
{
    if(!gvia)
	return NULL;
    
    return gvia->Adaptor;
}




/* @func ensGVIndividualAdaptorFetchByIdentifier ******************************
**
** Fetch an Ensembl Genetic Variation Individual via its
** SQL database-internal identifier.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Individual.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::IndividualAdaptor::fetch_by_dbID
** @param [r] gvia [const EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [r] identifier [ajuint] SQL database-internal Individual identifier
** @param [wP] Pgvi [EnsPGVIndividual*] Ensembl Genetic Variation
**                                      Individual address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualAdaptorFetchByIdentifier(EnsPGVIndividualAdaptor gvia,
                                               ajuint identifier,
                                               EnsPGVIndividual *Pgvi)
{
    EnsPBaseAdaptor ba = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pgvi)
	return ajFalse;
    
    ba = ensGVSampleAdaptorGetAdaptor(gvia->Adaptor);
    
    *Pgvi = (EnsPGVIndividual)
	ensBaseAdaptorFetchByIdentifier(ba, identifier);
    
    return ajTrue;
}




/* @func ensGVIndividualAdaptorFetchAllByName *********************************
**
** Fetch all Ensembl Genetic Variation Individuals by an
** Ensembl Genetic Variation Sample name.
**
** Individual names may be non-unique, therefore the function returns an
** AJAX List of Ensembl Genetic Variation Individuals.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::IndividualAdaptor::fetch_all_by_name
** @param [r] gvia [const EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [r] name [const AjPStr] Ensembl Genetic Variation Sample name
** @param [u] gvis [AjPList] AJAX List of Ensembl Genetic Variation Individuals
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualAdaptorFetchAllByName(EnsPGVIndividualAdaptor gvia,
                                            const AjPStr name,
                                            AjPList gvis)
{
    char *txtname = NULL;
    
    AjPStr constraint = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    if(!gvis)
	return ajFalse;
    
    ba = ensGVSampleAdaptorGetAdaptor(gvia->Adaptor);
    
    ensBaseAdaptorEscapeCS(ba, &txtname, name);
    
    constraint = ajFmtStr("sample.name = '%s'", txtname);
    
    ajCharDel(&txtname);
    
    ensBaseAdaptorGenericFetch(ba, constraint, NULL, NULL, gvis);
    
    /* FIXME: Should we warn here? */
    if(ajListGetLength(gvis) == 0)
	ajDebug("ensGVIndividualAdaptorFetchByName could not get an "
		"Ensembl Genetic Variation Individual "
		"for name '%S'.\n",
		name);
    
    if(ajListGetLength(gvis) > 1)
	ajDebug("ensGVIndividualAdaptorFetchByName got more than one "
		"Ensembl Genetic Variation Individual "
		"for name '%S'.\n",
		name);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensGVIndividualAdaptorFetchAllBySynonym ******************************
**
** Fetch all Ensembl Genetic Variation Individuals by an
** Ensembl Genetic Variation Sample synonym.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::IndividualAdaptor::
**     fetch_individual_by_synonym
** @param [r] gvia [const EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [r] synonym [const AjPStr] Ensembl Genetic Variation Sample synonym
** @param [u] gvis [AjPList] AJAX List of Ensembl Genetic Variation Individuals
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualAdaptorFetchAllBySynonym(EnsPGVIndividualAdaptor gvia,
                                               const AjPStr synonym,
                                               AjPList gvis)
{
    ajuint *Pidentifier = 0;
    
    AjPList idlist = NULL;
    
    EnsPGVIndividual gvi = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!synonym)
	return ajFalse;
    
    if(!gvis)
	return ajFalse;
    
    idlist = ajListNew();
    
    ensGVSampleAdaptorFetchAllIdentifiersBySynonym(gvia->Adaptor,
						   synonym,
						   NULL,
						   idlist);
    
    while(ajListPop(idlist, (void **) &Pidentifier))
    {
	ensGVIndividualAdaptorFetchByIdentifier(gvia,
						*Pidentifier,
						&gvi);
	
	ajListPushAppend(gvis, (void *) gvi);
	
	AJFREE(Pidentifier);
    }
    
    ajListFree(&idlist);
    
    return ajTrue;
}




/* @func ensGVIndividualAdaptorFetchAllByPopulation ***************************
**
** Fetch all Ensembl Genetic Variation Individuals by an
** Ensembl Genetic Variation Population.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::IndividualAdaptor::
**     fetch_all_by_Population
** @param [r] gvia [const EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [r] gvp [const EnsPGVPopulation] Ensembl Genetic Variation Population
** @param [u] gvis [AjPList] AJAX List of Ensembl Genetic Variation Individuals
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualAdaptorFetchAllByPopulation(EnsPGVIndividualAdaptor gvia,
                                                  const EnsPGVPopulation gvp,
                                                  AjPList gvis)
{
    AjPStr statement = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!gvp)
	return ajFalse;
    
    if(!gvis)
	return ajFalse;
    
    if(!gvp->Identifier)
    {
	ajDebug("ensGVIndividualAdaptorFetchAllByPopulation "
		"cannot fetch Population for Individual "
		"without identifier.\n");
	
	return ajFalse;
    }
    
    /*
    ** This function does not use the Ensembl Base Adaptor functionality
    ** because an additional 'population_structure' table is required.
    */
    
    statement = ajFmtStr("SELECT "
			 "individual.sample_id, "
			 "sample.name, "
			 "sample.description, "
			 "individual.gender, "
			 "individual.father_individual_sample_id, "
			 "individual.mother_individual_sample_id, "
			 "individual_type.name, "
			 "individual_type.description "
			 "FROM "
			 "individual, "
			 "individual_population, "
			 "sample, "
			 "individual_type "
			 "WHERE "
			 "individual.sample_id = "
			 "individual_population.individual_sample_id "
			 "AND "
			 "individual.sample_id = sample.sample_id "
			 "AND "
			 "individual.individual_type_id = "
			 "individual_type.individual_type_id "
			 "AND "
			 "individual_population.population_sample_id = %u",
			 gvp->Identifier);
    
    ba = ensGVSampleAdaptorGetAdaptor(gvia->Adaptor);
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(ba);
    
    gvIndividualAdaptorFetchAllBySQL(dba, statement, NULL, NULL, gvis);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensGVIndividualAdaptorFetchAllByParentIndividual *********************
**
** Fetch all Ensembl Genetic Variation Individuals by a parent
** Ensembl Genetic Variation Individual.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::IndividualAdaptor::
**     fetch_all_by_parent_Individual
** @param [r] gvia [const EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [r] parent [const EnsPGVIndividual] Ensembl Genetic Variation
**                                            Individual
** @param [u] gvis [AjPList] AJAX List of Ensembl Genetic Variation Individuals
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualAdaptorFetchAllByParentIndividual(
    EnsPGVIndividualAdaptor gvia,
    const EnsPGVIndividual parent,
    AjPList gvis)
{
    AjPStr constraint = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!parent)
	return ajFalse;
    
    if(!gvis)
	return ajFalse;
    
    if(!parent->Identifier)
    {
	ajDebug("ensGVIndividualAdaptorFetchAllByParentIndividual cannot get "
		"Individuals for parent Indeividual without identifier.\n");
	
	return ajFalse;
    }
    
    ba = ensGVSampleAdaptorGetAdaptor(gvia->Adaptor);
    
    if(parent->Gender == ensEGVIndividualGenderMale)
    {
	constraint = ajFmtStr("individual.father_individual_sample_id = %u",
			      parent->Identifier);
	
	ensBaseAdaptorGenericFetch(ba, constraint, NULL, NULL, gvis);
	
	ajStrDel(&constraint);
	
	return ajTrue;
    }
    else if(parent->Gender == ensEGVIndividualGenderFemale)
    {
	constraint = ajFmtStr("individual.mother_individual_sample_id = %u",
			      parent->Identifier);
	
	ensBaseAdaptorGenericFetch(ba, constraint, NULL, NULL, gvis);
	
	ajStrDel(&constraint);
	
	return ajTrue;
    }
    
    /* The gender is unknown, so assume this is the father. */
    
    constraint = ajFmtStr("individual.father_individual_sample_id = %u",
			  parent->Identifier);
    
    ensBaseAdaptorGenericFetch(ba, constraint, NULL, NULL, gvis);
    
    ajStrDel(&constraint);
    
    if(ajListGetLength(gvis))
	return ajTrue;
    
    /* No result for the father, so assume it is the mother. */
    
    constraint = ajFmtStr("individual.mother_individual_sample_id = %u",
			  parent->Identifier);
    
    ensBaseAdaptorGenericFetch(ba, constraint, NULL, NULL, gvis);
    
    ajStrDel(&constraint);
    
    /* Return regardless of a result. */
    
    return ajTrue;
}




/* @func ensGVIndividualAdaptorFetchAllStrains ********************************
**
** Fetch all Ensembl Genetic Variation Individuals by a parent
** Ensembl Genetic Variation Individual.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::IndividualAdaptor::fetch_all_strains
** @param [r] gvia [const EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [u] gvis [AjPList] AJAX List of Ensembl Genetic Variation Individuals
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualAdaptorFetchAllStrains(EnsPGVIndividualAdaptor gvia,
                                             AjPList gvis)
{
    AjPStr constraint = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!gvis)
	return ajFalse;
    
    ba = ensGVSampleAdaptorGetAdaptor(gvia->Adaptor);
    
    constraint = ajStrNewC("individual_type.name = 'fully_inbred'");
    
    ensBaseAdaptorGenericFetch(ba, constraint, NULL, NULL, gvis);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensGVIndividualAdaptorFetchReferenceStrainName ***********************
**
** Fetch the reference strain name.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::IndividualAdaptor::
**     get_reference_strain_name
** @param [r] gvia [const EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [w] Pname [AjPStr*] Reference strain name AJAX String address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualAdaptorFetchReferenceStrainName(
    EnsPGVIndividualAdaptor gvia,
    AjPStr *Pname)
{
    AjPList mis = NULL;
    
    AjPStr key = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPMetaInformation mi         = NULL;
    EnsPMetaInformationAdaptor mia = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!Pname)
	return ajFalse;
    
    ba = ensGVSampleAdaptorGetAdaptor(gvia->Adaptor);
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(ba);
    
    mia = ensRegistryGetMetaInformationAdaptor(dba);
    
    key = ajStrNewC("individual.reference_strain");
    
    mis = ajListNew();
    
    ensMetaInformationAdaptorFetchAllByKey(mia, key, mis);
    
    ajListPop(mis, (void **) &mi);
    
    if(mi)
    {
	ajStrAssignS(Pname, ensMetaInformationGetValue(mi));
	
	ensMetaInformationDel(&mi);
    }
    
    while(ajListPop(mis, (void **) &mi))
	ensMetaInformationDel(&mi);
    
    ajListFree(&mis);
    
    ajStrDel(&key);
    
    return ajTrue;
}




/* @func ensGVIndividualAdaptorFetchAllDefaultStrains *************************
**
** Fetch all default strain names.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::IndividualAdaptor::
**     get_default_strains
** @param [r] gvia [const EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [u] names [AjPList] AJAX List of default strain name AJAX Strings
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualAdaptorFetchAllDefaultStrains(
    EnsPGVIndividualAdaptor gvia,
    AjPList names)
{
    AjPList mis = NULL;
    
    AjPStr key = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPMetaInformation mi         = NULL;
    EnsPMetaInformationAdaptor mia = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!names)
	return ajFalse;
    
    ba = ensGVSampleAdaptorGetAdaptor(gvia->Adaptor);
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(ba);
    
    mia = ensRegistryGetMetaInformationAdaptor(dba);
    
    key = ajStrNewC("individual.default_strain");
    
    mis = ajListNew();
    
    ensMetaInformationAdaptorFetchAllByKey(mia, key, mis);
    
    while(ajListPop(mis, (void **) &mi))
    {
	ajListPushAppend(names,
			 (void *) ajStrNewS(ensMetaInformationGetValue(mi)));
	
	ensMetaInformationDel(&mi);
    }
    
    ajListFree(&mis);
    
    ajStrDel(&key);
    
    return ajTrue;
}




/* @func ensGVIndividualAdaptorFetchAllDisplayStrains *************************
**
** Fetch all display strain names.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::IndividualAdaptor::
**     get_display_strains
** @param [r] gvia [const EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [u] names [AjPList] AJAX List of display strain name AJAX Strings
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualAdaptorFetchAllDisplayStrains(
    EnsPGVIndividualAdaptor gvia,
    AjPList names)
{
    AjPList mis = NULL;
    
    AjPStr key = NULL;
    AjPStr name = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    EnsPMetaInformation mi         = NULL;
    EnsPMetaInformationAdaptor mia = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!names)
	return ajFalse;
    
    name = ajStrNew();
    
    /* Fetch the reference strain name. */
    
    ensGVIndividualAdaptorFetchReferenceStrainName(gvia, &name);
    
    ajListPushAppend(names, (void *) name);
    
    /* Fetch all default strain names. */
    
    ensGVIndividualAdaptorFetchAllDefaultStrains(gvia, names);
    
    /* Fetch all display strain names. */
    
    ba = ensGVSampleAdaptorGetAdaptor(gvia->Adaptor);
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(ba);
    
    mia = ensRegistryGetMetaInformationAdaptor(dba);
    
    key = ajStrNewC("individual.display_strain");
    
    mis = ajListNew();
    
    ensMetaInformationAdaptorFetchAllByKey(mia, key, mis);
    
    while(ajListPop(mis, (void **) &mi))
    {
	ajListPushAppend(names,
			 (void *) ajStrNewS(ensMetaInformationGetValue(mi)));
	
	ensMetaInformationDel(&mi);
    }
    
    ajListFree(&mis);
    
    ajStrDel(&key);
    
    return ajTrue;
}




/* @func ensGVIndividualAdaptorFetchAllStrainsWithCoverage ********************
**
** Fetch all Ensembl Genetic Variation Strain identifiers for which
** read coverage information is available.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::IndividualAdaptor::
**     fetch_all_strains_with_coverage
** @param [r] gvia [const EnsPGVIndividualAdaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [u] idlist [AjPList] AJAX List of Ensembl Genetic Variation Sample
**                             identifiers
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVIndividualAdaptorFetchAllStrainsWithCoverage(
    EnsPGVIndividualAdaptor gvia,
    AjPList idlist)
{
    ajuint *Pidentifier = 0;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr statement = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!idlist)
	return ajFalse;
    
    ba = ensGVSampleAdaptorGetAdaptor(gvia->Adaptor);
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(ba);
    
    statement = ajStrNewC("SELECT "
			  "DISTINCT "
			  "read_coverage.sample_id "
			  "FROM "
			  "read_coverage");
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	AJNEW0(Pidentifier);
	
	sqlr = ajSqlRowIterGet(sqli);
	
	ajSqlColumnToUint(sqlr, Pidentifier);
	
	ajListPushAppend(idlist, (void *) Pidentifier);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    return ajTrue;
}




/* @datasection [EnsPGVPopulation] Genetic Variation Population ***************
**
** Functions for manipulating Ensembl Genetic Variation Population objects
**
** @nam2rule GVPopulation
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Population by
** pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Population. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGVPopulation]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGVPopulation] Ensembl Genetic Variation Population
** @argrule Ref object [EnsPGVPopulation] Ensembl Genetic Variation Population
**
** @valrule * [EnsPGVPopulation] Ensembl Genetic Variation Population
**
** @fcategory new
******************************************************************************/




/* @func ensGVPopulationNew ***************************************************
**
** Default constructor for an Ensembl Genetic Variation Population.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvpa [EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                              Population Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Sample::new
** @param [u] gvs [EnsPGVSample] Ensembl Genetic Variation Sample
** @cc Bio::EnsEMBL::Variation::Population::new
** @param [r] subpopulations [AjPList] AJAX List of Ensembl Genetic
**                                     Variation (Sub-) Populations
**
** @return [EnsPGVPopulation] Ensembl Genetic Variation Population
** @@
******************************************************************************/

EnsPGVPopulation ensGVPopulationNew(EnsPGVPopulationAdaptor gvpa,
                                    ajuint identifier,
                                    EnsPGVSample gvs,
                                    AjPList subpopulations)
{
    AjIList iter = NULL;
    
    EnsPGVPopulation gvp    = NULL;
    EnsPGVPopulation subgvp = NULL;
    
    if(!gvs)
	return NULL;
    
    AJNEW0(gvp);
    
    gvp->Use = 1;
    
    gvp->Identifier = identifier;
    
    gvp->Adaptor = gvpa;
    
    gvp->GVSample = ensGVSampleNewRef(gvs);
    
    gvp->SubPopulations = ajListNew();
    
    iter = ajListIterNew(subpopulations);
    
    while(!ajListIterDone(iter))
    {
	subgvp = (EnsPGVPopulation) ajListIterGet(iter);
	
	ajListPushAppend(gvp->SubPopulations,
			 (void *) ensGVPopulationNewRef(subgvp));
    }
    
    ajListIterDel(&iter);
    
    return gvp;
}




/* @func ensGVPopulationNewObj ************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGVPopulation] Ensembl Genetic Variation
**                                            Population
**
** @return [EnsPGVPopulation] Ensembl Genetic Variation Population or NULL
** @@
******************************************************************************/

EnsPGVPopulation ensGVPopulationNewObj(const EnsPGVPopulation object)
{
    AjIList iter = NULL;
    
    EnsPGVPopulation gvp    = NULL;
    EnsPGVPopulation subgvp = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(gvp);
    
    gvp->Use = 1;
    
    gvp->Identifier = object->Identifier;
    
    gvp->Adaptor = object->Adaptor;
    
    gvp->GVSample = ensGVSampleNewRef(object->GVSample);
    
    /* Copy the AJAX List of Ensembl Genetic Variation (Sub-) Populations. */
    
    gvp->SubPopulations = ajListNew();
    
    iter = ajListIterNew(object->SubPopulations);
    
    while(!ajListIterDone(iter))
    {
	subgvp = (EnsPGVPopulation) ajListIterGet(iter);
	
	ajListPushAppend(gvp->SubPopulations,
			 (void *) ensGVPopulationNewRef(subgvp));
    }
    
    ajListIterDel(&iter);
    
    return gvp;
}




/* @func ensGVPopulationNewRef ************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvp [EnsPGVPopulation] Ensembl Genetic Variation Population
**
** @return [EnsPGVPopulation] Ensembl Genetic Variation Population
** @@
******************************************************************************/

EnsPGVPopulation ensGVPopulationNewRef(EnsPGVPopulation gvp)
{
    if(!gvp)
	return NULL;
    
    gvp->Use++;
    
    return gvp;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Population.
**
** @fdata [EnsPGVPopulation]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Population object
**
** @argrule * Pgvp [EnsPGVPopulation*] Genetic Variation Population
**                                     object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGVPopulationDel ***************************************************
**
** Default destructor for an Ensembl Genetic Variation Population.
**
** @param [d] Pgvp [EnsPGVPopulation*] Ensembl Genetic Variation Population
**                                     address
**
** @return [void]
** @@
******************************************************************************/

void ensGVPopulationDel(EnsPGVPopulation *Pgvp)
{
    EnsPGVPopulation subgvp = NULL;
    EnsPGVPopulation pthis = NULL;
    
    
    /*
     ajDebug("ensGVPopulationDel\n"
	     "  *Pgvp %p\n",
	     *Pgvp);
     
     ensGVPopulationTrace(*Pgvp, 1);
     */
    
    if(!Pgvp)
        return;
    
    if(!*Pgvp)
        return;

    pthis = *Pgvp;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pgvp = NULL;
	
	return;
    }
    
    ensGVSampleDel(&pthis->GVSample);
    
    while(ajListPop(pthis->SubPopulations, (void **) &subgvp))
	ensGVPopulationDel(&subgvp);
    
    ajListFree(&pthis->SubPopulations);
    
    AJFREE(pthis);

    *Pgvp = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Population object.
**
** @fdata [EnsPGVPopulation]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Population attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Population Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetGVSample Return the Ensembl Genetic Variation Sample
** @nam4rule GetSubPopulation Return the AJAX List of Ensembl Genetic
**                            Variation (sub-) Populations
**
** @argrule * population [const EnsPGVPopulation] Genetic Variation Population
**
** @valrule Adaptor [EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                            Population Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule GVSample [EnsPGVSample] Ensembl Genetic Variation Sample
** @valrule SubPopulations [AjPList] AJAX List of Ensembl Genetic Variation
**                                  (Sub-) Populations
**
** @fcategory use
******************************************************************************/




/* @func ensGVPopulationGetAdaptor ********************************************
**
** Get the Ensembl Genetic Variation Population Adaptor element of an
** Ensembl Genetic Variation Population.
**
** @param [r] gvp [const EnsPGVPopulation] Ensembl Genetic Variation Population
**
** @return [EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                   Population Adaptor
** @@
******************************************************************************/

EnsPGVPopulationAdaptor ensGVPopulationGetAdaptor(const EnsPGVPopulation gvp)
{
    if(!gvp)
        return NULL;
    
    return gvp->Adaptor;
}




/* @func ensGVPopulationGetIdentifier *****************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Population.
**
** @param [r] gvp [const EnsPGVPopulation] Ensembl Genetic Variation Population
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGVPopulationGetIdentifier(const EnsPGVPopulation gvp)
{
    if(!gvp)
        return 0;
    
    return gvp->Identifier;
}




/* @func ensGVPopulationGetGVSample *******************************************
**
** Get the Ensembl Genetic Variation Sample element of an
** Ensembl Genetic Variation Population.
**
** @param [r] gvp [const EnsPGVPopulation] Ensembl Genetic Variation Population
**
** @return [EnsPGVSample] Ensembl Genetic Variation Sample
** @@
******************************************************************************/

EnsPGVSample ensGVPopulationGetGVSample(const EnsPGVPopulation gvp)
{
    if(!gvp)
        return NULL;
    
    return gvp->GVSample;
}




/* @func ensGVPopulationGetSubPopulations *************************************
**
** Get the AJAX List of Ensembl Genetic Variation (Sub-) Populations of an
** Ensembl Genetic Variation Population.
**
** @param [r] gvp [const EnsPGVPopulation] Ensembl Genetic Variation Population
**
** @return [const AjPList] AJAX List of Ensembl Genetic Variation Populations
** @@
******************************************************************************/

const AjPList ensGVPopulationGetSubPopulations(const EnsPGVPopulation gvp)
{
    if(!gvp)
        return NULL;
    
    return gvp->SubPopulations;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Population object.
**
** @fdata [EnsPGVPopulation]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Population
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Population Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetName Set the name
** @nam4rule SetSample Set the Ensembl Genetic Variation Sample
**
** @argrule * gvp [EnsPGVPopulation] Ensembl Genetic Variation Population
**                                   object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGVPopulationSetAdaptor ********************************************
**
** Set the Ensembl Genetic Variation Population Adaptor element of an
** Ensembl Genetic Variation Population.
**
** @param [u] gvp [EnsPGVPopulation] Ensembl Genetic Variation Population
** @param [r] gvpa [EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                           Population Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVPopulationSetAdaptor(EnsPGVPopulation gvp,
                                 EnsPGVPopulationAdaptor gvpa)
{
    if(!gvp)
        return ajFalse;
    
    gvp->Adaptor = gvpa;
    
    return ajTrue;
}




/* @func ensGVPopulationSetIdentifier *****************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Population.
**
** @param [u] gvp [EnsPGVPopulation] Ensembl Genetic Variation Population
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVPopulationSetIdentifier(EnsPGVPopulation gvp, ajuint identifier)
{
    if(!gvp)
        return ajFalse;
    
    gvp->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGVPopulationSetGVSample *******************************************
**
** Set the Ensembl Genetic Variation Sample element of an
** Ensembl Genetic Variation Population.
**
** @param [u] gvp [EnsPGVPopulation] Ensembl Genetic Variation Population
** @param [u] gvs [EnsPGVSample] Ensembl Genetic Variation Sample
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVPopulationSetGVSample(EnsPGVPopulation gvp, EnsPGVSample gvs)
{
    if(!gvp)
        return ajFalse;
    
    ensGVSampleDel(&gvp->GVSample);
    
    gvp->GVSample = ensGVSampleNewRef(gvs);
    
    return ajTrue;
}




/* @func ensGVPopulationAddSubPopulation **************************************
**
** Add an Ensembl Genetic Variation (Sub-) Population to an
** Ensembl Genetic Variation Population.
**
** @cc Bio::EnsEMBL::Variation::Population::add_sub_Population
** @param [u] gvp [EnsPGVPopulation] Ensembl Genetic Variation Population
** @param [u] subgvp [EnsPGVPopulation] Ensembl Genetic Variation Population
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVPopulationAddSubPopulation(EnsPGVPopulation gvp,
                                       EnsPGVPopulation subgvp)
{
    if(!gvp)
	return ajFalse;
    
    if(!subgvp)
	return ajFalse;
    
    if(!gvp->SubPopulations)
	gvp->SubPopulations = ajListNew();
    
    ajListPushAppend(gvp->SubPopulations,
		     (void *) ensGVPopulationNewRef(subgvp));
    
    return ajTrue;
}




/* @func ensGVPopulationGetMemSize ********************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Population.
**
** @param [r] gvp [const EnsPGVPopulation] Ensembl Genetic Variation Population
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGVPopulationGetMemSize(const EnsPGVPopulation gvp)
{
    ajuint size = 0;
    
    AjIList iter = NULL;
    
    EnsPGVPopulation subgvp = NULL;
    
    if(!gvp)
	return 0;
    
    size += (ajuint) sizeof (EnsOGVPopulation);
    
    size += ensGVSampleGetMemSize(gvp->GVSample);
    
    /*
    ** Summarise the AJAX List of Ensembl Genetic Variation
    ** (Sub-) Populations.
    */

    iter = ajListIterNewread(gvp->SubPopulations);
    
    while(!ajListIterDone(iter))
    {
	subgvp = (EnsPGVPopulation) ajListIterGet(iter);
	
	size += ensGVPopulationGetMemSize(subgvp);
    }
    
    ajListIterDel(&iter);
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Genetic Variation Population object.
**
** @fdata [EnsPGVPopulation]
** @nam3rule Trace Report Ensembl Genetic Variation Population elements to
**                 debug file
**
** @argrule Trace gvp [const EnsPGVPopulation] Ensembl Genetic Variation
**                                             Population
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGVPopulationTrace *************************************************
**
** Trace an Ensembl Genetic Variation Population.
**
** @param [r] gvp [const EnsPGVPopulation] Ensembl Genetic Variation Population
** @param [r] level [ajuint] Indentation level
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVPopulationTrace(const EnsPGVPopulation gvp, ajuint level)
{
    AjIList iter = NULL;
    
    AjPStr indent = NULL;
    
    EnsPGVPopulation subgvp = NULL;
    
    if(!gvp)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensGVPopulationTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Sample %p\n"
	    "%S  SubPopulations %p\n",
	    indent, gvp,
	    indent, gvp->Use,
	    indent, gvp->Identifier,
	    indent, gvp->Adaptor,
	    indent, gvp->GVSample,
	    indent, gvp->SubPopulations);
    
    ensGVSampleTrace(gvp->GVSample, level + 1);
    
    /* Trace the AJAX List of Ensembl Genetic Variation (Sub-) Populations. */
    
    if(gvp->SubPopulations)
    {
	ajDebug("%S    AJAX List %p of "
		"Ensembl Genetic Variation (Sub-) Populations\n",
		indent, gvp->SubPopulations);
	
	iter = ajListIterNewread(gvp->SubPopulations);
	
	while(!ajListIterDone(iter))
	{
	    subgvp = (EnsPGVPopulation) ajListIterGet(iter);
	    
	    ensGVPopulationTrace(subgvp, level + 2);
	}
	
	ajListIterDel(&iter);
    }
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @datasection [EnsPGVPopulationAdaptor] Genetic Variation Population Adaptor
**
** Functions for manipulating Ensembl Genetic Variation Population Adaptor
** objects
**
** @nam2rule GVPopulationAdaptor
**
******************************************************************************/

static const char *gvPopulationAdaptorTables[] =
{
    "population",
    "sample",
    NULL
};




static const char *gvPopulationAdaptorColumns[] =
{
    "sample.sample_id",
    "sample.name",
    "sample.size",
    "sample.description",
    NULL
};




static EnsOBaseAdaptorLeftJoin gvPopulationAdaptorLeftJoin[] =
{
    {NULL, NULL}
};




static const char *gvPopulationAdaptorDefaultCondition =
"sample.sample_id = population.sample_id";




static const char *gvPopulationAdaptorFinalCondition = NULL;




/* @funcstatic gvPopulationAdaptorFetchAllBySQL *******************************
**
** Fetch all Ensembl Genetic Variation Population objects via an SQL statement.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblyMapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] gvps [AjPList] AJAX List of Ensembl Genetic Variation Population
**                           objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvPopulationAdaptorFetchAllBySQL(EnsPDatabaseAdaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblyMapper am,
                                               EnsPSlice slice,
                                               AjPList gvps)
{
    ajuint identifier = 0;
    ajuint ssize      = 0;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr sname        = NULL;
    AjPStr sdescription = NULL;
    
    EnsPGVPopulation gvp         = NULL;
    EnsPGVPopulationAdaptor gvpa = NULL;
    
    EnsPGVSample gvs         = NULL;
    EnsPGVSampleAdaptor gvsa = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    (void) am;
    
    (void) slice;
    
    if(!gvps)
	return ajFalse;
    
    gvpa = ensRegistryGetGVPopulationAdaptor(dba);
    
    gvsa = ensGVPopulationAdaptorGetAdaptor(gvpa);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(dba, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	identifier   = 0;
	sname        = ajStrNew();
	ssize        = 0;
	sdescription = ajStrNew();
	
	sqlr = ajSqlRowIterGet(sqli);
	
	ajSqlColumnToUint(sqlr, &identifier);
        ajSqlColumnToStr(sqlr, &sname);
	ajSqlColumnToUint(sqlr, &ssize);
        ajSqlColumnToStr(sqlr, &sdescription);
	
	gvs = ensGVSampleNew(gvsa,
			     identifier,
			     sname,
			     sdescription,
			     ssize);
	
	gvp = ensGVPopulationNew(gvpa,
				 identifier,
				 gvs,
				 (AjPList) NULL);
	
	ensGVSampleDel(&gvs);
	
	ajStrDel(&sname);
	ajStrDel(&sdescription);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    return ajTrue;
}




/* @func ensGVPopulationAdaptorNew ********************************************
**
** Default constructor for an Ensembl Genetic Variation Population Adaptor.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::PopulationAdaptor::new
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                   Population Adaptor or NULL
** @@
******************************************************************************/

EnsPGVPopulationAdaptor ensGVPopulationAdaptorNew(EnsPDatabaseAdaptor dba)
{
    EnsPGVPopulationAdaptor gvpa = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(gvpa);
    
    gvpa->Adaptor = ensGVSampleAdaptorNew(dba,
					  gvPopulationAdaptorTables,
					  gvPopulationAdaptorColumns,
					  gvPopulationAdaptorLeftJoin,
					  gvPopulationAdaptorDefaultCondition,
					  gvPopulationAdaptorFinalCondition,
					  gvPopulationAdaptorFetchAllBySQL);
    
    if(!gvpa->Adaptor)
	AJFREE(gvpa);
    
    return gvpa;
}




/* @func ensGVPopulationAdaptorDel ********************************************
**
** Default destructor for an Ensembl Gentic Variation Population Adaptor.
**
** @param [d] Pgvpa [EnsPGVPopulationAdaptor*] Ensembl Genetic Variation
**                                             Population Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGVPopulationAdaptorDel(EnsPGVPopulationAdaptor *Pgvpa)
{
    EnsPGVPopulationAdaptor pthis = NULL;
    
    if(!Pgvpa)
	return;
    
    if(!*Pgvpa)
	return;

    pthis = *Pgvpa;
    
    ensGVSampleAdaptorDel(&pthis->Adaptor);
    
    AJFREE(pthis);

    *Pgvpa = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Population Adaptor object.
**
** @fdata [EnsPGVPopulationAdaptor]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Population Adaptor attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Sample Adaptor
**
** @argrule * gvpa [const EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
**
** @valrule Adaptor [EnsPGVSampleAdaptor] Ensembl Genetic Variation
**                                        Sample Adaptor
**
** @fcategory use
******************************************************************************/




/* @func ensGVPopulationAdaptorGetAdaptor *************************************
**
** Get the Ensembl Genetic Variation Sample Adaptor element of an
** Ensembl Genetic Variation Population Adaptor.
**
** @param [r] gvpa [const EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
**
** @return [EnsPGVSampleAdaptor] Ensembl Genetic Variation Sample Adaptor
** @@
******************************************************************************/

EnsPGVSampleAdaptor ensGVPopulationAdaptorGetAdaptor(
    const EnsPGVPopulationAdaptor gvpa)
{
    if(!gvpa)
	return NULL;
    
    return gvpa->Adaptor;
}




/* @func ensGVPopulationAdaptorFetchByIdentifier ******************************
**
** Fetch an Ensembl Genetic Variation Population via its
** SQL database-internal identifier.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Population.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::PopulationAdaptor::fetch_by_dbID
** @param [r] gvpa [const EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
** @param [r] identifier [ajuint] SQL database-internal Population identifier
** @param [wP] Pgvp [EnsPGVPopulation*] Ensembl Genetic Variation Population
**                                      address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVPopulationAdaptorFetchByIdentifier(EnsPGVPopulationAdaptor gvpa,
                                               ajuint identifier,
                                               EnsPGVPopulation *Pgvp)
{
    EnsPBaseAdaptor ba = NULL;
    
    if(!gvpa)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pgvp)
	return ajFalse;
    
    ba = ensGVSampleAdaptorGetAdaptor(gvpa->Adaptor);
    
    *Pgvp = (EnsPGVPopulation)
	ensBaseAdaptorFetchByIdentifier(ba, identifier);
    
    return ajTrue;
}




/* @func ensGVPopulationAdaptorFetchByName ************************************
**
** Fetch an Ensembl Genetic Variation Population by name.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::PopulationAdaptor::fetch_by_name
** @param [r] gvpa [const EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
** @param [r] name [const AjPStr] Ensembl Genetic Variation Population name
** @param [w] Pgvp [EnsPGVPopulation] Ensembl Genetic Variation Population
**                                    address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVPopulationAdaptorFetchByName(EnsPGVPopulationAdaptor gvpa,
                                         const AjPStr name,
                                         EnsPGVPopulation* Pgvp)
{
    char *txtname = NULL;
    
    AjPList gvps = NULL;
    
    AjPStr constraint = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    EnsPGVPopulation gvp = NULL;
    
    if(!gvpa)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    if(!Pgvp)
	return ajFalse;
    
    ba = ensGVSampleAdaptorGetAdaptor(gvpa->Adaptor);
    
    ensBaseAdaptorEscapeCS(ba, &txtname, name);
    
    constraint = ajFmtStr("sample.name = '%s'", txtname);
    
    ajCharDel(&txtname);
    
    gvps = ajListNew();
    
    ensBaseAdaptorGenericFetch(ba,
			       constraint,
			       (EnsPAssemblyMapper) NULL,
			       (EnsPSlice) NULL,
			       gvps);
    
    /* FIXME: Should we warn here? */
    if(ajListGetLength(gvps) == 0)
	ajDebug("ensGVPopulationAdaptorFetchByName could not get an "
		"Ensembl Genetic Variation Population "
		"for name '%S'.\n",
		name);
    
    if(ajListGetLength(gvps) > 1)
	ajDebug("ensGVPopulationAdaptorFetchByName got more than one "
		"Ensembl Genetic Variation Population "
		"for name '%S'.\n",
		name);
    
    ajListPop(gvps, (void **) Pgvp);
    
    while(ajListPop(gvps, (void **) &gvp))
	ensGVPopulationDel(&gvp);
    
    ajListFree(&gvps);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensGVIndividualAdaptorFetchAllBySynonym ******************************
**
** Fetch all Ensembl Genetic Variation Individuals by an
** Ensembl Genetic Variation Sample synonym.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::PopulationAdaptor::
**     fetch_population_by_synonym
** @param [r] gvpa [const EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
** @param [r] synonym [const AjPStr] Ensembl Genetic Variation Sample synonym
** @param [u] gvps [AjPList] AJAX List of Ensembl Genetic Variation Populations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVPopulationAdaptorFetchAllBySynonym(EnsPGVPopulationAdaptor gvpa,
                                               const AjPStr synonym,
                                               AjPList gvps)
{
    ajuint *Pidentifier = 0;
    
    AjPList idlist = NULL;
    
    EnsPGVPopulation gvp = NULL;
    
    if(!gvpa)
	return ajFalse;
    
    if(!synonym)
	return ajFalse;
    
    if(!gvps)
	return ajFalse;
    
    idlist = ajListNew();
    
    ensGVSampleAdaptorFetchAllIdentifiersBySynonym(gvpa->Adaptor,
						   synonym,
						   (const AjPStr) NULL,
						   idlist);
    
    while(ajListPop(idlist, (void **) &Pidentifier))
    {
	ensGVPopulationAdaptorFetchByIdentifier(gvpa,
						*Pidentifier,
						&gvp);
	
	ajListPushAppend(gvps, (void *) gvp);
	
	AJFREE(Pidentifier);
    }
    
    ajListFree(&idlist);
    
    return ajTrue;
}




/* @func ensGVPopulationAdaptorFetchAllBySuperPopulation **********************
**
** Fetch all Ensembl Genetic Variation Populations by a super Population.
**
** @param [r] gvpa [const EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
** @param [r] gvp [const EnsPGVPopulation] Ensembl Genetic Variation Population
** @param [u] gvps [AjPList] AJAX List of Ensembl Genetic Variation Populations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVPopulationAdaptorFetchAllBySuperPopulation(
    EnsPGVPopulationAdaptor gvpa,
    const EnsPGVPopulation gvp,
    AjPList gvps)
{
    AjPStr statement = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!gvpa)
	return ajFalse;
    
    if(!gvp)
	return ajFalse;
    
    if(!gvps)
	return ajFalse;
    
    if(!gvp->Identifier)
    {
	ajDebug("ensGVPopulationAdaptorFetchAllBySuperPopulation "
		"cannot fetch Population for super Population "
		"without identifier.\n");
	
	return ajFalse;
    }
    
    /*
    ** This function does not use the Ensembl Base Adaptor functionality
    ** because an additional 'population_structure' table is required.
    */
    
    statement = ajFmtStr("SELECT "
			 "sample.sample_id, "
			 "sample.name, "
			 "sample.size, "
			 "sample.description "
			 "FROM "
			 "population, "
			 "population_structure, "
			 "sample "
			 "WHERE "
			 "population.sample_id = "
			 "population_structure.sub_population_sample_id AND "
			 "population_structure.super_population_sample_id "
                         "= %u AND "
			 "population.sample_id = sample.sample_id",
			 gvp->Identifier);
    
    ba = ensGVSampleAdaptorGetAdaptor(gvpa->Adaptor);
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(ba);
    
    gvPopulationAdaptorFetchAllBySQL(dba,
				     statement,
				     (EnsPAssemblyMapper) NULL,
				     (EnsPSlice) NULL,
				     gvps);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensGVPopulationAdaptorFetchAllBySubPopulation ************************
**
** Fetch all Ensembl Genetic Variation Populations by a sub Population.
**
** @param [r] gvpa [const EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
** @param [r] gvp [const EnsPGVPopulation] Ensembl Genetic Variation Population
** @param [u] gvps [AjPList] AJAX List of Ensembl Genetic Variation Populations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVPopulationAdaptorFetchAllBySubPopulation(
    EnsPGVPopulationAdaptor gvpa,
    const EnsPGVPopulation gvp,
    AjPList gvps)
{
    AjPStr statement = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!gvpa)
	return ajFalse;
    
    if(!gvp)
	return ajFalse;
    
    if(!gvps)
	return ajFalse;
    
    if(!gvp->Identifier)
    {
	ajDebug("ensGVPopulationAdaptorFetchAllBySubPopulation "
		"cannot fetch Population for sub Population "
		"without identifier.\n");
	
	return ajFalse;
    }
    
    /*
    ** This function does not use the Ensembl Base Adaptor functionality
    ** because an additional 'population_structure' table is required.
    */
    
    statement = ajFmtStr("SELECT "
			 "sample.sample_id, "
			 "sample.name, "
			 "sample.size, "
			 "sample.description "
			 "FROM "
			 "population, "
			 "population_structure, "
			 "sample "
			 "WHERE "
			 "population.sample_id = "
			 "population_structure.super_population_sample_id AND "
			 "population_structure.sub_population_sample_id "
                         "= %u AND "
			 "population.sample_id = sample.sample_id",
			 gvp->Identifier);
    
    ba = ensGVSampleAdaptorGetAdaptor(gvpa->Adaptor);
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(ba);
    
    gvPopulationAdaptorFetchAllBySQL(dba,
				     statement,
				     (EnsPAssemblyMapper) NULL,
				     (EnsPSlice) NULL,
				     gvps);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensGVPopulationAdaptorFetchAllByIndvividual **************************
**
** Fetch all Ensembl Genetic Variation Populations by an
** Ensembl Genetic Variation Individual.
**
** @param [r] gvpa [const EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
** @param [r] gvi [const EnsPGVIndividual] Ensembl Genetic Variation Individual
** @param [u] gvps [AjPList] AJAX List of Ensembl Genetic Variation Populations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVPopulationAdaptorFetchAllByIndvividual(
    EnsPGVPopulationAdaptor gvpa,
    EnsPGVIndividual gvi,
    AjPList gvps)
{
    AjPStr statement = NULL;
    
    EnsPBaseAdaptor ba = NULL;
    
    EnsPDatabaseAdaptor dba = NULL;
    
    if(!gvpa)
	return ajFalse;
    
    if(!gvi)
	return ajFalse;
    
    if(!gvps)
	return ajFalse;
    
    if(!gvi->Identifier)
    {
	ajDebug("ensGVPopulationAdaptorFetchAllByIndvividual "
		"cannot fetch Population for Individual "
		"without identifier.\n");
	
	return ajFalse;
    }
    
    /*
    ** This function does not use the Ensembl Base Adaptor functionality
    ** because an additional 'population_structure' table is required.
    */
    
    statement = ajFmtStr("SELECT "
			 "sample.sample_id, "
			 "sample.name, "
			 "sample.size, "
			 "sample.description "
			 "FROM "
			 "population, "
			 "individual_population, "
			 "sample "
			 "WHERE "
			 "sample.sample_id = "
			 "individual_population.population_sample_id AND "
			 "sample.sample_id = population.sample_id AND "
			 "individual_population.individual_sample_id = %u",
			 gvi->Identifier);
    
    ba = ensGVSampleAdaptorGetAdaptor(gvpa->Adaptor);
    
    dba = ensBaseAdaptorGetDatabaseAdaptor(ba);
    
    gvPopulationAdaptorFetchAllBySQL(dba,
				     statement,
				     (EnsPAssemblyMapper) NULL,
				     (EnsPSlice) NULL,
				     gvps);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @datasection [EnsPGVAllele] Genetic Variation Allele ***********************
**
** Functions for manipulating Ensembl Genetic Variation Allele objects
**
** @nam2rule GVAllele
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Allele by pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Allele. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGVAllele]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGVAllele] Ensembl Genetic Variation Allele
** @argrule Ref object [EnsPGVAllele] Ensembl Genetic Variation Allele
**
** @valrule * [EnsPGVAllele] Ensembl Genetic Variation Allele
**
** @fcategory new
******************************************************************************/




/* @func ensGVAlleleNew *******************************************************
**
** Default constructor for an Ensembl Genetic Variation Allele.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvaa [EnsPGVAlleleAdaptor] Ensembl Genetic Variation
**                                       Allele Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Allele::new
** @param [u] gvp [EnsPGVPopulation] Ensembl Genetic Variation Population
** @param [u] allelestr [AjPStr] Allele
** @param [r] frequency [float] Frequency
**
** @return [EnsPGVAllele] Ensembl Genetic Variation Allele
** @@
******************************************************************************/

EnsPGVAllele ensGVAlleleNew(EnsPGVAlleleAdaptor gvaa,
                            ajuint identifier,
                            EnsPGVPopulation gvp,
                            AjPStr allelestr,
                            float frequency)
{
    EnsPGVAllele gva = NULL;
    
    if(!gvp)
	return NULL;
    
    if(!allelestr)
	return NULL;
    
    AJNEW0(gva);
    
    gva->Use = 1;
    
    gva->Identifier = identifier;
    
    gva->Adaptor = gvaa;
    
    gva->GVPopulation = ensGVPopulationNewRef(gvp);
    
    if(allelestr)
	gva->Allele = ajStrNewS(allelestr);
    
    gva->Frequency = frequency;
    
    return gva;
}




/* @func ensGVAlleleNewObj ****************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGVAllele] Ensembl Genetic Variation Allele
**
** @return [EnsPGVAllele] Ensembl Genetic Variation Allele or NULL
** @@
******************************************************************************/

EnsPGVAllele ensGVAlleleNewObj(const EnsPGVAllele object)
{
    EnsPGVAllele gva = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(gva);
    
    gva->Use = 1;
    
    gva->Identifier = object->Identifier;
    
    gva->Adaptor = object->Adaptor;
    
    gva->GVPopulation = ensGVPopulationNewRef(object->GVPopulation);
    
    if(object->Allele)
	gva->Allele = ajStrNewRef(object->Allele);
    
    gva->Frequency = object->Frequency;
    
    return gva;
}




/* @func ensGVAlleleNewRef ****************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gva [EnsPGVAllele] Ensembl Genetic Variation Allele
**
** @return [EnsPGVAllele] Ensembl Genetic Variation Allele
** @@
******************************************************************************/

EnsPGVAllele ensGVAlleleNewRef(EnsPGVAllele gva)
{
    if(!gva)
	return NULL;
    
    gva->Use++;
    
    return gva;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Allele.
**
** @fdata [EnsPGVAllele]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Allele object
**
** @argrule * Pgva [EnsPGVAllele*] Genetic Variation Allele object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGVAlleleDel *******************************************************
**
** Default destructor for an Ensembl Genetic Variation Allele.
**
** @param [d] Pgva [EnsPGVAllele*] Ensembl Genetic Variation Allele address
**
** @return [void]
** @@
******************************************************************************/

void ensGVAlleleDel(EnsPGVAllele *Pgva)
{
    EnsPGVAllele pthis = NULL;
    
    /*
     ajDebug("ensGVAlleleDel\n"
	     "  *Pgva %p\n",
	     *Pgva);
     
     ensGVAlleleTrace(*Pgva, 1);
     */
    
    if(!Pgva)
        return;
    
    if(!*Pgva)
        return;

    pthis = *Pgva;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pgva = NULL;
	
	return;
    }
    
    ensGVPopulationDel(&pthis->GVPopulation);
    
    ajStrDel(&pthis->Allele);
    
    AJFREE(pthis);

    *Pgva = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Allele object.
**
** @fdata [EnsPGVAllele]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Allele attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Allele Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetGVPopulation Return the Ensembl Genetic Variation Population
** @nam4rule GetAllele Return the allele
** @nam4rule GetFrequency Return the frequency
**
** @argrule * gva [const EnsPGVAllele] Genetic Variation Allele
**
** @valrule Adaptor [EnsPGVAlleleAdaptor] Ensembl Genetic Variation
**                                        Allele Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule GVPopulation [EnsPGVPopulation] Ensembl Genetic Variation
**                                          Population
** @valrule Allele [AjPStr] Allele
** @valrule Frequency [float] Frequency
**
** @fcategory use
******************************************************************************/




/* @func ensGVAlleleGetAdaptor ************************************************
**
** Get the Ensembl Genetic Variation Allele Adaptor element of an
** Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGVAllele] Ensembl Genetic Variation Allele
**
** @return [EnsPGVAlleleAdaptor] Ensembl Genetic Variation Allele Adaptor
** @@
******************************************************************************/

EnsPGVAlleleAdaptor ensGVAlleleGetAdaptor(const EnsPGVAllele gva)
{
    if(!gva)
        return NULL;
    
    return gva->Adaptor;
}




/* @func ensGVAlleleGetIdentifier *********************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGVAllele] Ensembl Genetic Variation Allele
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGVAlleleGetIdentifier(const EnsPGVAllele gva)
{
    if(!gva)
        return 0;
    
    return gva->Identifier;
}




/* @func ensGVAlleleGetPopulation *********************************************
**
** Get the Ensembl Genetic Variation Population element of an
** Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGVAllele] Ensembl Genetic Variation Allele
**
** @return [EnsPGVPopulation] Ensembl Genetic Variation Population
** @@
******************************************************************************/

EnsPGVPopulation ensGVAlleleGetPopulation(const EnsPGVAllele gva)
{
    if(!gva)
        return NULL;
    
    return gva->GVPopulation;
}




/* @func ensGVAlleleGetAllele *************************************************
**
** Get the allele element of an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGVAllele] Ensembl Genetic Variation Allele
**
** @return [AjPStr] Allele
** @@
******************************************************************************/

AjPStr ensGVAlleleGetAllele(const EnsPGVAllele gva)
{
    if(!gva)
        return NULL;
    
    return gva->Allele;
}




/* @func ensGVAlleleGetFrequency **********************************************
**
** Get the frequency element of an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGVAllele] Ensembl Genetic Variation Allele
**
** @return [float] Frequency
** @@
******************************************************************************/

float ensGVAlleleGetFrequency(const EnsPGVAllele gva)
{
    if(!gva)
        return 0;
    
    return gva->Frequency;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Allele object.
**
** @fdata [EnsPGVAllele]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Allele
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Allele Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetGVPopulation Set the Ensembl Genetic Variation Population
** @nam4rule SetAllele Set the allele
** @nam4rule SetFrequency Set the frequency
**
** @argrule * gva [EnsPGVAllele] Ensembl Genetic Variation Allele object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGVAlleleSetAdaptor ************************************************
**
** Set the Ensembl Genetic Variation Allele Adaptor element of an
** Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGVAllele] Ensembl Genetic Variation Allele
** @param [r] gvaa [EnsPGVAlleleAdaptor] Ensembl Genetic Variation
**                                       Allele Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVAlleleSetAdaptor(EnsPGVAllele gva, EnsPGVAlleleAdaptor gvaa)
{
    if(!gva)
        return ajFalse;
    
    gva->Adaptor = gvaa;
    
    return ajTrue;
}




/* @func ensGVAlleleSetIdentifier *********************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGVAllele] Ensembl Genetic Variation Allele
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVAlleleSetIdentifier(EnsPGVAllele gva, ajuint identifier)
{
    if(!gva)
        return ajFalse;
    
    gva->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGVAlleleSetPopulation *********************************************
**
** Set the Ensembl Genetic Variation Population element of an
** Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGVAllele] Ensembl Genetic Variation Allele
** @param [u] gvp [EnsPGVPopulation] Ensembl Genetic Variation Population
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVAlleleSetPopulation(EnsPGVAllele gva, EnsPGVPopulation gvp)
{
    if(!gva)
        return ajFalse;
    
    ensGVPopulationDel(&gva->GVPopulation);
    
    gva->GVPopulation = ensGVPopulationNewRef(gvp);
    
    return ajTrue;
}




/* @func ensGVAlleleSetAllele *************************************************
**
** Set the allele element of an Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGVAllele] Ensembl Genetic Variation Allele
** @param [u] allelestr [AjPStr] Allele
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVAlleleSetAllele(EnsPGVAllele gva, AjPStr allelestr)
{
    if(!gva)
        return ajFalse;
    
    ajStrDel(&gva->Allele);
    
    if(allelestr)
	gva->Allele = ajStrNewRef(allelestr);
    
    return ajTrue;
}




/* @func ensGVAlleleSetFrequency **********************************************
**
** Set the frequency element of an Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGVAllele] Ensembl Genetic Variation Allele
** @param [r] frequency [float] Frequency
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVAlleleSetFrequency(EnsPGVAllele gva, float frequency)
{
    if(!gva)
        return ajFalse;
    
    gva->Frequency = frequency;
    
    return ajTrue;
}




/* @func ensGVAlleleGetMemSize ************************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGVAllele] Ensembl Genetic Variation Allele
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGVAlleleGetMemSize(const EnsPGVAllele gva)
{
    ajuint size = 0;
    
    if(!gva)
	return 0;
    
    size += (ajuint) sizeof (EnsOGVAllele);
    
    size += ensGVPopulationGetMemSize(gva->GVPopulation);
    
    if(gva->Allele)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gva->Allele);
    }
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Genetic Variation Allele object.
**
** @fdata [EnsPGVAllele]
** @nam3rule Trace Report Ensembl Genetic Variation Allele elements to
**                 debug file
**
** @argrule Trace gva [const EnsPGVAllele] Ensembl Genetic Variation Allele
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGVAlleleTrace *****************************************************
**
** Trace an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGVAllele] Ensembl Genetic Variation Allele
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVAlleleTrace(const EnsPGVAllele gva, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!gva)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensGVAlleleTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Population %p\n"
	    "%S  Allele '%S'\n"
	    "%S  Frequency %f\n",
	    indent, gva,
	    indent, gva->Use,
	    indent, gva->Identifier,
	    indent, gva->Adaptor,
	    indent, gva->GVPopulation,
	    indent, gva->Allele,
	    indent, gva->Frequency);
    
    ensGVPopulationTrace(gva->GVPopulation, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @datasection [EnsPGVGenotype] Genetic Variation Genotype *******************
**
** Functions for manipulating Ensembl Genetic Variation Genotype objects
**
** @nam2rule GVGenotype
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Genotype by pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Genotype. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGVGenotype]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGVGenotype] Ensembl Genetic Variation Genotype
** @argrule Ref object [EnsPGVGenotype] Ensembl Genetic Variation Genotype
**
** @valrule * [EnsPGVGenotype] Ensembl Genetic Variation Genotype
**
** @fcategory new
******************************************************************************/




/* @func ensGVGenotypeNew *****************************************************
**
** Default constructor for an Ensembl Genetic Variation Genotype.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvga [EnsPGVGenotypeAdaptor] Ensembl Genetic Variation
**                                         Genotype Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Genotype::new
** @param [u] allele1 [AjPStr] Allele 1
** @param [u] allele2 [AjPStr] Allele 2
**
** @return [EnsPGVGenotype] Ensembl Genetic Variation Genotype
** @@
******************************************************************************/

EnsPGVGenotype ensGVGenotypeNew(EnsPGVGenotypeAdaptor gvga,
                                ajuint identifier,
                                AjPStr allele1,
                                AjPStr allele2)
{
    EnsPGVGenotype gvg = NULL;
    
    AJNEW0(gvg);
    
    gvg->Use = 1;
    
    gvg->Identifier = identifier;
    
    gvg->Adaptor = gvga;
    
    if(allele1)
        gvg->Allele1 = ajStrNewRef(allele1);
    
    if(allele2)
        gvg->Allele2 = ajStrNewRef(allele2);
    
    return gvg;
}




/* @func ensGVGenotypeNewObj **************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGVGenotype] Ensembl Genetic Variation Genotype
**
** @return [EnsPGVGenotype] Ensembl Genetic Variation Genotype or NULL
** @@
******************************************************************************/

EnsPGVGenotype ensGVGenotypeNewObj(const EnsPGVGenotype object)
{
    EnsPGVGenotype gvg = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(gvg);
    
    gvg->Use = 1;
    
    gvg->Identifier = object->Identifier;
    
    gvg->Adaptor = object->Adaptor;
    
    if(object->Allele1)
        gvg->Allele1 = ajStrNewRef(object->Allele1);
    
    if(object->Allele2)
        gvg->Allele2 = ajStrNewRef(object->Allele2);
    
    return gvg;
}




/* @func ensGVGenotypeNewRef **************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvg [EnsPGVGenotype] Ensembl Genetic Variation Genotype
**
** @return [EnsPGVGenotype] Ensembl Genetic Variation Genotype
** @@
******************************************************************************/

EnsPGVGenotype ensGVGenotypeNewRef(EnsPGVGenotype gvg)
{
    if(!gvg)
	return NULL;
    
    gvg->Use++;
    
    return gvg;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Genotype.
**
** @fdata [EnsPGVGenotype]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Genotype object
**
** @argrule * Pgvg [EnsPGVGenotype*] Genetic Variation Genotype
**                                   object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGVGenotypeDel *****************************************************
**
** Default destructor for an Ensembl Genetic Variation Genotype.
**
** @param [d] Pgvg [EnsPGVGenotype*] Ensembl Genetic Variation Genotype
**                                   address
**
** @return [void]
** @@
******************************************************************************/

void ensGVGenotypeDel(EnsPGVGenotype *Pgvg)
{
    EnsPGVGenotype pthis = NULL;
    
    /*
     ajDebug("ensGVGenotypeDel\n"
	     "  *Pgvg %p\n",
	     *Pgvg);
     
     ensGVGenotypeTrace(*Pgvg, 1);
     */
    
    if(!Pgvg)
        return;
    
    if(!*Pgvg)
        return;

    pthis = *Pgvg;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pgvg = NULL;
	
	return;
    }
    
    ajStrDel(&pthis->Allele1);
    ajStrDel(&pthis->Allele2);
    
    AJFREE(pthis);

    *Pgvg = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Genotype object.
**
** @fdata [EnsPGVGenotype]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Genotype attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Genotype Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetAllele1 Return the allele 1
** @nam4rule GetAllele2 Return the allele 2
**
** @argrule * gvg [const EnsPGVGenotype] Genetic Variation Genotype
**
** @valrule Adaptor [EnsPGVGenotypeAdaptor] Ensembl Genetic Variation
**                                          Genotype Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Allele1 [AjPStr] Allele 1
** @valrule Allele2 [AjPStr] Allele 2
**
** @fcategory use
******************************************************************************/




/* @func ensGVGenotypeGetAdaptor **********************************************
**
** Get the Ensembl Genetic Variation Genotype Adaptor element of an
** Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGVGenotype] Ensembl Genetic Variation Genotype
**
** @return [EnsPGVGenotypeAdaptor] Ensembl Genetic Variation Genotype Adaptor
** @@
******************************************************************************/

EnsPGVGenotypeAdaptor ensGVGenotypeGetAdaptor(const EnsPGVGenotype gvg)
{
    if(!gvg)
        return NULL;
    
    return gvg->Adaptor;
}




/* @func ensGVGenotypeGetIdentifier *******************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGVGenotype] Ensembl Genetic Variation Genotype
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGVGenotypeGetIdentifier(const EnsPGVGenotype gvg)
{
    if(!gvg)
        return 0;
    
    return gvg->Identifier;
}




/* @func ensGVGenotypeGetAllele1 **********************************************
**
** Get the allele 1 element of an Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGVGenotype] Ensembl Genetic Variation Genotype
**
** @return [AjPStr] Allele 1
** @@
******************************************************************************/

AjPStr ensGVGenotypeGetAllele1(const EnsPGVGenotype gvg)
{
    if(!gvg)
        return NULL;
    
    return gvg->Allele1;
}




/* @func ensGVGenotypeGetAllele2 **********************************************
**
** Get the allele 2 element of an Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGVGenotype] Ensembl Genetic Variation Genotype
**
** @return [AjPStr] Allele 2
** @@
******************************************************************************/

AjPStr ensGVGenotypeGetAllele2(const EnsPGVGenotype gvg)
{
    if(!gvg)
        return NULL;
    
    return gvg->Allele2;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Genotype object.
**
** @fdata [EnsPGVGenotype]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Genotype
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Genotype Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetAllele1 Set the allele 1
** @nam4rule SetAllele2 Set the allele 2
**
** @argrule * gvg [EnsPGVGenotype] Ensembl Genetic Variation Genotype object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGVGenotypeSetAdaptor **********************************************
**
** Set the Ensembl Genetic Variation Genotype Adaptor element of an
** Ensembl Genetic Variation Genotype.
**
** @param [u] gvg [EnsPGVSample] Ensembl Genetic Variation Genotype
** @param [r] gvga [EnsPGVGenotypeAdaptor] Ensembl Genetic Variation
**                                         Genotype Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVGenotypeSetAdaptor(EnsPGVGenotype gvg, EnsPGVGenotypeAdaptor gvga)
{
    if(!gvg)
        return ajFalse;
    
    gvg->Adaptor = gvga;
    
    return ajTrue;
}




/* @func ensGVGenotypeSetIdentifier *******************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Genotype.
**
** @param [u] gvg [EnsPGVGenotype] Ensembl Genetic Variation Genotype
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVGenotypeSetIdentifier(EnsPGVGenotype gvg, ajuint identifier)
{
    if(!gvg)
        return ajFalse;
    
    gvg->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGVGenotypeSetAllele1 **********************************************
**
** Set the allele 1 element of an Ensembl Genetic Variation Genotype.
**
** @param [u] gvg [EnsPGVGenotype] Ensembl Genetic Variation Genotype
** @param [u] allele1 [AjPStr] Allele 1
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVGenotypeSetAllele1(EnsPGVGenotype gvg, AjPStr allele1)
{
    if(!gvg)
        return ajFalse;
    
    ajStrDel(&gvg->Allele1);
    
    if(allele1)
	gvg->Allele1 = ajStrNewRef(allele1);
    
    return ajTrue;
}




/* @func ensGVGenotypeSetAllele2 **********************************************
**
** Set the allele 2 element of an Ensembl Genetic Variation Genotype.
**
** @param [u] gvg [EnsPGVGenotype] Ensembl Genetic Variation Genotype
** @param [u] allele2 [AjPStr] Allele 2
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVGenotypeSetAllele2(EnsPGVGenotype gvg, AjPStr allele2)
{
    if(!gvg)
        return ajFalse;
    
    ajStrDel(&gvg->Allele2);
    
    if(allele2)
	gvg->Allele2 = ajStrNewRef(allele2);
    
    return ajTrue;
}




/* @func ensGVGenotypeGetMemSize **********************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGVGenotype] Ensembl Genetic Variation Genotype
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGVGenotypeGetMemSize(const EnsPGVGenotype gvg)
{
    ajuint size = 0;
    
    if(!gvg)
	return 0;
    
    size += (ajuint) sizeof (EnsOGVGenotype);
    
    if(gvg->Allele1)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvg->Allele1);
    }
    
    if(gvg->Allele2)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvg->Allele2);
    }
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Genetic Variation Genotype object.
**
** @fdata [EnsPGVGenotype]
** @nam3rule Trace Report Ensembl Genetic Variation Genotype elements to
**                 debug file
**
** @argrule Trace gvg [const EnsPGVGenotype] Ensembl Genetic Variation Genotype
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGVGenotypeTrace ***************************************************
**
** Trace an Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGVGenotype] Ensembl Genetic Variation Genotype
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVGenotypeTrace(const EnsPGVGenotype gvg, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!gvg)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensGVGenotypeTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Allele1 '%S'\n"
	    "%S  Allele2 '%S'\n",
	    indent, gvg,
	    indent, gvg->Use,
	    indent, gvg->Identifier,
	    indent, gvg->Adaptor,
	    indent, gvg->Allele1,
	    indent, gvg->Allele2);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @datasection [EnsPGVVariation] Genetic Variation Variation *****************
**
** Functions for manipulating Ensembl Genetic Variation Variation objects
**
** @nam2rule GVVariation
**
******************************************************************************/

static const char *gvVariationValidationState[] =
{
    "cluster",
    "freq",
    "submitter",
    "doublehit",
    "hapmap",
    "failed",
    "non-polymorphic",
    "observed",
    NULL
};




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Variation by
** pointer. It is the responsibility of the user to first destroy any previous
** Genetic Variation Variation. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGVVariation]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGVVariation] Ensembl Genetic Variation Variation
** @argrule Ref object [EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @valrule * [EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @fcategory new
******************************************************************************/




/* @func ensGVVariationNew ****************************************************
**
** Default constructor for an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvva [EnsPGVVariationAdaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Variation::new
** @param [u] name [AjPStr] Name
** @param [u] source [AjPStr] Source
** @param [r] synonyms [AjPTable] Synonyms, keys are source databases and
**                                values are AJAX Lists of AJAX Strings of
**                                synonyms
** @param [u] ancestralallele [AjPStr] Ancestral allele
** @param [r] alleles [AjPList] AJAX List of Ensembl Alleles
** @param [r] validationstates [AjPList] AJAX List of validation state
**                                       AJAX Strings
** @param [u] moltype [AjPStr] Molecule type
** @param [u] fiveflank [AjPStr] Five prime flanking sequence
** @param [u] threeflank [AjPStr] Three prime flanking sequence
** @param [u] faileddescription [AjPStr] Failed description
**
** @return [EnsPGVVariation] Ensembl Genetic Variation Variation
** @@
******************************************************************************/

EnsPGVVariation ensGVVariationNew(EnsPGVVariationAdaptor gvva,
                                  ajuint identifier,
                                  AjPStr name,
                                  AjPStr source,
                                  AjPStr ancestralallele,
                                  AjPTable synonyms,
                                  AjPList alleles,
                                  AjPList validationstates,
                                  AjPStr moltype,
                                  AjPStr fiveflank,
                                  AjPStr threeflank,
                                  AjPStr faileddescription)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    AjIList iter = NULL;
    AjPList list = NULL;
    
    AjPStr synonym = NULL;
    AjPStr vstate  = NULL;
    
    EnsPGVAllele gva = NULL;
    
    EnsPGVVariation gvv = NULL;
    
    /* FIXME: Input restrictions are missing. */
    
    AJNEW0(gvv);
    
    gvv->Use = 1;
    
    gvv->Identifier = identifier;
    
    gvv->Adaptor = gvva;
    
    if(name)
        gvv->Name = ajStrNewRef(name);
    
    if(source)
        gvv->Source = ajStrNewRef(source);
    
    if(ancestralallele)
        gvv->AncestralAllele = ajStrNewRef(ancestralallele);
    
    /*
    ** Copy the AJAX Table of AJAX String key data (source database) and
    ** AJAX List value data. The AJAX List contains AJAX Strings (synonyms).
    */
    
    if(synonyms)
    {
	gvv->Synonyms = ajTablestrNewLen(0);
	
	ajTableToarray(synonyms, &keyarray, &valarray);
	
	for(i = 0; keyarray[i]; i++)
	{
	    list = ajListstrNew();
	    
	    ajTablePut(gvv->Synonyms,
		       (void *) ajStrNewRef((AjPStr) keyarray[i]),
		       (void *) list);
	    
	    iter = ajListIterNew((AjPList) valarray[i]);
	    
	    while(!ajListIterDone(iter))
	    {
		synonym = (AjPStr) ajListIterGet(iter);
		
		if(synonym)
		    ajListPushAppend(list, (void *) ajStrNewRef(synonym));
	    }
	    
	    ajListIterDel(&iter);
	}
	
	AJFREE(keyarray);
	AJFREE(valarray);
    }
    
    /* Copy the AJAX List of Ensembl Alleles. */
    
    gvv->GVAlleles = ajListNew();
    
    iter = ajListIterNew(alleles);
    
    while(ajListIterDone(iter))
    {
	gva = (EnsPGVAllele) ajListIterGet(iter);
	
	ajListPushAppend(gvv->GVAlleles, (void *) ensGVAlleleNewRef(gva));
    }
    
    ajListIterDel(&iter);
    
    /* Copy the AJAX List of validation state AJAX Strings. */
    
    gvv->ValidationStates = ajListstrNew();
    
    iter = ajListIterNew(validationstates);
    
    while(!ajListIterDone(iter))
    {
	vstate = (AjPStr) ajListIterGet(iter);
	
	if(vstate)
	    ajListPushAppend(gvv->ValidationStates,
			     (void *) ajStrNewRef(vstate));
    }
    
    ajListIterDel(&iter);
    
    if(moltype)
	gvv->MoleculeType = ajStrNewRef(moltype);
    
    if(fiveflank)
	gvv->FivePrimeFlank = ajStrNewRef(fiveflank);
    
    if(threeflank)
	gvv->ThreePrimeFlank = ajStrNewRef(threeflank);
    
    if(faileddescription)
	gvv->FailedDescription = ajStrNewRef(faileddescription);
    
    return gvv;
}




/* @func ensGVVariationNewObj *************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGVVariation] Ensembl Genetic Variation
**                                           Variation
**
** @return [EnsPGVVariation] Ensembl Genetic Variation Variation or NULL
** @@
******************************************************************************/

EnsPGVVariation ensGVVariationNewObj(const EnsPGVVariation object)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    AjIList iter = NULL;
    AjPList list = NULL;
    
    AjPStr synonym = NULL;
    AjPStr vstate = NULL;
    
    EnsPGVAllele gva = NULL;
    
    EnsPGVVariation gvv = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(gvv);
    
    gvv->Use = 1;
    
    gvv->Identifier = object->Identifier;
    
    gvv->Adaptor = object->Adaptor;
    
    if(object->Name)
	gvv->Name = ajStrNewRef(object->Name);
    
    if(object->Source)
	gvv->Source = ajStrNewRef(object->Source);
    
    /*
    ** Copy the AJAX Table of AJAX String key data (source database) and
    ** AJAX List value data. The AJAX List contains AJAX Strings (synonyms).
    */
    
    if(object->Synonyms)
    {
	gvv->Synonyms = ajTablestrNewLen(0);
	
	ajTableToarray(object->Synonyms, &keyarray, &valarray);
	
	for(i = 0; keyarray[i]; i++)
	{
	    list = ajListstrNew();
	    
	    ajTablePut(gvv->Synonyms,
		       (void *) ajStrNewRef((AjPStr) keyarray[i]),
		       (void *) list);
	    
	    iter = ajListIterNew((AjPList) valarray[i]);
	    
	    while(!ajListIterDone(iter))
	    {
		synonym = (AjPStr) ajListIterGet(iter);
		
		if(synonym)
		    ajListPushAppend(list,
				     (void *) ajStrNewRef(synonym));
	    }
	    
	    ajListIterDel(&iter);
	}
	
	AJFREE(keyarray);
	AJFREE(valarray);
    }
    
    if(object->AncestralAllele)
	gvv->AncestralAllele = ajStrNewRef(object->AncestralAllele);
    
    /* Copy the AJAX List of Ensembl Genetic Variation Alleles. */
    
    gvv->GVAlleles = ajListNew();
    
    iter = ajListIterNew(object->GVAlleles);
    
    while(!ajListIterDone(iter))
    {
	gva = (EnsPGVAllele) ajListIterGet(iter);
	
	ajListPushAppend(gvv->GVAlleles, (void *) ensGVAlleleNewRef(gva));
    }
    
    ajListIterDel(&iter);
    
    /* Copy the AJAX List of variation state AJAX Strings. */
    
    /* FIXME: The variation API stores those as a bit score! */
    
    gvv->ValidationStates = ajListNew();
    
    iter = ajListIterNew(object->ValidationStates);
    
    while(!ajListIterDone(iter))
    {
	vstate = (AjPStr) ajListIterGet(iter);
	
	if(vstate)
	    ajListPushAppend(gvv->ValidationStates,
			     (void *) ajStrNewRef(vstate));
    }
    
    ajListIterDel(&iter);
    
    if(object->MoleculeType)
	gvv->MoleculeType = ajStrNewRef(object->MoleculeType);
    
    if(object->FivePrimeFlank)
	gvv->FivePrimeFlank = ajStrNewRef(object->FivePrimeFlank);
    
    if(object->ThreePrimeFlank)
	gvv->ThreePrimeFlank = ajStrNewRef(object->ThreePrimeFlank);
    
    if(object->FailedDescription)
	gvv->FailedDescription = ajStrNewRef(object->FailedDescription);
    
    return gvv;
}




/* @func ensGVVariationNewRef *************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [EnsPGVVariation] Ensembl Genetic Variation Variation
** @@
******************************************************************************/

EnsPGVVariation ensGVVariationNewRef(EnsPGVVariation gvv)
{
    if(!gvv)
	return NULL;
    
    gvv->Use++;
    
    return gvv;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Variation.
**
** @fdata [EnsPGVVariation]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Variation object
**
** @argrule * Pgvv [EnsPGVVariation*] Genetic Variation Variation
**                                    object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGVVariationDel ****************************************************
**
** Default destructor for an Ensembl Genetic Variation Variation.
**
** @param [d] Pgvv [EnsPGVVariation*] Ensembl Genetic Variation
**                                    Variation address
**
** @return [void]
** @@
******************************************************************************/

void ensGVVariationDel(EnsPGVVariation *Pgvv)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    AjPStr synonym = NULL;
    
    EnsPGVAllele gva = NULL;

    EnsPGVVariation pthis = NULL;
    
    
    /*
     ajDebug("ensGVVariationDel\n"
	     "  *Pgvv %p\n",
	     *Pgvv);
     
     ensGVVariationTrace(*Pgvv, 1);
     */
    
    if(!Pgvv)
	return;
    
    if(!*Pgvv)
	return;

    pthis = *Pgvv;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pgvv = NULL;
	
	return;
    }
    
    ajStrDel(&pthis->Name);
    ajStrDel(&pthis->Source);
    
    /*
    ** Clear the AJAX Table of AJAX String key data (source database) and
    ** AJAX List value data. The AJAX List contains AJAX Strings (synonyms).
    */
    
    if(pthis->Synonyms)
    {
	ajTableToarray(pthis->Synonyms, &keyarray, &valarray);
	
	for(i = 0; keyarray[i]; i++)
	{
	    ajStrDel((AjPStr *) &keyarray[i]);
	    
	    while(ajListPop((AjPList) valarray[i], (void **) &synonym))
		ajStrDel(&synonym);
	    
	    ajListFree((AjPList *) &valarray[i]);
	}
	
	AJFREE(keyarray);
	AJFREE(valarray);
    }
    
    ajStrDel(&pthis->AncestralAllele);
    
    /* Clear the AJAX List of Ensembl Genetic Variation Alleles. */
    
    while(ajListPop(pthis->GVAlleles, (void **) &gva))
	ensGVAlleleDel(&gva);
    
    ajListFree(&pthis->GVAlleles);
    
    /* Clear the AJAX List of validation state AJAX Strings. */
    
    while(ajListPop(pthis->ValidationStates, (void **) &synonym))
	ajStrDel(&synonym);
    
    ajListFree(&pthis->ValidationStates);
    
    ajStrDel(&pthis->MoleculeType);
    ajStrDel(&pthis->FivePrimeFlank);
    ajStrDel(&pthis->ThreePrimeFlank);
    ajStrDel(&pthis->FailedDescription);
    
    AJFREE(pthis);

    *Pgvv = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Variation object.
**
** @fdata [EnsPGVVariation]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Variation attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Variation Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetName Return the name
** @nam4rule GetSource Return the source
** @nam4rule GetSynoyms Return synonms
** @nam4rule GetAncestralAllele Return the ancestral allele
** @nam4rule GetGVAlleles Return Ensembl Genetic Variation Alleles
** @nam4rule GetValidationStates Return the validation states
** @nam4rule GetMoleculeType Return the molecule type
** @nam4rule GetFivePrimeFlank Return the five prime flank
** @nam4rule GetThreePrimeFlank Return the three prime flank
** @nam4rule GetFailedDescription Return the failed description
**
** @argrule * gvv [const EnsPGVVariation] Genetic Variation Variation
**
** @valrule Adaptor [EnsPGVVariationAdaptor] Ensembl Genetic Variation
**                                           Variation Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Name [AjPStr] Name
** @valrule Source [AjPStr] Source
** @valrule Synonyms [const AjPTable] Synoyms
** @valrule AncestralAllele [AjPStr] Ancestral allele
** @valrule GVAlleles [const AjPList] Ensembl Genetic Variation Alleles
** @valrule ValidationStates [const AjPList] Validation states
** @valrule MoleculeType [AjPStr] Molecule type
** @valrule fiveprimeflank [AjPStr] Five prime flank
** @valrule threeprimeflank [AjPStr] Three prime flank
** @valrule faileddescription [AjPStr] Failed description
**
** @fcategory use
******************************************************************************/




/* @func ensGVVariationGetAdaptor *********************************************
**
** Get the Ensembl Genetic Variation Variation Adaptor element of an
** Ensembl Genetic Variation Variation.
**
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [EnsPGVVariationAdaptor] Ensembl Genetic Variation
**                                  Variation Adaptor
** @@
******************************************************************************/

EnsPGVVariationAdaptor ensGVVariationGetAdaptor(const EnsPGVVariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->Adaptor;
}




/* @func ensGVVariationGetIdentifier ******************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation.
**
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGVVariationGetIdentifier(const EnsPGVVariation gvv)
{
    if(!gvv)
        return 0;
    
    return gvv->Identifier;
}




/* @func ensGVVariationGetName ************************************************
**
** Get the name element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::name
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensGVVariationGetName(const EnsPGVVariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->Name;
}




/* @func ensGVVariationGetSource **********************************************
**
** Get the source element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::source
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Source
** @@
******************************************************************************/

AjPStr ensGVVariationGetSource(const EnsPGVVariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->Source;
}




/* @func ensGVVariationGetSynonyms ********************************************
**
** Get the synonyms element of an Ensembl Genetic Variation Variation.
** Synonyms are stored in an AJAX Table of source AJAX String key data and
** AJAX List value data. The AJAX Lists contain synonym AJAX Strings.
**
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
** @see ensGVVariationFetchAllSynonyms
** @see ensGVVariationFetchAllSynonymSources
**
** @return [const AjPTable] Synonyms
** @@
******************************************************************************/

const AjPTable ensGVVariationGetSynonyms(const EnsPGVVariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->Synonyms;
}




/* @func ensGVVariationGetAncestralAllele *************************************
**
** Get the ancestral allele element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::ancestral_allele
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Ancestral allele
** @@
******************************************************************************/

AjPStr ensGVVariationGetAncestralAllele(const EnsPGVVariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->AncestralAllele;
}




/* @func ensGVVariationGetGVAlleles *******************************************
**
** Get all Ensembl Genetic Variation Alleles of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::get_all_Alleles
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [const AjPList] AJAX List of Ensembl Genetic Variation Alleles
** @@
******************************************************************************/

const AjPList ensGVVariationGetGVAlleles(const EnsPGVVariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->GVAlleles;
}




/* @func ensGVVariationGetValidationStates ************************************
**
** Get all validation states of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::get_all_validation_states
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [const AjPList] AJAX List of AJAX String validation states
** @@
******************************************************************************/

const AjPList ensGVVariationGetValidationStates(const EnsPGVVariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->ValidationStates;
}




/* @func ensGVVariationGetMoleculeType ****************************************
**
** Get the molecule type element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::moltype
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Molecule type
** @@
******************************************************************************/

AjPStr ensGVVariationGetMoleculeType(const EnsPGVVariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->MoleculeType;
}




/* @func ensGVVariationGetFivePrimeFlank **************************************
**
** Get the five prime flank element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::five_prime_flanking_seq
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Five prime flank
** @@
******************************************************************************/

AjPStr ensGVVariationGetFivePrimeFlank(const EnsPGVVariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->FivePrimeFlank;
}




/* @func ensGVVariationGetThreePrimeFlank *************************************
**
** Get the three prime flank element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::three_prime_flanking_seq
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Three prime flank
** @@
******************************************************************************/

AjPStr ensGVVariationGetThreePrimeFlank(const EnsPGVVariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->ThreePrimeFlank;
}




/* @func ensGVVariationGetFailedDescription ***********************************
**
** Get the failed description element of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::failed_description
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Three prime flank
** @@
******************************************************************************/

AjPStr ensGVVariationGetFailedDescription(const EnsPGVVariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->FailedDescription;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Variation object.
**
** @fdata [EnsPGVVariation]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Variation
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Variation Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetName Set the name
** @nam4rule SetSource Set the source
** @nam4rule SetAncestralAllele Set the ancestral allele
** @nam4rule SetMoleculeType Set the molecule type
** @nam4rule SetFivePrimeFlank Set the five prime flank
** @nam4rule SetThreePrimeFlank Set the three prime flank
** @nam4rule SetFailedDescription Set the failed description
**
** @argrule * gvv [EnsPGVVariation] Ensembl Genetic Variation Variation object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGVVariationSetAdaptor *********************************************
**
** Set the Ensembl Genetic Variation Variation Adaptor element of an
** Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [r] gvva [EnsPGVVariationAdaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationSetAdaptor(EnsPGVVariation gvv,
                                EnsPGVVariationAdaptor gvva)
{
    if(!gvv)
        return ajFalse;
    
    gvv->Adaptor = gvva;
    
    return ajTrue;
}




/* @func ensGVVariationSetIdentifier ******************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationSetIdentifier(EnsPGVVariation gvv, ajuint identifier)
{
    if(!gvv)
        return ajFalse;
    
    gvv->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGVVariationSetName ************************************************
**
** Set the name element of an Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationSetName(EnsPGVVariation gvv, AjPStr name)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->Name);
    
    if(name)
	gvv->Name = ajStrNewRef(name);
    
    return ajTrue;
}




/* @func ensGVVariationSetSource **********************************************
**
** Set the source element of an Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [u] source [AjPStr] Source
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationSetSource(EnsPGVVariation gvv, AjPStr source)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->Source);
    
    if(source)
	gvv->Source = ajStrNewRef(source);
    
    return ajTrue;
}




/* @func ensGVVariationSetAncestralAllele *************************************
**
** Set the ancestral allele element of an Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [u] ancestralallele [AjPStr] Ancestral allele
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationSetAncestralAllele(EnsPGVVariation gvv,
                                        AjPStr ancestralallele)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->AncestralAllele);
    
    if(ancestralallele)
	gvv->AncestralAllele = ajStrNewRef(ancestralallele);
    
    return ajTrue;
}




/* @func ensGVVariationSetMoleculeType ****************************************
**
** Set the molecule type element of an Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [u] moltype [AjPStr] Molecule type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationSetMoleculeType(EnsPGVVariation gvv, AjPStr moltype)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->AncestralAllele);
    
    if(moltype)
	gvv->AncestralAllele = ajStrNewRef(moltype);
    
    return ajTrue;
}




/* @func ensGVVariationSetFivePrimeFlank **************************************
**
** Set the five prime flank element of an Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [u] fiveflank [AjPStr] Five prime flank
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationSetFivePrimeFlank(EnsPGVVariation gvv, AjPStr fiveflank)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->FivePrimeFlank);
    
    if(fiveflank)
	gvv->FivePrimeFlank = ajStrNewRef(fiveflank);
    
    return ajTrue;
}




/* @func ensGVVariationSetThreePrimeFlank *************************************
**
** Set the three prime flank element of an Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [u] threeflank [AjPStr] Three prime flank
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationSetThreePrimeFlank(EnsPGVVariation gvv, AjPStr threeflank)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->ThreePrimeFlank);
    
    if(threeflank)
	gvv->ThreePrimeFlank = ajStrNewRef(threeflank);
    
    return ajTrue;
}




/* @func ensGVVariationSetFailedDescription ***********************************
**
** Set the failed description element of an
** Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [u] faileddescription [AjPStr] Failed description
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationSetFailedDescription(EnsPGVVariation gvv,
                                          AjPStr faileddescription)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->FailedDescription);
    
    if(faileddescription)
	gvv->FailedDescription = ajStrNewRef(faileddescription);
    
    return ajTrue;
}




/* @func ensGVVariationGetMemSize *********************************************
**
** Get the memory size in bytes of an
** Ensembl Genetic Variation Variation.
**
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGVVariationGetMemSize(const EnsPGVVariation gvv)
{
    ajuint size = 0;
    
    if(!gvv)
	return 0;
    
    size += (ajuint) sizeof (EnsOGVVariation);
    
    if(gvv->Name)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvv->Name);
    }
    
    if(gvv->Source)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvv->Source);
    }
    
    /* FIXME: AJAX Table of Synonms is missing! */
    
    if(gvv->AncestralAllele)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvv->AncestralAllele);
    }
    
    /* FIXME: AJAX List of Ensembl Genetic Variation Alleles is missing! */
    
    /* FIXME: AJAX List of AJAX String validation states is missing! */
    
    if(gvv->MoleculeType)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvv->MoleculeType);
    }
    
    if(gvv->FivePrimeFlank)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvv->FivePrimeFlank);
    }
    
    if(gvv->ThreePrimeFlank)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvv->ThreePrimeFlank);
    }
    
    if(gvv->FailedDescription)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvv->FailedDescription);
    }
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Genetic Variation Variation object.
**
** @fdata [EnsPGVVariation]
** @nam3rule Trace Report Ensembl Genetic Variation Variation elements
**                 to debug file
**
** @argrule Trace gvv [const EnsPGVVariation] Ensembl Genetic Variation
**                                            Variation
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGVVariationTrace **************************************************
**
** Trace an Ensembl Genetic Variation Variation.
**
** @param [r] gvv [const EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationTrace(const EnsPGVVariation gvv, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!gvv)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("ensGVVariationTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Name '%S'\n"
	    "%S  Source '%S'\n"
	    "%S  Synonyms %p\n"
	    "%S  AncestralAllele '%S'\n"
	    "%S  GVAlleles %p\n"
	    "%S  ValidationStates %p\n"
	    "%S  MoleculeType '%S'\n"
	    "%S  FivePrimeFlank '%S'\n"
	    "%S  ThreePrimeFlank '%S'\n"
	    "%S  FailedDescription '%S'\n",
	    indent, gvv,
	    indent, gvv->Use,
	    indent, gvv->Identifier,
	    indent, gvv->Adaptor,
	    indent, gvv->Name,
	    indent, gvv->Source,
	    indent, gvv->Synonyms,
	    indent, gvv->AncestralAllele,
	    indent, gvv->GVAlleles,
	    indent, gvv->ValidationStates,
	    indent, gvv->MoleculeType,
	    indent, gvv->FivePrimeFlank,
	    indent, gvv->ThreePrimeFlank,
	    indent, gvv->FailedDescription);
    
    ajStrDel(&indent);
    
    /* FIXME: AJAX Table of Synonms is missing! */
    
    /* FIXME: AJAX List of Ensembl Genetic Variation Alleles is missing! */
    
    /* FIXME: AJAX List of AJAX String validation states is missing! */
    
    return ajTrue;
}




/* @func ensGVVariationAddSynonym *********************************************
**
** Add a synonym to an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::add_synonym
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [u] source [AjPStr] Source database
** @param [u] synonym [AjPStr] Synonym
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationAddSynonym(EnsPGVVariation gvv, AjPStr source,
                                AjPStr synonym)
{
    AjPList list = NULL;
    
    if(!gvv)
	return ajFalse;
    
    if(!source)
	return ajFalse;
    
    if(!synonym)
	return ajFalse;
    
    if(!gvv->Synonyms)
	gvv->Synonyms = ajTablestrNewLen(0);
    
    list = (AjPList) ajTableFetch(gvv->Synonyms, (const void *) source);
    
    if(!list)
    {
	list = ajListstrNew();
	
	ajTablePut(gvv->Synonyms,
		   (void *) ajStrNewRef(source),
		   (void *) list);
    }
    
    ajListPushAppend(list, (void *) ajStrNewRef(synonym));
    
    return ajTrue;
}




/* @func ensGVVariationAddGVAllele ********************************************
**
** Add an Ensembl Genetic Variation Allele to an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::add_Allele
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [u] gva [EnsPGVAllele] Ensembl Genetic Variation Allele
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationAddGVAllele(EnsPGVVariation gvv, EnsPGVAllele gva)
{
    if(!gvv)
	return ajFalse;
    
    if(!gva)
	return ajFalse;
    
    ajListPushAppend(gvv->GVAlleles, (void *) ensGVAlleleNewRef(gva));
    
    return ajTrue;
}




/* @func ensGVVariationValidationStateFromStr *********************************
**
** Convert an AJAX String into an Ensembl Genetic Variaton Variation
** validation state element.
**
** @param [r] state [const AjPStr] Validation state string
**
** @return [AjEnum] Ensembl Genetic Variation validation state element or
**                  ensEGVVariationValidationStateNULL
** @@
******************************************************************************/

AjEnum ensGVVariationValidationStateFromStr(const AjPStr state)
{
    register ajint i = 0;
    
    AjEnum estate = ensEGVVariationValidationStateNULL;
    
    for(i = 1; gvVariationValidationState[i]; i++)
	if(ajStrMatchC(state, gvVariationValidationState[i]))
	    estate = i;
    
    if(!estate)
	ajDebug("ensGVVariationValidationStateFromStr encountered "
		"unexpected string '%S'.\n", state);
    
    return estate;
}




/* @func ensGVVariationValidationStateToChar **********************************
**
** Convert an Ensembl Genetic Variation Variation validation state element
** into a C-type (char*) string.
**
** @param [r] state [const AjEnum] Ensembl Genetic Variation Variation
**                                 validation state enumerator
**
** @return [const char*] Ensembl Genetic Variation Variation validation state
**                       C-type (char*) string
** @@
******************************************************************************/

const char* ensGVVariationValidationStateToChar(const AjEnum state)
{
    register ajint i = 0;
    
    if(!state)
	return NULL;
    
    for(i = 1; gvVariationValidationState[i] && (i < state); i++);
    
    if(!gvVariationValidationState[i])
	ajDebug("ensGVVariationValidationStateToChar encountered an "
		"out of boundary error on gender %d.\n", state);
    
    return gvVariationValidationState[i];
}




/* @func ensGVVariationFetchAllSynonyms ***************************************
**
** Fetch all synonyms of an Ensembl Genetic Variation Variation.
**
** @param [r] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [r] source [const AjPStr] Source
** @param [u] synonyms [AjPList] AJAX List of synonym AJAX Strings
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationFetchAllSynonyms(EnsPGVVariation gvv,
                                      const AjPStr source,
                                      AjPList synonyms)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    AjIList iter = NULL;
    AjPList list = NULL;
    
    AjPStr synonym = NULL;
    
    if(!gvv)
	return ajFalse;
    
    if(!synonyms)
	return ajFalse;
    
    if(gvv->Synonyms)
    {
	if(source)
	{
	    list = (AjPList)
	    ajTableFetch(gvv->Synonyms, (const void *) source);
	    
	    iter = ajListIterNew(list);
	    
	    while(!ajListIterDone(iter))
	    {
		synonym = (AjPStr) ajListIterGet(iter);
		
		if(synonym)
		    ajListPushAppend(synonyms,
				     (void *) ajStrNewRef(synonym));
	    }
	}
	else
	{
	    ajTableToarray(gvv->Synonyms, &keyarray, &valarray);
	    
	    for(i = 0; keyarray[i]; i++)
	    {
		iter = ajListIterNew((AjPList) valarray[i]);
		
		while(!ajListIterDone(iter))
		{
		    synonym = (AjPStr) ajListIterGet(iter);
		    
		    if(synonym)
			ajListPushAppend(synonyms,
					 (void *) ajStrNewRef(synonym));
		}
		
		ajListIterDel(&iter);
	    }
	    
	    AJFREE(keyarray);
	    
	    AJFREE(valarray);
	}
    }
    
    return ajTrue;
}




/* @func ensGVVariationFetchAllSynonymSources *********************************
**
** Fetch all synonym sources of an Ensembl Genetic Variation Variation.
**
** @param [r] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [u] sources [AjPList] AJAX List of source AJAX Strings
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationFetchAllSynonymSources(EnsPGVVariation gvv,
                                            AjPList sources)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    if(!gvv)
	return ajFalse;
    
    if(!sources)
	return ajFalse;
    
    if(gvv->Synonyms)
    {
	ajTableToarray(gvv->Synonyms, &keyarray, &valarray);
	
	for(i = 0; keyarray[i]; i++)
	    ajListPushAppend(sources,
			     (void *) ajStrNewRef((AjPStr) keyarray[i]));
	
	AJFREE(keyarray);
	AJFREE(valarray);
    }
    
    return ajTrue;
}




/* @datasection [EnsPGVVariationAdaptor] Genetic Variation Variation Adaptor
**
** Functions for manipulating Ensembl Genetic Variation Variation Adaptor
** objects
**
** @nam2rule GVVariationAdaptor
**
******************************************************************************/




/* @funcstatic gvVariationAdaptorClearAlleles *********************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Genetic Variation
** Variation Adaptor-internal Genetic Variation Alleles cache. This function
** deletes the unsigned identifier key data and the Ensembl Genetic Variation
** Alleles value data.
**
** @param [u] key [void **] AJAX unsigned integer key data address
** @param [u] value [void **] Ensembl Genetic Variation Allele
**                            value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void gvVariationAdaptorClearAlleles(void **key, void **value, void *cl)
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
    
    ensGVAlleleDel((EnsPGVAllele *) value);
    
    return;
}




/* @funcstatic gvVariationAdaptorClearPopulations *****************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Genetic Variation
** Variation Adaptor-internal Genetic Variation Population cache. This function
** deletes the unsigned identifier key data and the Ensembl Genetic Variation
** Population value data.
**
** @param [u] key [void **] AJAX unsigned integer key data address
** @param [u] value [void **] Ensembl Genetic Variation Population
**                            value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void gvVariationAdaptorClearPopulations(void **key, void **value,
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
    
    ensGVPopulationDel((EnsPGVPopulation *) value);
    
    return;
}




/* @funcstatic gvVariationAdaptorClearSynonyms ********************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Genetic Variation
** Variation Adaptor-internal synonym cache. This function deletes the
** AJAX String key data and the unsigned integer value data.
**
** @param [u] key [void **] AJAX String key data address
** @param [u] value [void **] AJAX unsigned integer value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void gvVariationAdaptorClearSynonyms(void **key, void **value, void *cl)
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
    
    ajStrDel((AjPStr *) value);
    
    AJFREE(*value);
    
    return;
}




/* @funcstatic gvVariationAdaptorFetchAllBySQL ********************************
**
** Fetch all Ensembl Genetic Variation Variation objects via an SQL statement.
**
** @param [r] gvva [EnsPGVVariationAdaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] gvvs [AjPList] AJAX List of Ensembl Genetic Variation
**                           Variation objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvVariationAdaptorFetchAllBySQL(EnsPGVVariationAdaptor gvva,
                                              const AjPStr statement,
                                              AjPList gvvs)
{
    float allelefreq = 0;
    
    ajuint identifier     = 0;
    ajuint alleleid       = 0;
    ajuint allelesampleid = 0;
    ajuint cidentifier    = 0;
    ajuint calleleid      = 0;
    
    ajuint *Pidentifier = NULL;
    
    AjPList vlist = NULL;
    
    AjPSqlStatement sqls = NULL;
    AjISqlRow sqli       = NULL;
    AjPSqlRow sqlr       = NULL;
    
    AjPStr name            = NULL;
    AjPStr vstatus         = NULL;
    AjPStr source          = NULL;
    AjPStr ancestralallele = NULL;
    AjPStr allelestr       = NULL;
    AjPStr moltype         = NULL;
    AjPStr synname         = NULL;
    AjPStr synsource       = NULL;
    AjPStr failed          = NULL;
    AjPStr key             = NULL;
    AjPStr vtoken          = NULL;
    
    AjPStrTok token = NULL;
    
    AjPTable alleles     = NULL;
    AjPTable populations = NULL;
    AjPTable synonyms    = NULL;
    
    EnsPGVAllele gva         = NULL;
    EnsPGVAlleleAdaptor gvaa = NULL;
    
    EnsPGVPopulation gvp         = NULL;
    EnsPGVPopulationAdaptor gvpa = NULL;
    
    EnsPGVVariation gvv = NULL;
    
    if(!gvva)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    if(!gvvs)
	return ajFalse;
    
    alleles = MENSTABLEUINTNEW(0);
    
    populations = MENSTABLEUINTNEW(0);
    
    synonyms = ajTablestrNewLen(0);
    
    gvpa = ensRegistryGetGVPopulationAdaptor(gvva->Adaptor);
    
    sqls = ensDatabaseAdaptorSqlStatementNew(gvva->Adaptor, statement);
    
    sqli = ajSqlRowIterNew(sqls);
    
    while(!ajSqlRowIterDone(sqli))
    {
	identifier     = 0;
	name           = ajStrNew();
	vstatus        = ajStrNew();
	vtoken         = ajStrNew();
	source         = ajStrNew();
	alleleid       = 0;
	allelestr      = ajStrNew();
	allelefreq     = 0;
	allelesampleid = 0;
	moltype        = ajStrNew();
	synname        = ajStrNew();
	synsource      = ajStrNew();
	failed         = ajStrNew();
	
	sqlr = ajSqlRowIterGet(sqli);
	
	ajSqlColumnToUint(sqlr, &identifier);
	ajSqlColumnToStr(sqlr, &name);
	ajSqlColumnToStr(sqlr, &vstatus);
	ajSqlColumnToStr(sqlr, &source);
	ajSqlColumnToStr(sqlr, &ancestralallele);
	ajSqlColumnToUint(sqlr, &alleleid);
	ajSqlColumnToStr(sqlr, &allelestr);
	ajSqlColumnToFloat(sqlr, &allelefreq);
	ajSqlColumnToUint(sqlr, &allelesampleid);
	ajSqlColumnToStr(sqlr, &moltype);
	ajSqlColumnToStr(sqlr, &synname);
	ajSqlColumnToStr(sqlr, &synsource);
	ajSqlColumnToStr(sqlr, &failed);
	
	if(cidentifier != identifier)
	{
	    /* Split the comma-separated list of validation states. */
	    
	    token = ajStrTokenNewC(vstatus, ",");
	    
	    vlist = ajListNew();
	    
	    while(ajStrTokenNextParse(&token, &vtoken))
		ajListPushAppend(vlist, (void *) ajStrNewS(vtoken));
	    
	    if(ajListGetLength(vlist) == 0)
		ajListPushAppend(vlist, (void *) ajStrNew());
	    
	    gvv = ensGVVariationNew(gvva,
				    identifier,
				    name,
				    source,
				    ancestralallele,
				    (AjPTable) NULL, /* synonyms */
				    (AjPList) NULL, /* alleles */
				    vlist,
				    moltype,
				    (AjPStr) NULL, /* fiveflank */
				    (AjPStr) NULL, /* threeflank */
				    failed);
	    
	    ajListPushAppend(gvvs, (void *) gvv);
	    
	    /* Clear the AJAX List of validation states. */
	    
	    while(ajListPop(vlist, (void **) &vtoken))
		ajStrDel(&vtoken);
	    
	    ajListFree(&vlist);
	    
	    cidentifier = identifier;
	}
	
	if(calleleid != alleleid)
	{
	    if(allelesampleid)
	    {
		gvp = (EnsPGVPopulation)
		ajTableFetch(populations, (void *) &allelesampleid);
		
		if(!gvp)
		{
		    ensGVPopulationAdaptorFetchByIdentifier(gvpa,
							    allelesampleid,
							    &gvp);
		    
		    if(gvp)
		    {
			AJNEW0(Pidentifier);
			
			ajTablePut(populations,
				   (void *) Pidentifier,
				   (void *) gvp);
		    }
		}
	    }
	    else
		gvp = (EnsPGVPopulation) NULL;
	    
	    if(alleleid)
	    {
		gva = ensGVAlleleNew(gvaa,
				     alleleid,
				     gvp,
				     allelestr,
				     allelefreq);
		
		ensGVVariationAddGVAllele(gvv, gva);
		
		ensGVAlleleDel(&gva);
		
		calleleid = alleleid;
	    }
	    
	    if(ajStrGetLen(synsource))
	    {
		key = ajFmtStr("%S:%S", synsource, synname);
		
		Pidentifier = (ajuint *)
		    ajTableFetch(synonyms, (const void *) key);
		
		if(Pidentifier)
		    ajStrDel(&key);
		else
		{
		    AJNEW0(Pidentifier);
		    
		    ajTablePut(synonyms, (void *) key, (void *) Pidentifier);
		    
		    ensGVVariationAddSynonym(gvv, synsource, synname);
		}
	    }
	}
	
	ajStrDel(&name);
	ajStrDel(&vstatus);
	ajStrDel(&source);
	ajStrDel(&ancestralallele);
	ajStrDel(&allelestr);
	ajStrDel(&moltype);
	ajStrDel(&synname);
	ajStrDel(&synsource);
	ajStrDel(&failed);
    }
    
    ajSqlRowIterDel(&sqli);
    
    ajSqlStatementDel(&sqls);
    
    /* Clear the AJAX Table of Ensembl Genetic Variation Populations. */
    
    ajTableMapDel(populations, gvVariationAdaptorClearPopulations, NULL);
    
    ajTableFree(&populations);
    
    /* Clear the AJAX Table of Ensembl Genetic Variation Alleles. */
    
    ajTableMapDel(alleles, gvVariationAdaptorClearAlleles, NULL);
    
    ajTableFree(&alleles);
    
    /* Clear the AJAX Table of Synonyms. */
    
    ajTableMapDel(synonyms, gvVariationAdaptorClearSynonyms, NULL);
    
    ajTableFree(&synonyms);
    
    return ajTrue;
}




/* @func ensGVVariationAdaptorNew *********************************************
**
** Default constructor for an Ensembl Genetic Variation Variation Adaptor.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::new
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPGVVariationAdaptor] Ensembl Genetic Variation Variation
**                                  Adaptor or NULL
** @@
******************************************************************************/

EnsPGVVariationAdaptor ensGVVariationAdaptorNew(EnsPDatabaseAdaptor dba)
{
    EnsPGVVariationAdaptor gvva = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(gvva);
    
    gvva->Adaptor = dba;
    
    return gvva;
}




/* @func ensGVVariationAdaptorDel *********************************************
**
** Default destructor for an Ensembl Gentic Variation Variation Adaptor.
**
** @param [d] Pgvva [EnsPGVVariationAdaptor*] Ensembl Genetic Variation
**                                            Variation Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGVVariationAdaptorDel(EnsPGVVariationAdaptor *Pgvva)
{
    if(!Pgvva)
	return;
    
    if(!*Pgvva)
	return;
    
    AJFREE(*Pgvva);

    *Pgvva = NULL;
    
    return;
}




/* @func ensGVVariationAdaptorFetchByIdentifier *******************************
**
** Fetch an Ensembl Genetic Variation Variation via its
** SQL database-internal identifier.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::fetch_by_dbID
** @param [r] gvva [const EnsPGVVariationAdaptor] Ensembl Genetic Variation
**                                                Variation Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pgvv [EnsPGVVariation*] Ensembl Genetic Variation
**                                     Variation address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationAdaptorFetchByIdentifier(EnsPGVVariationAdaptor gvva,
                                              ajuint identifier,
                                              EnsPGVVariation *Pgvv)
{
    AjPList variations = NULL;
    
    AjPStr statement = NULL;
    
    EnsPGVVariation gvv = NULL;
    
    if(!gvva)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pgvv)
	return ajFalse;
    
    statement = ajFmtStr("SELECT "
			 "variation.variation_id, "
			 "variation.name, "
			 "variation.validation_status, "
			 "source1.name, "
			 "variation.ancestral_allele, "
			 "allele.allele_id, "
			 "allele.allele, "
			 "allele.frequency, "
			 "allele.sample_id, "
			 "variation_synonym.moltype, "
			 "variation_synonym.name, "
			 "source2.name, "
			 "failed_description.description "
			 "FROM "
			 "(variation, source source1) "
			 "LEFT JOIN "
			 "allele "
			 "ON "
			 "variation.variation_id = allele.variation_id "
			 "LEFT JOIN "
			 "variation_synonym "
			 "ON "
			 "v.variation_id = variation_synonym.variation_id "
			 "LEFT JOIN "
			 "source source2 "
			 "ON "
			 "variation_synonym.source_id = source2.source_id "
			 "LEFT JOIN "
			 "failed_variation "
			 "ON "
			 "variation.variation_id = "
			 "failed_variation.variation_id "
			 "LEFT JOIN "
			 "failed_description "
			 "ON "
			 "failed_variation.failed_description_id = "
			 "failed_description.failed_description_id "
			 "WHERE "
			 "variation.source_id = source1.source_id "
			 "AND "
			 "variation.variation_id = %u",
			 identifier);
    
    variations = ajListNew();
    
    gvVariationAdaptorFetchAllBySQL(gvva, statement, variations);
    
    if(ajListGetLength(variations) > 1)
	ajWarn("ensGVVariationAdaptorFetchByIdentifier go more than one "
	       "Ensembl Genetic Variation Variation for identifier %u.",
	       identifier);
    
    ajListPop(variations, (void **) Pgvv);
    
    while(ajListPop(variations, (void *) &gvv))
	ensGVVariationDel(&gvv);
    
    ajListFree(&variations);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensGVVariationAdaptorFetchByName *************************************
**
** Fetch an Ensembl Genetic Variation Variation via its name.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::fetch_by_name
** @param [r] gvva [const EnsPGVVariationAdaptor] Ensembl Genetic Variation
**                                                Variation Adaptor
** @param [r] name [const AjPStr] Name
** @param [rN] source [const AjPStr] Source
** @param [wP] Pgvv [EnsPGVVariation*] Ensembl Genetic Variation
**                                     Variation address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationAdaptorFetchByName(EnsPGVVariationAdaptor gvva,
                                        const AjPStr name,
                                        const AjPStr source,
                                        EnsPGVVariation *Pgvv)
{
    char *Ptxtname   = NULL;
    char *Ptxtsource = NULL;
    
    AjPList gvvs = NULL;
    
    AjPStr extra     = NULL;
    AjPStr statement = NULL;
    
    EnsPGVVariation gvv = NULL;
    
    if(!gvva)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    if(!Pgvv)
	return ajFalse;
    
    ensDatabaseAdaptorEscapeCS(gvva->Adaptor, &Ptxtname, name);
    
    if(source && ajStrGetLen(source))
    {
	ensDatabaseAdaptorEscapeCS(gvva->Adaptor, &Ptxtsource, source);
	
	extra = ajFmtStr("AND source1.name = '%s' ", Ptxtsource);
	
	ajCharDel(&Ptxtsource);
    }
    else
	extra = ajStrNew();
    
    statement = ajFmtStr("SELECT "
			 "variation.variation_id, "
			 "variation.name, "
			 "variation.validation_status, "
			 "source1.name, "
			 "variation.ancestral_allele, "
			 "allele.allele_id, "
			 "allele.allele, "
			 "allele.frequency, "
			 "allele.sample_id, "
			 "variation_synonym.moltype, "
			 "variation_synonym.name, "
			 "source2.name, "
			 "failed_description.description "
			 /*
			  "FROM "
			  "variation, "
			  "source source1, "
			  "source source2, "
			  "allele, "
			  "variation_synonym "
			  */
			 "FROM "
			 "(variation, source source1) "
			 "LEFT JOIN "
			 "allele "
			 "ON "
			 "variation.variation_id = allele.variation_id "
			 "LEFT JOIN "
			 "variation_synonym "
			 "ON "
			 "variation.variation_id = "
			 "variation_synonym.variation_id "
			 "LEFT JOIN "
			 "source source2 "
			 "ON "
			 "variation_synonym.source_id = source2.source_id "
			 "LEFT JOIN "
			 "failed_variation "
			 "ON "
			 "variation.variation_id = "
			 "failed_variation.variation_id "
			 "LEFT JOIN "
			 "failed_description "
			 "ON "
			 "failed_variation.failed_description_id = "
			 "failed_description.failed_description_id "
			 /*
			  "WHERE "
			  "variation.variation_id = "
			  "allele.variation_id "
			  */
			 /*
			  "AND "
			  "variation.variation_id = "
			  "variation_synonym.variation_id "
			  */
			 "WHERE "
			 "variation.source_id = source1.source_id "
			 /*
			  "AND "
			  "variation_synonym.source_id = "
			  "source2.source_id "
			  */
			 "AND "
			 "variation.name = '%s' "
			 "%S "
			 "ORDER BY "
			 "allele.allele_id",
			 Ptxtname,
			 extra);
			 
    gvvs = ajListNew();
    
    gvVariationAdaptorFetchAllBySQL(gvva, statement, gvvs);
    
    ajStrDel(&statement);
    
    if(ajListGetLength(gvvs) > 1)
	ajWarn("ensGVVariationAdaptorFetchByIdentifier go more than one "
	       "Ensembl Genetic Variation Variation for name '%S' and "
	       "source '%S'.", name, source);
    
    if(ajListGetLength(gvvs) == 0)
    {
	statement = ajFmtStr("SELECT "
			     "variation.variation_id, "
			     "variation.name, "
			     "variation.validation_status, "
			     "source1.name, "
			     "variation.ancestral_allele, "
			     "allele.allele_id, "
			     "allele.allele, "
			     "allele.frequency, "
			     "allele.sample_id, "
			     "variation_synonym1.moltype, "
			     "variation_synonym2.name, "
			     "source2.name, "
			     "NULL "
			     "FROM "
			     "variation, "
			     "source source1, "
			     "source source2, "
			     "allele, "
			     "variation_synonym variation_synonym1, "
			     "variation_synonym variation_synonym2 "
			     "WHERE "
			     "variation.variation_id = allele.variation_id "
			     "AND "
			     "variation.variation_id = "
			     "variation_synonym1.variation_id "
			     "AND "
			     "variation.variation_id = "
			     "variation_synonym2.variation_id "
			     "AND "
			     "variation.source_id = "
			     "source1.source_id "
			     "AND "
			     "variation_synonym2.source_id = "
			     "source2.source_id "
			     "AND "
			     "variation_synonym1.name = '%s' "
			     "%S "
			     "ORDER BY "
			     "allele.allele_id",
			     Ptxtname,
			     extra
			     );
			     
	gvVariationAdaptorFetchAllBySQL(gvva, statement, gvvs);
	
	ajStrDel(&statement);
    }
    
    if(ajListGetLength(gvvs) > 1)
	ajWarn("ensGVVariationAdaptorFetchByIdentifier go more than one "
	       "Ensembl Genetic Variation Variation for name '%S' and "
	       "source '%S'.", name, source);
    
    ajListPop(gvvs, (void **) Pgvv);
    
    while(ajListPop(gvvs, (void *) &gvv))
	ensGVVariationDel(&gvv);
    
    ajListFree(&gvvs);
    
    ajCharDel(&Ptxtname);
    
    ajStrDel(&extra);
    
    return ajTrue;
}




/* @func ensGVVariationAdaptorFetchAllBySource ********************************
**
** Fetch all Ensembl Genetic Variation Variations via a source.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variations before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::VariationAdaptor::fetch_all_by_source
** @param [r] gvva [const EnsPGVVariationAdaptor] Ensembl Genetic Variation
**                                                Variation Adaptor
** @param [r] source [const AjPStr] Source
** @param [r] primary [AjBool] Primary
** @param [u] gvvs [AjPList] AJAX List of Ensembl Genetic Variation Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationAdaptorFetchAllBySource(EnsPGVVariationAdaptor gvva,
                                             const AjPStr source,
                                             AjBool primary,
                                             AjPList gvvs)
{
    char *Ptxtsource = NULL;
    
    AjPStr statement = NULL;
    
    if(!gvva)
	return ajFalse;
    
    if(!source)
	return ajFalse;
    
    if(!gvvs)
	return ajFalse;
    
    ensDatabaseAdaptorEscapeCS(gvva->Adaptor, &Ptxtsource, source);
    
    statement = ajFmtStr("SELECT "
			 "variation.variation_id, "
			 "variation.name, "
			 "variation.validation_status, "
			 "source1.name, "
			 "variation.ancestral_allele, "
			 "allele.allele_id, "
			 "allele.allele, "
			 "allele.frequency, "
			 "allele.sample_id, "
			 "variation_synonym.moltype, "
			 "variation_synonym.name, "
			 "source2.name, "
			 "failed_description.description "
			 "FROM "
			 "(variation, source source1) "
			 "LEFT JOIN "
			 "allele "
			 "ON "
			 "variation.variation_id = "
			 "allele.variation_id "
			 "LEFT JOIN "
			 "variation_synonym "
			 "ON "
			 "variation.variation_id = "
			 "variation_synonym.variation_id "
			 "LEFT JOIN "
			 "source source2 "
			 "ON "
			 "variation_synonym.source_id = "
			 "source2.source_id "
			 "LEFT JOIN "
			 "failed_variation "
			 "ON "
			 "variation.variation_id = "
			 "failed_variation.variation_id "
			 "LEFT JOIN "
			 "failed_description "
			 "ON "
			 "failed_variation.failed_description_id = "
			 "failed_description.failed_description_id "
			 "WHERE "
			 "variation.source_id = "
			 "source1.source_id "
			 "AND "
			 "source1.name = '%s'",
			 Ptxtsource);
    
    gvVariationAdaptorFetchAllBySQL(gvva, statement, gvvs);
    
    ajStrDel(&statement);
    
    if(!primary)
    {
	/*
	** We need to include variation_synonym as well,
	** where the variation was merged with NCBI dbSNP.
	*/
	
	statement = ajFmtStr("SELECT "
			     "variation.variation_id, "
			     "variation.name, "
			     "variation.validation_status, "
			     "source1.name, "
			     "variation.ancestral_allele, "
			     "allele.allele_id, "
			     "allele.allele, "
			     "allele.frequency, "
			     "allele.sample_id, "
			     "variation_synonym1.moltype, "
			     "variation_synonym1.name, "
			     "source2.name, "
			     "NULL "
			     "FROM "
			     "(variation, "
			     "source source1, "
			     "source source2, "
			     "variation_synonym variation_synonym1) "
			     "LEFT JOIN "
			     "allele "
			     "ON "
			     "variation.variation_id = "
			     "allele.variation_id "
			     "WHERE "
			     "variation.variation_id = "
			     "variation_synonym1.variation_id "
			     "AND "
			     "variation.source_id = "
			     "source1.source_id "
			     "AND "
			     "variation_synonym1.source_id = "
			     "source2.source_id "
			     "AND "
			     "source2.name = '%s' "
			     "ORDER BY "
			     "variation.variation_id",
			     Ptxtsource
			     );
	
	/* need to merge both lists, trying to avoid duplicates. */
	
	gvVariationAdaptorFetchAllBySQL(gvva, statement, gvvs);
	
	ajStrDel(&statement);
    }
    
    ajCharDel(&Ptxtsource);
    
    return ajTrue;
}




/* @datasection [EnsPGVVariationFeature] Genetic Variation Variation Feature **
**
** Functions for manipulating Ensembl Genetic Variation Variation Feature
** objects
**
** @nam2rule GVVariationFeature
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Variation Feature by
** pointer. It is the responsibility of the user to first destroy any previous
** Genetic Variation Variation Feature. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGVVariationFeature]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                              Variation Feature
** @argrule Ref object [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                              Variation Feature
**
** @valrule * [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                     Variation Feature
**
** @fcategory new
******************************************************************************/




/* @func ensGVVariationFeatureNew *********************************************
**
** Default constructor for an Ensembl Genetic Variation Variation Feature.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvvfa [EnsPGVVariationFeatureAdaptor] Ensembl Genetic Variation
**                                                  Variation Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::VariationFeature::new
** @param [u] feature [EnsPFeature] Ensembl Feature
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
** @param [u] name [AjPStr] Name
** @param [u] source [AjPStr] Source
** @param [u] validation [AjPStr] Validation code
** @param [u] consequence [AjPStr] Consequence type
** @param [r] mapweight [ajuint] Map weight
**
** @return [EnsPGVVariationFeature] Ensembl Genetic Variation Variation Feature
** @@
******************************************************************************/

EnsPGVVariationFeature ensGVVariationFeatureNew(
    EnsPGVVariationFeatureAdaptor gvvfa,
    ajuint identifier,
    EnsPFeature feature,
    EnsPGVVariation gvv,
    AjPStr name,
    AjPStr source,
    AjPStr validation,
    AjPStr consequence,
    ajuint mapweight)
{
    EnsPGVVariationFeature gvvf = NULL;
    
    /* FIXME: Input restrictions are missing. */
    
    AJNEW0(gvvf);
    
    gvvf->Use = 1;
    
    gvvf->Identifier = identifier;
    
    gvvf->Adaptor = gvvfa;
    
    gvvf->Feature = ensFeatureNewRef(feature);
    
    gvvf->GVVariation = ensGVVariationNewRef(gvv);
    
    if(name)
	gvvf->Name = ajStrNewRef(name);
    
    if(source)
	gvvf->Source = ajStrNewRef(source);
    
    if(validation)
	gvvf->ValidationCode = ajStrNewRef(validation);
    
    if(consequence)
	gvvf->ConsequenceType = ajStrNewRef(consequence);
    
    gvvf->MapWeight = mapweight;
    
    return gvvf;
}




/* @func ensGVVariationFeatureNewObj ******************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                  Variation Feature
**
** @return [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                  Variation Feature or NULL
** @@
******************************************************************************/

EnsPGVVariationFeature ensGVVariationFeatureNewObj(
    const EnsPGVVariationFeature object)
{
    EnsPGVVariationFeature gvvf = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(gvvf);
    
    gvvf->Use = 1;
    
    gvvf->Identifier = object->Identifier;
    
    gvvf->Adaptor = object->Adaptor;
    
    gvvf->Feature = ensFeatureNewRef(object->Feature);
    
    gvvf->GVVariation = ensGVVariationNewRef(object->GVVariation);
    
    if(object->Name)
	gvvf->Name = ajStrNewRef(object->Name);
    
    if(object->Source)
	gvvf->Source = ajStrNewRef(object->Source);
    
    if(object->ValidationCode)
	gvvf->Source = ajStrNewRef(object->ValidationCode);
    
    if(object->ConsequenceType)
	gvvf->Source = ajStrNewRef(object->ConsequenceType);
    
    gvvf->MapWeight = object->MapWeight;
    
    return gvvf;
}




/* @func ensGVVariationFeatureNewRef ******************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvvf [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                          Variation Feature
**
** @return [EnsPGVVariationFeature] Ensembl Genetic Variation Variation Feature
** @@
******************************************************************************/

EnsPGVVariationFeature ensGVVariationFeatureNewRef(EnsPGVVariationFeature gvvf)
{
    if(!gvvf)
	return NULL;
    
    gvvf->Use++;
    
    return gvvf;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Genetic Variation Variation Feature.
**
** @fdata [EnsPGVVariationFeature]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Variation Feature object
**
** @argrule * Pgvvf [EnsPGVVariationFeature*] Genetic Variation
**                                            Variation Feature object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGVVariationFeatureDel *********************************************
**
** Default destructor for an Ensembl Genetic Variation Variation Feature.
**
** @param [d] Pgvvf [EnsPGVVariationFeature*] Ensembl Genetic Variation
**                                            Variation Feature address
**
** @return [void]
** @@
******************************************************************************/

void ensGVVariationFeatureDel(EnsPGVVariationFeature *Pgvvf)
{
    EnsPGVVariationFeature pthis = NULL;
    
    /*
     ajDebug("ensGVVariationFeatureDel\n"
	     "  *Pgvvf %p\n",
	     *Pgvvf);
     
     ensGVVariationFeatureTrace(*Pgvvf, 1);
     */
    
    if(!Pgvvf)
	return;
    
    if(!*Pgvvf)
	return;

    pthis = *Pgvvf;
    
    pthis->Use--;
    
    if(pthis->Use)
    {
	*Pgvvf = NULL;
	
	return;
    }
    
    ensFeatureDel(&pthis->Feature);
    
    ensGVVariationDel(&pthis->GVVariation);
    
    ajStrDel(&pthis->Name);
    ajStrDel(&pthis->Source);
    ajStrDel(&pthis->ValidationCode);
    ajStrDel(&pthis->ConsequenceType);
    
    AJFREE(pthis);

    *Pgvvf = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Variation Feature object.
**
** @fdata [EnsPGVVariationFeature]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Variation Feature attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation
**                      Variation Feature Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetFeature Return the Ensembl Feature
** @nam4rule GetGVVariation Return the Ensembl Genetic Variation Variation
** @nam4rule GetName Return the name
** @nam4rule GetSource Return the source
** @nam4rule GetValidationCode Return the validation code
** @nam4rule GetConsequenceType Return the consequence type
** @nam4rule GetMapWeight Return the map weight
**
** @argrule * gvvf [const EnsPGVVariationFeature] Genetic Variation
**                                                Variation Feature
**
** @valrule Adaptor [EnsPGVVariationFeatureAdaptor] Ensembl Genetic Variation
**                                                  Variation Feature Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Feature [EnsPFeature] Ensembl Feature
** @valrule GVVariation [EnsPGVVariation] Ensembl Genetic Variation Variation
** @valrule Name [AjPStr] Name
** @valrule Source [AjPStr] Source
** @valrule ValidationCode [AjPStr] Validation code
** @valrule ConsequenceType [AjPStr] Consequence type
** @valrule MapWeight [ajuint] Map weight
**
** @fcategory use
******************************************************************************/




/* @func ensGVVariationFeatureGetAdaptor **************************************
**
** Get the Ensembl Genetic Variation Variation Feature Adaptor element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [EnsPGVVariationFeatureAdaptor] Ensembl Genetic Variation
**                                         Variation Feature Adaptor
** @@
******************************************************************************/

EnsPGVVariationFeatureAdaptor ensGVVariationFeatureGetAdaptor(
    const EnsPGVVariationFeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->Adaptor;
}




/* @func ensGVVariationFeatureGetIdentifier ***********************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGVVariationFeatureGetIdentifier(const EnsPGVVariationFeature gvvf)
{
    if(!gvvf)
        return 0;
    
    return gvvf->Identifier;
}




/* @func ensGVVariationFeatureGetFeature **************************************
**
** Get the Ensembl Feature element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

EnsPFeature ensGVVariationFeatureGetFeature(const EnsPGVVariationFeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->Feature;
}




/* @func ensGVVariationFeatureGetGVVariation **********************************
**
** Get the Ensembl Genetic Variation Variation element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [EnsPGVVariation] Ensembl Genetic Variation Variation
** @@
******************************************************************************/

EnsPGVVariation ensGVVariationFeatureGetGVVariation(
    const EnsPGVVariationFeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->GVVariation;
}




/* @func ensGVVariationFeatureGetName *****************************************
**
** Get the name element of an Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensGVVariationFeatureGetName(const EnsPGVVariationFeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->Name;
}




/* @func ensGVVariationFeatureGetSource ***************************************
**
** Get the source element of an Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [AjPStr] Source
** @@
******************************************************************************/

AjPStr ensGVVariationFeatureGetSource(const EnsPGVVariationFeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->Source;
}




/* @func ensGVVariationFeatureGetValidationCode *******************************
**
** Get the validation code element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [AjPStr] Validation code
** @@
******************************************************************************/

AjPStr ensGVVariationFeatureGetValidationCode(const EnsPGVVariationFeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->ValidationCode;
}




/* @func ensGVVariationFeatureGetConsequenceType ******************************
**
** Get the consequence type element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [AjPStr] Consequence type
** @@
******************************************************************************/

AjPStr ensGVVariationFeatureGetConsequenceType(
    const EnsPGVVariationFeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->ConsequenceType;
}




/* @func ensGVVariationFeatureGetMapWeight ************************************
**
** Get the map weight element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [ajuint] Map weight
** @@
******************************************************************************/

ajuint ensGVVariationFeatureGetMapWeight(const EnsPGVVariationFeature gvvf)
{
    if(!gvvf)
        return 0;
    
    return gvvf->MapWeight;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an
** Ensembl Genetic Variation Variation Feature object.
**
** @fdata [EnsPGVVariationFeature]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Variation Feature
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation
**                      Variation Feature Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetFeature Set the Ensembl Feature
** @nam4rule SetGVVariation Set the Ensembl Genetic Variation Variation
** @nam4rule SetName Set the name
** @nam4rule SetSource Set the source
** @nam4rule SetValidationCode Set the validation code
** @nam4rule SetConsequenceType Set the consequence type
** @nam4rule SetMapWeight Set the map weight
**
** @argrule * gvvf [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                          Variation Feature object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGVVariationFeatureSetAdaptor **************************************
**
** Set the Ensembl Genetic Variation Variation Feature Adaptor element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [r] gvvfa [EnsPGVVariationFeatureAdaptor] Ensembl Genetic Variation
**                                                  Variation Feature Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationFeatureSetAdaptor(EnsPGVVariationFeature gvvf,
                                       EnsPGVVariationFeatureAdaptor gvvfa)
{
    if(!gvvf)
        return ajFalse;
    
    gvvf->Adaptor = gvvfa;
    
    return ajTrue;
}




/* @func ensGVVariationSetIdentifier ******************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationFeatureSetIdentifier(EnsPGVVariationFeature gvvf,
                                          ajuint identifier)
{
    if(!gvvf)
        return ajFalse;
    
    gvvf->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGVVariationFeatureSetFeature **************************************
**
** Set the Ensembl Feature element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] feature [EnsPFeature] Ensembl Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationFeatureSetFeature(EnsPGVVariationFeature gvvf,
                                       EnsPFeature feature)
{
    if(!gvvf)
        return ajFalse;
    
    ensFeatureDel(&gvvf->Feature);
    
    gvvf->Feature = ensFeatureNewRef(feature);
    
    return ajTrue;
}




/* @func ensGVVariationFeatureSetGVVariation **********************************
**
** Set the Ensembl Gentic Variation Variation element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] gvv [EnsPGVVariation] Ensembl Genetic Variation Variation
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationFeatureSetGVVariation(EnsPGVVariationFeature gvvf,
                                           EnsPGVVariation gvv)
{
    if(!gvvf)
        return ajFalse;
    
    ensGVVariationDel(&gvvf->GVVariation);
    
    gvvf->GVVariation = ensGVVariationNewRef(gvv);
    
    return ajTrue;
}




/* @func ensGVVariationFeatureSetName *****************************************
**
** Set the name element of an Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationFeatureSetName(EnsPGVVariationFeature gvvf, AjPStr name)
{
    if(!gvvf)
        return ajFalse;
    
    ajStrDel(&gvvf->Name);
    
    if(name)
	gvvf->Name = ajStrNewRef(name);
    
    return ajTrue;
}




/* @func ensGVVariationFeatureSetSource ***************************************
**
** Set the source element of an Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] source [AjPStr] Source
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationFeatureSetSource(EnsPGVVariationFeature gvvf,
                                      AjPStr source)
{
    if(!gvvf)
        return ajFalse;
    
    ajStrDel(&gvvf->Source);
    
    if(source)
	gvvf->Name = ajStrNewRef(source);
    
    return ajTrue;
}




/* @func ensGVVariationFeatureSetValidationCode *******************************
**
** Set the validation code element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] validationcode [AjPStr] Validation code
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationFeatureSetValidationCode(EnsPGVVariationFeature gvvf,
                                              AjPStr validationcode)
{
    if(!gvvf)
        return ajFalse;
    
    ajStrDel(&gvvf->ValidationCode);
    
    if(validationcode)
	gvvf->ValidationCode = ajStrNewRef(validationcode);
    
    return ajTrue;
}




/* @func ensGVVariationFeatureSetConsequenceType ******************************
**
** Set the consequence type element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] consequencetype [AjPStr] Consequence type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationFeatureSetConsequenceType(EnsPGVVariationFeature gvvf,
                                               AjPStr consequencetype)
{
    if(!gvvf)
        return ajFalse;
    
    ajStrDel(&gvvf->ConsequenceType);
    
    if(consequencetype)
	gvvf->ConsequenceType = ajStrNewRef(consequencetype);
    
    return ajTrue;
}




/* @func ensGVVariationFeatureSetMapWeight ************************************
**
** Set the map weight element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGVVariationFeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [r] mapweight [ajuint] Map weight
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationFeatureSetMapWeight(EnsPGVVariationFeature gvvf,
                                         ajuint mapweight)
{
    if(!gvvf)
        return ajFalse;
    
    gvvf->MapWeight = mapweight;
    
    return ajTrue;
}




/* @func ensGVVariationFeatureGetMemSize **************************************
**
** Get the memory size in bytes of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGVVariationFeatureGetMemSize(const EnsPGVVariationFeature gvvf)
{
    ajuint size = 0;
    
    if(!gvvf)
	return 0;
    
    size += (ajuint) sizeof (EnsOGVVariationFeature);
    
    size += ensFeatureGetMemSize(gvvf->Feature);
    
    size += ensGVVariationGetMemSize(gvvf->GVVariation);
    
    if(gvvf->Name)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvvf->Name);
    }
    
    if(gvvf->Source)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvvf->Source);
    }
    
    if(gvvf->ValidationCode)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvvf->ValidationCode);
    }
    
    if(gvvf->ConsequenceType)
    {
	size += (ajuint) sizeof (AjOStr);
	
	size += ajStrGetRes(gvvf->ConsequenceType);
    }
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an
** Ensembl Genetic Variation Variation Feature object.
**
** @fdata [EnsPGVVariationFeature]
** @nam3rule Trace Report Ensembl Genetic Variation Variation Feature elements
**                 to debug file
**
** @argrule Trace gvvf [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                    Variation Feature
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGVVariationFeatureTrace *******************************************
**
** Trace an Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGVVariationFeature] Ensembl Genetic Variation
**                                                Variation Feature
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGVVariationFeatureTrace(const EnsPGVVariationFeature gvvf,
                                  ajuint level)
{
    AjPStr indent = NULL;
    
    if(!gvvf)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("ensGVVariationFeatureTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Feature %p\n"
	    "%S  GVVariation %p\n"
	    "%S  Name '%S'\n"
	    "%S  Source '%S'\n"
	    "%S  ValidationCode '%S'\n"
	    "%S  ConsequenceType '%S'\n"
	    "%S  MapWeight %u\n",
	    indent, gvvf,
	    indent, gvvf->Use,
	    indent, gvvf->Identifier,
	    indent, gvvf->Adaptor,
	    indent, gvvf->Feature,
	    indent, gvvf->GVVariation,
	    indent, gvvf->Name,
	    indent, gvvf->Source,
	    indent, gvvf->ValidationCode,
	    indent, gvvf->ConsequenceType,
	    indent, gvvf->MapWeight);
    
    ajStrDel(&indent);
    
    ensFeatureTrace(gvvf->Feature, level + 1);
    
    ensGVVariationTrace(gvvf->GVVariation, level + 1);
    
    return ajTrue;
}
