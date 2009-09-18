/******************************************************************************
** @source Ensembl Database Adaptor functions.
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

#include "ensdatabaseadaptor.h"




/* ==================================================================== */
/* ========================== private data ============================ */
/* ==================================================================== */

static const char *databaseAdaptorGroup[] =
{
    NULL,
    "core",
    "vega",
    "otherfeatures",
    "cdna",
    "variation",
    "funcgen",
    "compara",
    "go",
    "qc",
    "pipeline",
    "hive",
    "coreexpressionest",
    "coreexpressiongnf",
    "ancestral",
    "website",
    NULL
};




/* ==================================================================== */
/* ======================== private functions ========================= */
/* ==================================================================== */




/* @filesection ensdatabaseadaptor ********************************************
**
** @nam1rule ens Function belongs to the Ensembl library
**
******************************************************************************/




/* @datasection [EnsPDatabaseAdaptor] Database Adaptor ************************
**
** Functions for manipulating Ensembl Database Adaptor objects
**
** @cc Bio::EnsEMBL::DBSQL::DBAdaptor CVS Revision: 1.192
**
** @nam2rule DatabaseAdaptor
**
******************************************************************************/




/* @section constructors ******************************************************
**
** All constructors return a new Ensembl Database Adaptor by pointer.
** It is the responsibility of the user to first destroy any previous
** Database Adaptor. The target pointer does not need to be initialised to
** NULL, but it is good programming practice to do so anyway.
**
** @fdata [EnsPDatabaseAdaptor]
** @fnote None
**
** @nam3rule New Constructor
** @nam4rule NewObj Constructor with existing object
** @nam4rule NewRef Constructor by incrementing the reference counter
**
** @argrule Obj object [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @argrule Ref object [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @valrule * [EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @fcategory new
******************************************************************************/




/* @func ensDatabaseAdaptorNew ************************************************
**
** Default Ensembl Database Adaptor constructor.
**
** This function should not be called directly, rather the
** ensRegistryAddDatabaseAdaptor and ensRegistryGetDatabaseAdaptor functions
** should be used to instantiate an Ensembl Database Adaptor in the
** Ensembl Registry and subsequently return it for use.
**
** @cc Bio::EnsEMBL::DBSQL::DBAdaptor::new
** @param [r] dbc [EnsPDatabaseConnection] Ensembl Database Connection
** @param [u] database [AjPStr] Database name (optional). If not provided, the
**                              database name in the Database Connection will
**                              be used.
** @param [u] species [AjPStr] Species
** @param [r] group [AjEnum] Group
** @param [r] multi [AjBool] Multiple species
** @param [r] identifier [ajuint] Species identifier
**
** @return [EnsPDatabaseAdaptor] Ensembl Database Adaptor or NULL
** @@
** NOTE: The Perl API automatically registers the Ensembl Database Adaptor in
** the Ensembl Registry via Bio::EnsEMBL::Utils::ConfigRegistry::gen_load.
** In this implementation both objects are kept separate. The Ensembl Registry
** object provides a function, which generates an Ensembl Database Adaptor
** based on the same parameter set and automatically registers it.
******************************************************************************/

EnsPDatabaseAdaptor ensDatabaseAdaptorNew(
    EnsPDatabaseConnection dbc,
    AjPStr database,
    AjPStr species,
    AjEnum group,
    AjBool multi,
    ajuint identifier)
{
    EnsPDatabaseAdaptor dba = NULL;
    
    /*
     ajDebug("ensDatabaseAdaptorNew\n"
	     "  dbc %p\n"
	     "  database '%S'\n"
	     "  species '%S'\n"
	     "  group %d\n"
	     "  multi %B\n"
	     "  identifier %u\n",
	     dbc,
	     database,
	     species,
	     group,
	     multi,
	     identifier);
     
     ensDatabaseConnectionTrace(dbc, 1);
     */
    
    if(!dbc)
        return NULL;
    
    if(!species)
        return NULL;
    
    if(!group)
        return NULL;
    
    AJNEW0(dba);
    
    dba->DatabaseConnection = ensDatabaseConnectionNewC(dbc, database);
    
    if(species)
	dba->Species = ajStrNewRef(species);
    
    dba->Group = group;
    
    dba->MultiSpecies = multi;
    
    dba->Identifier = identifier;
    
    return dba;
}




/* @section destructors *******************************************************
**
** Destruction destroys all internal data structures and frees the
** memory allocated for the Ensembl Database Adaptor.
**
** @fdata [EnsPDatabaseAdaptor]
** @fnote None
**
** @nam3rule Del Destroy (free) an Ensembl Database Adaptor object
**
** @argrule * Pdba [EnsPDatabaseAdaptor*] Ensembl Database Adaptor object
**                                        address
**
** @valrule * [void]
**
** @fcategory delete
******************************************************************************/




/* @func ensDatabaseAdaptorDel ************************************************
**
** Default Ensembl Database Adaptor destructor.
**
** This function should only be called for Ensembl Database Adaptors that have
** been instantiated outside the ensRegistryAddDatabaseAdaptor function.
** For all other Database Adaptors ensRegistryRemoveDatabaseAdaptor can be
** used to destroy a particular Ensembl Database Adaptor. All remaining
** Database Adaptors will be automatically cleared from the Registry upon exit.
**
** @param [d] Pdba [EnsPDatabaseAdaptor*] Ensembl Database Adaptor address
**
** @return [void]
** @@
** NOTE: The Perl API removes the Ensembl Database Adaptor from the
** Ensembl Registry via Bio::EnsEMBL::Registry::remove_db.
** This implementation keeps both objects independent and Ensembl Database
** Adaptors are deleted from the Ensembl Registry.
******************************************************************************/

void ensDatabaseAdaptorDel(EnsPDatabaseAdaptor* Pdba)
{
    EnsPDatabaseAdaptor pthis = NULL;
    
    if(!Pdba)
        return;
    
    if(!*Pdba)
        return;

    pthis = *Pdba;
    
    /*
     ajDebug("ensDatabaseAdaptorDel\n"
	     "  *Pdba %p\n",
	     *Pdba);
     
     ensDatabaseAdaptorDebug(*Pdba, 1);
     */
    
    ensDatabaseConnectionDel(&pthis->DatabaseConnection);
    
    ajStrDel(&pthis->Species);
    
    AJFREE(pthis);

    *Pdba = NULL;
    
    return;
}




/* @section element retrieval *************************************************
**
** Functions for returning elements of an Ensembl Database Adaptor object.
**
** @fdata [EnsPDatabaseAdaptor]
** @fnote None
**
** @nam3rule Get Return Database Adaptor attribute(s)
** @nam4rule GetDatabaseConnection Return the Ensembl Database Connection
** @nam4rule GetSpecies Return the species
** @nam4rule GetGroup Return the group
** @nam4rule GetMultiSpecies Return the multi-species flag
** @nam4rule GetIdentifier Return the species identifier
**
** @argrule * dba [const EnsPDatabaseAdaptor] Database Adaptor
**
** @valrule DatabaseConnection [EnsPDatabaseConnection] Ensembl Database
**                                                      Connection
** @valrule Species [AjPStr] Species
** @valrule Group [AjEnum] Group
** @valrule MultiSpecies [AjBool] Multiple species
** @valrule Identifier [ajuint] Species identifier
**
** @fcategory use
******************************************************************************/




/* @func ensDatabaseAdaptorGetDatabaseConnection ******************************
**
** Get the Ensembl Database Connection element of an Ensembl Database Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::DBAdaptor::dbc
** @param [r] dba [const EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [EnsPDatabaseConnection] Ensembl Database Connection or NULL
** @@
******************************************************************************/

EnsPDatabaseConnection ensDatabaseAdaptorGetDatabaseConnection(
    const EnsPDatabaseAdaptor dba)
{
    if(!dba)
        return NULL;
    
    return dba->DatabaseConnection;
}




/* @func ensDatabaseAdaptorGetSpecies *****************************************
**
** Get the species element of an Ensembl Database Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::DBAdaptor::species
** @param [r] dba [const EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [AjPStr] Species or NULL
** @@
******************************************************************************/

AjPStr ensDatabaseAdaptorGetSpecies(const EnsPDatabaseAdaptor dba)
{
    if(!dba)
        return NULL;
    
    return dba->Species;
}




/* @func ensDatabaseAdaptorGetGroup *******************************************
**
** Get the group element of an Ensembl Database Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::DBAdaptor::group
** @param [r] dba [const EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [AjPStr] Group or ensEDatabaseAdaptorGroupNULL
** @@
******************************************************************************/

AjEnum ensDatabaseAdaptorGetGroup(const EnsPDatabaseAdaptor dba)
{
    if(!dba)
        return ensEDatabaseAdaptorGroupNULL;
    
    return dba->Group;
}




/* @func ensDatabaseAdaptorGetMultiSpecies ************************************
**
** Get the multi-species element of an Ensembl Database Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::DBAdaptor::is_multispecies
** @param [r] dba [const EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [AjBool] ajTrue if the database contains multiple species
** @@
******************************************************************************/

AjBool ensDatabaseAdaptorGetMultiSpecies(const EnsPDatabaseAdaptor dba)
{
    if(!dba)
        return ajFalse;
    
    return dba->MultiSpecies;
}




/* @func ensDatabaseAdaptorGetIdentifier **************************************
**
** Get the species identifier element of an Ensembl Database Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::DBAdaptor::species_id
** @param [r] dba [const EnsPDatabaseAdaptor] Ensembl Database Adaptor
**
** @return [ajuint] Species identifier, defaults to 1
** @@
******************************************************************************/

ajuint ensDatabaseAdaptorGetIdentifier(const EnsPDatabaseAdaptor dba)
{
    if(!dba)
        return 0;
    
    if(!dba->Identifier)
	return 1;
    
    return dba->Identifier;
}




/* @section element assignment ************************************************
**
** Functions for assigning elements of an Ensembl Database Adaptor object.
**
** @fdata [EnsPDatabaseAdaptor]
** @fnote None
**
** @nam3rule Set Set one element of an Ensembl Database Adaptor
** @nam4rule SetDatabaseConnection Set the Ensembl Database Connection
** @nam4rule SetSpecies Set the species
** @nam4rule SetGroup Set the group
** @nam4rule SetMultiSpecies Set the multiple-species flag
** @nam4rule SetIdentifier Set the species identifier
**
** @argrule * dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor object
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory modify
******************************************************************************/




/* @func ensDatabaseAdaptorSetDatabaseConnection ******************************
**
** Set the Ensembl Database Connection element of an Ensembl Database Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::DBAdaptor::dbc
** @param [u] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [u] dbc [EnsPDatabaseConnection] Ensembl Database Connection
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseAdaptorSetDatabaseConnection(EnsPDatabaseAdaptor dba,
                                               EnsPDatabaseConnection dbc)
{
    if(!dba)
        return ajFalse;
    
    ensDatabaseConnectionDel(&(dba->DatabaseConnection));
    
    dba->DatabaseConnection = ensDatabaseConnectionNewRef(dbc);
    
    return ajTrue;
}




/* @func ensDatabaseAdaptorSetSpecies *****************************************
**
** Set the species element of an Ensembl Database Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::DBAdaptor::species
** @param [u] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [u] species [AjPStr] Species
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseAdaptorSetSpecies(EnsPDatabaseAdaptor dba, AjPStr species)
{
    if(!dba)
        return ajFalse;
    
    ajStrDel(&dba->Species);
    
    if(species)
	dba->Species = ajStrNewRef(species);
    
    return ajTrue;
}




/* @func ensDatabaseAdaptorSetGroup *******************************************
**
** Set the group element of an Ensembl Database Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::DBAdaptor::group
** @param [u] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] group [AjEnum] Group
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseAdaptorSetGroup(EnsPDatabaseAdaptor dba, AjEnum group)
{
    if(!dba)
        return ajFalse;
    
    dba->Group = group;
    
    return ajTrue;
}




/* @func ensDatabaseAdaptorSetMultiSpecies ************************************
**
** Set the multi-species element of an Ensembl Database Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::DBAdaptor::is_multispecies
** @param [u] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] multi [AjBool] Multi-species attribute
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseAdaptorSetMultiSpecies(EnsPDatabaseAdaptor dba, AjBool multi)
{
    if(!dba)
        return ajFalse;
    
    dba->MultiSpecies = multi;
    
    return ajTrue;
}




/* @func ensDatabaseAdaptorSetIdentifier **************************************
**
** Set the species identifier element of an Ensembl Database Adaptor.
**
** @cc Bio::EnsEMBL::DBSQL::DBAdaptor::species_id
** @param [u] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] identifier [ajuint] Species identifier
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseAdaptorSetIdentifier(EnsPDatabaseAdaptor dba,
                                       ajuint identifier)
{
    if(!dba)
        return ajFalse;
    
    dba->Identifier = identifier;
    
    return ajTrue;
}




/* @func ensDatabaseAdaptorGroupFromStr ***************************************
**
** Convert an AJAX String into an Ensembl Database Adaptor group element.
**
** @param [r] group [const AjPStr] Group string
**
** @return [AjEnum] Ensembl Database Adaptor group element or
**                  ensEDatabaseAdaptorGroupNULL
** @@
******************************************************************************/

AjEnum ensDatabaseAdaptorGroupFromStr(const AjPStr group)
{
    register ajuint i = 0;
    
    AjEnum egroup = ensEDatabaseAdaptorGroupNULL;
    
    for(i = 1; databaseAdaptorGroup[i]; i++)
	if (ajStrMatchCaseC(group, databaseAdaptorGroup[i]))
	    egroup = i;
    
    if(!egroup)
	ajDebug("ensDatabaseAdaptorGroupFromStr encountered "
		"unexpected string '%S'.\n", group);
    
    return egroup;
}




/* @func ensDatabaseAdaptorGroupToChar ****************************************
**
** Convert an Ensembl Database Adaptor group element into a
** C-type (char*) string.
**
** @param [r] group [const AjEnum] Database Adaptor group enumerator
**
** @return [const char*] Database Adaptor group C-type (char*) string
** @@
******************************************************************************/

const char* ensDatabaseAdaptorGroupToChar(const AjEnum group)
{
    register ajint i = 0;
    
    if(!group)
	return NULL;
    
    for(i = 1; databaseAdaptorGroup[i] && (i < group); i++);
    
    if(!databaseAdaptorGroup[i])
	ajDebug("ensDatabaseAdaptorGroupToChar encountered an "
		"out of boundary error on group %d.\n", group);
    
    return databaseAdaptorGroup[i];
}




/* @func ensDatabaseAdaptorMatch **********************************************
**
** Tests for matching two Ensembl Database Adaptors.
**
** @param [r] dba1 [const EnsPDatabaseAdaptor] First Ensembl Database Adaptor
** @param [r] dba2 [const EnsPDatabaseAdaptor] Second Ensembl Database Adaptor
**
** @return [AjBool] ajTrue if the Ensembl Database Adaptors are equal
** @@
** The comparison is based on initial pointer equality and if that fails a
** comparison of the group element, the species identifier and a
** case-insensitive string comparison of the species element.
******************************************************************************/

AjBool ensDatabaseAdaptorMatch(const EnsPDatabaseAdaptor dba1,
                               const EnsPDatabaseAdaptor dba2)
{
    if(!dba1)
        return ajFalse;
    
    if(!dba2)
        return ajFalse;
    
    if(dba1 == dba2)
	return ajTrue;
    
    if(!ensDatabaseConnectionMatch(dba1->DatabaseConnection,
                                   dba2->DatabaseConnection))
	return ajFalse;
    
    if(dba1->Group != dba2->Group)
	return ajFalse;
    
    if(dba1->MultiSpecies != dba2->MultiSpecies)
	return ajFalse;
    
    if(dba1->Identifier != dba2->Identifier)
	return ajFalse;
    
    if(!ajStrMatchCaseS(dba1->Species, dba2->Species))
	return ajFalse;
    
    return ajTrue;
}




/* @func ensDatabaseAdaptorMatchComponents ************************************
**
** Tests whether an Ensembl Database Adaptor matches component elements.
**
** @param [r] dba [const EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] dbc [cons EnsPDatabaseConnection] Ensembl Database Connection
** @param [r] species [const AjPStr] Species
** @param [r] group [AjEnum] Group
** @param [r] multi [AjBool] Multiple species
** @param [r] identifier [ajuint] Species identifier
**
** @return [AjBool] ajTrue if all elements match
** @@
******************************************************************************/

AjBool ensDatabaseAdaptorMatchComponents(const EnsPDatabaseAdaptor dba,
                                         const EnsPDatabaseConnection dbc,
                                         const AjPStr species,
                                         AjEnum group,
                                         AjBool multi,
                                         ajuint identifier)
{
    if(!dba)
	return ajFalse;
    
    if(!dbc)
	return ajFalse;
    
    if(!species)
	return ajFalse;
    
    if(!group)
	return ajFalse;
    
    if(dba->Group != group)
	return ajFalse;
    
    if(dba->MultiSpecies != multi)
	return ajFalse;
    
    if(dba->Identifier != identifier)
	return ajFalse;
    
    if(!ensDatabaseConnectionMatch(dba->DatabaseConnection, dbc))
	return ajFalse;
    
    if(!ajStrMatchCaseS(dba->Species, species))
	return ajFalse;
    
    return ajTrue;
}




/* @func ensDatabaseAdaptorSqlStatementNew ************************************
**
** Run an SQL statement against an Ensembl Database Adaptor.
**
** @param [r] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] statement [const AjPStr] SQL statement
**
** @return [AjPSqlStatement] AJAX SQL Statement or NULL
** @@
******************************************************************************/

AjPSqlStatement ensDatabaseAdaptorSqlStatementNew(EnsPDatabaseAdaptor dba,
                                                  const AjPStr statement)
{
    if(!dba)
	return NULL;
    
    if(!statement)
	return NULL;
    
    return ensDatabaseConnectionSqlStatementNew(dba->DatabaseConnection,
						statement);
}




/* @func ensDatabaseAdaptorEscapeCS *******************************************
**
** Escape an AJAX String based on an AJAX SQL Connection.
** The caller is responsible for deleting the C-type character string.
**
** @param [u] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [wP] Ptxt [char **] Address of the (new) SQL-escaped C string
** @param [r] str [const AjPStr] AJAX String to be escaped
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseAdaptorEscapeCS(EnsPDatabaseAdaptor dba,
                                  char **Ptxt,
                                  const AjPStr str)
{
    if(!dba)
        return ajFalse;
    
    if(!str)
	return ajFalse;
    
    /*
     ajDebug("ensDatabaseAdaptorEscapeCS\n"
	     "  dba %p\n"
	     "  Ptxt %p\n"
	     "  str '%S'\n",
	     dba,
	     Ptxt,
	     str);
     
     ensDatabaseAdaptorTrace(dba, 1);
     */
    
    return ensDatabaseConnectionEscapeCS(dba->DatabaseConnection, Ptxt, str);
}




/* @func ensDatabaseAdaptorEscapeSS *******************************************
**
** Escape an AJAX String based on an AJAX SQL Connection.
** The caller is responsible for deleting the AJAX String.
**
** @param [u] dba [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [wP] Pstr [AjPStr*] Address of the (new) SQL-escaped AJAX String
** @param [r] str [const AjPStr] AJAX String to be escaped
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseAdaptorEscapeSS(EnsPDatabaseAdaptor dba,
                                  AjPStr *Pstr,
                                  const AjPStr str)
{
    if(!dba)
        return ajFalse;
    
    if(!str)
	return ajFalse;
    
    /*
     ajDebug("ensDatabaseAdaptorEscapeSS\n"
	     "  dba %p\n"
	     "  Pstr %p\n"
	     "  str '%S'\n",
	     dba,
	     Pstr,
	     str);
     
     ensDatabaseAdaptorTrace(dba, 1);
     */
    
    return ensDatabaseConnectionEscapeSS(dba->DatabaseConnection, Pstr, str);
}




/* @section debugging *********************************************************
**
** Functions for reporting of an Ensembl Database Adaptor object.
**
** @fdata [EnsPDatabaseAdaptor]
** @nam3rule Trace Report Ensembl Database Adaptor elements to debug file
**
** @argrule Trace dba [const EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @argrule Trace level [ajuint] Indentation level
**
** @valrule * [AjBool] ajTrue upon success, ajFalse otherwise
**
** @fcategory misc
******************************************************************************/




/* @func ensDatabaseAdaptorTrace **********************************************
**
** Trace an Ensembl Database Adaptor.
**
** @param [r] dba [const EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [r] level [ajuint] Indentation level
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseAdaptorTrace(const EnsPDatabaseAdaptor dba, ajuint level)
{
    AjPStr indent = NULL;
    
    if(!dba)
	return ajFalse;
    
    indent = ajStrNew();
    
    ajStrAppendCountK(&indent, ' ', level * 2);
    
    ajDebug("%SensDatabaseAdaptorTrace %p\n"
	    "%S  DatabaseConnection %p\n"
	    "%S  Species '%S'\n"
	    "%S  Group '%s'\n"
	    "%S  MultiSpecies %B\n"
	    "%S  Identifier %u\n",
	    indent, dba,
	    indent, dba->DatabaseConnection,
	    indent, dba->Species,
	    indent, ensDatabaseAdaptorGroupToChar(dba->Group),
	    indent, dba->MultiSpecies,
	    indent, dba->Identifier);
    
    ensDatabaseConnectionTrace(dba->DatabaseConnection, level + 1);
    
    ajStrDel(&indent);
    
    return ajTrue;
}




/* @func ensDatabaseAdaptorGetSchemaBuild *************************************
**
** Get the schema build of an Ensembl database, which is the software version
** and the data version separated by an underscore.
** So for database homo_sapiens_core_50_36l the schema build would be 50_36l.
**
** @param [r] dba [const EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @param [u] Pbuild [AjPStr*] Schema build
**
** @return [AjBool] ajTrue upon success, ajFalse otherwise
** @@
******************************************************************************/

AjBool ensDatabaseAdaptorGetSchemaBuild(EnsPDatabaseAdaptor dba, AjPStr *Pbuild)
{
    ajuint i = 0;
    ajuint tokens = 0;
    
    AjPStr database = NULL;
    AjPStr temp = NULL;
    AjPStrTok token = NULL;
    
    if(!dba)
	return ajFalse;
    
    if(!Pbuild)
	return ajFalse;
    
    ajStrAssignClear(Pbuild);
    
    database = ensDatabaseConnectionGetDatabaseName(dba->DatabaseConnection);
    
    tokens = ajStrParseCountC(database, "_");
    
    if(tokens < 2)
	return ajFalse;
    
    token = ajStrTokenNewC(database, "_");
    
    /* Parse all string tokens but the last two. */
    
    temp = ajStrNew();
    
    for(i = 0; i < (tokens - 2); i++)
	ajStrTokenNextParse(&token, &temp);
    
    ajStrDel(&temp);
    
    ajStrTokenRestParse(&token, Pbuild);
    
    ajStrTokenDel(&token);
    
    return ajTrue;
}
