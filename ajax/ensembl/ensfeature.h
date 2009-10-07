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




/* @data EnsPDNAAlignFeatureadaptor *******************************************
**
** Ensembl DNA Align Feature Adaptor.
**
** @alias EnsSDNAAlignFeatureadaptor
** @alias EnsODNAAlignFeatureadaptor
**
** @attr Adaptor [EnsPFeatureadaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSDNAAlignFeatureadaptor
{
    EnsPFeatureadaptor Adaptor;
} EnsODNAAlignFeatureadaptor;

#define EnsPDNAAlignFeatureadaptor EnsODNAAlignFeatureadaptor*




/* @data EnsPProteinAlignFeatureadaptor ***************************************
**
** Ensembl Protein Align Feature Adaptor.
**
** @alias EnsSProteinAlignFeatureadaptor
** @alias EnsOProteinAlignFeatureadaptor
**
** @attr Adaptor [EnsPFeatureadaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSProteinAlignFeatureadaptor
{
    EnsPFeatureadaptor Adaptor;
} EnsOProteinAlignFeatureadaptor;

#define EnsPProteinAlignFeatureadaptor EnsOProteinAlignFeatureadaptor*




/******************************************************************************
**
** Ensembl Base Align Feature Type enumeration.
**
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
** @attr DNAAlignFeatureadaptor [EnsPDNAAlignFeatureadaptor]
**                              Ensembl DNA Align Feature Adaptor
** @attr ProteinAlignFeatureadaptor [EnsPProteinAlignFeatureadaptor]
**                                  Ensembl Protein Align Feature Adaptor
** @cc Bio::EnsEMBL::FeaturePair
** @attr FeaturePair [EnsPFeaturePair] Ensembl Feature Pair
** @cc Bio::EnsEMBL::BaseAlignFeature
** @attr GetFeaturePair [(EnsPFeaturePair*)] Get Ensembl Feature Pair function
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
    EnsPDNAAlignFeatureadaptor DNAAlignFeatureadaptor;
    EnsPProteinAlignFeatureadaptor ProteinAlignFeatureadaptor;
    EnsPFeaturePair FeaturePair;
    EnsPFeaturePair (*GetFeaturePair)(void* object);
    AjPStr Cigar;
    AjEnum Type;
    ajuint AlignmentLength;
    ajuint PairDNAAlignFeatureIdentifier;
    ajuint Padding;
} EnsOBaseAlignFeature;

#define EnsPBaseAlignFeature EnsOBaseAlignFeature*




/* @data EnsPProteinFeatureadaptor ********************************************
**
** Ensembl Protein Feature Adaptor.
**
** @alias EnsSProteinFeatureadaptor
** @alias EnsOProteinFeatureadaptor
**
** @attr Adaptor [EnsPBaseadaptor] Ensembl Base Adaptor
** @@
******************************************************************************/

typedef struct EnsSProteinFeatureadaptor
{
    EnsPBaseadaptor Adaptor;
} EnsOProteinFeatureadaptor;

#define EnsPProteinFeatureadaptor EnsOProteinFeatureadaptor*




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
** @attr Adaptor [EnsPProteinFeatureadaptor] Ensembl Protein Feature Adaptor
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
    EnsPProteinFeatureadaptor Adaptor;
    EnsPFeaturePair FeaturePair;
    AjPStr Accession;
    AjPStr Description;
} EnsOProteinFeature;

#define EnsPProteinFeature EnsOProteinFeature*




/* @data EnsPSimpleFeatureadaptor *********************************************
**
** Ensembl Simple Feature Adaptor.
**
** @alias EnsSSimpleFeatureadaptor
** @alias EnsOSimpleFeatureadaptor
**
** @attr Adaptor [EnsPFeatureadaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSSimpleFeatureadaptor
{
    EnsPFeatureadaptor Adaptor;
} EnsOSimpleFeatureadaptor;

#define EnsPSimpleFeatureadaptor EnsOSimpleFeatureadaptor*




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
** @attr Adaptor [EnsPSimpleFeatureadaptor] Ensembl Simple Feature Adaptor
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
    EnsPSimpleFeatureadaptor Adaptor;
    EnsPFeature Feature;
    AjPStr DisplayLabel;
    double Score;
} EnsOSimpleFeature;

#define EnsPSimpleFeature EnsOSimpleFeature*




/* @data EnsPAssemblyExceptionFeatureadaptor **********************************
**
** Ensembl Assembly Exception Feature Adaptor.
**
** @alias EnsSAssemblyExceptionFeatureadaptor
** @alias EnsOAssemblyExceptionFeatureadaptor
**
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @attr Cache [AjPList] AJAX List of Ensembl Assembly Exception Features
** @attr CacheByIdentifier [AjPTable] AJAX Table of Ensembl Assembly Exception
**                                    Features
** @attr CacheBySlice [EnsPCache] Ensembl Cache by Ensembl Slice names
** @@
******************************************************************************/

typedef struct EnsSAssemblyExceptionFeatureadaptor
{
    EnsPDatabaseadaptor Adaptor;
    AjPList Cache;
    AjPTable CacheByIdentifier;
    EnsPCache CacheBySlice;
} EnsOAssemblyExceptionFeatureadaptor;

#define EnsPAssemblyExceptionFeatureadaptor EnsOAssemblyExceptionFeatureadaptor*




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
** @attr Adaptor [EnsPAssemblyExceptionFeatureadaptor]
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
    EnsPAssemblyExceptionFeatureadaptor Adaptor;
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

EnsPFeatureadaptor ensFeatureadaptorNew(
    EnsPDatabaseadaptor dba,
    const char **Ptables,
    const char **Pcolumns,
    EnsOBaseadaptorLeftJoin *left_join,
    const char *condition,
    const char *final,
    AjBool Fquery(EnsPDatabaseadaptor dba,
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

void ensFeatureadaptorDel(EnsPFeatureadaptor *Padaptor);

EnsPBaseadaptor ensFeatureadaptorGetBaseadaptor(
    const EnsPFeatureadaptor adaptor);

EnsPCache ensFeatureadaptorGetCache(const EnsPFeatureadaptor adaptor);

ajuint ensFeatureadaptorGetMaxFeatureLength(const EnsPFeatureadaptor adaptor);

EnsPDatabaseadaptor ensFeatureadaptorGetDatabaseadaptor(
    const EnsPFeatureadaptor adaptor);

AjBool ensFeatureadaptorSetMaximumFeatureLength(EnsPFeatureadaptor adaptor,
                                                ajuint length);

AjBool ensFeatureadaptorSetTables(EnsPFeatureadaptor adaptor,
                                  const char **Ptables);

AjBool ensFeatureadaptorSetColumns(EnsPFeatureadaptor adaptor,
                                   const char **Pcolumns);

AjBool ensFeatureadaptorSetDefaultCondition(EnsPFeatureadaptor adaptor,
                                            const char *condition);

AjBool ensFeatureadaptorSetFinalCondition(EnsPFeatureadaptor adaptor,
                                          const char *final);

AjBool ensFeatureadaptorEscapeC(EnsPFeatureadaptor adaptor,
                                 char **Ptxt,
                                 const AjPStr str);

AjBool ensFeatureadaptorEscapeS(EnsPFeatureadaptor adaptor,
                                 AjPStr *Pstr,
                                 const AjPStr str);

AjBool ensFeatureadaptorFetchAllBySliceConstraint(EnsPFeatureadaptor adaptor,
                                                  EnsPSlice slice,
                                                  const AjPStr constraint,
                                                  const AjPStr anname,
                                                  AjPList objects);

AjBool ensFeatureadaptorFetchAllBySlice(EnsPFeatureadaptor adaptor,
                                        EnsPSlice slice,
                                        const AjPStr anname,
                                        AjPList objects);

AjBool ensFeatureadaptorFetchAllBySliceScore(EnsPFeatureadaptor adaptor,
                                             EnsPSlice slice,
                                             double score,
                                             const AjPStr anname,
                                             AjPList objects);

AjBool ensFeatureadaptorFetchAllByAnalysisName(EnsPFeatureadaptor adaptor,
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
    EnsPDNAAlignFeatureadaptor adaptor,
    ajuint identifier,
    EnsPFeaturePair fp,
    AjPStr cigar,
    ajuint pair);

EnsPBaseAlignFeature ensBaseAlignFeatureNewP(
    EnsPProteinAlignFeatureadaptor adaptor,
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

EnsPDNAAlignFeatureadaptor ensBaseAlignFeatureGetDNAAlignFeatureadaptor(
    const EnsPBaseAlignFeature baf);

EnsPProteinAlignFeatureadaptor ensBaseAlignFeatureGetProteinAlignFeatureadaptor(
    const EnsPBaseAlignFeature baf);

ajuint ensBaseAlignFeatureGetIdentifier(const EnsPBaseAlignFeature baf);

EnsPFeaturePair ensBaseAlignFeatureGetFeaturePair(
    const EnsPBaseAlignFeature baf);

AjBool ensBaseAlignFeatureSetFeaturePair(EnsPBaseAlignFeature baf,
                                         EnsPFeaturePair fp);

EnsPFeature ensBaseAlignFeatureGetFeature(const EnsPBaseAlignFeature baf);

ajuint ensBaseAlignFeatureGetAlignmentLength(EnsPBaseAlignFeature baf);

void *ensBaseAlignFeatureGetadaptor(const EnsPBaseAlignFeature baf);

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

EnsPDNAAlignFeatureadaptor ensDNAAlignFeatureadaptorNew(
    EnsPDatabaseadaptor dba);

void ensDNAAlignFeatureadaptorDel(EnsPDNAAlignFeatureadaptor *Padaptor);

AjBool ensDNAAlignFeatureadaptorFetchByIdentifier(
    EnsPDNAAlignFeatureadaptor adaptor,
    ajuint identifier,
    EnsPBaseAlignFeature *Pbaf);

AjBool ensDNAAlignFeatureadaptorFetchAllBySliceIdentity(
    EnsPDNAAlignFeatureadaptor adaptor,
    EnsPSlice slice,
    float identity,
    const AjPStr anname,
    AjPList bafs);

AjBool ensDNAAlignFeatureadaptorFetchAllBySliceCoverage(
    EnsPDNAAlignFeatureadaptor adaptor,
    EnsPSlice slice,
    float coverage,
    const AjPStr anname,
    AjPList bafs);

AjBool ensDNAAlignFeatureadaptorFetchAllBySliceExternalDatabase(
    EnsPDNAAlignFeatureadaptor adaptor,
    EnsPSlice slice,
    const AjPStr edbname,
    const AjPStr anname,
    AjPList bafs);

AjBool ensDNAAlignFeatureadaptorFetchAllByHitName(
    EnsPDNAAlignFeatureadaptor adaptor,
    const AjPStr hitname,
    const AjPStr anname,
    AjPList bafs);

AjBool ensDNAAlignFeatureadaptorFetchAllByHitUnversioned(
    EnsPDNAAlignFeatureadaptor adaptor,
    const AjPStr hitname,
    const AjPStr anname,
    AjPList bafs);

AjBool ensDNAAlignFeatureadaptorFetchAllIdentifiers(
    EnsPDNAAlignFeatureadaptor adaptor,
    AjPList idlist);

/* Ensembl Protein Align Feature Adaptor */

EnsPProteinAlignFeatureadaptor ensProteinAlignFeatureadaptorNew(
    EnsPDatabaseadaptor dba);

void ensProteinAlignFeatureadaptorDel(EnsPProteinAlignFeatureadaptor *Padaptor);

AjBool ensProteinAlignFeatureadaptorFetchByIdentifier(
    EnsPProteinAlignFeatureadaptor adaptor,
    ajuint identifier,
    EnsPBaseAlignFeature *Pbaf);

AjBool ensProteinAlignFeatureadaptorFetchAllBySliceIdentity(
    EnsPProteinAlignFeatureadaptor adaptor,
    EnsPSlice slice,
    float identity,
    const AjPStr anname,
    AjPList bafs);

AjBool ensProteinAlignFeatureadaptorFetchAllBySliceCoverage(
    EnsPProteinAlignFeatureadaptor adaptor,
    EnsPSlice slice,
    float coverage,
    const AjPStr anname,
    AjPList bafs);

AjBool ensProteinAlignFeatureadaptorFetchAllBySliceExternalDatabase(
    EnsPProteinAlignFeatureadaptor adaptor,
    EnsPSlice slice,
    const AjPStr edbname,
    const AjPStr anname,
    AjPList bafs);

AjBool ensProteinAlignFeatureadaptorFetchAllByHitName(
    EnsPProteinAlignFeatureadaptor adaptor,
    const AjPStr hitname,
    const AjPStr anname,
    AjPList bafs);

AjBool ensProteinAlignFeatureadaptorFetchAllByHitUnversioned(
    EnsPProteinAlignFeatureadaptor adaptor,
    const AjPStr hitname,
    const AjPStr anname,
    AjPList bafs);

AjBool ensProteinAlignFeatureadaptorFetchAllIdentifiers(
    EnsPProteinAlignFeatureadaptor adaptor,
    AjPList idlist);

/* Ensembl Protein Feature */

EnsPProteinFeature ensProteinFeatureNew(
    EnsPProteinFeatureadaptor adaptor,
    ajuint identifier,
    EnsPFeaturePair fp,
    AjPStr accession,
    AjPStr description);

EnsPProteinFeature ensProteinFeatureNewObj(const EnsPProteinFeature object);

EnsPProteinFeature ensProteinFeatureNewRef(EnsPProteinFeature pf);

void ensProteinFeatureDel(EnsPProteinFeature *Ppf);

EnsPProteinFeatureadaptor ensProteinFeatureGetadaptor(
    const EnsPProteinFeature pf);

ajuint ensProteinFeatureGetIdentifier(const EnsPProteinFeature pf);

EnsPFeaturePair ensProteinFeatureGetFeaturePair(const EnsPProteinFeature pf);

AjPStr ensProteinFeatureGetAccession(const EnsPProteinFeature pf);

AjPStr ensProteinFeatureGetDescription(const EnsPProteinFeature pf);

AjBool ensProteinFeatureSetadaptor(EnsPProteinFeature pf,
                                   EnsPProteinFeatureadaptor adaptor);

AjBool ensProteinFeatureSetIdentifier(EnsPProteinFeature pf, ajuint identifier);

AjBool ensProteinFeatureSetFeaturePair(EnsPProteinFeature pf,
                                       EnsPFeaturePair fp);

AjBool ensProteinFeatureSetAccession(EnsPProteinFeature pf, AjPStr accession);

AjBool ensProteinFeatureSetDescription(EnsPProteinFeature pf,
                                       AjPStr description);

AjBool ensProteinFeatureTrace(const EnsPProteinFeature pf, ajuint level);

ajuint ensProteinFeatureGetMemSize(const EnsPProteinFeature pf);

/* Ensembl Protein Feature Adaptor */

EnsPProteinFeatureadaptor ensProteinFeatureadaptorNew(EnsPDatabaseadaptor dba);

void ensProteinFeatureadaptorDel(EnsPProteinFeatureadaptor *Padaptor);

AjBool ensProteinFeatureadaptorFetchAllByTranslationIdentifier(
    EnsPProteinFeatureadaptor adaptor,
    ajuint tlid,
    AjPList pfs);

AjBool ensProteinFeatureadaptorFetchByIdentifier(
    EnsPProteinFeatureadaptor adaptor,
    ajuint identifier,
    EnsPProteinFeature *Ppf);

/* Ensembl Simple Feature */

EnsPSimpleFeature ensSimpleFeatureNew(EnsPSimpleFeatureadaptor adaptor,
                                      ajuint identifier,
                                      EnsPFeature feature,
                                      AjPStr label,
                                      double score);

EnsPSimpleFeature ensSimpleFeatureNewObj(const EnsPSimpleFeature object);

EnsPSimpleFeature ensSimpleFeatureNewRef(EnsPSimpleFeature sf);

void ensSimpleFeatureDel(EnsPSimpleFeature* Psf);

EnsPSimpleFeatureadaptor ensSimpleFeatureGetadaptor(const EnsPSimpleFeature sf);

ajuint ensSimpleFeatureGetIdentifier(const EnsPSimpleFeature sf);

EnsPFeature ensSimpleFeatureGetFeature(const EnsPSimpleFeature sf);

AjPStr ensSimpleFeatureGetDisplayLabel(const EnsPSimpleFeature sf);

double ensSimpleFeatureGetScore(const EnsPSimpleFeature sf);

AjBool ensSimpleFeatureSetadaptor(EnsPSimpleFeature sf,
                                  EnsPSimpleFeatureadaptor adaptor);

AjBool ensSimpleFeatureSetIdentifier(EnsPSimpleFeature sf, ajuint identifier);

AjBool ensSimpleFeatureSetFeature(EnsPSimpleFeature sf, EnsPFeature feature);

AjBool ensSimpleFeatureSetDisplayLabel(EnsPSimpleFeature sf, AjPStr label);

AjBool ensSimpleFeatureSetScore(EnsPSimpleFeature sf, double score);

AjBool ensSimpleFeatureTrace(const EnsPSimpleFeature sf, ajuint level);

ajuint ensSimpleFeatureGetMemSize(const EnsPSimpleFeature sf);

/* Ensembl Simple Feature Adaptor */

EnsPSimpleFeatureadaptor ensSimpleFeatureadaptorNew(EnsPDatabaseadaptor dba);

void ensSimpleFeatureadaptorDel(EnsPSimpleFeatureadaptor *Padaptor);

AjBool ensSimpleFeatureadaptorFetchAllBySlice(EnsPSimpleFeatureadaptor adaptor,
                                              EnsPSlice slice,
                                              const AjPStr anname,
                                              AjPList sfs);

AjBool ensSimpleFeatureadaptorFetchAllBySliceScore(
    EnsPSimpleFeatureadaptor adaptor,
    EnsPSlice slice,
    double score,
    const AjPStr anname,
    AjPList sfs);

AjBool ensSimpleFeatureadaptorFetchAllByAnalysisName(
    EnsPSimpleFeatureadaptor adaptor,
    AjPStr anname,
    AjPList sfs);

/* Ensembl Assembly Exception Feature */

EnsPAssemblyExceptionFeature ensAssemblyExceptionFeatureNew(
    EnsPAssemblyExceptionFeatureadaptor adaptor,
    ajuint identifier,
    EnsPFeature feature,
    EnsPSlice slice,
    AjEnum type);

EnsPAssemblyExceptionFeature ensAssemblyExceptionFeatureNewObj(
    const EnsPAssemblyExceptionFeature object);

EnsPAssemblyExceptionFeature ensAssemblyExceptionFeatureNewRef(
    EnsPAssemblyExceptionFeature aef);

void ensAssemblyExceptionFeatureDel(EnsPAssemblyExceptionFeature* Paef);

EnsPAssemblyExceptionFeatureadaptor ensAssemblyExceptionFeatureGetadaptor(
    const EnsPAssemblyExceptionFeature aef);

ajuint ensAssemblyExceptionFeatureGetIdentifier(
    const EnsPAssemblyExceptionFeature aef);

EnsPFeature ensAssemblyExceptionFeatureGetFeature(
    const EnsPAssemblyExceptionFeature aef);

EnsPSlice ensAssemblyExceptionFeatureGetAlternateSlice(
    const EnsPAssemblyExceptionFeature aef);

AjEnum ensAssemblyExceptionFeatureGetType(
    const EnsPAssemblyExceptionFeature aef);

AjBool ensAssemblyExceptionFeatureSetadaptor(
    EnsPAssemblyExceptionFeature aef,
    EnsPAssemblyExceptionFeatureadaptor adaptor);

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

EnsPAssemblyExceptionFeatureadaptor ensAssemblyExceptionFeatureadaptorNew(
    EnsPDatabaseadaptor dba);

void ensAssemblyExceptionFeatureadaptorDel(
    EnsPAssemblyExceptionFeatureadaptor* Padaptor);

AjBool ensAssemblyExceptionFeatureadaptorFetchAll(
    EnsPAssemblyExceptionFeatureadaptor adaptor,
    AjPList aefs);

AjBool ensAssemblyExceptionFeatureadaptorFetchByIdentifier(
    EnsPAssemblyExceptionFeatureadaptor adaptor,
    ajuint identifier,
    EnsPAssemblyExceptionFeature* Paef);

AjBool ensAssemblyExceptionFeatureadaptorFetchAllBySlice(
    EnsPAssemblyExceptionFeatureadaptor adaptor,
    EnsPSlice slice,
    AjPList aefs);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
