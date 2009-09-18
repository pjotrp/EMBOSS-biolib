#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensprediction_h
#define ensprediction_h

#include "ensfeature.h"




/* @data EnsPPredictionExonAdaptor ********************************************
**
** Ensembl Prediction Exon Adaptor
**
** @alias EnsPFeatureAdaptor
** @@
******************************************************************************/

#define EnsPPredictionExonAdaptor EnsPFeatureAdaptor




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
** @attr Adaptor [EnsPPredictionExonAdaptor] Ensembl Prediction Exon Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::PredictionExon
** @cc 'prediction_exon' SQL table
** @attr StartPhase [ajint] Start phase
** @attr Score [double] Score
** @attr Pvalue [double] P-value
** @@
******************************************************************************/

typedef struct EnsSPredictionExon
{
    ajuint Use;
    ajuint Identifier;
    EnsPPredictionExonAdaptor Adaptor;
    EnsPFeature Feature;
    ajint StartPhase;
    double Score;
    double Pvalue;
} EnsOPredictionExon;

#define EnsPPredictionExon EnsOPredictionExon*




/* @data EnsPPredictionTranscriptAdaptor **************************************
**
** Ensembl Prediction Transcript Adaptor
**
** @alias EnsPFeatureAdaptor
** @@
******************************************************************************/

#define EnsPPredictionTranscriptAdaptor EnsPFeatureAdaptor




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
** @attr Adaptor [EnsPPredictionTranscriptAdaptor] Ensembl Prediction
**                                                 Transcript Adaptor
** @cc Bio::EnsEMBL::Feature
** @attr Feature [EnsPFeature] Ensembl Feature
** @cc Bio::EnsEMBL::PredictionTranscript
** @attr DisplayLabel [AjPStr] Display label
** @attr PredictionTranscript [AjPList] AJAX List of Ensembl Prediction Exons
** @@
******************************************************************************/

typedef struct EnsSPredictionTranscript
{
    ajuint Use;
    ajuint Identifier;
    EnsPPredictionTranscriptAdaptor Adaptor;
    EnsPFeature Feature;
    AjPStr DisplayLabel;
    AjPList PredictionExons;
} EnsOPredictionTranscript;

#define EnsPPredictionTranscript EnsOPredictionTranscript*




/*
** Prototype definitions
*/

/* Ensembl Prediction Exon */

EnsPPredictionExon ensPredictionExonNew(EnsPPredictionExonAdaptor adaptor,
                                        ajuint identifier,
                                        EnsPFeature feature,
                                        ajint sphase,
                                        double score,
                                        double pvalue);

EnsPPredictionExon ensPredictionExonNewObj(const EnsPPredictionExon object);

EnsPPredictionExon ensPredictionExonNewRef(EnsPPredictionExon pe);

void ensPredictionExonDel(EnsPPredictionExon* Ppe);

EnsPPredictionExonAdaptor ensPredictionExonGetAdaptor(
    const EnsPPredictionExon pe);

ajuint ensPredictionExonGetIdentifier(const EnsPPredictionExon pe);

EnsPFeature ensPredictionExonGetFeature(const EnsPPredictionExon pe);

ajint ensPredictionExonGetStartPhase(const EnsPPredictionExon pe);

double ensPredictionExonGetScore(const EnsPPredictionExon pe);

double ensPredictionExonGetPvalue(const EnsPPredictionExon pe);

AjBool ensPredictionExonSetAdaptor(EnsPPredictionExon pe,
                                   EnsPPredictionExonAdaptor adaptor);

AjBool ensPredictionExonSetIdentifier(EnsPPredictionExon pe, ajuint identifier);

AjBool ensPredictionExonSetFeature(EnsPPredictionExon pe, EnsPFeature feature);

AjBool ensPredictionExonSetStartPhase(EnsPPredictionExon pe, ajint sphase);

AjBool ensPredictionExonSetScore(EnsPPredictionExon pe, double score);

AjBool ensPredictionExonSetPvalue(EnsPPredictionExon pe, double pvalue);

AjBool ensPredictionExonTrace(const EnsPPredictionExon pe, ajuint level);

ajuint ensPredictionExonGetMemSize(const EnsPPredictionExon pe);

EnsPPredictionExon ensPredictionExonTransform(EnsPPredictionExon pe,
                                              const AjPStr csname,
                                              const AjPStr csversion);

EnsPPredictionExon ensPredictionExonTransfer(EnsPPredictionExon pe,
                                             EnsPSlice slice);

/* Ensembl Prediction Exon Adaptor */

EnsPPredictionExonAdaptor ensPredictionExonAdaptorNew(EnsPDatabaseAdaptor dba);

void ensPredictionExonAdaptorDel(EnsPPredictionExonAdaptor *Padaptor);

AjBool ensPredictionExonAdaptorFetchAllByPredictionTranscript(
    EnsPPredictionExonAdaptor pea,
    const EnsPPredictionTranscript pt,
    AjPList pes);

/* Ensembl Prediction Transcript */

EnsPPredictionTranscript ensPredictionTranscriptNew(
    EnsPPredictionTranscriptAdaptor adaptor,
    ajuint identifier,
    EnsPFeature feature,
    AjPStr label);

EnsPPredictionTranscript ensPredictionTranscriptNewObj(
    const EnsPPredictionTranscript object);

EnsPPredictionTranscript ensPredictionTranscriptNewRef(
    EnsPPredictionTranscript pt);

void ensPredictionTranscriptDel(EnsPPredictionTranscript *Ppt);

EnsPPredictionTranscriptAdaptor ensPredictionTranscriptGetAdaptor(
    const EnsPPredictionTranscript pt);

ajuint ensPredictionTranscriptGetIdentifier(const EnsPPredictionTranscript pt);

EnsPFeature ensPredictionTranscriptGetFeature(
    const EnsPPredictionTranscript pt);

AjPStr ensPredictionTranscriptGetDisplayLabel(
    const EnsPPredictionTranscript pt);

AjBool ensPredictionTranscriptSetAdaptor(
    EnsPPredictionTranscript pt,
    EnsPPredictionTranscriptAdaptor adaptor);

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

EnsPPredictionTranscriptAdaptor ensPredictionTranscriptAdaptorNew(
    EnsPDatabaseAdaptor dba);

void ensPredictionTranscriptAdaptorDel(
    EnsPPredictionTranscriptAdaptor *Padaptor);

AjBool ensPredictionTranscriptAdaptorFetchByIdentifier(
    EnsPPredictionTranscriptAdaptor adaptor,
    ajuint identifier,
    EnsPPredictionTranscript *Ppt);

AjBool ensPredictionTranscriptAdaptorFetchByStableIdentifier(
    EnsPPredictionTranscriptAdaptor adaptor,
    const AjPStr stableid,
    EnsPPredictionTranscript *Ppt);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
