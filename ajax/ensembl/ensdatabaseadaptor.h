#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensdatabaseadaptor_h
#define ensdatabaseadaptor_h

#include "ensdatabaseconnection.h"




/* @datatype EnsEDatabaseAdaptorGroup *****************************************
**
** Ensembl Database Adaptor group enumeration.
**
** @attr enum [EnsEDatabaseAdaptorGroup] Value
** @@
******************************************************************************/

enum EnsEDatabaseAdaptorGroup
{
    ensEDatabaseAdaptorGroupNULL,
    ensEDatabaseAdaptorGroupCore,
    ensEDatabaseAdaptorGroupVega,
    ensEDatabaseAdaptorGroupOtherFeatures,
    ensEDatabaseAdaptorGroupCopyDNA,
    ensEDatabaseAdaptorGroupGeneticVariation,
    ensEDatabaseAdaptorGroupFunctionalGenomics,
    ensEDatabaseAdaptorGroupComparativeGenomics,
    ensEDatabaseAdaptorGroupGeneOntology,
    ensEDatabaseAdaptorGroupQualityCheck,
    ensEDatabaseAdaptorGroupPipeline,
    ensEDatabaseAdaptorGroupHive,
    ensEDatabaseAdaptorGroupCoreExpressionEST,
    ensEDatabaseAdaptorGroupCoreExpressionGNF,
    ensEDatabaseAdaptorGroupAncestral,
    ensEDatabaseAdaptorGroupWebsite
};




/* @data EnsPDatabaseAdaptor **************************************************
**
** Ensembl Database Adaptor.
**
** Holds the Ensembl Database Group, the species name and the Ensembl
** Database Connection object connected to this SQL database.
**
** @alias EnsSDatabaseAdaptor
** @alias EnsODatabaseAdaptor
**
** @attr DatabaseConnection [EnsPDatabaseConnection] Ensembl Database
**                                                   Connection
** @attr Species [AjPStr] Species
** @attr Group [AjEnum] Group
** @attr MultiSpecies [AjBool] Multi-species database
** @attr Identifier [ajuint] Species identifier, defaults to 1
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSDatabaseAdaptor
{
    EnsPDatabaseConnection DatabaseConnection;
    AjPStr Species;
    AjEnum Group;
    AjBool MultiSpecies;
    ajuint Identifier;
    ajuint Padding;
} EnsODatabaseAdaptor;

#define EnsPDatabaseAdaptor EnsODatabaseAdaptor*




/*
** Prototype definitions
*/

EnsPDatabaseAdaptor ensDatabaseAdaptorNew(EnsPDatabaseConnection dbc,
                                          AjPStr database,
                                          AjPStr species,
                                          AjEnum group,
                                          AjBool multi,
                                          ajuint identifier);

void ensDatabaseAdaptorDel(EnsPDatabaseAdaptor* Pdba);

EnsPDatabaseConnection ensDatabaseAdaptorGetDatabaseConnection(
    const EnsPDatabaseAdaptor dba);

AjPStr ensDatabaseAdaptorGetSpecies(const EnsPDatabaseAdaptor dba);

AjEnum ensDatabaseAdaptorGetGroup(const EnsPDatabaseAdaptor dba);

AjBool ensDatabaseAdaptorGetMultiSpecies(const EnsPDatabaseAdaptor dba);

ajuint ensDatabaseAdaptorGetIdentifier(const EnsPDatabaseAdaptor dba);

AjBool ensDatabaseAdaptorSetDatabaseConnection(EnsPDatabaseAdaptor dba,
                                               EnsPDatabaseConnection dbc);

AjBool ensDatabaseAdaptorSetSpecies(EnsPDatabaseAdaptor dba, AjPStr species);

AjBool ensDatabaseAdaptorSetGroup(EnsPDatabaseAdaptor dba, AjEnum group);

AjBool ensDatabaseAdaptorSetMultiSpecies(EnsPDatabaseAdaptor dba, AjBool multi);

AjBool ensDatabaseAdaptorSetIdentifier(EnsPDatabaseAdaptor dba,
                                       ajuint identifier);

AjEnum ensDatabaseAdaptorGroupFromStr(const AjPStr group);

const char *ensDatabaseAdaptorGroupToChar(const AjEnum group);

AjBool ensDatabaseAdaptorMatch(const EnsPDatabaseAdaptor dba1,
			const EnsPDatabaseAdaptor dba2);

AjBool ensDatabaseAdaptorMatchComponents(const EnsPDatabaseAdaptor dba,
                                         const EnsPDatabaseConnection dbc,
                                         const AjPStr species,
                                         AjEnum group,
                                         AjBool multi,
                                         ajuint identifier);

AjPSqlStatement ensDatabaseAdaptorSqlStatementNew(EnsPDatabaseAdaptor dba,
                                                  const AjPStr statement);

AjBool ensDatabaseAdaptorEscapeCS(EnsPDatabaseAdaptor dba,
                                  char **Ptxt,
                                  const AjPStr str);

AjBool ensDatabaseAdaptorEscapeSS(EnsPDatabaseAdaptor dba,
                                  AjPStr *Pstr,
                                  const AjPStr str);

AjBool ensDatabaseAdaptorTrace(const EnsPDatabaseAdaptor dba, ajuint level);

AjBool ensDatabaseAdaptorGetSchemaBuild(EnsPDatabaseAdaptor dba,
                                        AjPStr *Pbuild);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
