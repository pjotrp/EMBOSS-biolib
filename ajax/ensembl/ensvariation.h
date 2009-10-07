#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensvariation_h
#define ensvariation_h

#include "ensfeature.h"




/* @data EnsPGVSampleadaptor **************************************************
**
** Ensembl Genetic Variation Sample Adaptor.
**
** @alias EnsSGVSampleadaptor
** @alias EnsOGVSampleadaptor
**
** @attr Adaptor [EnsPBaseadaptor] Ensembl Base Adaptor
** @@
******************************************************************************/

typedef struct EnsSGVSampleadaptor
{
    EnsPBaseadaptor Adaptor;
} EnsOGVSampleadaptor;

#define EnsPGVSampleadaptor EnsOGVSampleadaptor*




/* @data EnsPGVSample *********************************************************
**
** Ensembl Genetic Variation Sample.
**
** @alias EnsSGVSample
** @alias EnsOGVSample
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGVSampleadaptor] Ensembl Genetic Variation
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

typedef struct EnsSGVSample
{
    ajuint Use;
    ajuint Identifier;
    EnsPGVSampleadaptor Adaptor;
    AjPStr Name;
    AjPStr Description;
    ajuint Size;
    ajuint Padding;
} EnsOGVSample;

#define EnsPGVSample EnsOGVSample*




/* @data EnsPGVIndividualadaptor **********************************************
**
** Ensembl Genetic Variation Individual Adaptor.
**
** @alias EnsSGVIndividualadaptor
** @alias EnsOGVIndividualadaptor
**
** @attr Adaptor [EnsPGVSampleadaptor] Ensembl Genetic Variation
**                                     Sample Adaptor
** @@
******************************************************************************/

typedef struct EnsSGVIndividualadaptor
{
    EnsPGVSampleadaptor Adaptor;
} EnsOGVIndividualadaptor;

#define EnsPGVIndividualadaptor EnsOGVIndividualadaptor*




/******************************************************************************
**
** Ensembl Genetic Variation Individual Gender enumeration.
**
******************************************************************************/

enum EnsEGVIndividualGender
{
    ensEGVIndividualGenderNULL,
    ensEGVIndividualGenderMale,
    ensEGVIndividualGenderFemale,
    ensEGVIndividualGenderUnknown
};




/******************************************************************************
**
** Ensembl Genetic Variation Individual Type enumeration.
**
******************************************************************************/

enum EnsEGVIndividualType
{
    ensEGVIndividualTypeNULL,
    ensEGVIndividualTypeFullyInbred,
    ensEGVIndividualTypePartlyInbred,
    ensEGVIndividualTypeOutbred,
    ensEGVIndividualTypeMutant,
    ensEGVIndividualTypeUnknown
};




/* @data EnsPGVIndividual *****************************************************
**
** Ensembl Genetic Variation Individual.
**
** @alias EnsSGVIndividual
** @alias EnsOGVIndividual
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGVIndividualadaptor] Ensembl Genetic Variation
**                                         Individual Adaptor
** @cc Bio::EnsEMBL::Variation::Sample
** @attr GVSample [EnsPGVSample] Sample
** @cc Bio::EnsEMBL::Variation::Individual
** @attr Father [struct EnsSGVIndividual*] Father Individual
** @attr Mother [struct EnsSGVIndividual*] Mother Individual
** @attr Gender [AjEnum] Gender
** @attr Type [AjEnum] Type
** @attr Description [AjPStr] Description
** @@
******************************************************************************/

typedef struct EnsSGVIndividual
{
    ajuint Use;
    ajuint Identifier;
    EnsPGVIndividualadaptor Adaptor;
    EnsPGVSample GVSample;
    struct EnsSGVIndividual* Father;
    struct EnsSGVIndividual* Mother;
    AjEnum Gender;
    AjEnum Type;
    AjPStr Description;
} EnsOGVIndividual;

#define EnsPGVIndividual EnsOGVIndividual*




/* @data EnsPGVPopulationadaptor **********************************************
**
** Ensembl Genetic Variation Population Adaptor.
**
** @alias EnsSGVPopulationadaptor
** @alias EnsOGVPopulationadaptor
**
** @attr Adaptor [EnsPGVSampleadaptor] Ensembl Genetic Variation
**                                     Sample Adaptor
** @@
******************************************************************************/

typedef struct EnsSGVPopulationadaptor
{
    EnsPGVSampleadaptor Adaptor;
} EnsOGVPopulationadaptor;

#define EnsPGVPopulationadaptor EnsOGVPopulationadaptor*




/* @data EnsPGVPopulation *****************************************************
**
** Ensembl Genetic Variation Population.
**
** @alias EnsSGVPopulation
** @alias EnsOGVPopulation
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGVPopulationadaptor] Ensembl Genetic Variation
**                                         Population Adaptor
** @cc Bio::EnsEMBL::Variation::Sample
** @attr GVSample [EnsPGVSample] Sample
** @cc Bio::EnsEMBL::Variation::Population
** @attr SubPopulations [AjPList] AJAX List of Ensembl Genetic Variation
**                                (Sub-) Populations
** @@
******************************************************************************/

typedef struct EnsSGVPopulation
{
    ajuint Use;
    ajuint Identifier;
    EnsPGVPopulationadaptor Adaptor;
    EnsPGVSample GVSample;
    AjPList SubPopulations;
} EnsOGVPopulation;

#define EnsPGVPopulation EnsOGVPopulation*




/* @data EnsPGVAlleleadaptor **************************************************
**
** Ensembl Genetic Variation Allele Adaptor.
**
** @alias EnsSGVAlleleadaptor
** @alias EnsOGVAlleleadaptor
**
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor.
** @@
** NOTE: Although the Bio::EnsEMBL::Variation::Allele object is based on the
** Bio::EnsEMBL::Storable object, there is actually no
** Bio::EnsEMBL::Variation::DBSQL::Alleleadaptor as such in the
** Ensembl Variation API.
******************************************************************************/

typedef struct EnsSGVAlleleadaptor
{
    EnsPDatabaseadaptor Adaptor;
} EnsOGVAlleleadaptor;

#define EnsPGVAlleleadaptor EnsOGVAlleleadaptor*




/* @data EnsPGVAllele *********************************************************
**
** Ensembl Genetic Variation Allele.
**
** @alias EnsSGVAllele
** @alias EnsOGVAllele
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGVAlleleadaptor] Ensembl Genetic Variation
**                                     Allele Adaptor
** @cc Bio::EnsEMBL::Variation::Allele
** @attr GVPopulation [EnsPGVPopulation] Ensembl Genetic Variation Population
** @attr Allele [AjPStr] Allele
** @attr Frequency [float] Frequency
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSGVAllele
{
    ajuint Use;
    ajuint Identifier;
    EnsPGVAlleleadaptor Adaptor;
    EnsPGVPopulation GVPopulation;
    AjPStr Allele;
    float Frequency;
    ajuint Padding;
} EnsOGVAllele;

#define EnsPGVAllele EnsOGVAllele*




/* @data EnsPGVGenotypeadaptor ************************************************
**
** Ensembl Genetic Variation Genotype Adaptor.
**
** @alias EnsSGVGenotypeadaptor
** @alias EnsOGVGenotypeadaptor
**
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct EnsSGVGenotypeadaptor
{
    EnsPDatabaseadaptor Adaptor;
} EnsOGVGenotypeadaptor;

#define EnsPGVGenotypeadaptor EnsOGVGenotypeadaptor*




/* @data EnsPGVGenotype *******************************************************
**
** Ensembl Genetic Variation Genotype.
**
** @alias EnsSGVGenotype
** @alias EnsOGVGenotype
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGVGenotypeadaptor] Ensembl Genetic Variation
**                                       Genotype Adaptor
** @cc Bio::EnsEMBL::Variation::Genotype
** @attr Allele1 [AjPStr] Allele 1
** @attr Allele2 [AjPStr] Allele 2
** @@
******************************************************************************/

typedef struct EnsSGVGenotype
{
    ajuint Use;
    ajuint Identifier;
    EnsPGVGenotypeadaptor Adaptor;
    AjPStr Allele1;
    AjPStr Allele2;
} EnsOGVGenotype;

#define EnsPGVGenotype EnsOGVGenotype*




/* @data EnsPGVVariationadaptor ***********************************************
**
** Ensembl Genetic Variation Variation Adaptor.
**
** @alias EnsSGVVariationadaptor
** @alias EnsOGVVariationadaptor
**
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct EnsSGVVariationadaptor
{
    EnsPDatabaseadaptor Adaptor;
} EnsOGVVariationadaptor;

#define EnsPGVVariationadaptor EnsOGVVariationadaptor*




/******************************************************************************
**
** Ensembl Genetic Variation Variation validation state enumeration.
**
******************************************************************************/

enum EnsEGVVariationValidationState
{
    ensEGVVariationValidationStateNULL,
    ensEGVVariationValidationStateCluster,
    ensEGVVariationValidationStateFrequency,
    ensEGVVariationValidationStateSubmitter,
    ensEGVVariationValidationStateDoublehit,
    ensEGVVariationValidationStateHapMap,
    ensEGVVariationValidationStateFailed,
    ensEGVVariationValidationStateNonPolymorphic,
    ensEGVVariationValidationStateObserved,
    ensEGVVariationValidationStateUnknown
};




/* @data EnsPGVVariation ******************************************************
**
** Ensembl Genetic Variation Variation.
**
** @alias EnsSGVVariation
** @alias EnsOGVVariation
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGVVariationadaptor] Ensembl Genetic Variation
**                                        Variation Adaptor
** @cc Bio::EnsEMBL::Variation::Variation
** @attr Name [AjPStr] Name
** @attr Source [AjPStr] Source
** @attr Synonyms [AjPTable] Synonyms
** @attr AncestralAllele [AjPStr] Ancestral allele
** @attr GVAlleles [AjPList] AJAX List of Ensembl Genetic Variation Alleles
** @attr ValidationStates [AjPList] AJAX List of AJAX Strings of validation
**                                  states
** @attr MoleculeType [AjPStr] Molecule type
** @attr FivePrimeFlank [AjPStr] Five prime flanking sequence
** @attr ThreePrimeFlank [AjPStr] Three prime flanking sequence
** @attr FailedDescription [AjPStr] Failed description
** @@
******************************************************************************/

typedef struct EnsSGVVariation
{
    ajuint Use;
    ajuint Identifier;
    EnsPGVVariationadaptor Adaptor;
    AjPStr Name;
    AjPStr Source;
    AjPTable Synonyms;
    AjPStr AncestralAllele;
    AjPList GVAlleles;
    AjPList ValidationStates;
    AjPStr MoleculeType;
    AjPStr FivePrimeFlank;
    AjPStr ThreePrimeFlank;
    AjPStr FailedDescription;
} EnsOGVVariation;

#define EnsPGVVariation EnsOGVVariation*




/* @data EnsPGVVariationFeatureadaptor ****************************************
**
** Ensembl Genetic Variation Variation Feature Adaptor.
**
** @alias EnsSGVVariationFeatureadaptor
** @alias EnsOGVVariationFeatureadaptor
**
** @attr Adaptor [EnsPFeatureadaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSGVVariationFeatureadaptor
{
    EnsPFeatureadaptor Adaptor;
} EnsOGVVariationFeatureadaptor;

#define EnsPGVVariationFeatureadaptor EnsOGVVariationFeatureadaptor*




/* @data EnsPGVVariationFeature ***********************************************
**
** Ensembl Genetic Variation Variation Feature.
**
** @alias EnsSGVVariationFeature
** @alias EnsOGVVariationFeature
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGVVariationFeatureadaptor] Ensembl Genetic Variation
**                                       Genotype Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Variation::VariationFeature
** @attr GVVariation [EnsPGVVariation] Ensembl Variation
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

typedef struct EnsSGVVariationFeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPGVVariationFeatureadaptor Adaptor;
    EnsPFeature Feature;
    EnsPGVVariation GVVariation;
    AjPStr Name;
    AjPStr Source;
    AjPStr ValidationCode;
    AjPStr ConsequenceType;
    ajuint MapWeight;
    ajuint Padding;
} EnsOGVVariationFeature;

#define EnsPGVVariationFeature EnsOGVVariationFeature*




/*
** Prototype definitions
*/

/* Ensembl Genetic Variation Sample */

EnsPGVSample ensGVSampleNew(EnsPGVSampleadaptor gvsa,
                            ajuint identifier,
                            AjPStr name,
                            AjPStr description,
                            ajuint size);

EnsPGVSample ensGVSampleNewObj(const EnsPGVSample object);

EnsPGVSample ensGVSampleNewRef(EnsPGVSample gvs);

void ensGVSampleDel(EnsPGVSample* Pgvs);

EnsPGVSampleadaptor ensGVSampleGetadaptor(const EnsPGVSample gvs);

ajuint ensGVSampleGetIdentifier(const EnsPGVSample gvs);

AjPStr ensGVSampleGetName(const EnsPGVSample gvs);

AjPStr ensGVSampleGetDescription(const EnsPGVSample gvs);

ajuint ensGVSampleGetSize(const EnsPGVSample gvs);

AjBool ensGVSampleSetadaptor(EnsPGVSample gvs, EnsPGVSampleadaptor adaptor);

AjBool ensGVSampleSetIdentifier(EnsPGVSample gvs, ajuint identifier);

AjBool ensGVSampleSetName(EnsPGVSample gvs, AjPStr name);

AjBool ensGVSampleSetDescription(EnsPGVSample gvs, AjPStr description);

AjBool ensGVSampleSetSize(EnsPGVSample gvs, ajuint size);

ajuint ensGVSampleGetMemSize(const EnsPGVSample gvs);

AjBool ensGVSampleTrace(const EnsPGVSample gvs, ajuint level);

/* Ensembl Genetic Variation Sample Adaptor */

EnsPGVSampleadaptor ensGVSampleadaptorNew(
    EnsPDatabaseadaptor dba,
    const char **Ptables,
    const char **Pcolumns,
    EnsOBaseadaptorLeftJoin *left_join,
    const char *condition,
    const char *final,
    AjBool Fquery(EnsPDatabaseadaptor dba,
                  const AjPStr statement,
                  EnsPAssemblyMapper am,
                  EnsPSlice slice,
                  AjPList objects));

void ensGVSampleadaptorDel(EnsPGVSampleadaptor *Pgvsa);

EnsPBaseadaptor ensGVSampleadaptorGetadaptor(const EnsPGVSampleadaptor gvsa);

AjBool ensGVSampleadaptorFetchAllSynonymsByIdentifier(
    EnsPGVSampleadaptor gvsa,
    ajuint identifier,
    const AjPStr source,
    AjPList synonyms);

AjBool ensGVSampleadaptorFetchAllIdentifiersBySynonym(
    EnsPGVSampleadaptor gvsa,
    const AjPStr synonym,
    const AjPStr source,
    AjPList idlist);

AjBool ensGVSampleadaptorFetchByIdentifier(EnsPGVSampleadaptor gvsa,
                                           ajuint identifier,
                                           EnsPGVSample *Pgvs);

/* Ensembl Genetic Variation Individual */

EnsPGVIndividual ensGVIndividualNew(EnsPGVIndividualadaptor gvia,
                                    ajuint identifier,
                                    EnsPGVSample gvs,
                                    EnsPGVIndividual father,
                                    EnsPGVIndividual mother,
                                    AjEnum gender,
                                    AjEnum type,
                                    AjPStr description);

EnsPGVIndividual ensGVIndividualNewObj(const EnsPGVIndividual object);

EnsPGVIndividual ensGVIndividualNewRef(EnsPGVIndividual gvi);

void ensGVIndividualDel(EnsPGVIndividual* Pgvi);

EnsPGVIndividualadaptor ensGVIndividualGetadaptor(const EnsPGVIndividual gvi);

ajuint ensGVIndividualGetIdentifier(const EnsPGVIndividual gvi);

EnsPGVSample ensGVIndividualGetGVSample(const EnsPGVIndividual gvi);

EnsPGVIndividual ensGVIndividualGetFather(const EnsPGVIndividual gvi);

EnsPGVIndividual ensGVIndividualGetMother(const EnsPGVIndividual gvi);

AjEnum ensGVIndividualGetGender(const EnsPGVIndividual gvi);

AjEnum ensGVIndividualGetType(const EnsPGVIndividual gvi);

AjPStr ensGVIndividualGetDescription(const EnsPGVIndividual gvi);

AjBool ensGVIndividualSetadaptor(EnsPGVIndividual gvi,
                                 EnsPGVIndividualadaptor gvia);

AjBool ensGVIndividualSetIdentifier(EnsPGVIndividual gvi, ajuint identifier);

AjBool ensGVIndividualSetGVSample(EnsPGVIndividual gvi, EnsPGVSample gvs);

AjBool ensGVIndividualSetFather(EnsPGVIndividual gvi, EnsPGVIndividual father);

AjBool ensGVIndividualSetMother(EnsPGVIndividual gvi, EnsPGVIndividual mother);

AjBool ensGVIndividualSetGender(EnsPGVIndividual gvi, AjEnum gender);

AjBool ensGVIndividualSetType(EnsPGVIndividual gvi, AjEnum type);

AjBool ensGVIndividualSetDescription(EnsPGVIndividual gvi, AjPStr description);

AjEnum ensGVIndividualGenderFromStr(const AjPStr gender);

AjEnum ensGVIndividualTypeFromStr(const AjPStr type);

const char *ensGVIndividualGenderToChar(const AjEnum gender);

const char *ensGVIndividualTypeToChar(const AjEnum type);

ajuint ensGVIndividualGetMemSize(const EnsPGVIndividual gvi);

AjBool ensGVIndividualTrace(const EnsPGVIndividual gvi, ajuint level);

/* Ensembl Genetic Variation Individual Adaptor */

EnsPGVIndividualadaptor ensGVIndividualadaptorNew(EnsPDatabaseadaptor dba);

void ensGVIndividualadaptorDel(EnsPGVIndividualadaptor *Pgvia);

EnsPGVSampleadaptor ensGVIndividualadaptorGetadaptor(
    const EnsPGVIndividualadaptor gvia);

AjBool ensGVIndividualadaptorFetchByIdentifier(EnsPGVIndividualadaptor gvia,
                                               ajuint identifier,
                                               EnsPGVIndividual *Pgvi);

AjBool ensGVIndividualadaptorFetchAllByName(EnsPGVIndividualadaptor gvia,
                                            const AjPStr name,
                                            AjPList gvis);

AjBool ensGVIndividualadaptorFetchAllBySynonym(EnsPGVIndividualadaptor adaptor,
                                               const AjPStr synonym,
                                               AjPList individuals);

AjBool ensGVIndividualadaptorFetchAllByPopulation(EnsPGVIndividualadaptor gvia,
                                                  const EnsPGVPopulation gvp,
                                                  AjPList gvis);

AjBool ensGVIndividualadaptorFetchAllByParentIndividual(
    EnsPGVIndividualadaptor gvia,
    const EnsPGVIndividual parent,
    AjPList gvis);

AjBool ensGVIndividualadaptorFetchAllStrains(EnsPGVIndividualadaptor gvia,
                                             AjPList gvis);

AjBool ensGVIndividualadaptorFetchReferenceStrainName(
    EnsPGVIndividualadaptor gvia,
    AjPStr *Pname);

AjBool ensGVIndividualadaptorFetchAllDefaultStrains(
    EnsPGVIndividualadaptor gvia,
    AjPList names);

AjBool ensGVIndividualadaptorFetchAllDisplayStrains(
    EnsPGVIndividualadaptor gvia,
    AjPList individuals);

AjBool ensGVIndividualadaptorFetchAllStrainsWithCoverage(
    EnsPGVIndividualadaptor gvia,
    AjPList idlist);

/* Ensembl Genetic Variation Population */

EnsPGVPopulation ensGVPopulationNew(EnsPGVPopulationadaptor gvpa,
                                    ajuint identifier,
                                    EnsPGVSample gvs,
                                    AjPList subpopulations);

EnsPGVPopulation ensGVPopulationNewObj(const EnsPGVPopulation object);

EnsPGVPopulation ensGVPopulationNewRef(EnsPGVPopulation gvp);

void ensGVPopulationDel(EnsPGVPopulation* Pgvp);

EnsPGVPopulationadaptor ensGVPopulationGetadaptor(const EnsPGVPopulation gvp);

ajuint ensGVPopulationGetIdentifier(const EnsPGVPopulation gvp);

EnsPGVSample ensGVPopulationGetGVSample(const EnsPGVPopulation gvp);

const AjPList ensGVPopulationGetSubPopulations(const EnsPGVPopulation gvp);

AjBool ensGVPopulationSetadaptor(EnsPGVPopulation gvp,
                                 EnsPGVPopulationadaptor gvpa);

AjBool ensGVPopulationSetIdentifier(EnsPGVPopulation gvp, ajuint identifier);

AjBool ensGVPopulationSetGVSample(EnsPGVPopulation gvp, EnsPGVSample gvs);

AjBool ensGVPopulationAddSubPopulation(EnsPGVPopulation gvp,
                                       EnsPGVPopulation subgvp);

ajuint ensGVPopulationGetMemSize(const EnsPGVPopulation gvp);

AjBool ensGVPopulationTrace(const EnsPGVPopulation gvp, ajuint level);

/* Ensembl Genetic Variation Population Adaptor */

EnsPGVPopulationadaptor ensGVPopulationadaptorNew(EnsPDatabaseadaptor dba);

void ensGVPopulationadaptorDel(EnsPGVPopulationadaptor *Pgvpa);

EnsPGVSampleadaptor ensGVPopulationadaptorGetadaptor(
    const EnsPGVPopulationadaptor gvpa);

AjBool ensGVPopulationadaptorFetchByIdentifier(EnsPGVPopulationadaptor gvpa,
                                               ajuint identifier,
                                               EnsPGVPopulation *Pgvp);

AjBool ensGVPopulationadaptorFetchByName(EnsPGVPopulationadaptor gvpa,
                                         const AjPStr name,
                                         EnsPGVPopulation* Pgvp);

AjBool ensGVPopulationadaptorFetchAllBySynonym(EnsPGVPopulationadaptor gvpa,
                                               const AjPStr synonym,
                                               AjPList gvps);

AjBool ensGVPopulationadaptorFetchAllBySuperPopulation(
    EnsPGVPopulationadaptor gvpa,
    const EnsPGVPopulation gvp,
    AjPList gvps);

AjBool ensGVPopulationadaptorFetchAllBySubPopulation(
    EnsPGVPopulationadaptor gvpa,
    const EnsPGVPopulation gvp,
    AjPList gvps);

AjBool ensGVPopulationadaptorFetchAllByIndvividual(
    EnsPGVPopulationadaptor gvpa,
    EnsPGVIndividual gvi,
    AjPList gvps);

/* Ensembl Genetic Variation Allele */

EnsPGVAllele ensGVAlleleNew(EnsPGVAlleleadaptor gvaa,
                            ajuint identifier,
                            EnsPGVPopulation gvp,
                            AjPStr allele,
                            float frequency);

EnsPGVAllele ensGVAlleleNewObj(const EnsPGVAllele object);

EnsPGVAllele ensGVAlleleNewRef(EnsPGVAllele gva);

void ensGVAlleleDel(EnsPGVAllele* Pgva);

EnsPGVAlleleadaptor ensGVAlleleGetadaptor(const EnsPGVAllele gva);

ajuint ensGVAlleleGetIdentifier(const EnsPGVAllele gva);

EnsPGVPopulation ensGVAlleleGetPopulation(const EnsPGVAllele gva);

AjPStr ensGVAlleleGetAllele(const EnsPGVAllele gva);

float ensGVAlleleGetFrequency(const EnsPGVAllele gva);

AjBool ensGVAlleleSetadaptor(EnsPGVAllele gva, EnsPGVAlleleadaptor gvaa);

AjBool ensGVAlleleSetIdentifier(EnsPGVAllele gva, ajuint identifier);

AjBool ensGVAlleleSetPopulation(EnsPGVAllele gva, EnsPGVPopulation gvp);

AjBool ensGVAlleleSetAllele(EnsPGVAllele gva, AjPStr allelestr);

AjBool ensGVAlleleSetFrequency(EnsPGVAllele gva, float frequency);

ajuint ensGVAlleleGetMemSize(const EnsPGVAllele gva);

AjBool ensGVAlleleTrace(const EnsPGVAllele gva, ajuint level);

/* Ensembl Genetic Variation Genotype */

EnsPGVGenotype ensGVGenotypeNew(EnsPGVGenotypeadaptor gvga,
                                ajuint identifier,
                                AjPStr allele1,
                                AjPStr allele2);

EnsPGVGenotype ensGVGenotypeNewObj(const EnsPGVGenotype object);

EnsPGVGenotype ensGVGenotypeNewRef(EnsPGVGenotype gvg);

void ensGVGenotypeDel(EnsPGVGenotype* Pgvg);

EnsPGVGenotypeadaptor ensGVGenotypeGetadaptor(const EnsPGVGenotype gvg);

ajuint ensGVGenotypeGetIdentifier(const EnsPGVGenotype gvg);

AjPStr ensGVGenotypeGetAllele1(const EnsPGVGenotype gvg);

AjPStr ensGVGenotypeGetAllele2(const EnsPGVGenotype gvg);

AjBool ensGVGenotypeSetadaptor(EnsPGVGenotype gvg, EnsPGVGenotypeadaptor gvga);

AjBool ensGVGenotypeSetIdentifier(EnsPGVGenotype gvg, ajuint identifier);

AjBool ensGVGenotypeSetAllele1(EnsPGVGenotype gvg, AjPStr allele1);

AjBool ensGVGenotypeSetAllele2(EnsPGVGenotype gvg, AjPStr allele2);

ajuint ensGVGenotypeGetMemSize(const EnsPGVGenotype gvg);

AjBool ensGVGenotypeTrace(const EnsPGVGenotype gvg, ajuint level);

/* Ensembl Genetic Variation Variation */

EnsPGVVariation ensGVVariationNew(EnsPGVVariationadaptor adaptor,
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

EnsPGVVariation ensGVVariationNewObj(const EnsPGVVariation object);

EnsPGVVariation ensGVVariationNewRef(EnsPGVVariation gvv);

void ensGVVariationDel(EnsPGVVariation* Pgvv);

EnsPGVVariationadaptor ensGVVariationGetadaptor(const EnsPGVVariation gvv);

ajuint ensGVVariationGetIdentifier(const EnsPGVVariation gvv);

AjPStr ensGVVariationGetName(const EnsPGVVariation gvv);

AjPStr ensGVVariationGetSource(const EnsPGVVariation gvv);

const AjPTable ensGVVariationGetSynonyms(const EnsPGVVariation gvv);

AjPStr ensGVVariationGetAncestralAllele(const EnsPGVVariation gvv);

const AjPList ensGVVariationGetGVAlleles(const EnsPGVVariation gvv);

const AjPList ensGVVariationGetValidationStates(const EnsPGVVariation gvv);

AjPStr ensGVVariationGetMoleculeType(const EnsPGVVariation gvv);

AjPStr ensGVVariationGetFivePrimeFlank(const EnsPGVVariation gvv);

AjPStr ensGVVariationGetThreePrimeFlank(const EnsPGVVariation gvv);

AjPStr ensGVVariationGetFailedDescription(const EnsPGVVariation gvv);

AjBool ensGVVariationSetadaptor(EnsPGVVariation gvv,
                                EnsPGVVariationadaptor gvva);

AjBool ensGVVariationSetIdentifier(EnsPGVVariation gvv, ajuint identifier);

AjBool ensGVVariationSetName(EnsPGVVariation gvv, AjPStr name);

AjBool ensGVVariationSetSource(EnsPGVVariation gvv, AjPStr source);

AjBool ensGVVariationSetMoleculeType(EnsPGVVariation gvv, AjPStr moltype);

AjBool ensGVVariationSetAncestralAllele(EnsPGVVariation gvv,
                                        AjPStr ancestralallele);

AjBool ensGVVariationSetFivePrimeFlank(EnsPGVVariation gvv, AjPStr fiveflank);

AjBool ensGVVariationSetThreePrimeFlank(EnsPGVVariation gvv, AjPStr threeflank);

AjBool ensGVVariationSetFailedDescription(EnsPGVVariation gvv,
                                          AjPStr faileddescription);

ajuint ensGVVariationGetMemSize(const EnsPGVVariation gvv);

AjBool ensGVVariationTrace(const EnsPGVVariation gvv, ajuint level);

AjBool ensGVVariationAddSynonym(EnsPGVVariation gvv, AjPStr source,
                                AjPStr synonym);

AjBool ensGVVariationAddGVAllele(EnsPGVVariation gvv, EnsPGVAllele gva);

AjEnum ensGVVariationValidationStateFromStr(const AjPStr state);

const char *ensGVVariationValidationStateToChar(const AjEnum state);

AjBool ensGVVariationFetchAllSynonyms(EnsPGVVariation gvv,
                                      const AjPStr source,
                                      AjPList synonyms);

AjBool ensGVVariationFetchAllSynonymSources(EnsPGVVariation gvv,
                                            AjPList sources);

/* Ensembl Genetic Variation Variation Adaptor */

EnsPGVVariationadaptor ensGVVariationadaptorNew(EnsPDatabaseadaptor dba);

void ensGVVariationadaptorDel(EnsPGVVariationadaptor *Pgvva);

AjBool ensGVVariationadaptorFetchByIdentifier(EnsPGVVariationadaptor gvva,
                                              ajuint identifier,
                                              EnsPGVVariation *Pgvv);

AjBool ensGVVariationadaptorFetchByName(EnsPGVVariationadaptor gvva,
                                        const AjPStr name,
                                        const AjPStr source,
                                        EnsPGVVariation *Pgvv);

AjBool ensGVVariationadaptorFetchAllBySource(EnsPGVVariationadaptor gvva,
                                             const AjPStr source,
                                             AjBool primary,
                                             AjPList gvvs);

/* Ensembl Genetic Variation Variation Feature */

EnsPGVVariationFeature ensGVVariationFeatureNew(
    EnsPGVVariationFeatureadaptor adaptor,
    ajuint identifier,
    EnsPFeature feature,
    EnsPGVVariation gvv,
    AjPStr name,
    AjPStr source,
    AjPStr validation,
    AjPStr consequence,
    ajuint mapweight);

EnsPGVVariationFeature ensGVVariationFeatureNewObj(
    const EnsPGVVariationFeature object);

EnsPGVVariationFeature ensGVVariationFeatureNewRef(EnsPGVVariationFeature gvvf);

void ensGVVariationFeatureDel(EnsPGVVariationFeature *Pgvvf);

EnsPGVVariationFeatureadaptor ensGVVariationFeatureGetadaptor(
    const EnsPGVVariationFeature gvvf);

ajuint ensGVVariationFeatureGetIdentifier(const EnsPGVVariationFeature gvvf);

EnsPFeature ensGVVariationFeatureGetFeature(const EnsPGVVariationFeature gvvf);

EnsPGVVariation ensGVVariationFeatureGetGVVariation(
    const EnsPGVVariationFeature gvvf);

AjPStr ensGVVariationFeatureGetName(const EnsPGVVariationFeature gvvf);

AjPStr ensGVVariationFeatureGetSource(const EnsPGVVariationFeature gvvf);

AjPStr ensGVVariationFeatureGetValidationCode(
    const EnsPGVVariationFeature gvvf);

AjPStr ensGVVariationFeatureGetConsequenceType(
    const EnsPGVVariationFeature gvvf);

ajuint ensGVVariationFeatureGetMapWeight(const EnsPGVVariationFeature gvvf);

AjBool ensGVVariationFeatureSetadaptor(EnsPGVVariationFeature gvvf,
                                       EnsPGVVariationFeatureadaptor gvvfa);

AjBool ensGVVariationFeatureSetIdentifier(EnsPGVVariationFeature gvvf,
                                          ajuint identifier);

AjBool ensGVVariationFeatureSetFeature(EnsPGVVariationFeature gvvf,
                                       EnsPFeature feature);

AjBool ensGVVariationFeatureSetGVVariation(EnsPGVVariationFeature gvvf,
                                           EnsPGVVariation gvv);

AjBool ensGVVariationFeatureSetName(EnsPGVVariationFeature gvvf, AjPStr name);

AjBool ensGVVariationFeatureSetSource(EnsPGVVariationFeature gvvf,
                                      AjPStr source);

AjBool ensGVVariationFeatureSetValidationCode(EnsPGVVariationFeature gvvf,
                                              AjPStr validationcode);

AjBool ensGVVariationFeatureSetConsequenceType(EnsPGVVariationFeature gvvf,
                                               AjPStr consequencetype);

AjBool ensGVVariationFeatureSetMapWeight(EnsPGVVariationFeature gvvf,
                                         ajuint mapweight);

ajuint ensGVVariationFeatureGetMemSize(const EnsPGVVariationFeature gvvf);

AjBool ensGVVariationFeatureTrace(const EnsPGVVariationFeature gvvf,
                                  ajuint level);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
