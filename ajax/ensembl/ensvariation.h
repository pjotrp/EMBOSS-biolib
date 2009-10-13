#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensvariation_h
#define ensvariation_h

#include "ensfeature.h"




/* @data EnsPGvvsampleadaptor **************************************************
**
** Ensembl Genetic Variation Sample Adaptor.
**
** @alias EnsSGvvsampleadaptor
** @alias EnsOGvvsampleadaptor
**
** @attr Adaptor [EnsPBaseadaptor] Ensembl Base Adaptor
** @@
******************************************************************************/

typedef struct EnsSGvvsampleadaptor
{
    EnsPBaseadaptor Adaptor;
} EnsOGvvsampleadaptor;

#define EnsPGvvsampleadaptor EnsOGvvsampleadaptor*




/* @data EnsPGvvsample *********************************************************
**
** Ensembl Genetic Variation Sample.
**
** @alias EnsSGvvsample
** @alias EnsOGvvsample
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGvvsampleadaptor] Ensembl Genetic Variation
**                                     Sample Adaptor
** @cc Bio::EnsEMBL::Variation::Sample
** @attr Name [AjPStr] Name
** @attr Description [AjPStr] Description
** @attr Size [ajuint] Size
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

/*
** FIXME: The Sample seems to be a super class for Population, Individual and
** others. Most likely it does not need its own identifier and adaptor
** elements.
*/

typedef struct EnsSGvvsample
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvvsampleadaptor Adaptor;
    AjPStr Name;
    AjPStr Description;
    ajuint Size;
    ajuint Padding;
} EnsOGvvsample;

#define EnsPGvvsample EnsOGvvsample*




/* @data EnsPGvindividualadaptor **********************************************
**
** Ensembl Genetic Variation Individual Adaptor.
**
** @alias EnsSGvindividualadaptor
** @alias EnsOGvindividualadaptor
**
** @attr Adaptor [EnsPGvvsampleadaptor] Ensembl Genetic Variation
**                                     Sample Adaptor
** @@
******************************************************************************/

typedef struct EnsSGvindividualadaptor
{
    EnsPGvvsampleadaptor Adaptor;
} EnsOGvindividualadaptor;

#define EnsPGvindividualadaptor EnsOGvindividualadaptor*




/******************************************************************************
**
** Ensembl Genetic Variation Individual Gender enumeration.
**
******************************************************************************/

enum EnsEGvindividualGender
{
    ensEGvindividualGenderNULL,
    ensEGvindividualGenderMale,
    ensEGvindividualGenderFemale,
    ensEGvindividualGenderUnknown
};




/******************************************************************************
**
** Ensembl Genetic Variation Individual Type enumeration.
**
******************************************************************************/

enum EnsEGvindividualType
{
    ensEGvindividualTypeNULL,
    ensEGvindividualTypeFullyInbred,
    ensEGvindividualTypePartlyInbred,
    ensEGvindividualTypeOutbred,
    ensEGvindividualTypeMutant,
    ensEGvindividualTypeUnknown
};




/* @data EnsPGvindividual *****************************************************
**
** Ensembl Genetic Variation Individual.
**
** @alias EnsSGvindividual
** @alias EnsOGvindividual
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGvindividualadaptor] Ensembl Genetic Variation
**                                         Individual Adaptor
** @cc Bio::EnsEMBL::Variation::Sample
** @attr Gvvsample [EnsPGvvsample] Sample
** @cc Bio::EnsEMBL::Variation::Individual
** @attr Father [struct EnsSGvindividual*] Father Individual
** @attr Mother [struct EnsSGvindividual*] Mother Individual
** @attr Gender [AjEnum] Gender
** @attr Type [AjEnum] Type
** @attr Description [AjPStr] Description
** @@
******************************************************************************/

typedef struct EnsSGvindividual
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvindividualadaptor Adaptor;
    EnsPGvvsample Gvvsample;
    struct EnsSGvindividual* Father;
    struct EnsSGvindividual* Mother;
    AjEnum Gender;
    AjEnum Type;
    AjPStr Description;
} EnsOGvindividual;

#define EnsPGvindividual EnsOGvindividual*




/* @data EnsPGvpopulationadaptor **********************************************
**
** Ensembl Genetic Variation Population Adaptor.
**
** @alias EnsSGvpopulationadaptor
** @alias EnsOGvpopulationadaptor
**
** @attr Adaptor [EnsPGvvsampleadaptor] Ensembl Genetic Variation
**                                     Sample Adaptor
** @@
******************************************************************************/

typedef struct EnsSGvpopulationadaptor
{
    EnsPGvvsampleadaptor Adaptor;
} EnsOGvpopulationadaptor;

#define EnsPGvpopulationadaptor EnsOGvpopulationadaptor*




/* @data EnsPGvpopulation *****************************************************
**
** Ensembl Genetic Variation Population.
**
** @alias EnsSGvpopulation
** @alias EnsOGvpopulation
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGvpopulationadaptor] Ensembl Genetic Variation
**                                         Population Adaptor
** @cc Bio::EnsEMBL::Variation::Sample
** @attr Gvvsample [EnsPGvvsample] Sample
** @cc Bio::EnsEMBL::Variation::Population
** @attr SubPopulations [AjPList] AJAX List of Ensembl Genetic Variation
**                                (Sub-) Populations
** @@
******************************************************************************/

typedef struct EnsSGvpopulation
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvpopulationadaptor Adaptor;
    EnsPGvvsample Gvvsample;
    AjPList SubPopulations;
} EnsOGvpopulation;

#define EnsPGvpopulation EnsOGvpopulation*




/* @data EnsPGvalleleadaptor **************************************************
**
** Ensembl Genetic Variation Allele Adaptor.
**
** @alias EnsSGvalleleadaptor
** @alias EnsOGvalleleadaptor
**
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor.
** @@
** NOTE: Although the Bio::EnsEMBL::Variation::Allele object is based on the
** Bio::EnsEMBL::Storable object, there is actually no
** Bio::EnsEMBL::Variation::DBSQL::Alleleadaptor as such in the
** Ensembl Variation API.
******************************************************************************/

typedef struct EnsSGvalleleadaptor
{
    EnsPDatabaseadaptor Adaptor;
} EnsOGvalleleadaptor;

#define EnsPGvalleleadaptor EnsOGvalleleadaptor*




/* @data EnsPGvallele *********************************************************
**
** Ensembl Genetic Variation Allele.
**
** @alias EnsSGvallele
** @alias EnsOGvallele
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGvalleleadaptor] Ensembl Genetic Variation
**                                     Allele Adaptor
** @cc Bio::EnsEMBL::Variation::Allele
** @attr Gvpopulation [EnsPGvpopulation] Ensembl Genetic Variation Population
** @attr Allele [AjPStr] Allele
** @attr Frequency [float] Frequency
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSGvallele
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvalleleadaptor Adaptor;
    EnsPGvpopulation Gvpopulation;
    AjPStr Allele;
    float Frequency;
    ajuint Padding;
} EnsOGvallele;

#define EnsPGvallele EnsOGvallele*




/* @data EnsPGvgenotypeadaptor ************************************************
**
** Ensembl Genetic Variation Genotype Adaptor.
**
** @alias EnsSGvgenotypeadaptor
** @alias EnsOGvgenotypeadaptor
**
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct EnsSGvgenotypeadaptor
{
    EnsPDatabaseadaptor Adaptor;
} EnsOGvgenotypeadaptor;

#define EnsPGvgenotypeadaptor EnsOGvgenotypeadaptor*




/* @data EnsPGvgenotype *******************************************************
**
** Ensembl Genetic Variation Genotype.
**
** @alias EnsSGvgenotype
** @alias EnsOGvgenotype
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGvgenotypeadaptor] Ensembl Genetic Variation
**                                       Genotype Adaptor
** @cc Bio::EnsEMBL::Variation::Genotype
** @attr Allele1 [AjPStr] Allele 1
** @attr Allele2 [AjPStr] Allele 2
** @@
******************************************************************************/

typedef struct EnsSGvgenotype
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvgenotypeadaptor Adaptor;
    AjPStr Allele1;
    AjPStr Allele2;
} EnsOGvgenotype;

#define EnsPGvgenotype EnsOGvgenotype*




/* @data EnsPGvvariationadaptor ***********************************************
**
** Ensembl Genetic Variation Variation Adaptor.
**
** @alias EnsSGvvariationadaptor
** @alias EnsOGvvariationadaptor
**
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct EnsSGvvariationadaptor
{
    EnsPDatabaseadaptor Adaptor;
} EnsOGvvariationadaptor;

#define EnsPGvvariationadaptor EnsOGvvariationadaptor*




/******************************************************************************
**
** Ensembl Genetic Variation Variation validation state enumeration.
**
******************************************************************************/

enum EnsEGvvariationValidationState
{
    ensEGvvariationValidationStateNULL,
    ensEGvvariationValidationStateCluster,
    ensEGvvariationValidationStateFrequency,
    ensEGvvariationValidationStateSubmitter,
    ensEGvvariationValidationStateDoublehit,
    ensEGvvariationValidationStateHapMap,
    ensEGvvariationValidationStateFailed,
    ensEGvvariationValidationStateNonPolymorphic,
    ensEGvvariationValidationStateObserved,
    ensEGvvariationValidationStateUnknown
};




/* @data EnsPGvvariation ******************************************************
**
** Ensembl Genetic Variation Variation.
**
** @alias EnsSGvvariation
** @alias EnsOGvvariation
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGvvariationadaptor] Ensembl Genetic Variation
**                                        Variation Adaptor
** @cc Bio::EnsEMBL::Variation::Variation
** @attr Name [AjPStr] Name
** @attr Source [AjPStr] Source
** @attr Synonyms [AjPTable] Synonyms
** @attr AncestralAllele [AjPStr] Ancestral allele
** @attr Gvalleles [AjPList] AJAX List of Ensembl Genetic Variation Alleles
** @attr ValidationStates [AjPList] AJAX List of AJAX Strings of validation
**                                  states
** @attr MoleculeType [AjPStr] Molecule type
** @attr FivePrimeFlank [AjPStr] Five prime flanking sequence
** @attr ThreePrimeFlank [AjPStr] Three prime flanking sequence
** @attr FailedDescription [AjPStr] Failed description
** @@
******************************************************************************/

typedef struct EnsSGvvariation
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvvariationadaptor Adaptor;
    AjPStr Name;
    AjPStr Source;
    AjPTable Synonyms;
    AjPStr AncestralAllele;
    AjPList Gvalleles;
    AjPList ValidationStates;
    AjPStr MoleculeType;
    AjPStr FivePrimeFlank;
    AjPStr ThreePrimeFlank;
    AjPStr FailedDescription;
} EnsOGvvariation;

#define EnsPGvvariation EnsOGvvariation*




/* @data EnsPGvvariationfeatureadaptor ****************************************
**
** Ensembl Genetic Variation Variation Feature Adaptor.
**
** @alias EnsSGvvariationfeatureadaptor
** @alias EnsOGvvariationfeatureadaptor
**
** @attr Adaptor [EnsPFeatureadaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSGvvariationfeatureadaptor
{
    EnsPFeatureadaptor Adaptor;
} EnsOGvvariationfeatureadaptor;

#define EnsPGvvariationfeatureadaptor EnsOGvvariationfeatureadaptor*




/* @data EnsPGvvariationfeature ***********************************************
**
** Ensembl Genetic Variation Variation Feature.
**
** @alias EnsSGvvariationfeature
** @alias EnsOGvvariationfeature
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGvvariationfeatureadaptor] Ensembl Genetic Variation
**                                       Genotype Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Variation::VariationFeature
** @attr Gvvariation [EnsPGvvariation] Ensembl Variation
** @attr Name [AjPStr] Name
** @attr Source [AjPStr] Source
** @attr ValidationCode [AjPStr] Validation code
** @attr ConsequenceType [AjPStr] Consequence type
** @attr MapWeight [ajuint] Map weight or the number of times that the
**                          Variation associated with this Feature has hit
**                          the genome. If this was the only Feature associated
**                          with this Variation Feature the map weight would be
**                          1.
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSGvvariationfeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvvariationfeatureadaptor Adaptor;
    EnsPFeature Feature;
    EnsPGvvariation Gvvariation;
    AjPStr Name;
    AjPStr Source;
    AjPStr ValidationCode;
    AjPStr ConsequenceType;
    ajuint MapWeight;
    ajuint Padding;
} EnsOGvvariationfeature;

#define EnsPGvvariationfeature EnsOGvvariationfeature*




/*
** Prototype definitions
*/

/* Ensembl Genetic Variation Sample */

EnsPGvvsample ensGvvsampleNew(EnsPGvvsampleadaptor gvsa,
                            ajuint identifier,
                            AjPStr name,
                            AjPStr description,
                            ajuint size);

EnsPGvvsample ensGvvsampleNewObj(const EnsPGvvsample object);

EnsPGvvsample ensGvvsampleNewRef(EnsPGvvsample gvs);

void ensGvvsampleDel(EnsPGvvsample* Pgvs);

EnsPGvvsampleadaptor ensGvvsampleGetAdaptor(const EnsPGvvsample gvs);

ajuint ensGvvsampleGetIdentifier(const EnsPGvvsample gvs);

AjPStr ensGvvsampleGetName(const EnsPGvvsample gvs);

AjPStr ensGvvsampleGetDescription(const EnsPGvvsample gvs);

ajuint ensGvvsampleGetSize(const EnsPGvvsample gvs);

AjBool ensGvvsampleSetAdaptor(EnsPGvvsample gvs, EnsPGvvsampleadaptor adaptor);

AjBool ensGvvsampleSetIdentifier(EnsPGvvsample gvs, ajuint identifier);

AjBool ensGvvsampleSetName(EnsPGvvsample gvs, AjPStr name);

AjBool ensGvvsampleSetDescription(EnsPGvvsample gvs, AjPStr description);

AjBool ensGvvsampleSetSize(EnsPGvvsample gvs, ajuint size);

ajuint ensGvvsampleGetMemSize(const EnsPGvvsample gvs);

AjBool ensGvvsampleTrace(const EnsPGvvsample gvs, ajuint level);

/* Ensembl Genetic Variation Sample Adaptor */

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
                  AjPList objects));

void ensGvvsampleadaptorDel(EnsPGvvsampleadaptor *Pgvsa);

EnsPBaseadaptor ensGvvsampleadaptorGetAdaptor(const EnsPGvvsampleadaptor gvsa);

AjBool ensGvvsampleadaptorFetchAllSynonymsByIdentifier(
    const EnsPGvvsampleadaptor gvsa,
    ajuint identifier,
    const AjPStr source,
    AjPList synonyms);

AjBool ensGvvsampleadaptorFetchAllIdentifiersBySynonym(
    const EnsPGvvsampleadaptor gvsa,
    const AjPStr synonym,
    const AjPStr source,
    AjPList idlist);

AjBool ensGvvsampleadaptorFetchByIdentifier(const EnsPGvvsampleadaptor gvsa,
                                           ajuint identifier,
                                           EnsPGvvsample *Pgvs);

/* Ensembl Genetic Variation Individual */

EnsPGvindividual ensGvindividualNew(EnsPGvindividualadaptor gvia,
                                    ajuint identifier,
                                    EnsPGvvsample gvs,
                                    EnsPGvindividual father,
                                    EnsPGvindividual mother,
                                    AjEnum gender,
                                    AjEnum type,
                                    AjPStr description);

EnsPGvindividual ensGvindividualNewObj(const EnsPGvindividual object);

EnsPGvindividual ensGvindividualNewRef(EnsPGvindividual gvi);

void ensGvindividualDel(EnsPGvindividual* Pgvi);

EnsPGvindividualadaptor ensGvindividualGetAdaptor(const EnsPGvindividual gvi);

ajuint ensGvindividualGetIdentifier(const EnsPGvindividual gvi);

EnsPGvvsample ensGvindividualGetGvvsample(const EnsPGvindividual gvi);

EnsPGvindividual ensGvindividualGetFather(const EnsPGvindividual gvi);

EnsPGvindividual ensGvindividualGetMother(const EnsPGvindividual gvi);

AjEnum ensGvindividualGetGender(const EnsPGvindividual gvi);

AjEnum ensGvindividualGetType(const EnsPGvindividual gvi);

AjPStr ensGvindividualGetDescription(const EnsPGvindividual gvi);

AjBool ensGvindividualSetAdaptor(EnsPGvindividual gvi,
                                 EnsPGvindividualadaptor gvia);

AjBool ensGvindividualSetIdentifier(EnsPGvindividual gvi, ajuint identifier);

AjBool ensGvindividualSetGvvsample(EnsPGvindividual gvi, EnsPGvvsample gvs);

AjBool ensGvindividualSetFather(EnsPGvindividual gvi, EnsPGvindividual father);

AjBool ensGvindividualSetMother(EnsPGvindividual gvi, EnsPGvindividual mother);

AjBool ensGvindividualSetGender(EnsPGvindividual gvi, AjEnum gender);

AjBool ensGvindividualSetType(EnsPGvindividual gvi, AjEnum type);

AjBool ensGvindividualSetDescription(EnsPGvindividual gvi, AjPStr description);

AjEnum ensGvindividualGenderFromStr(const AjPStr gender);

AjEnum ensGvindividualTypeFromStr(const AjPStr type);

const char *ensGvindividualGenderToChar(const AjEnum gender);

const char *ensGvindividualTypeToChar(const AjEnum type);

ajuint ensGvindividualGetMemSize(const EnsPGvindividual gvi);

AjBool ensGvindividualTrace(const EnsPGvindividual gvi, ajuint level);

/* Ensembl Genetic Variation Individual Adaptor */

EnsPGvindividualadaptor ensGvindividualadaptorNew(EnsPDatabaseadaptor dba);

void ensGvindividualadaptorDel(EnsPGvindividualadaptor *Pgvia);

EnsPGvvsampleadaptor ensGvindividualadaptorGetAdaptor(
    const EnsPGvindividualadaptor gvia);

AjBool ensGvindividualadaptorFetchByIdentifier(const EnsPGvindividualadaptor gvia,
                                               ajuint identifier,
                                               EnsPGvindividual *Pgvi);

AjBool ensGvindividualadaptorFetchAllByName(const EnsPGvindividualadaptor gvia,
                                            const AjPStr name,
                                            AjPList gvis);

AjBool ensGvindividualadaptorFetchAllBySynonym(const EnsPGvindividualadaptor adaptor,
                                               const AjPStr synonym,
                                               AjPList individuals);

AjBool ensGvindividualadaptorFetchAllByPopulation(const EnsPGvindividualadaptor gvia,
                                                  const EnsPGvpopulation gvp,
                                                  AjPList gvis);

AjBool ensGvindividualadaptorFetchAllByParentIndividual(
    const EnsPGvindividualadaptor gvia,
    const EnsPGvindividual parent,
    AjPList gvis);

AjBool ensGvindividualadaptorFetchAllStrains(const EnsPGvindividualadaptor gvia,
                                             AjPList gvis);

AjBool ensGvindividualadaptorFetchReferenceStrainName(
    const EnsPGvindividualadaptor gvia,
    AjPStr *Pname);

AjBool ensGvindividualadaptorFetchAllDefaultStrains(
    const EnsPGvindividualadaptor gvia,
    AjPList names);

AjBool ensGvindividualadaptorFetchAllDisplayStrains(
    const EnsPGvindividualadaptor gvia,
    AjPList individuals);

AjBool ensGvindividualadaptorFetchAllStrainsWithCoverage(
    const EnsPGvindividualadaptor gvia,
    AjPList idlist);

/* Ensembl Genetic Variation Population */

EnsPGvpopulation ensGvpopulationNew(EnsPGvpopulationadaptor gvpa,
                                    ajuint identifier,
                                    EnsPGvvsample gvs,
                                    AjPList subpopulations);

EnsPGvpopulation ensGvpopulationNewObj(const EnsPGvpopulation object);

EnsPGvpopulation ensGvpopulationNewRef(EnsPGvpopulation gvp);

void ensGvpopulationDel(EnsPGvpopulation* Pgvp);

EnsPGvpopulationadaptor ensGvpopulationGetAdaptor(const EnsPGvpopulation gvp);

ajuint ensGvpopulationGetIdentifier(const EnsPGvpopulation gvp);

EnsPGvvsample ensGvpopulationGetGvvsample(const EnsPGvpopulation gvp);

const AjPList ensGvpopulationGetSubPopulations(const EnsPGvpopulation gvp);

AjBool ensGvpopulationSetAdaptor(EnsPGvpopulation gvp,
                                 EnsPGvpopulationadaptor gvpa);

AjBool ensGvpopulationSetIdentifier(EnsPGvpopulation gvp, ajuint identifier);

AjBool ensGvpopulationSetGvvsample(EnsPGvpopulation gvp, EnsPGvvsample gvs);

AjBool ensGvpopulationAddSubPopulation(EnsPGvpopulation gvp,
                                       EnsPGvpopulation subgvp);

ajuint ensGvpopulationGetMemSize(const EnsPGvpopulation gvp);

AjBool ensGvpopulationTrace(const EnsPGvpopulation gvp, ajuint level);

/* Ensembl Genetic Variation Population Adaptor */

EnsPGvpopulationadaptor ensGvpopulationadaptorNew(EnsPDatabaseadaptor dba);

void ensGvpopulationadaptorDel(EnsPGvpopulationadaptor *Pgvpa);

EnsPGvvsampleadaptor ensGvpopulationadaptorGetAdaptor(
    const EnsPGvpopulationadaptor gvpa);

AjBool ensGvpopulationadaptorFetchByIdentifier(const EnsPGvpopulationadaptor gvpa,
                                               ajuint identifier,
                                               EnsPGvpopulation *Pgvp);

AjBool ensGvpopulationadaptorFetchByName(const EnsPGvpopulationadaptor gvpa,
                                         const AjPStr name,
                                         EnsPGvpopulation* Pgvp);

AjBool ensGvpopulationadaptorFetchAllBySynonym(const EnsPGvpopulationadaptor gvpa,
                                               const AjPStr synonym,
                                               AjPList gvps);

AjBool ensGvpopulationadaptorFetchAllBySuperPopulation(
    const EnsPGvpopulationadaptor gvpa,
    const EnsPGvpopulation gvp,
    AjPList gvps);

AjBool ensGvpopulationadaptorFetchAllBySubPopulation(
    const EnsPGvpopulationadaptor gvpa,
    const EnsPGvpopulation gvp,
    AjPList gvps);

AjBool ensGvpopulationadaptorFetchAllByIndvividual(
    const EnsPGvpopulationadaptor gvpa,
    const EnsPGvindividual gvi,
    AjPList gvps);

/* Ensembl Genetic Variation Allele */

EnsPGvallele ensGvalleleNew(EnsPGvalleleadaptor gvaa,
                            ajuint identifier,
                            EnsPGvpopulation gvp,
                            AjPStr allele,
                            float frequency);

EnsPGvallele ensGvalleleNewObj(const EnsPGvallele object);

EnsPGvallele ensGvalleleNewRef(EnsPGvallele gva);

void ensGvalleleDel(EnsPGvallele* Pgva);

EnsPGvalleleadaptor ensGvalleleGetAdaptor(const EnsPGvallele gva);

ajuint ensGvalleleGetIdentifier(const EnsPGvallele gva);

EnsPGvpopulation ensGvalleleGetPopulation(const EnsPGvallele gva);

AjPStr ensGvalleleGetAllele(const EnsPGvallele gva);

float ensGvalleleGetFrequency(const EnsPGvallele gva);

AjBool ensGvalleleSetAdaptor(EnsPGvallele gva, EnsPGvalleleadaptor gvaa);

AjBool ensGvalleleSetIdentifier(EnsPGvallele gva, ajuint identifier);

AjBool ensGvalleleSetPopulation(EnsPGvallele gva, EnsPGvpopulation gvp);

AjBool ensGvalleleSetAllele(EnsPGvallele gva, AjPStr allelestr);

AjBool ensGvalleleSetFrequency(EnsPGvallele gva, float frequency);

ajuint ensGvalleleGetMemSize(const EnsPGvallele gva);

AjBool ensGvalleleTrace(const EnsPGvallele gva, ajuint level);

/* Ensembl Genetic Variation Genotype */

EnsPGvgenotype ensGvgenotypeNew(EnsPGvgenotypeadaptor gvga,
                                ajuint identifier,
                                AjPStr allele1,
                                AjPStr allele2);

EnsPGvgenotype ensGvgenotypeNewObj(const EnsPGvgenotype object);

EnsPGvgenotype ensGvgenotypeNewRef(EnsPGvgenotype gvg);

void ensGvgenotypeDel(EnsPGvgenotype* Pgvg);

EnsPGvgenotypeadaptor ensGvgenotypeGetAdaptor(const EnsPGvgenotype gvg);

ajuint ensGvgenotypeGetIdentifier(const EnsPGvgenotype gvg);

AjPStr ensGvgenotypeGetAllele1(const EnsPGvgenotype gvg);

AjPStr ensGvgenotypeGetAllele2(const EnsPGvgenotype gvg);

AjBool ensGvgenotypeSetAdaptor(EnsPGvgenotype gvg, EnsPGvgenotypeadaptor gvga);

AjBool ensGvgenotypeSetIdentifier(EnsPGvgenotype gvg, ajuint identifier);

AjBool ensGvgenotypeSetAllele1(EnsPGvgenotype gvg, AjPStr allele1);

AjBool ensGvgenotypeSetAllele2(EnsPGvgenotype gvg, AjPStr allele2);

ajuint ensGvgenotypeGetMemSize(const EnsPGvgenotype gvg);

AjBool ensGvgenotypeTrace(const EnsPGvgenotype gvg, ajuint level);

/* Ensembl Genetic Variation Variation */

EnsPGvvariation ensGvvariationNew(EnsPGvvariationadaptor adaptor,
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
                                  AjPStr faileddescription);

EnsPGvvariation ensGvvariationNewObj(const EnsPGvvariation object);

EnsPGvvariation ensGvvariationNewRef(EnsPGvvariation gvv);

void ensGvvariationDel(EnsPGvvariation* Pgvv);

EnsPGvvariationadaptor ensGvvariationGetAdaptor(const EnsPGvvariation gvv);

ajuint ensGvvariationGetIdentifier(const EnsPGvvariation gvv);

AjPStr ensGvvariationGetName(const EnsPGvvariation gvv);

AjPStr ensGvvariationGetSource(const EnsPGvvariation gvv);

const AjPTable ensGvvariationGetSynonyms(const EnsPGvvariation gvv);

AjPStr ensGvvariationGetAncestralAllele(const EnsPGvvariation gvv);

const AjPList ensGvvariationGetGvalleles(const EnsPGvvariation gvv);

const AjPList ensGvvariationGetValidationStates(const EnsPGvvariation gvv);

AjPStr ensGvvariationGetMoleculeType(const EnsPGvvariation gvv);

AjPStr ensGvvariationGetFivePrimeFlank(const EnsPGvvariation gvv);

AjPStr ensGvvariationGetThreePrimeFlank(const EnsPGvvariation gvv);

AjPStr ensGvvariationGetFailedDescription(const EnsPGvvariation gvv);

AjBool ensGvvariationSetAdaptor(EnsPGvvariation gvv,
                                EnsPGvvariationadaptor gvva);

AjBool ensGvvariationSetIdentifier(EnsPGvvariation gvv, ajuint identifier);

AjBool ensGvvariationSetName(EnsPGvvariation gvv, AjPStr name);

AjBool ensGvvariationSetSource(EnsPGvvariation gvv, AjPStr source);

AjBool ensGvvariationSetMoleculeType(EnsPGvvariation gvv, AjPStr moltype);

AjBool ensGvvariationSetAncestralAllele(EnsPGvvariation gvv,
                                        AjPStr ancestralallele);

AjBool ensGvvariationSetFivePrimeFlank(EnsPGvvariation gvv, AjPStr fiveflank);

AjBool ensGvvariationSetThreePrimeFlank(EnsPGvvariation gvv, AjPStr threeflank);

AjBool ensGvvariationSetFailedDescription(EnsPGvvariation gvv,
                                          AjPStr faileddescription);

ajuint ensGvvariationGetMemSize(const EnsPGvvariation gvv);

AjBool ensGvvariationTrace(const EnsPGvvariation gvv, ajuint level);

AjBool ensGvvariationAddSynonym(EnsPGvvariation gvv, AjPStr source,
                                AjPStr synonym);

AjBool ensGvvariationAddGvallele(EnsPGvvariation gvv, EnsPGvallele gva);

AjEnum ensGvvariationValidationStateFromStr(const AjPStr state);

const char *ensGvvariationValidationStateToChar(const AjEnum state);

AjBool ensGvvariationFetchAllSynonyms(const EnsPGvvariation gvv,
                                      const AjPStr source,
                                      AjPList synonyms);

AjBool ensGvvariationFetchAllSynonymSources(const EnsPGvvariation gvv,
                                            AjPList sources);

/* Ensembl Genetic Variation Variation Adaptor */

EnsPGvvariationadaptor ensGvvariationadaptorNew(EnsPDatabaseadaptor dba);

void ensGvvariationadaptorDel(EnsPGvvariationadaptor *Pgvva);

AjBool ensGvvariationadaptorFetchByIdentifier(EnsPGvvariationadaptor gvva,
                                              ajuint identifier,
                                              EnsPGvvariation *Pgvv);

AjBool ensGvvariationadaptorFetchByName(EnsPGvvariationadaptor gvva,
                                        const AjPStr name,
                                        const AjPStr source,
                                        EnsPGvvariation *Pgvv);

AjBool ensGvvariationadaptorFetchAllBySource(EnsPGvvariationadaptor gvva,
                                             const AjPStr source,
                                             AjBool primary,
                                             AjPList gvvs);

/* Ensembl Genetic Variation Variation Feature */

EnsPGvvariationfeature ensGvvariationfeatureNew(
    EnsPGvvariationfeatureadaptor adaptor,
    ajuint identifier,
    EnsPFeature feature,
    EnsPGvvariation gvv,
    AjPStr name,
    AjPStr source,
    AjPStr validation,
    AjPStr consequence,
    ajuint mapweight);

EnsPGvvariationfeature ensGvvariationfeatureNewObj(
    const EnsPGvvariationfeature object);

EnsPGvvariationfeature ensGvvariationfeatureNewRef(EnsPGvvariationfeature gvvf);

void ensGvvariationfeatureDel(EnsPGvvariationfeature *Pgvvf);

EnsPGvvariationfeatureadaptor ensGvvariationfeatureGetAdaptor(
    const EnsPGvvariationfeature gvvf);

ajuint ensGvvariationfeatureGetIdentifier(const EnsPGvvariationfeature gvvf);

EnsPFeature ensGvvariationfeatureGetFeature(const EnsPGvvariationfeature gvvf);

EnsPGvvariation ensGvvariationfeatureGetGvvariation(
    const EnsPGvvariationfeature gvvf);

AjPStr ensGvvariationfeatureGetName(const EnsPGvvariationfeature gvvf);

AjPStr ensGvvariationfeatureGetSource(const EnsPGvvariationfeature gvvf);

AjPStr ensGvvariationfeatureGetValidationCode(
    const EnsPGvvariationfeature gvvf);

AjPStr ensGvvariationfeatureGetConsequenceType(
    const EnsPGvvariationfeature gvvf);

ajuint ensGvvariationfeatureGetMapWeight(const EnsPGvvariationfeature gvvf);

AjBool ensGvvariationfeatureSetAdaptor(EnsPGvvariationfeature gvvf,
                                       EnsPGvvariationfeatureadaptor gvvfa);

AjBool ensGvvariationfeatureSetIdentifier(EnsPGvvariationfeature gvvf,
                                          ajuint identifier);

AjBool ensGvvariationfeatureSetFeature(EnsPGvvariationfeature gvvf,
                                       EnsPFeature feature);

AjBool ensGvvariationfeatureSetGvvariation(EnsPGvvariationfeature gvvf,
                                           EnsPGvvariation gvv);

AjBool ensGvvariationfeatureSetName(EnsPGvvariationfeature gvvf, AjPStr name);

AjBool ensGvvariationfeatureSetSource(EnsPGvvariationfeature gvvf,
                                      AjPStr source);

AjBool ensGvvariationfeatureSetValidationCode(EnsPGvvariationfeature gvvf,
                                              AjPStr validationcode);

AjBool ensGvvariationfeatureSetConsequenceType(EnsPGvvariationfeature gvvf,
                                               AjPStr consequencetype);

AjBool ensGvvariationfeatureSetMapWeight(EnsPGvvariationfeature gvvf,
                                         ajuint mapweight);

ajuint ensGvvariationfeatureGetMemSize(const EnsPGvvariationfeature gvvf);

AjBool ensGvvariationfeatureTrace(const EnsPGvvariationfeature gvvf,
                                  ajuint level);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
