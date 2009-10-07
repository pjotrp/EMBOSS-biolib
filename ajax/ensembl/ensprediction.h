#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensprediction_h
#define ensprediction_h

#include "ensfeature.h"




/******************************************************************************
**
** Ensembl Prediction Exon Adaptor
**
******************************************************************************/

#define EnsPPredictionExonadaptor EnsPFeatureadaptor




/* @data EnsPPredictionExon ***************************************************
**
** Ensembl Prediction Exon.
**
** @alias EnsSPredictionExon
** @alias EnsOPredictionExon
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] Internal SQL database identifier (primary key)
** @attr Adaptor [EnsPPredictionExonadaptor] Ensembl Prediction Exon Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::PredictionExon
** @cc 'prediction_exon' SQL table
** @attr Score [double] Score
** @attr Pvalue [double] P-value
** @attr StartPhase [ajint] Start phase
** @attr Padding [char[4]] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSPredictionExon
{
    ajuint Use;
    ajuint Identifier;
    EnsPPredictionExonadaptor Adaptor;
    EnsPFeature Feature;
    double Score;
    double Pvalue;
    ajint StartPhase;
    char Padding[4];
} EnsOPredictionExon;

#define EnsPPredictionExon EnsOPredictionExon*




/******************************************************************************
**
** Ensembl Prediction Transcript Adaptor
**
******************************************************************************/

#define EnsPPredictionTranscriptadaptor EnsPFeatureadaptor




/* @data EnsPPredictionTranscript *********************************************
**
** Ensembl Prediction Transcript.
**
** @alias EnsSPredictionTranscript
** @alias EnsOPredictionTranscript
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPPredictionTranscriptadaptor] Ensembl Prediction
**                                                 Transcript Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::PredictionTranscript
** @attr DisplayLabel [AjPStr] Display label
** @attr PredictionExons [AjPList] AJAX List of Ensembl Prediction Exons
** @@
******************************************************************************/

typedef struct EnsSPredictionTranscript
{
    ajuint Use;
    ajuint Identifier;
    EnsPPredictionTranscriptadaptor Adaptor;
    EnsPFeature Feature;
    AjPStr DisplayLabel;
    AjPList PredictionExons;
} EnsOPredictionTranscript;

#define EnsPPredictionTranscript EnsOPredictionTranscript*




/*
** Prototype definitions
*/

/* Ensembl Prediction Exon */

EnsPPredictionExon ensPredictionExonNew(EnsPPredictionExonadaptor adaptor,
                                        ajuint identifier,
                                        EnsPFeature feature,
                                        ajint sphase,
                                        double score,
                                        double pvalue);

EnsPPredictionExon ensPredictionExonNewObj(const EnsPPredictionExon object);

EnsPPredictionExon ensPredictionExonNewRef(EnsPPredictionExon pe);

void ensPredictionExonDel(EnsPPredictionExon* Ppe);

EnsPPredictionExonadaptor ensPredictionExonGetadaptor(
    const EnsPPredictionExon pe);

ajuint ensPredictionExonGetIdentifier(const EnsPPredictionExon pe);

EnsPFeature ensPredictionExonGetFeature(const EnsPPredictionExon pe);

ajint ensPredictionExonGetStartPhase(const EnsPPredictionExon pe);

double ensPredictionExonGetScore(const EnsPPredictionExon pe);

double ensPredictionExonGetPvalue(const EnsPPredictionExon pe);

AjBool ensPredictionExonSetadaptor(EnsPPredictionExon pe,
                                   EnsPPredictionExonadaptor adaptor);

AjBool ensPredictionExonSetIdentifier(EnsPPredictionExon pe, ajuint identifier);

AjBool ensPredictionExonSetFeature(EnsPPredictionExon pe, EnsPFeature feature);

AjBool ensPredictionExonSetStartPhase(EnsPPredictionExon pe, ajint sphase);

AjBool ensPredictionExonSetScore(EnsPPredictionExon pe, double score);

AjBool ensPredictionExonSetPvalue(EnsPPredictionExon pe, double pvalue);

AjBool ensPredictionExonTrace(const EnsPPredictionExon pe, ajuint level);

ajint ensPredictionExonGetEndPhase(const EnsPPredictionExon pe);

ajuint ensPredictionExonGetMemSize(const EnsPPredictionExon pe);

EnsPPredictionExon ensPredictionExonTransform(EnsPPredictionExon pe,
                                              const AjPStr csname,
                                              const AjPStr csversion);

EnsPPredictionExon ensPredictionExonTransfer(EnsPPredictionExon pe,
                                             EnsPSlice slice);

/* Ensembl Prediction Exon Adaptor */

EnsPPredictionExonadaptor ensPredictionExonadaptorNew(EnsPDatabaseadaptor dba);

void ensPredictionExonadaptorDel(EnsPPredictionExonadaptor *Padaptor);

AjBool ensPredictionExonadaptorFetchAllByPredictionTranscript(
    EnsPPredictionExonadaptor pea,
    const EnsPPredictionTranscript pt,
    AjPList pes);

/* Ensembl Prediction Transcript */

EnsPPredictionTranscript ensPredictionTranscriptNew(
    EnsPPredictionTranscriptadaptor adaptor,
    ajuint identifier,
    EnsPFeature feature,
    AjPStr label);

EnsPPredictionTranscript ensPredictionTranscriptNewObj(
    const EnsPPredictionTranscript object);

EnsPPredictionTranscript ensPredictionTranscriptNewRef(
    EnsPPredictionTranscript pt);

void ensPredictionTranscriptDel(EnsPPredictionTranscript *Ppt);

EnsPPredictionTranscriptadaptor ensPredictionTranscriptGetadaptor(
    const EnsPPredictionTranscript pt);

ajuint ensPredictionTranscriptGetIdentifier(const EnsPPredictionTranscript pt);

EnsPFeature ensPredictionTranscriptGetFeature(
    const EnsPPredictionTranscript pt);

AjPStr ensPredictionTranscriptGetDisplayLabel(
    const EnsPPredictionTranscript pt);

AjBool ensPredictionTranscriptSetadaptor(
    EnsPPredictionTranscript pt,
    EnsPPredictionTranscriptadaptor adaptor);

AjBool ensPredictionTranscriptSetIdentifier(EnsPPredictionTranscript pt,
                                            ajuint identifier);

AjBool ensPredictionTranscriptSetFeature(EnsPPredictionTranscript pt,
                                         EnsPFeature feature);

AjBool ensPredictionTranscriptSetDisplayLabel(EnsPPredictionTranscript pt,
                                              AjPStr label);

AjBool ensPredictionTranscriptTrace(const EnsPPredictionTranscript pt,
                                    ajuint level);

ajuint ensPredictionTranscriptGetMemSize(const EnsPPredictionTranscript pt);

const AjPList ensPredictionTranscriptGetExons(EnsPPredictionTranscript pt);

/* Ensembl Prediction Transcript Adaptor */

EnsPPredictionTranscriptadaptor ensPredictionTranscriptadaptorNew(
    EnsPDatabaseadaptor dba);

void ensPredictionTranscriptadaptorDel(
    EnsPPredictionTranscriptadaptor *Padaptor);

AjBool ensPredictionTranscriptadaptorFetchByIdentifier(
    EnsPPredictionTranscriptadaptor adaptor,
    ajuint identifier,
    EnsPPredictionTranscript *Ppt);

AjBool ensPredictionTranscriptadaptorFetchByStableIdentifier(
    EnsPPredictionTranscriptadaptor adaptor,
    const AjPStr stableid,
    EnsPPredictionTranscript *Ppt);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
