#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensqcalignment_h
#define ensqcalignment_h

#include "ensanalysis.h"
#include "ensqcsequence.h"




/******************************************************************************
**
** Ensembl QC Alignment Adaptor
**
******************************************************************************/

#define EnsPQCAlignmentadaptor EnsPBaseadaptor




/* @data EnsPQCAlignment ******************************************************
**
** Ensembl QC Alignment
**
** @alias EnsSQCAlignment
** @alias EnsOQCAlignment
**
** @attr Use [ajuint] Use counter
** @cc Bio::EnsEMBL::Storable
** @attr Identifier [ajuint] SQL database-internal identifier
** @attr Adaptor [EnsPQCAlignmentadaptor] Ensembl Alignment Adaptor
** @cc Bio::EnsEMBL::QC::Alignment
** @cc 'alignment' SQL table
** @attr Analysis [EnsPAnalysis] Ensembl Analysis
** @attr QuerySequence [EnsPQCSequence] Query Ensembl QC Sequence
** @attr TargetSequence [EnsPQCSequence] Target Ensembl QC Sequence
** @attr VULGAR [AjPStr] VULGAR line
** @attr QueryStart [ajuint] Query start
** @attr QueryEnd [ajuint] Query end
** @attr QueryStrand [ajint] Query strand
** @attr TargetStart [ajuint] Target start
** @attr TargetEnd [ajuint] Target end
** @attr TargetStrand [ajint] Target strand
** @attr SpliceStrand [ajint] Splice strand
** @attr Coverage [ajuint] Coverage score
** @attr Score [double] Score
** @attr Identity [float] Identity
** @attr Padding [char[4]] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct EnsSQCAlignment
{
    ajuint Use;
    ajuint Identifier;
    EnsPQCAlignmentadaptor Adaptor;
    EnsPAnalysis Analysis;
    EnsPQCSequence QuerySequence;
    EnsPQCSequence TargetSequence;
    AjPStr VULGAR;
    ajuint QueryStart;
    ajuint QueryEnd;
    ajint QueryStrand;
    ajuint TargetStart;
    ajuint TargetEnd;
    ajint TargetStrand;
    ajint SpliceStrand;
    ajuint Coverage;
    double Score;
    float Identity;
    char Padding[4];
} EnsOQCAlignment;

#define EnsPQCAlignment EnsOQCAlignment*




/*
** Prototype definitions
*/

/* Ensembl Quality Check Alignment */

EnsPQCAlignment ensQCAlignmentNew(EnsPQCAlignmentadaptor adaptor,
                                  ajuint identifier,
                                  EnsPAnalysis analysis,
                                  EnsPQCSequence qsequence,
                                  ajuint qstart,
                                  ajuint qend,
                                  ajint qstrand,
                                  EnsPQCSequence tsequence,
                                  ajuint tstart,
                                  ajuint tend,
                                  ajint tstrand,
                                  ajint sstrand,
                                  ajuint coverage,
                                  double score,
                                  float identity,
                                  AjPStr vulgar);

EnsPQCAlignment ensQCAlignmentNewObj(const EnsPQCAlignment object);

EnsPQCAlignment ensQCAlignmentNewRef(EnsPQCAlignment qca);

void ensQCAlignmentDel(EnsPQCAlignment* Pqca);

EnsPQCAlignmentadaptor ensQCAlignmentGetadaptor(const EnsPQCAlignment qca);

ajuint ensQCAlignmentGetIdentifier(const EnsPQCAlignment qca);

const EnsPAnalysis ensQCAlignmentGetAnalysis(const EnsPQCAlignment qca);

EnsPQCSequence ensQCAlignmentGetQuerySequence(const EnsPQCAlignment qca);

ajuint ensQCAlignmentGetQueryStart(const EnsPQCAlignment qca);

ajuint ensQCAlignmentGetQueryEnd(const EnsPQCAlignment qca);

ajint ensQCAlignmentGetQueryStrand(const EnsPQCAlignment qca);

EnsPQCSequence ensQCAlignmentGetTargetSequence(const EnsPQCAlignment qca);

ajuint ensQCAlignmentGetTargetStart(const EnsPQCAlignment qca);

ajuint ensQCAlignmentGetTargetEnd(const EnsPQCAlignment qca);

ajint ensQCAlignmentGetTargetStrand(const EnsPQCAlignment qca);

ajint ensQCAlignmentGetSpliceStrand(const EnsPQCAlignment qca);

ajuint ensQCAlignmentGetCoverage(const EnsPQCAlignment qca);

double ensQCAlignmentGetScore(const EnsPQCAlignment qca);

float ensQCAlignmentGetIdentity(const EnsPQCAlignment qca);

AjPStr ensQCAlignmentGetVULGAR(const EnsPQCAlignment qca);

AjBool ensQCAlignmentSetadaptor(EnsPQCAlignment qca,
                                EnsPQCAlignmentadaptor qcaa);

AjBool ensQCAlignmentSetIdentifier(EnsPQCAlignment qca, ajuint identifier);

AjBool ensQCAlignmentSetAnalysis(EnsPQCAlignment qca, EnsPAnalysis analysis);

AjBool ensQCAlignmentSetQuerySequence(EnsPQCAlignment qca, EnsPQCSequence qcs);

AjBool ensQCAlignmentSetQueryStart(EnsPQCAlignment qca, ajuint qstart);

AjBool ensQCAlignmentSetQueryEnd(EnsPQCAlignment qca, ajuint qend);

AjBool ensQCAlignmentSetQueryStrand(EnsPQCAlignment qca, ajint qstrand);

AjBool ensQCAlignmentSetTargetSequence(EnsPQCAlignment qca, EnsPQCSequence qcs);

AjBool ensQCAlignmentSetTargetStart(EnsPQCAlignment qca, ajuint tstart);

AjBool ensQCAlignmentSetTargetEnd(EnsPQCAlignment qca, ajuint tend);

AjBool ensQCAlignmentSetTargetStrand(EnsPQCAlignment qca, ajint tstrand);

AjBool ensQCAlignmentSetSpliceStrand(EnsPQCAlignment qca, ajint sstrand);

AjBool ensQCAlignmentSetCoverage(EnsPQCAlignment qca, ajuint coverage);

AjBool ensQCAlignmentSetScore(EnsPQCAlignment qca, double score);

AjBool ensQCAlignmentSetIdentity(EnsPQCAlignment qca, float identity);

AjBool ensQCAlignmentSetVULGAR(EnsPQCAlignment qca, AjPStr vulgar);

ajuint ensQCAlignmentGetMemSize(const EnsPQCAlignment qca);

AjBool ensQCAlignmentTrace(const EnsPQCAlignment qca, ajuint level);

AjBool ensQCAlignmentGetQueryCoordinates(const EnsPQCAlignment qca,
                                         ajint *Pstart,
                                         ajint *Pend,
                                         ajint *Pstrand,
                                         ajuint *Plength);

AjBool ensQCAlignmentGetTargetCoordinates(const EnsPQCAlignment qca,
                                          ajint *Pstart,
                                          ajint *Pend,
                                          ajint *Pstrand,
                                          ajuint *Plength);

AjBool ensQCAlignmentOverlapOnTarget(const EnsPQCAlignment qca1,
                                     const EnsPQCAlignment qca2);

AjBool ensQCAlignmentCalculateQueryCoverage(EnsPQCAlignment qca,
                                            float identity,
                                            ajuint edge);

AjBool ensQCAlignmentCalculateProteinToProteinQueryCoverage(EnsPQCAlignment qca,
                                                            float identity);

AjBool ensQCAlignmentCalculateProteinToGenomeQueryCoverage(EnsPQCAlignment qca,
                                                           float identity,
                                                           ajuint edge);

AjBool ensQCAlignmentCalculateDNAToDNAQueryCoverage(EnsPQCAlignment qca,
                                                    float identity,
                                                    ajuint edge);

AjBool ensQCAlignmentCalculateDNAToGenomeQueryCoverage(EnsPQCAlignment qca,
                                                       float identity,
                                                       ajuint edge);

ajuint ensQCAlignmentCalculateQueryToQueryCoverage(const EnsPQCAlignment qca1,
                                                   const EnsPQCAlignment qca2);

AjBool ensQCAlignmentReport(const EnsPQCAlignment qca,
                            AjPStr *Pstr,
                            AjBool internalquery,
                            AjBool internaltarget);

/* Ensembl Quality Check Alignment Adaptor */

EnsPQCAlignmentadaptor ensQCAlignmentadaptorNew(EnsPDatabaseadaptor dba);

void ensQCAlignmentadaptorDel(EnsPQCAlignmentadaptor* Pqcaa);

AjBool ensQCAlignmentadaptorFetchByIdentifier(EnsPQCAlignmentadaptor adaptor,
                                              ajuint identifier,
                                              EnsPQCAlignment *Pqca);

AjBool ensQCAlignmentadaptorFetchAllByAnalysisQueryTarget(
    EnsPQCAlignmentadaptor qcaa,
    const EnsPAnalysis analysis,
    const EnsPQCDatabase qdb,
    const EnsPQCDatabase tdb,
    AjPList qcas);


AjBool ensQCAlignmentadaptorFetchAllByQuery(EnsPQCAlignmentadaptor qcaa,
                                            const EnsPAnalysis analysis,
                                            const EnsPQCDatabase qdb,
                                            AjPList qcas);

AjBool ensQCAlignmentadaptorFetchAllByTarget(EnsPQCAlignmentadaptor qcaa,
                                             const EnsPAnalysis analysis,
                                             const EnsPQCDatabase tdb,
                                             AjPList qcas);

AjBool ensQCAlignmentadaptorFetchAllByCoverage(EnsPQCAlignmentadaptor qcaa,
                                               ajuint lower,
                                               ajuint upper,
                                               AjPList qcas);

AjBool ensQCAlignmentadaptorFetchAllByTargetLocation(
    EnsPQCAlignmentadaptor qcaa,
    const EnsPAnalysis analysis,
    const EnsPQCSequence tsequence,
    ajuint tstart,
    ajuint tend,
    AjPList qcas);

AjBool ensQCAlignmentadaptorStore(EnsPQCAlignmentadaptor qcaa,
                                  EnsPQCAlignment qca);

AjBool ensQCAlignmentadaptorUpdate(EnsPQCAlignmentadaptor qcaa,
                                   const EnsPQCAlignment qca);

AjBool ensQCAlignmentadaptorDelete(EnsPQCAlignmentadaptor qcaa,
                                   const EnsPQCAlignment qca);

void   endQCAlignmenttDummyFunction(void);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
