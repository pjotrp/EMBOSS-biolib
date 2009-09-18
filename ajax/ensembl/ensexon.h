#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensexon_h
#define ensexon_h

#include "ensdata.h"
#include "ensfeature.h"




/*
** Prototype definitions
*/

/* Ensembl Exon */

EnsPExon ensExonNew(EnsPExonAdaptor adaptor,
                    ajuint identifier,
                    EnsPFeature feature,
                    ajint sphase,
                    ajint ephase,
                    AjBool current,
                    AjBool constitutive,
                    AjPStr stableid,
                    ajuint version,
                    AjPStr cdate,
                    AjPStr mdate);

EnsPExon ensExonNewObj(const EnsPExon object);

EnsPExon ensExonNewRef(EnsPExon exon);

void ensExonDel(EnsPExon* Pexon);

EnsPExonAdaptor ensExonGetAdaptor(const EnsPExon exon);

ajuint ensExonGetIdentifier(const EnsPExon exon);

EnsPFeature ensExonGetFeature(const EnsPExon exon);

ajint ensExonGetStartPhase(const EnsPExon exon);

ajint ensExonGetEndPhase(const EnsPExon exon);

AjBool ensExonGetCurrent(const EnsPExon exon);

AjBool ensExonGetConstitutive(const EnsPExon exon);

AjPStr ensExonGetStableIdentifier(const EnsPExon exon);

ajuint ensExonGetVersion(const EnsPExon exon);

AjPStr ensExonGetCreationDate(const EnsPExon exon);

AjPStr ensExonGetModificationDate(const EnsPExon exon);

const AjPList ensExonGetSupportingFeatures(EnsPExon exon);

AjBool ensExonSetAdaptor(EnsPExon exon, EnsPExonAdaptor adaptor);

AjBool ensExonSetIdentifier(EnsPExon exon, ajuint identifier);

AjBool ensExonSetFeature(EnsPExon exon, EnsPFeature feature);

AjBool ensExonSetStartPhase(EnsPExon exon, ajint sphase);

AjBool ensExonSetEndPhase(EnsPExon exon, ajint ephase);

AjBool ensExonSetCurrent(EnsPExon exon, AjBool current);

AjBool ensExonSetConstitutive(EnsPExon exon, AjBool constitutive);

AjBool ensExonSetStableIdentifier(EnsPExon exon, AjPStr stableid);

AjBool ensExonSetVersion(EnsPExon exon, ajuint version);

AjBool ensExonSetCreationDate(EnsPExon exon, AjPStr cdate);

AjBool ensExonSetModificationDate(EnsPExon exon, AjPStr mdate);

AjBool ensExonTrace(const EnsPExon exon, ajuint level);

ajint ensExonGetFrame(const EnsPExon exon);

ajuint ensExonGetMemSize(const EnsPExon exon);

EnsPExon ensExonTransform(EnsPExon exon, const AjPStr csname,
                          const AjPStr csversion);

EnsPExon ensExonTransfer(EnsPExon exon, EnsPSlice slice);

AjBool ensExonFetchDisplayIdentifier(const EnsPExon exon, AjPStr* Pidentifier);

AjBool ensExonFetchSequenceStr(EnsPExon exon, AjPStr* Psequence);

AjBool ensExonFetchSequenceSeq(EnsPExon exon, AjPSeq* Psequence);

ajuint ensExonGetTranscriptStart(EnsPExon exon, EnsPTranscript transcript);

ajuint ensExonGetTranscriptEnd(EnsPExon exon, EnsPTranscript transcript);

ajuint ensExonGetTranscriptCodingStart(EnsPExon exon,
                                       EnsPTranscript transcript);

ajuint ensExonGetTranscriptCodingEnd(EnsPExon exon, EnsPTranscript transcript);

ajuint ensExonGetSliceCodingStart(EnsPExon exon, EnsPTranscript transcript);

ajuint ensExonGetSliceCodingEnd(EnsPExon exon, EnsPTranscript transcript);

/* Ensembl Exon Adaptor */

EnsPExonAdaptor ensExonAdaptorNew(EnsPDatabaseAdaptor dba);

void ensExonAdaptorDel(EnsPExonAdaptor *Padaptor);

EnsPFeatureAdaptor ensExonAdaptorGetFeatureAdaptor(
    const EnsPExonAdaptor adaptor);

EnsPDatabaseAdaptor ensExonAdaptorGetDatabaseAdaptor(
    const EnsPExonAdaptor adaptor);

AjBool ensExonAdaptorFetchAll(EnsPExonAdaptor ea, AjPList exons);

AjBool ensExonAdaptorFetchAllBySlice(EnsPExonAdaptor ea,
                                     EnsPSlice slice,
                                     AjPList exons);

AjBool ensExonAdaptorFetchByIdentifier(EnsPExonAdaptor adaptor,
                                       ajuint identifier,
                                       EnsPExon *Pexon);

AjBool ensExonAdaptorFetchByStableIdentifier(EnsPExonAdaptor ea,
                                             const AjPStr stableid,
                                             ajuint version,
                                             EnsPExon *Pexon);

AjBool ensExonAdaptorFetchAllVersionsByStableId(EnsPExonAdaptor ea,
                                                const AjPStr stableid,
                                                AjPList exons);

AjBool ensExonAdaptorFetchAllByTranscript(EnsPExonAdaptor ea,
                                          const EnsPTranscript transcript,
                                          AjPList exons);

AjBool ensExonAdaptorFetchAllIdentifiers(EnsPExonAdaptor ea, AjPList idlist);

AjBool ensExonAdaptorFetchAllStableIdentifiers(EnsPExonAdaptor ea,
                                               AjPList idlist);

/* Ensembl Supporting Feature Adaptor */

AjBool ensSupportingFeatureAdaptorFetchAllByExon(EnsPDatabaseAdaptor dba,
                                                 EnsPExon exon,
                                                 AjPList bafs);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
