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

EnsPTranscript ensTranscriptNew(EnsPTranscriptAdaptor adaptor,
                                ajuint identifier,
                                EnsPFeature feature,
                                EnsPDatabaseEntry displaydbe,
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

EnsPTranscriptAdaptor ensTranscriptGetAdaptor(const EnsPTranscript transcript);

ajuint ensTranscriptGetIdentifier(const EnsPTranscript transcript);

EnsPFeature ensTranscriptGetFeature(const EnsPTranscript transcript);

EnsPDatabaseEntry ensTranscriptGetDisplayReference(
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

const AjPList ensTranscriptGetSupportingFeatures(EnsPTranscript transcript);

const AjPList ensTranscriptGetTranslations(const EnsPTranscript transcript);

ajuint ensTranscriptGetTranscriptCodingStart(EnsPTranscript transcript);

ajuint ensTranscriptGetTranscriptCodingEnd(EnsPTranscript transcript);

ajuint ensTranscriptGetSliceCodingStart(EnsPTranscript transcript);

ajuint ensTranscriptGetSliceCodingEnd(EnsPTranscript transcript);

ajuint ensTranscriptGetLength(EnsPTranscript transcript);

ajuint ensTranscriptGetMemSize(const EnsPTranscript transcript);

AjBool ensTranscriptSetAdaptor(EnsPTranscript transcript,
                               EnsPTranscriptAdaptor ta);

AjBool ensTranscriptSetIdentifier(EnsPTranscript transcript, ajuint identifier);

AjBool ensTranscriptSetFeature(EnsPTranscript transcript, EnsPFeature feature);

AjBool ensTranscriptAddAttribute(EnsPTranscript transcript,
                                 EnsPAttribute attribute);

AjBool ensTranscriptAddDatabaseEntry(EnsPTranscript transcript,
                                     EnsPDatabaseEntry dbe);

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

AjBool ensTranscriptFetchAllDatabaseEntries(EnsPTranscript transcript,
                                            const AjPStr name,
                                            AjEnum type,
                                            AjPList dbes);

AjBool ensTranscriptFetchAllIntrons(EnsPTranscript transcript, AjPList introns);

AjBool ensTranscriptFetchAllSequenceEdits(EnsPTranscript transcript,
                                          AjPList selist);

AjBool ensTranscriptFetchDisplayIdentifier(const EnsPTranscript transcript,
                                           AjPStr *Pidentifier);

AjBool ensTranscriptFetchSequenceSeq(EnsPTranscript transcript,
                                     AjPSeq *Psequence);

AjBool ensTranscriptTranslateStr(EnsPTranscript transcript, AjPStr *Psequence);

AjBool ensTranscriptFetchSequenceStr(EnsPTranscript transcript,
                                     AjPStr *Psequence);

AjBool ensTranscriptFetchTranslatableSequence(
    EnsPTranscript transcript,
    AjPStr* Psequence);

EnsPTranslation ensTranscriptFetchTranslation(EnsPTranscript transcript);

/* Ensembl Transcript Adaptor */

EnsPTranscriptAdaptor ensTranscriptAdaptorNew(EnsPDatabaseAdaptor dba);

void ensTranscriptAdaptorDel(EnsPTranscriptAdaptor *Padaptor);

EnsPFeatureAdaptor ensTranscriptAdaptorGetFeatureAdaptor(
    const EnsPTranscriptAdaptor adaptor);

EnsPDatabaseAdaptor ensTranscriptAdaptorGetDatabaseAdaptor(
    const EnsPTranscriptAdaptor adaptor);

AjBool ensTranscriptAdaptorFetchAll(EnsPTranscriptAdaptor adaptor,
                                    AjPList transcripts);

AjBool ensTranscriptAdaptorFetchAllByGene(EnsPTranscriptAdaptor adaptor,
                                          EnsPGene gene,
                                          AjPList transcripts);

AjBool ensTranscriptAdaptorFetchByIdentifier(EnsPTranscriptAdaptor adaptor,
                                             ajuint identifier,
                                             EnsPTranscript *Ptranscript);

AjBool ensTranscriptAdaptorFetchByStableIdentifier(
    EnsPTranscriptAdaptor adaptor,
    const AjPStr stableid,
    ajuint version,
    EnsPTranscript *Ptranscript);

AjBool ensTranscriptAdaptorFetchByTranslationIdentifier(
    EnsPTranscriptAdaptor adaptor,
    ajuint identifier,
    EnsPTranscript* Ptranscript);

AjBool ensTranscriptAdaptorFetchByTranslationStableIdentifier(
    EnsPTranscriptAdaptor adaptor,
    const AjPStr stableid,
    EnsPTranscript* Ptranscript);

/* Ensembl Transcript Mapper */

AjBool ensTranscriptMapperInit(EnsPTranscript transcript);

AjBool ensTranscriptMapperTranscript2Slice(EnsPTranscript transcript,
                                           ajuint start,
                                           ajuint end,
                                           AjPList mrs);

AjBool ensTranscriptMapperSlice2Transcript(EnsPTranscript transcript,
                                           ajuint start,
                                           ajuint end,
                                           ajint strand,
                                           AjPList mrs);

AjBool ensTranscriptMapperTranslation2Slice(EnsPTranscript transcript,
                                            ajuint start,
                                            ajuint end,
                                            AjPList mrs);

AjBool ensTranscriptMapperSlice2CDS(EnsPTranscript transcript,
                                    ajuint start,
                                    ajuint end,
                                    ajint strand,
                                    AjPList mrs);

AjBool ensTranscriptMapperSlice2Translation(EnsPTranscript transcript,
                                            ajuint start,
                                            ajuint end,
                                            ajint strand,
                                            AjPList mrs);

/* Ensembl Supporting Feature Adaptor */

AjBool ensSupportingFeatureAdaptorFetchAllByTranscript(
    EnsPDatabaseAdaptor dba,
    EnsPTranscript transcript,
    AjPList bafs);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif