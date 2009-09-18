#ifdef __cplusplus
"C"
{
#endif

#ifndef ensgene_h
#define ensgene_h

#include "ensdata.h"
#include "ensfeature.h"




/*
** Prototype definitions
*/

/* Ensembl Gene */

EnsPGene ensGeneNew(EnsPGeneAdaptor adaptor,
                    ajuint identifier,
                    EnsPFeature feature,
                    EnsPDatabaseEntry displaydbe,
                    AjPStr description,
                    AjPStr source,
                    AjPStr biotype,
                    AjEnum status,
                    AjBool current,
                    AjPStr stableid,
                    ajuint version,
                    AjPStr cdate,
                    AjPStr mdate,
                    AjPList transcripts);

EnsPGene ensGeneNewObj(const EnsPGene object);

EnsPGene ensGeneNewRef(EnsPGene gene);

void ensGeneDel(EnsPGene* Pgene);

EnsPGeneAdaptor ensGeneGetAdaptor(const EnsPGene gene);

ajuint ensGeneGetIdentifier(const EnsPGene gene);

EnsPFeature ensGeneGetFeature(const EnsPGene gene);

EnsPDatabaseEntry ensGeneGetDisplayReference(const EnsPGene gene);

AjPStr ensGeneGetDescription(const EnsPGene gene);

AjPStr ensGeneGetSource(const EnsPGene gene);

AjPStr ensGeneGetBioType(const EnsPGene gene);

AjEnum ensGeneGetStatus(const EnsPGene gene);

AjBool ensGeneGetCurrent(const EnsPGene gene);

AjPStr ensGeneGetStableIdentifier(const EnsPGene gene);

ajuint ensGeneGetVersion(const EnsPGene gene);

AjPStr ensGeneGetCreationDate(const EnsPGene gene);

AjPStr ensGeneGetModificationDate(const EnsPGene gene);

const AjPList ensGeneGetAttributes(EnsPGene gene);

const AjPList ensGeneGetDatabaseEntries(EnsPGene gene);

const AjPList ensGeneGetTranscripts(EnsPGene gene);

AjBool ensGeneSetAdaptor(EnsPGene gene, EnsPGeneAdaptor adaptor);

AjBool ensGeneSetIdentifier(EnsPGene gene, ajuint identifier);

AjBool ensGeneSetFeature(EnsPGene gene, EnsPFeature feature);

AjBool ensGeneSetDisplayReference(EnsPGene gene, EnsPDatabaseEntry dbe);

AjBool ensGeneSetDescription(EnsPGene gene, AjPStr description);

AjBool ensGeneSetSource(EnsPGene gene, AjPStr source);

AjBool ensGeneSetBioType(EnsPGene gene, AjPStr biotype);

AjBool ensGeneSetStatus(EnsPGene gene, AjEnum status);

AjBool ensGeneSetCurrent(EnsPGene gene, AjBool current);

AjBool ensGeneSetStableIdentifier(EnsPGene gene, AjPStr stableid);

AjBool ensGeneSetVersion(EnsPGene gene, ajuint version);

AjBool ensGeneSetCreationDate(EnsPGene gene, AjPStr cdate);

AjBool ensGeneSetModificationDate(EnsPGene gene, AjPStr mdate);

AjBool ensGeneTrace(const EnsPGene gene, ajuint level);

ajuint ensGeneGetMemSize(const EnsPGene gene);

AjBool ensGeneAddAttribute(EnsPGene gene, EnsPAttribute attribute);

AjBool ensGeneAddDatabaseEntry(EnsPGene gene, EnsPDatabaseEntry dbe);

AjBool ensGeneAddTranscript(EnsPGene gene, EnsPTranscript transcript);

AjEnum ensGeneStatusFromStr(const AjPStr status);

const char* ensGeneStatusToChar(const AjEnum status);

AjBool ensGeneCalculateCoordinates(EnsPGene gene);

AjBool ensGeneFetchAllAttributes(EnsPGene gene, const AjPStr code,
                                AjPList attributes);

AjBool ensGeneFetchAllDatabaseEntries(EnsPGene gene,
                                     const AjPStr name,
                                     AjEnum type,
                                     AjPList dbes);

AjBool ensGeneFetchAllExons(EnsPGene gene, AjPList exons);

EnsPGene ensGeneTransform(EnsPGene gene, const AjPStr csname,
                         const AjPStr csversion);

EnsPGene ensGeneTransfer(EnsPGene gene, EnsPSlice slice);

/* Ensembl Gene Adaptor */

EnsPGeneAdaptor ensGeneAdaptorNew(EnsPDatabaseAdaptor dba);

void ensGeneAdaptorDel(EnsPGeneAdaptor *Padaptor);

EnsPFeatureAdaptor
ensGeneAdaptorGetFeatureAdaptor(const EnsPGeneAdaptor adaptor);

EnsPDatabaseAdaptor
ensGeneAdaptorGetDatabaseAdaptor(const EnsPGeneAdaptor adaptor);

AjBool ensGeneAdaptorFetchAll(EnsPGeneAdaptor adaptor, AjPList genes);

AjBool ensGeneAdaptorFetchByIdentifier(EnsPGeneAdaptor adaptor,
                                       ajuint identifier,
                                       EnsPGene *Pgene);

AjBool ensGeneAdaptorFetchByStableIdentifier(EnsPGeneAdaptor adaptor,
                                             const AjPStr stableid,
                                             ajuint version,
                                             EnsPGene *Pgene);

/*
** End of prototype definitions
*/




#endif

#ifdef __cplusplus
}
#endif
