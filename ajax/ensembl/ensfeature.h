#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensfeature_h
#define ensfeature_h

#include "ensdata.h"
#include "ensassemblyexception.h"
#include "ensdatabaseentry.h"
#include "ensmetainformation.h"
#include "ensmetacoordinate.h"
#include "ensslice.h"




/* @data EnsPFeaturePair ******************************************************
**
** Ensembl Feature Pair.
**
** @alias EnsSFeaturePair
** @alias EnsOFeaturePair
**
** @cc Bio::EnsEMBL::FeaturePair
** @attr SourceFeature [EnsPFeature] Source Ensembl Feature (Query)
** @attr TargetFeature [EnsPFeature] Target Ensembl Feature (Target)
** @attr ExternalDatabase [EnsPExternalDatabase] Ensembl External Database
** @attr ExtraData [AjPStr] Extra data
** @attr SourceSpecies [AjPStr] Source species name
** @attr TargetSpecies [AjPStr] Target species name
** @attr GroupIdentifier [ajuint] Group identifier
** @attr LevelIdentifier [ajuint] Level identifier
** @attr Evalue [double] e- or p-value
** @attr Score [double] Score
** @attr SourceCoverage [float] Source coverage in percent
** @attr TargetCoverage [float] Target coverage in percent
** @attr SequenceIdentity [float] Sequence identity in percent
** @attr Use [ajuint] Use counter
** @@
******************************************************************************/

typedef struct EnsSFeaturePair
{
    EnsPFeature SourceFeature;
    EnsPFeature TargetFeature;
    EnsPExternalDatabase ExternalDatabase;
    AjPStr ExtraData;
    AjPStr SourceSpecies;
    AjPStr TargetSpecies;
    ajuint GroupIdentifier;
    ajuint LevelIdentifier;
    double Evalue;
    double Score;
    float SourceCoverage;
    float TargetCoverage;
    float SequenceIdentity;
    ajuint Use;
} EnsOFeaturePair;

#define EnsPFeaturePair EnsOFeaturePair*




/* @data EnsPDNAAlignFeatureAdaptor *******************************************
**
** Ensembl DNA Align Feature Adaptor.
**
** @alias EnsSDNAAlignFeatureAdaptor
** @alias EnsODNAAlignFeatureAdaptor
**
** @attr Adaptor [EnsPFeatureAdaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSDNAAlignFeatureAdaptor
{
    EnsPFeatureAdaptor Adaptor;
} EnsODNAAlignFeatureAdaptor;

#define EnsPDNAAlignFeatureAdaptor EnsODNAAlignFeatureAdaptor*




/* @data EnsPProteinAlignFeatureAdaptor ***************************************
**
** Ensembl Protein Align Feature Adaptor.
**
** @alias EnsSProteinAlignFeatureAdaptor
** @alias EnsOProteinAlignFeatureAdaptor
**
** @attr Adaptor [EnsPFeatureAdaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSProteinAlignFeatureAdaptor
{
    EnsPFeatureAdaptor Adaptor;
} EnsOProteinAlignFeatureAdaptor;

#define EnsPProteinAlignFeatureAdaptor EnsOProteinAlignFeatureAdaptor*




/* @datatype EnsEBaseAlignFeatureType *****************************************
**
** Ensembl Base Align Feature Type enumeration.
**
** @attr enum [EnsEBaseAlignFeatureType] Value
** @@
******************************************************************************/

enum EnsEBaseAlignFeatureType
{
    ensEBaseAlignFeatureTypeNULL,
    ensEBaseAlignFeatureTypeDNA,
    ensEBaseAlignFeatureTypeProtein
};




/* @data EnsPBaseAlignFeature *************************************************
**
** Ensembl Base Align Feature.
**
** @alias EnsSBaseAlignFeature
** @alias EnsOBaseAlignFeature
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr DNAAlignFeatureAdaptor [EnsPDNAAlignFeatureAdaptor]
**                              Ensembl DNA Align Feature Adaptor
** @attr ProteinAlignFeatureAdaptor [EnsPDNAAlignFeatureAdaptor]
**                                  Ensembl Protein Align Feature Adaptor
** @cc Bio::EnsEMBL::FeaturePair
** @attr FeaturePair [EnsPFeaturePair] Ensembl Feature Pair
** @cc Bio::EnsEMBL::BaseAlignFeature
** @attr GetFeaturePair [(EnsPFeaturePair)] Get Ensembl Feature Pair function
** @attr Cigar [AjPStr] CIGAR line
** @attr Type [AjEnum] Type (ensEBaseAlignFeatureTypeDNA, ...)
** @attr AlignmentLength [ajuint] Target component alignment length
** @attr PairDNAAlignFeatureIdentifier [ajuint] Pair DNA Align Feature
**                                              identifier
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSBaseAlignFeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPDNAAlignFeatureAdaptor DNAAlignFeatureAdaptor;
    EnsPProteinAlignFeatureAdaptor ProteinAlignFeatureAdaptor;
    EnsPFeaturePair FeaturePair;
    EnsPFeaturePair (*GetFeaturePair)(void* object);
    AjPStr Cigar;
    AjEnum Type;
    ajuint AlignmentLength;
    ajuint PairDNAAlignFeatureIdentifier;
    ajuint Padding;
} EnsOBaseAlignFeature;

#define EnsPBaseAlignFeature EnsOBaseAlignFeature*




/* @data EnsPProteinFeatureAdaptor ********************************************
**
** Ensembl Protein Feature Adaptor.
**
** @alias EnsSProteinFeatureAdaptor
** @alias EnsOProteinFeatureAdaptor
**
** @attr Adaptor [EnsPBaseAdaptor] Ensembl Base Adaptor
** @@
******************************************************************************/

typedef struct EnsSProteinFeatureAdaptor
{
    EnsPBaseAdaptor Adaptor;
} EnsOProteinFeatureAdaptor;

#define EnsPProteinFeatureAdaptor EnsOProteinFeatureAdaptor*




/* @data EnsPProteinFeature ***************************************************
**
** Ensembl Protein Feature.
**
** @alias EnsSProteinFeature
** @alias EnsOProteinFeature
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPProteinFeatureAdaptor] Ensembl Protein Feature Adaptor
** @cc Bio::EnsEMBL::FeaturePair
** @attr FeaturePair [EnsPFeaturePair] Ensembl Feature Pair
** @cc Bio::EnsEMBL::ProteinFeature
** @attr Accession [AjPStr] (InterPro) Accession
** @attr Description [AjPStr] (InterPro) Description
** @@
******************************************************************************/

typedef struct EnsSProteinFeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPProteinFeatureAdaptor Adaptor;
    EnsPFeaturePair FeaturePair;
    AjPStr Accession;
    AjPStr Description;
} EnsOProteinFeature;

#define EnsPProteinFeature EnsOProteinFeature*




/* @data EnsPSimpleFeatureAdaptor *********************************************
**
** Ensembl Simple Feature Adaptor.
**
** @alias EnsSSimpleFeatureAdaptor
** @alias EnsOSimpleFeatureAdaptor
**
** @attr Adaptor [EnsPFeatureAdaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSSimpleFeatureAdaptor
{
    EnsPFeatureAdaptor Adaptor;
} EnsOSimpleFeatureAdaptor;

#define EnsPSimpleFeatureAdaptor EnsOSimpleFeatureAdaptor*




/* @data EnsPSimpleFeature ****************************************************
**
** Ensembl Simple Feature.
**
** @alias EnsSSimpleFeature
** @alias EnsOSimpleFeature
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPSimpleFeatureAdaptor] Ensembl Simple Feature Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::SimpleFeature
** @attr DisplayLabel [AjPStr] Display label
** @attr Score [double] Score
** @@
******************************************************************************/

typedef struct EnsSSimpleFeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPSimpleFeatureAdaptor Adaptor;
    EnsPFeature Feature;
    AjPStr DisplayLabel;
    double Score;
} EnsOSimpleFeature;

#define EnsPSimpleFeature EnsOSimpleFeature*




/* @data EnsPAssemblyExceptionFeatureAdaptor **********************************
**
** Ensembl Assembly Exception Feature Adaptor.
**
** @alias EnsSAssemblyExceptionFeatureAdaptor
** @alias EnsOAssemblyExceptionFeatureAdaptor
**
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @attr Cache [AjList] AJAX List of Ensembl Assembly Exception Features
** @attr CacheByIdentifier [AjPTable] AJAX Table of Ensembl Assembly Exception
**                                    Features
** @attr CacheBySlice [EnsPCache] Ensembl Cache by Ensembl Slice names
** @@
******************************************************************************/

typedef struct EnsSAssemblyExceptionFeatureAdaptor
{
    EnsPDatabaseAdaptor Adaptor;
    AjPList Cache;
    AjPTable CacheByIdentifier;
    EnsPCache CacheBySlice;
} EnsOAssemblyExceptionFeatureAdaptor;

#define EnsPAssemblyExceptionFeatureAdaptor EnsOAssemblyExceptionFeatureAdaptor*




/* @data EnsPAssemblyExceptionFeature *****************************************
**
** Ensembl Assembly Exception Feature.
**
** @alias EnsSAssemblyExceptionFeature
** @alias EnsOAssemblyExceptionFeature
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPAssemblyExceptionFeatureAdaptor]
**                Ensembl Assembly Exception Feature Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::AssemblyExceptionFeature
** @attr AlternateSlice [EnsPSlice] Alternate Ensembl Slice
** @attr Type [AjEnum] Ensembl Assembly Exception Feature type
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSAssemblyExceptionFeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPAssemblyExceptionFeatureAdaptor Adaptor;
    EnsPFeature Feature;
    EnsPSlice AlternateSlice;
    AjEnum Type;
    ajuint Padding;
} EnsOAssemblyExceptionFeature;

#define EnsPAssemblyExceptionFeature EnsOAssemblyExceptionFeature*




/*
** Prototype definitions
*/

/* Ensembl Feature */

EnsPFeature ensFeatureNewS(EnsPAnalysis analysis,
                           EnsPSlice slice,
                           ajint start,
                           ajint end,
                           ajint strand);

EnsPFeature ensFeatureNewN(EnsPAnalysis analysis,
                           AjPStr seqname,
                           ajint start,
                           ajint end,
                           ajint strand);

EnsPFeature ensFeatureNewObj(const EnsPFeature object);

EnsPFeature ensFeatureNewRef(EnsPFeature feature);

void ensFeatureDel(EnsPFeature* Pfeature);

EnsPAnalysis ensFeatureGetAnalysis(const EnsPFeature feature);

EnsPSlice ensFeatureGetSlice(const EnsPFeature feature);

AjPStr ensFeatureGetSequenceName(const EnsPFeature feature);

ajint ensFeatureGetStart(const EnsPFeature feature);

ajint ensFeatureGetEnd(const EnsPFeature feature);

ajint ensFeatureGetStrand(const EnsPFeature feature);

AjBool ensFeatureSetAnalysis(EnsPFeature feature, EnsPAnalysis analysis);

AjBool ensFeatureSetSlice(EnsPFeature feature, EnsPSlice slice);

AjBool ensFeatureSetSequenceName(EnsPFeature, AjPStr seqname);

AjBool ensFeatureSetStart(EnsPFeature feature, ajint start);

AjBool ensFeatureSetEnd(EnsPFeature feature, ajint end);

AjBool ensFeatureSetStrand(EnsPFeature feature, ajint strand);

AjBool ensFeatureTrace(const EnsPFeature feature, ajuint level);

ajuint ensFeatureGetLength(const EnsPFeature feature);

ajuint ensFeatureGetMemSize(const EnsPFeature feature);

const EnsPSeqRegion ensFeatureGetSeqRegion(const EnsPFeature feature);

ajint ensFeatureGetSeqRegionStart(const EnsPFeature feature);

ajint ensFeatureGetSeqRegionEnd(const EnsPFeature feature);

ajint ensFeatureGetSeqRegionStrand(const EnsPFeature feature);

const AjPStr ensFeatureGetSeqRegionName(const EnsPFeature feature);

ajuint ensFeatureGetSeqRegionLength(const EnsPFeature feature);

AjBool ensFeatureMove(EnsPFeature feature, ajint start, ajint end,
                      ajint strand);

AjBool ensFeatureProject(const EnsPFeature feature,
                         const AjPStr csname,
                         const AjPStr csversion,
                         AjPList pslist);

EnsPFeature ensFeatureTransform(EnsPFeature feature,
                                const AjPStr csname,
                                const AjPStr csversion);

EnsPFeature ensFeatureTransfer(EnsPFeature feature, EnsPSlice slice);

AjBool ensFeatureFetchAllAlternativeLocations(EnsPFeature feature,
                                              AjBool all,
                                              AjPList features);

AjBool ensFeatureOverlap(const EnsPFeature feature1,
                         const EnsPFeature feature2);

AjBool ensFeatureFetchSeqRegionName(const EnsPFeature feature, AjPStr* Pname);

/* Ensembl Feature Adaptor */

EnsPFeatureAdaptor ensFeatureAdaptorNew(
    EnsPDatabaseAdaptor dba,
    const char **Ptables,
    const char **Pcolumns,
    EnsOBaseAdaptorLeftJoin *left_join,
    const char *condition,
    const char *final,
    AjBool Fquery(EnsPDatabaseAdaptor dba,
                  const AjPStr statement,
                  EnsPAssemblyMapper mapper,
                  EnsPSlice slice,
                  AjPList objects),
    void* Fread(const void* key),
    void* Freference(void* value),
    AjBool Fwrite(const void* value),
    void Fdelete(void** value),
    ajuint Fsize(const void* value),
    EnsPFeature Fgetfeature(const void *object),
    const char *label);

void ensFeatureAdaptorDel(EnsPFeatureAdaptor *Padaptor);

EnsPBaseAdaptor ensFeatureAdaptorGetBaseAdaptor(
    const EnsPFeatureAdaptor adaptor);

EnsPCache ensFeatureAdaptorGetCache(const EnsPFeatureAdaptor adaptor);

ajuint ensFeatureAdaptorGetMaxFeatureLength(const EnsPFeatureAdaptor adaptor);

EnsPDatabaseAdaptor ensFeatureAdaptorGetDatabaseAdaptor(
    const EnsPFeatureAdaptor adaptor);

AjBool ensFeatureAdaptorSetMaximumFeatureLength(EnsPFeatureAdaptor adaptor,
                                                ajuint length);

AjBool ensFeatureAdaptorSetTables(EnsPFeatureAdaptor adaptor,
                                  const char **Ptables);

AjBool ensFeatureAdaptorSetColumns(EnsPFeatureAdaptor adaptor,
                                   const char **Pcolumns);

AjBool ensFeatureAdaptorSetDefaultCondition(EnsPFeatureAdaptor adaptor,
                                            const char *condition);

AjBool ensFeatureAdaptorSetFinalCondition(EnsPFeatureAdaptor adaptor,
                                          const char *final);

AjBool ensFeatureAdaptorEscapeCS(EnsPFeatureAdaptor adaptor,
                                 char **Ptxt,
                                 const AjPStr str);

AjBool ensFeatureAdaptorEscapeSS(EnsPFeatureAdaptor adaptor,
                                 AjPStr *Pstr,
                                 const AjPStr str);

AjBool ensFeatureAdaptorFetchAllBySliceConstraint(EnsPFeatureAdaptor adaptor,
                                                  EnsPSlice slice,
                                                  const AjPStr constraint,
                                                  const AjPStr anname,
                                                  AjPList objects);

AjBool ensFeatureAdaptorFetchAllBySlice(EnsPFeatureAdaptor adaptor,
                                        EnsPSlice slice,
                                        const AjPStr anname,
                                        AjPList objects);

AjBool ensFeatureAdaptorFetchAllBySliceScore(EnsPFeatureAdaptor adaptor,
                                             EnsPSlice slice,
                                             double score,
                                             const AjPStr anname,
                                             AjPList objects);

AjBool ensFeatureAdaptorFetchAllByAnalysisName(EnsPFeatureAdaptor adaptor,
                                               AjPStr anname,
                                               AjPList objects);

/* Ensembl Feature Pair */

EnsPFeaturePair ensFeaturePairNew(EnsPFeature srcfeature,
                                  EnsPFeature trgfeature,
                                  EnsPExternalDatabase edb,
                                  AjPStr extra,
                                  AjPStr srcspecies,
                                  AjPStr trgspecies,
                                  ajuint groupid,
                                  ajuint levelid,
                                  double evalue,
                                  double score,
                                  float srccoverage,
                                  float trgcoverage,
                                  float identity);

EnsPFeaturePair ensFeaturePairNewObj(const EnsPFeaturePair object);

EnsPFeaturePair ensFeaturePairNewRef(EnsPFeaturePair fp);

void ensFeaturePairDel(EnsPFeaturePair* Pfp);

EnsPFeature ensFeaturePairGetSourceFeature(const EnsPFeaturePair fp);

EnsPFeature ensFeaturePairGetTargetFeature(const EnsPFeaturePair fp);

EnsPExternalDatabase ensFeaturePairGetExternalDatabase(
    const EnsPFeaturePair fp);

AjPStr ensFeaturePairGetExtraData(const EnsPFeaturePair fp);

AjPStr ensFeaturePairGetSourceSpecies(const EnsPFeaturePair fp);

AjPStr ensFeaturePairGetTargetSpecies(const EnsPFeaturePair fp);

ajuint ensFeaturePairGetGroupIdentifier(const EnsPFeaturePair fp);

ajuint ensFeaturePairGetLevelIdentifier(const EnsPFeaturePair fp);

double ensFeaturePairGetEvalue(const EnsPFeaturePair fp);

double ensFeaturePairGetScore(const EnsPFeaturePair fp);

float ensFeaturePairGetSourceCoverage(const EnsPFeaturePair fp);

float ensFeaturePairGetTargetCoverage(const EnsPFeaturePair fp);

float ensFeaturePairGetSequenceIdentity(const EnsPFeaturePair fp);

AjBool ensFeaturePairSetSourceFeature(EnsPFeaturePair fp, EnsPFeature feature);

AjBool ensFeaturePairSetTargetFeature(EnsPFeaturePair fp, EnsPFeature feature);

AjBool ensFeaturePairSetExternalDatabase(EnsPFeaturePair fp,
                                         EnsPExternalDatabase edb);

AjBool ensFeaturePairSetExtraData(EnsPFeaturePair fp, AjPStr extra);

AjBool ensFeaturePairSetSourceSpecies(EnsPFeaturePair fp, AjPStr species);

AjBool ensFeaturePairSetTargetSpecies(EnsPFeaturePair fp, AjPStr species);

AjBool ensFeaturePairSetGroupIdentifier(EnsPFeaturePair fp, ajint groupid);

AjBool ensFeaturePairSetLevelIdentifier(EnsPFeaturePair fp, ajint levelid);

AjBool ensFeaturePairSetEvalue(EnsPFeaturePair fp, double evalue);

AjBool ensFeaturePairSetScore(EnsPFeaturePair fp, double score);

AjBool ensFeaturePairSetSourceCoverage(EnsPFeaturePair fp, float coverage);

AjBool ensFeaturePairSetTargetCoverage(EnsPFeaturePair fp, float coverage);

AjBool ensFeaturePairTrace(const EnsPFeaturePair fp, ajuint level);

AjBool ensFeaturePairSetSequenceIdentity(EnsPFeaturePair fp, float identity);

ajuint ensFeaturePairGetMemSize(const EnsPFeaturePair fp);

EnsPFeaturePair ensFeaturePairTransform(EnsPFeaturePair fp,
                                        const AjPStr csname,
                                        const AjPStr csversion);

EnsPFeaturePair ensFeaturePairTransfer(EnsPFeaturePair fp, EnsPSlice slice);

/* Ensembl Base Align Feature */

EnsPBaseAlignFeature ensBaseAlignFeatureNewD(
    EnsPDNAAlignFeatureAdaptor adaptor,
    ajuint identifier,
    EnsPFeaturePair fp,
    AjPStr cigar,
    ajuint pair);

EnsPBaseAlignFeature ensBaseAlignFeatureNewP(
    EnsPProteinAlignFeatureAdaptor adaptor,
    ajuint identifier,
    EnsPFeaturePair fp,
    AjPStr cigar);

EnsPBaseAlignFeature ensBaseAlignFeatureNew(
    EnsPFeaturePair fp,
    AjPStr cigar,
    AjPList fps,
    AjEnum type,
    ajuint pair);

EnsPBaseAlignFeature ensBaseAlignFeatureNewObj(
    const EnsPBaseAlignFeature object);

EnsPBaseAlignFeature ensBaseAlignFeatureNewRef(EnsPBaseAlignFeature baf);

void ensBaseAlignFeatureDel(EnsPBaseAlignFeature *Pbaf);

EnsPDNAAlignFeatureAdaptor ensBaseAlignFeatureGetDNAAlignFeatureAdaptor(
    const EnsPBaseAlignFeature baf);

EnsPProteinAlignFeatureAdaptor ensBaseAlignFeatureGetProteinAlignFeatureAdaptor(
    const EnsPBaseAlignFeature baf);

ajuint ensBaseAlignFeatureGetIdentifier(const EnsPBaseAlignFeature baf);

EnsPFeaturePair ensBaseAlignFeatureGetFeaturePair(
    const EnsPBaseAlignFeature baf);

AjBool ensBaseAlignFeatureSetFeaturePair(EnsPBaseAlignFeature baf,
                                         EnsPFeaturePair fp);

EnsPFeature ensBaseAlignFeatureGetFeature(const EnsPBaseAlignFeature baf);

ajuint ensBaseAlignFeatureGetAlignmentLength(EnsPBaseAlignFeature baf);

void *ensBaseAlignFeatureGetAdaptor(const EnsPBaseAlignFeature baf);

ajuint ensBaseAlignFeatureGetSourceUnit(const EnsPBaseAlignFeature baf);

ajuint ensBaseAlignFeatureGetTargetUnit(const EnsPBaseAlignFeature baf);

ajuint ensBaseAlignFeatureGetMemSize(const EnsPBaseAlignFeature baf);

EnsPBaseAlignFeature ensBaseAlignFeatureTransform(
    EnsPBaseAlignFeature baf,
    const AjPStr csname,
    const AjPStr csversion);

EnsPBaseAlignFeature ensBaseAlignFeatureTransfer(EnsPBaseAlignFeature baf,
                                                 EnsPSlice slice);

AjBool ensBaseAlignFeatureTrace(const EnsPBaseAlignFeature baf, ajuint level);

AjBool ensBaseAlignFeatureFetchAllFeaturePairs(EnsPBaseAlignFeature baf,
                                               AjPList fps);

/* Ensembl DNA Align Feature Adaptor */

EnsPDNAAlignFeatureAdaptor ensDNAAlignFeatureAdaptorNew(
    EnsPDatabaseAdaptor dba);

void ensDNAAlignFeatureAdaptorDel(EnsPDNAAlignFeatureAdaptor *Padaptor);

AjBool ensDNAAlignFeatureAdaptorFetchByIdentifier(
    EnsPDNAAlignFeatureAdaptor adaptor,
    ajuint identifier,
    EnsPBaseAlignFeature *Pbaf);

AjBool ensDNAAlignFeatureAdaptorFetchAllBySliceIdentity(
    EnsPDNAAlignFeatureAdaptor adaptor,
    EnsPSlice slice,
    float identity,
    const AjPStr anname,
    AjPList bafs);

AjBool ensDNAAlignFeatureAdaptorFetchAllBySliceCoverage(
    EnsPDNAAlignFeatureAdaptor adaptor,
    EnsPSlice slice,
    float coverage,
    const AjPStr anname,
    AjPList bafs);

AjBool ensDNAAlignFeatureAdaptorFetchAllBySliceExternalDatabase(
    EnsPDNAAlignFeatureAdaptor adaptor,
    EnsPSlice slice,
    const AjPStr edbname,
    const AjPStr anname,
    AjPList bafs);

AjBool ensDNAAlignFeatureAdaptorFetchAllByHitName(
    EnsPDNAAlignFeatureAdaptor adaptor,
    const AjPStr hitname,
    const AjPStr anname,
    AjPList bafs);

AjBool ensDNAAlignFeatureAdaptorFetchAllByHitUnversioned(
    EnsPDNAAlignFeatureAdaptor adaptor,
    const AjPStr hitname,
    const AjPStr anname,
    AjPList bafs);

AjBool ensDNAAlignFeatureAdaptorFetchAllIdentifiers(
    EnsPDNAAlignFeatureAdaptor adaptor,
    AjPList idlist);

/* Ensembl Protein Align Feature Adaptor */

EnsPProteinAlignFeatureAdaptor ensProteinAlignFeatureAdaptorNew(
    EnsPDatabaseAdaptor dba);

void ensProteinAlignFeatureAdaptorDel(EnsPProteinAlignFeatureAdaptor *Padaptor);

AjBool ensProteinAlignFeatureAdaptorFetchByIdentifier(
    EnsPProteinAlignFeatureAdaptor adaptor,
    ajuint identifier,
    EnsPBaseAlignFeature *Pbaf);

AjBool ensProteinAlignFeatureAdaptorFetchAllBySliceIdentity(
    EnsPProteinAlignFeatureAdaptor adaptor,
    EnsPSlice slice,
    float identity,
    const AjPStr anname,
    AjPList bafs);

AjBool ensProteinAlignFeatureAdaptorFetchAllBySliceCoverage(
    EnsPProteinAlignFeatureAdaptor adaptor,
    EnsPSlice slice,
    float coverage,
    const AjPStr anname,
    AjPList bafs);

AjBool ensProteinAlignFeatureAdaptorFetchAllBySliceExternalDatabase(
    EnsPProteinAlignFeatureAdaptor adaptor,
    EnsPSlice slice,
    const AjPStr edbname,
    const AjPStr anname,
    AjPList bafs);

AjBool ensProteinAlignFeatureAdaptorFetchAllByHitName(
    EnsPProteinAlignFeatureAdaptor adaptor,
    const AjPStr hitname,
    const AjPStr anname,
    AjPList bafs);

AjBool ensProteinAlignFeatureAdaptorFetchAllByHitUnversioned(
    EnsPProteinAlignFeatureAdaptor adaptor,
    const AjPStr hitname,
    const AjPStr anname,
    AjPList bafs);

AjBool ensProteinAlignFeatureAdaptorFetchAllIdentifiers(
    EnsPProteinAlignFeatureAdaptor adaptor,
    AjPList idlist);

/* Ensembl Protein Feature */

EnsPProteinFeature ensProteinFeatureNew(
    EnsPProteinFeatureAdaptor adaptor,
    ajuint identifier,
    EnsPFeaturePair fp,
    AjPStr accession,
    AjPStr description);

EnsPProteinFeature ensProteinFeatureNewObj(const EnsPProteinFeature object);

EnsPProteinFeature ensProteinFeatureNewRef(EnsPProteinFeature pf);

void ensProteinFeatureDel(EnsPProteinFeature *Ppf);

EnsPProteinFeatureAdaptor ensProteinFeatureGetAdaptor(
    const EnsPProteinFeature pf);

ajuint ensProteinFeatureGetIdentifier(const EnsPProteinFeature pf);

EnsPFeaturePair ensProteinFeatureGetFeaturePair(const EnsPProteinFeature pf);

AjPStr ensProteinFeatureGetAccession(const EnsPProteinFeature pf);

AjPStr ensProteinFeatureGetDescription(const EnsPProteinFeature pf);

AjBool ensProteinFeatureSetAdaptor(EnsPProteinFeature pf,
                                   EnsPProteinFeatureAdaptor adaptor);

AjBool ensProteinFeatureSetIdentifier(EnsPProteinFeature pf, ajuint identifier);

AjBool ensProteinFeatureSetFeaturePair(EnsPProteinFeature pf,
                                       EnsPFeaturePair fp);

AjBool ensProteinFeatureSetAccession(EnsPProteinFeature pf, AjPStr accession);

AjBool ensProteinFeatureSetDescription(EnsPProteinFeature pf,
                                       AjPStr description);

AjBool ensProteinFeatureTrace(const EnsPProteinFeature pf, ajuint level);

ajuint ensProteinFeatureGetMemSize(const EnsPProteinFeature pf);

/* Ensembl Protein Feature Adaptor */

EnsPProteinFeatureAdaptor ensProteinFeatureAdaptorNew(EnsPDatabaseAdaptor dba);

void ensProteinFeatureAdaptorDel(EnsPProteinFeatureAdaptor *Padaptor);

AjBool ensProteinFeatureAdaptorFetchAllByTranslationIdentifier(
    EnsPProteinFeatureAdaptor adaptor,
    ajuint tlid,
    AjPList pfs);

AjBool ensProteinFeatureAdaptorFetchByIdentifier(
    EnsPProteinFeatureAdaptor adaptor,
    ajuint identifier,
    EnsPProteinFeature *Ppf);

/* Ensembl Simple Feature */

EnsPSimpleFeature ensSimpleFeatureNew(EnsPSimpleFeatureAdaptor adaptor,
                                      ajuint identifier,
                                      EnsPFeature feature,
                                      AjPStr label,
                                      double score);

EnsPSimpleFeature ensSimpleFeatureNewObj(const EnsPSimpleFeature object);

EnsPSimpleFeature ensSimpleFeatureNewRef(EnsPSimpleFeature sf);

void ensSimpleFeatureDel(EnsPSimpleFeature* Psf);

EnsPSimpleFeatureAdaptor ensSimpleFeatureGetAdaptor(const EnsPSimpleFeature sf);

ajuint ensSimpleFeatureGetIdentifier(const EnsPSimpleFeature sf);

EnsPFeature ensSimpleFeatureGetFeature(const EnsPSimpleFeature sf);

AjPStr ensSimpleFeatureGetDisplayLabel(const EnsPSimpleFeature sf);

double ensSimpleFeatureGetScore(const EnsPSimpleFeature sf);

AjBool ensSimpleFeatureSetAdaptor(EnsPSimpleFeature sf,
                                  EnsPSimpleFeatureAdaptor adaptor);

AjBool ensSimpleFeatureSetIdentifier(EnsPSimpleFeature sf, ajuint identifier);

AjBool ensSimpleFeatureSetFeature(EnsPSimpleFeature sf, EnsPFeature feature);

AjBool ensSimpleFeatureSetDisplayLabel(EnsPSimpleFeature sf, AjPStr label);

AjBool ensSimpleFeatureSetScore(EnsPSimpleFeature sf, double score);

AjBool ensSimpleFeatureTrace(const EnsPSimpleFeature sf, ajuint level);

ajuint ensSimpleFeatureGetMemSize(const EnsPSimpleFeature sf);

/* Ensembl Simple Feature Adaptor */

EnsPSimpleFeatureAdaptor ensSimpleFeatureAdaptorNew(EnsPDatabaseAdaptor dba);

void ensSimpleFeatureAdaptorDel(EnsPSimpleFeatureAdaptor *Padaptor);

AjBool ensSimpleFeatureAdaptorFetchAllBySlice(EnsPSimpleFeatureAdaptor adaptor,
                                              EnsPSlice slice,
                                              const AjPStr anname,
                                              AjPList sfs);

AjBool ensSimpleFeatureAdaptorFetchAllBySliceScore(
    EnsPSimpleFeatureAdaptor adaptor,
    EnsPSlice slice,
    double score,
    const AjPStr anname,
    AjPList sfs);

AjBool ensSimpleFeatureAdaptorFetchAllByAnalysisName(
    EnsPSimpleFeatureAdaptor adaptor,
    AjPStr anname,
    AjPList sfs);

/* Ensembl Assembly Exception Feature */

EnsPAssemblyExceptionFeature ensAssemblyExceptionFeatureNew(
    EnsPAssemblyExceptionFeatureAdaptor adaptor,
    ajuint identifier,
    EnsPFeature feature,
    EnsPSlice slice,
    AjEnum type);

EnsPAssemblyExceptionFeature ensAssemblyExceptionFeatureNewObj(
    const EnsPAssemblyExceptionFeature object);

EnsPAssemblyExceptionFeature ensAssemblyExceptionFeatureNewRef(
    EnsPAssemblyExceptionFeature aef);

void ensAssemblyExceptionFeatureDel(EnsPAssemblyExceptionFeature* Paef);

EnsPAssemblyExceptionFeatureAdaptor ensAssemblyExceptionFeatureGetAdaptor(
    const EnsPAssemblyExceptionFeature aef);

ajuint ensAssemblyExceptionFeatureGetIdentifier(
    const EnsPAssemblyExceptionFeature aef);

EnsPFeature ensAssemblyExceptionFeatureGetFeature(
    const EnsPAssemblyExceptionFeature aef);

EnsPSlice ensAssemblyExceptionFeatureGetAlternateSlice(
    const EnsPAssemblyExceptionFeature aef);

AjEnum ensAssemblyExceptionFeatureGetType(
    const EnsPAssemblyExceptionFeature aef);

AjBool ensAssemblyExceptionFeatureSetAdaptor(
    EnsPAssemblyExceptionFeature aef,
    EnsPAssemblyExceptionFeatureAdaptor adaptor);

AjBool ensAssemblyExceptionFeatureSetIdentifier(
    EnsPAssemblyExceptionFeature aef,
    ajuint identifier);

AjBool ensAssemblyExceptionFeatureSetFeature(EnsPAssemblyExceptionFeature aef,
                                             EnsPFeature feature);

AjBool ensAssemblyExceptionFeatureSetAlternateSlice(
    EnsPAssemblyExceptionFeature aef,
    EnsPSlice altslice);

AjBool ensAssemblyExceptionFeatureSetType(EnsPAssemblyExceptionFeature aef,
                                          AjEnum type);

AjBool ensAssemblyExceptionFeatureFetchDisplayIdentifier(
    const EnsPAssemblyExceptionFeature aef,
    AjPStr *Pidentifier);

AjBool ensAssemblyExceptionFeatureTrace(const EnsPAssemblyExceptionFeature aef,
                                        ajuint level);

/* Ensembl Assembly Exception Feature Adaptor */

EnsPAssemblyExceptionFeatureAdaptor ensAssemblyExceptionFeatureAdaptorNew(
    EnsPDatabaseAdaptor dba);

void ensAssemblyExceptionFeatureAdaptorDel(
    EnsPAssemblyExceptionFeatureAdaptor* Padaptor);

AjBool ensAssemblyExceptionFeatureAdaptorFetchAll(
    EnsPAssemblyExceptionFeatureAdaptor adaptor,
    AjPList aefs);

AjBool ensAssemblyExceptionFeatureAdaptorFetchByIdentifier(
    EnsPAssemblyExceptionFeatureAdaptor adaptor,
    ajuint identifier,
    EnsPAssemblyExceptionFeature* Paef);

AjBool ensAssemblyExceptionFeatureAdaptorFetchAllBySlice(
    EnsPAssemblyExceptionFeatureAdaptor adaptor,
    EnsPSlice slice,
    AjPList aefs);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
