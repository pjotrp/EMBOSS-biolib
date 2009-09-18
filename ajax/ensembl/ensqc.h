#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensqc_h
#define ensqc_h

#include "ensanalysis.h"
#include "ensqcalignment.h"
#include "ensqcsequence.h"




/* @data EnsPQCDASFeatureAdaptor **********************************************
**
** Ensembl QC DAS Feature Adaptor
**
** @alias EnsPBaseAdaptor
** @@
******************************************************************************/

#define EnsPQCDASFeatureAdaptor EnsPBaseAdaptor




/* @datatype EnsEQCDASFeatureCategory *****************************************
**
** Ensembl QC DAS Feature Category enumeration
**
** @attr enum [EnsEQCDASFeatureCategory] Value
** @@
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




/* @datatype EnsEQCDASFeatureType *********************************************
**
** Ensembl QC DAS Feature Type enumeration
**
** @attr enum [EnsEQCDASFeatureType] Value
** @@
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
** @attr Adaptor [EnsPQCAlignmentAdaptor] Ensembl Alignment Adaptor
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
    EnsPQCDASFeatureAdaptor Adaptor;
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




/* @data EnsPQCVariationAdaptor ***********************************************
**
** Ensembl QC Variation Adaptor
**
** @alias EnsPBaseAdaptor
** @@
******************************************************************************/

#define EnsPQCVariationAdaptor EnsPBaseAdaptor




/* @datatype EnsEQCVariationClass *********************************************
**
** Ensembl QC Variation Class enumeration
**
** @attr enum [EnsEQCVariationClass Value
** @@
******************************************************************************/

enum EnsEQCVariationClass
{
    ensEQCVariationClassNULL,
    ensEQCVariationClassNone,
    ensEQCVariationClassSimple,
    ensEQCVariationClassSplice,
    ensEQCVariationClassExon
};




/* @datatype EnsEQCVariationType **********************************************
**
** Ensembl QC Variation Type enumeration
**
** @attr enum [EnsEQCVariationType] Value
** @@
******************************************************************************/

enum EnsEQCVariationType
{
    ensEQCVariationTypeNULL,
    ensEQCVariationTypeNone,
    ensEQCVariationTypeSingle,
    ensEQCVariationTypeMulti
};




/* @datatype EnsEQCVariationState *********************************************
**
** Ensembl QC Variation State enumeration
**
** @attr enum [EnsEQCVariationState] Value
** @@
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
** @attr Adaptor [EnsPQCVariationAdaptor] Ensembl QC Variation Adaptor
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
** @@
******************************************************************************/

typedef struct EnsSQCVariation
{
    ajuint Use;
    ajuint Identifier;
    EnsPQCVariationAdaptor Adaptor;
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
} EnsOQCVariation;

#define EnsPQCVariation EnsOQCVariation*




/* @data EnsPQCSubmissionAdaptor **********************************************
**
** Ensembl QC Submission Adaptor
**
** @alias EnsPBaseAdaptor
** @@
******************************************************************************/

#define EnsPQCSubmissionAdaptor EnsPBaseAdaptor




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
** @attr Adaptor [EnsPSubmissionAdaptor] Ensembl QC Submission Adaptor
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
    EnsPQCSubmissionAdaptor Adaptor;
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

EnsPQCDASFeature ensQCDASFeatureNew(EnsPQCDASFeatureAdaptor adaptor,
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

EnsPQCDASFeatureAdaptor ensQCDASFeatureGetAdaptor(
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

AjBool ensQCDASFeatureSetAdaptor(EnsPQCDASFeature qcdasf,
                                 EnsPQCDASFeatureAdaptor adaptor);

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

EnsPQCDASFeatureAdaptor ensQCDASFeatureAdaptorNew(EnsPDatabaseAdaptor dba);

void ensQCDASFeatureAdaptorDel(EnsPQCDASFeatureAdaptor* Pqcdasfa);

AjBool ensQCDASFeatureAdaptorFetchByIdentifier(EnsPQCDASFeatureAdaptor adaptor,
                                               ajuint identifier,
                                               EnsPQCDASFeature *Pqcdasf);

AjBool ensQCDASFeatureAdaptorFetchAllByQCAlignment(
    EnsPQCDASFeatureAdaptor qcdasfa,
    const EnsPQCAlignment qca,
    AjPList qcdasfs);

AjBool ensQCDASFeatureAdaptorFetchAllByFeature(EnsPQCDASFeatureAdaptor qcdasfa,
                                               const EnsPAnalysis analysis,
                                               const EnsPQCSequence feature,
                                               AjPList qcdasfs);

AjBool ensQCDASFeatureAdaptorFetchAllBySegment(EnsPQCDASFeatureAdaptor qcdasfa,
                                               const EnsPAnalysis analysis,
                                               const EnsPQCSequence segment,
                                               AjPList qcdasfs);

AjBool ensQCDASFeatureAdaptorFetchAllByAFS(EnsPQCDASFeatureAdaptor qcdasfa,
                                           const EnsPAnalysis analysis,
                                           const EnsPQCSequence feature,
                                           const EnsPQCSequence segment,
                                           AjPList qcdasfs);

AjBool ensQCDASFeatureAdaptorFetchAllByRegion(EnsPQCDASFeatureAdaptor qcdasfa,
                                              const EnsPAnalysis analysis,
                                              const EnsPQCSequence segment,
                                              ajuint start,
                                              ajuint end,
                                              AjPList qcdasfs);

AjBool ensQCDASFeatureAdaptorStore(EnsPQCDASFeatureAdaptor qcdasfa,
                                   EnsPQCDASFeature qcdasf);

AjBool ensQCDASFeatureAdaptorUpdate(EnsPQCDASFeatureAdaptor qcdasfa,
                                    const EnsPQCDASFeature qcdasf);

AjBool ensQCDASFeatureAdaptorDelete(EnsPQCDASFeatureAdaptor qcdasfa,
                                    const EnsPQCDASFeature qcdasf);

/* Ensembl Quality Check Variation */

EnsPQCVariation ensQCVariationNew(EnsPQCVariationAdaptor adaptor,
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

EnsPQCVariationAdaptor ensQCVariationGetAdaptor(const EnsPQCVariation qcv);

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

AjBool ensQCVariationSetAdaptor(EnsPQCVariation qcv,
                                EnsPQCVariationAdaptor qcva);

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

EnsPQCVariationAdaptor ensQCVariationAdaptorNew(EnsPDatabaseAdaptor dba);

void ensQCVariationAdaptorDel(EnsPQCVariationAdaptor* Pqcva);

AjBool ensQCVariationAdaptorFetchByIdentifier(EnsPQCVariationAdaptor adaptor,
                                              ajuint identifier,
                                              EnsPQCVariation *Pqcvf);

AjBool ensQCVariationAdaptorFetchAllByQCAlignment(EnsPQCVariationAdaptor qcva,
                                                  const EnsPQCAlignment qca,
                                                  AjPList qcvs);

AjBool ensQCVariationAdaptorFetchAllByAnalysisQueryTarget(
    EnsPQCVariationAdaptor qcva,
    const EnsPAnalysis analysis,
    const EnsPQCDatabase qdb,
    const EnsPQCDatabase tdb,
    AjPList qcvs);

AjBool ensQCVariationAdaptorFetchAllByQuery(EnsPQCVariationAdaptor qcva,
                                            const EnsPAnalysis analysis,
                                            const EnsPQCDatabase qdb,
                                            AjPList qcvs);

AjBool ensQCVariationAdaptorFetchAllByTarget(EnsPQCVariationAdaptor qcva,
                                             const EnsPAnalysis analysis,
                                             const EnsPQCDatabase tdb,
                                             AjPList qcvs);

AjBool ensQCVariationAdaptorStore(EnsPQCVariationAdaptor qcva,
                                  EnsPQCVariation qcv);

AjBool ensQCVariationAdaptorUpdate(EnsPQCVariationAdaptor qcva,
                                   const EnsPQCVariation qcv);

AjBool ensQCVariationAdaptorDelete(EnsPQCVariationAdaptor qcva,
                                   const EnsPQCVariation qcv);

/* Ensembl Quality Check Submission */

EnsPQCSubmission ensQCSubmissionNew(EnsPQCSubmissionAdaptor adaptor,
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

EnsPQCSubmissionAdaptor ensQCSubmissionGetAdaptor(const EnsPQCSubmission qcs);

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

AjBool ensQCSubmissionSetAdaptor(EnsPQCSubmission qcs,
                                 EnsPQCSubmissionAdaptor qcsa);

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

EnsPQCSubmissionAdaptor ensQCSubmissionAdaptorNew(EnsPDatabaseAdaptor dba);

void ensQCSubmissionAdaptorDel(EnsPQCSubmissionAdaptor* Pqcsa);

AjBool ensQCSubmissionAdaptorFetchByIdentifier(EnsPQCSubmissionAdaptor adaptor,
                                               ajuint identifier,
                                               EnsPQCSubmission *Pqcs);

AjBool ensQCSubmissionAdaptorFetchAllByAnalysisQueryTarget(
    EnsPQCSubmissionAdaptor qcsa,
    const EnsPAnalysis analysis,
    const EnsPQCDatabase qdb,
    const EnsPQCDatabase tdb,
    AjPList qcss);

AjBool ensQCSubmissionAdaptorFetchAllByANQIDTDB(EnsPQCSubmissionAdaptor qcsa,
                                                const EnsPAnalysis analysis,
                                                const EnsPQCSequence qseqeunce,
                                                const EnsPQCDatabase tdb,
                                                const EnsPQCSequence tsequence,
                                                ajuint tstart,
                                                ajuint tend,
                                                ajint tstrand,
                                                AjPList qcss);

AjBool ensQCSubmissionAdaptorFetchAllByQuery(EnsPQCSubmissionAdaptor qcsa,
                                             const EnsPAnalysis analysis,
                                             const EnsPQCDatabase qdb,
                                             AjPList qcss);

AjBool ensQCSubmissionAdaptorFetchAllByTarget(EnsPQCSubmissionAdaptor qcsa,
                                              const EnsPAnalysis analysis,
                                              const EnsPQCDatabase tdb,
                                              AjPList qcss);

AjBool ensQCSubmissionAdaptorStore(EnsPQCSubmissionAdaptor qcsa,
                                   EnsPQCSubmission qcs);

AjBool ensQCSubmissionAdaptorUpdate(EnsPQCSubmissionAdaptor qcsa,
                                    const EnsPQCSubmission qcs);

AjBool ensQCSubmissionAdaptorDelete(EnsPQCSubmissionAdaptor qcsa,
                                    const EnsPQCSubmission qcs);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
