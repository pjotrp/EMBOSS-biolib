#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensvariation_h
#define ensvariation_h

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ensgvdata.h"
#include "ensgvsample.h"
#include "ensgvpopulation.h"




/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */

/* #data EnsPGvalleleadaptor **************************************************
**
** Ensembl Genetic Variation Allele Adaptor.
** Defined as an alias in EnsPDatabaseadaptor
**
** #alias EnsPDatabaseadaptor
** ##
** NOTE: Although the Bio::EnsEMBL::Variation::Allele object is based on the
** Bio::EnsEMBL::Storable object, there is actually no
** Bio::EnsEMBL::Variation::DBSQL::AlleleAdaptor as such in the
** Ensembl Variation API.
******************************************************************************/

#define EnsPGvalleleadaptor EnsPDatabaseadaptor




/* #data EnsPGvgenotypeadaptor ************************************************
**
** Ensembl Genetic Variation Genotype Adaptor.
** Defined as an alias in EnsPDatabaseadaptor
**
** #alias EnsPDatabaseadaptor
** ##
******************************************************************************/

#define EnsPGvgenotypeadaptor EnsPDatabaseadaptor




/* #data EnsPGvpopulationgenotypeadaptor **************************************
**
** Ensembl Genetic Variation Population Genotype Adaptor.
** Defined as an alias in EnsPBaseadaptor
**
** #alias EnsPBaseadaptor
** ##
******************************************************************************/

#define EnsPGvpopulationgenotypeadaptor EnsPBaseadaptor




/* #data EnsPGvsynonymadaptor *************************************************
**
** Ensembl Genetic Variation Synonym Adaptor.
** Defined as an alias in EnsPDatabaseadaptor
**
** #alias EnsPDatabaseadaptor
** ##
******************************************************************************/

#define EnsPGvsynonymadaptor EnsPDatabaseadaptor




/* #data EnsPGvvariationadaptor ***********************************************
**
** Ensembl Genetic Variation Variation Adaptor.
** Defined as an alias in EnsPDatabaseadaptor
**
** #alias EnsPDatabaseadaptor
** ##
******************************************************************************/

#define EnsPGvvariationadaptor EnsPDatabaseadaptor




/* #data EnsPGvvariationfeatureadaptor ****************************************
**
** Ensembl Genetic Variation Variation Feature Adaptor.
** Defined as an alias in EnsPFeatureadaptor
**
** #alias EnsPFeatureadaptor
** ##
******************************************************************************/

#define EnsPGvvariationfeatureadaptor EnsPFeatureadaptor




/* #data EnsPGvvariationsetadaptor ********************************************
**
** Ensembl Genetic Variation Variation Set Adaptor.
** Defined as an alias in EnsPBaseadaptor
**
** #alias EnsPBaseadaptor
** ##
******************************************************************************/

#define EnsPGvvariationsetadaptor EnsPBaseadaptor




/* EnsEGvvariationValidationState *********************************************
**
** Ensembl Genetic Variation Variation validation state enumeration.
**
******************************************************************************/

typedef enum EnsOGvvariationValidationState
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
} EnsEGvvariationValidationState;




/* ==================================================================== */
/* ========================== public data ============================= */
/* ==================================================================== */

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
** @attr SubSNPHandle [AjPStr] Sub SNP handle
** @attr Frequency [float] Frequency
** @attr SubSNPIdentifier [ajuint] Sub SNP Identifier
** @@
******************************************************************************/

typedef struct EnsSGvallele
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvalleleadaptor Adaptor;
    EnsPGvpopulation Gvpopulation;
    AjPStr Allele;
    AjPStr SubSNPHandle;
    float Frequency;
    ajuint SubSNPIdentifier;
} EnsOGvallele;

#define EnsPGvallele EnsOGvallele*




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




/* @data EnsPGvsourceadaptor **************************************************
**
** Ensembl Genetic Variation Source Adaptor.
**
** @alias EnsSGvsourceadaptor
** @alias EnsOGvsourceadaptor
**
** @attr Adaptor [EnsPBaseadaptor] Ensembl Base Adaptor
** @attr CacheByIdentifier [AjPTable] Identifier cache
** @attr CacheByName [AjPTable] Name cache
** @attr DefaultGvsource [void*] Default Ensembl Genetic Variation Source
** @@
******************************************************************************/

typedef struct EnsSGvsourceadaptor {
    EnsPBaseadaptor Adaptor;
    AjPTable CacheByIdentifier;
    AjPTable CacheByName;
    void * DefaultGvsource;
} EnsOGvsourceadaptor;

#define EnsPGvsourceadaptor EnsOGvsourceadaptor*




/* @data EnsPGvsource *********************************************************
**
** Ensembl Genetic Variation Source.
**
** @alias EnsSGvsource
** @alias EnsOGvsource
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGvsourceadaptor] Ensembl Genetic Variation
**                                     Source Adaptor
** @cc Bio::EnsEMBL::Variation::??
** @attr Name [AjPStr] Name
** @attr Version [AjPStr] Version
** @attr Description [AjPStr] Description
** @attr URL [AjPStr] Uniform Resource Locator
** @attr Somatic [AjBool] Somatic or germline variation
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSGvsource
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvsourceadaptor Adaptor;
    AjPStr Name;
    AjPStr Version;
    AjPStr Description;
    AjPStr URL;
    AjBool Somatic;
    ajuint Padding;
} EnsOGvsource;

#define EnsPGvsource EnsOGvsource*




/* @data EnsPGvsynonym ********************************************************
**
** Ensembl Genetic Variation Synonym.
**
** @alias EnsSGvsynonym
** @alias EnsOGvsynonym
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGvsynonymadaptor] Ensembl Genetic Variation
**                                      Synonym Adaptor
** @cc Bio::EnsEMBL::Variation::??
** @attr Gvsource [EnsPGvsource] Ensembl Genetic Variation Source
** @attr Name [AjPStr] Name
** @attr Moleculetype [AjPStr] Molecule type
** @attr Gvvariationidentifier [ajuint] Ensembl Genetic Variation identifier
** @attr Subidentifier [ajuint] Sub Identifier
** @@
******************************************************************************/

typedef struct EnsSGvsynonym
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvsynonymadaptor Adaptor;
    EnsPGvsource Gvsource;
    AjPStr Name;
    AjPStr Moleculetype;
    ajuint Gvvariationidentifier;
    ajuint Subidentifier;
} EnsOGvsynonym;

#define EnsPGvsynonym EnsOGvsynonym*




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
** @attr Gvsource [EnsPGvsource] Ensembl Genetic Variation Source
** @attr Name [AjPStr] Name
** @attr Handles [AjPTable] Handles
** @attr AncestralAllele [AjPStr] Ancestral allele
** @attr Gvalleles [AjPList] AJAX List of Ensembl Genetic Variation Alleles
** @attr Gvsynonyms [AjPList] AJAX List of Ensembl Genetic Variation Synonyms
** @attr MoleculeType [AjPStr] Molecule type
** @attr FivePrimeFlank [AjPStr] Five prime flanking sequence
** @attr ThreePrimeFlank [AjPStr] Three prime flanking sequence
** @attr FailedDescription [AjPStr] Failed description
** @attr Flank [AjBool] Flank
** @attr ValidationStates [ajuint] Bit field of validation states
** @@
******************************************************************************/

typedef struct EnsSGvvariation
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvvariationadaptor Adaptor;
    EnsPGvsource Gvsource;
    AjPStr Name;
    AjPTable Handles;
    AjPStr AncestralAllele;
    AjPList Gvalleles;
    AjPList Gvsynonyms;
    AjPStr MoleculeType;
    AjPStr FivePrimeFlank;
    AjPStr ThreePrimeFlank;
    AjPStr FailedDescription;
    AjBool Flank;
    ajuint ValidationStates;
} EnsOGvvariation;

#define EnsPGvvariation EnsOGvvariation*




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
**                                               Genotype Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Variation::VariationFeature
** @attr Gvsource [EnsPGvsource] Ensembl Genetic Variation Source
** @attr Gvvariation [EnsPGvvariation] Ensembl Genetic Variation Variation
** @attr Allele [AjPStr] Allele, denormalisation from allele.allele
** @attr Name [AjPStr] Name, denormalisation from variation.name
** @attr ValidationCode [AjPStr] Validation code
** @attr ConsequenceType [AjPStr] Consequence type
** @attr MapWeight [ajuint] Map weight or the number of times that the
**                          Variation associated with this Feature has hit
**                          the genome. If this was the only Feature associated
**                          with this Variation Feature the map weight would be
**                          1.
** @attr Gvvariationidentifier [ajuint] Ensembl Genetic Variation Variation
**                                      identifier
** @@
******************************************************************************/

typedef struct EnsSGvvariationfeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvvariationfeatureadaptor Adaptor;
    EnsPFeature Feature;
    EnsPGvsource Gvsource;
    EnsPGvvariation Gvvariation;
    AjPStr Allele;
    AjPStr Name;
    AjPStr ValidationCode;
    AjPStr ConsequenceType;
    ajuint MapWeight;
    ajuint Gvvariationidentifier;
} EnsOGvvariationfeature;

#define EnsPGvvariationfeature EnsOGvvariationfeature*




/* @data EnsPGvconsequence ****************************************************
**
** Ensembl Genetic Variation Consequence.
**
** @alias EnsSGvconsequence
** @alias EnsOGvconsequence
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Variation::ConsequenceType
** @attr TranscriptIdentifier [ajuint] Ensembl Transcript identifier
** @attr GvvariationfeatureIdentifier [ajuint] Ensembl Genetic Variation
**                                             Variation Feature identifier
** @attr Start [ajint] Start
** @attr End [ajint] End
** @attr Strand [ajuint] Strand
** @attr Alleles [AjPList] AJAX List of AJAX String alleles
** @attr Types [AjPList] AJAX List of AJAX String consequence types
** @@
******************************************************************************/

typedef struct EnsSGvconsequence
{
    ajuint Use;
    ajuint TranscriptIdentifier;
    ajuint GvvariationfeatureIdentifier;
    ajuint Start;
    ajuint End;
    ajuint Strand;
    AjPList Alleles;
    AjPList Types;
} EnsOGvconsequence;

#define EnsPGvconsequence EnsOGvconsequence*




/* @data EnsPGvvariationset ***************************************************
**
** Ensembl Genetic Variation Variation Set.
**
** @alias EnsSGvvariationset
** @alias EnsOGvvariationset
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGvsetadaptor] Ensembl Genetic Variation Set Adaptor
** @cc Bio::EnsEMBL::Variation::VariationSet
** @attr Name [AjPStr] Name
** @attr Description [AjPstr] Description
** @@
******************************************************************************/

typedef struct EnsSGvvariationset
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvvariationsetadaptor Adaptor;
    AjPStr Name;
    AjPStr Description;
} EnsOGvvariationset;

#define EnsPGvvariationset EnsOGvvariationset*




/* @data EnsPGvpopulationgenotype *****************************************
**
** Ensembl Genetic Variation Population Genotype.
**
** @alias EnsSGvpopulationgenotype
** @alias EnsOGvpopulationgenotype
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPGvpopulationgenotypeadaptor] Ensembl Genetic
**                                       Variation Population Genotype Adaptor
** @cc Bio::EnsEMBL::Variation::PopulationGenotype
** @attr Gvpopulation [EnsPGvpopulation] Ensembl Genetic Variation Population
** @attr Gvvariation [EnsPGvvariation] Ensembl Genetic Variation Variation
** @attr Allele1 [AjPStr] Allele1
** @attr Allele2 [AjPStr] Allele2
** @attr Subidentifier [ajuint] Sub-Identifier
** @attr Frequency [float] Frequency
** @@
******************************************************************************/

typedef struct EnsSGvpopulationgenotype
{
    ajuint Use;
    ajuint Identifier;
    EnsPGvpopulationgenotypeadaptor Adaptor;
    EnsPGvpopulation Gvpopulation;
    EnsPGvvariation Gvvariation;
    AjPStr Allele1;
    AjPStr Allele2;
    ajuint Subidentifier;
    float Frequency;
} EnsOGvpopulationgenotype;

#define EnsPGvpopulationgenotype EnsOGvpopulationgenotype*




/* ==================================================================== */
/* ======================= public functions =========================== */
/* ==================================================================== */

/*
** Prototype definitions
*/

/* Ensembl Genetic Variation Allele */

EnsPGvallele ensGvalleleNew(EnsPGvalleleadaptor gvaa,
                            ajuint identifier,
                            EnsPGvpopulation gvp,
                            AjPStr allele,
                            float frequency,
                            ajuint subsnpid);

EnsPGvallele ensGvalleleNewObj(const EnsPGvallele object);

EnsPGvallele ensGvalleleNewRef(EnsPGvallele gva);

void ensGvalleleDel(EnsPGvallele* Pgva);

EnsPGvalleleadaptor ensGvalleleGetAdaptor(const EnsPGvallele gva);

ajuint ensGvalleleGetIdentifier(const EnsPGvallele gva);

EnsPGvpopulation ensGvalleleGetPopulation(const EnsPGvallele gva);

AjPStr ensGvalleleGetAllele(const EnsPGvallele gva);

float ensGvalleleGetFrequency(const EnsPGvallele gva);

AjPStr ensGvalleleGetSubSNPHandle(EnsPGvallele gva);

ajuint ensGvalleleGetSubSNPIdentifier(const EnsPGvallele gva);

AjBool ensGvalleleSetAdaptor(EnsPGvallele gva, EnsPGvalleleadaptor gvaa);

AjBool ensGvalleleSetIdentifier(EnsPGvallele gva, ajuint identifier);

AjBool ensGvalleleSetPopulation(EnsPGvallele gva, EnsPGvpopulation gvp);

AjBool ensGvalleleSetAllele(EnsPGvallele gva, AjPStr allelestr);

AjBool ensGvalleleSetFrequency(EnsPGvallele gva, float frequency);

AjBool ensGvalleleSetSubSNPHandle(EnsPGvallele gva, AjPStr subsnphandle);

AjBool ensGvalleleSetSubSNPIdentifier(EnsPGvallele gva, ajuint subsnpid);

ajulong ensGvalleleGetMemsize(const EnsPGvallele gva);

AjBool ensGvalleleTrace(const EnsPGvallele gva, ajuint level);

/* Ensembl Genetic Variation Allele Adaptor */

EnsPGvalleleadaptor ensRegistryGetGvalleleadaptor(
    EnsPDatabaseadaptor dba);

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

ajulong ensGvgenotypeGetMemsize(const EnsPGvgenotype gvg);

AjBool ensGvgenotypeTrace(const EnsPGvgenotype gvg, ajuint level);

/* Ensembl Genetic Variation Genotype Adaptor */

EnsPGvgenotypeadaptor ensRegistryGetGvgenotypeadaptor(
    EnsPDatabaseadaptor dba);

/* Ensembl Genetic Variation Source */

EnsPGvsource ensGvsourceNew(EnsPGvsourceadaptor gvsa,
                            ajuint identifier,
                            AjPStr name,
                            AjPStr version,
                            AjPStr description,
                            AjPStr url,
                            AjBool somatic);

EnsPGvsource ensGvsourceNewObj(const EnsPGvsource object);

EnsPGvsource ensGvsourceNewRef(EnsPGvsource gvs);

void ensGvsourceDel(EnsPGvsource *Pgvs);

EnsPGvsourceadaptor ensGvsourceGetAdaptor(const EnsPGvsource gvs);

ajuint ensGvsourceGetIdentifier(const EnsPGvsource gvs);

AjPStr ensGvsourceGetName(const EnsPGvsource gvs);

AjPStr ensGvsourceGetVersion(const EnsPGvsource gvs);

AjPStr ensGvsourceGetDescription(const EnsPGvsource gvs);

AjPStr ensGvsourceGetURL(const EnsPGvsource gvs);

AjBool ensGvsourceGetSomatic(const EnsPGvsource gvs);

AjBool ensGvsourceSetAdaptor(EnsPGvsource gvs, EnsPGvsourceadaptor gvsa);

AjBool ensGvsourceSetIdentifier(EnsPGvsource gvs, ajuint identifier);

AjBool ensGvsourceSetName(EnsPGvsource gvs, AjPStr name);

AjBool ensGvsourceSetVersion(EnsPGvsource gvs, AjPStr version);

AjBool ensGvsourceSetDescription(EnsPGvsource gvs, AjPStr description);

AjBool ensGvsourceSetURL(EnsPGvsource gvs, AjPStr url);

AjBool ensGvsourceSetSomatic(EnsPGvsource gvs, AjBool somatic);

ajulong ensGvsourceGetMemsize(const EnsPGvsource gvs);

AjBool ensGvsourceTrace(const EnsPGvsource gvs, ajuint level);

AjBool ensGvsourceMatch(const EnsPGvsource gvs1, const EnsPGvsource gvs2);

/* Ensembl Genetic Variation Source Adaptor */

EnsPGvsourceadaptor ensRegistryGetGvsourceadaptor(
    EnsPDatabaseadaptor dba);

EnsPGvsourceadaptor ensGvsourceadaptorNew(EnsPDatabaseadaptor dba);

void ensGvsourceadaptorDel(EnsPGvsourceadaptor* Pgvsa);

EnsPBaseadaptor ensGvsourceadaptorGetBaseadaptor(
    const EnsPGvsourceadaptor gvsa);

EnsPDatabaseadaptor ensGvsourceadaptorGetDatabaseadaptor(
    const EnsPGvsourceadaptor gvsa);

AjBool ensGvsourceadaptorFetchAll(EnsPGvsourceadaptor gvsa,
                                  AjPList gvss);

AjBool ensGvsourceadaptorFetchByIdentifier(EnsPGvsourceadaptor gvsa,
                                           ajuint identifier,
                                           EnsPGvsource *Pgvs);

AjBool ensGvsourceadaptorFetchByName(EnsPGvsourceadaptor gvsa,
                                     const AjPStr name,
                                     EnsPGvsource *Pgvs);

AjBool ensGvsourceadaptorFetchDefault(EnsPGvsourceadaptor gvsa,
                                      EnsPGvsource *Pgvs);

/* Ensembl Genetic Variation Synonym */

EnsPGvsynonym ensGvsynonymNew(EnsPGvsynonymadaptor gvsa,
                              ajuint identifier,
                              EnsPGvsource gvsource,
                              AjPStr name,
                              AjPStr moleculetype,
                              ajuint gvvidentifier,
                              ajuint subidentifier);

EnsPGvsynonym ensGvsynonymNewObj(const EnsPGvsynonym object);

EnsPGvsynonym ensGvsynonymNewRef(EnsPGvsynonym gvs);

void ensGvsynonymDel(EnsPGvsynonym *Pgvs);

EnsPGvsynonymadaptor ensGvsynonymGetAdaptor(const EnsPGvsynonym gvs);

ajuint ensGvsynonymGetIdentifier(const EnsPGvsynonym gvs);

EnsPGvsource ensGvsynonymGetGvsource(const EnsPGvsynonym gvs);

AjPStr ensGvsynonymGetName(const EnsPGvsynonym gvs);

AjPStr ensGvsynonymGetMoleculetype(const EnsPGvsynonym gvs);

ajuint ensGvsynonymGetGvvariationidentifier(const EnsPGvsynonym gvs);

ajuint ensGvsynonymGetSubidentifier(const EnsPGvsynonym gvs);

AjBool ensGvsynonymSetAdaptor(EnsPGvsynonym gvs,
                              EnsPGvsynonymadaptor gvsa);

AjBool ensGvsynonymSetIdentifier(EnsPGvsynonym gvs,
                                 ajuint identifier);

AjBool ensGvsynonymSetGvsource(EnsPGvsynonym gvs,
                               EnsPGvsource gvsource);

AjBool ensGvsynonymSetName(EnsPGvsynonym gvs,
                           AjPStr name);

AjBool ensGvsynonymSetMoleculetype(EnsPGvsynonym gvs,
                                   AjPStr moleculetype);

AjBool ensGvsynonymSetGvvariationidentifier(EnsPGvsynonym gvs,
                                            ajuint gvvidentifier);

AjBool ensGvsynonymSetSubidentifier(EnsPGvsynonym gvs,
                                    ajuint subidentifier);

ajulong ensGvsynonymGetMemsize(const EnsPGvsynonym gvs);

AjBool ensGvsynonymTrace(const EnsPGvsynonym gvs, ajuint level);

/* Ensembl Genetic Variation Variation */

EnsPGvvariation ensGvvariationNew(EnsPGvvariationadaptor gvva,
                                  ajuint identifier,
                                  EnsPGvsource gvs,
                                  AjPStr name,
                                  AjPStr ancestralallele,
                                  AjPList gvalleles,
                                  AjPList gvsynonyms,
                                  AjPStr validationstates,
                                  AjPStr moltype,
                                  AjPStr fiveflank,
                                  AjPStr threeflank,
                                  AjPStr faileddescription,
                                  AjBool flank);

EnsPGvvariation ensGvvariationNewObj(const EnsPGvvariation object);

EnsPGvvariation ensGvvariationNewRef(EnsPGvvariation gvv);

void ensGvvariationDel(EnsPGvvariation* Pgvv);

EnsPGvvariationadaptor ensGvvariationGetAdaptor(const EnsPGvvariation gvv);

ajuint ensGvvariationGetIdentifier(const EnsPGvvariation gvv);

EnsPGvsource ensGvvariationGetGvsource(const EnsPGvvariation gvv);

AjPStr ensGvvariationGetName(const EnsPGvvariation gvv);

AjPStr ensGvvariationGetAncestralAllele(const EnsPGvvariation gvv);

const AjPList ensGvvariationGetGvalleles(const EnsPGvvariation gvv);

const AjPList ensGvvariationGetGvsynonyms(const EnsPGvvariation gvv);

ajuint ensGvvariationGetValidationStates(const EnsPGvvariation gvv);

AjPStr ensGvvariationGetMoleculeType(const EnsPGvvariation gvv);

AjPStr ensGvvariationGetFivePrimeFlank(EnsPGvvariation gvv);

AjPStr ensGvvariationGetThreePrimeFlank(EnsPGvvariation gvv);

AjPStr ensGvvariationGetFailedDescription(const EnsPGvvariation gvv);

AjBool ensGvvariationGetFlank(const EnsPGvvariation gvv);

AjBool ensGvvariationSetAdaptor(EnsPGvvariation gvv,
                                EnsPGvvariationadaptor gvva);

AjBool ensGvvariationSetIdentifier(EnsPGvvariation gvv,
                                   ajuint identifier);

AjBool ensGvvariationSetGvsource(EnsPGvvariation gvv,
                                 EnsPGvsource gvs);

AjBool ensGvvariationSetName(EnsPGvvariation gvv,
                             AjPStr name);

AjBool ensGvvariationSetMoleculeType(EnsPGvvariation gvv,
                                     AjPStr moltype);

AjBool ensGvvariationSetAncestralAllele(EnsPGvvariation gvv,
                                        AjPStr ancestralallele);

AjBool ensGvvariationSetFivePrimeFlank(EnsPGvvariation gvv,
                                       AjPStr fiveflank);

AjBool ensGvvariationSetThreePrimeFlank(EnsPGvvariation gvv,
                                        AjPStr threeflank);

AjBool ensGvvariationSetFailedDescription(EnsPGvvariation gvv,
                                          AjPStr faileddescription);

AjBool ensGvvariationSetFlank(EnsPGvvariation gvv,
                              AjBool flank);

ajulong ensGvvariationGetMemsize(const EnsPGvvariation gvv);

AjBool ensGvvariationTrace(const EnsPGvvariation gvv, ajuint level);

AjBool ensGvvariationAddGvallele(EnsPGvvariation gvv, EnsPGvallele gva);

AjBool ensGvvariationAddGvsynonym(EnsPGvvariation gvv, EnsPGvsynonym gvs);

AjBool ensGvvariationAddValidationState(EnsPGvvariation gvv,
                                        EnsEGvvariationValidationState state);

EnsEGvvariationValidationState ensGvvariationValidationStateFromStr(
    const AjPStr state);

const char *ensGvvariationValidationStateToChar(
    EnsEGvvariationValidationState state);

ajuint ensGvvariationValidationStatesFromSet(const AjPStr set);

AjBool ensGvvariationValidationStatesToSet(ajuint states, AjPStr *Pstr);

AjBool ensGvvariationFetchAllGvsynonyms(const EnsPGvvariation gvv,
                                        const AjPStr gvsourcename,
                                        AjPList gvss);

AjBool ensGvvariationFetchAllGvsynonymGvsources(const EnsPGvvariation gvv,
                                                AjPList gvss);

AjBool ensGvvariationFetchAllGenes(EnsPGvvariation gvv,
                                   ajint flank,
                                   AjPList genes);

AjBool ensGvvariationFetchAllGvvariationfeatures(EnsPGvvariation gvv,
                                                 AjPList gvvfs);

AjBool ensTableDeleteGvvariations(AjPTable *Pgvvs);

/* Ensembl Genetic Variation Variation Adaptor */

EnsPGvvariationadaptor ensRegistryGetGvvariationadaptor(
    EnsPDatabaseadaptor dba);

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

AjBool ensGvvariationadaptorFetchAllByGvpopulation(EnsPGvvariationadaptor gvva,
                                                   const EnsPGvpopulation gvp,
                                                   AjPList gvvs);

AjBool ensGvvariationadaptorFetchAllByGvpopulationFrequency(
    EnsPGvvariationadaptor gvva,
    const EnsPGvpopulation gvp,
    float frequency,
    AjPList gvvs);

AjBool ensGvvariationadaptorFetchAllByGvvariationset(
    EnsPGvvariationadaptor gvva,
    EnsPGvvariationset gvvset,
    AjPList gvvs);

AjBool ensGvvariationadaptorFetchFlankingSequence(EnsPGvvariationadaptor gvva,
                                                  ajuint gvvidentifier,
                                                  AjPStr *Pfiveseq,
                                                  AjPStr *Pthreeseq);

AjBool ensGvvariationadaptorFetchAllByIdentifiers(EnsPGvvariationadaptor gvva,
                                                  AjPTable gvvs);

AjBool ensGvvariationadaptorFetchAllByNames(EnsPGvvariationadaptor gvva,
                                            const AjPList names,
                                            AjPList gvvs);

/* Ensembl Genetic Variation Variation Feature */

EnsPGvvariationfeature ensGvvariationfeatureNew(
    EnsPGvvariationfeatureadaptor adaptor,
    ajuint identifier,
    EnsPFeature feature,
    EnsPGvsource gvsource,
    EnsPGvvariation gvv,
    AjPStr allele,
    AjPStr name,
    AjPStr validation,
    AjPStr consequence,
    ajuint mapweight);

EnsPGvvariationfeature ensGvvariationfeatureNewIdentifier(
    EnsPGvvariationfeatureadaptor gvvfa,
    ajuint identifier,
    EnsPFeature feature,
    EnsPGvsource gvsource,
    ajuint gvvidentifier,
    AjPStr allele,
    AjPStr name,
    AjPStr validation,
    AjPStr consequence,
    ajuint mapweight);

EnsPGvvariationfeature ensGvvariationfeatureNewObj(
    const EnsPGvvariationfeature object);

EnsPGvvariationfeature ensGvvariationfeatureNewRef(EnsPGvvariationfeature gvvf);

void ensGvvariationfeatureDel(EnsPGvvariationfeature *Pgvvf);

EnsPGvvariationfeatureadaptor ensGvvariationfeatureGetAdaptor(
    const EnsPGvvariationfeature gvvf);

ajuint ensGvvariationfeatureGetIdentifier(
    const EnsPGvvariationfeature gvvf);

EnsPFeature ensGvvariationfeatureGetFeature(
    const EnsPGvvariationfeature gvvf);

EnsPGvsource ensGvvariationfeatureGetGvsource(
    const EnsPGvvariationfeature gvvf);

EnsPGvvariation ensGvvariationfeatureGetGvvariation(
    EnsPGvvariationfeature gvvf);

AjPStr ensGvvariationfeatureGetAllele(
    const EnsPGvvariationfeature gvvf);

AjPStr ensGvvariationfeatureGetName(
    const EnsPGvvariationfeature gvvf);

AjPStr ensGvvariationfeatureGetSource(
    const EnsPGvvariationfeature gvvf);

AjPStr ensGvvariationfeatureGetValidationCode(
    const EnsPGvvariationfeature gvvf);

AjPStr ensGvvariationfeatureGetConsequenceType(
    const EnsPGvvariationfeature gvvf);

ajuint ensGvvariationfeatureGetMapWeight(
    const EnsPGvvariationfeature gvvf);

AjBool ensGvvariationfeatureSetAdaptor(EnsPGvvariationfeature gvvf,
                                       EnsPGvvariationfeatureadaptor gvvfa);

AjBool ensGvvariationfeatureSetIdentifier(EnsPGvvariationfeature gvvf,
                                          ajuint identifier);

AjBool ensGvvariationfeatureSetFeature(EnsPGvvariationfeature gvvf,
                                       EnsPFeature feature);

AjBool ensGvvariationfeatureSetGvsource(EnsPGvvariationfeature gvvf,
                                        EnsPGvsource gvsource);

AjBool ensGvvariationfeatureSetGvvariation(EnsPGvvariationfeature gvvf,
                                           EnsPGvvariation gvv);

AjBool ensGvvariationfeatureSetAllele(EnsPGvvariationfeature gvvf,
                                      AjPStr allele);

AjBool ensGvvariationfeatureSetName(EnsPGvvariationfeature gvvf,
                                    AjPStr name);

AjBool ensGvvariationfeatureSetValidationCode(EnsPGvvariationfeature gvvf,
                                              AjPStr validationcode);

AjBool ensGvvariationfeatureSetConsequenceType(EnsPGvvariationfeature gvvf,
                                               AjPStr consequencetype);

AjBool ensGvvariationfeatureSetMapWeight(EnsPGvvariationfeature gvvf,
                                         ajuint mapweight);

ajulong ensGvvariationfeatureGetMemsize(const EnsPGvvariationfeature gvvf);

AjBool ensGvvariationfeatureTrace(const EnsPGvvariationfeature gvvf,
                                  ajuint level);

AjBool ensGvvariationfeatureSortByStartAscending(AjPList gvvfs);

AjBool ensGvvariationfeatureSortByStartDescending(AjPList gvvfs);

/* Ensembl Genetic Variation Feature Adaptor */

EnsPGvvariationfeatureadaptor ensRegistryGetGvvariationfeatureadaptor(
    EnsPDatabaseadaptor dba);

EnsPGvvariationfeatureadaptor ensGvvariationfeatureadaptorNew(
    EnsPDatabaseadaptor dba);

void ensGvvariationfeatureadaptorDel(EnsPGvvariationfeatureadaptor *Pgvvfa);

AjBool ensGvvariationfeatureAdaptorFetchAllBySliceConstraint(
    EnsPGvvariationfeatureadaptor gvvfa,
    EnsPSlice slice,
    const AjPStr constraint,
    AjBool somatic,
    AjPList gvvfs);

AjBool ensGvvariationfeatureAdaptorFetchAllBySlice(
    EnsPGvvariationfeatureadaptor gvvfa,
    EnsPSlice slice,
    AjBool somatic,
    AjPList gvvfs);

AjBool ensGvvariationfeatureadaptorFetchAllBySliceGenotyped(
    EnsPGvvariationfeatureadaptor gvvfa,
    EnsPSlice slice,
    AjPList gvvfs);

AjBool ensGvvariationfeatureadaptorFetchAllBySliceAnnotated(
    EnsPGvvariationfeatureadaptor gvvfa,
    const AjPStr vsource,
    const AjPStr psource,
    const AjPStr annotation,
    AjBool somatic,
    EnsPSlice slice,
    AjPList gvvfs);

AjBool ensGvvariationfeatureadaptorFetchAllByGvvariation(
    EnsPGvvariationfeatureadaptor gvvfa,
    EnsPGvvariation gvv,
    AjPList gvvfs);

AjBool ensGvvariationfeatureadaptorFetchAllByGvpopulation(
    EnsPGvvariationfeatureadaptor gvvfa,
    const EnsPGvpopulation gvp,
    EnsPSlice slice,
    AjPList gvvfs);

AjBool ensGvvariationfeatureadaptorFetchAllByGvpopulationFrequency(
    EnsPGvvariationfeatureadaptor gvvfa,
    const EnsPGvpopulation gvp,
    float frequency,
    EnsPSlice slice,
    AjPList gvvfs);

AjBool ensGvvariationfeatureadaptorFetchAllIdentifiers(
    const EnsPGvvariationfeatureadaptor gvvfa,
    AjPList identifiers);

/* Ensembl Genetic Variation Variation Set */

EnsPGvvariationset ensGvvariationsetNew(EnsPGvvariationsetadaptor gvvsa,
                                        ajuint identifier,
                                        AjPStr name,
                                        AjPStr description);

EnsPGvvariationset ensGvvariationsetNewObj(const EnsPGvvariationset object);

EnsPGvvariationset ensGvvariationsetNewRef(EnsPGvvariationset gvvs);

void ensGvvariationsetDel(EnsPGvvariationset *Pgvvs);

EnsPGvvariationsetadaptor ensGvvariationsetGetAdaptor(
    const EnsPGvvariationset gvvs);

ajuint ensGvvariationsetGetIdentifier(
    const EnsPGvvariationset gvvs);

AjPStr ensGvvariationsetGetName(
    const EnsPGvvariationset gvvs);

AjPStr ensGvvariationsetGetDescription(
    const EnsPGvvariationset gvvs);

AjBool ensGvvariationsetSetAdaptor(EnsPGvvariationset gvvs,
                                   EnsPGvvariationsetadaptor gvvsa);

AjBool ensGvvariationsetSetIdentifier(EnsPGvvariationset gvvs,
                                      ajuint identifier);

AjBool ensGvvariationsetSetName(EnsPGvvariationset gvvs,
                                AjPStr name);

AjBool ensGvvariationsetSetDescription(EnsPGvvariationset gvvs,
                                       AjPStr description);

ajulong ensGvvariationsetGetMemsize(const EnsPGvvariationset gvvs);

AjBool ensGvvariationsetTrace(const EnsPGvvariationset gvvs,
                              ajuint level);

AjBool ensGvvariationsetFetchAllSubSets(EnsPGvvariationset gvvs,
                                        AjBool immediate,
                                        AjPList gvvss);

AjBool ensGvvariationsetFetchAllSuperSets(EnsPGvvariationset gvvs,
                                          AjBool immediate,
                                          AjPList gvvss);

AjBool ensGvvariationsetFetchAllGvvariations(
    EnsPGvvariationset gvvs,
    AjPList gvvss);

/* Ensembl Genetic Variation Variation Set Adaptor */

EnsPGvvariationsetadaptor ensRegistryGetGvvariationsetadaptor(
    EnsPDatabaseadaptor dba);

EnsPGvvariationsetadaptor ensGvvariationsetadaptorNew(
    EnsPDatabaseadaptor dba);

void ensGvvariationsetadaptorDel(EnsPGvvariationsetadaptor* Pgvvsa);

AjBool ensGvvariationsetadaptorFetchByIdentifier(
    EnsPGvvariationsetadaptor gvvsa,
    ajuint identifier,
    EnsPGvvariationset *Pgvvs);

AjBool ensGvvariationsetadaptorFetchByName(
    EnsPGvvariationsetadaptor gvvsa,
    const AjPStr name,
    EnsPGvvariationset *Pgvvs);

AjBool ensGvvariationsetadaptorFetchAllToplevel(
    EnsPGvvariationsetadaptor gvvsa,
    AjPList gvvss);

AjBool ensGvvariationsetadaptorFetchAllByGvvariation(
    EnsPGvvariationsetadaptor gvvsa,
    const EnsPGvvariation gvv,
    AjPList gvvss);

AjBool ensGvvariationsetadaptorFetchAllBySubSet(
    EnsPGvvariationsetadaptor gvvsa,
    const EnsPGvvariationset gvvs,
    AjBool immediate,
    AjPList gvvss);

AjBool ensGvvariationsetadaptorFetchAllBySuperSet(
    EnsPGvvariationsetadaptor gvvsa,
    const EnsPGvvariationset gvvs,
    AjBool immediate,
    AjPList gvvss);

/* Ensembl Genetic Variation Consequence */

EnsPGvconsequence ensGvconsequenceNew(ajuint transcriptid,
                                      ajuint gvvfid,
                                      ajint start,
                                      ajint end,
                                      ajint strand);

EnsPGvconsequence ensGvconsequenceNewObj(const EnsPGvconsequence object);

EnsPGvconsequence ensGvconsequenceNewRef(EnsPGvconsequence gvvc);

void ensGvconsequenceDel(EnsPGvconsequence *Pgvvc);

ajuint ensGvconsequenceGetTranscriptIdentifier(
    const EnsPGvconsequence gvvc);
ajuint ensGvconsequenceGetGvvariationfeatureIdentifier(
    const EnsPGvconsequence gvvc);
ajint ensGvconsequenceGetStart(
    const EnsPGvconsequence gvvc);

ajint ensGvconsequenceGetEnd(
    const EnsPGvconsequence gvvc);

ajint ensGvconsequenceGetStrand(
    const EnsPGvconsequence gvvc);

AjBool ensGvconsequenceSetTranscriptIdentifier(EnsPGvconsequence gvvc,
                                               ajuint transcriptid);

AjBool ensGvconsequenceSetGvvariationfeatureIdentifier(EnsPGvconsequence gvvc,
                                                       ajuint gvvfid);

AjBool ensGvconsequenceSetStart(EnsPGvconsequence gvvc,
                                ajint start);

AjBool ensGvconsequenceSetEnd(EnsPGvconsequence gvvc,
                              ajint end);

AjBool ensGvconsequenceSetStrand(EnsPGvconsequence gvvc,
                                 ajint strand);

ajulong ensGvconsequenceGetMemsize(const EnsPGvconsequence gvvc);

AjBool ensGvconsequenceTrace(const EnsPGvconsequence gvvc,
                             ajuint level);

/* Ensembl Genetic Variation Poulation Genotype */

EnsPGvpopulationgenotype ensGvpopulationgenotypeNew(
    EnsPGvpopulationgenotypeadaptor gvpga,
    ajuint identifier,
    EnsPGvpopulation gvp,
    EnsPGvvariation gvv,
    AjPStr allele1,
    AjPStr allele2,
    float frequency);

EnsPGvpopulationgenotype ensGvpopulationgenotypeNewObj(
    const EnsPGvpopulationgenotype object);

EnsPGvpopulationgenotype ensGvpopulationgenotypeNewRef(
    EnsPGvpopulationgenotype gvpg);

void ensGvpopulationgenotypeDel(EnsPGvpopulationgenotype *Pgvpg);

EnsPGvpopulationgenotypeadaptor ensGvpopulationgenotypeGetAdaptor(
    const EnsPGvpopulationgenotype gvpg);

ajuint ensGvpopulationgenotypeGetIdentifier(
    const EnsPGvpopulationgenotype gvpg);

EnsPGvpopulation ensGvpopulationgenotypeGetGvpopulation(
    const EnsPGvpopulationgenotype gvpg);

EnsPGvvariation ensGvpopulationgenotypeGetGvvariation(
    const EnsPGvpopulationgenotype gvpg);

AjPStr ensGvpopulationgenotypeGetAllele1(
    const EnsPGvpopulationgenotype gvpg);

AjPStr ensGvpopulationgenotypeGetAllele2(
    const EnsPGvpopulationgenotype gvpg);

ajuint ensGvpopulationgenotypeGetSubidentifier(
    const EnsPGvpopulationgenotype gvpg);

float ensGvpopulationgenotypeGetFrequency(
    const EnsPGvpopulationgenotype gvpg);

AjBool ensGvpopulationgenotypeSetAdaptor(EnsPGvpopulationgenotype gvpg,
                                         EnsPGvpopulationgenotypeadaptor gvpga);

AjBool ensGvpopulationgenotypeSetIdentifier(EnsPGvpopulationgenotype gvpg,
                                            ajuint identifier);

AjBool ensGvpopulationgenotypeSetGvpopulation(EnsPGvpopulationgenotype gvpg,
                                              EnsPGvpopulation gvp);

AjBool ensGvpopulationgenotypeSetGvvariation(EnsPGvpopulationgenotype gvpg,
                                             EnsPGvvariation gvv);

AjBool ensGvpopulationgenotypeSetAllele1(EnsPGvpopulationgenotype gvpg,
                                         AjPStr allele1);

AjBool ensGvpopulationgenotypeSetAllele2(EnsPGvpopulationgenotype gvpg,
                                         AjPStr allele2);

AjBool ensGvpopulationgenotypeSetSubidentifier(EnsPGvpopulationgenotype gvpg,
                                               ajuint subidentifier);

AjBool ensGvpopulationgenotypeSetFrequency(EnsPGvpopulationgenotype gvpg,
                                           float frequency);

ajulong ensGvpopulationgenotypeGetMemsize(const EnsPGvpopulationgenotype gvpg);

AjBool ensGvpopulationgenotypeTrace(const EnsPGvpopulationgenotype gvpg,
                                    ajuint level);

/* Ensembl Genetic Variation Population Genotype Adaptor */

EnsPGvpopulationgenotypeadaptor ensRegistryGetGvpopulationgenotypeadaptor(
    EnsPDatabaseadaptor dba);

EnsPGvpopulationgenotypeadaptor ensGvpopulationgenotypeadaptorNew(
    EnsPDatabaseadaptor dba);

void ensGvpopulationgenotypeadaptorDel(EnsPGvpopulationgenotypeadaptor* Pgvpga);

/*
** End of prototype definitions
*/




#endif /* ensvariation_h */

#ifdef __cplusplus
}
#endif
