#ifdef __cplusplus
extern "C"
{
#endif

#ifndef enstranslation_h
#define enstranslation_h

#include "ensdata.h"
#include "ensfeature.h"




/*
** Prototype definitions
*/

void ensTranslationInit(void);

void ensTranslationExit(void);

AjPTrn ensTranslationCacheGetTranslation(ajint codontable);

/* Ensembl Translation */

EnsPTranslation ensTranslationNew(EnsPTranslationAdaptor adaptor,
                                  ajuint identifier,
                                  EnsPTranscript transcript,
                                  EnsPExon startexon,
                                  EnsPExon endexon,
                                  ajuint start,
                                  ajuint end,
                                  AjPStr sequence,
                                  AjPStr stableid,
                                  ajuint version,
                                  AjPStr cdate,
                                  AjPStr mdate);

EnsPTranslation ensTranslationNewObj(const EnsPTranslation object);

EnsPTranslation ensTranslationNewRef(EnsPTranslation translation);

void ensTranslationDel(EnsPTranslation* Ptranslation);

const EnsPTranslationAdaptor ensTranslationGetAdaptor(
    const EnsPTranslation translation);

ajuint ensTranslationGetIdentifier(const EnsPTranslation translation);

EnsPTranscript ensTranslationGetTranscript(const EnsPTranslation translation);

EnsPExon ensTranslationGetStartExon(const EnsPTranslation translation);

EnsPExon ensTranslationGetEndExon(const EnsPTranslation translation);

ajuint ensTranslationGetStart(const EnsPTranslation translation);

ajuint ensTranslationGetEnd(const EnsPTranslation translation);

AjPStr ensTranslationGetStableIdentifier(const EnsPTranslation translation);

ajuint ensTranslationGetVersion(const EnsPTranslation translation);
    
AjPStr ensTranslationGetCreationDate(const EnsPTranslation translation);

AjPStr ensTranslationGetModificationDate(const EnsPTranslation translation);

const AjPList ensTranslationGetAttributes(EnsPTranslation translation);

const AjPList ensTranslationGetDatabaseEntries(EnsPTranslation translation);

const AjPList ensTranslationGetProteinFeatures(EnsPTranslation translation);

ajuint ensTranslationGetTranscriptStart(EnsPTranslation translation);

ajuint ensTranslationGetTranscriptEnd(EnsPTranslation translation);

ajuint ensTranslationGetSliceStart(EnsPTranslation translation);

ajuint ensTranslationGetSliceEnd(EnsPTranslation translation);

ajuint ensTranslationGetMemSize(const EnsPTranslation translation);

AjBool ensTranslationSetAdaptor(EnsPTranslation translation,
                                EnsPTranslationAdaptor adaptor);

AjBool ensTranslationSetIdentifier(EnsPTranslation translation,
                                   ajuint identifier);

AjBool ensTranslationSetTranscript(EnsPTranslation translation,
                                   EnsPTranscript transcript);

AjBool ensTranslationSetStartExon(EnsPTranslation translation, EnsPExon exon);

AjBool ensTranslationSetEndExon(EnsPTranslation translation, EnsPExon exon);

AjBool ensTranslationSetStart(EnsPTranslation translation, ajuint start);

AjBool ensTranslationSetEnd(EnsPTranslation translation, ajuint end);

AjBool ensTranslationSetStableIdentifier(EnsPTranslation translation,
                                         AjPStr stableid);

AjBool ensTranslationSetVersion(EnsPTranslation translation, ajuint version);

AjBool ensTranslationSetCreationDate(EnsPTranslation translation, AjPStr cdate);

AjBool ensTranslationSetModificationDate(EnsPTranslation translation,
                                         AjPStr mdate);

AjBool ensTranslationAddAttribute(EnsPTranslation translation,
                                  EnsPAttribute attribute);

AjBool ensTranslationAddDatabaseEntry(EnsPTranslation translation,
                                      EnsPDatabaseEntry dbe);

AjBool ensTranslationAddProteinFeature(EnsPTranslation translation,
                                       EnsPProteinFeature pf);

AjBool ensTranslationFetchAllAttributes(EnsPTranslation translation,
                                        const AjPStr code,
                                        AjPList attributes);

AjBool ensTranslationFetchAllDatabaseEntries(EnsPTranslation translation,
                                             const AjPStr name,
                                             AjEnum type,
                                             AjPList dbes);

AjBool ensTranslationFetchAllProteinFeatures(EnsPTranslation translation,
                                             const AjPStr name,
                                             AjPList pfs);

AjBool ensTranslationFetchAllProteinFeaturesDomain(EnsPTranslation translation,
                                                   AjPList pfs);

AjBool ensTranslationFetchAllSequenceEdits(EnsPTranslation translation,
                                           AjPList selist);

AjBool ensTranslationFetchDisplayIdentifier(const EnsPTranslation translation,
                                            AjPStr* Pidentifier);
    
AjBool ensTranslationModify(EnsPTranslation translation, AjPStr* Psequence);

AjBool ensTranslationTrace(const EnsPTranslation translation, ajuint level);

AjBool ensTranslationFetchSequenceStr(EnsPTranslation translation,
                                      AjPStr *Psequence);

AjBool ensTranslationFetchSequenceSeq(EnsPTranslation translation,
                                      AjPSeq* Psequence);

/* Ensembl Translation Adaptor */

EnsPTranslationAdaptor ensTranslationAdaptorNew(EnsPDatabaseAdaptor dba);

void ensTranslationAdaptorDel(EnsPTranslationAdaptor *Padaptor);

EnsPDatabaseAdaptor ensTranslationAdaptorGetDatabaseAdaptor(
    EnsPTranslationAdaptor adaptor);

AjBool ensTranslationAdaptorFetchByIdentifier(EnsPTranslationAdaptor adaptor,
                                              ajuint identifier,
                                              EnsPTranslation *Ptranslation);

AjBool ensTranslationAdaptorFetchByStableIdentifier(
    EnsPTranslationAdaptor adaptor,
    const AjPStr stableid,
    ajuint version,
    EnsPTranslation *Ptranslation);

AjBool ensTranslationAdaptorFetchByTranscript(EnsPTranslationAdaptor adaptor,
                                              EnsPTranscript transcript,
                                              EnsPTranslation *Ptranslation);

AjBool ensTranslationAdaptorFetchAllByExternalName(
    EnsPTranslationAdaptor adaptor,
    AjPStr externalname,
    AjPStr externaldbname,
    AjPList translations);

AjBool ensTranslationAdaptorFetchAllByTranscriptList(
    EnsPTranslationAdaptor adaptor,
    AjPList transcripts,
    AjPList translations);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
