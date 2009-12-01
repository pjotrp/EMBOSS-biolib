/******************************************************************************
** @source Ensembl Genetic Variation functions
**
** @author Copyright (C) 1999 Ensembl Developers
** @author Copyright (C) 2006 Michael K. Schuster
** @modified 2009 by Alan Bleasby for incorporation into EMBOSS core
** @version $Revision: 1.4 $
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

extern EnsPMetainformationadaptor
ensRegistryGetMetainformationadaptor(EnsPDatabaseadaptor dba);

extern EnsPGvindividualadaptor
ensRegistryGetGvindividualadaptor(EnsPDatabaseadaptor dba);

extern EnsPGvpopulationadaptor
ensRegistryGetGvpopulationadaptor(EnsPDatabaseadaptor dba);

extern EnsPGvvariationadaptor
ensRegistryGetGvvariationadaptor(EnsPDatabaseadaptor dba);

static AjBool gvIndividualadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblymapper am,
                                               EnsPSlice slice,
                                               AjPList gvis);

static AjBool gvPopulationadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblymapper am,
                                               EnsPSlice slice,
                                               AjPList gvps);

static void gvVariationadaptorClearAlleles(void **key, void **value, void *cl);

static void gvVariationadaptorClearPopulations(void **key, void **value,
                                               void *cl);

static void gvVariationadaptorClearSynonyms(void **key, void **value, void *cl);

static AjBool gvVariationadaptorFetchAllBySQL(EnsPGvvariationadaptor gvva,
                                              const AjPStr statement,
                                              AjPList gvvs);




/* @filesection ensvariation **************************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPGvvsample] Genetic Variation Sample ***********************
**
** Functions for manipulating Ensembl Genetic Variation Sample objects
**
** @cc Bio::EnsEMBL::Variation::Sample CVS Revision: 1.2
**
** @nam2rule Gvvsample
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Sample by pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Sample. The target pointer does not need to be initialised
** to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvvsample]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvvsample] Ensembl Genetic Variation Sample
** @argrule Ref object [EnsPGvvsample] Ensembl Genetic Variation Sample
**
** @valrule * [EnsPGvvsample] Ensembl Genetic Variation Sample
**
** @fcategory new
******************************************************************************/




/* @func ensGvvsampleNew *******************************************************
**
** Default constructor for an Ensembl Genetic Variation Sample.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvsa [EnsPGvvsampleadaptor] Ensembl Genetic Variation
**                                       Sample Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Sample::new
** @param [u] name [AjPStr] Name
** @param [u] description [AjPStr] Description
** @param [r] size [ajuint] Size
**
** @return [EnsPGvvsample] Ensembl Genetic Variation Sample or NULL
** @@
******************************************************************************/

EnsPGvvsample ensGvvsampleNew(EnsPGvvsampleadaptor gvsa,
                            ajuint identifier,
                            AjPStr name,
                            AjPStr description,
                            ajuint size)
{
    EnsPGvvsample gvs = NULL;
    
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




/* @func ensGvvsampleNewObj ****************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvvsample] Ensembl Genetic Variation Sample
**
** @return [EnsPGvvsample] Ensembl Genetic Variation Sample or NULL
** @@
******************************************************************************/

EnsPGvvsample ensGvvsampleNewObj(const EnsPGvvsample object)
{
    EnsPGvvsample gvs = NULL;
    
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




/* @func ensGvvsampleNewRef ****************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvs [EnsPGvvsample] Ensembl Genetic Variation Sample
**
** @return [EnsPGvvsample] Ensembl Genetic Variation Sample
** @@
******************************************************************************/

EnsPGvvsample ensGvvsampleNewRef(EnsPGvvsample gvs)
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
** @fdata [EnsPGvvsample]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Sample object
**
** @argrule * Pgvs [EnsPGvvsample*] Genetic Variation Sample object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvvsampleDel *******************************************************
**
** Default destructor for an Ensembl Genetic Variation Sample.
**
** @param [d] Pgvs [EnsPGvvsample*] Ensembl Genetic Variation Sample address
**
** @return [void]
** @@
******************************************************************************/

void ensGvvsampleDel(EnsPGvvsample *Pgvs)
{
    EnsPGvvsample pthis = NULL;
    
    /*
     ajDebug("ensGvvsampleDel\n"
	     "  *Psample %p\n",
	     *Pgvs);
     
     ensGvvsampleTrace(*Pgvs, 1);
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
** @fdata [EnsPGvvsample]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Sample attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Sample Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetName Return the name
** @nam4rule GetDescription Return the description
** @nam4rule GetSize Return the size
**
** @argrule * gvs [const EnsPGvvsample] Genetic Variation Sample
**
** @valrule Adaptor [EnsPGvvsampleadaptor] Ensembl Genetic Variation
**                                        Sample Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Name [AjPStr] Name
** @valrule Description [AjPStr] Description
**
** @fcategory use
******************************************************************************/




/* @func ensGvvsampleGetAdaptor ************************************************
**
** Get the Ensembl Genetic Variation Sample Adaptor element of an
** Ensembl Genetic Variation Sample.
**
** @param [r] gvs [const EnsPGvvsample] Ensembl Genetic Variation Sample
**
** @return [EnsPGvvsampleadaptor] Ensembl Genetic Variation Sample Adaptor
** @@
******************************************************************************/

EnsPGvvsampleadaptor ensGvvsampleGetAdaptor(const EnsPGvvsample gvs)
{
    if(!gvs)
        return NULL;
    
    return gvs->Adaptor;
}




/* @func ensGvvsampleGetIdentifier *********************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Sample.
**
** @param [r] gvs [const EnsPGvvsample] Ensembl Genetic Variation Sample
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvvsampleGetIdentifier(const EnsPGvvsample gvs)
{
    if(!gvs)
        return 0;
    
    return gvs->Identifier;
}




/* @func ensGvvsampleGetName ***************************************************
**
** Get the name element of an Ensembl Genetic Variation Sample.
**
** @param  [r] gvs [const EnsPGvvsample] Ensembl Genetic Variation Sample
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensGvvsampleGetName(const EnsPGvvsample gvs)
{
    if(!gvs)
        return NULL;
    
    return gvs->Name;
}




/* @func ensGvvsampleGetDescription ********************************************
**
** Get the description element of an Ensembl Genetic Variation Sample.
**
** @param  [r] gvs [const EnsPGvvsample] Ensembl Genetic Variation Sample
**
** @return [AjPStr] Description
** @@
******************************************************************************/

AjPStr ensGvvsampleGetDescription(const EnsPGvvsample gvs)
{
    if(!gvs)
        return NULL;
    
    return gvs->Description;
}




/* @func ensGvvsampleGetSize ***************************************************
**
** Get the size element of an Ensembl Genetic Variation Sample.
**
** @param  [r] gvs [const EnsPGvvsample] Ensembl Genetic Variation Sample
**
** @return [ajuint] Size
** @@
******************************************************************************/

ajuint ensGvvsampleGetSize(const EnsPGvvsample gvs)
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
** @fdata [EnsPGvvsample]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Sample
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Sample Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetName Set the name
** @nam4rule SetDescription Set the description
** @nam4rule SetSize Set the size
**
** @argrule * gvs [EnsPGvvsample] Ensembl Genetic Variation Sample object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvvsampleSetAdaptor ************************************************
**
** Set the Ensembl Genetic Variation Sample Adaptor element of an
** Ensembl Genetic Variation Sample.
**
** @param [u] gvs [EnsPGvvsample] Ensembl Genetic Variation Sample
** @param [r] gvsa [EnsPGvvsampleadaptor] Ensembl Genetic Variation
**                                       Sample Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvsampleSetAdaptor(EnsPGvvsample gvs, EnsPGvvsampleadaptor gvsa)
{
    if(!gvs)
        return ajFalse;
    
    gvs->Adaptor = gvsa;
    
    return ajTrue;
}




/* @func ensGvvsampleSetIdentifier *********************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Sample.
**
** @param [u] gvs [EnsPGvvsample] Ensembl Genetic Variation Sample
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvsampleSetIdentifier(EnsPGvvsample gvs, ajuint identifier)
{
    if(!gvs)
        return ajFalse;
    
    gvs->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGvvsampleSetName ***************************************************
**
** Set the name element of an Ensembl Genetic Variation Sample.
**
** @param [u] gvs [EnsPGvvsample] Ensembl Genetic Variation Sample
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvsampleSetName(EnsPGvvsample gvs, AjPStr name)
{
    if(!gvs)
        return ajFalse;
    
    ajStrDel(&gvs->Name);
    
    if(name)
	gvs->Name = ajStrNewRef(name);
    
    return ajTrue;
}




/* @func ensGvvsampleSetDescription ********************************************
**
** Set the description element of an Ensembl Genetic Variation Sample.
**
** @param [u] gvs [EnsPGvvsample] Ensembl Genetic Variation Sample
** @param [u] description [AjPStr] Description
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvsampleSetDescription(EnsPGvvsample gvs, AjPStr description)
{
    if(!gvs)
        return ajFalse;
    
    ajStrDel(&gvs->Description);
    
    if(description)
	gvs->Description = ajStrNewRef(description);
    
    return ajTrue;
}




/* @func ensGvvsampleSetSize ***************************************************
**
** Set the size element of an Ensembl Genetic Variation Sample.
**
** @param [u] gvs [EnsPGvvsample] Ensembl Genetic Variation Sample
** @param [r] size [ajuint] Size
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvsampleSetSize(EnsPGvvsample gvs, ajuint size)
{
    if(!gvs)
        return ajFalse;
    
    gvs->Size = size;
    
    return ajTrue;
}




/* @func ensGvvsampleGetMemSize ************************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Sample.
**
** @param [r] gvs [const EnsPGvvsample] Ensembl Genetic Variation Sample
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGvvsampleGetMemSize(const EnsPGvvsample gvs)
{
    ajuint size = 0;
    
    if(!gvs)
	return 0;
    
    size += (ajuint) sizeof (EnsOGvvsample);
    
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
** @fdata [EnsPGvvsample]
** @nam3rule Trace Report Ensembl Genetic Variation Sample elements to
**                 debug file
**
** @argrule Trace gvs [const EnsPGvvsample] Ensembl Genetic Variation Sample
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvvsampleTrace *****************************************************
**
** Trace an Ensembl Genetic Variation Sample.
**
** @param [r] gvs [const EnsPGvvsample] Ensembl Genetic Variation Sample
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvsampleTrace(const EnsPGvvsample gvs, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!gvs)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensGvvsampleTrace %p\n"
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




/* @datasection [EnsPGvvsampleadaptor] Genetic Variation Sample Adaptor ********
**
** Functions for manipulating Ensembl Genetic Variation Sample Adaptor
** objects
**
** @nam2rule Gvvsampleadaptor
**
******************************************************************************/




/* @func ensGvvsampleadaptorNew ************************************************
**
** Default constructor for an Ensembl Genetic Variation Sample Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::Baseadaptor::new
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] Ptables [const char**] Array of char strings of table names
** @param [r] Pcolumns [const char**] Array of char strings of column names
** @param [r] left_join [EnsOBaseadaptorLeftJoin*] Address of an array of
**                                   Ensembl Base Adaptor Left Join Statements
** @param [r] condition [const char *] Default condition in a
**                                     SQL SELECT statement
** @param [r] final [const char *] Final condition in a
**                                 SQL SELECT statement
** @param [f] Fquery [AjBool function] Pointer to a function to handle the
**                              Ensembl Object-specific SQL statement
**
** @return [EnsPGvvsampleadaptor] Ensembl Genetic Variation Sample Adaptor
**                               or NULL
** @@
******************************************************************************/

EnsPGvvsampleadaptor ensGvvsampleadaptorNew(
    EnsPDatabaseadaptor dba,
    const char **Ptables,
    const char **Pcolumns,
    EnsOBaseadaptorLeftJoin *left_join,
    const char *condition,
    const char *final,
    AjBool Fquery(EnsPDatabaseadaptor dba,
                  const AjPStr statement,
                  EnsPAssemblymapper am,
                  EnsPSlice slice,
                  AjPList objects))

{
    EnsPGvvsampleadaptor gvsa = NULL;
    
    AJNEW0(gvsa);
    
    gvsa->Adaptor = ensBaseadaptorNew(dba,
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




/* @func ensGvvsampleadaptorDel ************************************************
**
** Default destructor for an Ensembl Gentic Variation Sample Adaptor.
**
** @param [d] Pgvsa [EnsPGvvsampleadaptor*] Ensembl Genetic Variation
**                                         Sample Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGvvsampleadaptorDel(EnsPGvvsampleadaptor *Pgvsa)
{
    EnsPGvvsampleadaptor pthis = NULL;
    
    if(!Pgvsa)
	return;
    
    if(!*Pgvsa)
	return;

    pthis = *Pgvsa;
    
    ensBaseadaptorDel(&pthis->Adaptor);
    
    AJFREE(pthis);

    *Pgvsa = NULL;
    
    return;
}




/* @func ensGvvsampleadaptorGetAdaptor *****************************************
**
** Get the Ensembl Base Adaptor element of an
** Ensembl Genetic Variation Sample Adaptor.
**
** @param [r] gvsa [const EnsPGvvsampleadaptor] Ensembl Genetic Variation
**                                             Sample Adaptor
**
** @return [EnsPBaseadaptor] Ensembl Base Adaptor
** @@
******************************************************************************/

EnsPBaseadaptor ensGvvsampleadaptorGetAdaptor(const EnsPGvvsampleadaptor gvsa)
{
    if(!gvsa)
        return NULL;
    
    return gvsa->Adaptor;
}




/* @func ensGvvsampleadaptorFetchAllSynonymsByIdentifier ***********************
**
** Fetch all Ensembl Genetic Variation Sample synonyms for an
** Ensembl Genetic Variation Sample identifier.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Sampleadaptor::fetch_synonyms
** @param [r] gvsa [const EnsPGvvsampleadaptor] Ensembl Genetic Variation
**                                             Sample Adaptor
** @param [r] identifier [ajuint] Ensembl Genetic Variation Sample identifier
** @param [r] source [const AjPStr] Source
** @param [u] synonyms [AjPList] AJAX List of synonym AJAX Strings
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvsampleadaptorFetchAllSynonymsByIdentifier(const EnsPGvvsampleadaptor gvsa,
                                                      ajuint identifier,
                                                      const AjPStr source,
                                                      AjPList synonyms)
{
    char *txtsource = NULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr name      = NULL;
    AjPStr statement = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!gvsa)
	return ajFalse;
    
    if(!synonyms)
	return ajFalse;
    
    dba = ensBaseadaptorGetDatabaseadaptor(gvsa->Adaptor);
    
    if(!dba)
	return ajFalse;
    
    if(source)
    {
	ensDatabaseadaptorEscapeC(dba, &txtsource, source);
	
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
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	name = ajStrNew();
	
	sqlr = ajSqlrowiterGet(sqli);
	
        ajSqlcolumnToStr(sqlr, &name);
	
	ajListPushAppend(synonyms, (void *) ajStrNewRef(name));
	
	ajStrDel(&name);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensGvvsampleadaptorFetchAllIdentifiersBySynonym ***********************
**
** Fetch all Ensembl Genetic Variation Sample identifiers for an
** Ensembl Genetic Variation Sample synonym.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Sampleadaptor::fetch_sample_by_synonym
** @param [r] gvsa [const EnsPGvvsampleadaptor] Ensembl Genetic Variation
**                                             Sample Adaptor
** @param [r] synonym [const AjPStr] Ensembl Genetic Variation Sample synonym
** @param [r] source [const AjPStr] Source
** @param [u] idlist [AjPList] AJAX List of Ensembl Genetic Variation
**                             Sample identifiers
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvsampleadaptorFetchAllIdentifiersBySynonym(
    const EnsPGvvsampleadaptor gvsa,
    const AjPStr synonym,
    const AjPStr source,
    AjPList idlist)
{
    char *txtsynonym = NULL;
    char *txtsource  = NULL;
    
    ajuint *Pidentifier = NULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr statement = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!gvsa)
	return ajFalse;
    
    if(!idlist)
	return ajFalse;
    
    dba = ensBaseadaptorGetDatabaseadaptor(gvsa->Adaptor);
    
    if(!dba)
	return ajFalse;
    
    ensDatabaseadaptorEscapeC(dba, &txtsynonym, synonym);
    
    if(source)
    {
	ensDatabaseadaptorEscapeC(dba, &txtsource, source);
	
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
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	AJNEW0(Pidentifier);
	
	sqlr = ajSqlrowiterGet(sqli);
	
        ajSqlcolumnToUint(sqlr, Pidentifier);
	
	ajListPushAppend(idlist, (void *) Pidentifier);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensGvvsampleadaptorFetchByIdentifier **********************************
**
** Fetch an Ensembl Genetic Variation Sample by its SQL database-internal
** identifier.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Sampleadaptor::fetch_by_dbID
** @param [r] gvsa [const EnsPGvvsampleadaptor] Ensembl Genetic Variation
**                                             Sample Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pgvs [EnsPGvvsample*] Ensembl Genetic Variation Sample address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvsampleadaptorFetchByIdentifier(const EnsPGvvsampleadaptor gvsa,
                                           ajuint identifier,
                                           EnsPGvvsample *Pgvs)
{
    AjPList gvss = NULL;
    
    AjPStr constraint = NULL;
    
    EnsPGvvsample gvs = NULL;
    
    if(!gvsa)
	return ajFalse;
    
    if(!Pgvs)
	return ajFalse;
    
    /*
    ** The constraint must contain the sample identifier field that is used
    ** either for individuals or populations.
    */
    
    constraint = ajFmtStr("%s.sample_id = %d",
			  ensBaseadaptorGetPrimaryTable(gvsa->Adaptor),
			  identifier);
    
    gvss = ajListNew();
    
    ensBaseadaptorGenericFetch(gvsa->Adaptor,
			       constraint,
			       NULL,
			       NULL,
			       gvss);
    
    if(ajListGetLength(gvss) > 1)
	ajDebug("ensGvvsampleadaptorFetchByIdentifier got more than one "
		"Sample for constraint '%S'\n", constraint);
    
    ajListPop(gvss, (void **) Pgvs);
    
    while(ajListPop(gvss, (void **) &gvs))
	ensGvvsampleDel(&gvs);
    
    ajListFree(&gvss);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/*
** TODO: The following method is missing.
** Bio::EnsEMBL::Variation::DBSQL::Sampleadaptor::fetch_all_by_dbID_list
*/




/* @datasection [EnsPGvindividual] Genetic Variation Individual ***************
**
** Functions for manipulating Ensembl Genetic Variation Individual objects
**
** @nam2rule Gvindividual
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
** @fdata [EnsPGvindividual]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvindividual] Ensembl Genetic Variation Individual
** @argrule Ref object [EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @valrule * [EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @fcategory new
******************************************************************************/




/* @func ensGvindividualNew ***************************************************
**
** Default constructor for an Ensembl Genetic Variation Individual.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvia [EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                           Individual Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Sample::new
** @param [u] sample [EnsPGvvsample] Ensembl Genetic Variation Sample
** @cc Bio::EnsEMBL::Variation::Individual::new
** @param [u] father [EnsPGvindividual] Father
** @param [u] mother [EnsPGvindividual] Mother
** @param [r] gender [AjEnum] Gender
** @param [r] type [AjEnum] Type
** @param [u] description [AjPStr] Description
**
** @return [EnsPGvindividual] Ensembl Genetic Variation Individual or NULL
** @@
******************************************************************************/

EnsPGvindividual ensGvindividualNew(EnsPGvindividualadaptor gvia,
                                    ajuint identifier,
                                    EnsPGvvsample gvs,
                                    EnsPGvindividual father,
                                    EnsPGvindividual mother,
                                    AjEnum gender,
                                    AjEnum type,
                                    AjPStr description)
{
    EnsPGvindividual gvi = NULL;
    
    if(!gvs)
	return NULL;
    
    AJNEW0(gvi);
    
    gvi->Use = 1;
    
    gvi->Identifier = identifier;
    
    gvi->Adaptor = gvia;
    
    gvi->Gvvsample = ensGvvsampleNewRef(gvs);
    
    gvi->Father = ensGvindividualNewRef(father);
    
    gvi->Mother = ensGvindividualNewRef(mother);
    
    gvi->Gender = gender;
    
    gvi->Type = type;
    
    if(description)
	gvi->Description = ajStrNewRef(description);
    
    return gvi;
}




/* @func ensGvindividualNewObj ************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvindividual] Ensembl Genetic Variation
**                                            Individual
**
** @return [EnsPGvindividual] Ensembl Genetic Variation Individual or NULL
** @@
******************************************************************************/

EnsPGvindividual ensGvindividualNewObj(const EnsPGvindividual object)
{
    EnsPGvindividual gvi = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(gvi);
    
    gvi->Use = 1;
    
    gvi->Identifier = object->Identifier;
    
    gvi->Adaptor = object->Adaptor;
    
    gvi->Gvvsample = ensGvvsampleNewRef(object->Gvvsample);
    
    gvi->Father = ensGvindividualNewRef(object->Father);
    
    gvi->Mother = ensGvindividualNewRef(object->Mother);
    
    gvi->Gender = object->Gender;
    
    gvi->Type = object->Type;
    
    if(object->Description)
	gvi->Description = ajStrNewRef(object->Description);
    
    return gvi;
}




/* @func ensGvindividualNewRef ************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvi [EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @return [EnsPGvindividual] Ensembl Genetic Variation Individual
** @@
******************************************************************************/

EnsPGvindividual ensGvindividualNewRef(EnsPGvindividual gvi)
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
** @fdata [EnsPGvindividual]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Individual object
**
** @argrule * Pgvi [EnsPGvindividual*] Genetic Variation Individual
**                                     object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvindividualDel ***************************************************
**
** Default destructor for an Ensembl Genetic Variation Individual.
**
** @param [d] Pgvi [EnsPGvindividual*] Ensembl Genetic Variation Individual
**                                     address
**
** @return [void]
** @@
******************************************************************************/

void ensGvindividualDel(EnsPGvindividual *Pgvi)
{
    EnsPGvindividual pthis = NULL;
    
    /*
     ajDebug("ensGvindividualDel\n"
	     "  *Pgvi %p\n",
	     *Pgvi);
     
     ensGvindividualTrace(*Pgvi, 1);
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
    
    ensGvvsampleDel(&pthis->Gvvsample);
    
    ensGvindividualDel(&pthis->Father);
    
    ensGvindividualDel(&pthis->Mother);
    
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
** @fdata [EnsPGvindividual]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Individual attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Individual Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetGvvsample Return the Ensembl Genetic Variation Sample
** @nam4rule GetFather Return the father Ensembl Genetic Variation Individual
** @nam4rule GetMother Return the mother Ensembl Genetic Variation Individual
** @nam4rule GetGender Return the gender
** @nam4rule GetType Return the type
**
** @argrule * gvi [const EnsPGvindividual] Genetic Variation Individual
**
** @valrule Adaptor [EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                            Individual Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Gvvsample [EnsPGvvsample] Ensembl Genetic Variation Sample
** @valrule Father [EnsPGvindividual] Ensembl Genetic Variation Individual
** @valrule Mother [EnsPGvindividual] Ensembl Genetic Variation Individual
** @valrule Gender [AjEnum] Gender
** @valrule Type [AjEnum] Type
** @valrule Description [AjPStr] Description
**
** @fcategory use
******************************************************************************/




/* @func ensGvindividualGetAdaptor ********************************************
**
** Get the Ensembl Genetic Variation Individual Adaptor element of an
** Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @return [EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                   Individual Adaptor
** @@
******************************************************************************/

EnsPGvindividualadaptor ensGvindividualGetAdaptor(const EnsPGvindividual gvi)
{
    if(!gvi)
        return NULL;
    
    return gvi->Adaptor;
}




/* @func ensGvindividualGetIdentifier *****************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvindividualGetIdentifier(const EnsPGvindividual gvi)
{
    if(!gvi)
        return 0;
    
    return gvi->Identifier;
}




/* @func ensGvindividualGetGvvsample *******************************************
**
** Get the Ensembl Genetic Variation Sample element of an
** Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @return [EnsPGvvsample] Ensembl Genetic Variation Sample
** @@
******************************************************************************/

EnsPGvvsample ensGvindividualGetGvvsample(const EnsPGvindividual gvi)
{
    if(!gvi)
        return NULL;
    
    return gvi->Gvvsample;
}




/* @func ensGvindividualGetFather *********************************************
**
** Get the father Ensembl Genetic Variation Individual element of an
** Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @return [EnsPGvindividual] Ensembl Genetic Variation Individual
** @@
******************************************************************************/

EnsPGvindividual ensGvindividualGetFather(const EnsPGvindividual gvi)
{
    if(!gvi)
        return NULL;
    
    return gvi->Father;
}




/* @func ensGvindividualGetMother *********************************************
**
** Get the mother Ensembl Genetic Variation Individual element of an
** Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @return [EnsPGvindividual] Ensembl Genetic Variation Individual
** @@
******************************************************************************/

EnsPGvindividual ensGvindividualGetMother(const EnsPGvindividual gvi)
{
    if(!gvi)
        return NULL;
    
    return gvi->Mother;
}




/* @func ensGvindividualGetGender *********************************************
**
** Get the gender element of an Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @return [AjEnum] Gender or ensEGvindividualGenderNULL
** @@
******************************************************************************/

AjEnum ensGvindividualGetGender(const EnsPGvindividual gvi)
{
    if(!gvi)
        return ensEGvindividualGenderNULL;
    
    return gvi->Gender;
}




/* @func ensGvindividualGetType ***********************************************
**
** Get the type element of an Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @return [AjEnum] Type or ensEGvindividualTypeNULL
** @@
******************************************************************************/

AjEnum ensGvindividualGetType(const EnsPGvindividual gvi)
{
    if(!gvi)
        return ensEGvindividualTypeNULL;
    
    return gvi->Type;
}




/* @func ensGvindividualGetDescription ****************************************
**
** Get the description element of an Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @return [AjPStr] Description
** @@
******************************************************************************/

AjPStr ensGvindividualGetDescription(const EnsPGvindividual gvi)
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
** @fdata [EnsPGvindividual]
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
** @argrule * gvi [EnsPGvindividual] Ensembl Genetic Variation Individual
**                                   object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvindividualSetAdaptor ********************************************
**
** Set the Ensembl Genetic Variation Individual Adaptor element of an
** Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGvindividual] Ensembl Genetic Variation Individual
** @param [r] gvia [EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                           Individual Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualSetAdaptor(EnsPGvindividual gvi,
                                 EnsPGvindividualadaptor gvia)
{
    if(!gvi)
        return ajFalse;
    
    gvi->Adaptor = gvia;
    
    return ajTrue;
}




/* @func ensGvindividualSetIdentifier *****************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGvindividual] Ensembl Genetic Variation Individual
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualSetIdentifier(EnsPGvindividual gvi, ajuint identifier)
{
    if(!gvi)
        return ajFalse;
    
    gvi->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGvindividualSetGvvsample *******************************************
**
** Set the Ensembl Genetic Variation Sample element of an
** Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGvindividual] Ensembl Genetic Variation Individual
** @param [u] gvs [EnsPGvvsample] Ensembl Genetic Variation Sample
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualSetGvvsample(EnsPGvindividual gvi, EnsPGvvsample gvs)
{
    if(!gvi)
        return ajFalse;
    
    ensGvvsampleDel(&(gvi->Gvvsample));
    
    gvi->Gvvsample = ensGvvsampleNewRef(gvs);
    
    return ajTrue;
}




/* @func ensGvindividualSetFather *********************************************
**
** Set the father Ensembl Genetic Variation Individual element of an
** Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGvindividual] Ensembl Genetic Variation Individual
** @param [u] father [EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualSetFather(EnsPGvindividual gvi, EnsPGvindividual father)
{
    if(!gvi)
        return ajFalse;
    
    ensGvindividualDel(&(gvi->Father));
    
    gvi->Father = ensGvindividualNewRef(father);
    
    return ajTrue;
}




/* @func ensGvindividualSetMother *********************************************
**
** Set the mother Ensembl Genetic Variation Individual element of an
** Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGvindividual] Ensembl Genetic Variation Individual
** @param [u] mother [EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualSetMother(EnsPGvindividual gvi, EnsPGvindividual mother)
{
    if(!gvi)
        return ajFalse;
    
    ensGvindividualDel(&(gvi->Mother));
    
    gvi->Mother = ensGvindividualNewRef(mother);
    
    return ajTrue;
}




/* @func ensGvindividualSetGender *********************************************
**
** Set the gender element of an Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGvindividual] Ensembl Genetic Variation Individual
** @param [r] gender [AjEnum] Gender
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualSetGender(EnsPGvindividual gvi, AjEnum gender)
{
    if(!gvi)
        return ajFalse;
    
    gvi->Gender = gender;
    
    return ajTrue;
}




/* @func ensGvindividualSetType ***********************************************
**
** Set the type element of an Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGvindividual] Ensembl Genetic Variation Individual
** @param [r] type [AjEnum] Type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualSetType(EnsPGvindividual gvi, AjEnum type)
{
    if(!gvi)
        return ajFalse;
    
    gvi->Type = type;
    
    return ajTrue;
}




/* @func ensGvindividualSetDescription ****************************************
**
** Set the description element of an Ensembl Genetic Variation Individual.
**
** @param [u] gvi [EnsPGvindividual] Ensembl Genetic Variation Individual
** @param [u] description [AjPStr] Description
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualSetDescription(EnsPGvindividual gvi, AjPStr description)
{
    if(!gvi)
        return ajFalse;
    
    ajStrDel(&gvi->Description);
    
    if(description)
	gvi->Description = ajStrNewRef(description);
    
    return ajTrue;
}




/* @func ensGvindividualGenderFromStr *****************************************
**
** Convert an AJAX String into an Ensembl Genetic Variaton Individual
** gender element.
**
** @param [r] gender [const AjPStr] Gender string
**
** @return [AjEnum] Ensembl Genetic Variation Individual gender element or
**                  ensEGvindividualGenderNULL
** @@
******************************************************************************/

AjEnum ensGvindividualGenderFromStr(const AjPStr gender)
{
    register ajint i = 0;
    
    AjEnum egender = ensEGvindividualGenderNULL;
    
    for(i = 1; gvIndividualGender[i]; i++)
	if(ajStrMatchC(gender, gvIndividualGender[i]))
	    egender = i;
    
    if(!egender)
	ajDebug("ensGvindividualGenderFromStr encountered "
		"unexpected string '%S'.\n", gender);
    
    return egender;
}




/* @func ensGvindividualTypeFromStr *******************************************
**
** Convert an AJAX String into an Ensembl Genetic Variaton Individual
** type element.
**
** @param [r] type [const AjPStr] Type string
**
** @return [AjEnum] Ensembl Genetic Variation Individual type element or
**                  ensEGvindividualTypeNULL
** @@
******************************************************************************/

AjEnum ensGvindividualTypeFromStr(const AjPStr type)
{
    register ajint i = 0;
    
    AjEnum etype = ensEGvindividualTypeNULL;
    
    for(i = 1; gvIndividualType[i]; i++)
	if(ajStrMatchC(type, gvIndividualType[i]))
	    etype = i;
    
    if(!etype)
	ajDebug("ensGvindividualTypeFromStr encountered "
		"unexpected string '%S'.\n", type);
    
    return etype;
}




/* @func ensGvindividualGenderToChar ******************************************
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

const char* ensGvindividualGenderToChar(const AjEnum gender)
{
    register ajint i = 0;
    
    if(!gender)
	return NULL;
    
    for(i = 1; gvIndividualGender[i] && (i < gender); i++);
    
    if(!gvIndividualGender[i])
	ajDebug("ensGvindividualGenderToChar encountered an "
		"out of boundary error on gender %d.\n", gender);
    
    return gvIndividualGender[i];
}




/* @func ensGvindividualTypeToChar ********************************************
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

const char* ensGvindividualTypeToChar(const AjEnum type)
{
    register ajint i = 0;
    
    if(!type)
	return NULL;
    
    for(i = 1; gvIndividualType[i] && (i < type); i++);
    
    if(!gvIndividualType[i])
	ajDebug("ensGvindividualTypeToChar encountered an "
		"out of boundary error on type %d.\n", type);
    
    return gvIndividualType[i];
}




/* @func ensGvindividualGetMemSize ********************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGvindividual] Ensembl Genetic Variation Individual
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGvindividualGetMemSize(const EnsPGvindividual gvi)
{
    ajuint size = 0;
    
    if(!gvi)
	return 0;
    
    size += (ajuint) sizeof (EnsOGvindividual);
    
    size += ensGvvsampleGetMemSize(gvi->Gvvsample);
    
    size += ensGvindividualGetMemSize(gvi->Father);
    
    size += ensGvindividualGetMemSize(gvi->Mother);
    
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
** @fdata [EnsPGvindividual]
** @nam3rule Trace Report Ensembl Genetic Variation Individual elements to
**                 debug file
**
** @argrule Trace gvi [const EnsPGvindividual] Ensembl Genetic Variation
**                                             Individual
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @valrule * [void]
**
** @fcategory misc
******************************************************************************/




/* @func ensGvindividualTrace *************************************************
**
** Trace an Ensembl Genetic Variation Individual.
**
** @param [r] gvi [const EnsPGvindividual] Ensembl Genetic Variation Individual
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualTrace(const EnsPGvindividual gvi, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!gvi)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensGvindividualTrace %p\n"
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
	    indent, gvi->Gvvsample,
	    indent, gvi->Father,
	    indent, gvi->Mother,
	    indent, ensGvindividualGenderToChar(gvi->Gender),
	    indent, ensGvindividualTypeToChar(gvi->Type),
	    indent, gvi->Description);
    
    ensGvvsampleTrace(gvi->Gvvsample, level + 1);
    
    ensGvindividualTrace(gvi->Father, level + 1);
    ensGvindividualTrace(gvi->Mother, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @datasection [EnsPGvindividualadaptor] Genetic Variation Individual Adaptor
**
** Functions for manipulating Ensembl Genetic Variation Individual Adaptor
** objects
**
** @nam2rule Gvindividualadaptor
**
******************************************************************************/

static const char *gvIndividualadaptorTables[] =
{
    "individual",
    "sample",
    "individual_type",
    NULL
};




static const char *gvIndividualadaptorColumns[] =
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




static EnsOBaseadaptorLeftJoin gvIndividualadaptorLeftJoin[] =
{
    {NULL, NULL}
};




static const char *gvIndividualadaptorDefaultCondition =
"sample.sample_id = individual.sample_id AND "
"individual.individual_type_id = individual_type.individual_type_id";




static const char *gvIndividualadaptorFinalCondition = NULL;




/* @funcstatic gvIndividualadaptorFetchAllBySQL *******************************
**
** Fetch all Ensembl Genetic Variation Individual objects via an SQL statement.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblymapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] gvis [AjPList] AJAX List of Ensembl Genetic Variation Individual
**                           objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvIndividualadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblymapper am,
                                               EnsPSlice slice,
                                               AjPList gvis)
{
    ajuint sidentifier = 0;
    ajuint fatherid    = 0;
    ajuint motherid    = 0;
    ajuint ssize       = 0;
    
    AjEnum egender = ensEGvindividualGenderNULL;
    AjEnum etype   = ensEGvindividualTypeNULL;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr sname        = NULL;
    AjPStr sdescription = NULL;
    AjPStr gender       = NULL;
    AjPStr tname        = NULL;
    AjPStr tdescription = NULL;
    
    EnsPGvindividual gvi         = NULL;
    EnsPGvindividual father      = NULL;
    EnsPGvindividual mother      = NULL;
    EnsPGvindividualadaptor gvia = NULL;
    
    EnsPGvvsample gvs         = NULL;
    EnsPGvvsampleadaptor gvsa = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    (void) am;
    
    (void) slice;
    
    if(!gvis)
	return ajFalse;
    
    gvia = ensRegistryGetGvindividualadaptor(dba);
    
    gvsa = ensGvindividualadaptorGetAdaptor(gvia);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
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
	
	sqlr = ajSqlrowiterGet(sqli);
	
	ajSqlcolumnToUint(sqlr, &sidentifier);
        ajSqlcolumnToStr(sqlr, &sname);
	ajSqlcolumnToUint(sqlr, &ssize);
        ajSqlcolumnToStr(sqlr, &sdescription);
	ajSqlcolumnToStr(sqlr, &gender);
	ajSqlcolumnToUint(sqlr, &fatherid);
	ajSqlcolumnToUint(sqlr, &motherid);
        ajSqlcolumnToStr(sqlr, &tname);
        ajSqlcolumnToStr(sqlr, &tdescription);
	
	egender = ensGvindividualGenderFromStr(gender);
	etype = ensGvindividualTypeFromStr(tname);
	
	gvs = ensGvvsampleNew(gvsa,
			     sidentifier,
			     sname,
			     sdescription,
			     ssize);
	
	ensGvindividualadaptorFetchByIdentifier(gvia, fatherid, &father);
	
	ensGvindividualadaptorFetchByIdentifier(gvia, motherid, &mother);
	
	gvi = ensGvindividualNew(gvia,
				 sidentifier,
				 gvs,
				 father,
				 mother,
				 egender,
				 etype,
				 tdescription);
	
	ajListPushAppend(gvis, (void *) gvi);
	
	ensGvvsampleDel(&gvs);
	
	ensGvindividualDel(&father);
	ensGvindividualDel(&mother);
	
	ajStrDel(&sname);
	ajStrDel(&sdescription);
	ajStrDel(&gender);
	ajStrDel(&tname);
	ajStrDel(&tdescription);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    return ajTrue;
}




/* @func ensGvindividualadaptorNew ********************************************
**
** Default constructor for an Ensembl Genetic Variation Individual Adaptor.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Individualadaptor::new
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                   Individual Adaptor or NULL
** @@
******************************************************************************/

EnsPGvindividualadaptor ensGvindividualadaptorNew(EnsPDatabaseadaptor dba)
{
    EnsPGvindividualadaptor gvia = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(gvia);
    
    gvia->Adaptor = ensGvvsampleadaptorNew(dba,
					  gvIndividualadaptorTables,
					  gvIndividualadaptorColumns,
					  gvIndividualadaptorLeftJoin,
					  gvIndividualadaptorDefaultCondition,
					  gvIndividualadaptorFinalCondition,
					  gvIndividualadaptorFetchAllBySQL);
    
    if(!gvia->Adaptor)
	AJFREE(gvia);
    
    return gvia;
}




/* @func ensGvindividualadaptorDel ********************************************
**
** Default destructor for an Ensembl Gentic Variation Individual Adaptor.
**
** @param [d] Pgvia [EnsPGvindividualadaptor*] Ensembl Genetic Variation
**                                             Individual Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGvindividualadaptorDel(EnsPGvindividualadaptor *Pgvi)
{
    EnsPGvindividualadaptor pthis = NULL;
    
    if(!Pgvi)
	return;
    
    if(!*Pgvi)
	return;

    pthis = *Pgvi;
    
    ensGvvsampleadaptorDel(&pthis->Adaptor);
    
    AJFREE(pthis);

    *Pgvi = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Individual Adaptor object.
**
** @fdata [EnsPGvindividualadaptor]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Individual Adaptor attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Sample Adaptor
**
** @argrule * gvia [const EnsPGvindividualadaptor] Genetic Variation
**                                                 Individual Adaptor
**
** @valrule Adaptor [EnsPGvvsampleadaptor] Ensembl Genetic Variation
**                                        Sample Adaptor
**
** @fcategory use
******************************************************************************/




/* @func ensGvindividualadaptorGetAdaptor *************************************
**
** Get the Ensembl Genetic Variation Sample Adaptor element of an
** Ensembl Genetic Variation Individual Adaptor.
**
** @param [r] gvia [const EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                                    Individual Adaptor
**
** @return [EnsPGvvsampleadaptor] Ensembl Genetic Variation Sample Adaptor
** @@
******************************************************************************/

EnsPGvvsampleadaptor ensGvindividualadaptorGetAdaptor(
    const EnsPGvindividualadaptor gvia)
{
    if(!gvia)
	return NULL;
    
    return gvia->Adaptor;
}




/* @func ensGvindividualadaptorFetchByIdentifier ******************************
**
** Fetch an Ensembl Genetic Variation Individual via its
** SQL database-internal identifier.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Individual.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Individualadaptor::fetch_by_dbID
** @param [r] gvia [const EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [r] identifier [ajuint] SQL database-internal Individual identifier
** @param [wP] Pgvi [EnsPGvindividual*] Ensembl Genetic Variation
**                                      Individual address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualadaptorFetchByIdentifier(const EnsPGvindividualadaptor gvia,
                                               ajuint identifier,
                                               EnsPGvindividual *Pgvi)
{
    EnsPBaseadaptor ba = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pgvi)
	return ajFalse;
    
    ba = ensGvvsampleadaptorGetAdaptor(gvia->Adaptor);
    
    *Pgvi = (EnsPGvindividual)
	ensBaseadaptorFetchByIdentifier(ba, identifier);
    
    return ajTrue;
}




/* @func ensGvindividualadaptorFetchAllByName *********************************
**
** Fetch all Ensembl Genetic Variation Individuals by an
** Ensembl Genetic Variation Sample name.
**
** Individual names may be non-unique, therefore the function returns an
** AJAX List of Ensembl Genetic Variation Individuals.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Individualadaptor::fetch_all_by_name
** @param [r] gvia [const EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [r] name [const AjPStr] Ensembl Genetic Variation Sample name
** @param [u] gvis [AjPList] AJAX List of Ensembl Genetic Variation Individuals
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualadaptorFetchAllByName(const EnsPGvindividualadaptor gvia,
                                            const AjPStr name,
                                            AjPList gvis)
{
    char *txtname = NULL;
    
    AjPStr constraint = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    if(!gvis)
	return ajFalse;
    
    ba = ensGvvsampleadaptorGetAdaptor(gvia->Adaptor);
    
    ensBaseadaptorEscapeC(ba, &txtname, name);
    
    constraint = ajFmtStr("sample.name = '%s'", txtname);
    
    ajCharDel(&txtname);
    
    ensBaseadaptorGenericFetch(ba, constraint, NULL, NULL, gvis);
    
    /* FIXME: Should we warn here? */
    if(ajListGetLength(gvis) == 0)
	ajDebug("ensGvindividualadaptorFetchByName could not get an "
		"Ensembl Genetic Variation Individual "
		"for name '%S'.\n",
		name);
    
    if(ajListGetLength(gvis) > 1)
	ajDebug("ensGvindividualadaptorFetchByName got more than one "
		"Ensembl Genetic Variation Individual "
		"for name '%S'.\n",
		name);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensGvindividualadaptorFetchAllBySynonym ******************************
**
** Fetch all Ensembl Genetic Variation Individuals by an
** Ensembl Genetic Variation Sample synonym.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Individualadaptor::
**     fetch_individual_by_synonym
** @param [r] gvia [const EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [r] synonym [const AjPStr] Ensembl Genetic Variation Sample synonym
** @param [u] gvis [AjPList] AJAX List of Ensembl Genetic Variation Individuals
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualadaptorFetchAllBySynonym(const EnsPGvindividualadaptor gvia,
                                               const AjPStr synonym,
                                               AjPList gvis)
{
    ajuint *Pidentifier = 0;
    
    AjPList idlist = NULL;
    
    EnsPGvindividual gvi = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!synonym)
	return ajFalse;
    
    if(!gvis)
	return ajFalse;
    
    idlist = ajListNew();
    
    ensGvvsampleadaptorFetchAllIdentifiersBySynonym(gvia->Adaptor,
						   synonym,
						   NULL,
						   idlist);
    
    while(ajListPop(idlist, (void **) &Pidentifier))
    {
	ensGvindividualadaptorFetchByIdentifier(gvia,
						*Pidentifier,
						&gvi);
	
	ajListPushAppend(gvis, (void *) gvi);
	
	AJFREE(Pidentifier);
    }
    
    ajListFree(&idlist);
    
    return ajTrue;
}




/* @func ensGvindividualadaptorFetchAllByPopulation ***************************
**
** Fetch all Ensembl Genetic Variation Individuals by an
** Ensembl Genetic Variation Population.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Individualadaptor::
**     fetch_all_by_Population
** @param [r] gvia [const EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [u] gvis [AjPList] AJAX List of Ensembl Genetic Variation Individuals
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualadaptorFetchAllByPopulation(const EnsPGvindividualadaptor gvia,
                                                  const EnsPGvpopulation gvp,
                                                  AjPList gvis)
{
    AjPStr statement = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!gvp)
	return ajFalse;
    
    if(!gvis)
	return ajFalse;
    
    if(!gvp->Identifier)
    {
	ajDebug("ensGvindividualadaptorFetchAllByPopulation "
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
    
    ba = ensGvvsampleadaptorGetAdaptor(gvia->Adaptor);
    
    dba = ensBaseadaptorGetDatabaseadaptor(ba);
    
    gvIndividualadaptorFetchAllBySQL(dba, statement, NULL, NULL, gvis);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensGvindividualadaptorFetchAllByParentIndividual *********************
**
** Fetch all Ensembl Genetic Variation Individuals by a parent
** Ensembl Genetic Variation Individual.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Individualadaptor::
**     fetch_all_by_parent_Individual
** @param [r] gvia [const EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [r] parent [const EnsPGvindividual] Ensembl Genetic Variation
**                                            Individual
** @param [u] gvis [AjPList] AJAX List of Ensembl Genetic Variation Individuals
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualadaptorFetchAllByParentIndividual(
    const EnsPGvindividualadaptor gvia,
    const EnsPGvindividual parent,
    AjPList gvis)
{
    AjPStr constraint = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!parent)
	return ajFalse;
    
    if(!gvis)
	return ajFalse;
    
    if(!parent->Identifier)
    {
	ajDebug("ensGvindividualadaptorFetchAllByParentIndividual cannot get "
		"Individuals for parent Indeividual without identifier.\n");
	
	return ajFalse;
    }
    
    ba = ensGvvsampleadaptorGetAdaptor(gvia->Adaptor);
    
    if(parent->Gender == ensEGvindividualGenderMale)
    {
	constraint = ajFmtStr("individual.father_individual_sample_id = %u",
			      parent->Identifier);
	
	ensBaseadaptorGenericFetch(ba, constraint, NULL, NULL, gvis);
	
	ajStrDel(&constraint);
	
	return ajTrue;
    }
    else if(parent->Gender == ensEGvindividualGenderFemale)
    {
	constraint = ajFmtStr("individual.mother_individual_sample_id = %u",
			      parent->Identifier);
	
	ensBaseadaptorGenericFetch(ba, constraint, NULL, NULL, gvis);
	
	ajStrDel(&constraint);
	
	return ajTrue;
    }
    
    /* The gender is unknown, so assume this is the father. */
    
    constraint = ajFmtStr("individual.father_individual_sample_id = %u",
			  parent->Identifier);
    
    ensBaseadaptorGenericFetch(ba, constraint, NULL, NULL, gvis);
    
    ajStrDel(&constraint);
    
    if(ajListGetLength(gvis))
	return ajTrue;
    
    /* No result for the father, so assume it is the mother. */
    
    constraint = ajFmtStr("individual.mother_individual_sample_id = %u",
			  parent->Identifier);
    
    ensBaseadaptorGenericFetch(ba, constraint, NULL, NULL, gvis);
    
    ajStrDel(&constraint);
    
    /* Return regardless of a result. */
    
    return ajTrue;
}




/* @func ensGvindividualadaptorFetchAllStrains ********************************
**
** Fetch all Ensembl Genetic Variation Individuals by a parent
** Ensembl Genetic Variation Individual.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Individualadaptor::fetch_all_strains
** @param [r] gvia [const EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [u] gvis [AjPList] AJAX List of Ensembl Genetic Variation Individuals
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualadaptorFetchAllStrains(const EnsPGvindividualadaptor gvia,
                                             AjPList gvis)
{
    AjPStr constraint = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!gvis)
	return ajFalse;
    
    ba = ensGvvsampleadaptorGetAdaptor(gvia->Adaptor);
    
    constraint = ajStrNewC("individual_type.name = 'fully_inbred'");
    
    ensBaseadaptorGenericFetch(ba, constraint, NULL, NULL, gvis);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensGvindividualadaptorFetchReferenceStrainName ***********************
**
** Fetch the reference strain name.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Individualadaptor::
**     get_reference_strain_name
** @param [r] gvia [const EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [w] Pname [AjPStr*] Reference strain name AJAX String address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualadaptorFetchReferenceStrainName(
    const EnsPGvindividualadaptor gvia,
    AjPStr *Pname)
{
    AjPList mis = NULL;
    
    AjPStr key = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    EnsPMetainformation mi         = NULL;
    EnsPMetainformationadaptor mia = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!Pname)
	return ajFalse;
    
    ba = ensGvvsampleadaptorGetAdaptor(gvia->Adaptor);
    
    dba = ensBaseadaptorGetDatabaseadaptor(ba);
    
    mia = ensRegistryGetMetainformationadaptor(dba);
    
    key = ajStrNewC("individual.reference_strain");
    
    mis = ajListNew();
    
    ensMetainformationadaptorFetchAllByKey(mia, key, mis);
    
    ajListPop(mis, (void **) &mi);
    
    if(mi)
    {
	ajStrAssignS(Pname, ensMetainformationGetValue(mi));
	
	ensMetainformationDel(&mi);
    }
    
    while(ajListPop(mis, (void **) &mi))
	ensMetainformationDel(&mi);
    
    ajListFree(&mis);
    
    ajStrDel(&key);
    
    return ajTrue;
}




/* @func ensGvindividualadaptorFetchAllDefaultStrains *************************
**
** Fetch all default strain names.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Individualadaptor::
**     get_default_strains
** @param [r] gvia [const EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [u] names [AjPList] AJAX List of default strain name AJAX Strings
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualadaptorFetchAllDefaultStrains(
    const EnsPGvindividualadaptor gvia,
    AjPList names)
{
    AjPList mis = NULL;
    
    AjPStr key = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    EnsPMetainformation mi         = NULL;
    EnsPMetainformationadaptor mia = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!names)
	return ajFalse;
    
    ba = ensGvvsampleadaptorGetAdaptor(gvia->Adaptor);
    
    dba = ensBaseadaptorGetDatabaseadaptor(ba);
    
    mia = ensRegistryGetMetainformationadaptor(dba);
    
    key = ajStrNewC("individual.default_strain");
    
    mis = ajListNew();
    
    ensMetainformationadaptorFetchAllByKey(mia, key, mis);
    
    while(ajListPop(mis, (void **) &mi))
    {
	ajListPushAppend(names,
			 (void *) ajStrNewS(ensMetainformationGetValue(mi)));
	
	ensMetainformationDel(&mi);
    }
    
    ajListFree(&mis);
    
    ajStrDel(&key);
    
    return ajTrue;
}




/* @func ensGvindividualadaptorFetchAllDisplayStrains *************************
**
** Fetch all display strain names.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Individualadaptor::
**     get_display_strains
** @param [r] gvia [const EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [u] names [AjPList] AJAX List of display strain name AJAX Strings
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualadaptorFetchAllDisplayStrains(
    const EnsPGvindividualadaptor gvia,
    AjPList names)
{
    AjPList mis = NULL;
    
    AjPStr key = NULL;
    AjPStr name = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    EnsPMetainformation mi         = NULL;
    EnsPMetainformationadaptor mia = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!names)
	return ajFalse;
    
    name = ajStrNew();
    
    /* Fetch the reference strain name. */
    
    ensGvindividualadaptorFetchReferenceStrainName(gvia, &name);
    
    ajListPushAppend(names, (void *) name);
    
    /* Fetch all default strain names. */
    
    ensGvindividualadaptorFetchAllDefaultStrains(gvia, names);
    
    /* Fetch all display strain names. */
    
    ba = ensGvvsampleadaptorGetAdaptor(gvia->Adaptor);
    
    dba = ensBaseadaptorGetDatabaseadaptor(ba);
    
    mia = ensRegistryGetMetainformationadaptor(dba);
    
    key = ajStrNewC("individual.display_strain");
    
    mis = ajListNew();
    
    ensMetainformationadaptorFetchAllByKey(mia, key, mis);
    
    while(ajListPop(mis, (void **) &mi))
    {
	ajListPushAppend(names,
			 (void *) ajStrNewS(ensMetainformationGetValue(mi)));
	
	ensMetainformationDel(&mi);
    }
    
    ajListFree(&mis);
    
    ajStrDel(&key);
    
    return ajTrue;
}




/* @func ensGvindividualadaptorFetchAllStrainsWithCoverage ********************
**
** Fetch all Ensembl Genetic Variation Strain identifiers for which
** read coverage information is available.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Individualadaptor::
**     fetch_all_strains_with_coverage
** @param [r] gvia [const EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                                 Individual Adaptor
** @param [u] idlist [AjPList] AJAX List of Ensembl Genetic Variation Sample
**                             identifiers
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvindividualadaptorFetchAllStrainsWithCoverage(
    const EnsPGvindividualadaptor gvia,
    AjPList idlist)
{
    ajuint *Pidentifier = 0;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr statement = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!gvia)
	return ajFalse;
    
    if(!idlist)
	return ajFalse;
    
    ba = ensGvvsampleadaptorGetAdaptor(gvia->Adaptor);
    
    dba = ensBaseadaptorGetDatabaseadaptor(ba);
    
    statement = ajStrNewC("SELECT "
			  "DISTINCT "
			  "read_coverage.sample_id "
			  "FROM "
			  "read_coverage");
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	AJNEW0(Pidentifier);
	
	sqlr = ajSqlrowiterGet(sqli);
	
	ajSqlcolumnToUint(sqlr, Pidentifier);
	
	ajListPushAppend(idlist, (void *) Pidentifier);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    return ajTrue;
}




/* @datasection [EnsPGvpopulation] Genetic Variation Population ***************
**
** Functions for manipulating Ensembl Genetic Variation Population objects
**
** @nam2rule Gvpopulation
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
** @fdata [EnsPGvpopulation]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvpopulation] Ensembl Genetic Variation Population
** @argrule Ref object [EnsPGvpopulation] Ensembl Genetic Variation Population
**
** @valrule * [EnsPGvpopulation] Ensembl Genetic Variation Population
**
** @fcategory new
******************************************************************************/




/* @func ensGvpopulationNew ***************************************************
**
** Default constructor for an Ensembl Genetic Variation Population.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvpa [EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                              Population Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Sample::new
** @param [u] gvs [EnsPGvvsample] Ensembl Genetic Variation Sample
** @cc Bio::EnsEMBL::Variation::Population::new
** @param [r] subpopulations [AjPList] AJAX List of Ensembl Genetic
**                                     Variation (Sub-) Populations
**
** @return [EnsPGvpopulation] Ensembl Genetic Variation Population
** @@
******************************************************************************/

EnsPGvpopulation ensGvpopulationNew(EnsPGvpopulationadaptor gvpa,
                                    ajuint identifier,
                                    EnsPGvvsample gvs,
                                    AjPList subpopulations)
{
    AjIList iter = NULL;
    
    EnsPGvpopulation gvp    = NULL;
    EnsPGvpopulation subgvp = NULL;
    
    if(!gvs)
	return NULL;
    
    AJNEW0(gvp);
    
    gvp->Use = 1;
    
    gvp->Identifier = identifier;
    
    gvp->Adaptor = gvpa;
    
    gvp->Gvvsample = ensGvvsampleNewRef(gvs);
    
    gvp->SubPopulations = ajListNew();
    
    iter = ajListIterNew(subpopulations);
    
    while(!ajListIterDone(iter))
    {
	subgvp = (EnsPGvpopulation) ajListIterGet(iter);
	
	ajListPushAppend(gvp->SubPopulations,
			 (void *) ensGvpopulationNewRef(subgvp));
    }
    
    ajListIterDel(&iter);
    
    return gvp;
}




/* @func ensGvpopulationNewObj ************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvpopulation] Ensembl Genetic Variation
**                                            Population
**
** @return [EnsPGvpopulation] Ensembl Genetic Variation Population or NULL
** @@
******************************************************************************/

EnsPGvpopulation ensGvpopulationNewObj(const EnsPGvpopulation object)
{
    AjIList iter = NULL;
    
    EnsPGvpopulation gvp    = NULL;
    EnsPGvpopulation subgvp = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(gvp);
    
    gvp->Use = 1;
    
    gvp->Identifier = object->Identifier;
    
    gvp->Adaptor = object->Adaptor;
    
    gvp->Gvvsample = ensGvvsampleNewRef(object->Gvvsample);
    
    /* Copy the AJAX List of Ensembl Genetic Variation (Sub-) Populations. */
    
    gvp->SubPopulations = ajListNew();
    
    iter = ajListIterNew(object->SubPopulations);
    
    while(!ajListIterDone(iter))
    {
	subgvp = (EnsPGvpopulation) ajListIterGet(iter);
	
	ajListPushAppend(gvp->SubPopulations,
			 (void *) ensGvpopulationNewRef(subgvp));
    }
    
    ajListIterDel(&iter);
    
    return gvp;
}




/* @func ensGvpopulationNewRef ************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvp [EnsPGvpopulation] Ensembl Genetic Variation Population
**
** @return [EnsPGvpopulation] Ensembl Genetic Variation Population
** @@
******************************************************************************/

EnsPGvpopulation ensGvpopulationNewRef(EnsPGvpopulation gvp)
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
** @fdata [EnsPGvpopulation]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Population object
**
** @argrule * Pgvp [EnsPGvpopulation*] Genetic Variation Population
**                                     object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvpopulationDel ***************************************************
**
** Default destructor for an Ensembl Genetic Variation Population.
**
** @param [d] Pgvp [EnsPGvpopulation*] Ensembl Genetic Variation Population
**                                     address
**
** @return [void]
** @@
******************************************************************************/

void ensGvpopulationDel(EnsPGvpopulation *Pgvp)
{
    EnsPGvpopulation subgvp = NULL;
    EnsPGvpopulation pthis = NULL;
    
    
    /*
     ajDebug("ensGvpopulationDel\n"
	     "  *Pgvp %p\n",
	     *Pgvp);
     
     ensGvpopulationTrace(*Pgvp, 1);
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
    
    ensGvvsampleDel(&pthis->Gvvsample);
    
    while(ajListPop(pthis->SubPopulations, (void **) &subgvp))
	ensGvpopulationDel(&subgvp);
    
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
** @fdata [EnsPGvpopulation]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Population attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Population Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetGvvsample Return the Ensembl Genetic Variation Sample
** @nam4rule GetSubPopulation Return the AJAX List of Ensembl Genetic
**                            Variation (sub-) Populations
**
** @argrule * population [const EnsPGvpopulation] Genetic Variation Population
**
** @valrule Adaptor [EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                            Population Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Gvvsample [EnsPGvvsample] Ensembl Genetic Variation Sample
** @valrule SubPopulations [AjPList] AJAX List of Ensembl Genetic Variation
**                                  (Sub-) Populations
**
** @fcategory use
******************************************************************************/




/* @func ensGvpopulationGetAdaptor ********************************************
**
** Get the Ensembl Genetic Variation Population Adaptor element of an
** Ensembl Genetic Variation Population.
**
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
**
** @return [EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                   Population Adaptor
** @@
******************************************************************************/

EnsPGvpopulationadaptor ensGvpopulationGetAdaptor(const EnsPGvpopulation gvp)
{
    if(!gvp)
        return NULL;
    
    return gvp->Adaptor;
}




/* @func ensGvpopulationGetIdentifier *****************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Population.
**
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvpopulationGetIdentifier(const EnsPGvpopulation gvp)
{
    if(!gvp)
        return 0;
    
    return gvp->Identifier;
}




/* @func ensGvpopulationGetGvvsample *******************************************
**
** Get the Ensembl Genetic Variation Sample element of an
** Ensembl Genetic Variation Population.
**
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
**
** @return [EnsPGvvsample] Ensembl Genetic Variation Sample
** @@
******************************************************************************/

EnsPGvvsample ensGvpopulationGetGvvsample(const EnsPGvpopulation gvp)
{
    if(!gvp)
        return NULL;
    
    return gvp->Gvvsample;
}




/* @func ensGvpopulationGetSubPopulations *************************************
**
** Get the AJAX List of Ensembl Genetic Variation (Sub-) Populations of an
** Ensembl Genetic Variation Population.
**
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
**
** @return [const AjPList] AJAX List of Ensembl Genetic Variation Populations
** @@
******************************************************************************/

const AjPList ensGvpopulationGetSubPopulations(const EnsPGvpopulation gvp)
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
** @fdata [EnsPGvpopulation]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Population
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Population Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetName Set the name
** @nam4rule SetSample Set the Ensembl Genetic Variation Sample
**
** @argrule * gvp [EnsPGvpopulation] Ensembl Genetic Variation Population
**                                   object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvpopulationSetAdaptor ********************************************
**
** Set the Ensembl Genetic Variation Population Adaptor element of an
** Ensembl Genetic Variation Population.
**
** @param [u] gvp [EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [r] gvpa [EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                           Population Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationSetAdaptor(EnsPGvpopulation gvp,
                                 EnsPGvpopulationadaptor gvpa)
{
    if(!gvp)
        return ajFalse;
    
    gvp->Adaptor = gvpa;
    
    return ajTrue;
}




/* @func ensGvpopulationSetIdentifier *****************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Population.
**
** @param [u] gvp [EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationSetIdentifier(EnsPGvpopulation gvp, ajuint identifier)
{
    if(!gvp)
        return ajFalse;
    
    gvp->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGvpopulationSetGvvsample *******************************************
**
** Set the Ensembl Genetic Variation Sample element of an
** Ensembl Genetic Variation Population.
**
** @param [u] gvp [EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [u] gvs [EnsPGvvsample] Ensembl Genetic Variation Sample
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationSetGvvsample(EnsPGvpopulation gvp, EnsPGvvsample gvs)
{
    if(!gvp)
        return ajFalse;
    
    ensGvvsampleDel(&gvp->Gvvsample);
    
    gvp->Gvvsample = ensGvvsampleNewRef(gvs);
    
    return ajTrue;
}




/* @func ensGvpopulationAddSubPopulation **************************************
**
** Add an Ensembl Genetic Variation (Sub-) Population to an
** Ensembl Genetic Variation Population.
**
** @cc Bio::EnsEMBL::Variation::Population::add_sub_Population
** @param [u] gvp [EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [u] subgvp [EnsPGvpopulation] Ensembl Genetic Variation Population
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationAddSubPopulation(EnsPGvpopulation gvp,
                                       EnsPGvpopulation subgvp)
{
    if(!gvp)
	return ajFalse;
    
    if(!subgvp)
	return ajFalse;
    
    if(!gvp->SubPopulations)
	gvp->SubPopulations = ajListNew();
    
    ajListPushAppend(gvp->SubPopulations,
		     (void *) ensGvpopulationNewRef(subgvp));
    
    return ajTrue;
}




/* @func ensGvpopulationGetMemSize ********************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Population.
**
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGvpopulationGetMemSize(const EnsPGvpopulation gvp)
{
    ajuint size = 0;
    
    AjIList iter = NULL;
    
    EnsPGvpopulation subgvp = NULL;
    
    if(!gvp)
	return 0;
    
    size += (ajuint) sizeof (EnsOGvpopulation);
    
    size += ensGvvsampleGetMemSize(gvp->Gvvsample);
    
    /*
    ** Summarise the AJAX List of Ensembl Genetic Variation
    ** (Sub-) Populations.
    */

    iter = ajListIterNewread(gvp->SubPopulations);
    
    while(!ajListIterDone(iter))
    {
	subgvp = (EnsPGvpopulation) ajListIterGet(iter);
	
	size += ensGvpopulationGetMemSize(subgvp);
    }
    
    ajListIterDel(&iter);
    
    return size;
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Genetic Variation Population object.
**
** @fdata [EnsPGvpopulation]
** @nam3rule Trace Report Ensembl Genetic Variation Population elements to
**                 debug file
**
** @argrule Trace gvp [const EnsPGvpopulation] Ensembl Genetic Variation
**                                             Population
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvpopulationTrace *************************************************
**
** Trace an Ensembl Genetic Variation Population.
**
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationTrace(const EnsPGvpopulation gvp, ajuint level)
{
    AjIList iter = NULL;
    
    AjPStr indent = NULL;
    
    EnsPGvpopulation subgvp = NULL;
    
    if(!gvp)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensGvpopulationTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Sample %p\n"
	    "%S  SubPopulations %p\n",
	    indent, gvp,
	    indent, gvp->Use,
	    indent, gvp->Identifier,
	    indent, gvp->Adaptor,
	    indent, gvp->Gvvsample,
	    indent, gvp->SubPopulations);
    
    ensGvvsampleTrace(gvp->Gvvsample, level + 1);
    
    /* Trace the AJAX List of Ensembl Genetic Variation (Sub-) Populations. */
    
    if(gvp->SubPopulations)
    {
	ajDebug("%S    AJAX List %p of "
		"Ensembl Genetic Variation (Sub-) Populations\n",
		indent, gvp->SubPopulations);
	
	iter = ajListIterNewread(gvp->SubPopulations);
	
	while(!ajListIterDone(iter))
	{
	    subgvp = (EnsPGvpopulation) ajListIterGet(iter);
	    
	    ensGvpopulationTrace(subgvp, level + 2);
	}
	
	ajListIterDel(&iter);
    }
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @datasection [EnsPGvpopulationadaptor] Genetic Variation Population Adaptor
**
** Functions for manipulating Ensembl Genetic Variation Population Adaptor
** objects
**
** @nam2rule Gvpopulationadaptor
**
******************************************************************************/

static const char *gvPopulationadaptorTables[] =
{
    "population",
    "sample",
    NULL
};




static const char *gvPopulationadaptorColumns[] =
{
    "sample.sample_id",
    "sample.name",
    "sample.size",
    "sample.description",
    NULL
};




static EnsOBaseadaptorLeftJoin gvPopulationadaptorLeftJoin[] =
{
    {NULL, NULL}
};




static const char *gvPopulationadaptorDefaultCondition =
"sample.sample_id = population.sample_id";




static const char *gvPopulationadaptorFinalCondition = NULL;




/* @funcstatic gvPopulationadaptorFetchAllBySQL *******************************
**
** Fetch all Ensembl Genetic Variation Population objects via an SQL statement.
**
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] am [EnsPAssemblymapper] Ensembl Assembly Mapper
** @param [r] slice [EnsPSlice] Ensembl Slice
** @param [u] gvps [AjPList] AJAX List of Ensembl Genetic Variation Population
**                           objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvPopulationadaptorFetchAllBySQL(EnsPDatabaseadaptor dba,
                                               const AjPStr statement,
                                               EnsPAssemblymapper am,
                                               EnsPSlice slice,
                                               AjPList gvps)
{
    ajuint identifier = 0;
    ajuint ssize      = 0;
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
    AjPStr sname        = NULL;
    AjPStr sdescription = NULL;
    
    EnsPGvpopulation gvp         = NULL;
    EnsPGvpopulationadaptor gvpa = NULL;
    
    EnsPGvvsample gvs         = NULL;
    EnsPGvvsampleadaptor gvsa = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    (void) am;
    
    (void) slice;
    
    if(!gvps)
	return ajFalse;
    
    gvpa = ensRegistryGetGvpopulationadaptor(dba);
    
    gvsa = ensGvpopulationadaptorGetAdaptor(gvpa);
    
    sqls = ensDatabaseadaptorSqlstatementNew(dba, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
    {
	identifier   = 0;
	sname        = ajStrNew();
	ssize        = 0;
	sdescription = ajStrNew();
	
	sqlr = ajSqlrowiterGet(sqli);
	
	ajSqlcolumnToUint(sqlr, &identifier);
        ajSqlcolumnToStr(sqlr, &sname);
	ajSqlcolumnToUint(sqlr, &ssize);
        ajSqlcolumnToStr(sqlr, &sdescription);
	
	gvs = ensGvvsampleNew(gvsa,
			     identifier,
			     sname,
			     sdescription,
			     ssize);
	
	gvp = ensGvpopulationNew(gvpa,
				 identifier,
				 gvs,
				 (AjPList) NULL);
	
	ensGvvsampleDel(&gvs);
	
	ajStrDel(&sname);
	ajStrDel(&sdescription);
    }
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    return ajTrue;
}




/* @func ensGvpopulationadaptorNew ********************************************
**
** Default constructor for an Ensembl Genetic Variation Population Adaptor.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Populationadaptor::new
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                   Population Adaptor or NULL
** @@
******************************************************************************/

EnsPGvpopulationadaptor ensGvpopulationadaptorNew(EnsPDatabaseadaptor dba)
{
    EnsPGvpopulationadaptor gvpa = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(gvpa);
    
    gvpa->Adaptor = ensGvvsampleadaptorNew(dba,
					  gvPopulationadaptorTables,
					  gvPopulationadaptorColumns,
					  gvPopulationadaptorLeftJoin,
					  gvPopulationadaptorDefaultCondition,
					  gvPopulationadaptorFinalCondition,
					  gvPopulationadaptorFetchAllBySQL);
    
    if(!gvpa->Adaptor)
	AJFREE(gvpa);
    
    return gvpa;
}




/* @func ensGvpopulationadaptorDel ********************************************
**
** Default destructor for an Ensembl Gentic Variation Population Adaptor.
**
** @param [d] Pgvpa [EnsPGvpopulationadaptor*] Ensembl Genetic Variation
**                                             Population Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGvpopulationadaptorDel(EnsPGvpopulationadaptor *Pgvpa)
{
    EnsPGvpopulationadaptor pthis = NULL;
    
    if(!Pgvpa)
	return;
    
    if(!*Pgvpa)
	return;

    pthis = *Pgvpa;
    
    ensGvvsampleadaptorDel(&pthis->Adaptor);
    
    AJFREE(pthis);

    *Pgvpa = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an
** Ensembl Genetic Variation Population Adaptor object.
**
** @fdata [EnsPGvpopulationadaptor]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Population Adaptor attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Sample Adaptor
**
** @argrule * gvpa [const EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
**
** @valrule Adaptor [EnsPGvvsampleadaptor] Ensembl Genetic Variation
**                                        Sample Adaptor
**
** @fcategory use
******************************************************************************/




/* @func ensGvpopulationadaptorGetAdaptor *************************************
**
** Get the Ensembl Genetic Variation Sample Adaptor element of an
** Ensembl Genetic Variation Population Adaptor.
**
** @param [r] gvpa [const EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
**
** @return [EnsPGvvsampleadaptor] Ensembl Genetic Variation Sample Adaptor
** @@
******************************************************************************/

EnsPGvvsampleadaptor ensGvpopulationadaptorGetAdaptor(
    const EnsPGvpopulationadaptor gvpa)
{
    if(!gvpa)
	return NULL;
    
    return gvpa->Adaptor;
}




/* @func ensGvpopulationadaptorFetchByIdentifier ******************************
**
** Fetch an Ensembl Genetic Variation Population via its
** SQL database-internal identifier.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Population.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Populationadaptor::fetch_by_dbID
** @param [r] gvpa [const EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
** @param [r] identifier [ajuint] SQL database-internal Population identifier
** @param [wP] Pgvp [EnsPGvpopulation*] Ensembl Genetic Variation Population
**                                      address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationadaptorFetchByIdentifier(const EnsPGvpopulationadaptor gvpa,
                                               ajuint identifier,
                                               EnsPGvpopulation *Pgvp)
{
    EnsPBaseadaptor ba = NULL;
    
    if(!gvpa)
	return ajFalse;
    
    if(!identifier)
	return ajFalse;
    
    if(!Pgvp)
	return ajFalse;
    
    ba = ensGvvsampleadaptorGetAdaptor(gvpa->Adaptor);
    
    *Pgvp = (EnsPGvpopulation)
	ensBaseadaptorFetchByIdentifier(ba, identifier);
    
    return ajTrue;
}




/* @func ensGvpopulationadaptorFetchByName ************************************
**
** Fetch an Ensembl Genetic Variation Population by name.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Populationadaptor::fetch_by_name
** @param [r] gvpa [const EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
** @param [r] name [const AjPStr] Ensembl Genetic Variation Population name
** @param [w] Pgvp [EnsPGvpopulation] Ensembl Genetic Variation Population
**                                    address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationadaptorFetchByName(const EnsPGvpopulationadaptor gvpa,
                                         const AjPStr name,
                                         EnsPGvpopulation* Pgvp)
{
    char *txtname = NULL;
    
    AjPList gvps = NULL;
    
    AjPStr constraint = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    EnsPGvpopulation gvp = NULL;
    
    if(!gvpa)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    if(!Pgvp)
	return ajFalse;
    
    ba = ensGvvsampleadaptorGetAdaptor(gvpa->Adaptor);
    
    ensBaseadaptorEscapeC(ba, &txtname, name);
    
    constraint = ajFmtStr("sample.name = '%s'", txtname);
    
    ajCharDel(&txtname);
    
    gvps = ajListNew();
    
    ensBaseadaptorGenericFetch(ba,
			       constraint,
			       (EnsPAssemblymapper) NULL,
			       (EnsPSlice) NULL,
			       gvps);
    
    /* FIXME: Should we warn here? */
    if(ajListGetLength(gvps) == 0)
	ajDebug("ensGvpopulationadaptorFetchByName could not get an "
		"Ensembl Genetic Variation Population "
		"for name '%S'.\n",
		name);
    
    if(ajListGetLength(gvps) > 1)
	ajDebug("ensGvpopulationadaptorFetchByName got more than one "
		"Ensembl Genetic Variation Population "
		"for name '%S'.\n",
		name);
    
    ajListPop(gvps, (void **) Pgvp);
    
    while(ajListPop(gvps, (void **) &gvp))
	ensGvpopulationDel(&gvp);
    
    ajListFree(&gvps);
    
    ajStrDel(&constraint);
    
    return ajTrue;
}




/* @func ensGvindividualadaptorFetchAllBySynonym ******************************
**
** Fetch all Ensembl Genetic Variation Individuals by an
** Ensembl Genetic Variation Sample synonym.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Populationadaptor::
**     fetch_population_by_synonym
** @param [r] gvpa [const EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
** @param [r] synonym [const AjPStr] Ensembl Genetic Variation Sample synonym
** @param [u] gvps [AjPList] AJAX List of Ensembl Genetic Variation Populations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationadaptorFetchAllBySynonym(const EnsPGvpopulationadaptor gvpa,
                                               const AjPStr synonym,
                                               AjPList gvps)
{
    ajuint *Pidentifier = 0;
    
    AjPList idlist = NULL;
    
    EnsPGvpopulation gvp = NULL;
    
    if(!gvpa)
	return ajFalse;
    
    if(!synonym)
	return ajFalse;
    
    if(!gvps)
	return ajFalse;
    
    idlist = ajListNew();
    
    ensGvvsampleadaptorFetchAllIdentifiersBySynonym(gvpa->Adaptor,
						   synonym,
						   (const AjPStr) NULL,
						   idlist);
    
    while(ajListPop(idlist, (void **) &Pidentifier))
    {
	ensGvpopulationadaptorFetchByIdentifier(gvpa,
						*Pidentifier,
						&gvp);
	
	ajListPushAppend(gvps, (void *) gvp);
	
	AJFREE(Pidentifier);
    }
    
    ajListFree(&idlist);
    
    return ajTrue;
}




/* @func ensGvpopulationadaptorFetchAllBySuperPopulation **********************
**
** Fetch all Ensembl Genetic Variation Populations by a super Population.
**
** @param [r] gvpa [const EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [u] gvps [AjPList] AJAX List of Ensembl Genetic Variation Populations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationadaptorFetchAllBySuperPopulation(
    const EnsPGvpopulationadaptor gvpa,
    const EnsPGvpopulation gvp,
    AjPList gvps)
{
    AjPStr statement = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!gvpa)
	return ajFalse;
    
    if(!gvp)
	return ajFalse;
    
    if(!gvps)
	return ajFalse;
    
    if(!gvp->Identifier)
    {
	ajDebug("ensGvpopulationadaptorFetchAllBySuperPopulation "
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
    
    ba = ensGvvsampleadaptorGetAdaptor(gvpa->Adaptor);
    
    dba = ensBaseadaptorGetDatabaseadaptor(ba);
    
    gvPopulationadaptorFetchAllBySQL(dba,
				     statement,
				     (EnsPAssemblymapper) NULL,
				     (EnsPSlice) NULL,
				     gvps);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensGvpopulationadaptorFetchAllBySubPopulation ************************
**
** Fetch all Ensembl Genetic Variation Populations by a sub Population.
**
** @param [r] gvpa [const EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
** @param [r] gvp [const EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [u] gvps [AjPList] AJAX List of Ensembl Genetic Variation Populations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationadaptorFetchAllBySubPopulation(
    const EnsPGvpopulationadaptor gvpa,
    const EnsPGvpopulation gvp,
    AjPList gvps)
{
    AjPStr statement = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!gvpa)
	return ajFalse;
    
    if(!gvp)
	return ajFalse;
    
    if(!gvps)
	return ajFalse;
    
    if(!gvp->Identifier)
    {
	ajDebug("ensGvpopulationadaptorFetchAllBySubPopulation "
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
    
    ba = ensGvvsampleadaptorGetAdaptor(gvpa->Adaptor);
    
    dba = ensBaseadaptorGetDatabaseadaptor(ba);
    
    gvPopulationadaptorFetchAllBySQL(dba,
				     statement,
				     (EnsPAssemblymapper) NULL,
				     (EnsPSlice) NULL,
				     gvps);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensGvpopulationadaptorFetchAllByIndvividual **************************
**
** Fetch all Ensembl Genetic Variation Populations by an
** Ensembl Genetic Variation Individual.
**
** @param [r] gvpa [const EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                                 Population Adaptor
** @param [r] gvi [const EnsPGvindividual] Ensembl Genetic Variation Individual
** @param [u] gvps [AjPList] AJAX List of Ensembl Genetic Variation Populations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvpopulationadaptorFetchAllByIndvividual(
    const EnsPGvpopulationadaptor gvpa,
    const EnsPGvindividual gvi,
    AjPList gvps)
{
    AjPStr statement = NULL;
    
    EnsPBaseadaptor ba = NULL;
    
    EnsPDatabaseadaptor dba = NULL;
    
    if(!gvpa)
	return ajFalse;
    
    if(!gvi)
	return ajFalse;
    
    if(!gvps)
	return ajFalse;
    
    if(!gvi->Identifier)
    {
	ajDebug("ensGvpopulationadaptorFetchAllByIndvividual "
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
    
    ba = ensGvvsampleadaptorGetAdaptor(gvpa->Adaptor);
    
    dba = ensBaseadaptorGetDatabaseadaptor(ba);
    
    gvPopulationadaptorFetchAllBySQL(dba,
				     statement,
				     (EnsPAssemblymapper) NULL,
				     (EnsPSlice) NULL,
				     gvps);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @datasection [EnsPGvallele] Genetic Variation Allele ***********************
**
** Functions for manipulating Ensembl Genetic Variation Allele objects
**
** @nam2rule Gvallele
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Allele by pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Allele. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvallele]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvallele] Ensembl Genetic Variation Allele
** @argrule Ref object [EnsPGvallele] Ensembl Genetic Variation Allele
**
** @valrule * [EnsPGvallele] Ensembl Genetic Variation Allele
**
** @fcategory new
******************************************************************************/




/* @func ensGvalleleNew *******************************************************
**
** Default constructor for an Ensembl Genetic Variation Allele.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvaa [EnsPGvalleleadaptor] Ensembl Genetic Variation
**                                       Allele Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Allele::new
** @param [u] gvp [EnsPGvpopulation] Ensembl Genetic Variation Population
** @param [u] allelestr [AjPStr] Allele
** @param [r] frequency [float] Frequency
**
** @return [EnsPGvallele] Ensembl Genetic Variation Allele
** @@
******************************************************************************/

EnsPGvallele ensGvalleleNew(EnsPGvalleleadaptor gvaa,
                            ajuint identifier,
                            EnsPGvpopulation gvp,
                            AjPStr allelestr,
                            float frequency)
{
    EnsPGvallele gva = NULL;
    
    if(!gvp)
	return NULL;
    
    if(!allelestr)
	return NULL;
    
    AJNEW0(gva);
    
    gva->Use = 1;
    
    gva->Identifier = identifier;
    
    gva->Adaptor = gvaa;
    
    gva->Gvpopulation = ensGvpopulationNewRef(gvp);
    
    if(allelestr)
	gva->Allele = ajStrNewS(allelestr);
    
    gva->Frequency = frequency;
    
    return gva;
}




/* @func ensGvalleleNewObj ****************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [EnsPGvallele] Ensembl Genetic Variation Allele or NULL
** @@
******************************************************************************/

EnsPGvallele ensGvalleleNewObj(const EnsPGvallele object)
{
    EnsPGvallele gva = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(gva);
    
    gva->Use = 1;
    
    gva->Identifier = object->Identifier;
    
    gva->Adaptor = object->Adaptor;
    
    gva->Gvpopulation = ensGvpopulationNewRef(object->Gvpopulation);
    
    if(object->Allele)
	gva->Allele = ajStrNewRef(object->Allele);
    
    gva->Frequency = object->Frequency;
    
    return gva;
}




/* @func ensGvalleleNewRef ****************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [EnsPGvallele] Ensembl Genetic Variation Allele
** @@
******************************************************************************/

EnsPGvallele ensGvalleleNewRef(EnsPGvallele gva)
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
** @fdata [EnsPGvallele]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Allele object
**
** @argrule * Pgva [EnsPGvallele*] Genetic Variation Allele object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvalleleDel *******************************************************
**
** Default destructor for an Ensembl Genetic Variation Allele.
**
** @param [d] Pgva [EnsPGvallele*] Ensembl Genetic Variation Allele address
**
** @return [void]
** @@
******************************************************************************/

void ensGvalleleDel(EnsPGvallele *Pgva)
{
    EnsPGvallele pthis = NULL;
    
    /*
     ajDebug("ensGvalleleDel\n"
	     "  *Pgva %p\n",
	     *Pgva);
     
     ensGvalleleTrace(*Pgva, 1);
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
    
    ensGvpopulationDel(&pthis->Gvpopulation);
    
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
** @fdata [EnsPGvallele]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Allele attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Allele Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetGvpopulation Return the Ensembl Genetic Variation Population
** @nam4rule GetAllele Return the allele
** @nam4rule GetFrequency Return the frequency
**
** @argrule * gva [const EnsPGvallele] Genetic Variation Allele
**
** @valrule Adaptor [EnsPGvalleleadaptor] Ensembl Genetic Variation
**                                        Allele Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Gvpopulation [EnsPGvpopulation] Ensembl Genetic Variation
**                                          Population
** @valrule Allele [AjPStr] Allele
** @valrule Frequency [float] Frequency
**
** @fcategory use
******************************************************************************/




/* @func ensGvalleleGetAdaptor ************************************************
**
** Get the Ensembl Genetic Variation Allele Adaptor element of an
** Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [EnsPGvalleleadaptor] Ensembl Genetic Variation Allele Adaptor
** @@
******************************************************************************/

EnsPGvalleleadaptor ensGvalleleGetAdaptor(const EnsPGvallele gva)
{
    if(!gva)
        return NULL;
    
    return gva->Adaptor;
}




/* @func ensGvalleleGetIdentifier *********************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvalleleGetIdentifier(const EnsPGvallele gva)
{
    if(!gva)
        return 0;
    
    return gva->Identifier;
}




/* @func ensGvalleleGetPopulation *********************************************
**
** Get the Ensembl Genetic Variation Population element of an
** Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [EnsPGvpopulation] Ensembl Genetic Variation Population
** @@
******************************************************************************/

EnsPGvpopulation ensGvalleleGetPopulation(const EnsPGvallele gva)
{
    if(!gva)
        return NULL;
    
    return gva->Gvpopulation;
}




/* @func ensGvalleleGetAllele *************************************************
**
** Get the allele element of an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [AjPStr] Allele
** @@
******************************************************************************/

AjPStr ensGvalleleGetAllele(const EnsPGvallele gva)
{
    if(!gva)
        return NULL;
    
    return gva->Allele;
}




/* @func ensGvalleleGetFrequency **********************************************
**
** Get the frequency element of an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [float] Frequency
** @@
******************************************************************************/

float ensGvalleleGetFrequency(const EnsPGvallele gva)
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
** @fdata [EnsPGvallele]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Allele
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Allele Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetGvpopulation Set the Ensembl Genetic Variation Population
** @nam4rule SetAllele Set the allele
** @nam4rule SetFrequency Set the frequency
**
** @argrule * gva [EnsPGvallele] Ensembl Genetic Variation Allele object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvalleleSetAdaptor ************************************************
**
** Set the Ensembl Genetic Variation Allele Adaptor element of an
** Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
** @param [r] gvaa [EnsPGvalleleadaptor] Ensembl Genetic Variation
**                                       Allele Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleSetAdaptor(EnsPGvallele gva, EnsPGvalleleadaptor gvaa)
{
    if(!gva)
        return ajFalse;
    
    gva->Adaptor = gvaa;
    
    return ajTrue;
}




/* @func ensGvalleleSetIdentifier *********************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleSetIdentifier(EnsPGvallele gva, ajuint identifier)
{
    if(!gva)
        return ajFalse;
    
    gva->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGvalleleSetPopulation *********************************************
**
** Set the Ensembl Genetic Variation Population element of an
** Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
** @param [u] gvp [EnsPGvpopulation] Ensembl Genetic Variation Population
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleSetPopulation(EnsPGvallele gva, EnsPGvpopulation gvp)
{
    if(!gva)
        return ajFalse;
    
    ensGvpopulationDel(&gva->Gvpopulation);
    
    gva->Gvpopulation = ensGvpopulationNewRef(gvp);
    
    return ajTrue;
}




/* @func ensGvalleleSetAllele *************************************************
**
** Set the allele element of an Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
** @param [u] allelestr [AjPStr] Allele
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleSetAllele(EnsPGvallele gva, AjPStr allelestr)
{
    if(!gva)
        return ajFalse;
    
    ajStrDel(&gva->Allele);
    
    if(allelestr)
	gva->Allele = ajStrNewRef(allelestr);
    
    return ajTrue;
}




/* @func ensGvalleleSetFrequency **********************************************
**
** Set the frequency element of an Ensembl Genetic Variation Allele.
**
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
** @param [r] frequency [float] Frequency
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleSetFrequency(EnsPGvallele gva, float frequency)
{
    if(!gva)
        return ajFalse;
    
    gva->Frequency = frequency;
    
    return ajTrue;
}




/* @func ensGvalleleGetMemSize ************************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGvalleleGetMemSize(const EnsPGvallele gva)
{
    ajuint size = 0;
    
    if(!gva)
	return 0;
    
    size += (ajuint) sizeof (EnsOGvallele);
    
    size += ensGvpopulationGetMemSize(gva->Gvpopulation);
    
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
** @fdata [EnsPGvallele]
** @nam3rule Trace Report Ensembl Genetic Variation Allele elements to
**                 debug file
**
** @argrule Trace gva [const EnsPGvallele] Ensembl Genetic Variation Allele
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvalleleTrace *****************************************************
**
** Trace an Ensembl Genetic Variation Allele.
**
** @param [r] gva [const EnsPGvallele] Ensembl Genetic Variation Allele
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvalleleTrace(const EnsPGvallele gva, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!gva)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensGvalleleTrace %p\n"
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
	    indent, gva->Gvpopulation,
	    indent, gva->Allele,
	    indent, gva->Frequency);
    
    ensGvpopulationTrace(gva->Gvpopulation, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @datasection [EnsPGvgenotype] Genetic Variation Genotype *******************
**
** Functions for manipulating Ensembl Genetic Variation Genotype objects
**
** @nam2rule Gvgenotype
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Genotype by pointer.
** It is the responsibility of the user to first destroy any previous
** Genetic Variation Genotype. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvgenotype]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @argrule Ref object [EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @valrule * [EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @fcategory new
******************************************************************************/




/* @func ensGvgenotypeNew *****************************************************
**
** Default constructor for an Ensembl Genetic Variation Genotype.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvga [EnsPGvgenotypeadaptor] Ensembl Genetic Variation
**                                         Genotype Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Genotype::new
** @param [u] allele1 [AjPStr] Allele 1
** @param [u] allele2 [AjPStr] Allele 2
**
** @return [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @@
******************************************************************************/

EnsPGvgenotype ensGvgenotypeNew(EnsPGvgenotypeadaptor gvga,
                                ajuint identifier,
                                AjPStr allele1,
                                AjPStr allele2)
{
    EnsPGvgenotype gvg = NULL;
    
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




/* @func ensGvgenotypeNewObj **************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [EnsPGvgenotype] Ensembl Genetic Variation Genotype or NULL
** @@
******************************************************************************/

EnsPGvgenotype ensGvgenotypeNewObj(const EnsPGvgenotype object)
{
    EnsPGvgenotype gvg = NULL;
    
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




/* @func ensGvgenotypeNewRef **************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvg [EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @@
******************************************************************************/

EnsPGvgenotype ensGvgenotypeNewRef(EnsPGvgenotype gvg)
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
** @fdata [EnsPGvgenotype]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Genotype object
**
** @argrule * Pgvg [EnsPGvgenotype*] Genetic Variation Genotype
**                                   object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvgenotypeDel *****************************************************
**
** Default destructor for an Ensembl Genetic Variation Genotype.
**
** @param [d] Pgvg [EnsPGvgenotype*] Ensembl Genetic Variation Genotype
**                                   address
**
** @return [void]
** @@
******************************************************************************/

void ensGvgenotypeDel(EnsPGvgenotype *Pgvg)
{
    EnsPGvgenotype pthis = NULL;
    
    /*
     ajDebug("ensGvgenotypeDel\n"
	     "  *Pgvg %p\n",
	     *Pgvg);
     
     ensGvgenotypeTrace(*Pgvg, 1);
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
** @fdata [EnsPGvgenotype]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Genotype attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Genotype Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetAllele1 Return the allele 1
** @nam4rule GetAllele2 Return the allele 2
**
** @argrule * gvg [const EnsPGvgenotype] Genetic Variation Genotype
**
** @valrule Adaptor [EnsPGvgenotypeadaptor] Ensembl Genetic Variation
**                                          Genotype Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Allele1 [AjPStr] Allele 1
** @valrule Allele2 [AjPStr] Allele 2
**
** @fcategory use
******************************************************************************/




/* @func ensGvgenotypeGetAdaptor **********************************************
**
** Get the Ensembl Genetic Variation Genotype Adaptor element of an
** Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [EnsPGvgenotypeadaptor] Ensembl Genetic Variation Genotype Adaptor
** @@
******************************************************************************/

EnsPGvgenotypeadaptor ensGvgenotypeGetAdaptor(const EnsPGvgenotype gvg)
{
    if(!gvg)
        return NULL;
    
    return gvg->Adaptor;
}




/* @func ensGvgenotypeGetIdentifier *******************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvgenotypeGetIdentifier(const EnsPGvgenotype gvg)
{
    if(!gvg)
        return 0;
    
    return gvg->Identifier;
}




/* @func ensGvgenotypeGetAllele1 **********************************************
**
** Get the allele 1 element of an Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [AjPStr] Allele 1
** @@
******************************************************************************/

AjPStr ensGvgenotypeGetAllele1(const EnsPGvgenotype gvg)
{
    if(!gvg)
        return NULL;
    
    return gvg->Allele1;
}




/* @func ensGvgenotypeGetAllele2 **********************************************
**
** Get the allele 2 element of an Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [AjPStr] Allele 2
** @@
******************************************************************************/

AjPStr ensGvgenotypeGetAllele2(const EnsPGvgenotype gvg)
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
** @fdata [EnsPGvgenotype]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Genotype
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation Genotype Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetAllele1 Set the allele 1
** @nam4rule SetAllele2 Set the allele 2
**
** @argrule * gvg [EnsPGvgenotype] Ensembl Genetic Variation Genotype object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvgenotypeSetAdaptor **********************************************
**
** Set the Ensembl Genetic Variation Genotype Adaptor element of an
** Ensembl Genetic Variation Genotype.
**
** @param [u] gvg [EnsPGvvsample] Ensembl Genetic Variation Genotype
** @param [u] gvga [EnsPGvgenotypeadaptor] Ensembl Genetic Variation
**                                         Genotype Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvgenotypeSetAdaptor(EnsPGvgenotype gvg, EnsPGvgenotypeadaptor gvga)
{
    if(!gvg)
        return ajFalse;
    
    gvg->Adaptor = gvga;
    
    return ajTrue;
}




/* @func ensGvgenotypeSetIdentifier *******************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Genotype.
**
** @param [u] gvg [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvgenotypeSetIdentifier(EnsPGvgenotype gvg, ajuint identifier)
{
    if(!gvg)
        return ajFalse;
    
    gvg->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGvgenotypeSetAllele1 **********************************************
**
** Set the allele 1 element of an Ensembl Genetic Variation Genotype.
**
** @param [u] gvg [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @param [u] allele1 [AjPStr] Allele 1
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvgenotypeSetAllele1(EnsPGvgenotype gvg, AjPStr allele1)
{
    if(!gvg)
        return ajFalse;
    
    ajStrDel(&gvg->Allele1);
    
    if(allele1)
	gvg->Allele1 = ajStrNewRef(allele1);
    
    return ajTrue;
}




/* @func ensGvgenotypeSetAllele2 **********************************************
**
** Set the allele 2 element of an Ensembl Genetic Variation Genotype.
**
** @param [u] gvg [EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @param [u] allele2 [AjPStr] Allele 2
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvgenotypeSetAllele2(EnsPGvgenotype gvg, AjPStr allele2)
{
    if(!gvg)
        return ajFalse;
    
    ajStrDel(&gvg->Allele2);
    
    if(allele2)
	gvg->Allele2 = ajStrNewRef(allele2);
    
    return ajTrue;
}




/* @func ensGvgenotypeGetMemSize **********************************************
**
** Get the memory size in bytes of an Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGvgenotypeGetMemSize(const EnsPGvgenotype gvg)
{
    ajuint size = 0;
    
    if(!gvg)
	return 0;
    
    size += (ajuint) sizeof (EnsOGvgenotype);
    
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
** @fdata [EnsPGvgenotype]
** @nam3rule Trace Report Ensembl Genetic Variation Genotype elements to
**                 debug file
**
** @argrule Trace gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvgenotypeTrace ***************************************************
**
** Trace an Ensembl Genetic Variation Genotype.
**
** @param [r] gvg [const EnsPGvgenotype] Ensembl Genetic Variation Genotype
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvgenotypeTrace(const EnsPGvgenotype gvg, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!gvg)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensGvgenotypeTrace %p\n"
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




/* @datasection [EnsPGvvariation] Genetic Variation Variation *****************
**
** Functions for manipulating Ensembl Genetic Variation Variation objects
**
** @nam2rule Gvvariation
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
** @fdata [EnsPGvvariation]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvvariation] Ensembl Genetic Variation Variation
** @argrule Ref object [EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @valrule * [EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @fcategory new
******************************************************************************/




/* @func ensGvvariationNew ****************************************************
**
** Default constructor for an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [u] gvva [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::Variation::new
** @param [u] name [AjPStr] Name
** @param [u] source [AjPStr] Source
** @param [u] ancestralallele [AjPStr] Ancestral allele
** @param [r] synonyms [AjPTable] Synonyms, keys are source databases and
**                                values are AJAX Lists of AJAX Strings of
**                                synonyms
** @param [r] alleles [AjPList] AJAX List of Ensembl Alleles
** @param [r] validationstates [AjPList] AJAX List of validation state
**                                       AJAX Strings
** @param [u] moltype [AjPStr] Molecule type
** @param [u] fiveflank [AjPStr] Five prime flanking sequence
** @param [u] threeflank [AjPStr] Three prime flanking sequence
** @param [u] faileddescription [AjPStr] Failed description
**
** @return [EnsPGvvariation] Ensembl Genetic Variation Variation
** @@
******************************************************************************/

EnsPGvvariation ensGvvariationNew(EnsPGvvariationadaptor gvva,
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
    
    EnsPGvallele gva = NULL;
    
    EnsPGvvariation gvv = NULL;
    
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
	
	ajTableToarrayKeysValues(synonyms, &keyarray, &valarray);
	
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
    
    gvv->Gvalleles = ajListNew();
    
    iter = ajListIterNew(alleles);
    
    while(ajListIterDone(iter))
    {
	gva = (EnsPGvallele) ajListIterGet(iter);
	
	ajListPushAppend(gvv->Gvalleles, (void *) ensGvalleleNewRef(gva));
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




/* @func ensGvvariationNewObj *************************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvvariation] Ensembl Genetic Variation
**                                           Variation
**
** @return [EnsPGvvariation] Ensembl Genetic Variation Variation or NULL
** @@
******************************************************************************/

EnsPGvvariation ensGvvariationNewObj(const EnsPGvvariation object)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    AjIList iter = NULL;
    AjPList list = NULL;
    
    AjPStr synonym = NULL;
    AjPStr vstate = NULL;
    
    EnsPGvallele gva = NULL;
    
    EnsPGvvariation gvv = NULL;
    
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
	
	ajTableToarrayKeysValues(object->Synonyms, &keyarray, &valarray);
	
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
    
    gvv->Gvalleles = ajListNew();
    
    iter = ajListIterNew(object->Gvalleles);
    
    while(!ajListIterDone(iter))
    {
	gva = (EnsPGvallele) ajListIterGet(iter);
	
	ajListPushAppend(gvv->Gvalleles, (void *) ensGvalleleNewRef(gva));
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




/* @func ensGvvariationNewRef *************************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [EnsPGvvariation] Ensembl Genetic Variation Variation
** @@
******************************************************************************/

EnsPGvvariation ensGvvariationNewRef(EnsPGvvariation gvv)
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
** @fdata [EnsPGvvariation]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Variation object
**
** @argrule * Pgvv [EnsPGvvariation*] Genetic Variation Variation
**                                    object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvvariationDel ****************************************************
**
** Default destructor for an Ensembl Genetic Variation Variation.
**
** @param [d] Pgvv [EnsPGvvariation*] Ensembl Genetic Variation
**                                    Variation address
**
** @return [void]
** @@
******************************************************************************/

void ensGvvariationDel(EnsPGvvariation *Pgvv)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    
    register ajuint i = 0;
    
    AjPStr synonym = NULL;
    
    EnsPGvallele gva = NULL;

    EnsPGvvariation pthis = NULL;
    
    
    /*
     ajDebug("ensGvvariationDel\n"
	     "  *Pgvv %p\n",
	     *Pgvv);
     
     ensGvvariationTrace(*Pgvv, 1);
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
	ajTableToarrayKeysValues(pthis->Synonyms, &keyarray, &valarray);
	
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
    
    while(ajListPop(pthis->Gvalleles, (void **) &gva))
	ensGvalleleDel(&gva);
    
    ajListFree(&pthis->Gvalleles);
    
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
** @fdata [EnsPGvvariation]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Variation attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation Variation Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetName Return the name
** @nam4rule GetSource Return the source
** @nam4rule GetSynoyms Return synonms
** @nam4rule GetAncestralAllele Return the ancestral allele
** @nam4rule GetGvalleles Return Ensembl Genetic Variation Alleles
** @nam4rule GetValidationStates Return the validation states
** @nam4rule GetMoleculeType Return the molecule type
** @nam4rule GetFivePrimeFlank Return the five prime flank
** @nam4rule GetThreePrimeFlank Return the three prime flank
** @nam4rule GetFailedDescription Return the failed description
**
** @argrule * gvv [const EnsPGvvariation] Genetic Variation Variation
**
** @valrule Adaptor [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                           Variation Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Name [AjPStr] Name
** @valrule Source [AjPStr] Source
** @valrule Synonyms [const AjPTable] Synoyms
** @valrule AncestralAllele [AjPStr] Ancestral allele
** @valrule Gvalleles [const AjPList] Ensembl Genetic Variation Alleles
** @valrule ValidationStates [const AjPList] Validation states
** @valrule MoleculeType [AjPStr] Molecule type
** @valrule fiveprimeflank [AjPStr] Five prime flank
** @valrule threeprimeflank [AjPStr] Three prime flank
** @valrule faileddescription [AjPStr] Failed description
**
** @fcategory use
******************************************************************************/




/* @func ensGvvariationGetAdaptor *********************************************
**
** Get the Ensembl Genetic Variation Variation Adaptor element of an
** Ensembl Genetic Variation Variation.
**
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                  Variation Adaptor
** @@
******************************************************************************/

EnsPGvvariationadaptor ensGvvariationGetAdaptor(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->Adaptor;
}




/* @func ensGvvariationGetIdentifier ******************************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation.
**
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvvariationGetIdentifier(const EnsPGvvariation gvv)
{
    if(!gvv)
        return 0;
    
    return gvv->Identifier;
}




/* @func ensGvvariationGetName ************************************************
**
** Get the name element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::name
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensGvvariationGetName(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->Name;
}




/* @func ensGvvariationGetSource **********************************************
**
** Get the source element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::source
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Source
** @@
******************************************************************************/

AjPStr ensGvvariationGetSource(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->Source;
}




/* @func ensGvvariationGetSynonyms ********************************************
**
** Get the synonyms element of an Ensembl Genetic Variation Variation.
** Synonyms are stored in an AJAX Table of source AJAX String key data and
** AJAX List value data. The AJAX Lists contain synonym AJAX Strings.
**
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
** @see ensGvvariationFetchAllSynonyms
** @see ensGvvariationFetchAllSynonymSources
**
** @return [const AjPTable] Synonyms
** @@
******************************************************************************/

const AjPTable ensGvvariationGetSynonyms(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->Synonyms;
}




/* @func ensGvvariationGetAncestralAllele *************************************
**
** Get the ancestral allele element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::ancestral_allele
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Ancestral allele
** @@
******************************************************************************/

AjPStr ensGvvariationGetAncestralAllele(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->AncestralAllele;
}




/* @func ensGvvariationGetGvalleles *******************************************
**
** Get all Ensembl Genetic Variation Alleles of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::get_all_Alleles
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [const AjPList] AJAX List of Ensembl Genetic Variation Alleles
** @@
******************************************************************************/

const AjPList ensGvvariationGetGvalleles(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->Gvalleles;
}




/* @func ensGvvariationGetValidationStates ************************************
**
** Get all validation states of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::get_all_validation_states
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [const AjPList] AJAX List of AJAX String validation states
** @@
******************************************************************************/

const AjPList ensGvvariationGetValidationStates(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->ValidationStates;
}




/* @func ensGvvariationGetMoleculeType ****************************************
**
** Get the molecule type element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::moltype
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Molecule type
** @@
******************************************************************************/

AjPStr ensGvvariationGetMoleculeType(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->MoleculeType;
}




/* @func ensGvvariationGetFivePrimeFlank **************************************
**
** Get the five prime flank element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::five_prime_flanking_seq
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Five prime flank
** @@
******************************************************************************/

AjPStr ensGvvariationGetFivePrimeFlank(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->FivePrimeFlank;
}




/* @func ensGvvariationGetThreePrimeFlank *************************************
**
** Get the three prime flank element of an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::three_prime_flanking_seq
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Three prime flank
** @@
******************************************************************************/

AjPStr ensGvvariationGetThreePrimeFlank(const EnsPGvvariation gvv)
{
    if(!gvv)
        return NULL;
    
    return gvv->ThreePrimeFlank;
}




/* @func ensGvvariationGetFailedDescription ***********************************
**
** Get the failed description element of an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::failed_description
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjPStr] Three prime flank
** @@
******************************************************************************/

AjPStr ensGvvariationGetFailedDescription(const EnsPGvvariation gvv)
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
** @fdata [EnsPGvvariation]
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
** @argrule * gvv [EnsPGvvariation] Ensembl Genetic Variation Variation object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvvariationSetAdaptor *********************************************
**
** Set the Ensembl Genetic Variation Variation Adaptor element of an
** Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [r] gvva [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetAdaptor(EnsPGvvariation gvv,
                                EnsPGvvariationadaptor gvva)
{
    if(!gvv)
        return ajFalse;
    
    gvv->Adaptor = gvva;
    
    return ajTrue;
}




/* @func ensGvvariationSetIdentifier ******************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetIdentifier(EnsPGvvariation gvv, ajuint identifier)
{
    if(!gvv)
        return ajFalse;
    
    gvv->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGvvariationSetName ************************************************
**
** Set the name element of an Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetName(EnsPGvvariation gvv, AjPStr name)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->Name);
    
    if(name)
	gvv->Name = ajStrNewRef(name);
    
    return ajTrue;
}




/* @func ensGvvariationSetSource **********************************************
**
** Set the source element of an Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] source [AjPStr] Source
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetSource(EnsPGvvariation gvv, AjPStr source)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->Source);
    
    if(source)
	gvv->Source = ajStrNewRef(source);
    
    return ajTrue;
}




/* @func ensGvvariationSetAncestralAllele *************************************
**
** Set the ancestral allele element of an Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] ancestralallele [AjPStr] Ancestral allele
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetAncestralAllele(EnsPGvvariation gvv,
                                        AjPStr ancestralallele)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->AncestralAllele);
    
    if(ancestralallele)
	gvv->AncestralAllele = ajStrNewRef(ancestralallele);
    
    return ajTrue;
}




/* @func ensGvvariationSetMoleculeType ****************************************
**
** Set the molecule type element of an Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] moltype [AjPStr] Molecule type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetMoleculeType(EnsPGvvariation gvv, AjPStr moltype)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->AncestralAllele);
    
    if(moltype)
	gvv->AncestralAllele = ajStrNewRef(moltype);
    
    return ajTrue;
}




/* @func ensGvvariationSetFivePrimeFlank **************************************
**
** Set the five prime flank element of an Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] fiveflank [AjPStr] Five prime flank
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetFivePrimeFlank(EnsPGvvariation gvv, AjPStr fiveflank)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->FivePrimeFlank);
    
    if(fiveflank)
	gvv->FivePrimeFlank = ajStrNewRef(fiveflank);
    
    return ajTrue;
}




/* @func ensGvvariationSetThreePrimeFlank *************************************
**
** Set the three prime flank element of an Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] threeflank [AjPStr] Three prime flank
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetThreePrimeFlank(EnsPGvvariation gvv, AjPStr threeflank)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->ThreePrimeFlank);
    
    if(threeflank)
	gvv->ThreePrimeFlank = ajStrNewRef(threeflank);
    
    return ajTrue;
}




/* @func ensGvvariationSetFailedDescription ***********************************
**
** Set the failed description element of an
** Ensembl Genetic Variation Variation.
**
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] faileddescription [AjPStr] Failed description
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationSetFailedDescription(EnsPGvvariation gvv,
                                          AjPStr faileddescription)
{
    if(!gvv)
        return ajFalse;
    
    ajStrDel(&gvv->FailedDescription);
    
    if(faileddescription)
	gvv->FailedDescription = ajStrNewRef(faileddescription);
    
    return ajTrue;
}




/* @func ensGvvariationGetMemSize *********************************************
**
** Get the memory size in bytes of an
** Ensembl Genetic Variation Variation.
**
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGvvariationGetMemSize(const EnsPGvvariation gvv)
{
    ajuint size = 0;
    
    if(!gvv)
	return 0;
    
    size += (ajuint) sizeof (EnsOGvvariation);
    
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
** @fdata [EnsPGvvariation]
** @nam3rule Trace Report Ensembl Genetic Variation Variation elements
**                 to debug file
**
** @argrule Trace gvv [const EnsPGvvariation] Ensembl Genetic Variation
**                                            Variation
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvvariationTrace **************************************************
**
** Trace an Ensembl Genetic Variation Variation.
**
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationTrace(const EnsPGvvariation gvv, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!gvv)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("ensGvvariationTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Name '%S'\n"
	    "%S  Source '%S'\n"
	    "%S  Synonyms %p\n"
	    "%S  AncestralAllele '%S'\n"
	    "%S  Gvalleles %p\n"
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
	    indent, gvv->Gvalleles,
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




/* @func ensGvvariationAddSynonym *********************************************
**
** Add a synonym to an Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::add_synonym
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] source [AjPStr] Source database
** @param [u] synonym [AjPStr] Synonym
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationAddSynonym(EnsPGvvariation gvv, AjPStr source,
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




/* @func ensGvvariationAddGvallele ********************************************
**
** Add an Ensembl Genetic Variation Allele to an
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::Variation::add_Allele
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] gva [EnsPGvallele] Ensembl Genetic Variation Allele
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationAddGvallele(EnsPGvvariation gvv, EnsPGvallele gva)
{
    if(!gvv)
	return ajFalse;
    
    if(!gva)
	return ajFalse;
    
    ajListPushAppend(gvv->Gvalleles, (void *) ensGvalleleNewRef(gva));
    
    return ajTrue;
}




/* @func ensGvvariationValidationStateFromStr *********************************
**
** Convert an AJAX String into an Ensembl Genetic Variaton Variation
** validation state element.
**
** @param [r] state [const AjPStr] Validation state string
**
** @return [AjEnum] Ensembl Genetic Variation validation state element or
**                  ensEGvvariationValidationStateNULL
** @@
******************************************************************************/

AjEnum ensGvvariationValidationStateFromStr(const AjPStr state)
{
    register ajint i = 0;
    
    AjEnum estate = ensEGvvariationValidationStateNULL;
    
    for(i = 1; gvVariationValidationState[i]; i++)
	if(ajStrMatchC(state, gvVariationValidationState[i]))
	    estate = i;
    
    if(!estate)
	ajDebug("ensGvvariationValidationStateFromStr encountered "
		"unexpected string '%S'.\n", state);
    
    return estate;
}




/* @func ensGvvariationValidationStateToChar **********************************
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

const char* ensGvvariationValidationStateToChar(const AjEnum state)
{
    register ajint i = 0;
    
    if(!state)
	return NULL;
    
    for(i = 1; gvVariationValidationState[i] && (i < state); i++);
    
    if(!gvVariationValidationState[i])
	ajDebug("ensGvvariationValidationStateToChar encountered an "
		"out of boundary error on gender %d.\n", state);
    
    return gvVariationValidationState[i];
}




/* @func ensGvvariationFetchAllSynonyms ***************************************
**
** Fetch all synonyms of an Ensembl Genetic Variation Variation.
**
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [r] source [const AjPStr] Source
** @param [u] synonyms [AjPList] AJAX List of synonym AJAX Strings
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationFetchAllSynonyms(const EnsPGvvariation gvv,
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
	    ajTableToarrayKeysValues(gvv->Synonyms, &keyarray, &valarray);
	    
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




/* @func ensGvvariationFetchAllSynonymSources *********************************
**
** Fetch all synonym sources of an Ensembl Genetic Variation Variation.
**
** @param [r] gvv [const EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] sources [AjPList] AJAX List of source AJAX Strings
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationFetchAllSynonymSources(const EnsPGvvariation gvv,
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
	ajTableToarrayKeysValues(gvv->Synonyms, &keyarray, &valarray);
	
	for(i = 0; keyarray[i]; i++)
	    ajListPushAppend(sources,
			     (void *) ajStrNewRef((AjPStr) keyarray[i]));
	
	AJFREE(keyarray);
	AJFREE(valarray);
    }
    
    return ajTrue;
}




/* @datasection [EnsPGvvariationadaptor] Genetic Variation Variation Adaptor
**
** Functions for manipulating Ensembl Genetic Variation Variation Adaptor
** objects
**
** @nam2rule Gvvariationadaptor
**
******************************************************************************/




/* @funcstatic gvVariationadaptorClearAlleles *********************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Genetic Variation
** Variation Adaptor-internal Genetic Variation Alleles cache. This function
** deletes the unsigned identifier key data and the Ensembl Genetic Variation
** Alleles value data.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Genetic Variation Allele
**                            value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void gvVariationadaptorClearAlleles(void **key, void **value, void *cl)
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
    
    ensGvalleleDel((EnsPGvallele *) value);
    
    return;
}




/* @funcstatic gvVariationadaptorClearPopulations *****************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Genetic Variation
** Variation Adaptor-internal Genetic Variation Population cache. This function
** deletes the unsigned identifier key data and the Ensembl Genetic Variation
** Population value data.
**
** @param [u] key [void**] AJAX unsigned integer key data address
** @param [u] value [void**] Ensembl Genetic Variation Population
**                            value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void gvVariationadaptorClearPopulations(void **key, void **value,
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
    
    ensGvpopulationDel((EnsPGvpopulation *) value);
    
    return;
}




/* @funcstatic gvVariationadaptorClearSynonyms ********************************
**
** An ajTableMapDel 'apply' function to clear the Ensembl Genetic Variation
** Variation Adaptor-internal synonym cache. This function deletes the
** AJAX String key data and the unsigned integer value data.
**
** @param [u] key [void**] AJAX String key data address
** @param [u] value [void**] AJAX unsigned integer value data address
** @param [u] cl [void*] Standard, passed in from ajTableMapDel
** @see ajTableMapDel
**
** @return [void]
** @@
******************************************************************************/

static void gvVariationadaptorClearSynonyms(void **key, void **value, void *cl)
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




/* @funcstatic gvVariationadaptorFetchAllBySQL ********************************
**
** Fetch all Ensembl Genetic Variation Variation objects via an SQL statement.
**
** @param [r] gvva [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                          Variation Adaptor
** @param [r] statement [const AjPStr] SQL statement
** @param [u] gvvs [AjPList] AJAX List of Ensembl Genetic Variation
**                           Variation objects
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

static AjBool gvVariationadaptorFetchAllBySQL(EnsPGvvariationadaptor gvva,
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
    
    AjPSqlstatement sqls = NULL;
    AjISqlrow sqli       = NULL;
    AjPSqlrow sqlr       = NULL;
    
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
    
    EnsPGvallele gva         = NULL;
    EnsPGvalleleadaptor gvaa = NULL;
    
    EnsPGvpopulation gvp         = NULL;
    EnsPGvpopulationadaptor gvpa = NULL;
    
    EnsPGvvariation gvv = NULL;
    
    if(!gvva)
	return ajFalse;
    
    if(!statement)
	return ajFalse;
    
    if(!gvvs)
	return ajFalse;
    
    alleles = MENSTABLEUINTNEW(0);
    
    populations = MENSTABLEUINTNEW(0);
    
    synonyms = ajTablestrNewLen(0);
    
    gvpa = ensRegistryGetGvpopulationadaptor(gvva->Adaptor);
    
    sqls = ensDatabaseadaptorSqlstatementNew(gvva->Adaptor, statement);
    
    sqli = ajSqlrowiterNew(sqls);
    
    while(!ajSqlrowiterDone(sqli))
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
	
	sqlr = ajSqlrowiterGet(sqli);
	
	ajSqlcolumnToUint(sqlr, &identifier);
	ajSqlcolumnToStr(sqlr, &name);
	ajSqlcolumnToStr(sqlr, &vstatus);
	ajSqlcolumnToStr(sqlr, &source);
	ajSqlcolumnToStr(sqlr, &ancestralallele);
	ajSqlcolumnToUint(sqlr, &alleleid);
	ajSqlcolumnToStr(sqlr, &allelestr);
	ajSqlcolumnToFloat(sqlr, &allelefreq);
	ajSqlcolumnToUint(sqlr, &allelesampleid);
	ajSqlcolumnToStr(sqlr, &moltype);
	ajSqlcolumnToStr(sqlr, &synname);
	ajSqlcolumnToStr(sqlr, &synsource);
	ajSqlcolumnToStr(sqlr, &failed);
	
	if(cidentifier != identifier)
	{
	    /* Split the comma-separated list of validation states. */
	    
	    token = ajStrTokenNewC(vstatus, ",");
	    
	    vlist = ajListNew();
	    
	    while(ajStrTokenNextParse(&token, &vtoken))
		ajListPushAppend(vlist, (void *) ajStrNewS(vtoken));
	    
	    if(ajListGetLength(vlist) == 0)
		ajListPushAppend(vlist, (void *) ajStrNew());
	    
	    gvv = ensGvvariationNew(gvva,
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
		gvp = (EnsPGvpopulation)
		ajTableFetch(populations, (void *) &allelesampleid);
		
		if(!gvp)
		{
		    ensGvpopulationadaptorFetchByIdentifier(gvpa,
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
		gvp = (EnsPGvpopulation) NULL;
	    
	    if(alleleid)
	    {
		gva = ensGvalleleNew(gvaa,
				     alleleid,
				     gvp,
				     allelestr,
				     allelefreq);
		
		ensGvvariationAddGvallele(gvv, gva);
		
		ensGvalleleDel(&gva);
		
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
		    
		    ensGvvariationAddSynonym(gvv, synsource, synname);
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
    
    ajSqlrowiterDel(&sqli);
    
    ajSqlstatementDel(&sqls);
    
    /* Clear the AJAX Table of Ensembl Genetic Variation Populations. */
    
    ajTableMapDel(populations, gvVariationadaptorClearPopulations, NULL);
    
    ajTableFree(&populations);
    
    /* Clear the AJAX Table of Ensembl Genetic Variation Alleles. */
    
    ajTableMapDel(alleles, gvVariationadaptorClearAlleles, NULL);
    
    ajTableFree(&alleles);
    
    /* Clear the AJAX Table of Synonyms. */
    
    ajTableMapDel(synonyms, gvVariationadaptorClearSynonyms, NULL);
    
    ajTableFree(&synonyms);
    
    return ajTrue;
}




/* @func ensGvvariationadaptorNew *********************************************
**
** Default constructor for an Ensembl Genetic Variation Variation Adaptor.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Variationadaptor::new
** @param [r] dba [EnsPDatabaseadaptor] Ensembl Database Adaptor
**
** @return [EnsPGvvariationadaptor] Ensembl Genetic Variation Variation
**                                  Adaptor or NULL
** @@
******************************************************************************/

EnsPGvvariationadaptor ensGvvariationadaptorNew(EnsPDatabaseadaptor dba)
{
    EnsPGvvariationadaptor gvva = NULL;
    
    if(!dba)
	return NULL;
    
    AJNEW0(gvva);
    
    gvva->Adaptor = dba;
    
    return gvva;
}




/* @func ensGvvariationadaptorDel *********************************************
**
** Default destructor for an Ensembl Gentic Variation Variation Adaptor.
**
** @param [d] Pgvva [EnsPGvvariationadaptor*] Ensembl Genetic Variation
**                                            Variation Adaptor address
**
** @return [void]
** @@
******************************************************************************/

void ensGvvariationadaptorDel(EnsPGvvariationadaptor *Pgvva)
{
    if(!Pgvva)
	return;
    
    if(!*Pgvva)
	return;
    
    AJFREE(*Pgvva);

    *Pgvva = NULL;
    
    return;
}




/* @func ensGvvariationadaptorFetchByIdentifier *******************************
**
** Fetch an Ensembl Genetic Variation Variation via its
** SQL database-internal identifier.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Variationadaptor::fetch_by_dbID
** @param [u] gvva [const EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                                Variation Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @param [wP] Pgvv [EnsPGvvariation*] Ensembl Genetic Variation
**                                     Variation address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationadaptorFetchByIdentifier(EnsPGvvariationadaptor gvva,
                                              ajuint identifier,
                                              EnsPGvvariation *Pgvv)
{
    AjPList variations = NULL;
    
    AjPStr statement = NULL;
    
    EnsPGvvariation gvv = NULL;
    
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
    
    gvVariationadaptorFetchAllBySQL(gvva, statement, variations);
    
    if(ajListGetLength(variations) > 1)
	ajWarn("ensGvvariationadaptorFetchByIdentifier go more than one "
	       "Ensembl Genetic Variation Variation for identifier %u.",
	       identifier);
    
    ajListPop(variations, (void **) Pgvv);
    
    while(ajListPop(variations, (void *) &gvv))
	ensGvvariationDel(&gvv);
    
    ajListFree(&variations);
    
    ajStrDel(&statement);
    
    return ajTrue;
}




/* @func ensGvvariationadaptorFetchByName *************************************
**
** Fetch an Ensembl Genetic Variation Variation via its name.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variation.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Variationadaptor::fetch_by_name
** @param [u] gvva [const EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                                Variation Adaptor
** @param [r] name [const AjPStr] Name
** @param [rN] source [const AjPStr] Source
** @param [wP] Pgvv [EnsPGvvariation*] Ensembl Genetic Variation
**                                     Variation address
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationadaptorFetchByName(EnsPGvvariationadaptor gvva,
                                        const AjPStr name,
                                        const AjPStr source,
                                        EnsPGvvariation *Pgvv)
{
    char *Ptxtname   = NULL;
    char *Ptxtsource = NULL;
    
    AjPList gvvs = NULL;
    
    AjPStr extra     = NULL;
    AjPStr statement = NULL;
    
    EnsPGvvariation gvv = NULL;
    
    if(!gvva)
	return ajFalse;
    
    if(!name)
	return ajFalse;
    
    if(!Pgvv)
	return ajFalse;
    
    ensDatabaseadaptorEscapeC(gvva->Adaptor, &Ptxtname, name);
    
    if(source && ajStrGetLen(source))
    {
	ensDatabaseadaptorEscapeC(gvva->Adaptor, &Ptxtsource, source);
	
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
    
    gvVariationadaptorFetchAllBySQL(gvva, statement, gvvs);
    
    ajStrDel(&statement);
    
    if(ajListGetLength(gvvs) > 1)
	ajWarn("ensGvvariationadaptorFetchByIdentifier go more than one "
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
			     
	gvVariationadaptorFetchAllBySQL(gvva, statement, gvvs);
	
	ajStrDel(&statement);
    }
    
    if(ajListGetLength(gvvs) > 1)
	ajWarn("ensGvvariationadaptorFetchByIdentifier go more than one "
	       "Ensembl Genetic Variation Variation for name '%S' and "
	       "source '%S'.", name, source);
    
    ajListPop(gvvs, (void **) Pgvv);
    
    while(ajListPop(gvvs, (void *) &gvv))
	ensGvvariationDel(&gvv);
    
    ajListFree(&gvvs);
    
    ajCharDel(&Ptxtname);
    
    ajStrDel(&extra);
    
    return ajTrue;
}




/* @func ensGvvariationadaptorFetchAllBySource ********************************
**
** Fetch all Ensembl Genetic Variation Variations via a source.
** The caller is responsible for deleting the
** Ensembl Genetic Variation Variations before deleting the AJAX List.
**
** @cc Bio::EnsEMBL::Variation::DBSQL::Variationadaptor::fetch_all_by_source
** @param [u] gvva [const EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                                Variation Adaptor
** @param [r] source [const AjPStr] Source
** @param [r] primary [AjBool] Primary
** @param [u] gvvs [AjPList] AJAX List of Ensembl Genetic Variation Variations
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationadaptorFetchAllBySource(EnsPGvvariationadaptor gvva,
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
    
    ensDatabaseadaptorEscapeC(gvva->Adaptor, &Ptxtsource, source);
    
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
    
    gvVariationadaptorFetchAllBySQL(gvva, statement, gvvs);
    
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
	
	gvVariationadaptorFetchAllBySQL(gvva, statement, gvvs);
	
	ajStrDel(&statement);
    }
    
    ajCharDel(&Ptxtsource);
    
    return ajTrue;
}




/* @datasection [EnsPGvvariationfeature] Genetic Variation Variation Feature **
**
** Functions for manipulating Ensembl Genetic Variation Variation Feature
** objects
**
** @nam2rule Gvvariationfeature
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Genetic Variation Variation Feature by
** pointer. It is the responsibility of the user to first destroy any previous
** Genetic Variation Variation Feature. The target pointer does not need to be
** initialised to NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPGvvariationfeature]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                              Variation Feature
** @argrule Ref object [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                              Variation Feature
**
** @valrule * [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                     Variation Feature
**
** @fcategory new
******************************************************************************/




/* @func ensGvvariationfeatureNew *********************************************
**
** Default constructor for an Ensembl Genetic Variation Variation Feature.
**
** @cc Bio::EnsEMBL::Storable::new
** @param [r] gvvfa [EnsPGvvariationfeatureadaptor] Ensembl Genetic Variation
**                                                  Variation Feature Adaptor
** @param [r] identifier [ajuint] SQL database-internal identifier
** @cc Bio::EnsEMBL::Variation::VariationFeature::new
** @param [u] feature [EnsPFeature] Ensembl Feature
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
** @param [u] name [AjPStr] Name
** @param [u] source [AjPStr] Source
** @param [u] validation [AjPStr] Validation code
** @param [u] consequence [AjPStr] Consequence type
** @param [r] mapweight [ajuint] Map weight
**
** @return [EnsPGvvariationfeature] Ensembl Genetic Variation Variation Feature
** @@
******************************************************************************/

EnsPGvvariationfeature ensGvvariationfeatureNew(
    EnsPGvvariationfeatureadaptor gvvfa,
    ajuint identifier,
    EnsPFeature feature,
    EnsPGvvariation gvv,
    AjPStr name,
    AjPStr source,
    AjPStr validation,
    AjPStr consequence,
    ajuint mapweight)
{
    EnsPGvvariationfeature gvvf = NULL;
    
    /* FIXME: Input restrictions are missing. */
    
    AJNEW0(gvvf);
    
    gvvf->Use = 1;
    
    gvvf->Identifier = identifier;
    
    gvvf->Adaptor = gvvfa;
    
    gvvf->Feature = ensFeatureNewRef(feature);
    
    gvvf->Gvvariation = ensGvvariationNewRef(gvv);
    
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




/* @func ensGvvariationfeatureNewObj ******************************************
**
** Object-based constructor function, which returns an independent object.
**
** @param [r] object [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                  Variation Feature
**
** @return [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                  Variation Feature or NULL
** @@
******************************************************************************/

EnsPGvvariationfeature ensGvvariationfeatureNewObj(
    const EnsPGvvariationfeature object)
{
    EnsPGvvariationfeature gvvf = NULL;
    
    if(!object)
	return NULL;
    
    AJNEW0(gvvf);
    
    gvvf->Use = 1;
    
    gvvf->Identifier = object->Identifier;
    
    gvvf->Adaptor = object->Adaptor;
    
    gvvf->Feature = ensFeatureNewRef(object->Feature);
    
    gvvf->Gvvariation = ensGvvariationNewRef(object->Gvvariation);
    
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




/* @func ensGvvariationfeatureNewRef ******************************************
**
** Ensembl Object referencing function, which returns a pointer to the
** Ensembl Object passed in and increases its reference count.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
**
** @return [EnsPGvvariationfeature] Ensembl Genetic Variation Variation Feature
** @@
******************************************************************************/

EnsPGvvariationfeature ensGvvariationfeatureNewRef(EnsPGvvariationfeature gvvf)
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
** @fdata [EnsPGvvariationfeature]
** @fnote None
**
** @nam3rule Del Destroy (free) a Genetic Variation Variation Feature object
**
** @argrule * Pgvvf [EnsPGvvariationfeature*] Genetic Variation
**                                            Variation Feature object address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensGvvariationfeatureDel *********************************************
**
** Default destructor for an Ensembl Genetic Variation Variation Feature.
**
** @param [d] Pgvvf [EnsPGvvariationfeature*] Ensembl Genetic Variation
**                                            Variation Feature address
**
** @return [void]
** @@
******************************************************************************/

void ensGvvariationfeatureDel(EnsPGvvariationfeature *Pgvvf)
{
    EnsPGvvariationfeature pthis = NULL;
    
    /*
     ajDebug("ensGvvariationfeatureDel\n"
	     "  *Pgvvf %p\n",
	     *Pgvvf);
     
     ensGvvariationfeatureTrace(*Pgvvf, 1);
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
    
    ensGvvariationDel(&pthis->Gvvariation);
    
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
** @fdata [EnsPGvvariationfeature]
** @fnote None
**
** @nam3rule Get Return Genetic Variation Variation Feature attribute(s)
** @nam4rule GetAdaptor Return the Ensembl Genetic Variation
**                      Variation Feature Adaptor
** @nam4rule GetIdentifier Return the SQL database-internal identifier
** @nam4rule GetFeature Return the Ensembl Feature
** @nam4rule GetGvvariation Return the Ensembl Genetic Variation Variation
** @nam4rule GetName Return the name
** @nam4rule GetSource Return the source
** @nam4rule GetValidationCode Return the validation code
** @nam4rule GetConsequenceType Return the consequence type
** @nam4rule GetMapWeight Return the map weight
**
** @argrule * gvvf [const EnsPGvvariationfeature] Genetic Variation
**                                                Variation Feature
**
** @valrule Adaptor [EnsPGvvariationfeatureadaptor] Ensembl Genetic Variation
**                                                  Variation Feature Adaptor
** @valrule Identifier [ajuint] SQL database-internal identifier
** @valrule Feature [EnsPFeature] Ensembl Feature
** @valrule Gvvariation [EnsPGvvariation] Ensembl Genetic Variation Variation
** @valrule Name [AjPStr] Name
** @valrule Source [AjPStr] Source
** @valrule ValidationCode [AjPStr] Validation code
** @valrule ConsequenceType [AjPStr] Consequence type
** @valrule MapWeight [ajuint] Map weight
**
** @fcategory use
******************************************************************************/




/* @func ensGvvariationfeatureGetAdaptor **************************************
**
** Get the Ensembl Genetic Variation Variation Feature Adaptor element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [EnsPGvvariationfeatureadaptor] Ensembl Genetic Variation
**                                         Variation Feature Adaptor
** @@
******************************************************************************/

EnsPGvvariationfeatureadaptor ensGvvariationfeatureGetAdaptor(
    const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->Adaptor;
}




/* @func ensGvvariationfeatureGetIdentifier ***********************************
**
** Get the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [ajuint] Internal database identifier
** @@
******************************************************************************/

ajuint ensGvvariationfeatureGetIdentifier(const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return 0;
    
    return gvvf->Identifier;
}




/* @func ensGvvariationfeatureGetFeature **************************************
**
** Get the Ensembl Feature element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [EnsPFeature] Ensembl Feature
** @@
******************************************************************************/

EnsPFeature ensGvvariationfeatureGetFeature(const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->Feature;
}




/* @func ensGvvariationfeatureGetGvvariation **********************************
**
** Get the Ensembl Genetic Variation Variation element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [EnsPGvvariation] Ensembl Genetic Variation Variation
** @@
******************************************************************************/

EnsPGvvariation ensGvvariationfeatureGetGvvariation(
    const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->Gvvariation;
}




/* @func ensGvvariationfeatureGetName *****************************************
**
** Get the name element of an Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [AjPStr] Name
** @@
******************************************************************************/

AjPStr ensGvvariationfeatureGetName(const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->Name;
}




/* @func ensGvvariationfeatureGetSource ***************************************
**
** Get the source element of an Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [AjPStr] Source
** @@
******************************************************************************/

AjPStr ensGvvariationfeatureGetSource(const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->Source;
}




/* @func ensGvvariationfeatureGetValidationCode *******************************
**
** Get the validation code element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [AjPStr] Validation code
** @@
******************************************************************************/

AjPStr ensGvvariationfeatureGetValidationCode(const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->ValidationCode;
}




/* @func ensGvvariationfeatureGetConsequenceType ******************************
**
** Get the consequence type element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [AjPStr] Consequence type
** @@
******************************************************************************/

AjPStr ensGvvariationfeatureGetConsequenceType(
    const EnsPGvvariationfeature gvvf)
{
    if(!gvvf)
        return NULL;
    
    return gvvf->ConsequenceType;
}




/* @func ensGvvariationfeatureGetMapWeight ************************************
**
** Get the map weight element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [ajuint] Map weight
** @@
******************************************************************************/

ajuint ensGvvariationfeatureGetMapWeight(const EnsPGvvariationfeature gvvf)
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
** @fdata [EnsPGvvariationfeature]
** @fnote None
**
** @nam3rule Set Set one element of a Genetic Variation Variation Feature
** @nam4rule SetAdaptor Set the Ensembl Genetic Variation
**                      Variation Feature Adaptor
** @nam4rule SetIdentifier Set the SQL database-internal identifier
** @nam4rule SetFeature Set the Ensembl Feature
** @nam4rule SetGvvariation Set the Ensembl Genetic Variation Variation
** @nam4rule SetName Set the name
** @nam4rule SetSource Set the source
** @nam4rule SetValidationCode Set the validation code
** @nam4rule SetConsequenceType Set the consequence type
** @nam4rule SetMapWeight Set the map weight
**
** @argrule * gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensGvvariationfeatureSetAdaptor **************************************
**
** Set the Ensembl Genetic Variation Variation Feature Adaptor element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [r] gvvfa [EnsPGvvariationfeatureadaptor] Ensembl Genetic Variation
**                                                  Variation Feature Adaptor
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetAdaptor(EnsPGvvariationfeature gvvf,
                                       EnsPGvvariationfeatureadaptor gvvfa)
{
    if(!gvvf)
        return ajFalse;
    
    gvvf->Adaptor = gvvfa;
    
    return ajTrue;
}




/* @func ensGvvariationSetIdentifier ******************************************
**
** Set the SQL database-internal identifier element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [r] identifier [ajuint] SQL database-internal identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetIdentifier(EnsPGvvariationfeature gvvf,
                                          ajuint identifier)
{
    if(!gvvf)
        return ajFalse;
    
    gvvf->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensGvvariationfeatureSetFeature **************************************
**
** Set the Ensembl Feature element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] feature [EnsPFeature] Ensembl Feature
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetFeature(EnsPGvvariationfeature gvvf,
                                       EnsPFeature feature)
{
    if(!gvvf)
        return ajFalse;
    
    ensFeatureDel(&gvvf->Feature);
    
    gvvf->Feature = ensFeatureNewRef(feature);
    
    return ajTrue;
}




/* @func ensGvvariationfeatureSetGvvariation **********************************
**
** Set the Ensembl Gentic Variation Variation element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] gvv [EnsPGvvariation] Ensembl Genetic Variation Variation
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetGvvariation(EnsPGvvariationfeature gvvf,
                                           EnsPGvvariation gvv)
{
    if(!gvvf)
        return ajFalse;
    
    ensGvvariationDel(&gvvf->Gvvariation);
    
    gvvf->Gvvariation = ensGvvariationNewRef(gvv);
    
    return ajTrue;
}




/* @func ensGvvariationfeatureSetName *****************************************
**
** Set the name element of an Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] name [AjPStr] Name
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetName(EnsPGvvariationfeature gvvf, AjPStr name)
{
    if(!gvvf)
        return ajFalse;
    
    ajStrDel(&gvvf->Name);
    
    if(name)
	gvvf->Name = ajStrNewRef(name);
    
    return ajTrue;
}




/* @func ensGvvariationfeatureSetSource ***************************************
**
** Set the source element of an Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] source [AjPStr] Source
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetSource(EnsPGvvariationfeature gvvf,
                                      AjPStr source)
{
    if(!gvvf)
        return ajFalse;
    
    ajStrDel(&gvvf->Source);
    
    if(source)
	gvvf->Name = ajStrNewRef(source);
    
    return ajTrue;
}




/* @func ensGvvariationfeatureSetValidationCode *******************************
**
** Set the validation code element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] validationcode [AjPStr] Validation code
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetValidationCode(EnsPGvvariationfeature gvvf,
                                              AjPStr validationcode)
{
    if(!gvvf)
        return ajFalse;
    
    ajStrDel(&gvvf->ValidationCode);
    
    if(validationcode)
	gvvf->ValidationCode = ajStrNewRef(validationcode);
    
    return ajTrue;
}




/* @func ensGvvariationfeatureSetConsequenceType ******************************
**
** Set the consequence type element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [u] consequencetype [AjPStr] Consequence type
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetConsequenceType(EnsPGvvariationfeature gvvf,
                                               AjPStr consequencetype)
{
    if(!gvvf)
        return ajFalse;
    
    ajStrDel(&gvvf->ConsequenceType);
    
    if(consequencetype)
	gvvf->ConsequenceType = ajStrNewRef(consequencetype);
    
    return ajTrue;
}




/* @func ensGvvariationfeatureSetMapWeight ************************************
**
** Set the map weight element of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [u] gvvf [EnsPGvvariationfeature] Ensembl Genetic Variation
**                                          Variation Feature
** @param [r] mapweight [ajuint] Map weight
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureSetMapWeight(EnsPGvvariationfeature gvvf,
                                         ajuint mapweight)
{
    if(!gvvf)
        return ajFalse;
    
    gvvf->MapWeight = mapweight;
    
    return ajTrue;
}




/* @func ensGvvariationfeatureGetMemSize **************************************
**
** Get the memory size in bytes of an
** Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
**
** @return [ajuint] Memory size
** @@
******************************************************************************/

ajuint ensGvvariationfeatureGetMemSize(const EnsPGvvariationfeature gvvf)
{
    ajuint size = 0;
    
    if(!gvvf)
	return 0;
    
    size += (ajuint) sizeof (EnsOGvvariationfeature);
    
    size += ensFeatureGetMemSize(gvvf->Feature);
    
    size += ensGvvariationGetMemSize(gvvf->Gvvariation);
    
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
** @fdata [EnsPGvvariationfeature]
** @nam3rule Trace Report Ensembl Genetic Variation Variation Feature elements
**                 to debug file
**
** @argrule Trace gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                    Variation Feature
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensGvvariationfeatureTrace *******************************************
**
** Trace an Ensembl Genetic Variation Variation Feature.
**
** @param [r] gvvf [const EnsPGvvariationfeature] Ensembl Genetic Variation
**                                                Variation Feature
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensGvvariationfeatureTrace(const EnsPGvvariationfeature gvvf,
                                  ajuint level)
{
    AjPStr indent = NULL;
    
    if(!gvvf)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("ensGvvariationfeatureTrace %p\n"
	    "%S  Use %u\n"
	    "%S  Identifier %u\n"
	    "%S  Adaptor %p\n"
	    "%S  Feature %p\n"
	    "%S  Gvvariation %p\n"
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
	    indent, gvvf->Gvvariation,
	    indent, gvvf->Name,
	    indent, gvvf->Source,
	    indent, gvvf->ValidationCode,
	    indent, gvvf->ConsequenceType,
	    indent, gvvf->MapWeight);
    
    ajStrDel(&indent);
    
    ensFeatureTrace(gvvf->Feature, level + 1);
    
    ensGvvariationTrace(gvvf->Gvvariation, level + 1);
    
    return ajTrue;
}
