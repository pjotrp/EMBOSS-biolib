#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensdensity_h
#define ensdensity_h

#include "ensfeature.h"




/* @data EnsPDensityTypeadaptor ***********************************************
**
** Ensembl Density Type Adaptor.
**
** @alias EnsSDensityTypeadaptor
** @alias EnsODensityTypeadaptor
**
** @attr Adaptor [EnsPBaseadaptor] Ensembl Base Adaptor
** @attr CacheByIdentifier [AjPTable] Identifier cache
** @@
******************************************************************************/

typedef struct EnsSDensityTypeadaptor
{
    EnsPBaseadaptor Adaptor;
    AjPTable CacheByIdentifier;
} EnsODensityTypeadaptor;

#define EnsPDensityTypeadaptor EnsODensityTypeadaptor*




/******************************************************************************
**
** Ensembl Density Type value type enumeration.
**
******************************************************************************/

enum EnsEDensityTypeValueType
{
    ensEDensityTypeValueTypeNULL,
    ensEDensityTypeValueTypeSum,
    ensEDensityTypeValueTypeRatio
};




/* @data EnsPDensityType ******************************************************
**
** Ensembl Density Type.
**
** @alias EnsSDensityType
** @alias EnsODensityType
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPDensityTypeadaptor] Ensembl Density Type Adaptor
** @cc Bio::EnsEMBL::DensityType
** @attr Analysis [EnsPAnalysis] Ensembl Analysis
** @attr ValueType [AjEnum] Value type enumeration
** @attr BlockSize [ajuint] Block size
** @attr RegionFeatures [ajuint] Number of Features per Sequence Region in
**                               this Density Type
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSDensityType
{
    ajuint Use;
    ajuint Identifier;
    EnsPDensityTypeadaptor Adaptor;
    EnsPAnalysis Analysis;
    AjEnum ValueType;
    ajuint BlockSize;
    ajuint RegionFeatures;
    ajuint Padding;
} EnsODensityType;

#define EnsPDensityType EnsODensityType*




/* @data EnsPDensityFeatureadaptor ********************************************
**
** Ensembl Density Feature Adaptor.
**
** @alias EnsSDensityFeatureadaptor
** @alias EnsODensityFeatureadaptor
**
** @attr Adaptor [EnsPFeatureadaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSDensityFeatureadaptor
{
    EnsPFeatureadaptor Adaptor;
} EnsODensityFeatureadaptor;

#define EnsPDensityFeatureadaptor EnsODensityFeatureadaptor*




/* @data EnsPDensityFeature ***************************************************
**
** Ensembl Density Feature.
**
** @alias EnsSDensityFeature
** @alias EnsODensityFeature
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPDensityFeatureadaptor] Ensembl Density Feature Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::DensityFeature
** @attr DensityType [EnsPDensityType] Ensembl Density Type
** @attr DensityValue [float] Density value
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSDensityFeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPDensityFeatureadaptor Adaptor;
    EnsPFeature Feature;
    EnsPDensityType DensityType;
    float DensityValue;
    ajuint Padding;
} EnsODensityFeature;

#define EnsPDensityFeature EnsODensityFeature*




/*
** Prototype definitions
*/

/* Ensembl Density Type */

EnsPDensityType ensDensityTypeNew(EnsPDensityTypeadaptor adaptor,
                                  ajuint identifier,
                                  EnsPAnalysis analysis,
                                  AjEnum type,
                                  ajuint size,
                                  ajuint features);

EnsPDensityType ensDensityTypeNewObj(const EnsPDensityType object);

EnsPDensityType ensDensityTypeNewRef(EnsPDensityType dt);

void ensDensityTypeDel(EnsPDensityType* Pdt);

EnsPDensityTypeadaptor ensDensityTypeGetadaptor(const EnsPDensityType dt);

ajuint ensDensityTypeGetIdentifier(const EnsPDensityType dt);

EnsPAnalysis ensDensityTypeGetAnalysis(const EnsPDensityType dt);

AjEnum ensDensityTypeGetValueType(const EnsPDensityType dt);

ajuint ensDensityTypeGetBlockSize(const EnsPDensityType dt);

ajuint ensDensityTypeGetRegionFeatures(const EnsPDensityType dt);

AjBool ensDensityTypeSetadaptor(EnsPDensityType dt,
                                EnsPDensityTypeadaptor adaptor);

AjBool ensDensityTypeSetIdentifier(EnsPDensityType dt, ajuint identifier);

AjBool ensDensityTypeSetAnalysis(EnsPDensityType dt, EnsPAnalysis analysis);

AjBool ensDensityTypeSetValueType(EnsPDensityType dt, AjEnum type);

AjBool ensDensityTypeSetBlockSize(EnsPDensityType dt, ajuint size);

AjBool ensDensityTypeSetRegionFeatures(EnsPDensityType dt, ajuint features);

AjBool ensDensityTypeTrace(const EnsPDensityType dt, ajuint level);

ajuint ensDensityTypeGetMemSize(const EnsPDensityType dt);

AjEnum ensDensityTypeValeTypeFromStr(const AjPStr type);

const char* ensDensityTypeValeTypeToChar(const AjEnum type);

/* Ensembl Density Type Adaptor */

EnsPDensityTypeadaptor ensDensityTypeadaptorNew(EnsPDatabaseadaptor dba);

void ensDensityTypeadaptorDel(EnsPDensityTypeadaptor* Padaptor);

EnsPBaseadaptor ensDensityTypeadaptorGetBaseadaptor(
    const EnsPDensityTypeadaptor adaptor);

EnsPDatabaseadaptor ensDensityTypeadaptorGetDatabaseadaptor(
    const EnsPDensityTypeadaptor adaptor);

AjBool ensDensityTypeadaptorFetchAll(EnsPDensityTypeadaptor adaptor,
                                     AjPList dts);

AjBool ensDensityTypeadaptorFetchByIdentifier(EnsPDensityTypeadaptor adaptor,
                                              ajuint identifier,
                                              EnsPDensityType *Pdt);

AjBool ensDensityTypeadaptorFetchAllByAnalysisName(
    EnsPDensityTypeadaptor adaptor,
    const AjPStr name,
    AjPList dts);

/* Ensembl Density Feature */

EnsPDensityFeature ensDensityFeatureNew(EnsPDensityFeatureadaptor adaptor,
                                        ajuint identifier,
                                        EnsPFeature feature,
                                        EnsPDensityType dt,
                                        float value);

EnsPDensityFeature ensDensityFeatureNewObj(const EnsPDensityFeature object);

EnsPDensityFeature ensDensityFeatureNewRef(EnsPDensityFeature df);

void ensDensityFeatureDel(EnsPDensityFeature* Pdf);

EnsPDensityFeatureadaptor ensDensityFeatureGetadaptor(
    const EnsPDensityFeature df);

ajuint ensDensityFeatureGetIdentifier(const EnsPDensityFeature df);

EnsPFeature ensDensityFeatureGetFeature(const EnsPDensityFeature df);

EnsPDensityType ensDensityFeatureGetDensityType(const EnsPDensityFeature df);

float ensDensityFeatureGetDensityValue(const EnsPDensityFeature df);

AjBool ensDensityFeatureSetadaptor(EnsPDensityFeature df,
                                   EnsPDensityFeatureadaptor adaptor);

AjBool ensDensityFeatureSetIdentifier(EnsPDensityFeature df, ajuint identifier);

AjBool ensDensityFeatureSetFeature(EnsPDensityFeature df, EnsPFeature feature);

AjBool ensDensityFeatureSetDensityType(EnsPDensityFeature df,
                                       EnsPDensityType dt);

AjBool ensDensityFeatureSetDensityValue(EnsPDensityFeature df, float value);

AjBool ensDensityFeatureTrace(const EnsPDensityFeature df, ajuint level);

ajuint ensDensityFeatureGetMemSize(const EnsPDensityFeature df);

EnsPDensityFeatureadaptor ensDensityFeatureadaptorNew(EnsPDatabaseadaptor dba);

void ensDensityFeatureadaptorDel(EnsPDensityFeatureadaptor *Padaptor);

AjBool ensDensityFeatureadaptorFetchAllBySlice(
    EnsPDensityFeatureadaptor adaptor,
    EnsPSlice slice,
    const AjPStr anname,
    ajuint blocks,
    AjBool interpolate,
    float maxratio,
    AjPList dfs);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
