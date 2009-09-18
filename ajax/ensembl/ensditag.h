#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensditag_h
#define ensditag_h

#include "ensfeature.h"




/* @data EnsPDiTag ************************************************************
**
** Ensembl DiTag
**
** @alias EnsSDiTag
** @alias EnsODiTag
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPDatabaseAdaptor] Ensembl Database Adaptor
** @cc Bio::EnsEMBL::Map::Ditag
** @attr Name [AjPStr] Name
** @attr Type [AjPStr] Source
** @attr Sequence [AjPStr] Sequence
** @attr Count [ajuint] Count
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSDiTag
{
    ajuint Use;
    ajuint Identifier;
    EnsPDatabaseAdaptor Adaptor;
    AjPStr Name;
    AjPStr Type;
    AjPStr Sequence;
    ajuint Count;
    ajuint Padding;
} EnsODiTag;

#define EnsPDiTag EnsODiTag*




/* @data EnsPDiTagFeatureAdaptor **********************************************
**
** Ensembl DiTag Feature Adaptor.
**
** @alias EnsSDiTagFeatureAdaptor
** @alias EnsODiTagFeatureAdaptor
**
** @cc Bio::EnsEMBL::Map::DBSQL::DiTagFeatureAdaptor
** @attr Adaptor [EnsPFeatureAdaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSDiTagFeatureAdaptor
{
    EnsPFeatureAdaptor Adaptor;
} EnsODiTagFeatureAdaptor;

#define EnsPDiTagFeatureAdaptor EnsODiTagFeatureAdaptor*




/* @datatype EnsEDiTagFeatureSide *********************************************
**
** Ensembl DiTag Feature Side enumeration.
**
** @attr enum [EnsEDiTagFeatureSide] Value
** @@
******************************************************************************/

enum EnsEDiTagFeatureSide
{
    ensEDiTagFeatureSideNULL,
    ensEDiTagFeatureSideLeft,
    ensEDiTagFeatureSideRight,
    ensEDiTagFeatureSideFull
};




/* @data EnsPDiTagFeature *****************************************************
**
** Ensembl DiTag Feature
**
** @alias EnsSDiTagFeature
** @alias EnsODiTagFeature
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPDiTagFeatureAdaptor] Ensembl DiTag Feature Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::Map::DitagFeature
** @attr DiTag [EnsPDiTag] DiTag
** @attr Cigar [AjPStr] CIGAR line
** @attr Side [AjEnum] Side
** @attr TargetStart [ajint] Target start
** @attr TargetEnd [ajint] Target end
** @attr TargetStrand [ajint] Target strand
** @attr PairIdentifier [ajuint] Pair identifier
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSDiTagFeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPDiTagFeatureAdaptor Adaptor;
    EnsPFeature Feature;
    EnsPDiTag DiTag;
    AjPStr Cigar;
    AjEnum Side;
    ajint TargetStart;
    ajint TargetEnd;
    ajint TargetStrand;
    ajuint PairIdentifier;
    ajuint Padding;
} EnsODiTagFeature;

#define EnsPDiTagFeature EnsODiTagFeature*




/*
** Prototype definitions
*/

/* Ensembl DiTag */

EnsPDiTag ensDiTagNew(EnsPDatabaseAdaptor adaptor,
                      ajuint identifier,
                      AjPStr name,
                      AjPStr type,
                      AjPStr sequence,
                      ajuint count);

EnsPDiTag ensDiTagNewObj(const EnsPDiTag object);

EnsPDiTag ensDiTagNewRef(EnsPDiTag dt);

void ensDiTagDel(EnsPDiTag* Pdt);

EnsPDatabaseAdaptor ensDiTagGetAdaptor(const EnsPDiTag dt);

ajuint ensDiTagGetIdentifier(const EnsPDiTag dt);

AjPStr ensDiTagGetName(const EnsPDiTag dt);

AjPStr ensDiTagGetType(const EnsPDiTag dt);

AjPStr ensDiTagGetSequence(const EnsPDiTag dt);

ajuint ensDiTagGetCount(const EnsPDiTag dt);

AjBool ensDiTagSetAdaptor(EnsPDiTag dt, EnsPDatabaseAdaptor adaptor);

AjBool ensDiTagSetIdentifier(EnsPDiTag dt, ajuint identifier);

AjBool ensDiTagSetName(EnsPDiTag dt, AjPStr name);

AjBool ensDiTagSetType(EnsPDiTag dt, AjPStr type);

AjBool ensDiTagSetSequence(EnsPDiTag dt, AjPStr sequence);

AjBool ensDiTagSetCount(EnsPDiTag dt, ajuint count);

AjBool ensDiTagTrace(const EnsPDiTag dt, ajuint level);

ajuint ensDiTagGetMemSize(const EnsPDiTag dt);

/* Ensembl DiTag Adaptor */

AjBool ensDiTagAdaptorFetchByIdentifier(EnsPDatabaseAdaptor dba,
                                        ajuint identifier,
                                        EnsPDiTag *Pdt);

AjBool ensDiTagAdaptorFetchAll(EnsPDatabaseAdaptor dba,
                               const AjPStr name,
                               const AjPStr type,
                               AjPList dts);

AjBool ensDiTagAdaptorFetchAllByName(EnsPDatabaseAdaptor dba,
                                     const AjPStr name,
                                     AjPList dts);

AjBool ensDiTagAdaptorFetchAllByType(EnsPDatabaseAdaptor dba,
                                     const AjPStr type,
                                     AjPList dts);

/* Ensembl DiTag Feature */

EnsPDiTagFeature ensDiTagFeatureNew(EnsPDiTagFeatureAdaptor adaptor,
                                    ajuint identifier,
                                    EnsPFeature feature,
                                    EnsPDiTag dt,
                                    AjPStr cigar,
                                    AjEnum side,
                                    ajint tstart,
                                    ajint tend,
                                    ajint tstrand,
                                    ajuint pairid);

EnsPDiTagFeature ensDiTagFeatureNewObj(const EnsPDiTagFeature object);

EnsPDiTagFeature ensDiTagFeatureNewRef(EnsPDiTagFeature dtf);

void ensDiTagFeatureDel(EnsPDiTagFeature* Pdtf);

EnsPDiTagFeatureAdaptor ensDiTagFeatureGetAdaptor(const EnsPDiTagFeature dtf);

ajuint ensDiTagFeatureGetIdentifier(const EnsPDiTagFeature dtf);

EnsPFeature ensDiTagFeatureGetFeature(const EnsPDiTagFeature dtf);

EnsPDiTag ensDiTagFeatureGetDiTag(const EnsPDiTagFeature dtf);

AjPStr ensDiTagFeatureGetCigar(const EnsPDiTagFeature dtf);

AjEnum ensDiTagFeatureGetSide(const EnsPDiTagFeature dtf);

ajint ensDiTagFeatureGetTargetStart(const EnsPDiTagFeature dtf);

ajint ensDiTagFeatureGetTargetEnd(const EnsPDiTagFeature dtf);

ajint ensDiTagFeatureGetTargetStrand(const EnsPDiTagFeature dtf);

ajuint ensDiTagFeatureGetPairIdentifier(const EnsPDiTagFeature dtf);

AjBool ensDiTagFeatureSetAdaptor(EnsPDiTagFeature dtf,
                                 EnsPDiTagFeatureAdaptor adaptor);

AjBool ensDiTagFeatureSetIdentifier(EnsPDiTagFeature dtf, ajuint identifier);

AjBool ensDiTagFeatureSetFeature(EnsPDiTagFeature dtf, EnsPFeature feature);

AjBool ensDiTagFeatureSetDiTag(EnsPDiTagFeature dtf, EnsPDiTag dt);

AjBool ensDiTagFeatureSetCigar(EnsPDiTagFeature dtf, AjPStr cigar);

AjBool ensDiTagFeatureSetSide(EnsPDiTagFeature dtf, AjEnum side);

AjBool ensDiTagFeatureSetTargetStart(EnsPDiTagFeature dtf, ajint tstart);

AjBool ensDiTagFeatureSetTargetEnd(EnsPDiTagFeature dtf, ajint tend);

AjBool ensDiTagFeatureSetTargetStrand(EnsPDiTagFeature dtf, ajint tstrand);

AjBool ensDiTagFeatureSetPairIdentifier(EnsPDiTagFeature dtf, ajuint pairid);

AjBool ensDiTagFeatureTrace(const EnsPDiTagFeature dtf, ajuint level);

ajuint ensDiTagFeatureGetMemSize(const EnsPDiTagFeature dtf);

AjEnum ensDiTagFeatureSideFromStr(const AjPStr side);

const char *ensDiTagFeatureSideToChar(const AjEnum side);

/* Ensembl DiTag Feature Adaptor */

EnsPDiTagFeatureAdaptor ensDiTagFeatureAdaptorNew(EnsPDatabaseAdaptor dba);

void ensDiTagFeatureAdaptorDel(EnsPDiTagFeatureAdaptor *Padaptor);

AjBool ensDiTagAdaptorFetchAllByIdentifier(EnsPDiTagFeatureAdaptor adaptor,
                                           ajuint identifier,
                                           EnsPDiTagFeature *Pdtf);

AjBool ensDiTagFeatureAdaptorFetchAllByDiTag(EnsPDiTagFeatureAdaptor adaptor,
                                             const EnsPDiTag dt,
                                             AjPList dtfs);

AjBool ensDiTagFeatureAdaptorFetchAllByType(EnsPDiTagFeatureAdaptor adaptor,
                                            const AjPStr type,
                                            AjPList dtfs);

AjBool ensDiTagFeatureAdaptorFetchAllBySlice(EnsPDiTagFeatureAdaptor adaptor,
                                             EnsPSlice slice,
                                             const AjPStr type,
                                             const AjPStr anname,
                                             AjPList dtfs);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
