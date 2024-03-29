#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ensexon_h
#define ensexon_h

/* ==================================================================== */
/* ========================== include files =========================== */
/* ==================================================================== */

#include "ensdata.h"
#include "ensfeature.h"




/* ==================================================================== */
/* ============================ constants ============================= */
/* ==================================================================== */




/* ==================================================================== */
/* ========================== public data ============================= */
/* ==================================================================== */




/* ==================================================================== */
/* ======================= public functions =========================== */
/* ==================================================================== */

/*
** Prototype definitions
*/

/* Ensembl Exon */

EnsPExon ensExonNew(EnsPExonadaptor ea,
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

EnsPExonadaptor ensExonGetAdaptor(const EnsPExon exon);

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

const AjPList ensExonGetSupportingfeatures(EnsPExon exon);

AjBool ensExonSetAdaptor(EnsPExon exon, EnsPExonadaptor ea);

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

ajulong ensExonGetMemsize(const EnsPExon exon);

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

AjBool ensExonSortByStartAscending(AjPList exons);

AjBool ensExonSortByStartDescending(AjPList exons);

/* Ensembl Exon Adaptor */

EnsPExonadaptor ensRegistryGetExonadaptor(
    EnsPDatabaseadaptor dba);

EnsPExonadaptor ensExonadaptorNew(
    EnsPDatabaseadaptor dba);

void ensExonadaptorDel(EnsPExonadaptor *Pea);

EnsPFeatureadaptor ensExonadaptorGetFeatureadaptor(
    const EnsPExonadaptor ea);

EnsPDatabaseadaptor ensExonadaptorGetDatabaseadaptor(
    const EnsPExonadaptor ea);

AjBool ensExonadaptorFetchAll(const EnsPExonadaptor ea, AjPList exons);

AjBool ensExonadaptorFetchAllBySlice(const EnsPExonadaptor ea,
                                     EnsPSlice slice,
                                     AjPList exons);

AjBool ensExonadaptorFetchAllBySliceConstraint(const EnsPExonadaptor ea,
                                               EnsPSlice slice,
                                               const AjPStr constraint,
                                               AjPList exons);

AjBool ensExonadaptorFetchByIdentifier(const EnsPExonadaptor ea,
                                       ajuint identifier,
                                       EnsPExon *Pexon);

AjBool ensExonadaptorFetchByStableIdentifier(const EnsPExonadaptor ea,
                                             const AjPStr stableid,
                                             ajuint version,
                                             EnsPExon *Pexon);

AjBool ensExonadaptorFetchAllByStableIdentifier(const EnsPExonadaptor ea,
                                                const AjPStr stableid,
                                                AjPList exons);

AjBool ensExonadaptorFetchAllByTranscript(const EnsPExonadaptor ea,
                                          const EnsPTranscript transcript,
                                          AjPList exons);

AjBool ensExonadaptorFetchAllIdentifiers(const EnsPExonadaptor ea,
                                         AjPList identifiers);

AjBool ensExonadaptorFetchAllStableIdentifiers(const EnsPExonadaptor ea,
                                               AjPList identifiers);

/* Ensembl Supporting Feature Adaptor */

AjBool ensSupportingfeatureadaptorFetchAllByExon(EnsPDatabaseadaptor dba,
                                                 EnsPExon exon,
                                                 AjPList bafs);

/*
** End of prototype definitions
*/




#endif /* ensexon_h */

#ifdef __cplusplus
}
#endif
