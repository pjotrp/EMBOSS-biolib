#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensvariation_h
#define ensvariation_h

#include "ensfeature.h"




/* @data EnsPGVSampleAdaptor **************************************************
**
** Ensembl Genetic Variation Sample Adaptor.
**
** @alias EnsSGVSampleAdaptor
** @alias EnsOGVSampleAdaptor
**
** @attr Adaptor [EnsPBaseAdaptor] Ensembl Base Adaptor
** @@
******************************************************************************/

typedef struct EnsSGVSampleAdaptor
{
    EnsPBaseAdaptor Adaptor;
} EnsOGVSampleAdaptor;

#define EnsPGVSampleAdaptor EnsOGVSampleAdaptor*




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
** @attr Adaptor [EnsPGVSampleAdaptor] Ensembl Genetic Variation
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
    EnsPGVSampleAdaptor Adaptor;
    AjPStr Name;
    AjPStr Description;
    ajuint Size;
    ajuint Padding;
} EnsOGVSample;

#define EnsPGVSample EnsOGVSample*




/* @data EnsPGVIndividualAdaptor **********************************************
**
** Ensembl Genetic Variation Individual Adaptor.
**
** @alias EnsSGVIndividualAdaptor
** @alias EnsOGVIndividualAdaptor
**
** @attr Adaptor [EnsPGVSampleAdaptor] Ensembl Genetic Variation
**                                     Sample Adaptor
** @@
******************************************************************************/

typedef struct EnsSGVIndividualAdaptor
{
    EnsPGVSampleAdaptor Adaptor;
} EnsOGVIndividualAdaptor;

#define EnsPGVIndividualAdaptor EnsOGVIndividualAdaptor*




/* @datatype EnsEGVIndividualGender *******************************************
**
** Ensembl Genetic Variation Individual Gender enumeration.
**
** @attr enum [EnsEGVIndividualGender] Value
** @@
******************************************************************************/

enum EnsEGVIndividualGender
{
    ensEGVIndividualGenderNULL,
    ensEGVIndividualGenderMale,
    ensEGVIndividualGenderFemale,
    ensEGVIndividualGenderUnknown
};




/* @datatype EnsEGVIndividualType *********************************************
**
** Ensembl Genetic Variation Individual Type enumeration.
**
** @attr enum [EnsEGVIndividualType] Value
** @@
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
** @attr Adaptor [EnsPGVIndividualAdaptor] Ensembl Genetic Variation
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
    EnsPGVIndividualAdaptor Adaptor;
    EnsPGVSample GVSample;
    struct EnsSGVIndividual* Father;
    struct EnsSGVIndividual* Mother;
    AjEnum Gender;
    AjEnum Type;
    AjPStr Description;
} EnsOGVIndividual;

#define EnsPGVIndividual EnsOGVIndividual*




/* @data EnsPGVPopulationAdaptor **********************************************
**
** Ensembl Genetic Variation Population Adaptor.
**
** @alias EnsSGVPopulationAdaptor
** @alias EnsOGVPopulationAdaptor
**
** @attr Adaptor [EnsPGVSampleAdaptor] Ensembl Genetic Variation
**                                     Sample Adaptor
** @@
******************************************************************************/

typedef struct EnsSGVPopulationAdaptor
{
    EnsPGVSampleAdaptor Adaptor;
} EnsOGVPopulationAdaptor;

#define EnsPGVPopulationAdaptor EnsOGVPopulationAdaptor*




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
** @attr Adaptor [EnsPGVPopulationAdaptor] Ensembl Genetic Variation
**                                         Population Adaptor
** @cc Bio::EnsEMBL::Variation::Sample
** @attr Sample [EnsPGVSample] Sample
** @cc Bio::EnsEMBL::Variation::Population
** @attr SubPopulations [AjPList] AJAX List of Ensembl Genetic Variation
**                                (Sub-) Populations
** @@
******************************************************************************/

typedef struct EnsSGVPopulation
{
    ajuint Use;
    ajuint Identifier;
    EnsPGVPopulationAdaptor Adaptor;
    EnsPGVSample GVSample;
    AjPList SubPopulations;
} EnsOGVPopulation;

#define EnsPGVPopulation EnsOGVPopulation*




/* @data EnsPGVAlleleAdaptor **************************************************
**
** Ensembl Genetic Variation Allele Adaptor.
**
** @alias EnsSGVAlleleAdaptor
** @alias EnsOGVAlleleAdaptor
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor.
** @@
** NOTE: Although the Bio::EnsEMBL::Variation::Allele object is based on the
** Bio::EnsEMBL::Storable object, there is actually no
** Bio::EnsEMBL::Variation::DBSQL::AlleleAdaptor as such in the
** Ensembl Variation API.
******************************************************************************/

typedef struct EnsSGVAlleleAdaptor
{
    EnsPDatabaseAdaptor Adaptor;
} EnsOGVAlleleAdaptor;

#define EnsPGVAlleleAdaptor EnsOGVAlleleAdaptor*




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
** @attr Adaptor [EnsPGVAlleleAdaptor] Ensembl Genetic Variation
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
    EnsPGVAlleleAdaptor Adaptor;
    EnsPGVPopulation GVPopulation;
    AjPStr Allele;
    float Frequency;
    ajuint Padding;
} EnsOGVAllele;

#define EnsPGVAllele EnsOGVAllele*




/* @data EnsPGVGenotypeAdaptor ************************************************
**
** Ensembl Genetic Variation Genotype Adaptor.
**
** @alias EnsSGVGenotypeAdaptor
** @alias EnsOGVGenotypeAdaptor
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct EnsSGVGenotypeAdaptor
{
    EnsPDatabaseAdaptor Adaptor;
} EnsOGVGenotypeAdaptor;

#define EnsPGVGenotypeAdaptor EnsOGVGenotypeAdaptor*




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
** @attr Adaptor [EnsPGVGenotypeAdaptor] Ensembl Genetic Variation
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
    EnsPGVGenotypeAdaptor Adaptor;
    AjPStr Allele1;
    AjPStr Allele2;
} EnsOGVGenotype;

#define EnsPGVGenotype EnsOGVGenotype*




/* @data EnsPGVVariationAdaptor ***********************************************
**
** Ensembl Genetic Variation Variation Adaptor.
**
** @alias EnsSGVVariationAdaptor
** @alias EnsOGVVariationAdaptor
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @@
******************************************************************************/

typedef struct EnsSGVVariationAdaptor
{
    EnsPDatabaseAdaptor Adaptor;
} EnsOGVVariationAdaptor;

#define EnsPGVVariationAdaptor EnsOGVVariationAdaptor*




/* @datatype EnsEGVVariationValidationState ***********************************
**
** Ensembl Genetic Variation Variation validation state enumeration.
**
** @attr enum [EnsEGVVariationValidationState] Value
** @@
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
** @attr Adaptor [EnsPGVVariationAdaptor] Ensembl Genetic Variation
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
    EnsPGVVariationAdaptor Adaptor;
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




/* @data EnsPGVVariationFeatureAdaptor ****************************************
**
** Ensembl Genetic Variation Variation Feature Adaptor.
**
** @alias EnsSGVVariationFeatureAdaptor
** @alias EnsOGVVariationFeatureAdaptor
**
** @attr Adaptor [EnsPFeatureAdaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSGVVariationFeatureAdaptor
{
    EnsPFeatureAdaptor Adaptor;
} EnsOGVVariationFeatureAdaptor;

#define EnsPGVVariationFeatureAdaptor EnsOGVVariationFeatureAdaptor*




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
** @attr Adaptor [EnsPGVGenotypeAdaptor] Ensembl Genetic Variation
**                                       Genotype Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Variation::VariationFeature
** @attr GVVariation [EnsPVariation] Ensembl Variation
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
    EnsPGVVariationFeatureAdaptor Adaptor;
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

EnsPGVSample ensGVSampleNew(EnsPGVSampleAdaptor gvsa,
                            ajuint identifier,
                            AjPStr name,
                            AjPStr description,
                            ajuint size);

EnsPGVSample ensGVSampleNewObj(const EnsPGVSample object);

EnsPGVSample ensGVSampleNewRef(EnsPGVSample gvs);

void ensGVSampleDel(EnsPGVSample* Pgvs);

EnsPGVSampleAdaptor ensGVSampleGetAdaptor(const EnsPGVSample gvs);

ajuint ensGVSampleGetIdentifier(const EnsPGVSample gvs);

AjPStr ensGVSampleGetName(const EnsPGVSample gvs);

AjPStr ensGVSampleGetDescription(const EnsPGVSample gvs);

ajuint ensGVSampleGetSize(const EnsPGVSample gvs);

AjBool ensGVSampleSetAdaptor(EnsPGVSample gvs, EnsPGVSampleAdaptor adaptor);

AjBool ensGVSampleSetIdentifier(EnsPGVSample gvs, ajuint identifier);

AjBool ensGVSampleSetName(EnsPGVSample gvs, AjPStr name);

AjBool ensGVSampleSetDescription(EnsPGVSample gvs, AjPStr description);

AjBool ensGVSampleSetSize(EnsPGVSample gvs, ajuint size);

ajuint ensGVSampleGetMemSize(const EnsPGVSample gvs);

AjBool ensGVSampleTrace(const EnsPGVSample gvs, ajuint level);

/* Ensembl Genetic Variation Sample Adaptor */

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
                  AjPList objects));

void ensGVSampleAdaptorDel(EnsPGVSampleAdaptor *Pgvsa);

EnsPBaseAdaptor ensGVSampleAdaptorGetAdaptor(const EnsPGVSampleAdaptor gvsa);

AjBool ensGVSampleAdaptorFetchAllSynonymsByIdentifier(
    EnsPGVSampleAdaptor gvsa,
    ajuint identifier,
    const AjPStr source,
    AjPList synonyms);

AjBool ensGVSampleAdaptorFetchAllIdentifiersBySynonym(
    EnsPGVSampleAdaptor gvsa,
    const AjPStr synonym,
    const AjPStr source,
    AjPList idlist);

AjBool ensGVSampleAdaptorFetchByIdentifier(EnsPGVSampleAdaptor gvsa,
                                           ajuint identifier,
                                           EnsPGVSample *Pgvs);

/* Ensembl Genetic Variation Individual */

EnsPGVIndividual ensGVIndividualNew(EnsPGVIndividualAdaptor gvia,
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

EnsPGVIndividualAdaptor ensGVIndividualGetAdaptor(const EnsPGVIndividual gvi);

ajuint ensGVIndividualGetIdentifier(const EnsPGVIndividual gvi);

EnsPGVSample ensGVIndividualGetGVSample(const EnsPGVIndividual gvi);

EnsPGVIndividual ensGVIndividualGetFather(const EnsPGVIndividual gvi);

EnsPGVIndividual ensGVIndividualGetMother(const EnsPGVIndividual gvi);

AjEnum ensGVIndividualGetGender(const EnsPGVIndividual gvi);

AjEnum ensGVIndividualGetType(const EnsPGVIndividual gvi);

AjPStr ensGVIndividualGetDescription(const EnsPGVIndividual gvi);

AjBool ensGVIndividualSetAdaptor(EnsPGVIndividual gvi,
                                 EnsPGVIndividualAdaptor gvia);

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

EnsPGVIndividualAdaptor ensGVIndividualAdaptorNew(EnsPDatabaseAdaptor dba);

void ensGVIndividualAdaptorDel(EnsPGVIndividualAdaptor *Pgvia);

EnsPGVSampleAdaptor ensGVIndividualAdaptorGetAdaptor(
    const EnsPGVIndividualAdaptor gvia);

AjBool ensGVIndividualAdaptorFetchByIdentifier(EnsPGVIndividualAdaptor gvia,
                                               ajuint identifier,
                                               EnsPGVIndividual *Pgvi);

AjBool ensGVIndividualAdaptorFetchAllByName(EnsPGVIndividualAdaptor gvia,
                                            const AjPStr name,
                                            AjPList gvis);

AjBool ensGVIndividualAdaptorFetchAllBySynonym(EnsPGVIndividualAdaptor adaptor,
                                               const AjPStr synonym,
                                               AjPList individuals);

AjBool ensGVIndividualAdaptorFetchAllByPopulation(EnsPGVIndividualAdaptor gvia,
                                                  const EnsPGVPopulation gvp,
                                                  AjPList gvis);

AjBool ensGVIndividualAdaptorFetchAllByParentIndividual(
    EnsPGVIndividualAdaptor gvia,
    const EnsPGVIndividual parent,
    AjPList gvis);

AjBool ensGVIndividualAdaptorFetchAllStrains(EnsPGVIndividualAdaptor gvia,
                                             AjPList gvis);

AjBool ensGVIndividualAdaptorFetchReferenceStrainName(
    EnsPGVIndividualAdaptor gvia,
    AjPStr *Pname);

AjBool ensGVIndividualAdaptorFetchAllDefaultStrains(
    EnsPGVIndividualAdaptor gvia,
    AjPList names);

AjBool ensGVIndividualAdaptorFetchAllDisplayStrains(
    EnsPGVIndividualAdaptor gvia,
    AjPList individuals);

AjBool ensGVIndividualAdaptorFetchAllStrainsWithCoverage(
    EnsPGVIndividualAdaptor gvia,
    AjPList idlist);

/* Ensembl Genetic Variation Population */

EnsPGVPopulation ensGVPopulationNew(EnsPGVPopulationAdaptor gvpa,
                                    ajuint identifier,
                                    EnsPGVSample gvs,
                                    AjPList subpopulations);

EnsPGVPopulation ensGVPopulationNewObj(const EnsPGVPopulation object);

EnsPGVPopulation ensGVPopulationNewRef(EnsPGVPopulation gvp);

void ensGVPopulationDel(EnsPGVPopulation* Pgvp);

EnsPGVPopulationAdaptor ensGVPopulationGetAdaptor(const EnsPGVPopulation gvp);

ajuint ensGVPopulationGetIdentifier(const EnsPGVPopulation gvp);

EnsPGVSample ensGVPopulationGetGVSample(const EnsPGVPopulation gvp);

const AjPList ensGVPopulationGetSubPopulations(const EnsPGVPopulation gvp);

AjBool ensGVPopulationSetAdaptor(EnsPGVPopulation gvp,
                                 EnsPGVPopulationAdaptor gvpa);

AjBool ensGVPopulationSetIdentifier(EnsPGVPopulation gvp, ajuint identifier);

AjBool ensGVPopulationSetGVSample(EnsPGVPopulation gvp, EnsPGVSample gvs);

AjBool ensGVPopulationAddSubPopulation(EnsPGVPopulation gvp,
                                       EnsPGVPopulation subgvp);

ajuint ensGVPopulationGetMemSize(const EnsPGVPopulation gvp);

AjBool ensGVPopulationTrace(const EnsPGVPopulation gvp, ajuint level);

/* Ensembl Genetic Variation Population Adaptor */

EnsPGVPopulationAdaptor ensGVPopulationAdaptorNew(EnsPDatabaseAdaptor dba);

void ensGVPopulationAdaptorDel(EnsPGVPopulationAdaptor *Pgvpa);

EnsPGVSampleAdaptor ensGVPopulationAdaptorGetAdaptor(
    const EnsPGVPopulationAdaptor gvpa);

AjBool ensGVPopulationAdaptorFetchByIdentifier(EnsPGVPopulationAdaptor gvpa,
                                               ajuint identifier,
                                               EnsPGVPopulation *Pgvp);

AjBool ensGVPopulationAdaptorFetchByName(EnsPGVPopulationAdaptor gvpa,
                                         const AjPStr name,
                                         EnsPGVPopulation* Pgvp);

AjBool ensGVPopulationAdaptorFetchAllBySynonym(EnsPGVPopulationAdaptor gvpa,
                                               const AjPStr synonym,
                                               AjPList gvps);

AjBool ensGVPopulationAdaptorFetchAllBySuperPopulation(
    EnsPGVPopulationAdaptor gvpa,
    const EnsPGVPopulation gvp,
    AjPList gvps);

AjBool ensGVPopulationAdaptorFetchAllBySubPopulation(
    EnsPGVPopulationAdaptor gvpa,
    const EnsPGVPopulation gvp,
    AjPList gvps);

AjBool ensGVPopulationAdaptorFetchAllByIndvividual(
    EnsPGVPopulationAdaptor gvpa,
    EnsPGVIndividual gvi,
    AjPList gvps);

/* Ensembl Genetic Variation Allele */

EnsPGVAllele ensGVAlleleNew(EnsPGVAlleleAdaptor gvaa,
                            ajuint identifier,
                            EnsPGVPopulation gvp,
                            AjPStr allele,
                            float frequency);

EnsPGVAllele ensGVAlleleNewObj(const EnsPGVAllele object);

EnsPGVAllele ensGVAlleleNewRef(EnsPGVAllele gva);

void ensGVAlleleDel(EnsPGVAllele* Pgva);

EnsPGVAlleleAdaptor ensGVAlleleGetAdaptor(const EnsPGVAllele gva);

ajuint ensGVAlleleGetIdentifier(const EnsPGVAllele gva);

EnsPGVPopulation ensGVAlleleGetPopulation(const EnsPGVAllele gva);

AjPStr ensGVAlleleGetAllele(const EnsPGVAllele gva);

float ensGVAlleleGetFrequency(const EnsPGVAllele gva);

AjBool ensGVAlleleSetAdaptor(EnsPGVAllele gva, EnsPGVAlleleAdaptor gvaa);

AjBool ensGVAlleleSetIdentifier(EnsPGVAllele gva, ajuint identifier);

AjBool ensGVAlleleSetPopulation(EnsPGVAllele gva, EnsPGVPopulation gvp);

AjBool ensGVAlleleSetAllele(EnsPGVAllele gva, AjPStr allelestr);

AjBool ensGVAlleleSetFrequency(EnsPGVAllele gva, float frequency);

ajuint ensGVAlleleGetMemSize(const EnsPGVAllele gva);

AjBool ensGVAlleleTrace(const EnsPGVAllele gva, ajuint level);

/* Ensembl Genetic Variation Genotype */

EnsPGVGenotype ensGVGenotypeNew(EnsPGVGenotypeAdaptor gvga,
                                ajuint identifier,
                                AjPStr allele1,
                                AjPStr allele2);

EnsPGVGenotype ensGVGenotypeNewObj(const EnsPGVGenotype object);

EnsPGVGenotype ensGVGenotypeNewRef(EnsPGVGenotype gvg);

void ensGVGenotypeDel(EnsPGVGenotype* Pgvg);

EnsPGVGenotypeAdaptor ensGVGenotypeGetAdaptor(const EnsPGVGenotype gvg);

ajuint ensGVGenotypeGetIdentifier(const EnsPGVGenotype gvg);

AjPStr ensGVGenotypeGetAllele1(const EnsPGVGenotype gvg);

AjPStr ensGVGenotypeGetAllele2(const EnsPGVGenotype gvg);

AjBool ensGVGenotypeSetAdaptor(EnsPGVGenotype gvg, EnsPGVGenotypeAdaptor gvga);

AjBool ensGVGenotypeSetIdentifier(EnsPGVGenotype gvg, ajuint identifier);

AjBool ensGVGenotypeSetAllele1(EnsPGVGenotype gvg, AjPStr allele1);

AjBool ensGVGenotypeSetAllele2(EnsPGVGenotype gvg, AjPStr allele2);

ajuint ensGVGenotypeGetMemSize(const EnsPGVGenotype gvg);

AjBool ensGVGenotypeTrace(const EnsPGVGenotype gvg, ajuint level);

/* Ensembl Genetic Variation Variation */

EnsPGVVariation ensGVVariationNew(EnsPGVVariationAdaptor adaptor,
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

EnsPGVVariationAdaptor ensGVVariationGetAdaptor(const EnsPGVVariation gvv);

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

AjBool ensGVVariationSetAdaptor(EnsPGVVariation gvv,
                                EnsPGVVariationAdaptor gvva);

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

EnsPGVVariationAdaptor ensGVVariationAdaptorNew(EnsPDatabaseAdaptor dba);

void ensGVVariationAdaptorDel(EnsPGVVariationAdaptor *Pgvva);

AjBool ensGVVariationAdaptorFetchByIdentifier(EnsPGVVariationAdaptor gvva,
                                              ajuint identifier,
                                              EnsPGVVariation *Pgvv);

AjBool ensGVVariationAdaptorFetchByName(EnsPGVVariationAdaptor gvva,
                                        const AjPStr name,
                                        const AjPStr source,
                                        EnsPGVVariation *Pgvv);

AjBool ensGVVariationAdaptorFetchAllBySource(EnsPGVVariationAdaptor gvva,
                                             const AjPStr source,
                                             AjBool primary,
                                             AjPList gvvs);

/* Ensembl Genetic Variation Variation Feature */

EnsPGVVariationFeature ensGVVariationFeatureNew(
    EnsPGVVariationFeatureAdaptor adaptor,
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

EnsPGVVariationFeatureAdaptor ensGVVariationFeatureGetAdaptor(
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

AjBool ensGVVariationFeatureSetAdaptor(EnsPGVVariationFeature gvvf,
                                       EnsPGVVariationFeatureAdaptor gvvfa);

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
