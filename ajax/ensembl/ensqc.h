#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensqc_h
#define ensqc_h

#include "ensanalysis.h"
#include "ensqcalignment.h"
#include "ensqcsequence.h"




/******************************************************************************
**
** Ensembl QC DAS Feature Adaptor
**
******************************************************************************/

#define EnsPQCDASFeatureadaptor EnsPBaseadaptor




/******************************************************************************
**
** Ensembl QC DAS Feature Category enumeration
**
******************************************************************************/

enum EnsEQCDASFeatureCategory
{
    ensEQCDASFeatureCategoryNULL,
    ensEQCDASFeatureCategoryUnknown,
    ensEQCDASFeatureCategoryTranscriptPerfect,
    ensEQCDASFeatureCategoryTranscriptTolerance,
    ensEQCDASFeatureCategoryTranscriptPartial,
    ensEQCDASFeatureCategoryTranscriptMissing,
    ensEQCDASFeatureCategoryTranscript,
    ensEQCDASFeatureCategoryTranslationPerfect,
    ensEQCDASFeatureCategoryTranslationTolerance,
    ensEQCDASFeatureCategoryTranslationPartial,
    ensEQCDASFeatureCategoryTranslationMissing,
    ensEQCDASFeatureCategoryTranslation
};




/******************************************************************************
**
** Ensembl QC DAS Feature Type enumeration
**
******************************************************************************/

enum EnsEQCDASFeatureType
{
    ensEQCDASFeatureTypeNULL,
    ensEQCDASFeatureTypeUnknown,
    ensEQCDASFeatureTypeExonPerfect,
    ensEQCDASFeatureTypeExonPartial,
    ensEQCDASFeatureTypeExonMissing,
    ensEQCDASFeatureTypeExonFrameshift,
    ensEQCDASFeatureTypeExonGap,
    ensEQCDASFeatureTypeExon
};




/* @data EnsPQCDASFeature *****************************************************
**
** Ensembl QC DAS Feature
**
** @alias EnsSQCDASFeature
** @alias EnsOQCDASFeature
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPQCDASFeatureadaptor] Ensembl QC DAS Feature Adaptor
** @cc Bio::EnsEMBL::QC::DASFeature
** @cc 'das_feature' SQL table
** @attr QCAlignment [EnsPQCAlignment] Ensembl QC Alignment
** @attr Analysis [EnsPAnalysis] Ensembl Analysis
** @attr SegmentSequence [EnsPQCSequence] Segment Ensembl QC Sequence
** @attr FeatureSequence [EnsPQCSequence] Feature Ensembl QC Sequence
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

typedef struct EnsSQCDASFeature
{
    ajuint Use;
    ajuint Identifier;
    EnsPQCDASFeatureadaptor Adaptor;
    EnsPQCAlignment QCAlignment;
    EnsPAnalysis Analysis;
    EnsPQCSequence SegmentSequence;
    EnsPQCSequence FeatureSequence;
    ajuint SegmentStart;
    ajuint SegmentEnd;
    ajint SegmentStrand;
    ajuint FeatureStart;
    ajuint FeatureEnd;
    ajint Phase;
    AjEnum Category;
    AjEnum Type;
} EnsOQCDASFeature;

#define EnsPQCDASFeature EnsOQCDASFeature*




/******************************************************************************
**
** Ensembl QC Variation Adaptor
**
******************************************************************************/

#define EnsPQCVariationadaptor EnsPBaseadaptor




/******************************************************************************
**
** Ensembl QC Variation Class enumeration
**
******************************************************************************/

enum EnsEQCVariationClass
{
    ensEQCVariationClassNULL,
    ensEQCVariationClassNone,
    ensEQCVariationClassSimple,
    ensEQCVariationClassSplice,
    ensEQCVariationClassExon
};




/******************************************************************************
**
** Ensembl QC Variation Type enumeration
**
******************************************************************************/

enum EnsEQCVariationType
{
    ensEQCVariationTypeNULL,
    ensEQCVariationTypeNone,
    ensEQCVariationTypeSingle,
    ensEQCVariationTypeMulti
};




/******************************************************************************
**
** Ensembl QC Variation State enumeration
**
******************************************************************************/

enum EnsEQCVariationState
{
    ensEQCVariationStateNULL,
    ensEQCVariationStateNone,
    ensEQCVariationStateMatch,
    ensEQCVariationStateGap,
    ensEQCVariationStateFrameshift,
    ensEQCVariationState5ss,
    ensEQCVariationState3ss,
    EnsEQCVariationStateSplit
};




/* @data EnsPQCVariation ******************************************************
**
** Ensembl QC Variation
**
** @alias EnsSQCVariation
** @alias EnsOQCVariation
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPQCVariationadaptor] Ensembl QC Variation Adaptor
** @cc Bio::EnsEMBL::QC::Variation
** @cc 'variation' SQL table
** @attr Analysis [EnsPAnalysis] Ensembl Analysis
** @attr QCAlignment [EnsPQCAlignment] Ensembl QC Alignment
** @attr QuerySequence [EnsPQCSequence] Query Ensembl QC Sequence
** @attr QueryStart [ajuint] Query start
** @attr QueryEnd [ajuint] Query end
** @attr QueryString [AjPStr] Query string
** @attr TargetSequence [EnsPQCSequence] Target Ensembl QC Sequence
** @attr TargetStart [ajuint] Target start
** @attr TargetEnd [ajuint] Target end
** @attr TargetString [AjPStr] Target string
** @attr Class [AjEnum] Class
** @attr Type [AjEnum] Type
** @attr State [AjEnum] State
** @attr Padding [char[4]] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSQCVariation
{
    ajuint Use;
    ajuint Identifier;
    EnsPQCVariationadaptor Adaptor;
    EnsPAnalysis Analysis;
    EnsPQCAlignment QCAlignment;
    EnsPQCSequence QuerySequence;
    ajuint QueryStart;
    ajuint QueryEnd;
    AjPStr QueryString;
    EnsPQCSequence TargetSequence;
    ajuint TargetStart;
    ajuint TargetEnd;
    AjPStr TargetString;
    AjEnum Class;
    AjEnum Type;
    AjEnum State;
    char Padding[4];
} EnsOQCVariation;

#define EnsPQCVariation EnsOQCVariation*




/******************************************************************************
**
** Ensembl QC Submission Adaptor
**
******************************************************************************/

#define EnsPQCSubmissionadaptor EnsPBaseadaptor




/* @data EnsPQCSubmission *****************************************************
**
** Ensembl QC Submission
**
** @alias EnsSQCSubmission
** @alias EnsOQCSubmission
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPQCSubmissionadaptor] Ensembl QC Submission Adaptor
** @cc Bio::EnsEMBL::QC::Submission
** @cc 'submission' SQL table
** @attr Analysis [EnsPAnalysis] Ensembl Analysis
** @attr QuerySequence [EnsPQCSequence] Query Ensembl QC Sequence
** @attr TargetSequence [EnsPQCSequence] Target Ensembl QC Sequence
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

typedef struct EnsSQCSubmission
{
    ajuint Use;
    ajuint Identifier;
    EnsPQCSubmissionadaptor Adaptor;
    EnsPAnalysis Analysis;
    EnsPQCSequence QuerySequence;
    EnsPQCSequence TargetSequence;
    ajuint QueryStart;
    ajuint QueryEnd;
    ajint QueryStrand;
    ajuint TargetStart;
    ajuint TargetEnd;
    ajint TargetStrand;
    ajuint AnalysisJobIdentifier;
    ajuint Padding;
} EnsOQCSubmission;

#define EnsPQCSubmission EnsOQCSubmission*




/*
** Prototype definitions
*/

/* Ensembl Quality Check DAS Feature */

EnsPQCDASFeature ensQCDASFeatureNew(EnsPQCDASFeatureadaptor adaptor,
                                    ajuint identifier,
                                    EnsPQCAlignment qca,
                                    EnsPAnalysis analysis,
                                    EnsPQCSequence segment,
                                    ajuint sstart,
                                    ajuint send,
                                    ajint sstrand,
                                    EnsPQCSequence feature,
                                    ajuint fstart,
                                    ajuint fend,
                                    ajint phase,
                                    AjEnum category,
                                    AjEnum type);

EnsPQCDASFeature ensQCDASFeatureNewObj(const EnsPQCDASFeature object);

EnsPQCDASFeature ensQCDASFeatureNewRef(EnsPQCDASFeature qcdasf);

void ensQCDASFeatureDel(EnsPQCDASFeature* Pqcdasf);

EnsPQCDASFeatureadaptor ensQCDASFeatureGetadaptor(
    const EnsPQCDASFeature qcdasf);

ajuint ensQCDASFeatureGetIdentifier(const EnsPQCDASFeature qcdasf);

EnsPQCAlignment ensQCDASFeatureGetQCAlignment(const EnsPQCDASFeature qcdasf);

EnsPAnalysis ensQCDASFeatureGetAnalysis(const EnsPQCDASFeature qcdasf);

EnsPQCSequence ensQCDASFeatureGetSegmentSequence(const EnsPQCDASFeature qcdasf);

EnsPQCSequence ensQCDASFeatureGetFeatureSequence(const EnsPQCDASFeature qcdasf);

ajuint ensQCDASFeatureGetSegmentStart(const EnsPQCDASFeature qcdasf);

ajuint ensQCDASFeatureGetSegmentEnd(const EnsPQCDASFeature qcdasf);

ajint ensQCDASFeatureGetSegmentStrand(const EnsPQCDASFeature qcdasf);

ajuint ensQCDASFeatureGetFeatureStart(const EnsPQCDASFeature qcdasf);

ajuint ensQCDASFeatureGetFeatureEnd(const EnsPQCDASFeature qcdasf);

ajint ensQCDASFeatureGetPhase(const EnsPQCDASFeature qcdasf);

AjEnum ensQCDASFeatureGetCategory(const EnsPQCDASFeature qcdasf);

AjEnum ensQCDASFeatureGetType(const EnsPQCDASFeature qcdasf);

AjBool ensQCDASFeatureSetadaptor(EnsPQCDASFeature qcdasf,
                                 EnsPQCDASFeatureadaptor adaptor);

AjBool ensQCDASFeatureSetIdentifier(EnsPQCDASFeature qcdasf, ajuint identifier);

AjBool ensQCDASFeatureSetQCAlignment(EnsPQCDASFeature qcdasf,
                                     EnsPQCAlignment qca);

AjBool ensQCDASFeatureSetAnalysis(EnsPQCDASFeature qcdasf,
                                  EnsPAnalysis analysis);

AjBool ensQCDASFeatureSetSegmentSequence(EnsPQCDASFeature qcdasf,
                                         EnsPQCSequence qcs);

AjBool ensQCDASFeatureSetFeatureSequence(EnsPQCDASFeature qcdasf,
                                         EnsPQCSequence qcs);

AjBool ensQCDASFeatureSetSegmentStart(EnsPQCDASFeature qcdasf, ajuint start);

AjBool ensQCDASFeatureSetSegmentEnd(EnsPQCDASFeature qcdasf, ajuint end);

AjBool ensQCDASFeatureSetSegmentStrand(EnsPQCDASFeature qcdasf, ajint strand);

AjBool ensQCDASFeatureSetFeatureStart(EnsPQCDASFeature qcdasf, ajuint start);

AjBool ensQCDASFeatureSetFeatureEnd(EnsPQCDASFeature qcdasf, ajuint end);

AjBool ensQCDASFeatureSetPhase(EnsPQCDASFeature qcdasf, ajint phase);

AjBool ensQCDASFeatureSetCategory(EnsPQCDASFeature qcdasf, AjEnum category);

AjBool ensQCDASFeatureSetType(EnsPQCDASFeature qcdasf, AjEnum type);

ajuint ensQCDASFeatureGetMemSize(const EnsPQCDASFeature qcdasf);

AjBool ensQCDASFeatureTrace(const EnsPQCDASFeature qcdasf, ajuint level);

AjEnum ensQCDASFeatureCategoryFromStr(const AjPStr category);

AjEnum ensQCDASFeatureTypeFromStr(const AjPStr type);

const char *ensQCDASFeatureCategoryToChar(const AjEnum category);

const char *ensQCDASFeatureTypeToChar(const AjEnum type);

/* Ensembl Quality Check DAS Feature Adaptor */

EnsPQCDASFeatureadaptor ensQCDASFeatureadaptorNew(EnsPDatabaseadaptor dba);

void ensQCDASFeatureadaptorDel(EnsPQCDASFeatureadaptor* Pqcdasfa);

AjBool ensQCDASFeatureadaptorFetchByIdentifier(EnsPQCDASFeatureadaptor adaptor,
                                               ajuint identifier,
                                               EnsPQCDASFeature *Pqcdasf);

AjBool ensQCDASFeatureadaptorFetchAllByQCAlignment(
    EnsPQCDASFeatureadaptor qcdasfa,
    const EnsPQCAlignment qca,
    AjPList qcdasfs);

AjBool ensQCDASFeatureadaptorFetchAllByFeature(EnsPQCDASFeatureadaptor qcdasfa,
                                               const EnsPAnalysis analysis,
                                               const EnsPQCSequence feature,
                                               AjPList qcdasfs);

AjBool ensQCDASFeatureadaptorFetchAllBySegment(EnsPQCDASFeatureadaptor qcdasfa,
                                               const EnsPAnalysis analysis,
                                               const EnsPQCSequence segment,
                                               AjPList qcdasfs);

AjBool ensQCDASFeatureadaptorFetchAllByAFS(EnsPQCDASFeatureadaptor qcdasfa,
                                           const EnsPAnalysis analysis,
                                           const EnsPQCSequence feature,
                                           const EnsPQCSequence segment,
                                           AjPList qcdasfs);

AjBool ensQCDASFeatureadaptorFetchAllByRegion(EnsPQCDASFeatureadaptor qcdasfa,
                                              const EnsPAnalysis analysis,
                                              const EnsPQCSequence segment,
                                              ajuint start,
                                              ajuint end,
                                              AjPList qcdasfs);

AjBool ensQCDASFeatureadaptorStore(EnsPQCDASFeatureadaptor qcdasfa,
                                   EnsPQCDASFeature qcdasf);

AjBool ensQCDASFeatureadaptorUpdate(EnsPQCDASFeatureadaptor qcdasfa,
                                    const EnsPQCDASFeature qcdasf);

AjBool ensQCDASFeatureadaptorDelete(EnsPQCDASFeatureadaptor qcdasfa,
                                    const EnsPQCDASFeature qcdasf);

/* Ensembl Quality Check Variation */

EnsPQCVariation ensQCVariationNew(EnsPQCVariationadaptor adaptor,
                                  ajuint identifier,
                                  EnsPQCAlignment qca,
                                  EnsPAnalysis analysis,
                                  EnsPQCSequence qsequence,
                                  ajuint qstart,
                                  ajuint qend,
                                  AjPStr qstring,
                                  EnsPQCSequence tsequence,
                                  ajuint tstart,
                                  ajuint tend,
                                  AjPStr tstring,
                                  AjEnum class,
                                  AjEnum type,
                                  AjEnum state);

EnsPQCVariation ensQCVariationNewObj(const EnsPQCVariation object);

EnsPQCVariation ensQCVariationNewRef(EnsPQCVariation qcv);

void ensQCVariationDel(EnsPQCVariation* Pqcv);

EnsPQCVariationadaptor ensQCVariationGetadaptor(const EnsPQCVariation qcv);

ajuint ensQCVariationGetIdentifier(const EnsPQCVariation qcv);

EnsPQCAlignment ensQCVariationGetQCAlignment(const EnsPQCVariation qcv);

EnsPAnalysis ensQCVariationGetAnalysis(const EnsPQCVariation qcv);

EnsPQCSequence ensQCVariationGetQuerySequence(const EnsPQCVariation qcv);

ajuint ensQCVariationGetQueryStart(const EnsPQCVariation qcv);

ajuint ensQCVariationGetQueryEnd(const EnsPQCVariation qcv);

AjPStr ensQCVariationGetQueryString(const EnsPQCVariation qcv);

EnsPQCSequence ensQCVariationGetTargetSequence(const EnsPQCVariation qcv);

ajuint ensQCVariationGetTargetStart(const EnsPQCVariation qcv);

ajuint ensQCVariationGetTargetEnd(const EnsPQCVariation qcv);

AjPStr ensQCVariationGetTargetString(const EnsPQCVariation qcv);

AjEnum ensQCVariationGetClass(const EnsPQCVariation qcv);

AjEnum ensQCVariationGetType(const EnsPQCVariation qcv);

AjEnum ensQCVariationGetState(const EnsPQCVariation qcv);

AjBool ensQCVariationSetadaptor(EnsPQCVariation qcv,
                                EnsPQCVariationadaptor qcva);

AjBool ensQCVariationSetIdentifier(EnsPQCVariation qcv, ajuint identifier);

AjBool ensQCVariationSetQCAlignment(EnsPQCVariation qcv, EnsPQCAlignment qca);

AjBool ensQCVariationSetAnalysis(EnsPQCVariation qcv, EnsPAnalysis analysis);

AjBool ensQCVariationSetQuerySequence(EnsPQCVariation qcv,
                                      EnsPQCSequence qsequence);

AjBool ensQCVariationSetQueryStart(EnsPQCVariation qcv, ajuint qstart);

AjBool ensQCVariationSetQueryEnd(EnsPQCVariation qcv, ajuint qend);

AjBool ensQCVariationSetQueryString(EnsPQCVariation qcv, AjPStr qstring);

AjBool ensQCVariationSetTargetSequence(EnsPQCVariation qcv,
                                       EnsPQCSequence tsequence);

AjBool ensQCVariationSetTargetStart(EnsPQCVariation qcv, ajuint tstart);

AjBool ensQCVariationSetTargetEnd(EnsPQCVariation qcv, ajuint tend);

AjBool ensQCVariationSetTargetString(EnsPQCVariation qcv, AjPStr tstring);

AjBool ensQCVariationSetClass(EnsPQCVariation qcv, AjEnum class);

AjBool ensQCVariationSetType(EnsPQCVariation qcv, AjEnum type);

AjBool ensQCVariationSetState(EnsPQCVariation qcv, AjEnum state);

ajuint ensQCVariationGetMemSize(const EnsPQCVariation qcv);

AjBool ensQCVariationTrace(const EnsPQCVariation qcv, ajuint level);

AjEnum ensQCVariationClassFromStr(const AjPStr vclass);

AjEnum ensQCVariationTypeFromStr(const AjPStr type);

AjEnum ensQCVariationStateFromStr(const AjPStr state);

const char *ensQCVariationClassToChar(const AjEnum vclass);

const char *ensQCVariationTypeToChar(const AjEnum type);

const char *ensQCVariationStateToChar(const AjEnum state);

/* Ensembl Quality Check Variation Adaptor */

EnsPQCVariationadaptor ensQCVariationadaptorNew(EnsPDatabaseadaptor dba);

void ensQCVariationadaptorDel(EnsPQCVariationadaptor* Pqcva);

AjBool ensQCVariationadaptorFetchByIdentifier(EnsPQCVariationadaptor adaptor,
                                              ajuint identifier,
                                              EnsPQCVariation *Pqcvf);

AjBool ensQCVariationadaptorFetchAllByQCAlignment(EnsPQCVariationadaptor qcva,
                                                  const EnsPQCAlignment qca,
                                                  AjPList qcvs);

AjBool ensQCVariationadaptorFetchAllByAnalysisQueryTarget(
    EnsPQCVariationadaptor qcva,
    const EnsPAnalysis analysis,
    const EnsPQCDatabase qdb,
    const EnsPQCDatabase tdb,
    AjPList qcvs);

AjBool ensQCVariationadaptorFetchAllByQuery(EnsPQCVariationadaptor qcva,
                                            const EnsPAnalysis analysis,
                                            const EnsPQCDatabase qdb,
                                            AjPList qcvs);

AjBool ensQCVariationadaptorFetchAllByTarget(EnsPQCVariationadaptor qcva,
                                             const EnsPAnalysis analysis,
                                             const EnsPQCDatabase tdb,
                                             AjPList qcvs);

AjBool ensQCVariationadaptorStore(EnsPQCVariationadaptor qcva,
                                  EnsPQCVariation qcv);

AjBool ensQCVariationadaptorUpdate(EnsPQCVariationadaptor qcva,
                                   const EnsPQCVariation qcv);

AjBool ensQCVariationadaptorDelete(EnsPQCVariationadaptor qcva,
                                   const EnsPQCVariation qcv);

/* Ensembl Quality Check Submission */

EnsPQCSubmission ensQCSubmissionNew(EnsPQCSubmissionadaptor adaptor,
                                    ajuint identifier,
                                    EnsPAnalysis analysis,
                                    EnsPQCSequence qsequence,
                                    EnsPQCSequence tsequence,
                                    ajuint tstart,
                                    ajuint tend,
                                    ajint tstrand,
                                    ajuint analysisjobid);

EnsPQCSubmission ensQCSubmissionNewObj(const EnsPQCSubmission object);

EnsPQCSubmission ensQCSubmissionNewRef(EnsPQCSubmission qcs);

void ensQCSubmissionDel(EnsPQCSubmission* Pqcs);

EnsPQCSubmissionadaptor ensQCSubmissionGetadaptor(const EnsPQCSubmission qcs);

ajuint ensQCSubmissionGetIdentifier(const EnsPQCSubmission qcs);

EnsPAnalysis ensQCSubmissionGetAnalysis(const EnsPQCSubmission qcs);

EnsPQCSequence ensQCSubmissionGetQuerySequence(const EnsPQCSubmission qcs);

ajuint ensQCSubmissionGetQueryStart(const EnsPQCSubmission qcs);

ajuint ensQCSubmissionGetQueryEnd(const EnsPQCSubmission qcs);

ajint ensQCSubmissionGetQueryStrand(const EnsPQCSubmission qcs);

EnsPQCSequence ensQCSubmissionGetTargetSequence(const EnsPQCSubmission qcs);

ajuint ensQCSubmissionGetTargetStart(const EnsPQCSubmission qcs);

ajuint ensQCSubmissionGetTargetEnd(const EnsPQCSubmission qcs);

ajint ensQCSubmissionGetTargetStrand(const EnsPQCSubmission qcs);

ajuint ensQCSubmissionGetAnalysisJobIdentifier(const EnsPQCSubmission qcs);

AjBool ensQCSubmissionSetadaptor(EnsPQCSubmission qcs,
                                 EnsPQCSubmissionadaptor qcsa);

AjBool ensQCSubmissionSetIdentifier(EnsPQCSubmission qcs, ajuint identifier);

AjBool ensQCSubmissionSetAnalysis(EnsPQCSubmission qcs, EnsPAnalysis analysis);

AjBool ensQCSubmissionSetQuerySequence(EnsPQCSubmission qcs,
                                       EnsPQCSequence qcseq);

AjBool ensQCSubmissionSetQueryStart(EnsPQCSubmission qcs, ajuint qstart);

AjBool ensQCSubmissionSetQueryEnd(EnsPQCSubmission qcs, ajuint qend);

AjBool ensQCSubmissionSetQueryStrand(EnsPQCSubmission qcs, ajint qstrand);

AjBool ensQCSubmissionSetTargetSequence(EnsPQCSubmission qcs,
                                        EnsPQCSequence qcseq);

AjBool ensQCSubmissionSetTargetStart(EnsPQCSubmission qcs, ajuint tstart);

AjBool ensQCSubmissionSetTargetEnd(EnsPQCSubmission qcs, ajuint tend);

AjBool ensQCSubmissionSetTargetStrand(EnsPQCSubmission qcs, ajint tstrand);

AjBool ensQCSubmissionSetAnalysisJobIdentifier(EnsPQCSubmission qcs,
                                               ajuint jobid);

ajuint ensQCSubmissionGetMemSize(const EnsPQCSubmission qcs);

AjBool ensQCSubmissionTrace(const EnsPQCSubmission qcs, ajuint level);

/* Ensembl Quality Check Submission Adaptor */

EnsPQCSubmissionadaptor ensQCSubmissionadaptorNew(EnsPDatabaseadaptor dba);

void ensQCSubmissionadaptorDel(EnsPQCSubmissionadaptor* Pqcsa);

AjBool ensQCSubmissionadaptorFetchByIdentifier(EnsPQCSubmissionadaptor adaptor,
                                               ajuint identifier,
                                               EnsPQCSubmission *Pqcs);

AjBool ensQCSubmissionadaptorFetchAllByAnalysisQueryTarget(
    EnsPQCSubmissionadaptor qcsa,
    const EnsPAnalysis analysis,
    const EnsPQCDatabase qdb,
    const EnsPQCDatabase tdb,
    AjPList qcss);

AjBool ensQCSubmissionadaptorFetchAllByANQIDTDB(EnsPQCSubmissionadaptor qcsa,
                                                const EnsPAnalysis analysis,
                                                const EnsPQCSequence qseqeunce,
                                                const EnsPQCDatabase tdb,
                                                const EnsPQCSequence tsequence,
                                                ajuint tstart,
                                                ajuint tend,
                                                ajint tstrand,
                                                AjPList qcss);

AjBool ensQCSubmissionadaptorFetchAllByQuery(EnsPQCSubmissionadaptor qcsa,
                                             const EnsPAnalysis analysis,
                                             const EnsPQCDatabase qdb,
                                             AjPList qcss);

AjBool ensQCSubmissionadaptorFetchAllByTarget(EnsPQCSubmissionadaptor qcsa,
                                              const EnsPAnalysis analysis,
                                              const EnsPQCDatabase tdb,
                                              AjPList qcss);

AjBool ensQCSubmissionadaptorStore(EnsPQCSubmissionadaptor qcsa,
                                   EnsPQCSubmission qcs);

AjBool ensQCSubmissionadaptorUpdate(EnsPQCSubmissionadaptor qcsa,
                                    const EnsPQCSubmission qcs);

AjBool ensQCSubmissionadaptorDelete(EnsPQCSubmissionadaptor qcsa,
                                    const EnsPQCSubmission qcs);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
