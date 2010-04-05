#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensqc_h
#define ensqc_h

#include "ensanalysis.h"
#include "ensqcalignment.h"
#include "ensqcsequence.h"




/* EnsPQcdasfeatureadaptor ****************************************************
**
** Ensembl QC DAS Feature Adaptor
**
******************************************************************************/

#define EnsPQcdasfeatureadaptor EnsPBaseadaptor




/* EnsEQcdasfeatureCategory ***************************************************
**
** Ensembl QC DAS Feature Category enumeration
**
******************************************************************************/

enum EnsEQcdasfeatureCategory
{
    ensEQcdasfeatureCategoryNULL,
    ensEQcdasfeatureCategoryUnknown,
    ensEQcdasfeatureCategoryTranscriptPerfect,
    ensEQcdasfeatureCategoryTranscriptTolerance,
    ensEQcdasfeatureCategoryTranscriptPartial,
    ensEQcdasfeatureCategoryTranscriptMissing,
    ensEQcdasfeatureCategoryTranscript,
    ensEQcdasfeatureCategoryTranslationPerfect,
    ensEQcdasfeatureCategoryTranslationTolerance,
    ensEQcdasfeatureCategoryTranslationPartial,
    ensEQcdasfeatureCategoryTranslationMissing,
    ensEQcdasfeatureCategoryTranslation
};




/* EnsEQcdasfeatureType *******************************************************
**
** Ensembl QC DAS Feature Type enumeration
**
******************************************************************************/

enum EnsEQcdasfeatureType
{
    ensEQcdasfeatureTypeNULL,
    ensEQcdasfeatureTypeUnknown,
    ensEQcdasfeatureTypeExonPerfect,
    ensEQcdasfeatureTypeExonPartial,
    ensEQcdasfeatureTypeExonMissing,
    ensEQcdasfeatureTypeExonFrameshift,
    ensEQcdasfeatureTypeExonGap,
    ensEQcdasfeatureTypeExon
};




/* @data EnsPQcdasfeature *****************************************************
**
** Ensembl QC DAS Feature
**
** @alias EnsSQcdasfeature
** @alias EnsOQcdasfeature
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPQcdasfeatureadaptor] Ensembl QC DAS Feature Adaptor
** @cc Bio::EnsEMBL::QC::DASFeature
** @cc 'das_feature' SQL table
** @attr Qcalignment [EnsPQcalignment] Ensembl QC Alignment
** @attr Analysis [EnsPAnalysis] Ensembl Analysis
** @attr SegmentSequence [EnsPQcsequence] Segment Ensembl QC Sequence
** @attr FeatureSequence [EnsPQcsequence] Feature Ensembl QC Sequence
** @attr SegmentStart [ajuint] Segment start
** @attr SegmentEnd [ajuint] Segment end
** @attr SegmentStrand [ajint] Segment strand
** @attr FeatureStart [ajuint] Feature start
** @attr FeatureEnd [ajuint] Feature end
** @attr Phase [ajint] Phase
** @attr Category [AjEnum] Category
** @attr Type [AjEnum] Type
** @@
******************************************************************************/

typedef struct EnsSQcdasfeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPQcdasfeatureadaptor Adaptor;
    EnsPQcalignment Qcalignment;
    EnsPAnalysis Analysis;
    EnsPQcsequence SegmentSequence;
    EnsPQcsequence FeatureSequence;
    ajuint SegmentStart;
    ajuint SegmentEnd;
    ajint SegmentStrand;
    ajuint FeatureStart;
    ajuint FeatureEnd;
    ajint Phase;
    AjEnum Category;
    AjEnum Type;
} EnsOQcdasfeature;

#define EnsPQcdasfeature EnsOQcdasfeature*




/* EnsPQcvariationadaptor *****************************************************
**
** Ensembl QC Variation Adaptor
**
******************************************************************************/

#define EnsPQcvariationadaptor EnsPBaseadaptor




/* EnsEQcvariationClass *******************************************************
**
** Ensembl QC Variation Class enumeration
**
******************************************************************************/

enum EnsEQcvariationClass
{
    ensEQcvariationClassNULL,
    ensEQcvariationClassNone,
    ensEQcvariationClassSimple,
    ensEQcvariationClassSplice,
    ensEQcvariationClassExon
};




/* EnsEQcvariationType ********************************************************
**
** Ensembl QC Variation Type enumeration
**
******************************************************************************/

enum EnsEQcvariationType
{
    ensEQcvariationTypeNULL,
    ensEQcvariationTypeNone,
    ensEQcvariationTypeSingle,
    ensEQcvariationTypeMulti
};




/* EnsEQcvariationState *******************************************************
**
** Ensembl QC Variation State enumeration
**
******************************************************************************/

enum EnsEQcvariationState
{
    ensEQcvariationStateNULL,
    ensEQcvariationStateNone,
    ensEQcvariationStateMatch,
    ensEQcvariationStateGap,
    ensEQcvariationStateFrameshift,
    ensEQcvariationState5ss,
    ensEQcvariationState3ss,
    EnsEQcvariationStateSplit
};




/* @data EnsPQcvariation ******************************************************
**
** Ensembl QC Variation
**
** @alias EnsSQcvariation
** @alias EnsOQcvariation
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPQcvariationadaptor] Ensembl QC Variation Adaptor
** @cc Bio::EnsEMBL::QC::Variation
** @cc 'variation' SQL table
** @attr Analysis [EnsPAnalysis] Ensembl Analysis
** @attr Qcalignment [EnsPQcalignment] Ensembl QC Alignment
** @attr QuerySequence [EnsPQcsequence] Query Ensembl QC Sequence
** @attr QueryStart [ajuint] Query start
** @attr QueryEnd [ajuint] Query end
** @attr QueryString [AjPStr] Query string
** @attr TargetSequence [EnsPQcsequence] Target Ensembl QC Sequence
** @attr TargetStart [ajuint] Target start
** @attr TargetEnd [ajuint] Target end
** @attr TargetString [AjPStr] Target string
** @attr Class [AjEnum] Class
** @attr Type [AjEnum] Type
** @attr State [AjEnum] State
** @attr Padding [char[4]] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSQcvariation
{
    ajuint Use;
    ajuint Identifier;
    EnsPQcvariationadaptor Adaptor;
    EnsPAnalysis Analysis;
    EnsPQcalignment Qcalignment;
    EnsPQcsequence QuerySequence;
    ajuint QueryStart;
    ajuint QueryEnd;
    AjPStr QueryString;
    EnsPQcsequence TargetSequence;
    ajuint TargetStart;
    ajuint TargetEnd;
    AjPStr TargetString;
    AjEnum Class;
    AjEnum Type;
    AjEnum State;
    char Padding[4];
} EnsOQcvariation;

#define EnsPQcvariation EnsOQcvariation*




/* EnsPQcsubmissionadaptor ****************************************************
**
** Ensembl QC Submission Adaptor
**
******************************************************************************/

#define EnsPQcsubmissionadaptor EnsPBaseadaptor




/* @data EnsPQcsubmission *****************************************************
**
** Ensembl QC Submission
**
** @alias EnsSQcsubmission
** @alias EnsOQcsubmission
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPQcsubmissionadaptor] Ensembl QC Submission Adaptor
** @cc Bio::EnsEMBL::QC::Submission
** @cc 'submission' SQL table
** @attr Analysis [EnsPAnalysis] Ensembl Analysis
** @attr QuerySequence [EnsPQcsequence] Query Ensembl QC Sequence
** @attr TargetSequence [EnsPQcsequence] Target Ensembl QC Sequence
** @attr QueryStart [ajuint] Query start
** @attr QueryEnd [ajuint] Query end
** @attr QueryStrand [ajint] Query strand
** @attr TargetStart [ajuint] Target start
** @attr TargetEnd [ajuint] Target end
** @attr TargetStrand [ajint] Target strand
** @attr AnalysisJobIdentifier [ajuint] Ensembl Hive Analysis Job identifier
** @attr Padding [ajuint] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSQcsubmission
{
    ajuint Use;
    ajuint Identifier;
    EnsPQcsubmissionadaptor Adaptor;
    EnsPAnalysis Analysis;
    EnsPQcsequence QuerySequence;
    EnsPQcsequence TargetSequence;
    ajuint QueryStart;
    ajuint QueryEnd;
    ajint QueryStrand;
    ajuint TargetStart;
    ajuint TargetEnd;
    ajint TargetStrand;
    ajuint AnalysisJobIdentifier;
    ajuint Padding;
} EnsOQcsubmission;

#define EnsPQcsubmission EnsOQcsubmission*




/*
** Prototype definitions
*/

/* Ensembl Quality Check DAS Feature */

EnsPQcdasfeature ensQcdasfeatureNew(EnsPQcdasfeatureadaptor qcdasfa,
                                    ajuint identifier,
                                    EnsPQcalignment qca,
                                    EnsPAnalysis analysis,
                                    EnsPQcsequence segment,
                                    ajuint segstart,
                                    ajuint segend,
                                    ajint segstrand,
                                    EnsPQcsequence feature,
                                    ajuint fstart,
                                    ajuint fend,
                                    ajint phase,
                                    AjEnum category,
                                    AjEnum type);

EnsPQcdasfeature ensQcdasfeatureNewObj(const EnsPQcdasfeature object);

EnsPQcdasfeature ensQcdasfeatureNewRef(EnsPQcdasfeature qcdasf);

void ensQcdasfeatureDel(EnsPQcdasfeature* Pqcdasf);

EnsPQcdasfeatureadaptor ensQcdasfeatureGetAdaptor(
    const EnsPQcdasfeature qcdasf);

ajuint ensQcdasfeatureGetIdentifier(const EnsPQcdasfeature qcdasf);

EnsPQcalignment ensQcdasfeatureGetQcalignment(const EnsPQcdasfeature qcdasf);

EnsPAnalysis ensQcdasfeatureGetAnalysis(const EnsPQcdasfeature qcdasf);

EnsPQcsequence ensQcdasfeatureGetSegmentSequence(
    const EnsPQcdasfeature qcdasf);

EnsPQcsequence ensQcdasfeatureGetFeatureSequence(
    const EnsPQcdasfeature qcdasf);

ajuint ensQcdasfeatureGetSegmentStart(const EnsPQcdasfeature qcdasf);

ajuint ensQcdasfeatureGetSegmentEnd(const EnsPQcdasfeature qcdasf);

ajint ensQcdasfeatureGetSegmentStrand(const EnsPQcdasfeature qcdasf);

ajuint ensQcdasfeatureGetFeatureStart(const EnsPQcdasfeature qcdasf);

ajuint ensQcdasfeatureGetFeatureEnd(const EnsPQcdasfeature qcdasf);

ajint ensQcdasfeatureGetPhase(const EnsPQcdasfeature qcdasf);

AjEnum ensQcdasfeatureGetCategory(const EnsPQcdasfeature qcdasf);

AjEnum ensQcdasfeatureGetType(const EnsPQcdasfeature qcdasf);

AjBool ensQcdasfeatureSetAdaptor(EnsPQcdasfeature qcdasf,
                                 EnsPQcdasfeatureadaptor qcdasfa);

AjBool ensQcdasfeatureSetIdentifier(EnsPQcdasfeature qcdasf,
                                    ajuint identifier);

AjBool ensQcdasfeatureSetQcalignment(EnsPQcdasfeature qcdasf,
                                     EnsPQcalignment qca);

AjBool ensQcdasfeatureSetAnalysis(EnsPQcdasfeature qcdasf,
                                  EnsPAnalysis analysis);

AjBool ensQcdasfeatureSetSegmentSequence(EnsPQcdasfeature qcdasf,
                                         EnsPQcsequence qcs);

AjBool ensQcdasfeatureSetFeatureSequence(EnsPQcdasfeature qcdasf,
                                         EnsPQcsequence qcs);

AjBool ensQcdasfeatureSetSegmentStart(EnsPQcdasfeature qcdasf,
                                      ajuint start);

AjBool ensQcdasfeatureSetSegmentEnd(EnsPQcdasfeature qcdasf,
                                    ajuint end);

AjBool ensQcdasfeatureSetSegmentStrand(EnsPQcdasfeature qcdasf,
                                       ajint strand);

AjBool ensQcdasfeatureSetFeatureStart(EnsPQcdasfeature qcdasf,
                                      ajuint start);

AjBool ensQcdasfeatureSetFeatureEnd(EnsPQcdasfeature qcdasf,
                                    ajuint end);

AjBool ensQcdasfeatureSetPhase(EnsPQcdasfeature qcdasf,
                               ajint phase);

AjBool ensQcdasfeatureSetCategory(EnsPQcdasfeature qcdasf,
                                  AjEnum category);

AjBool ensQcdasfeatureSetType(EnsPQcdasfeature qcdasf,
                              AjEnum type);

ajuint ensQcdasfeatureGetMemSize(const EnsPQcdasfeature qcdasf);

AjBool ensQcdasfeatureTrace(const EnsPQcdasfeature qcdasf, ajuint level);

AjEnum ensQcdasfeatureCategoryFromStr(const AjPStr category);

AjEnum ensQcdasfeatureTypeFromStr(const AjPStr type);

const char *ensQcdasfeatureCategoryToChar(const AjEnum category);

const char *ensQcdasfeatureTypeToChar(const AjEnum type);

/* Ensembl Quality Check DAS Feature Adaptor */

EnsPQcdasfeatureadaptor ensQcdasfeatureadaptorNew(EnsPDatabaseadaptor dba);

void ensQcdasfeatureadaptorDel(EnsPQcdasfeatureadaptor *Pqcdasfa);

AjBool ensQcdasfeatureadaptorFetchByIdentifier(
    EnsPQcdasfeatureadaptor qcdasfa,
    ajuint identifier,
    EnsPQcdasfeature *Pqcdasf);

AjBool ensQcdasfeatureadaptorFetchAllByQcalignment(
    EnsPQcdasfeatureadaptor qcdasfa,
    const EnsPQcalignment qca,
    AjPList qcdasfs);

AjBool ensQcdasfeatureadaptorFetchAllByFeature(
    EnsPQcdasfeatureadaptor qcdasfa,
    const EnsPAnalysis analysis,
    const EnsPQcsequence feature,
    AjPList qcdasfs);

AjBool ensQcdasfeatureadaptorFetchAllBySegment(
    EnsPQcdasfeatureadaptor qcdasfa,
    const EnsPAnalysis analysis,
    const EnsPQcsequence segment,
    AjPList qcdasfs);

AjBool ensQcdasfeatureadaptorFetchAllByAFS(
    EnsPQcdasfeatureadaptor qcdasfa,
    const EnsPAnalysis analysis,
    const EnsPQcsequence feature,
    const EnsPQcsequence segment,
    AjPList qcdasfs);

AjBool ensQcdasfeatureadaptorFetchAllByRegion(
    EnsPQcdasfeatureadaptor qcdasfa,
    const EnsPAnalysis analysis,
    const EnsPQcsequence segment,
    ajuint start,
    ajuint end,
    AjPList qcdasfs);

AjBool ensQcdasfeatureadaptorStore(EnsPQcdasfeatureadaptor qcdasfa,
                                   EnsPQcdasfeature qcdasf);

AjBool ensQcdasfeatureadaptorUpdate(EnsPQcdasfeatureadaptor qcdasfa,
                                    const EnsPQcdasfeature qcdasf);

AjBool ensQcdasfeatureadaptorDelete(EnsPQcdasfeatureadaptor qcdasfa,
                                    EnsPQcdasfeature qcdasf);

/* Ensembl Quality Check Variation */

EnsPQcvariation ensQcvariationNew(EnsPQcvariationadaptor adaptor,
                                  ajuint identifier,
                                  EnsPQcalignment qca,
                                  EnsPAnalysis analysis,
                                  EnsPQcsequence qsequence,
                                  ajuint qstart,
                                  ajuint qend,
                                  AjPStr qstring,
                                  EnsPQcsequence tsequence,
                                  ajuint tstart,
                                  ajuint tend,
                                  AjPStr tstring,
                                  AjEnum class,
                                  AjEnum type,
                                  AjEnum state);

EnsPQcvariation ensQcvariationNewObj(const EnsPQcvariation object);

EnsPQcvariation ensQcvariationNewRef(EnsPQcvariation qcv);

void ensQcvariationDel(EnsPQcvariation* Pqcv);

EnsPQcvariationadaptor ensQcvariationGetAdaptor(const EnsPQcvariation qcv);

ajuint ensQcvariationGetIdentifier(const EnsPQcvariation qcv);

EnsPQcalignment ensQcvariationGetQcalignment(const EnsPQcvariation qcv);

EnsPAnalysis ensQcvariationGetAnalysis(const EnsPQcvariation qcv);

EnsPQcsequence ensQcvariationGetQuerySequence(const EnsPQcvariation qcv);

ajuint ensQcvariationGetQueryStart(const EnsPQcvariation qcv);

ajuint ensQcvariationGetQueryEnd(const EnsPQcvariation qcv);

AjPStr ensQcvariationGetQueryString(const EnsPQcvariation qcv);

EnsPQcsequence ensQcvariationGetTargetSequence(const EnsPQcvariation qcv);

ajuint ensQcvariationGetTargetStart(const EnsPQcvariation qcv);

ajuint ensQcvariationGetTargetEnd(const EnsPQcvariation qcv);

AjPStr ensQcvariationGetTargetString(const EnsPQcvariation qcv);

AjEnum ensQcvariationGetClass(const EnsPQcvariation qcv);

AjEnum ensQcvariationGetType(const EnsPQcvariation qcv);

AjEnum ensQcvariationGetState(const EnsPQcvariation qcv);

AjBool ensQcvariationSetAdaptor(EnsPQcvariation qcv,
                                EnsPQcvariationadaptor qcva);

AjBool ensQcvariationSetIdentifier(EnsPQcvariation qcv,
                                   ajuint identifier);

AjBool ensQcvariationSetQcalignment(EnsPQcvariation qcv,
                                    EnsPQcalignment qca);

AjBool ensQcvariationSetAnalysis(EnsPQcvariation qcv,
                                 EnsPAnalysis analysis);

AjBool ensQcvariationSetQuerySequence(EnsPQcvariation qcv,
                                      EnsPQcsequence qsequence);

AjBool ensQcvariationSetQueryStart(EnsPQcvariation qcv,
                                   ajuint qstart);

AjBool ensQcvariationSetQueryEnd(EnsPQcvariation qcv,
                                 ajuint qend);

AjBool ensQcvariationSetQueryString(EnsPQcvariation qcv,
                                    AjPStr qstring);

AjBool ensQcvariationSetTargetSequence(EnsPQcvariation qcv,
                                       EnsPQcsequence tsequence);

AjBool ensQcvariationSetTargetStart(EnsPQcvariation qcv,
                                    ajuint tstart);

AjBool ensQcvariationSetTargetEnd(EnsPQcvariation qcv,
                                  ajuint tend);

AjBool ensQcvariationSetTargetString(EnsPQcvariation qcv,
                                     AjPStr tstring);

AjBool ensQcvariationSetClass(EnsPQcvariation qcv,
                              AjEnum class);

AjBool ensQcvariationSetType(EnsPQcvariation qcv,
                             AjEnum type);

AjBool ensQcvariationSetState(EnsPQcvariation qcv,
                              AjEnum state);

ajuint ensQcvariationGetMemSize(const EnsPQcvariation qcv);

AjBool ensQcvariationTrace(const EnsPQcvariation qcv, ajuint level);

AjEnum ensQcvariationClassFromStr(const AjPStr vclass);

AjEnum ensQcvariationTypeFromStr(const AjPStr type);

AjEnum ensQcvariationStateFromStr(const AjPStr state);

const char *ensQcvariationClassToChar(const AjEnum vclass);

const char *ensQcvariationTypeToChar(const AjEnum type);

const char *ensQcvariationStateToChar(const AjEnum state);

/* Ensembl Quality Check Variation Adaptor */

EnsPQcvariationadaptor ensQcvariationadaptorNew(EnsPDatabaseadaptor dba);

void ensQcvariationadaptorDel(EnsPQcvariationadaptor* Pqcva);

AjBool ensQcvariationadaptorFetchByIdentifier(
    EnsPQcvariationadaptor adaptor,
    ajuint identifier,
    EnsPQcvariation *Pqcvf);

AjBool ensQcvariationadaptorFetchAllByQcalignment(
    EnsPQcvariationadaptor qcva,
    const EnsPQcalignment qca,
    AjPList qcvs);

AjBool ensQcvariationadaptorFetchAllByAnalysisQueryTarget(
    EnsPQcvariationadaptor qcva,
    const EnsPAnalysis analysis,
    const EnsPQcdatabase qdb,
    const EnsPQcdatabase tdb,
    AjPList qcvs);

AjBool ensQcvariationadaptorFetchAllByQuery(
    EnsPQcvariationadaptor qcva,
    const EnsPAnalysis analysis,
    const EnsPQcdatabase qdb,
    AjPList qcvs);

AjBool ensQcvariationadaptorFetchAllByTarget(
    EnsPQcvariationadaptor qcva,
    const EnsPAnalysis analysis,
    const EnsPQcdatabase tdb,
    AjPList qcvs);

AjBool ensQcvariationadaptorStore(EnsPQcvariationadaptor qcva,
                                  EnsPQcvariation qcv);

AjBool ensQcvariationadaptorUpdate(EnsPQcvariationadaptor qcva,
                                   const EnsPQcvariation qcv);

AjBool ensQcvariationadaptorDelete(EnsPQcvariationadaptor qcva,
                                   EnsPQcvariation qcv);

/* Ensembl Quality Check Submission */

EnsPQcsubmission ensQcsubmissionNew(EnsPQcsubmissionadaptor adaptor,
                                    ajuint identifier,
                                    EnsPAnalysis analysis,
                                    EnsPQcsequence qsequence,
                                    EnsPQcsequence tsequence,
                                    ajuint tstart,
                                    ajuint tend,
                                    ajint tstrand,
                                    ajuint analysisjobid);

EnsPQcsubmission ensQcsubmissionNewObj(const EnsPQcsubmission object);

EnsPQcsubmission ensQcsubmissionNewRef(EnsPQcsubmission qcsb);

void ensQcsubmissionDel(EnsPQcsubmission* Pqcsb);

EnsPQcsubmissionadaptor ensQcsubmissionGetAdaptor(const EnsPQcsubmission qcsb);

ajuint ensQcsubmissionGetIdentifier(const EnsPQcsubmission qcsb);

EnsPAnalysis ensQcsubmissionGetAnalysis(const EnsPQcsubmission qcsb);

EnsPQcsequence ensQcsubmissionGetQuerySequence(const EnsPQcsubmission qcsb);

ajuint ensQcsubmissionGetQueryStart(const EnsPQcsubmission qcsb);

ajuint ensQcsubmissionGetQueryEnd(const EnsPQcsubmission qcsb);

ajint ensQcsubmissionGetQueryStrand(const EnsPQcsubmission qcsb);

EnsPQcsequence ensQcsubmissionGetTargetSequence(const EnsPQcsubmission qcsb);

ajuint ensQcsubmissionGetTargetStart(const EnsPQcsubmission qcsb);

ajuint ensQcsubmissionGetTargetEnd(const EnsPQcsubmission qcsb);

ajint ensQcsubmissionGetTargetStrand(const EnsPQcsubmission qcsb);

ajuint ensQcsubmissionGetAnalysisJobIdentifier(const EnsPQcsubmission qcsb);

AjBool ensQcsubmissionSetAdaptor(EnsPQcsubmission qcsb,
                                 EnsPQcsubmissionadaptor qcsba);

AjBool ensQcsubmissionSetIdentifier(EnsPQcsubmission qcsb,
                                    ajuint identifier);

AjBool ensQcsubmissionSetAnalysis(EnsPQcsubmission qcsb,
                                  EnsPAnalysis analysis);

AjBool ensQcsubmissionSetQuerySequence(EnsPQcsubmission qcsb,
                                       EnsPQcsequence qsequence);

AjBool ensQcsubmissionSetQueryStart(EnsPQcsubmission qcsb,
                                    ajuint qstart);

AjBool ensQcsubmissionSetQueryEnd(EnsPQcsubmission qcsb,
                                  ajuint qend);

AjBool ensQcsubmissionSetQueryStrand(EnsPQcsubmission qcsb,
                                     ajint qstrand);

AjBool ensQcsubmissionSetTargetSequence(EnsPQcsubmission qcsb,
                                        EnsPQcsequence tsequence);

AjBool ensQcsubmissionSetTargetStart(EnsPQcsubmission qcsb,
                                     ajuint tstart);

AjBool ensQcsubmissionSetTargetEnd(EnsPQcsubmission qcsb,
                                   ajuint tend);

AjBool ensQcsubmissionSetTargetStrand(EnsPQcsubmission qcsb,
                                      ajint tstrand);

AjBool ensQcsubmissionSetAnalysisJobIdentifier(EnsPQcsubmission qcsb,
                                               ajuint jobid);

ajuint ensQcsubmissionGetMemSize(const EnsPQcsubmission qcsb);

AjBool ensQcsubmissionTrace(const EnsPQcsubmission qcsb, ajuint level);

/* Ensembl Quality Check Submission Adaptor */

EnsPQcsubmissionadaptor ensQcsubmissionadaptorNew(EnsPDatabaseadaptor dba);

void ensQcsubmissionadaptorDel(EnsPQcsubmissionadaptor* Pqcsba);

AjBool ensQcsubmissionadaptorFetchByIdentifier(
    EnsPQcsubmissionadaptor adaptor,
    ajuint identifier,
    EnsPQcsubmission *Pqcsb);

AjBool ensQcsubmissionadaptorFetchAllByAnalysisQueryTarget(
    EnsPQcsubmissionadaptor qcsba,
    const EnsPAnalysis analysis,
    const EnsPQcdatabase qdb,
    const EnsPQcdatabase tdb,
    AjPList qcsbs);

AjBool ensQcsubmissionadaptorFetchAllByANQIDTDB(
    EnsPQcsubmissionadaptor qcsba,
    const EnsPAnalysis analysis,
    const EnsPQcsequence qseqeunce,
    const EnsPQcdatabase tdb,
    const EnsPQcsequence tsequence,
    ajuint tstart,
    ajuint tend,
    ajint tstrand,
    AjPList qcsbs);

AjBool ensQcsubmissionadaptorFetchAllByQuery(
    EnsPQcsubmissionadaptor qcsba,
    const EnsPAnalysis analysis,
    const EnsPQcdatabase qdb,
    AjPList qcsbs);

AjBool ensQcsubmissionadaptorFetchAllByTarget(
    EnsPQcsubmissionadaptor qcsba,
    const EnsPAnalysis analysis,
    const EnsPQcdatabase tdb,
    AjPList qcsbs);

AjBool ensQcsubmissionadaptorStore(EnsPQcsubmissionadaptor qcsba,
                                   EnsPQcsubmission qcsb);

AjBool ensQcsubmissionadaptorUpdate(EnsPQcsubmissionadaptor qcsba,
                                    const EnsPQcsubmission qcsb);

AjBool ensQcsubmissionadaptorDelete(EnsPQcsubmissionadaptor qcsba,
                                    EnsPQcsubmission qcsb);

/*
** End of prototype definitions
*/




#endif /* ensqc_h */

#ifdef __cplusplus
}
#endif
