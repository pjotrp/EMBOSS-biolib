#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensrepeat_h
#define ensrepeat_h

#include "ensfeature.h"
#include "enstable.h"




/******************************************************************************
**
** Ensembl Repeat Mask Type enumeration.
**
******************************************************************************/

enum EnsERepeatMaskType
{
    ensERepeatMaskTypeNULL,
    ensERepeatMaskTypeNone,
    ensERepeatMaskTypeSoft,
    ensERepeatMaskTypeHard
};




/* @data EnsPRepeatConsensus **************************************************
**
** Ensembl Repeat Consensus.
**
** @alias EnsSRepeatConsensus
** @alias EnsORepeatConsensus
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] The SQL database-internal identifier
** @attr Adaptor [EnsPDatabaseadaptor] Ensembl Database Adaptor
** @cc Bio::EnsEMBL::RepeatConsensus
** @attr Name [AjPStr] Repeat Consensus name
** @attr Class [AjPStr] Repeat Consensus class
** @attr Type [AjPStr] Repeat Consensus type
** @attr Consensus [AjPStr] Repeat Consensus sequence
** @attr Length [ajuint] Repeat Consensus sequence length
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSRepeatConsensus
{
    ajuint Use;
    ajuint Identifier;
    EnsPDatabaseadaptor Adaptor;
    AjPStr Name;
    AjPStr Class;
    AjPStr Type;
    AjPStr Consensus;
    ajuint Length;
    ajuint Padding;
} EnsORepeatConsensus;

#define EnsPRepeatConsensus EnsORepeatConsensus*




/* @data EnsPRepeatFeatureadaptor *********************************************
**
** Ensembl Repeat Feature Adaptor.
**
** @alias EnsSRepeatFeatureadaptor
** @alias EnsORepeatFeatureadaptor
**
** @cc Bio::EnsEMBL::DBSQL::BaseFeatureadaptor
** @attr Adaptor [EnsPFeatureadaptor] Ensembl Feature Adaptor
** @@
******************************************************************************/

typedef struct EnsSRepeatFeatureadaptor
{
    EnsPFeatureadaptor Adaptor;
} EnsORepeatFeatureadaptor;

#define EnsPRepeatFeatureadaptor EnsORepeatFeatureadaptor*




/* @data EnsPRepeatFeature ****************************************************
**
** Ensembl Repeat Feature.
**
** @alias EnsSRepeatFeature
** @alias EnsORepeatFeature
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] The SQL database-internal identifier
** @attr Adaptor [EnsPRepeatFeatureadaptor] Ensembl Repeat Feature Adaptor
** @cc Bio::EnsEMBL::RepeatFeature
** @attr Feature [EnsPFeature] Ensembl Feature
** @attr RepeatConsensus [EnsPRepeatConsensus] Ensembl Repeat Consensus
** @attr HitStart [ajint] The hit start on he consensus sequence
** @attr HitEnd [ajint] The hit end on the consensus sequence
** @attr Score [double] Score
** @@
******************************************************************************/

typedef struct EnsSRepeatFeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPRepeatFeatureadaptor Adaptor;
    EnsPFeature Feature;
    EnsPRepeatConsensus RepeatConsensus;
    ajint HitStart;
    ajint HitEnd;
    double Score;
} EnsORepeatFeature;

#define EnsPRepeatFeature EnsORepeatFeature*




/*
** Prototype definitions
*/

/* Ensembl Repeat Consensus */

EnsPRepeatConsensus ensRepeatConsensusNew(EnsPDatabaseadaptor adaptor,
                                          ajuint identifier,
                                          AjPStr name,
                                          AjPStr class,
                                          AjPStr type,
                                          AjPStr consensus,
                                          ajuint length);

EnsPRepeatConsensus ensRepeatConsensusNewObj(EnsPRepeatConsensus object);

EnsPRepeatConsensus ensRepeatConsensusNewRef(EnsPRepeatConsensus rc);

void ensRepeatConsensusDel(EnsPRepeatConsensus* Prc);

EnsPDatabaseadaptor ensRepeatConsensusGetadaptor(const EnsPRepeatConsensus rc);

ajuint ensRepeatConsensusGetIdentifier(const EnsPRepeatConsensus rc);

AjPStr ensRepeatConsensusGetName(const EnsPRepeatConsensus rc);

AjPStr ensRepeatConsensusGetClass(const EnsPRepeatConsensus rc);

AjPStr ensRepeatConsensusGetType(const EnsPRepeatConsensus rc);

AjPStr ensRepeatConsensusGetConsensus(const EnsPRepeatConsensus rc);

ajuint ensRepeatConsensusGetLength(const EnsPRepeatConsensus rc);

AjBool ensRepeatConsensusSetadaptor(EnsPRepeatConsensus rc,
                                    EnsPDatabaseadaptor dba);

AjBool ensRepeatConsensusSetIdentifier(EnsPRepeatConsensus rc,
                                       ajuint identifier);

AjBool ensRepeatConsensusSetName(EnsPRepeatConsensus rc, AjPStr name);

AjBool ensRepeatConsensusSetClass(EnsPRepeatConsensus rc, AjPStr class);

AjBool ensRepeatConsensusSetType(EnsPRepeatConsensus rc, AjPStr type);

AjBool ensRepeatConsensusSetConsensus(EnsPRepeatConsensus rc, AjPStr consensus);

AjBool ensRepeatConsensusSetLength(EnsPRepeatConsensus rc, ajuint length);

ajuint ensRepeatConsensusGetMemSize(const EnsPRepeatConsensus rc);

AjBool ensRepeatConsensusTrace(const EnsPRepeatConsensus rc, ajuint level);

/* Ensembl Repeat Consensus Adaptor */

AjBool ensRepeatConsensusadaptorFetchByIdentifier(EnsPDatabaseadaptor dba,
                                                  ajuint identifier,
                                                  EnsPRepeatConsensus *Prc);

AjBool ensRepeatConsensusadaptorFetchByName(EnsPDatabaseadaptor dba,
                                            const AjPStr name,
                                            EnsPRepeatConsensus *Prc);

AjBool ensRepeatConsensusadaptorFetchByNameClass(EnsPDatabaseadaptor dba,
                                                 const AjPStr name,
                                                 const AjPStr class,
                                                 EnsPRepeatConsensus *Prc);

AjBool ensRepeatConsensusadaptorFetchAllByClassConsensus(
    EnsPDatabaseadaptor dba,
    const AjPStr class,
    const AjPStr consensus,
    AjPList rci);

/* Ensembl Repeat Feature */

EnsPRepeatFeature ensRepeatFeatureNew(EnsPRepeatFeatureadaptor adaptor,
                                      ajuint identifier,
                                      EnsPFeature feature,
                                      EnsPRepeatConsensus rc,
                                      ajint hstart,
                                      ajint hend,
                                      double score);

EnsPRepeatFeature ensRepeatFeatureNewObj(EnsPRepeatFeature object);

EnsPRepeatFeature ensRepeatFeatureNewRef(EnsPRepeatFeature rf);

void ensRepeatFeatureDel(EnsPRepeatFeature* Prf);

const EnsPRepeatFeatureadaptor ensRepeatFeatureGetadaptor(
    const EnsPRepeatFeature rf);

ajuint ensRepeatFeatureGetIdentifier(const EnsPRepeatFeature rf);

EnsPFeature ensRepeatFeatureGetFeature(const EnsPRepeatFeature rf);

EnsPRepeatConsensus ensRepeatFeatureGetRepeatConsensus(
    const EnsPRepeatFeature rf);

ajint ensRepeatFeatureGetHitStart(const EnsPRepeatFeature rf);

ajint ensRepeatFeatureGetHitEnd(const EnsPRepeatFeature rf);

ajint ensRepeatFeatureGetHitStrand(const EnsPRepeatFeature rf);

double ensRepeatFeatureGetScore(const EnsPRepeatFeature rf);

AjBool ensRepeatFeatureSetadaptor(EnsPRepeatFeature rf,
                                  EnsPRepeatFeatureadaptor rfa);

AjBool ensRepeatFeatureSetIdentifier(EnsPRepeatFeature rf, ajuint identifier);

AjBool ensRepeatFeatureSetFeature(EnsPRepeatFeature rf, EnsPFeature feature);

AjBool ensRepeatFeatureSetRepeatConsensus(EnsPRepeatFeature rf,
                                          EnsPRepeatConsensus rc);

AjBool ensRepeatFeatureSetHitStart(EnsPRepeatFeature rf, ajuint hstart);

AjBool ensRepeatFeatureSetHitEnd(EnsPRepeatFeature rf, ajuint hend);

AjBool ensRepeatFeatureSetScore(EnsPRepeatFeature rf, double score);

AjBool ensRepeatFeatureTrace(const EnsPRepeatFeature rf, ajuint level);

ajuint ensRepeatFeatureGetMemSize(const EnsPRepeatFeature rf);

/* Ensembl Repeat Feature Adaptor */

EnsPRepeatFeatureadaptor ensRepeatFeatureadaptorNew(EnsPDatabaseadaptor dba);

void ensRepeatFeatureadaptorDel(EnsPRepeatFeatureadaptor *Prfa);

AjBool ensRepeatFeatureadaptorFetchAllBySlice(
    const EnsPRepeatFeatureadaptor rfa,
    EnsPSlice slice,
    const AjPStr anname,
    const AjPStr rctype,
    const AjPStr rcclass,
    const AjPStr rcname,
    AjPList rfs);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
