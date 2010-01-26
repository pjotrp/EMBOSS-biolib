#ifdef __cplusplus
extern "C"
{
#endif

#ifndef enstranscript_h
#define enstranscript_h

#include "ensdata.h"




/*
** Prototype definitions
*/

/* Ensembl Transcript */

EnsPTranscript ensTranscriptNew(EnsPTranscriptadaptor tca,
                                ajuint identifier,
                                EnsPFeature feature,
                                EnsPDatabaseentry displaydbe,
                                AjPStr description,
                                AjPStr biotype,
                                AjEnum status,
                                AjBool current,
                                AjPStr stableid,
                                ajuint version,
                                AjPStr cdate,
                                AjPStr mdate,
                                AjPList exons);

EnsPTranscript ensTranscriptNewObj(const EnsPTranscript object);

EnsPTranscript ensTranscriptNewRef(EnsPTranscript transcript);

void ensTranscriptDel(EnsPTranscript* Ptranscript);

EnsPTranscriptadaptor ensTranscriptGetAdaptor(const EnsPTranscript transcript);

ajuint ensTranscriptGetIdentifier(const EnsPTranscript transcript);

EnsPFeature ensTranscriptGetFeature(const EnsPTranscript transcript);

EnsPDatabaseentry ensTranscriptGetDisplayReference(
    const EnsPTranscript transcript);

AjPStr ensTranscriptGetDescription(const EnsPTranscript transcript);

AjPStr ensTranscriptGetBioType(const EnsPTranscript transcript);

AjEnum ensTranscriptGetStatus(const EnsPTranscript transcript);

ajuint ensTranscriptGetGeneIdentifier(const EnsPTranscript transcript);

AjBool ensTranscriptGetCurrent(const EnsPTranscript transcript);

AjPStr ensTranscriptGetStableIdentifier(const EnsPTranscript transcript);

ajuint ensTranscriptGetVersion(const EnsPTranscript transcript);

AjPStr ensTranscriptGetCreationDate(const EnsPTranscript transcript);

AjPStr ensTranscriptGetModificationDate(const EnsPTranscript transcript);

const AjPList ensTranscriptGetAttributes(EnsPTranscript transcript);

const AjPList ensTranscriptGetDatabaseEntries(EnsPTranscript transcript);

const AjPList ensTranscriptGetExons(EnsPTranscript transcript);

const AjPList ensTranscriptGetSupportingfeatures(EnsPTranscript transcript);

const AjPList ensTranscriptGetTranslations(const EnsPTranscript transcript);

ajuint ensTranscriptGetTranscriptCodingStart(EnsPTranscript transcript);

ajuint ensTranscriptGetTranscriptCodingEnd(EnsPTranscript transcript);

ajuint ensTranscriptGetSliceCodingStart(EnsPTranscript transcript);

ajuint ensTranscriptGetSliceCodingEnd(EnsPTranscript transcript);

AjBool ensTranscriptGetEnableSequenceEdits(EnsPTranscript transcript);

ajuint ensTranscriptGetLength(EnsPTranscript transcript);

ajuint ensTranscriptGetMemSize(const EnsPTranscript transcript);

AjBool ensTranscriptSetAdaptor(EnsPTranscript transcript,
                               EnsPTranscriptadaptor tca);

AjBool ensTranscriptSetIdentifier(EnsPTranscript transcript,
                                  ajuint identifier);

AjBool ensTranscriptSetFeature(EnsPTranscript transcript, EnsPFeature feature);

AjBool ensTranscriptSetDisplayReference(EnsPTranscript transcript,
                                        EnsPDatabaseentry displaydbe);

AjBool ensTranscriptSetDescription(EnsPTranscript transcript,
                                   AjPStr description);

AjBool ensTranscriptSetBioType(EnsPTranscript transcript,
                               AjPStr biotype);

AjBool ensTranscriptSetStatus(EnsPTranscript transcript,
                              AjEnum status);

AjBool ensTranscriptSetGeneIdentifier(EnsPTranscript transcript,
                                      ajuint geneid);

AjBool ensTranscriptSetCurrent(EnsPTranscript transcript,
                               AjBool current);

AjBool ensTranscriptSetStableIdentifier(EnsPTranscript transcript,
                                        AjPStr stableid);

AjBool ensTranscriptSetVersion(EnsPTranscript transcript,
                               ajuint version);

AjBool ensTranscriptSetCreationDate(EnsPTranscript transcript,
                                    AjPStr cdate);

AjBool ensTranscriptSetModificationDate(EnsPTranscript transcript,
                                        AjPStr mdate);

AjBool ensTranscriptSetEnableSequenceEdits(EnsPTranscript transcript,
                                           AjBool enablese);

AjBool ensTranscriptAddAttribute(EnsPTranscript transcript,
                                 EnsPAttribute attribute);

AjBool ensTranscriptAddDatabaseentry(EnsPTranscript transcript,
                                     EnsPDatabaseentry dbe);

AjBool ensTranscriptLinkTranslation(EnsPTranscript transcript,
                                    EnsPTranslation translation);

AjBool ensTranscriptUnlinkTranslation(EnsPTranscript transcript,
                                      EnsPTranslation translation);

AjBool ensTranscriptTrace(const EnsPTranscript transcript, ajuint level);

AjEnum ensTranscriptStatusFromStr(const AjPStr status);

const char *ensTranscriptStatusToChar(const AjEnum status);

AjBool ensTranscriptCalculateCoordinates(EnsPTranscript transcript);

EnsPTranscript ensTranscriptTransform(EnsPTranscript transcript,
                                      const AjPStr csname,
                                      const AjPStr csversion);

EnsPTranscript ensTranscriptTransfer(EnsPTranscript transcript,
                                     EnsPSlice slice);

AjBool ensTranscriptFetchAllAttributes(EnsPTranscript transcript,
                                       const AjPStr code,
                                       AjPList attributes);

AjBool ensTranscriptFetchAllConstitutiveExons(EnsPTranscript transcript,
                                              AjPList exons);

AjBool ensTranscriptFetchAllDatabaseEntries(EnsPTranscript transcript,
                                            const AjPStr name,
                                            AjEnum type,
                                            AjPList dbes);

AjBool ensTranscriptFetchAllIntrons(EnsPTranscript transcript,
                                    AjPList introns);

AjBool ensTranscriptFetchAllSequenceEdits(EnsPTranscript transcript,
                                          AjPList ses);

AjBool ensTranscriptFetchDisplayIdentifier(const EnsPTranscript transcript,
                                           AjPStr *Pidentifier);

AjBool ensTranscriptFetchSequenceSeq(EnsPTranscript transcript,
                                     AjPSeq *Psequence);

AjBool ensTranscriptFetchSequenceStr(EnsPTranscript transcript,
                                     AjPStr *Psequence);

AjBool ensTranscriptFetchTranslatableSequence(EnsPTranscript transcript,
                                              AjPStr* Psequence);

AjBool ensTranscriptFetchTranslation(EnsPTranscript transcript,
                                     EnsPTranslation *Ptranslation);

AjBool ensTranscriptFetchTranslatedSequence(EnsPTranscript transcript,
                                            AjPStr* Psequence);

/* Ensembl Transcript Adaptor */

EnsPTranscriptadaptor ensTranscriptadaptorNew(EnsPDatabaseadaptor dba);

void ensTranscriptadaptorDel(EnsPTranscriptadaptor *Ptca);

EnsPFeatureadaptor ensTranscriptadaptorGetFeatureadaptor(
    const EnsPTranscriptadaptor tca);

EnsPDatabaseadaptor ensTranscriptadaptorGetDatabaseadaptor(
    const EnsPTranscriptadaptor tca);

AjBool ensTranscriptadaptorFetchAll(EnsPTranscriptadaptor tca,
                                    AjPList transcripts);

AjBool ensTranscriptadaptorFetchAllByGene(EnsPTranscriptadaptor tca,
                                          EnsPGene gene,
                                          AjPList transcripts);

AjBool ensTranscriptadaptorFetchAllByStableIdentifier(
    const EnsPTranscriptadaptor tca,
    const AjPStr stableid,
    AjPList transcripts);

AjBool ensTranscriptadaptorFetchByIdentifier(EnsPTranscriptadaptor tca,
                                             ajuint identifier,
                                             EnsPTranscript *Ptranscript);

AjBool ensTranscriptadaptorFetchByStableIdentifier(
    EnsPTranscriptadaptor tca,
    const AjPStr stableid,
    ajuint version,
    EnsPTranscript *Ptranscript);

AjBool ensTranscriptadaptorFetchByExonIdentifier(
    EnsPTranscriptadaptor tca,
    ajuint identifier,
    EnsPTranscript* Ptranscript);

AjBool ensTranscriptadaptorFetchByExonStableIdentifier(
    EnsPTranscriptadaptor tca,
    const AjPStr stableid,
    EnsPTranscript *Ptranscript);

AjBool ensTranscriptadaptorFetchByTranslationIdentifier(
    EnsPTranscriptadaptor tca,
    ajuint identifier,
    EnsPTranscript* Ptranscript);

AjBool ensTranscriptadaptorFetchByTranslationStableIdentifier(
    EnsPTranscriptadaptor tca,
    const AjPStr stableid,
    EnsPTranscript* Ptranscript);

AjBool ensTranscriptadaptorFetchByDisplayLabel(EnsPTranscriptadaptor tca,
                                               const AjPStr label,
                                               EnsPTranscript *Ptranscript);

AjBool ensTranscriptadaptorFetchAllIdentifiers(
    const EnsPTranscriptadaptor tca,
    AjPList identifiers);

AjBool ensTranscriptadaptorFetchAllStableIdentifiers(
    const EnsPTranscriptadaptor tca,
    AjPList identifiers);

/* Ensembl Transcript Mapper */

AjBool ensTranscriptMapperInit(EnsPTranscript transcript);

AjBool ensTranscriptMapperTranscriptToSlice(EnsPTranscript transcript,
                                            ajuint start,
                                            ajuint end,
                                            AjPList mrs);

AjBool ensTranscriptMapperSliceToTranscript(EnsPTranscript transcript,
                                            ajuint start,
                                            ajuint end,
                                            ajint strand,
                                            AjPList mrs);

AjBool ensTranscriptMapperTranslationToSlice(EnsPTranscript transcript,
                                             ajuint start,
                                             ajuint end,
                                             AjPList mrs);

AjBool ensTranscriptMapperSliceToCDS(EnsPTranscript transcript,
                                     ajuint start,
                                     ajuint end,
                                     ajint strand,
                                     AjPList mrs);

AjBool ensTranscriptMapperSliceToTranslation(EnsPTranscript transcript,
                                             ajuint start,
                                             ajuint end,
                                             ajint strand,
                                             AjPList mrs);

/* Ensembl Supporting Feature Adaptor */

AjBool ensSupportingfeatureadaptorFetchAllByTranscript(
    EnsPDatabaseadaptor dba,
    EnsPTranscript transcript,
    AjPList bafs);

/*
** End of prototype definitions
*/




#endif /* enstranscript_h */

#ifdef __cplusplus
}
#endif
